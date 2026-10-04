#!/usr/bin/env python3
"""install_batch.py -- install a whole batch of landings from agent MANIFEST.json files.

    python3 tools/install_batch.py scratch_elev/b320/*/MANIFEST.json            # validate only
    python3 tools/install_batch.py .../MANIFEST.json --phase exports
    python3 tools/install_batch.py .../MANIFEST.json --phase splits
    python3 tools/install_batch.py .../MANIFEST.json --phase install

WHY THIS EXISTS.  By batch 319 the coordinator had become the bottleneck, not the
agents.  Twenty-one landings arrived across batches 317-319, and for each one the
coordinator read the candidate's prose header to recover the install path, the
split command, the Makefile flag group, the fakematch row and which parks to
retire -- then did them one at a time.  That reading is most of the coordinator's
context per batch, and it is what caps how many functions a batch can carry.

A manifest moves that information from prose a human must parse into data a script
can act on.  Agents already know every field; they were just writing it in English.

PHASES, AND WHY THEY ARE SEPARATE.  The manual process gates between steps because
*a layout mistake and a bad decompilation look identical at the end*
(tools/split_s.py says so, and batches 317-318 found it true three times).  This
tool keeps that discipline instead of doing everything at once:

    validate  (default)  touch nothing; check every field of every entry
    exports              add the `.global` lines a text/data split needs
      -> GATE: make + compare MUST be green.  A .global emits no bytes.
    splits               run split_s.py for each entry that needs one
      -> GATE: make + compare MUST be green.  Byte-neutral by construction.
    install              write the .c, delete the hand .s, add Makefile flag
                         rules and fakematch rows, retire the parks
      -> GATE: make + compare MUST be green.  THIS is the real test.

It never runs make itself -- it prints the gate command and stops, so the operator
decides.  Running the gate from inside an install script invites treating a red
gate as something to debug rather than to stop on.

MANIFEST FORMAT.  One JSON file per brief, a list of entries:

[
  {
    "function":      "OvlFunc_971_2008128",      // required, as .thumb_func_start spells it
    "candidate":     "scratch_elev/b320/B/p1_candidate.c",
    "reference":     "asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_b.s",
    "install_path":  "src/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_b.c",
    "figure":        0,                           // 0 for a landing; N for a park
    "pins":          0,                           // tools/shimcount.py, INCLUDING inline-asm
    "split":         null,                        // or {"file": "asm/...s", "func": "..."}
    "exports":       [],                          // or [".L2430"] for a text/data split
    "flag_group":    null,                        // or "GCSE_CFLAGS" / "CSE_CFLAGS"
    "flag_reason":   null,                        // required when flag_group is set
    "fakematch":     false,                       // true IFF pins > 0 or a shim ships
    "parks_retire":  ["src/non_matching/ovl_7fb4a8/2008128.c"],
    "notes":         "free text; not acted on"
  }
]

VALIDATION IS THE POINT, not convenience.  Every check below is a mistake this
project has actually made:

  * `function` must appear as `.thumb_func_start` in `reference` -- batch 318's
    briefs were given THREE WRONG SYMBOL NAMES taken from park filenames.
  * `parks_retire` entries must have the landed function as their SUBJECT.  In
    batch 317 the coordinator nearly deleted `8022a7c.c`, a park for a DIFFERENT
    function that merely cited the landing as a callee.
  * `figure` 0 and `pins` > 0 together require `fakematch: true`.  Three landings
    shipped a shim before anyone noticed shimcount.py misses the inline-asm class.
  * `flag_group` requires `flag_reason`, because a per-file flag is an owner-facing
    decision and an unexplained one is unreviewable.
  * `exports` must actually be referenced by the function, and a split whose
    exports are missing is refused by split_s.py anyway -- better to say so here.
  * PIN POLICY (owner decision, batch 319): PREFER A PIN-FREE BODY.  If a landing
    needs pins and no pin-free landing is readily available, DO NOT LAND IT --
    park it at its pin-free figure and leave it for pass 3, which is the depinning
    pass.  An entry with figure 0 and pins > 0 is therefore WARNED about, loudly,
    every time.  The seven such landings already in the tree predate this policy
    and are grandfathered; they are listed in reports/pass3-depin.md.
"""
import argparse, glob, json, os, re, shutil, subprocess, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GATE = ("docker run --rm --security-opt seccomp=unconfined -v \"$PWD:/work\" -w /work \\\n"
        "  goldensun-build sh -c 'make AGBCC_DIR=/opt/agbcc -j8 && "
        "make AGBCC_DIR=/opt/agbcc compare'")
