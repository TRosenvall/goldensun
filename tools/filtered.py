#!/usr/bin/env python3
"""filtered.py -- the selection filter from docs/elevation.md, as a tool.

The filter was described in the doc ("A SELECTION FILTER that works") and used
once, but never committed as code, so every round has had to re-derive it. It
rejects a candidate if ANY of these hold:

  * under 40 instructions   -- no register pressure for the levers to act on
  * over 120 instructions   -- too many independent residues to converge on
  * uses r8-r11             -- allocation-priority residues
  * repeats an expensive constant (a pooled value, or a mov+lsl or mov+neg
                               pair) WITHIN ONE BASIC BLOCK
                            -- CSE if the uses are close, PRE hoisting if one
                               dominates the other; neither yields to spelling.
                               A repeat that SPANS A JOIN is no longer a reject:
                               see below.
  * fewer than 8 calls      -- arithmetic-heavy bodies hit instruction
                               selection rather than the documented levers

It does NOT reject a function with no conditional branch, because plenty of
straight-line functions have been elevated. But it REPORTS the branch count,
because the argument-interleave and constant-CSE levers both need a dominating
branch to rematerialise across, and a straight-line candidate whose residue
turns out to be an interleave has nowhere to go. OvlFunc_927_200a1b0 was offered
by this filter, came out at exactly the ROM's 108 lines with six differing, and
every one of the six was a straight-line interleave -- unreachable. Read the
`br` column before picking.

Detector note carried over from the doc: a `mov rN, #imm` followed by a later
`lsl rN, #k` counts as ONE constant even when other instructions sit between
them. Requiring adjacency is what let the first version pass the very function
whose PRE hoisting motivated the filter.

A REPEATED CONSTANT THAT SPANS A JOIN IS A CANDIDATE, NOT A REJECT. Batch 182
established that gcc never re-materialises a value it kept live across a branch,
so a second `mov rN, #imm` on a path where rN already holds imm means the source
had a SECOND VARIABLE whose live range starts there -- not a CSE the ROM
defeated. `OvlFunc_941_2008210` is the specimen: the filter as written rejected
it, and splitting one shared local into two took it from 18 differing to exact.

So the duplicate rule now asks WHERE the repeats sit. If every repeated value
has a label between two of its materialisations, the function is offered with a
`[join]` marker, because the batch-182 lever applies to it. If any value repeats
inside one straight-line run, that is still the hard class and the function is
rejected. This unhid 26 candidates that had been filtered out for the life of
the tool -- the single largest rejection reason in the remaining set.

Scans must use \\s+ and never a literal space: .s files put a TAB between
mnemonic and operands, and a literal space silently returns zero hits.
"""
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import pickable
import shapesib

HIGH = re.compile(r"\br(?:8|9|10|11)\b")
POOL = re.compile(r"\bldr\s+r\d+,\s*=(\S+)")
MOVI = re.compile(r"\bmov\s+(r\d+),\s*#(0x[0-9a-f]+|\d+)")
LSL = re.compile(r"\blsl\s+(r\d+),\s*#(0x[0-9a-f]+|\d+)")
NEG = re.compile(r"\bneg\s+(r\d+),\s*(r\d+)")
WRITE = re.compile(r"\b(?:mov|add|sub|ldr|ldrb|ldrh|lsl|lsr|asr|and|orr|eor|neg|mul|bic)\s+(r\d+)")
LABEL = re.compile(r"^\s*\.?\w+:")

