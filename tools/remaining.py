#!/usr/bin/env python3
"""remaining.py -- how many functions are still in assembly.

WHY THIS EXISTS

Batches 130-133 each printed a "remaining" figure in their header, and the
series ran 2224 -> 2219 -> 2214 -> 2208 -> 2202 with every step exactly equal to
that batch's elevation count. It was a hand-maintained counter being
decremented, not a measurement, so an error in its baseline could never correct
itself. By batch 134 it had drifted 46 below the real number.

Counting it four different ways -- with and without excluding TUs that already
have a .c, by raw occurrence and by distinct name -- gives the SAME answer, so
there is no genuine ambiguity in the definition. gcc-generated .s intermediates
carry `.thumb_func` rather than `.thumb_func_start`, so they never contaminate
the count and no exclusion rule is needed.

    python3 tools/remaining.py [--verbose]

Print the number in a batch report from this tool rather than by subtracting.
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
START = re.compile(r"^\s*\.(?:thumb_func_start(?:_noalign)?|arm_func_start)\s+(\S+)")


def remaining():
    names = []
    for root, _, files in os.walk(os.path.join(ROOT, "asm")):
        for fn in files:
            if not fn.endswith(".s"):
                continue
            with open(os.path.join(root, fn), errors="ignore") as f:
                for line in f:
                    m = START.match(line)
                    if m:
                        names.append(m.group(1))
    return names


def main():
    names = remaining()
    elevated = sum(1 for r, _, fs in os.walk(os.path.join(ROOT, "src"))
                   for f in fs if f.endswith(".c") and "non_matching" not in r)
    parks = sum(1 for r, _, fs in os.walk(os.path.join(ROOT, "src", "non_matching"))
                for f in fs if f.endswith(".c"))
    if "--verbose" in sys.argv:
        for n in sorted(names):
            print(n)
    print(f"remaining {len(names)}  (distinct {len(set(names))})"
          f"   elevated {elevated}   parked {parks}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
