#!/usr/bin/env python3
"""showfunc.py -- print named functions out of the asm corpus.

Elevation targets usually sit inside a multi-function .s of several thousand
lines, so reading the whole file to see thirty instructions is not workable.
Takes function names and finds them wherever they live.

    python3 tools/showfunc.py MapActor_SetIdle free gfree
"""
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
START = re.compile(r"\s*\.(?:thumb_func_start(?:_noalign)?|arm_func_start)\s+(\S+)")


def main():
    want = set(sys.argv[1:])
    found = set()
    for p in sorted(glob.glob(os.path.join(ROOT, "asm", "**", "*.s"), recursive=True)):
        L = open(p, errors="replace").read().split("\n")
        for i, l in enumerate(L):
            m = START.match(l)
            if not m or m.group(1) not in want:
                continue
            found.add(m.group(1))
            # walk back over the annotation block so the prose comes with it
            j = i
            while j > 0 and (L[j - 1].lstrip().startswith("@") or not L[j - 1].strip()):
                j -= 1
            k = i + 1
            while k < len(L) and not re.match(r"\s*\.func_end", L[k]) and not START.match(L[k]):
                k += 1
            print(f"@@@@ {m.group(1)}  --  {os.path.relpath(p, ROOT)}")
            print("\n".join(x for x in L[j:k + 1] if x.strip()))
            print()
    for w in sorted(want - found):
        print(f"@@@@ {w}  --  NOT FOUND")


if __name__ == "__main__":
    main()
