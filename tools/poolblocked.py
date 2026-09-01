#!/usr/bin/env python3
"""poolblocked.py -- functions whose ROM dumps a literal pool mid-body.

**CORRECTED, AND THE ORIGINAL CLAIM WAS FALSE.** This file used to report these
as UNREACHABLE, on the premise quoted below, and the census counted them as a
certain blocker -- 501 of 1979 remaining functions, a quarter of the corpus.

    "old_agbcc emits a function's constant pool at `.func_end` and never in the
     middle. That `b` is a real instruction we cannot produce."

The premise is about **old_agbcc**, which in this tree builds only five m4a and
agb_flash objects. Every elevated function is compiled by **gcc-2.96**, and
gcc-2.96 emits mid-body pools with skip jumps routinely. Compiling a plain
transcription of Func_80bad7c produces:

        b       .L10
    .L19:
        .align  2, 0
    .L18:
        .word   256
        .word   iwram_3001e74
    .L3:

which is exactly the shape the tool called impossible -- two of them, in one
function, from ordinary C with no lever applied.

The supporting measurement was real but circular: "mid-function pools appear in
ZERO of the elevated translation units" is true, and it is true BECAUSE this
tool rejected every candidate that would have one. Nobody attempted a
pool-blocked function, so none was ever elevated, so the count stayed at zero.

AND HALF THE COUNT WAS NEVER A POOL AT ALL

`.pool_aligned` is a macro that flushes any PENDING literals. When there are
none it emits nothing. The old test looked for the directive with code after it
and counted the marker, not the data, so CreateSpriteLayer -- whose two markers
are both empty and whose two `b` instructions are ordinary control flow (a loop
entry and a branch to the epilogue) -- was reported as blocked.

Splitting the 501 by whether any `.word`/`.byte`/`.hword` actually precedes the
marker:

    167   EMPTY marker -- nothing to jump over, never blocked in any sense
    334   real mid-body pool -- a hazard, and reproducible (see above)

(Run the tool for the live split; the boundary depends on how far back you look
for data before the marker, and a hand count with a wider window gave 235/266.
The point is not the exact figure -- it is that a third of the class has no pool
at all.)

So the class contains ZERO certain blockers. It contains 167 functions that were
rejected for a directive that emits no bytes, and 334 that need pool placement
watched.

WHAT THE TEST STILL MEASURES, AND WHY IT IS STILL WORTH RUNNING

A mid-body pool is not a blocker but it IS a hazard, and two of them are
recorded:

  * The skip jump is a real instruction and it counts. `rom_f6000/80f6148.c` is
    two lines SHORT of its ROM because the ROM has two skip jumps and our pool
    went to the end; `rom_15000/801bd98.c` is one line LONG because OURS has the
    jump and the ROM's pool is elsewhere. A one-line difference next to a `b`
    whose target is the next label is pool placement.

  * Placement follows the pool's CONTENTS, so it is reachable from C. Removing
    one pool entry -- by naming a stored value so gcc builds it with mov/lsl
    instead of pooling it -- removed a whole pool and its skip jump in
    OvlFunc_921_2008384 (batch 176) and again twice in ovl_7fa4ec/2008da4.c.

So: treat the output as "expect to fight over pool placement", not "do not
start". Reported functions are candidates like any other.

    python3 tools/poolblocked.py [--list]
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
START = re.compile(r"^\s*\.(?:thumb_func_start(?:_noalign)?|arm_func_start)\s+(\S+)")
POOL = re.compile(r"^\s*\.pool(_aligned)?\s*$")
CODE = re.compile(r"^\t[a-z]")


DATA = re.compile(r"^\s*\.(word|byte|hword|short|long|incbin|space)\b")


def scan(real_only=False):
    """Functions with a mid-body pool marker.

    real_only=True reports only those where DATA actually precedes the marker;
    the rest are empty `.pool_aligned` flushes that emit nothing.
    """
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
            recent = []
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
                    seen_pool = sum(1 for x in recent[-6:] if DATA.match(x)) or not real_only
                elif seen_pool and CODE.match(l):
                    blocked.append((cur, os.path.relpath(p, ROOT)))
                    cur = None
                recent.append(l)
    return blocked, total


def main():
    blocked, total = scan()
    if "--list" in sys.argv:
        for n, p in sorted(blocked):
            print(f"{n:32} {p}")
    pct = 100.0 * len(blocked) / total if total else 0.0
    real, _ = scan(real_only=True)
    print(f"remaining {total}   mid-body pool marker {len(blocked)}"
          f"   of which REAL pools {len(real)}, empty markers {len(blocked) - len(real)}")
    print("NEITHER IS A BLOCKER -- gcc-2.96 emits mid-body pools. See the docstring.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
