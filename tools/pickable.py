#!/usr/bin/env python3
"""pickable.py -- rank the remaining functions by how likely they are to match.

Thirteen rounds of near-misses established the blocker conditions in
docs/elevation.md. This encodes them as a filter, which is what they are for.
The first candidate it produced, GetJupiterDjinni, matched after one documented
lever.

REJECTS a function if any of these hold:

  * fewer than 40 instructions -- every lever here works through the register
    allocator, and a tiny function gives it nothing to act on
  * more than 120 instructions -- too many independent residues to converge
  * any use of r8-r11 -- allocation-priority residues, the wall that holds
    OvlFunc_883_200d64c, OvlFunc_901_2008350 and OvlFunc_949_200807c
  * an expensive constant used twice anywhere in the body -- cse if the uses are
    close, PRE hoisting if one dominates, and neither yields to any spelling
  * fewer than 8 calls -- arithmetic bodies hit instruction selection instead

    python3 tools/pickable.py [--limit N]

Prints the survivors, most calls first, with the .s they live in and whether a
split is needed.  Functions already parked under src/non_matching are dropped:
the filter scores assembly, so a parked function ranks exactly as well as a
fresh one and will otherwise be re-derived from scratch every round.

A CAUTION EARNED THE HARD WAY. Count `mov rN, #imm` and a later `lsl rN, #k` as
ONE constant even with instructions between them. The first version of this
filter required them adjacent, and on that reading it PASSED
OvlFunc_952_200be40 -- the function whose hoisting motivated the filter, where
the ROM puts `mov r0, #8` between the pair. Requiring adjacency admits exactly
the case the filter exists to reject.
"""
import collections
import os
import re
import subprocess
import sys

FSTART = re.compile(r"^\.(thumb|arm)_func_start\s+(\S+)", re.M)
MOV = re.compile(r"^\tmov\t(r\d+), #(0x[0-9a-f]+|\d+)$")
LSL = re.compile(r"^\tlsl\t(r\d+), #(\d+)$")
POOL = re.compile(r"^\tldr\tr\d+, =(\S+)$")


def constants(body):
    """Count expensive constants, pairing a mov with a later lsl of the same reg."""
    counts = collections.Counter()
    pending = {}
    for line in body.split("\n"):
        m = MOV.match(line)
        if m:
            pending[m.group(1)] = int(m.group(2), 0)
            continue
        m = LSL.match(line)
        if m and m.group(1) in pending:
            counts[pending.pop(m.group(1)) << int(m.group(2))] += 1
            continue
        m = POOL.match(line)
        if m:
            counts[m.group(1)] += 1
    return counts


def parked():
    """Addresses already parked under src/non_matching, by file basename.

    The filter ranks on the assembly alone, so a function that was tried and
    parked in an earlier round scores exactly as well as a fresh one and floats
    straight back to the top.  Two rounds were spent re-deriving parks that were
    already written -- OvlFunc_955_2009424 and OvlFunc_967_2008308 were both at
    the head of the list on the day their park notes were sitting in the tree.
    """
    out = set()
    for root, _, files in os.walk("src/non_matching"):
        for f in files:
            if f.endswith(".c"):
                out.add(f[:-2])
    return out


def main():
    limit = 20
    skip = parked()
    if "--limit" in sys.argv:
        limit = int(sys.argv[sys.argv.index("--limit") + 1])
    files = [f for f in subprocess.run(["git", "ls-files", "asm"],
                                       capture_output=True, text=True).stdout.split()
             if f.endswith(".s") and "/m4a" not in f and "f9000" not in f
             and os.path.exists(f)
             and not os.path.exists("src/" + f[len("asm/"):-2] + ".c")]
    rows = []
    for f in files:
        t = open(f, errors="ignore").read()
        starts = [(m.start(), m.group(1), m.group(2)) for m in FSTART.finditer(t)]
        for i, (off, kind, name) in enumerate(starts):
            if kind != "thumb":
                continue
            end = starts[i + 1][0] if i + 1 < len(starts) else len(t)
            body = t[off:end]
            size = len(re.findall(r"^\t[a-z]", body, re.M))
            if not 40 <= size <= 120:
                continue
            calls = len(re.findall(r"^\tbl\t", body, re.M))
            if calls < 8:
                continue
            if re.search(r"\b(r8|r9|r10|r11|sl|fp)\b", body):
                continue
            if any(v > 1 for v in constants(body).values()):
                continue
            addr = name.rsplit("_", 1)[-1]
            if addr in skip:
                continue
            rows.append((calls, size, name, f, len(starts)))
    rows.sort(reverse=True)
    print(f"{len(rows)} candidates pass the filter (already-parked addresses excluded)\n")
    for calls, size, name, f, n in rows[:limit]:
        split = "SINGLE" if n == 1 else f"split from {n}"
        print(f"  {calls:3d} calls  {size:3d} insns  {name:<26} {split:<14} {f}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
