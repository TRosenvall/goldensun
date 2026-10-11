"""autosweep.py -- the JUDGMENT-FREE half of elevating a park, run from bash.

    python3 tools/autosweep.py src/non_matching/rom_b5000/80b6d30.c
    python3 tools/autosweep.py <park.c> --flags      # per-file flag groups only
    python3 tools/autosweep.py <park.c> --anchor     # anchor placements only

WHAT THIS IS FOR.  Most of elevating a park needs judgment: reading the
disassembly, forming a hypothesis, reading `.00.rtl` or `.18.greg` to name the
pass.  None of that is scriptable.  But TWO axes are completely mechanical, they
need no hypothesis at all, and in batch 337 each of them produced a landing:

  * THE FLAG AXIS.  Ten per-file flag groups exist in the Makefile.  Trying the
    UNCHANGED body under each is pure measurement -- no source edit, nothing to
    decide.  OvlFunc_common1_588 landed on -O1, and the brief's note was that
    "the park had closed the source axis hard and never swept the flag axis."
  * THE ANCHOR AXIS.  Inserting `do { } while (0);` at each top-level statement
    boundary is text manipulation.  Func_801fd34 landed on one such placement,
    after a hand sweep of 82; Field_Move and Func_80ab21c landed the same shape
    with a barrier, and the anchor is strictly preferable (same reach, emits
    nothing, not counted by shimcount -- see docs/elevation.md).

So this tool is worth running on a park BEFORE spending an agent on it, because
it costs wall-clock and zero tokens, and the two cheapest classes of answer come
back without anyone forming an opinion.

WHAT IT CANNOT DO, stated plainly so nobody mistakes a clean sweep for a verdict:
it does not read dumps, cannot invent a spelling, cannot write a body for a
prose park, and cannot correct a park's diagnosis.  A park that survives this
sweep has had its two cheapest axes closed -- which is genuinely useful to a
brief, as a recorded negative -- and nothing more.

The anchor variants are device-free.  `--barrier` also exists for completeness
but a bare `__asm__ volatile ("")` is fakematch-class and carries pass-3 debt,
so prefer the anchor and reach for the barrier only to confirm a mechanism.
"""
import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# SPLIT AT THE IMAGE NAME DELIBERATELY.  Everything before it is a docker flag;
# everything after is the container's command.  The first version of this file
# appended `-e OBJCMP_EXTRA=...` to the full list, i.e. AFTER the image name, so
# docker treated `-e` as the command to run and the flag never reached objcmp --
# and the sweep reported all eight flag groups as "no verdict", which reads
# exactly like a clean negative.  Verified by hand against a direct objcmp run
# that DID report 20 of 76 with -fno-gcse.
DOCKER_PRE = ["docker", "run", "--rm", "--security-opt", "seccomp=unconfined",
              "-v", f"{ROOT}:/work", "-w", "/work"]
IMAGE = "goldensun-build"
DOCKER = DOCKER_PRE + [IMAGE]

# The distinguishing flag of each per-file group in the Makefile.  Kept as data
# rather than parsed, because the groups are named by their effect and a regex
# over the Makefile would pick up the prose discussing them -- the
# prose-as-machinery trap this tree keeps hitting.
FLAGS = [
    ("-O1",                       "O1_CFLAGS"),
    ("-fno-gcse",                 "GCSE_CFLAGS"),
    ("-fno-schedule-insns2",      "SCHED2_CFLAGS"),
    ("-ffixed-r7",                "FIXEDR7_CFLAGS"),
    ("-fno-strict-aliasing",      "ALIAS_CFLAGS"),
    ("-fno-strength-reduce",      "STRENGTH_CFLAGS"),
    ("-fno-rerun-cse-after-loop", "CSE_CFLAGS"),
    ("-fno-rerun-loop-opt",       "RERUNLOOP_CFLAGS"),
]

