#!/usr/bin/env python3
"""rescreen_park.py -- re-run every parked function against the current screen.

WHY

A parked function is not a graveyard entry. It is a queue item that gets
cheaper every time a technique lands, and the cost of checking is a few
seconds per file.

This was learned the expensive way. `OvlFunc_906_2008314` sat parked with a
note that correctly diagnosed both of its residual diffs and proposed the
permuter for one of them. It needed neither -- only an in-tree precedent that
happened to be written three batches later. Re-screening the park turned that
one parked function into ten elevated ones.

So: run this after ANY new technique lands, not when the candidate list runs
dry.

WHAT IT DOES NOT TELL YOU

`tools/tryc.py` is a screen, not a verdict. A file reported here as matching
still has to go through a split (if it needs one) and `make compare`.

Parked files live in src/non_matching/, which is NOT where the build would put
them, so the Makefile has no per-file rule for their path. tryc.py takes the
flags from the --ref assembly for exactly this reason -- an earlier version of
this sweep screened at -O2 a translation unit the build compiles at -O1 and
reported a clean match for a function that does not match at all. The parked
note had warned about it in prose. Prose does not run.
"""
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = re.compile(r"Source asm:\s*goldensun/(\S+\.s)")


def screen(path, ref):
    r = subprocess.run(
        ["docker", "run", "--rm", "-v", ROOT + ":/work", "-w", "/work",
         "goldensun-build", "python3", "tools/tryc.py", path, "--ref", ref],
        capture_output=True, text=True, cwd=ROOT)
    return r.stdout + r.stderr


def main():
    verbose = "-v" in sys.argv
    hits, rows = [], []
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        rel = os.path.relpath(p, ROOT)
        m = SRC.search(open(p, errors="replace").read())
        if not m:
            rows.append((rel, "no Source asm: header"))
            continue
        ref = m.group(1)
        if not os.path.exists(os.path.join(ROOT, ref)):
            # the .s was elevated or split out from under this note
            rows.append((rel, f"asm gone: {ref}"))
            continue
        out = screen(rel, ref)
        if "  OK " in out:
            hits.append((rel, ref))
            rows.append((rel, "*** MATCHES ***"))
        elif "REFUSING" in out:
            rows.append((rel, "ref is generated"))
        elif "COMPILE FAILED" in out:
            rows.append((rel, "compile failed"))
        else:
            d = re.search(r"first diff at (\d+)\)", out)
            n = re.search(r"rom (\d+) lines, ours (\d+)", out)
            note = f"diff at {d.group(1)}" if d else "mismatch"
            if n and n.group(1) != n.group(2):
                note += f"  ({n.group(1)} vs {n.group(2)} lines)"
            rows.append((rel, note))

    for rel, note in rows:
        if verbose or "MATCH" in note or "gone" in note or "failed" in note:
            print(f"{note:28s} {rel}")

    print(f"\n{len(rows)} parked, {len(hits)} now matching")
    for rel, ref in hits:
        print(f"  {rel}\n      --ref {ref}")
    if not hits:
        print("Nothing new. Re-run after the next technique lands.")


if __name__ == "__main__":
    main()
