#!/usr/bin/env python3
"""poolblocked.py -- functions that cannot match because of literal-pool placement.

WHY

old_agbcc emits a function's constant pool at `.func_end` and never in the
middle. Some ROM functions instead dump the pool early and jump over it:

        b .L6a0
        .pool_aligned
    .L6a0:
        ...

That `b` is a real instruction we cannot produce. It was measured across the
whole tree: mid-function pools appear in ZERO of the elevated translation units,
and the cluster hypothesis -- that pool placement might be a property of the
translation unit rather than the function -- was tested and refuted (compiling a
function alone and with a second function appended puts the pool in the same
place both times). See docs/elevation.md.

So this is a toolchain limit, not a spelling problem, and a function carrying it
is unreachable however correctly the body is transcribed.

RUN IT BEFORE TRANSCRIBING. It costs one pass over the .s files and it saved a
196-call transcription the first time it was used, on OvlFunc_974_200829c --
which otherwise looked ideal: 588 instructions, three callees, no branches, no
shifts, reuse 0.

    python3 tools/poolblocked.py [--list]
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
START = re.compile(r"^\s*\.(?:thumb_func_start(?:_noalign)?|arm_func_start)\s+(\S+)")
BR = re.compile(r"^\tb\t\.L\w+$")


def scan():
    blocked, total = [], 0
    for root, _, files in os.walk(os.path.join(ROOT, "asm")):
        for fn in files:
            if not fn.endswith(".s"):
                continue
            p = os.path.join(root, fn)
            if os.path.exists(p.replace("/asm/", "/src/", 1)[:-2] + ".c"):
                continue
            L = open(p, errors="ignore").read().split("\n")
            cur = None
            for i, l in enumerate(L):
                m = START.match(l)
                if m:
                    cur = m.group(1)
                    total += 1
                    continue
                if cur and l.strip() == ".pool_aligned":
                    if any(BR.match(L[j]) for j in range(max(0, i - 3), i)):
                        blocked.append((cur, os.path.relpath(p, ROOT)))
                        cur = None
    return blocked, total


def main():
    blocked, total = scan()
    if "--list" in sys.argv:
        for n, p in sorted(blocked):
            print(f"{n:32} {p}")
    pct = 100.0 * len(blocked) / total if total else 0.0
    print(f"remaining {total}   branch-over-pool {len(blocked)}   ({pct:.1f}% unreachable)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