# gcc-2.96 CANNOT emit either of these, so a function containing one was never
# compiled from C and is not an elevation candidate at any effort.  Verified
# against ground truth: ZERO of the 3,474 compiler-output .s files in this tree
# contain either pattern, while 38 unparked functions do.
#
#   mov r12, lr ... bx r12   -- saving the link register in ip instead of a
#                               push/pop frame.  gcc always emits push {lr} and
#                               pops into a scratch to `bx`.
#   bl .Lnnnn                -- branch-and-link to a LOCAL label, usually inside
#                               a different function.  No C construct produces a
#                               call to a label.
#
# Twelve of the 38 are the MP2K sound driver in asm/rom_f9000/rom_f95e0.s, which
# ships as hand-written assembly in real GBA titles and was never C to begin
# with.  They are small and call-light, so a size-calibrated filter offers them
# first -- which is exactly why this check has to run before the size band.
# NOTE the absence of `bl .L` here. It was in this list and it was WRONG.
#
# Thumb encodes a long unconditional branch as a BL pair, and the disassembler
# that produced asm/ renders that pair as `bl <label>`. So `bl .Lxxxx` in these
# files is a JUMP, not a call, and it appears in ordinary compiled code as soon
# as a function grows past the short-branch range. The giveaway shape is a
# conditional branch hopping over it:
#
#       cmp r0, #0x1a
#       beq .Le4f7a        <- skip
#       bl  .Le65f8        <- long branch, ~2700 lines forward
#   .Le4f7a:
#
# The tell was "verified" against 3,474 compiler-OUTPUT .s files, none of which
# contain it -- but that proves nothing, because gcc writes `b` and lets gas
# choose the encoding. The corpus being classified is DISASSEMBLY, a different
# alphabet. Checking ground truth in the wrong alphabet excluded 50 perfectly
# ordinary functions -- 18 whole files -- from the candidate pool, and they
# skewed large precisely because long branches need a large function.
#
# Counted: the buggy pattern matched 23 files / 126 functions; the fixed one
# matches 5 files / 76 functions.
ARM_START = re.compile(r"^\s*\.arm_func_start(?:_noalign)?\s+(\S+)")
HANDASM = re.compile(r"\bmov\s+r12,\s*lr\b|\bbx\s+r12\b")

# A CALL IS NOT ALWAYS A `bl`. gcc-2.96 makes a Thumb indirect call by putting
# the return address in ip and branching through a register:
#
#     mov r12, pc
#     bx  r7
#
# A `bl`-only test walks straight past that, which matters because a call
# clobbers the argument registers and is therefore a boundary for every
# question this file asks about repeated constants. `mov r12, pc` is the
# reliable marker: a bare `bx rN` is just as often a function RETURN
# (`pop {r0}` / `bx r0`), so matching on `bx` alone would count every epilogue
# as a call.
#
# Found while trying to prove a set of functions unreachable: two sites the
# tool reported as separated by neither a label nor a call were in fact
# arguments rebuilt for two successive indirect calls.
CALL = re.compile(r"\bbl\s+|\bmov\s+r12,\s*pc\b")

_handasm_file = {}
_arm_file = {}

# ARM FUNCTIONS ARE NOT ATTEMPTABLE, AND THE HAND-ASM TEST CANNOT SEE THEM.
#
# This build compiles no ARM code at all: every one of the 935 gcc-2.96 rules
# passes -mthumb, and none of the 3,518 solved files is ARM. So a
# `.arm_func_start` function cannot be elevated without a build path that does
# not exist.
#
# They are hand-written besides, but HANDASM will not say so -- it looks for
# `mov r12, lr` and `bx r12`, which are Thumb-era idioms. The ARM ones announce
# themselves differently: three-way transfer-width selection by predication
# (ldrccb/ldreqh/ldrgt), `rrx`, constant tables read via adr+ldm instead of a
# literal pool, and code that rewrites Thumb BL pairs to relocate itself.
# Rather than chase those, reject on the directive, which is exact.
#
# WHY THE ROM HAS ARM AT ALL: these routines are staged in ROM and DMA-copied
# into IWRAM to run there -- see LoadMapCode in src/rom_c0/rom_2e00_c_b.c, which
# copies FixupRamCode_ROM using the size the .s exports and then calls it. IWRAM
# is a 32-bit bus with no waitstates, where ARM beats Thumb; ROM is 16-bit,
# where it would need two fetches per instruction and lose. Everything that
# actually executes from ROM is Thumb.
#
# The test is PER FUNCTION, not per file: rom_15430.s and rom_2544.s each hold
# ARM and Thumb functions side by side.


def arm_functions(path):
    """Names declared with `.arm_func_start` in this .s."""
    hit = _arm_file.get(path)
    if hit is None:
        hit = set()
        try:
            with open(path, errors="ignore") as f:
                for line in f:
                    m = ARM_START.match(line)
                    if m:
                        hit.add(m.group(1))
        except OSError:
            pass
        _arm_file[path] = hit
    return hit


