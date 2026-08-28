#!/usr/bin/env python3
"""poolblocked.py -- functions that cannot match because of literal-pool placement.

WHY

old_agbcc emits a function's constant pool at `.func_end` and never in the
middle. Some ROM functions instead dump the pool early and jump over it:

        b .L6a0
        .pool_aligned
    .L6a0:
        ...

That `b` is a real instruction we cannot produce.

CORRECTED (batch 142). The original test here looked for `.pool_aligned` with a
`b .L...` in the THREE PRECEDING LINES, and reported 312 of 2212. That is too
narrow twice over: the directive is sometimes plain `.pool`, and the ROM often
puts `.align` and one or more `.word` blocks between the branch and the pool,
so the `b` is further than three lines away. OvlFunc_919_200815c is the case
that exposed it -- a textbook branch-over-pool that the old test missed and
that therefore kept being offered as a fresh candidate.

The test is now "a `.pool` or `.pool_aligned` inside the body with CODE after
it", which is the property that actually cannot be produced, and it gives 521.

That wider test was checked against the corpus that already matches before
being adopted: ZERO of 3494 matching functions contain a mid-body pool. A
LOOSER version -- any mid-body data, including `.word` -- is wrong, because 85
matching functions do have mid-body `.word` blocks and they are switch JUMP
TABLES, which gcc emits routinely. Pools and jump tables are both "data in the
middle" and mean opposite things.

It was measured across the
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
POOL = re.compile(r"^\s*\.pool(_aligned)?\s*$")
CODE = re.compile(r"^\t[a-z]")


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
            seen_pool = False
            for l in L:
                m = START.match(l)
                if m:
                    cur = m.group(1)
                    total += 1
                    seen_pool = False
                    continue
                if not cur:
                    continue
                if l.startswith(".func_end"):
                    cur = None
                elif POOL.match(l):
                    seen_pool = True
                elif seen_pool and CODE.match(l):
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
