#!/usr/bin/env python3
"""aliastell.py -- find functions whose ROM RE-READS a field across a store of a
different width.

WHY

`-fno-strict-aliasing` closed two functions in two rounds and a systematic sweep
of the thirty closest parks found NOTHING. It is not a flag to try
speculatively; it is the remedy for one shape, and that shape is visible in the
ROM listing:

    ldr  rX, [rB, #K]      a field read
    ...
    strh rY, [rC, #J]      a store of a DIFFERENT width
    ...
    ldr  rZ, [rB, #K]      the SAME field, read again

with NO intervening call: a call forces a reload regardless, so a re-read across
one says nothing about aliasing.

At -O2 gcc-2.96 has strict aliasing on, so a short store provably cannot alias
an int read and the second load is commoned away. Losing it shifts everything
downstream, so one missing load presents as fifty instructions of divergence --
Func_808d828 was 68 differing against 7 with the flag, Func_80935d4 was 54
against 4.

MATCHING is textual on the load operand, which is deliberately loose: the same
`[rB, #K]` may be a different object after a reallocation, and a genuine re-read
may use a different register pair. It is a candidate generator. Confirm by
screening with and without the flag; the doc's caution about sweeping applies.

Scans use \\s+ and never a literal space -- .s files put a TAB between mnemonic
and operands.
"""
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pickable
import shapesib

LOAD = re.compile(r"^\s*(ldr|ldrh|ldrb|ldrsh|ldrsb)\s+r\d+,\s*(\[r\d+,\s*#[^\]]*\])")
STORE = re.compile(r"^\s*(str|strh|strb)\s+r\d+,")
CALL = re.compile(r"^\s*bl\s+")
WIDTH = {"ldr": 4, "str": 4, "ldrh": 2, "strh": 2, "ldrsh": 2,
         "ldrb": 1, "strb": 1, "ldrsb": 1}


def hits(body):
    """Count re-read-across-a-narrower-store sites."""
    n = 0
    seen = {}                       # operand -> width of the load
    for l in body:
        m = LOAD.match(l)
        if m:
            op, addr = m.group(1), m.group(2)
            w = WIDTH[op]
            if addr in seen and seen[addr][1]:
                n += 1
                seen[addr] = (w, False)
            else:
                seen[addr] = (w, False)
            continue
        if CALL.match(l):
            # A CALL forces a reload anyway -- gcc cannot assume a global or a
            # pointed-to field survives it -- so a re-read across a call says
            # nothing about aliasing. OvlFunc_888_2008848 was offered on exactly
            # that shape and the flag changed nothing; its real blocker is a
            # dominance hoist of a repeated mask.
            seen.clear()
            continue
        m = STORE.match(l)
        if m:
            sw = WIDTH[m.group(1)]
            # a store of a DIFFERENT width makes every wider pending load
            # re-readable in the ROM but commonable for us
            for a, (w, _) in list(seen.items()):
                if w != sw:
                    seen[a] = (w, True)
    return n


if __name__ == "__main__":
    parked = pickable.parked()
    rows = []
    for s in glob.glob("asm/**/*.s", recursive=True):
        if os.path.exists(s[:-2].replace("asm/", "src/", 1) + ".c"):
            continue
        for name, body in shapesib.functions(s):
            if name in parked:
                continue
            code = [l for l in body
                    if l.strip() and not l.strip().startswith((".", "@", "/*"))]
            if not (20 <= len(code) <= 130):
                continue
            h = hits(code)
            if h:
                rows.append((h, len(code), name, s))
    rows.sort(key=lambda r: (-r[0], r[1]))
    print("%d unparked candidates show the re-read tell\n" % len(rows))
    for h, n, name, s in rows[:25]:
        print("  %d site(s) %3di %-28s %s" % (h, n, name, s))