def hand_written(path):
    """True if this .s was assembled by hand rather than compiled.

    THE TEST IS PER FILE, NOT PER FUNCTION, and that distinction matters. A .s
    builds one object, and an object is either compiled or assembled -- never
    both -- so one hand-written routine condemns its whole translation unit.
    Checking per function lets the small helpers through: `umul3232H32` is five
    instructions with no calls and no r12 idiom of its own, so it sorts to the
    very TOP of a size-calibrated ranking while being just as unreachable as the
    driver around it.
    """
    hit = _handasm_file.get(path)
    if hit is None:
        try:
            with open(path, errors="ignore") as f:
                hit = bool(HANDASM.search(f.read()))
        except OSError:
            hit = False
        _handasm_file[path] = hit
    return hit


def constant_sites(body):
    """{value key: [line index of each materialisation]}.

    The index is where the value BEGINS to be built -- the `mov` of a mov+lsl
    pair, not the `lsl` -- because that is where a source-level assignment to it
    would sit, and the join test below compares those positions to label
    positions.
    """
    pos = {}
    pend = {}
    for i, l in enumerate(body):
        m = POOL.search(l)
        if m:
            pos.setdefault("pool:" + m.group(1), []).append(i)
        m = MOVI.search(l)
        if m:
            pend[m.group(1)] = (int(m.group(2), 0), i)
            continue
        m = LSL.search(l)
        if m and m.group(1) in pend:
            v, st = pend.pop(m.group(1))
            pos.setdefault("built:%d" % (v << int(m.group(2), 0)), []).append(st)
            continue
        m = NEG.search(l)
        if m and m.group(2) in pend:
            v, st = pend.pop(m.group(2))
            pos.setdefault("built:%d" % (-v), []).append(st)
            if m.group(1) != m.group(2):
                pend.pop(m.group(1), None)
            continue
        m = WRITE.search(l)
        if m:
            pend.pop(m.group(1), None)
    return pos


POOLDATA = re.compile(r"^\s*\.(?:align|word|byte|hword|short|pool|ltorg|4byte)\b")


def _site_kind(body, i):
    """Where the value built at line i ends up: "sp", "call", or "other".

    Walk forward to whichever comes first -- a stack-slot store, a `bl`, or a
    real label. THE THREE-WAY ANSWER MATTERS. An early version returned a
    boolean, "reaches a bl" against "everything else", which swept STRUCT
    OFFSETS into the split bucket: an offset feeding `add` and `ldrsh` reaches
    neither a stack slot nor a call, so it read as the split shape and sent a
    screen looking for a local that did not exist.

    A MID-FUNCTION LITERAL POOL IS NOT A LABEL FOR THIS PURPOSE. gcc dumps a
    pool in the middle of a function and branches over it whenever a load's
    pool_range is short -- an HImode constant's is 64 bytes -- and the pool
    carries its own `.L` label. Treating that as a terminator stopped the walk
    before it ever reached the `bl`, so EVERY BRANCH-OVER-POOL FUNCTION READ AS
    [offset] WHEN IT WAS [cse]. OvlFunc_932_200a804 is the specimen: classified
    [offset], actually a guard/set pair that CSE_CFLAGS closed. Pool directives
    and the labels that introduce them are therefore skipped.
    """
    n = len(body)
    j = i
    while j < min(i + 12, n):
        l = body[j]
        t = l.strip()
        if POOLDATA.match(l):
            j += 1
            continue
        if LABEL.match(l):
            # a label is a real basic-block boundary only if what follows is
            # code; if it introduces pool data, skip the whole run
            k = j + 1
            while k < n and (not body[k].strip() or POOLDATA.match(body[k])):
                if POOLDATA.match(body[k]):
                    break
                k += 1
            if k < n and POOLDATA.match(body[k]):
                j = k
                continue
            return "other"
        if re.search(r"\bstr\s+r\d+,\s*\[sp", t):
            return "sp"
        if CALL.search(t):
            return "call"
        j += 1
    return "other"


