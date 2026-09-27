#!/usr/bin/env python3
"""repoint_parks.py OLD.s -- after splitting OLD.s, repoint park recipes at the new pieces.

    python3 tools/repoint_parks.py asm/rom_8a000/rom_944ec_a_c_a_c_c_a.s

split_s.py deletes OLD.s and writes OLD_a.s / OLD_b.s / OLD_c.s. Every park under
src/non_matching whose `Verify with:` recipe names OLD.s with `--func NAME` still
points at a file that no longer exists, and parkcheck then reports it as
UNCHECKABLE ("reference not found"). This rewrites each such recipe to the piece
that now holds NAME.

A park that mentions OLD.s WITHOUT a `--func` (prose, or a single-function
recipe) is listed as UNCHANGED for a human to fix -- guessing which piece a
prose mention meant is not safe.

Written during batches 286-289 (web session), where it was run after every split.
"""
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__.strip().splitlines()[2].strip())
    old = sys.argv[1]
    os.chdir(ROOT)
    stem = old[:-2]
    where = {}
    for p in sorted(glob.glob(stem + "_*.s")):
        for l in open(p, errors="ignore"):
            m = re.match(r"\s*\.thumb_func_start\s+(\S+)", l, re.I)
            if m:
                where[m.group(1)] = p
    for park in sorted(glob.glob("src/non_matching/**/*.c", recursive=True)):
        s = open(park, errors="ignore").read()
        if old not in s:
            continue
        t = re.sub(re.escape(old) + r"(\s*(?:\\\n\s*\*\s*)?--func\s+)(\w+)",
                   lambda m: where.get(m.group(2), old) + m.group(1) + m.group(2), s)
        if t != s:
            open(park, "w").write(t)
            print("repointed", park)
        else:
            print("UNCHANGED (mention without --func)", park)


if __name__ == "__main__":
    main()
