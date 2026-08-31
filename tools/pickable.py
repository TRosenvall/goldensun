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
  * (NO LONGER A REJECT, see below) any use of r8-r11 -- the wall that holds
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
# The `(?!r0,)` is LOAD-BEARING and was missing until batch 154.  Without it the
# back-reference can bind to r0 itself, and then the pattern reads
#
#       mov r0, #imm        <- first argument of one call
#       ...
#       mov r0, #imm        <- first argument of the NEXT call
#       lsl r0, #k          <- ...and its shift
#
# which is not an interleave at all: it is two consecutive calls each loading
# r0, with nothing nested inside anything.  An interleave needs TWO DIFFERENT
# registers -- r0 landing inside another register's mov/lsl pair.  Across asm/
# the unguarded pattern fired 4616 times and 320 of those (6.9%) were this
# self-match, which is why the column read high.
# BATCH 156: this shape is ARGUMENT FILL by construction -- the regex requires
# a `mov r0` (a first argument) nested inside another argument's build -- and
# the argument-temporary boundary in docs/elevation.md says naming cannot reach
# it. Measured on three functions in one round, and the count PREDICTS the
# residue exactly at two differing lines per site:
#
#   OvlFunc_883_2008fec   1 site  ->  9 of 91, and 4 spellings byte-identical
#   OvlFunc_953_2008648   3 sites ->  6 of 43, predicted 6 before screening
#
# Still NOT a hard reject, because OvlFunc_927_20099b8 matched carrying one and
# that measurement stands. Sites are now sorted to the bottom rather than
# excluded, so a clean candidate is always tried first.
INTERLEAVE = re.compile(r"\tmov\t(?!r0,)(r\d+), #\S+\n(?:\t[^\n]*\n)*?\tmov\tr0, #\S+\n\tlsl\t\1, #")
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
# A park header names its function as `NAME -- 0xaddr` OR `NAME -- asm/<path>.s`.
# Only the first was matched until batch 159, and 80 of the 182 park headers in
# the tree use the second -- so any park whose FILENAME is not the address (a
# class park, or a function without a hex suffix in its name) was re-offered.
# OvlFunc_common1_148 came back to the top of the list the round after it was
# parked; that is the third re-offer this batch and the reason both forms are
# accepted here rather than fixed park by park.
# THREE header conventions are in use, and each was found the hard way -- by a
# parked function reappearing at the top of the candidate list:
#     NAME -- 0xaddr        107 headers
#     NAME -- asm/<path>.s   81 headers  (added batch 159)
#     NAME @ 0xaddr          29 headers  (added batch 161)
# Func_80064b8 was parked with the third form and was re-offered two batches
# later; it was caught before any C was written only because its shape looked
# familiar. Accept all three separators rather than normalising 217 headers.
PARK_HDR = re.compile(
    r"^\s*(?:/\*)?\s*([A-Za-z_]\w*)\s*(?:--|@)\s*(?:0x|asm/)", re.M)
PARK_LST = re.compile(r"\b([A-Za-z_]\w*)\s+asm/\S+\.s")
FUNC_ANY = re.compile(
    r"^\s*\.thumb_func_start(?:_noalign)?\s+(\S+)|^\s*\.type\s+(\S+?),\s*function",
    re.M)
CDEF = re.compile(r"^[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\)\s*$", re.M)
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


WRITES_REG = re.compile(
    r"^\s*(mov|add|sub|lsl|lsr|asr|and|orr|eor|mul|neg|ldr|ldrb|ldrh|ldrsb|ldrsh|ldm\w*)"
    r"\s+(r\d+)")
MOV_REG_REG = re.compile(r"^\s*mov\s+(r\d+),\s*(r\d+)\s*$")