def duplicate_class(body):
    """None if no value repeats, else one of "block", "cse", "split".

    "block" -- some value repeats inside one straight-line run. The hard class
    the filter was built to reject: CSE if the uses are close, PRE hoisting if
    one dominates the other, and neither yields to spelling.

    Everything else has a label between the repeats, and batches 182-183 split
    that case in two. WHAT IS REPEATED decides which:

    "split" -- a repeated small immediate feeding a STACK-ARGUMENT slot. gcc
        does not re-materialise a value it kept live across a branch, so this is
        TWO source variables and splitting the local closes it
        (OvlFunc_941_2008210, 18 differing to exact).

    "cse"   -- a repeated POOLED id consumed as a register argument by a `bl`.
        This is one source literal that gcc's rerun-CSE pass commons, and the
        source cannot reach it: constant propagation folds any name back to the
        same const_int. Four unrelated spellings of OvlFunc_920_2008304's flag
        id all measured identically. The answer is a CSE_CFLAGS rule in the
        Makefile, not a source change. NOTE it is a hint and not a verdict:
        OvlFunc_932_200a934 was offered as [cse] and matched with no flag,
        because its two sites are in mutually exclusive arms and rerun-CSE does
        not common across those. The flag is for the guard/set shape, where one
        use DOMINATES the other. Ask whether either site can reach the other.

    "offset" -- a repeated shiftable constant that reaches neither a stack slot
        nor a call, i.e. a STRUCT OFFSET feeding `add` or a `ldrsh` index. DO
        NOT SPLIT THESE. gcc-2.96 Thumb builds a shiftable constant with
        `mov`/`lsl` from a bare literal already, so plain literal offsets on a
        named base reproduce the ROM's per-block rebuild; giving each block its
        own offset local makes two locals holding one constant, which folds to a
        single const_int kept live across the branch. OvlFunc_939_2009668 went
        33 to 38 that way.

    The three look identical without this test -- all are "a constant built more
    than once with a label in between" -- and each fix is inert or harmful on
    the other two.
    """
    pos = constant_sites(body)
    reps = {k: sorted(v) for k, v in pos.items() if len(v) > 1}
    if not reps:
        return None
    labels = [i for i, l in enumerate(body) if LABEL.match(l)]
    kinds = set()
    for key, idxs in reps.items():
        if not any(any(idxs[j] < L < idxs[j + 1] for L in labels)
                   for j in range(len(idxs) - 1)):
            return "block"
        sites = [_site_kind(body, i) for i in idxs]
        if all(k == "call" for k in sites):
            kinds.add("cse")
        elif any(k == "sp" for k in sites):
            kinds.add("split")
        else:
            kinds.add("offset")
    # a function showing several gets the most actionable label, and "split" is
    # the only one that is a source change we can make directly
    for k in ("split", "cse", "offset"):
        if k in kinds:
            return k
    return None


def expensive_constants(body):
    """Pooled values, plus mov+lsl builds, as a list of value keys."""
    vals = []
    pend = {}
    for l in body:
        m = POOL.search(l)
        if m:
            vals.append("pool:" + m.group(1))
        m = MOVI.search(l)
        if m:
            pend[m.group(1)] = int(m.group(2), 0)
            continue
        m = LSL.search(l)
        if m and m.group(1) in pend:
            vals.append("built:%d" % (pend.pop(m.group(1)) << int(m.group(2), 0)))
            continue
        # `mov`+`neg` is a split two-instruction build exactly as `mov`+`lsl`
        # is; docs/elevation.md says so under the argument-order lever, and
        # OvlFunc_974_2008b10 was offered as a candidate and parked on a
        # repeated -0x64 because this branch was missing.
        m = NEG.search(l)
        if m and m.group(2) in pend:
            vals.append("built:%d" % (-pend.pop(m.group(2))))
            if m.group(1) != m.group(2):
                pend.pop(m.group(1), None)
            continue
        m = WRITE.search(l)
        if m:
            pend.pop(m.group(1), None)
    return vals


def passes(body):
    # labels are kept here and stripped for the counts, because duplicate_class
    # needs to know where the joins are
    lines = [l for l in body if l.strip() and not l.strip().startswith(("@", "/*"))]
    ins = [l for l in lines if not l.strip().startswith(".")]
    n = len(ins)
    if not (40 <= n <= 120):
        return None
    if any(HIGH.search(l) for l in ins):
        return None
    calls = sum(1 for l in ins if CALL.search(l))
    if calls < 8:
        return None
    dup = duplicate_class(lines)
    if dup == "block":
        return None
    # not a reject -- a warning column; see the module docstring
    cond = sum(1 for l in ins
               if re.search(r"\bb(?:eq|ne|ge|gt|le|lt|hi|ls|cs|cc|mi|pl)\b", l))
    return n, calls, cond, dup


