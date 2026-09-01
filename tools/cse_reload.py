#!/usr/bin/env python3
"""cse_reload.py -- find functions whose ROM shows a CSEd SECOND READ.

Batch 178 established that the ROM shape

    ldrb r2, [r5]
    mov  r3, r2
    cmp  r3, #0

is NOT a redundant register copy. It is a second read of the same memory in the
source, which gcc-2.96 turns into a register copy. Six functions were parked on
the other reading; writing them with nothing named -- `if (*p == 0) ...;
*p = *p + 1;` rather than `n = *p; if (n == 0) ...` -- matched them on the first
screen.

That makes the shape a SELECTOR, and unlike every other ranking in this tree it
is read off the assembly directly rather than inferred from size, family or
flags. This tool reports it: a load into rA immediately followed by
`mov rB, rA`, counted per function.

    python3 tools/cse_reload.py [--max-insns N] [--parked | --unparked]

The count is a hint, not a promise. A load-then-copy also appears at a jump
table's `mov pc, r3`, at a return value reloaded from the stack, and wherever
the allocator simply wanted a different register -- so read the function before
believing the column. What it is good at is the opposite direction: a body that
comes out ONE OR MORE LINES SHORT of a reference that has this shape is almost
always missing reads, and the fix is to REMOVE a name rather than add one.

At the time of writing: 344 remaining functions show it, 89 of them already
parked. The parked ones are the better hunting ground, because a park that
recorded "a redundant copy we cannot produce" was usually one spelling away.
"""
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pickable
import shapesib

LOAD = re.compile(r"^\s*(ldr|ldrb|ldrh|ldrsb|ldrsh)\s+(r\d+),\s*\[")
MOVRR = re.compile(r"^\s*mov\s+(r\d+),\s*(r\d+)\s*$")
IP = re.compile(r"\br(?:12|14)\b")


def copies(ins):
    """How many load-then-copy pairs the instruction list contains."""
    n = 0
    for i in range(len(ins) - 1):
        m = LOAD.match(ins[i])
        if not m:
            continue
        c = MOVRR.match(ins[i + 1])
        if c and c.group(2) == m.group(2) and c.group(1) != m.group(2):
            n += 1
    return n


def main():
    max_insns = 90
    want = None
    for i, a in enumerate(sys.argv):
        if a == "--max-insns":
            max_insns = int(sys.argv[i + 1])
        if a == "--parked":
            want = True
        if a == "--unparked":
            want = False

    parked = pickable.parked()
    rows = []
    for s in glob.glob("asm/**/*.s", recursive=True):
        if os.path.exists(s[:-2].replace("asm/", "src/", 1) + ".c"):
            continue
        for name, body in shapesib.functions(s):
            pk = name in parked
            if want is not None and pk != want:
                continue
            ins = [l for l in body if l.strip()
                   and not l.strip().startswith((".", "@", "/*"))]
            if len(ins) > max_insns:
                continue
            n = copies(ins)
            if not n:
                continue
            # r12/r14 holding a value is a real wall; r8-r11 is not (batch 177)
            bo = [l for l in ins if not re.search(r"^\s*(push|pop)\b", l)]
            hi = any(IP.search(l) for l in bo)
            rows.append((n, len(ins), name, s, pk, hi))

    rows.sort(key=lambda r: (-r[0], r[1]))
    print("%d functions show the CSEd-second-read shape (%d parked)\n"
          % (len(rows), sum(1 for r in rows if r[4])))
    for n, ln, name, s, pk, hi in rows:
        print("  %d copies %3di  %-28s %s%s%s"
              % (n, ln, name, s,
                 "  [PARKED]" if pk else "",
                 "  <- r12/r14" if hi else ""))


if __name__ == "__main__":
    main()
