#!/usr/bin/env python3
"""guarded_interleave.py -- find interleave sites that are actually REACHABLE.

The split-constant interleave is one of the biggest blocker classes in the tree
and most of it is out of reach, but the two halves are separable and only one of
them has ever been solved:

  * the constant is an ARGUMENT TEMPORARY at a STRAIGHT-LINE site -- gcc
    rematerialises it during argument fill and discards whatever statement
    structure the source imposed. Nothing reaches these. docs/elevation.md sizes
    98 remaining functions this way and this round added two more parks to them
    (ovl_7987ac/200811c.c, ovl_7f2f14/2009150.c).

  * the site is DOMINATED BY A CONDITIONAL BRANCH -- name the two-instruction
    builds as locals in the entry block, and gcc declines to keep them live
    across the guard and rematerialises at the use, interleaving the way the ROM
    does. This is the construct in src/overlays/rom_7ef4f4/ovl_30_a_c_a_c.c,
    which applies it at FIVE sites at once, and it is what closed
    OvlFunc_909_2008338 and OvlFunc_913_2008a68.

So the branch is the lever and the naming is only how you reach across it.
Selecting on "has an interleave" mixes the two populations and mostly returns
the hopeless one; selecting on "has an interleave WITH a guard before it" is the
list worth working.

Counts a `mov`+`lsl` and a `mov`+`neg` build alike -- docs/elevation.md
generalises the shape to any two-instruction build, and a filter that
implemented only the `lsl` example let a `neg` case through this round.

Scans must use \\s+, never a literal space: .s files put a TAB between mnemonic
and operands.
"""
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pickable
import shapesib

MOVI = re.compile(r"^\s*mov\s+(r\d+),\s*#(0x[0-9a-f]+|\d+)\s*$")
SPLIT = re.compile(r"^\s*(?:lsl|neg)\s+(r\d+),")
COND = re.compile(r"^\s*b(?:eq|ne|lt|gt|le|ge|hi|ls|cs|cc|mi|pl)\b")
CALL = re.compile(r"^\s*bl\s+")


def sites(body):
    """(guarded, unguarded) interleave-site counts for one function body."""
    guarded = unguarded = 0
    seen_branch = False
    for i, l in enumerate(body):
        if COND.match(l):
            seen_branch = True
        m = MOVI.match(l)
        if not m:
            continue
        reg = m.group(1)
        # a single-instruction argument sitting between the mov and its
        # completing lsl/neg is what the ROM does and gcc will not do
        for j in range(i + 1, min(i + 5, len(body))):
            s = SPLIT.match(body[j])
            if s and s.group(1) == reg:
                if j > i + 1 and any(MOVI.match(body[k]) for k in range(i + 1, j)):
                    if seen_branch:
                        guarded += 1
                    else:
                        unguarded += 1
                break
            if CALL.match(body[j]):
                break
    return guarded, unguarded


if __name__ == "__main__":
    parked = pickable.parked()
    rows = []
    for s in glob.glob("asm/**/*.s", recursive=True):
        if os.path.exists(s[:-2].replace("asm/", "src/", 1) + ".c"):
            continue
        for name, body in shapesib.functions(s):
            if name in parked:
                continue
            ins = [l for l in body
                   if l.strip() and not l.strip().startswith((".", "@", "/*"))]
            if not (25 <= len(ins) <= 130):
                continue
            g, u = sites(ins)
            # every site must be guarded, or the unguarded one blocks the function
            if g and not u:
                rows.append((g, len(ins), name, s))
    rows.sort(key=lambda r: (-r[0], r[1]))
    print("%d candidates with ONLY guarded interleave sites\n" % len(rows))
    for g, n, name, s in rows[:25]:
        print("  %d site(s) %3di %-28s %s" % (g, n, name, s))
