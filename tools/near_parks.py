#!/usr/bin/env python3
"""near_parks.py -- how close is every parked function to matching?

WHY

tools/rank_parks.py ranks by `--align` distance too, but it re-screens with
flag sweeps and is slow enough that it is run rarely. This does the one thing
that turned out to matter for planning: count the instructions in DISAGREEING
REGIONS for every park, and sort.

WHAT THE FIRST RUN FOUND (batch 58): 60 of the parked set are within SIX
instructions of matching. That is the single most useful number for deciding
what to work on, because it separates two very different populations that the
word "parked" hides:

  a park at 2 of 24 is a compiler difference nobody has cracked
  a park at 30 of 34 is a function whose C is probably wrong

Both are "not matching". Only the second is worth re-reading from scratch.

USE THE NUMBER, NOT THE RANK. Being close does NOT mean being reachable -- many
of the 60 sit on named, settled classes (lower-bound canonicalisation, the
dma.h register binding, pre-header load merge) where the residual is a floor,
not a gap. Read the park note before spending a round; it says which.

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \\
        python3 tools/near_parks.py
    python3 tools/near_parks.py --max 3     # only the very close ones
"""
import glob
import os
import re
import subprocess
import sys

ROOT = "/work" if os.path.isdir("/work/asm") else \
    os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = re.compile(r"Source asm:\s*goldensun/(\S+\.s)")
HEAD = re.compile(r"XX (\S+)\s+\(rom (\d+) lines, ours (\d+)")
DIS = re.compile(r"(\d+) instruction\(s\) in disagreeing regions, of (\d+)")


def main():
    a = sys.argv[1:]
    cap = int(a[a.index("--max") + 1]) if "--max" in a else 6
    rows, screened, matched = [], 0, 0
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        rel = os.path.relpath(p, ROOT)
        m = SRC.search(open(p, errors="replace").read())
        if not m or not os.path.exists(os.path.join(ROOT, m.group(1))):
            continue
        screened += 1
        r = subprocess.run(
            [sys.executable, os.path.join(ROOT, "tools", "tryc.py"),
             rel, "--ref", m.group(1), "--align", "--quiet"],
            capture_output=True, text=True, cwd=ROOT)
        out = r.stdout + r.stderr
        h = HEAD.search(out)
        if not h:
            # no XX line means it screened clean -- worth knowing, it may be
            # unparkable now for a reason unrelated to this sweep
            if " OK " in out:
                matched += 1
                print(f"  ?? screens clean (NOT necessarily unparkable): {rel}")
            continue
        d = DIS.search(out)
        n = int(d.group(1)) if d else abs(int(h.group(2)) - int(h.group(3)))
        rows.append((n, h.group(1), rel, h.group(2)))
    rows.sort()
    near = [r for r in rows if r[0] <= cap]
    print(f"\n{screened} parks screened, {len(near)} within {cap} instructions\n")
    for n, fn, rel, rom in near:
        print(f"  {n:2d} of {rom:<4s} {fn:<24s} {rel}")
    if matched:
        print(f"\n{matched} park(s) screen clean. THIS IS NOT AN UNPARK SIGNAL ON ITS")
        print("OWN. tryc normalises literal-pool loads, so a function whose only")
        print("defect is POOL PLACEMENT screens OK and still fails make compare --")
        print("src/non_matching/ovl_7ec19c/200816c.c is exactly that and says so in")
        print("its note. Read the park before acting; it may already record this.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
