#!/usr/bin/env python3
"""rank_parks.py -- parked functions, closest first.

WHY

The parked set is a hundred files and reads as a flat list of dead ends. It is
not one: some members are a single instruction from matching and some are
thirty. Nothing said which, so picking one to re-attempt meant opening files
until one looked close, and the file that LOOKS closest is the one with the best
written note rather than the smallest diff.

This screens every park with `tryc.py --align` and sorts by instructions inside
disagreeing regions. The first run put a park at ONE instruction out -- a load's
base and offset the wrong way round -- and it was matched the same round by
naming the table pointer. Nothing about that function had changed since it was
parked; it was just never at the top of anything.

WHY --align AND NOT THE HEADLINE COUNT

The headline count is positional, so one extra instruction on either side makes
every later position report as different. A park that is short by one reads as
"thirty differ" and sorts below one that is genuinely thirty out. See
docs/elevation.md.

CAVEATS

  * A park whose .s no longer exists is skipped -- some were elevated from a
    file that has since been split, and the reference moved.
  * `0 of 0` means the screen says OK. That is not a match: it is the
    pool-placement false positive, and `make compare` is the authority. See
    src/non_matching/ovl_7ec19c/200816c.c.
  * The count says how far the OUTPUT is, not how hard the remaining step is.
    A one-instruction park can be a known wall and a ten-instruction one can be
    three levers already in docs/elevation.md.

    python3 tools/rank_parks.py            # closest first
    python3 tools/rank_parks.py --limit 40
"""
import glob
import os
import re
import subprocess
import sys

ROOT = "/work" if os.path.isdir("/work/asm") else \
    os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = re.compile(r"Source asm:\s*goldensun/(\S+\.s)")
COUNT = re.compile(r"(\d+) instruction\(s\) in disagreeing regions, of (\d+)")


def main():
    a = sys.argv[1:]
    lim = int(a[a.index("--limit") + 1]) if "--limit" in a else 25
    rows, skipped = [], 0
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        rel = os.path.relpath(p, ROOT)
        m = SRC.search(open(p, errors="replace").read())
        if not m or not os.path.exists(os.path.join(ROOT, m.group(1))):
            skipped += 1
            continue
        r = subprocess.run([sys.executable, os.path.join(ROOT, "tools/tryc.py"),
                            rel, "--ref", m.group(1), "--align"],
                           capture_output=True, text=True, cwd=ROOT)
        out = r.stdout + r.stderr
        if re.search(r"^\s+OK ", out, re.M):
            rows.append((0, 0, rel, "screens OK -- verify with make compare"))
            continue
        c = COUNT.search(out)
        if c:
            rows.append((int(c.group(1)), int(c.group(2)), rel, ""))
    rows.sort()
    print(f"{len(rows)} parked files screened, {skipped} skipped (no reference)\n")
    print(f"{'out':>4} {'of':>5}  file")
    for bad, tot, rel, note in rows[:lim]:
        print(f"{bad:4d} {tot:5d}  {rel}  {note}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
