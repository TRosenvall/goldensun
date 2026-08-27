#!/usr/bin/env python3
"""prologue_families.py -- cluster every remaining assembly function by its
FIRST N INSTRUCTIONS, so families can be worked together.

WHY

Batch 108 got five of its seven functions by taking a function that had just
matched, escaping its first six instructions, and grepping every .s. That
worked because these functions are identical only at the START and diverge
freely afterwards -- whole-function shape comparison (tools/find_twins.py,
tools/find_families.py) scores them apart, but the prologue is where the
decisions that cost screens are already made: which values are cached in
callee-saved registers, which globals are read, what the loop bounds are.

Doing it one function at a time means only finding a family AFTER solving a
member. This does it for the whole tree at once, so the biggest families can be
picked first.

WHAT IT DOES

Reads every .thumb_func_start in asm/, takes the first N instruction lines
(default 6), canonicalises the operands that vary within a family -- immediates
and pool symbols -- and groups on the result. Prints clusters of two or more,
largest first, with the raw prologue of the first member.

    python3 tools/prologue_families.py [--n 6] [--min 2] [--max-insn 200]

CANONICALISATION IS THE WHOLE DESIGN QUESTION. Too aggressive and unrelated
functions collide; too little and the family splits. What varies within a real
family, measured on the two batch-108 families:

  * immediates          `mov r5, #8`  vs  `mov r5, #0x10`     -> #K
  * pool symbols        `ldr r0, =.L250c` vs `=.L1d00`        -> =S
  * branch targets      `.L652` vs `.L472`                    -> L

and what does NOT vary is the register numbering, the opcode sequence, and
which operand position each register sits in. So registers are kept literal.
"""
import argparse
import glob
import re
import sys
from collections import defaultdict

START = re.compile(r"^\.thumb_func_start (\S+)")
END = re.compile(r"^\.func_end")
INSN = re.compile(r"^\t([a-z][a-z0-9.]*)\t?(.*)$")
IMM = re.compile(r"#(0x[0-9a-f]+|\d+)")
POOL = re.compile(r"=\S+")
LABEL = re.compile(r"\.L\w+")


def canon(op, args):
    a = IMM.sub("#K", args)
    a = POOL.sub("=S", a)
    a = LABEL.sub("L", a)
    return op + "\t" + a


def functions(path):
    name, body = None, []
    for line in open(path, errors="replace").read().split("\n"):
        m = START.match(line)
        if m:
            name, body = m.group(1), []
            continue
        if name is None:
            continue
        if END.match(line):
            yield name, body
            name = None
            continue
        m = INSN.match(line)
        if m and m.group(1) not in ("align",):
            body.append((m.group(1), m.group(2)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--n", type=int, default=6)
    ap.add_argument("--min", type=int, default=2)
    ap.add_argument("--max-insn", type=int, default=200)
    ap.add_argument("--show", type=int, default=20)
    args = ap.parse_args()

    groups = defaultdict(list)
    for f in glob.glob("asm/**/*.s", recursive=True):
        for name, body in functions(f):
            if len(body) < args.n or len(body) > args.max_insn:
                continue
            key = "\n".join(canon(o, a) for o, a in body[:args.n])
            groups[key].append((name, len(body), f, body[:args.n]))

    fams = sorted((v for v in groups.values() if len(v) >= args.min),
                  key=lambda v: (-len(v), min(x[1] for x in v)))
    print("%d families of %d+ sharing their first %d instructions"
          % (len(fams), args.min, args.n))
    print("%d functions in them\n" % sum(len(v) for v in fams))
    for fam in fams[:args.show]:
        fam.sort(key=lambda x: x[1])
        print("=== %d members, %d..%d instructions" %
              (len(fam), fam[0][1], fam[-1][1]))
        for op, a in fam[0][3]:
            print("      %s %s" % (op, a))
        for name, n, f, _ in fam:
            print("    %4d  %-26s %s" % (n, name, f))
        print()
    return 0


if __name__ == "__main__":
    sys.exit(main())
