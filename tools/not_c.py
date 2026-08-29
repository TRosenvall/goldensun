#!/usr/bin/env python3
"""not_c.py -- the functions that are NOT compiler output and must never be
elevated to C.

WHY THIS EXISTS

The candidate tools rank by size and call count, so the smallest functions in
the ROM float to the top of every list. Eighteen of the twenty-six functions
under ten instructions are the MP2K sound driver, and they are hand-written
assembly -- not gcc output at any flag setting. Nothing in C produces them, so
they will be re-proposed, re-examined and re-rejected on every sweep unless
something says otherwise. This is that something.

HOW THEY ARE IDENTIFIED, from the ROM's own bytes rather than by reputation:

  mov r12, lr ... bx r12    a hand-rolled return convention. gcc pushes lr.
  adr r2, X / bx r2         a deliberate switch INTO ARM mode -- umul3232H32
                            does this to reach `umull`, which Thumb lacks.
  bl .Lxxxx                 a `bl` to a label INSIDE another function, not to a
                            function entry. No compiler emits that.
  b .Lxxxx (cross-function) falling out of one function into a neighbour's label.

CORROBORATION ALREADY IN THIS TREE. `src/lib/m4a/m4a0.s` is a hand-written
assembly copy of the same driver, carried in from Coaltergeist's tree in the
"Adopt Coaltergeist's tree as the build base" commit. It is not built and not
linked -- its own `.include` paths do not even resolve here -- but it is
evidence of the established practice: upstream ships the MP2K driver AS
ASSEMBLY. The C half of m4a (`m4a.c`, `m4a_tables.c`) is built and linked; the
driver half is not decompiled by anyone.

WHAT THIS DOES NOT SAY. These functions are not unmatched -- they assemble and
they are byte-exact today. They are simply already in their final form. They
should be counted as DONE, not as remaining work.

USAGE

    from not_c import is_not_c, NOT_C_FILES
    if is_not_c(fn, path): continue

or on the command line, to see the list and the current count:

    python3 tools/not_c.py
"""
import os
import re
import sys

# Whole files that are the MP2K driver. Every function in them is hand-written.
NOT_C_FILES = (
    "asm/rom_f9000/rom_f95e0.s",
    "asm/rom_f9000/rom_f9ef8_a.s",
)

# Idioms that identify hand-written assembly if it turns up anywhere else.
HAND_WRITTEN = re.compile(
    r"mov\s+r12,\s*lr"          # hand-rolled return convention
    r"|bx\s+r12"
    r"|\badr\b"                 # deliberate ARM-mode switch
    r"|\bumull\b"
    r"|\bbl\s+\.L"              # bl to a label inside another function
)


def is_not_c(path, body=None):
    """True if this function must not be elevated.

    `path` is the .s it lives in; `body` is its instruction text, if available.
    The file list is the fast answer; the idiom check catches anything that
    moves or is split out later.
    """
    p = path.replace("\\", "/")
    for f in NOT_C_FILES:
        if p.endswith(f) or p == f:
            return True
    return bool(body and HAND_WRITTEN.search(body))


def main():
    root = "/work" if os.path.isdir("/work/asm") else \
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    total = 0
    for f in NOT_C_FILES:
        p = os.path.join(root, f)
        if not os.path.exists(p):
            print(f"  MISSING {f}")
            continue
        names = re.findall(r"\.thumb_func_start (\S+)", open(p, errors="replace").read())
        total += len(names)
        print(f"{f}: {len(names)} functions")
        for n in names:
            print(f"    {n}")
    print(f"\n{total} functions are NOT compiler output and are excluded from "
          f"elevation candidates.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