def pure_copies(insns):
    """Count `mov rX, rY` where rY is NEVER WRITTEN AGAIN.

    Such a copy is a pure duplicate: the two registers hold the same value for
    the rest of the function, so they never diverge.  docs/elevation.md records
    that this shape is UNREACHABLE from C -- any local initialised from the
    source is provably the same rtx and gcc coalesces it, emitting no `mov`.
    Confirmed on Func_80a8b10 (a loop limit), Func_80e38b8 (a base pointer),
    HeightTile_B (an index) and Func_801cee0 (a loaded byte), each of which sat
    within a few lines of the ROM with the copy as the entire residue.

    So a nonzero count is a floor on the differing count that no spelling will
    clear.  It is reported rather than rejected -- a function can carry one and
    still be worth screening if everything else is clean -- but of 51 low-call
    candidates only 17 scored zero, and the first zero-scoring one tried matched
    in two screens (Func_80cd418).

    Deliberately conservative: only `mov rD, rS` is counted, and any later
    write to rS disqualifies it, so a copy whose source is later reassigned
    (the reachable two-names case that closed Func_8092980) is NOT counted.
    """
    n = 0
    for i, line in enumerate(insns):
        m = MOV_REG_REG.match(line)
        if not m:
            continue
        src = m.group(2)
        if any((w := WRITES_REG.match(x)) and w.group(2) == src
               for x in insns[i + 1:]):
            continue
        n += 1
    return n


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
    HEADER FORMATS ARE NOT A RELIABLE KEY -- do not add a sixth regex.

    Four header conventions were accommodated one at a time, each after a
    parked function was re-offered as a candidate.  Auditing all 467 park files
    at once showed the regex approach recognising only 257 of them: the tree
    also contains `NAME [path]`, `NAME -- NON-MATCHING.`, `NAME -- NOT MATCHING`,
    `NAME [path] -- 0xaddr`, `NAME and NAME2 [path]`, headers whose first line
    is a bare `/*`, and class write-ups that name no function at all.  Nearly
    half the parked set was invisible, which is why candidate scans kept
    re-offering work that was already done.

    So this no longer parses the header.  It builds the universe of real
    function names from the .s corpus and looks for any of them in each park
    file's leading comment, plus any C function the file defines.  A new
    header convention cannot break that, because it keys on the NAME rather
    than on the punctuation around it.
    """
    universe = set()
    for root, _, files in os.walk("asm"):
        for f in files:
            if f.endswith(".s"):
                text = open(os.path.join(root, f), errors="ignore").read()
                for a, b in FUNC_ANY.findall(text):
                    universe.add(a or b)

    out = set()
    for root, _, files in os.walk("src/non_matching"):
        for f in files:
            if not f.endswith(".c"):
                continue
            path = os.path.join(root, f)
            out.add(f[:-2])
            text = open(path, errors="ignore").read()
            out |= set(PARK_HDR.findall(text))
            out |= set(PARK_LST.findall(text))
            # the leading comment block, whatever shape it takes
            m = re.match(r"\s*/\*.*?\*/", text, re.S)
            if m:
                out |= {t for t in re.findall(r"\b\w+\b", m.group(0))
                        if t in universe}
            # and anything the file actually defines
            out |= {n for n in CDEF.findall(text) if n in universe}
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
            # r8-r11 IS NOT A REJECT. Measured batch 157: of the matching
            # translation units that use a high register, 202 are GENUINE
            # matches and only 30 are fakematches. The wall this reject was
            # written for is three named functions, not the class. Blanket-
            # rejecting it hid every unparked duplicate group -- all of them
            # use high registers -- and with them the 72 functions that follow
            # from 25 group solutions. Reported as a column instead.
            if any(v > 1 for v in constants(body).values()):
                continue
            if len(NEG.findall(body)) >= 3:
                continue
            if name.rsplit("_", 1)[-1] in skip or name in skip:
                continue
            hireg = 1 if re.search(r"\b(r8|r9|r10|r11|sl|fp)\b", body) else 0
            rows.append((calls, size, name, f, len(starts),
                         len(INTERLEAVE.findall(body)), hireg))
    # Interleave-free candidates FIRST, then most calls. Batch 156 measured why:
    # each interleave site costs exactly two differing lines and no spelling
    # removes it, so a candidate with three of them starts six lines down.
    rows.sort(key=lambda r: (r[5], -r[0]))
    print(f"{len(rows)} candidates pass the filter (already-parked addresses excluded)\n")
    for calls, size, name, f, n, iv, hireg in rows[:limit]:
        split = "SINGLE" if n == 1 else f"split from {n}"
        floor = f"  >={2 * iv} differ" if iv else ""
        hr = "  hi-reg" if hireg else ""
        print(f"  {calls:3d} calls  {size:3d} insns  {iv} interleave  "
              f"{name:<26} {split:<14} {f}{floor}{hr}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
