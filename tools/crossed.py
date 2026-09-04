#!/usr/bin/env python3
"""crossed.py -- reject functions whose ROM crosses mov order against shift order.

    python3 tools/crossed.py OvlFunc_922_2009154 [more names...]

WHY. gcc emits the `mov`s that feed an argument fill IN THE ORDER THEIR
CONSUMING SHIFTS APPEAR. That is measured, on a minimal reproducer, and it is
not about the constants being equal -- `q1 = 0x81` against `q2 = 0x80`
transposes exactly as two identical values do. See "the same-value movs class
is really MOV ORDER SLAVED TO SHIFT ORDER" in docs/elevation.md.

The consequence is a shape no source reaches. Where the ROM has

    mov r1 / mov r2 / lsl r2 / ... / lsl r1

the mov order says r1 first and the shift order says r2 first, and those two
cannot be set independently: writing the third argument inline flips the mov
pair and takes the tail with it. Two mutually exclusive reachable states, the
ROM a third.

So this is a PRE-FILTER, not a diagnosis. It costs one pass over the .s and it
is worth running before choosing a target, because three of the first five
candidates ranked by tools/templated.py carried the shape -- which is why two
consecutive rounds stalled on it after long per-function sweeps.

A CLEAN result is not a promise the function will match; it only means this
particular wall is absent.

Validated against the two functions known to carry it,
src/non_matching/ovl_7c460c/2008ff0.c and src/non_matching/ovl_7d30e0/2008b68.c,
both of which it flags. An earlier version reported both CLEAN because it split
call blocks on `startswith('bl ')` and the disassembly separates the mnemonic
with a TAB, so nothing ever split and the whole function collapsed into one
block. A filter that passes the cases it exists to catch is worse than no
filter; check any change here against those two names.
"""
import re
import sys
import glob
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def call_blocks(body):
    """Split a function body into the run of instructions before each call."""
    cur = []
    for line in body:
        s = line.strip()
        cur.append(s)
        if re.match(r"bl\b", s):
            yield cur
            cur = []


def crossed_sites(body):
    n = 0
    for blk in call_blocks(body):
        movs, lsls = [], []
        for s in blk:
            m = re.match(r"mov\s+r([0-3]),\s*#", s)
            if m:
                movs.append(m.group(1))
            m = re.match(r"lsl\s+r([0-3]),", s)
            if m:
                lsls.append(m.group(1))
        shifted = [r for r in movs if r in lsls]
        first = []
        for r in shifted:
            if r not in first:
                first.append(r)
        if len(first) >= 2:
            order = [r for r in lsls if r in first]
            if order[:len(first)] != first:
                n += 1
    return n


def find(name):
    for s in glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True):
        txt = open(s, errors="ignore").read()
        if ".thumb_func_start " + name in txt:
            return s
    return None


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    bad = 0
    for name in sys.argv[1:]:
        path = find(name)
        if not path:
            print("%-28s NOT FOUND (already elevated, or a typo)" % name)
            continue
        lines = open(path, errors="ignore").read().split("\n")
        i = next(k for k, l in enumerate(lines)
                 if ".thumb_func_start " + name in l)
        j = next((k for k in range(i + 1, len(lines))
                  if ".func_end" in lines[k]), len(lines))
        n = crossed_sites(lines[i + 1:j])
        bad += 1 if n else 0
        print("%-28s crossed-sites=%d  %s" % (name, n, "CLEAN" if not n else "AVOID"))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