REQUIRED = ("function", "candidate", "reference", "install_path", "figure", "pins")


def rel(p):
    return os.path.join(ROOT, p) if not os.path.isabs(p) else p


def func_in(ref, fn):
    try:
        t = open(rel(ref), errors="ignore").read()
    except OSError:
        return False
    return bool(re.search(rf"(?im)^\.(thumb|arm)_func_start\s+{re.escape(fn)}([ \t]|$)", t))


def park_subject(path):
    """The function a park file is ABOUT: its recipe's --func, else its first line."""
    try:
        s = open(rel(path), errors="replace").read()
    except OSError:
        return None
    m = re.search(r"--func\s+(\w+)", s[:s.find("*/") if s.find("*/") > 0 else 3000])
    if m:
        return m.group(1)
    m = re.match(r"/\*\s*\**\s*((?:Ovl)?Func_[0-9a-zA-Z_]+|[A-Z][A-Za-z0-9_]*)", s)
    return m.group(1) if m else None


def split_already_applied(e):
    """True when this entry's split has run: its source .s is gone and the parts exist.

    THE MANIFEST DESCRIBES THE PRE-SPLIT TREE, BUT THE INSTALL PHASE RUNS
    POST-SPLIT.  split_s.py consumes the original .s, so by install time an
    entry's `reference` and `split.file` legitimately no longer exist -- and the
    first version of this tool reported that as four hard errors and refused a
    whole batch that was proceeding correctly.  A phase tool has to know which
    phase it is in.
    """
    sp = e.get("split")
    if not sp:
        return False
    src = rel(sp["file"])
    if os.path.exists(src):
        return False
    stem = src[:-2]
    return any(os.path.exists(f"{stem}_{p}.s") for p in ("a", "b", "c", "d"))


