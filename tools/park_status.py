#!/usr/bin/env python3
"""park_status.py -- what each parked function is blocked on and how close it
got, parsed from the park files themselves.

WHY

349 parks accumulate the record of what has been tried, and a quarter of
everything ever parked has since been elevated -- 117 of 466 -- because a new
lever landed and somebody re-read the note. That only works if the closest ones
can be found again. A hand-rolled regex over these files got it WRONG twice in
one sitting: it reported two parks at "0 differing" that are really 4 and a
one-instruction length difference, and it matched a blocker line in only 77 of
349 because the headers use several formats.

So the parsing is deliberate here rather than incidental.

WHAT IT READS

  differing   the best screen recorded, from any of the formats in use:
                  "N of M", "N differing", "N instructions in disagreeing
                  regions", "Best screen: N"
              Only counts a number that is followed or preceded by a word tying
              it to a diff, never a bare integer.
  class       the text after "BLOCKER:" or "BLOCKER CLASS:", normalised.
  code        whether the park carries its candidate C, or points at a
              scratch/ file, or has neither -- a park with no C cannot be
              re-screened without rebuilding it from the .s.

    python3 tools/park_status.py [--close N] [--class]
"""
import os
import re
import sys
import collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DIFF = [
    re.compile(r"(\d+)\s+of\s+\d+\s+differ", re.I),
    re.compile(r"(\d+)\s+differing", re.I),
    re.compile(r"(\d+)\s+instructions in disagreeing", re.I),
    re.compile(r"Best screen:\s*(\d+)", re.I),
    re.compile(r"(\d+)\s+of\s+\d+[,.]", re.I),
]
BLOCK = re.compile(r"BLOCKER(?:\s+CLASS)?:\s*([^\n]{3,70})", re.I)


def parse(path):
    t = open(path, errors="ignore").read()
    best = None
    for rx in DIFF:
        for m in rx.finditer(t):
            v = int(m.group(1))
            if v <= 400 and (best is None or v < best):
                best = v
    m = BLOCK.search(t)
    cls = re.sub(r"\s+", " ", m.group(1)).strip(" .-") if m else None
    code = re.sub(r"//[^\n]*", "", re.sub(r"/\*.*?\*/", "", t, flags=re.S))
    has_c = bool(re.search(r"\w", code))
    scratch = re.findall(r"scratch/(\S+?\.c)", t)
    return best, cls, has_c, scratch


def main():
    lim = int(sys.argv[sys.argv.index("--close") + 1]) if "--close" in sys.argv else 6
    rows, classes, nocode = [], collections.Counter(), 0
    for root, _, fs in os.walk(os.path.join(ROOT, "src", "non_matching")):
        for f in fs:
            if not f.endswith(".c"):
                continue
            p = os.path.join(root, f)
            best, cls, has_c, scratch = parse(p)
            classes[cls or "(unstated)"] += 1
            usable = has_c or any(
                os.path.exists(os.path.join(ROOT, "scratch", s)) for s in scratch)
            if not usable:
                nocode += 1
            rows.append((best if best is not None else 999, usable,
                         os.path.relpath(p, ROOT), cls))
    if "--class" in sys.argv:
        print(f"{len(rows)} parks by stated blocker class:\n")
        for k, v in classes.most_common():
            print(f"  {v:4d}  {k}")
        return 0
    rows.sort()
    close = [r for r in rows if r[0] <= lim]
    print(f"{len(rows)} parks; {nocode} have no usable C anywhere; "
          f"{len(close)} at {lim} differing or fewer\n")
    for best, usable, p, cls in close:
        print(f"  {best:3d}  {'C ' if usable else '--'}  {p}")
        if cls:
            print(f"          {cls[:66]}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
