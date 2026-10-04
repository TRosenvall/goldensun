#!/usr/bin/env python3
"""crossfire.py -- cross a park's candidate edits instead of testing them one at a time.

    python3 tools/crossfire.py <base.c> <ref.s> --func <Name> --edits edits.json
    python3 tools/crossfire.py <base.c> <ref.s> --func <Name> --edits e.json --depth 3

WHY THIS IS A TOOL.  One-at-a-time testing is the single biggest obstacle in this
project, measured across batches 305-318, and it fails in six distinct shapes
(docs/elevation.md, "the cross the lists law"):

  * inert for want of a prerequisite;
  * pins jointly load-bearing while individually inert (20 inert singly, 65 together);
  * edits each a clear regression, jointly worth 57 -> 40;
  * TWO EDITS EACH EXACTLY INERT, jointly worth the whole residue (10 -> 0);
  * a rejected-because-worse edit that is HALF of a two-part fix;
  * one half in the rejected list and the other in the inert list, NEVER CROSSED.

Every landing in batches 316-318 that came from crossing was found by hand, and
every agent rebuilt a sweep harness to do it -- five times in batch 317 alone.
One of those hand-rolled harnesses imported a DRIFTED FORK of objcmp.py (32 diff
lines).  This tool exists so that never happens again, and so the three traps
below are screened by default rather than remembered.

THREE THINGS IT DOES THAT A HAND-ROLLED SWEEP KEEPS GETTING WRONG.

1. IT IMPORTS tools/objcmp.py, THE AUTHORITY.  It does not reimplement scoring
   and it cannot drift from it.

2. IT SEPARATES ALIGNMENT FROM DISTANCE.  When a candidate's instruction COUNT
   differs from the reference, the positional figure measures MISALIGNMENT, not
   distance -- every index after the first insertion shifts.  A correct two-edit
   fix once read 27 against a park's 16 while being 27 of 27 instructions EXACT,
   and a park buried a CORRECT FLAG in its negatives at "71 of 199" because 71
   was 201 instructions.  So this tool prints the count beside every figure and
   flags any row whose count differs, instead of ranking on the figure alone.

3. IT SCREENS PER-OPCODE MEMORY COUNTS.  A false improvement is a WRONG PROGRAM.
   `if (count != 0)` once read 58 from 66 at IDENTICAL instruction count while
   reading the count word ONCE WHERE THE ROM READS IT TWICE; of 30 crossed
   variants only FIVE passed a memory screen and FOUR of those only by
   cancellation.  At this distance from zero, a better figure obtained by doing
   less work than the ROM is a LIKELY outcome of a random search.  Rows whose
   ldr/ldrb/ldrh/ldrsb/ldrsh/str/strb/strh totals do not match the reference are
   marked MEM and must not be believed without reading them.

EDITS FILE.  JSON list of objects, each applied to the base by exact string
replacement (the same contract as the Edit tool -- `old` must occur exactly once):

    [ {"name": "block-scoped k",   "old": "int k;",        "new": "int k = 0;"},
      {"name": "pin r3",           "old": "int t;",        "new": "register int t __asm__(\\"r3\\");"},
      {"name": "swap stores",      "old": "a = x;\\n    b = y;", "new": "b = y;\\n    a = x;"} ]

An edit whose `old` is missing or ambiguous is reported as SKIPPED, loudly -- a
silently unapplied edit measures as a clean inert result, which is how BSD sed's
missing \\t support once produced a whole round of false inerts.

OUTPUT.  Every subset up to --depth (default 2), sorted by figure, with the
base row marked.  Read the INSTRUCTION COUNT column first, then MEM, then the
figure.  Cells that tie the base are the interesting ones: an exactly-inert edit
is a CANDIDATE PREREQUISITE, not a dead end.
"""
import argparse, itertools, json, os, re, shutil, subprocess, sys, tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import objcmp  # THE AUTHORITY -- never fork this

MEMOPS = ("ldr", "ldrb", "ldrh", "ldrsb", "ldrsh", "str", "strb", "strh")


