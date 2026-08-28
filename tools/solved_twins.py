#!/usr/bin/env python3
"""solved_twins.py -- find remaining functions whose shape matches a SOLVED one.

WHY

OvlFunc_924_200d900 turned out to be OvlFunc_923_200a370 with one call target
changed. 200a370 had been elevated the round before, and the second cost one
`sed` and one screen. It was found by accident.

tools/twin_families.py groups the REMAINING functions against each other, which
finds families but still needs the first member solved the hard way. This
searches the other direction: remaining functions against the ones already
matched. A hit is the cheapest possible elevation -- copy the .c, change the
immediates, screen.

An earlier attempt at this returned zero and the reason is recorded in
twin_families.py: gcc-generated .s use `.thumb_func` + `NAME:`, not the ROM's
`.thumb_func_start` macro, so a single parser sees no solved functions at all.
This reads both notations, and refuses to run if either corpus comes back empty
-- a zero on one side is what made that failure look like a real answer. The
guard earned its keep immediately: the first run here reported 0 solved out of
3095 files, because gcc puts a `.type` line between `.thumb_func` and the
label and the parser wanted them adjacent.

THE CORPUS IS A BUILD ARTEFACT. `asm/<path>/X.s` is gcc's output whenever
`src/<path>/X.c` exists (Makefile rule `asm/%.o: src/%.c`), so the solved corpus
is simply every .s that HAS a .c counterpart, and it is only as current as the
last build. Build before trusting the output.

MATCHING is on the mnemonic stream alone -- no registers, no immediates, no
branch targets. That is deliberately loose: it is a candidate generator, and
differing immediates are exactly what makes a twin cheap rather than useless.
Screen every hit; a matching mnemonic stream is not a matching function.
"""
import os
import re
import sys
import collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ROM_START = re.compile(r"^\.thumb_func_start(?:_noalign)? (\S+)")
ROM_END = re.compile(r"^\.func_end")
GCC_NAME = re.compile(r"^(\w+):$")
INSN = re.compile(r"^\t([a-z][a-z0-9]*)\b")
SKIP = {"thumb_func", "align", "size", "type", "global", "text", "code"}


def opcodes(lines, i, stop):
    ops = []
    while i < stop:
        m = INSN.match(lines[i])
        if m and m.group(1) not in SKIP:
            ops.append(m.group(1))
        i += 1
    return tuple(ops)


def scan(path, solved):
    lines = [l.rstrip("\n") for l in open(path, errors="ignore")]
    out = []
    if solved:
        # gcc emits `.thumb_func` then `.type NAME,function` then `NAME:` --
        # the directive is NOT the immediately preceding line, and requiring
        # that it be is what made the first run of this report zero solved
        # functions out of 3095 files.
        marks = []
        for i, l in enumerate(lines):
            m = GCC_NAME.match(l)
            if m and any(lines[j].strip() == ".thumb_func"
                         for j in range(max(0, i - 3), i)):
                marks.append((i, m.group(1)))
        for n, (i, name) in enumerate(marks):
            stop = marks[n + 1][0] - 1 if n + 1 < len(marks) else len(lines)
            out.append((name, opcodes(lines, i + 1, stop)))
    else:
        cur, start = None, 0
        for i, l in enumerate(lines):
            m = ROM_START.match(l)
            if m:
                cur, start = m.group(1), i + 1
            elif cur and ROM_END.match(l):
                out.append((cur, opcodes(lines, start, i)))
                cur = None
    return out


def main():
    minlen = int(sys.argv[1]) if len(sys.argv) > 1 else 12
    solved, remaining = {}, []
    for root, _, files in os.walk(os.path.join(ROOT, "asm")):
        for fn in files:
            if not fn.endswith(".s"):
                continue
            p = os.path.join(root, fn)
            is_solved = os.path.exists(p.replace("/asm/", "/src/", 1)[:-2] + ".c")
            for name, ops in scan(p, is_solved):
                if len(ops) < minlen:
                    continue
                if is_solved:
                    solved.setdefault(ops, []).append((name, p))
                else:
                    remaining.append((name, ops, p))

    if not solved or not remaining:
        sys.exit(f"refusing to report: solved={len(solved)} remaining={len(remaining)}."
                 " One side is empty, so a zero result would be meaningless."
                 " Build first (asm/*.s for solved functions are build output).")

    hits = collections.defaultdict(list)
    for name, ops, p in remaining:
        if ops in solved:
            hits[solved[ops][0]].append((name, len(ops), p))

    print(f"solved shapes {len(solved)}, remaining functions {len(remaining)}, "
          f"min length {minlen}")
    print(f"REMAINING FUNCTIONS WITH A SOLVED TWIN: {sum(len(v) for v in hits.values())}"
          f" across {len(hits)} templates\n")
    for (sname, spath), members in sorted(hits.items(), key=lambda kv: -len(kv[1])):
        src = spath.replace("/asm/", "/src/", 1)[:-2] + ".c"
        print(f"  template {sname}  ({os.path.relpath(src, ROOT)})")
        for name, n, p in members:
            print(f"      {n:4} insns  {name:28} {os.path.relpath(p, ROOT)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
