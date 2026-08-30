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
  * fewer than 5 calls -- arithmetic bodies hit instruction selection instead
    (was 8 until batch 151; see the note at the test)
  * three or more `neg` -- the `-1` triple, an unbroken class (batch 148)

    python3 tools/pickable.py [--limit N]

Prints the survivors, most calls first, with the .s they live in and whether a
split is needed, and how many ARGUMENT-CONSTRUCTION INTERLEAVE sites it carries
-- `mov r0` sitting inside another argument's mov/lsl pair. That is not a reject
condition (a function has matched with one), but it is the residue that parked
three functions in batch 148 and no lever reaches it in straight-line code, so
prefer a zero over a three.  Functions already parked under src/non_matching are
dropped:
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
# `mov rN, #imm` ... `mov r0, #imm` ... `lsl rN, #k` -- a first argument loaded
# INSIDE another argument's two-instruction constant build. Batch 148 parked
# three functions on nothing but this, six sites between them, and none of the
# levers in docs/elevation.md moved a single one on a function with no register
# pressure. It is not a reject condition -- OvlFunc_927_20099b8 matched with one
# -- but a function carrying three of them is three separate coin flips, so the
# count is worth seeing before choosing.
INTERLEAVE = re.compile(r"\tmov\t(r\d+), #\S+\n(?:\t[^\n]*\n)*?\tmov\tr0, #\S+\n\tlsl\t\1, #")
# Three or more `neg` in one function is the `-1` triple of
# src/non_matching/overlays/constant_reuse.c: the ROM materialises the same
# small negative constant once per argument register and gcc builds it once and
# copies. Every function in the tree carrying it is parked, none is elevated,
# and batch 148 confirmed it on a function with a SINGLE such call, so it is not
# cross-site CSE and there is nothing at the call site to change. Treat it as a
# reject until that class breaks.
NEG = re.compile(r"^\tneg\t", re.M)
# The two ways a park note names the function it covers.
# Any identifier, not just Func_<hex>: plenty of functions carry real names.
# StartEarthquake was parked and came straight back to the top of this list
# because the old pattern required a hex address suffix in the NAME.  The
# `-- 0x<addr>` and `<name>  asm/<path>.s` anchors are specific enough on their
# own; loose mentions in prose still do not match.
PARK_HDR = re.compile(r"^\s*(?:/\*)?\s*([A-Za-z_]\w*)\s*--\s*0x", re.M)
PARK_LST = re.compile(r"\b([A-Za-z_]\w*)\s+asm/\S+\.s")
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
    """Functions already parked under src/non_matching.

    Two ways a park claims a function, and BOTH are needed.  The filename is
    one -- src/non_matching/ovl_7b2078/2008658.c parks OvlFunc_926_2008658.
    But a park covering a CLASS is named for the class and lists its members
    inside: message_base_register.c parks OvlFunc_962_200806c and
    OvlFunc_950_2008500, and neither address appears in any filename.  Both sat
    at the top of this list with their note already written.

    So the contents are scanned too, for the two conventions park notes use:
    `<NAME> -- 0x<addr>` in a header and `<NAME>  asm/<path>.s` in a member
    list.  Loose mentions ("see OvlFunc_926_200a484") are deliberately not
    matched.  Names that ARE matched but have since been elevated cost nothing:
    a function whose src/*.c exists never reaches this list.

    The filter ranks on the assembly alone, so a function that was tried and
    parked in an earlier round scores exactly as well as a fresh one and floats
    straight back to the top.  Two rounds were spent re-deriving parks that were
    already written -- OvlFunc_955_2009424 and OvlFunc_967_2008308 were both at
    the head of the list on the day their park notes were sitting in the tree.
    """
    out = set()
    for root, _, files in os.walk("src/non_matching"):
        for f in files:
            if not f.endswith(".c"):
                continue
            out.add(f[:-2])
            text = open(os.path.join(root, f), errors="ignore").read()
            out |= set(PARK_HDR.findall(text))
            out |= set(PARK_LST.findall(text))
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
            if not 25 <= size <= 120:
                continue
            calls = len(re.findall(r"^\tbl\t", body, re.M))
            # RELAXED in batch 151 from 8 to 5.  The floor was measured when
            # call-dense functions were plentiful; they are now worked through,
            # and at >=8 the list had fallen to 17 with 7 clean of interleave.
            # At >=5 it is 31 and 20.  Measured, not guessed: dropping the floor
            # is what moves the population, and the size floor is worth almost
            # nothing (40 -> 25 adds one).
            #
            # HONEST CAVEAT: the two newly surfaced functions tried first --
            # OvlFunc_951_20096a8 and OvlFunc_947_2008f58 -- both PARKED, and
            # both on the register-allocation coin flip, which has nothing to do
            # with call density.  The relaxation admits legitimate candidates;
            # it does not make the dominant wall any softer.
            if calls < 5:
                continue
            if re.search(r"\b(r8|r9|r10|r11|sl|fp)\b", body):
                continue
            if any(v > 1 for v in constants(body).values()):
                continue
            if len(NEG.findall(body)) >= 3:
                continue
            if name.rsplit("_", 1)[-1] in skip or name in skip:
                continue
            rows.append((calls, size, name, f, len(starts),
                         len(INTERLEAVE.findall(body))))
    rows.sort(reverse=True)
    print(f"{len(rows)} candidates pass the filter (already-parked addresses excluded)\n")
    for calls, size, name, f, n, iv in rows[:limit]:
        split = "SINGLE" if n == 1 else f"split from {n}"
        print(f"  {calls:3d} calls  {size:3d} insns  {iv} interleave  "
              f"{name:<26} {split:<14} {f}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
