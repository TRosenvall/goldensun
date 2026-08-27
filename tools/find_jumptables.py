#!/usr/bin/env python3
"""find_jumptables.py -- list the functions still in asm/ that dispatch through a
JUMP TABLE, smallest first.

WHY

gcc-2.96 lowers a dense switch into a real table:

    cmp r0, #5 / bhi <default>
    ldr r2, =.Ltable / lsl r3, r0, #2 / ldr r3, [r3, r2] / mov pc, r3
  .Ltable:
    .word ... one entry per value in the range ...

Until batch 101 no function of this shape had been matched, so they were
effectively invisible: pick_candidates.py ranks by call count and instruction
count and never singled them out. There are 106 of them, and they share a small
set of readings (see docs/elevation.md), so working them as a class is worth
more than meeting them one at a time.

WHAT THE COLUMNS MEAN

    insn    instructions in the function body, excluding the .word table
    slots   entries in the table -- the size of the case range, NOT the number
            of distinct cases. A big number with few distinct targets means most
            of the range falls to the default.
    tgts    distinct labels the table points at. `slots - tgts` is roughly how
            much of the range is default, and a function with tgts == 2 is
            usually a two-case switch that happened to be dense.

USAGE

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \\
        python3 tools/find_jumptables.py [--max-insn N]

READ THE TABLE BEFORE WRITING THE SWITCH. The table's own entries tell you the
case values: slot i is case (base + i), and slots pointing at the default label
are values with no case. Batch 101's GetWeaponSpriteID has a slot for case 4
holding the default label, which is what keeps the range dense enough for gcc to
choose a table at all -- write those cases out or the table collapses into a
decision tree.
"""
import glob
import os
import re
import sys

FUNC = re.compile(r"^\.thumb_func_start (\S+)", re.M)
JUMP = re.compile(r"^\s*mov\s+pc,\s*r\d+\s*$", re.M)
WORD = re.compile(r"^\s*\.word\s+(\.L\w+)\s*$", re.M)
CODE = re.compile(r"^\t[a-z]")


def scan(max_insn):
    out = []
    for path in sorted(glob.glob("asm/**/*.s", recursive=True)):
        text = open(path, errors="replace").read()
        if not JUMP.search(text):
            continue
        parts = FUNC.split(text)
        for i in range(1, len(parts), 2):
            name = parts[i]
            body = parts[i + 1].split(".func_end")[0]
            if not JUMP.search(body):
                continue
            words = WORD.findall(body)
            insn = sum(1 for ln in body.split("\n")
                       if CODE.match(ln) and ".word" not in ln)
            if insn <= max_insn:
                out.append((insn, len(words), len(set(words)), name, path))
    return sorted(out)


def main():
    max_insn = 10 ** 9
    if "--max-insn" in sys.argv:
        max_insn = int(sys.argv[sys.argv.index("--max-insn") + 1])
    rows = scan(max_insn)
    print(f"{'insn':>5} {'slots':>6} {'tgts':>5}  name / file")
    for insn, slots, tgts, name, path in rows:
        print(f"{insn:5} {slots:6} {tgts:5}  {name}  {path}")
    print(f"\n{len(rows)} jump-table function(s) still in asm/")
    return 0


if __name__ == "__main__":
    sys.exit(main())