def score(src, ref, func):
    """(differing, ref_count, our_count, relocdiff, error) via objcmp itself.

    `error` is non-empty when there is NO figure, and it says WHY.  A variant that
    fails to compile and a variant whose score cannot be parsed are different
    events, and reporting them identically is the silent-unverifiability bug this
    project keeps finding in its own tools -- the first version of this file did
    exactly that, and a `void` return-type edit came back indistinguishable from
    an edit whose `old` string was missing.
    """
    cmd = [sys.executable, os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                        "objcmp.py"), src, ref, "--func", func]
    r = subprocess.run(cmd, capture_output=True, text=True, cwd=objcmp.ROOT)
    out, err = r.stdout, r.stderr
    if " OK " in out:
        return (0, None, None, False, "")
    m = re.search(r"ENCODINGS differ in (\d+) place\(s\) \(ref (\d+), ours (\d+)\)", out)
    if m:
        return (int(m.group(1)), int(m.group(2)), int(m.group(3)),
                "RELOCATIONS differ" in out, "")
    blob = (err + "\n" + out)
    cerr = [l for l in blob.splitlines()
            if re.search(r"\berror\b|\bundefined\b|parse error|syntax error", l, re.I)]
    if cerr:
        return (None, None, None, False, "COMPILEFAIL: " + cerr[0].strip()[:110])
    last = (blob.strip().splitlines() or ["no output"])[-1]
    return (None, None, None, False, "NOFIGURE: " + last.strip()[:110])


def memhist(src, ref, func):
    """Per-opcode memory-access counts for candidate and reference."""
    try:
        cf, _adjust = objcmp.cflags_for(ref)   # returns (flags, adjust), not a list
    except Exception:
        cf = []
    tmp = tempfile.mkdtemp(prefix="crossfire.")
    try:
        asm = os.path.join(tmp, "c.s")
        gcc = os.path.join(objcmp.GCC, "xgcc")
        r = subprocess.run([gcc, "-B" + objcmp.GCC + "/"] + cf +
                           ["-I" + os.path.join(objcmp.ROOT, "include"), "-S", "-o", asm, src],
                           capture_output=True, text=True, cwd=objcmp.ROOT)
        if r.returncode != 0:
            return None
        # PROFILE THE CANDIDATE THE SAME WAY AS THE REFERENCE: assemble and
        # objdump.  The first version grepped the .s TEXT, and that is the
        # identical-encoding trap this project documents in its own method notes
        # -- gcc spells a HImode pool reference `ldrh r5, .L20`, which ASSEMBLES
        # to a plain `ldr rN,[pc,#imm]`, because Thumb-1 has no PC-relative
        # halfword load.  Grepped: ldr=34 ldrh=2.  Objdumped: ldr=35 ldrh=1.
        # The reference was objdumped, so the screen fired on EVERY row
        # including BASE -- which makes it misleading rather than merely noisy.
        obj = os.path.join(tmp, "c.o")
        r = subprocess.run(objcmp.AS + ["-o", obj, asm], capture_output=True,
                           text=True, cwd=objcmp.ROOT)
        if r.returncode != 0:
            return None
        d = subprocess.run(["arm-none-eabi-objdump", "-d", "--no-show-raw-insn", obj],
                           capture_output=True, text=True).stdout
        return {op: len(re.findall(rf"(?m)^\s+\S+:\s+{op}\b", d)) for op in MEMOPS}
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