CLAIM = re.compile(
    r"(\d+)\s+(?:differing\s+)?(?:[A-Za-z]+\s+){0,3}encodings?\s+of\s+(\d+)", re.I)
DEF = re.compile(r"^[A-Za-z_][A-Za-z0-9_ \t\*]*?\b(\w+)\s*\([^;{]*?\)\s*\{", re.M | re.S)
SUBJ = re.compile(r"\s*/\*+\s*((?:Ovl)?Func_\w+|[A-Z][A-Za-z0-9_]*)")


def resolve(park):
    """-> (reference .s, function name).  Same rules as backfill.py / piece.py."""
    t = open(os.path.join(ROOT, park), errors="replace").read()
    ref = None
    for m in re.finditer(r"(asm/[A-Za-z0-9_/]+\.s)", t):
        if os.path.exists(os.path.join(ROOT, m.group(1))):
            ref = m.group(1)
            break
    code = re.sub(r"/\*.*?\*/", " ", t, flags=re.S)
    code = re.sub(r"//[^\n]*", " ", code)
    m = DEF.search(code)
    fn = m.group(1) if m else (SUBJ.match(t).group(1) if SUBJ.match(t) else None)
    return ref, fn


def measure(path, ref, fn, extra=""):
    """One objcmp run.  -> (ndiff, total) or None when it did not get that far."""
    cmd = DOCKER_PRE[:]
    if extra:
        cmd += ["-e", f"OBJCMP_EXTRA={extra}"]       # BEFORE the image name
    cmd += [IMAGE, "python3", "tools/objcmp.py", path, ref, "--func", fn]
    out = subprocess.run(cmd, capture_output=True, text=True).stdout
    if re.search(r"^\s+OK\b", out, re.M):
        return (0, 0)
    m = re.search(r"differ in (\d+) place\(s\) \(ref (\d+)", out)
    return (int(m.group(1)), int(m.group(2))) if m else None


def body_span(text, fn):
    """Character span of fn's body, braces balanced.  -> (open_idx, close_idx)."""
    m = re.search(r"^[A-Za-z_][A-Za-z0-9_ \t\*]*?\b" + re.escape(fn) +
                  r"\s*\([^;{]*?\)\s*\{", text, re.M | re.S)
    if not m:
        return None
    i = text.index("{", m.start())
    depth = 0
    for j in range(i, len(text)):
        if text[j] == "{":
            depth += 1
        elif text[j] == "}":
            depth -= 1
            if depth == 0:
                return (i, j)
    return None