def validate(entries):
    errs, warns = [], []
    for e in entries:
        tag = e.get("function", "<no function>")
        for k in REQUIRED:
            if k not in e:
                errs.append(f"{tag}: missing required field `{k}`")
        if errs and any(tag in x for x in errs):
            continue
        if not os.path.exists(rel(e["candidate"])):
            errs.append(f"{tag}: candidate not found: {e['candidate']}")
        post = split_already_applied(e)
        if not os.path.exists(rel(e["reference"])):
            if post:
                warns.append(f"{tag}: split already applied -- reference "
                             f"{e['reference']} is gone, as expected")
            else:
                errs.append(f"{tag}: reference not found: {e['reference']}")
        elif not func_in(e["reference"], e["function"]):
            # TWO DIFFERENT FAILURES, AND REPORTING THEM THE SAME WAY IS THE BUG
            # CLASS THIS PROJECT KEEPS PAYING FOR.  A reference with no
            # `.thumb_func_start` for the symbol is EITHER a wrong symbol name
            # (batch 318's briefs carried three, taken from park filenames) OR a
            # reference that is already GENERATED output, because the function
            # landed in an earlier batch -- gcc's .s declares a bare `name:` label
            # and carries its own banner, with no .thumb_func_start anywhere.
            reftext = ""
            try:
                reftext = open(rel(e["reference"]), errors="ignore").read(400)
            except OSError:
                pass
            generated = "Generated by gcc" in reftext
            if generated:
                errs.append(f"{tag}: {e['reference']} is GENERATED output, not hand "
                            f"asm -- this function appears to have ALREADY LANDED. "
                            f"Check whether this manifest entry is a replay.")
            else:
                errs.append(f"{tag}: `.thumb_func_start {e['function']}` is NOT in "
                            f"{e['reference']} -- wrong symbol name, or wrong "
                            f"reference.  Take the name from .thumb_func_start "
                            f"itself, NEVER from a park filename.")
        d = os.path.dirname(rel(e["install_path"]))
        if not os.path.isdir(d):
            errs.append(f"{tag}: install dir does not exist: {d}")
        if e.get("flag_group") and not e.get("flag_reason"):
            errs.append(f"{tag}: flag_group={e['flag_group']} with no flag_reason")
        if e["figure"] == 0 and e["pins"] and not e.get("fakematch"):
            errs.append(f"{tag}: landing with {e['pins']} pin(s) but fakematch is false")
        if e["figure"] != 0 and e.get("install_path", "").startswith("src/") \
           and "non_matching" not in e["install_path"]:
            errs.append(f"{tag}: figure {e['figure']} is NOT a landing, but install_path "
                        f"is a matching-source path: {e['install_path']}")
        for p in e.get("parks_retire", []):
            if not os.path.exists(rel(p)):
                warns.append(f"{tag}: park to retire not found (already gone?): {p}")
                continue
            subj = park_subject(p)
            if subj and subj != e["function"]:
                errs.append(f"{tag}: REFUSING to retire {p} -- its subject is {subj}, "
                            f"not {e['function']}.  Retiring a park for a different "
                            f"function hides that function from every scan.")
        sp = e.get("split")
        if sp and not post:
            if not os.path.exists(rel(sp["file"])):
                errs.append(f"{tag}: split file not found: {sp['file']}")
            elif not func_in(sp["file"], sp.get("func", e["function"])):
                errs.append(f"{tag}: split func {sp.get('func', e['function'])} not in "
                            f"{sp['file']}")
        # PIN POLICY WARNING -- owner decision, batch 319
        if e["figure"] == 0 and e["pins"]:
            warns.append(f"{tag}: *** LANDS WITH {e['pins']} PIN(S).  The owner's policy is "
                         f"PREFER PIN-FREE: if no pin-free landing is readily available, "
                         f"park it at the pin-free figure and leave it for pass 3. "
                         f"Install only if the pin is genuinely unavoidable AND recorded.")
    return errs, warns


def do_exports(entries, dry):
    for e in entries:
        exports = e.get("exports") or []
        if not exports:
            continue
        sp = e.get("split")
        target = rel(sp["file"]) if sp else rel(e["reference"])
        s = open(target).read()
        add = [x for x in exports if f"\t.global {x}\n" not in s]
        if not add:
            print(f"  {e['function']}: exports already present"); continue
        m = re.search(r"\n\t\.section\s+\.(data|rodata)\b[^\n]*\n", s)
        if not m:
            print(f"  !! {e['function']}: no .data/.rodata section found in {target}; "
                  f"add exports by hand BESIDE THE LABELS (never in the preamble -- "
                  f"split_s.py copies the preamble into every part and refuses)")
            continue
        blk = ("@ Exported for a text/data split: the function moves to C and these\n"
               "@ labels stay in asm.  A .global emits no bytes.  They live HERE rather\n"
               "@ than in the preamble because split_s.py copies the preamble into EVERY\n"
               "@ part and correctly refuses when it holds anything but includes/comments.\n"
               + "".join(f"\t.global {x}\n" for x in add))
        print(f"  {e['function']}: + {', '.join(add)} -> {os.path.relpath(target, ROOT)}")
        if not dry:
            open(target, "w").write(s[:m.end()] + blk + s[m.end():])


def do_splits(entries, dry):
    for e in entries:
        sp = e.get("split")
        if not sp:
            continue
        if split_already_applied(e):
            print(f"  {e['function']}: split already applied, skipping")
            continue
        cmd = [sys.executable, os.path.join(ROOT, "tools", "split_s.py"),
               sp["file"], sp.get("func", e["function"])]
        print(f"  {e['function']}: {' '.join(cmd[1:])}")
        if dry:
            r = subprocess.run(cmd + ["--dry-run"], capture_output=True, text=True, cwd=ROOT)
            for l in r.stdout.strip().splitlines()[-4:]:
                print(f"      {l}")
        else:
            r = subprocess.run(cmd, capture_output=True, text=True, cwd=ROOT)
            if r.returncode != 0:
                print(f"      !! split FAILED: {r.stdout.strip().splitlines()[-1:]}")
                sys.exit(1)