def wide(body, path=None):
    """A ranking calibrated on what this project has ACTUALLY matched.

    The filter above was written for one hard class and its thresholds were
    never checked against the corpus. Measured over the 3,474 compiler-output
    .s files in the tree:

        85% of matched functions make FEWER than 8 calls
        77% of matched functions fall OUTSIDE the 40-120 instruction band
        median matched size is 21 instructions -- below the 40 floor
        only 8% use r8-r11

    So `calls >= 8` would have rejected five sixths of this project's own
    successes, and the size band nearly as many. Those criteria describe the
    functions the filter's author was working on at the time, not the ones that
    yield.

    This mode keeps only the checks that survive contact with the record:

      * hand-written assembly is excluded (unreachable from C at any effort)
      * ARM functions are excluded by the caller (no ARM build path exists)
      * a same-block repeated expensive constant is excluded (proved
        unreachable: within one basic block a constant >= 256 always loses to a
        pseudo in cse.c's cost model)
      * everything else is ranked by SIZE, smallest first, because that is what
        the corpus says converges

    r8-r11 and the call count are reported, not rejected.
    """
    if path is not None and hand_written(path):
        return None
    lines = [l for l in body if l.strip() and not l.strip().startswith(("@", "/*"))]
    ins = [l for l in lines if not l.strip().startswith(".")]
    n = len(ins)
    if n < 4:
        return None
    dup = duplicate_class(lines)
    if dup == "block":
        return None
    calls = sum(1 for l in ins if CALL.search(l))
    high = any(HIGH.search(l) for l in ins)
    cond = sum(1 for l in ins
               if re.search(r"\bb(?:eq|ne|ge|gt|le|lt|hi|ls|cs|cc|mi|pl)\b", l))
    return n, calls, cond, dup, high


if __name__ == "__main__":
    parked = pickable.parked()
    if "--wide" in sys.argv:
        rows = []
        for s in glob.glob("asm/**/*.s", recursive=True):
            if os.path.exists(s[:-2].replace("asm/", "src/", 1) + ".c"):
                continue
            for name, body in shapesib.functions(s):
                if name in parked:
                    continue
                r = wide(body, s)
                if r:
                    rows.append((r[0], r[1], r[2], r[3], r[4], name, s))
        # sort on the numeric fields only -- dup is None or a string, and a
        # bare tuple sort reaches it on ties
        rows.sort(key=lambda r: (r[0], r[1], r[2]))
        lim = 30
        for a in sys.argv:
            if a.startswith("--top="):
                lim = int(a.split("=", 1)[1])
        print("%d candidates, smallest first (calibrated on the corpus)\n"
              % len(rows))
        for n, calls, cond, dup, high, name, s in rows[:lim]:
            print("  %4di %2dc %2dbr %-30s %s%s%s"
                  % (n, calls, cond, name, s,
                     "  [%s]" % dup if dup else "",
                     "  [r8-r11]" if high else ""))
        raise SystemExit(0)
    rows = []
    for s in glob.glob("asm/**/*.s", recursive=True):
        if os.path.exists(s[:-2].replace("asm/", "src/", 1) + ".c"):
            continue
        for name, body in shapesib.functions(s):
            if name in parked:
                continue
            if hand_written(s):
                continue
            if name in arm_functions(s):
                continue
            r = passes(body)
            if r:
                rows.append((r[1], r[0], r[2], name, s, bool(shapesib.kin(s)), r[3]))
    rows.sort(key=lambda r: (-r[0], r[1]))
    tally = " ".join("%d [%s]" % (sum(1 for r in rows if r[6] == k), k)
                     for k in ("split", "cse", "offset")
                     if any(r[6] == k for r in rows))
    print("%d candidates pass the filter (%s)\n" % (len(rows), tally))
    for calls, n, cond, name, s, haskin, dup in rows[:25]:
        print("  %2d calls %3di %2dbr %-28s %s%s%s%s"
              % (calls, n, cond, name, s,
                 "  [kin]" if haskin else "",
                 ("  [%s]" % dup) if dup else "",
                 "  <- NO GUARD" if cond == 0 else ""))