def boundaries(text, i, j):
    """Top-level statement boundaries inside the body: offsets just after a `;`
    at brace depth 1.  Declarations are included -- placing an anchor among them
    is exactly how Field_Move's 0 was found (after a load, not before)."""
    out, depth = [], 0
    for k in range(i, j):
        c = text[k]
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
        elif c == ";" and depth == 1:
            out.append(k + 1)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("park")
    ap.add_argument("--flags", action="store_true")
    ap.add_argument("--anchor", action="store_true")
    ap.add_argument("--barrier", action="store_true")
    ap.add_argument("--max", type=int, default=40,
                    help="cap on placement variants (default 40)")
    a = ap.parse_args()
    if not (a.flags or a.anchor or a.barrier):
        a.flags = a.anchor = True          # the two device-free axes

    park = os.path.relpath(os.path.abspath(a.park), ROOT)
    ref, fn = resolve(park)
    if not ref or not fn:
        print(f"cannot resolve: reference={ref} function={fn}")
        return 1
    text = open(os.path.join(ROOT, park), errors="replace").read()
    claimed = CLAIM.search(text[:9000])
    print(f"park      {park}")
    print(f"reference {ref}")
    print(f"function  {fn}")
    print(f"claims    {claimed.group(1) + ' of ' + claimed.group(2) if claimed else '(no figure)'}")

    base = measure(park, ref, fn)
    if base is None:
        print("\nBASE DOES NOT MEASURE -- objcmp stopped before the encoding compare.")
        print("Nothing here is meaningful until the body reaches the reference's length.")
        return 1
    print(f"base      {base[0]} of {base[1]}\n")
    best = [(base[0], "base", "")]

    if a.flags:
        print("FLAG AXIS -- unchanged body, one per-file flag group at a time")
        for flag, group in FLAGS:
            r = measure(park, ref, fn, extra=flag)
            v = f"{r[0]} of {r[1]}" if r else "NO VERDICT -- length changed, or a parse miss; CHECK BY HAND"
            mark = ""
            if r and r[0] < base[0]:
                mark = "  <-- BETTER"
            if r and r[0] == 0:
                mark = "  <-- *** ZERO ***"
            print(f"  {flag:28} {group:20} {v}{mark}")
            if r:
                best.append((r[0], f"flag {flag}", group))
        print()

    span = body_span(text, fn)
    if (a.anchor or a.barrier) and span:
        i, j = span
        pts = boundaries(text, i, j)
        for label, snippet, dev in (("ANCHOR", "\n    do { } while (0);", False),
                                    ("BARRIER", '\n    __asm__ volatile ("");', True)):
            if label == "ANCHOR" and not a.anchor:
                continue
            if label == "BARRIER" and not a.barrier:
                continue
            print(f"{label} AXIS -- {min(len(pts), a.max)} top-level statement "
                  f"boundaries{'  (DEVICE: fakematch-class, pass-3 debt)' if dev else '  (device-free)'}")
            d = tempfile.mkdtemp(prefix="autosweep_", dir=os.path.join(ROOT, "scratch_elev"))
            try:
                names = []
                for n, p in enumerate(pts[:a.max]):
                    v = text[:p] + snippet + text[p:]
                    nm = f"{label.lower()}{n:02d}.c"
                    open(os.path.join(d, nm), "w").write(v)
                    names.append(nm)
                rel = os.path.relpath(d, ROOT)
                out = subprocess.run(
                    DOCKER + ["python3", "tools/sweep_variants.py", park, ref, fn, rel],
                    capture_output=True, text=True).stdout
                # sweep_variants.py prints `NAME  ndiff=N  first=N  dsize=N  rel=ok`,
                # NOT a bare column -- parsing f[1] as an int silently matched
                # nothing and the axis reported no rows at all, which reads like a
                # clean sweep.  Parse the key.
                for line in out.splitlines():
                    f = line.split()
                    m = re.search(r"ndiff=(\d+)", line)
                    if f and f[0].endswith(".c") and m:
                        nd = int(m.group(1))
                        tag = ""
                        if nd < base[0]:
                            tag = "  <-- BETTER"
                        if nd == 0:
                            tag = "  <-- *** ZERO ***"
                        if tag:
                            print(f"  {line}{tag}")
                        best.append((nd, f"{label.lower()} @ {f[0]}", ""))
                shown = [b for b in best if b[1].startswith(label.lower())]
                if not shown:
                    print(f"  !! NO ROWS PARSED from sweep_variants -- do NOT read this"
                          f" as a clean sweep; check its output by hand")
                elif min(x[0] for x in shown) >= base[0]:
                    print(f"  all {len(shown)} inert or worse (best "
                          f"{min(x[0] for x in shown)}, base {base[0]})")
            finally:
                shutil.rmtree(d, ignore_errors=True)
            print()

    best.sort()
    print("BEST")
    for nd, how, grp in best[:5]:
        print(f"  {nd:5}  {how} {grp}")
    if best[0][0] == 0:
        print("\n*** A ZERO WAS FOUND BY SWEEP ALONE -- verify it, then land it. ***")
    elif best[0][0] < base[0]:
        print(f"\nImproved {base[0]} -> {best[0][0]} with no judgment applied.")
    else:
        print("\nBoth cheap axes are CLOSED on this park.  That is a recorded")
        print("negative worth putting in its header before an agent starts.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