def apply_edits(base_text, edits, chosen):
    t = base_text
    for i in chosen:
        e = edits[i]
        if t.count(e["old"]) != 1:
            return None, f'edit "{e["name"]}": `old` occurs {t.count(e["old"])} times, need exactly 1'
        t = t.replace(e["old"], e["new"], 1)
    return t, None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("base"); ap.add_argument("ref")
    ap.add_argument("--func", required=True)
    ap.add_argument("--edits", required=True)
    ap.add_argument("--depth", type=int, default=2)
    ap.add_argument("--keep", action="store_true", help="keep the generated variant files")
    a = ap.parse_args()

    edits = json.load(open(a.edits))
    base_text = open(a.base).read()
    work = tempfile.mkdtemp(prefix="crossfire.work.")
    rows = []

    base_mem = memhist(a.base, a.ref, a.func)
    ref_mem, ref_mem_err = None, ""
    # The reference's own memory profile.  This MUST NOT fail quietly: it is the
    # input to the MEM screen, and with it absent the screen silently passes
    # everything -- a missing input printing "exact" is rung 6 of the
    # figures-that-lie ladder in docs/elevation.md.  The first version of this
    # file swallowed the exception and printed `reference memory profile: None`.
    try:
        rp = os.path.join(work, "ref.s")
        _of = objcmp.one_function(a.ref, a.func)   # returns a LIST of lines
        open(rp, "w").write(_of if isinstance(_of, str) else "\n".join(_of))
        o = os.path.join(work, "ref.o")
        r = subprocess.run(objcmp.AS + ["-o", o, rp], capture_output=True,
                           text=True, cwd=objcmp.ROOT)
        if r.returncode != 0:
            raise RuntimeError("as: " + (r.stderr.strip().splitlines() or ["?"])[-1])
        d = subprocess.run(["arm-none-eabi-objdump", "-d", "--no-show-raw-insn", o],
                           capture_output=True, text=True).stdout
        body = "\n".join(d.splitlines()[d.splitlines().index(next(
            l for l in d.splitlines() if l.endswith(">:"))):]) if ">:" in d else d
        ref_mem = {op: len(re.findall(rf"(?m)^\s+\S+:\s+{op}\b", body)) for op in MEMOPS}
        if not any(ref_mem.values()):
            ref_mem = {op: len(re.findall(rf"(?m)\b{op}\b", body)) for op in MEMOPS}
    except Exception as e:
        ref_mem_err = f"{type(e).__name__}: {e}"

    combos = [()] + [c for n in range(1, a.depth + 1)
                     for c in itertools.combinations(range(len(edits)), n)]
    for c in combos:
        txt, err = apply_edits(base_text, edits, c)
        label = "BASE" if not c else " + ".join(edits[i]["name"] for i in c)
        if err:
            rows.append((None, None, None, "SKIP", label, err)); continue
        vp = os.path.join(work, f"v{len(rows):04d}.c")
        open(vp, "w").write(txt)
        d, rc, oc, reloc, serr = score(vp, a.ref, a.func)
        mem = memhist(vp, a.ref, a.func)
        flag = ""
        if reloc: flag += "RELOC "
        if rc is not None and oc is not None and rc != oc: flag += "COUNT "
        if ref_mem and mem and any(mem[k] != ref_mem[k] for k in MEMOPS): flag += "MEM "
        rows.append((d, rc, oc, flag.strip() or "-", label, serr))

    base = next((r for r in rows if r[4] == "BASE"), None)
    print(f"\ncrossfire: {a.func}   base={a.base}")
    if ref_mem:
        print(f"  reference memory profile: "
              + " ".join(f"{k}={v}" for k, v in ref_mem.items() if v))
    else:
        print(f"  *** MEM SCREEN DISABLED -- could not profile the reference: "
              f"{ref_mem_err or 'unknown'}")
        print(f"  *** Every row below is UNSCREENED for memory accesses.  Do not "
              f"accept an improvement from this run without reading its loads.")
    if base and base[0] is not None:
        print(f"  BASE figure {base[0]}  (ref {base[1]} / ours {base[2]})")
    print(f"\n  {'figure':>6} {'ref':>5} {'ours':>5}  {'flags':<14} edit set")
    print("  " + "-" * 92)
    ok = [r for r in rows if r[0] is not None]
    for d, rc, oc, flag, label, _ in sorted(ok, key=lambda r: (r[0], r[4])):
        mark = "  <= BASE" if label == "BASE" else ""
        tie = "  (exactly inert -- CANDIDATE PREREQUISITE)" if base and d == base[0] and label != "BASE" else ""
        print(f"  {d:6d} {str(rc or '-'):>5} {str(oc or '-'):>5}  {flag:<14} {label}{mark}{tie}")
    for r in rows:
        if r[0] is None:
            kind = ("SKIPPED" if r[5].startswith("edit ") else
                    "COMPILEFAIL" if r[5].startswith("COMPILEFAIL") else
                    "NOFIGURE" if r[5].startswith("NOFIGURE") else "SKIPPED")
            print(f"  {'--':>6} {'':>5} {'':>5}  {kind:<14} {r[4]}\n"
                  f"  {'':>6} {'':>5} {'':>5}  {'':<14}   -> {r[5]}")
    print(f"\n  flags: COUNT = instruction count differs, so the figure measures MISALIGNMENT")
    print(f"         MEM   = memory-access counts differ from the reference -- A BETTER FIGURE")
    print(f"                 HERE MAY BE A WRONG PROGRAM.  Read it before believing it.")
    print(f"         RELOC = relocations differ -- the figure is NOT a distance.")
    if a.keep:
        print(f"\n  variants kept in {work}")
    else:
        shutil.rmtree(work, ignore_errors=True)


if __name__ == "__main__":
    main()