def do_install(entries, dry):
    fm, rules = [], []
    for e in entries:
        src, dst = rel(e["candidate"]), rel(e["install_path"])
        print(f"  {e['function']}: {e['candidate']} -> {e['install_path']}  (figure {e['figure']})")
        if not dry:
            shutil.copyfile(src, dst)
        if e["figure"] == 0:
            hand = rel(e["install_path"].replace("src/", "asm/", 1)[:-2] + ".s")
            if os.path.exists(hand):
                print(f"      rm {os.path.relpath(hand, ROOT)}  (hand asm; build regenerates it)")
                if not dry:
                    os.remove(hand)
        for p in e.get("parks_retire", []):
            if os.path.exists(rel(p)):
                print(f"      retire park {p}")
                if not dry:
                    subprocess.run(["git", "rm", "-q", p], cwd=ROOT)
        if e.get("fakematch"):
            fm.append(f"{e['function']}  {e['install_path']}")
        if e.get("flag_group"):
            rules.append((e, e["flag_group"]))
    if fm:
        print(f"\n  fakematch.txt += {len(fm)} row(s)")
        for r in fm:
            print(f"      {r}")
        if not dry:
            with open(os.path.join(ROOT, "fakematch.txt"), "a") as f:
                f.write("".join(r + "\n" for r in fm))
    if rules:
        print(f"\n  *** {len(rules)} per-file flag rule(s) NOT written -- add these to the "
              f"Makefile BY HAND, beside the existing group, with the reason as a comment:")
        for e, g in rules:
            o = e["install_path"].replace("src/", "asm/", 1)[:-2] + ".o"
            print(f"\n      # {g}, {e.get('flag_reason')}")
            print(f"      {o}: {e['install_path']}")
            print(f"      \t$(GCC296_CC) $({g}) -S -o $(@:.o=.s) $<")
            print(f"      \tprintf '\\n\\t.text\\n\\t.align\\t2, 0\\n' >> $(@:.o=.s)")
            print(f"      \tarm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude "
                  f"-o $@ $(@:.o=.s)")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("manifests", nargs="+")
    ap.add_argument("--phase", choices=["validate", "exports", "splits", "install"],
                    default="validate")
    ap.add_argument("--apply", action="store_true", help="actually modify the tree")
    a = ap.parse_args()

    entries = []
    for g in a.manifests:
        for p in sorted(glob.glob(g)):
            try:
                d = json.load(open(p))
            except Exception as ex:
                print(f"!! {p}: unreadable manifest: {ex}")
                sys.exit(1)
            for e in (d if isinstance(d, list) else [d]):
                e["_from"] = p
                entries.append(e)
    print(f"{len(entries)} entr{'y' if len(entries)==1 else 'ies'} from "
          f"{len(set(e['_from'] for e in entries))} manifest(s)\n")

    errs, warns = validate(entries)
    for w in warns:
        print(f"  WARN  {w}")
    if errs:
        print()
        for x in errs:
            print(f"  ERROR {x}")
        print(f"\n{len(errs)} error(s).  Nothing was modified.")
        sys.exit(1)
    print(f"  validation OK ({len(warns)} warning(s))\n")

    if a.phase == "validate":
        lands = sum(1 for e in entries if e["figure"] == 0)
        print(f"  {lands} landing(s), {len(entries)-lands} park(s)")
        print(f"  needs exports : {sum(1 for e in entries if e.get('exports'))}")
        print(f"  needs split   : {sum(1 for e in entries if e.get('split'))}")
        print(f"  needs flag    : {sum(1 for e in entries if e.get('flag_group'))}")
        print(f"\n  run phases in order: exports -> splits -> install, "
              f"gating after EACH with:\n\n{GATE}")
        return

    dry = not a.apply
    print(f"--- phase {a.phase} {'(DRY RUN -- pass --apply to modify)' if dry else '(APPLYING)'} ---")
    {"exports": do_exports, "splits": do_splits, "install": do_install}[a.phase](entries, dry)
    print(f"\n  NOW GATE -- this must be green before the next phase:\n\n{GATE}")


if __name__ == "__main__":
    main()
