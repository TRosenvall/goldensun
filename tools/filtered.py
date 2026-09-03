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


def _consumed_as_call_arg(body, i):
    """True if the value built at line i reaches a `bl` as a register argument
    without being stored to the stack first.

    Cheap and deliberately local: walk forward to the next `bl` and stop at any
    label. A pooled flag id loaded into r0 and passed straight to a call is the
    CSE_CFLAGS shape; a constant that lands in a stack slot is the split shape.
    """
    for l in body[i:i + 12]:
        t = l.strip()
        if LABEL.match(l):
            return False
        if re.search(r"\bstr\s+r\d+,\s*\[sp", t):
            return False
        if re.search(r"\bbl\s+", t):
            return True
    return False


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
        Makefile, not a source change.

    The two look identical without this test -- both are "a constant built more
    than once with a label in between" -- and two independent screens reached
    the same refinement on the same day, which is why it lives in code now
    rather than in the notebook.
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
        kinds.add("cse" if all(_consumed_as_call_arg(body, i) for i in idxs)
                  else "split")
    # a function showing both gets the more actionable label: the split is a
    # source change we can make, the CSE flag is a build-rule change we can make
    # too, and doing the source one first is free either way
    return "split" if "split" in kinds else "cse"


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
    calls = sum(1 for l in ins if re.search(r"\bbl\s+", l))
    if calls < 8:
        return None
    dup = duplicate_class(lines)
    if dup == "block":
        return None
    # not a reject -- a warning column; see the module docstring
    cond = sum(1 for l in ins
               if re.search(r"\bb(?:eq|ne|ge|gt|le|lt|hi|ls|cs|cc|mi|pl)\b", l))
    return n, calls, cond, dup


if __name__ == "__main__":
    parked = pickable.parked()
    rows = []
    for s in glob.glob("asm/**/*.s", recursive=True):
        if os.path.exists(s[:-2].replace("asm/", "src/", 1) + ".c"):
            continue
        for name, body in shapesib.functions(s):
            if name in parked:
                continue
            r = passes(body)
            if r:
                rows.append((r[1], r[0], r[2], name, s, bool(shapesib.kin(s)), r[3]))
    rows.sort(key=lambda r: (-r[0], r[1]))
    nsplit = sum(1 for r in rows if r[6] == "split")
    ncse = sum(1 for r in rows if r[6] == "cse")
    print("%d candidates pass the filter (%d [split], %d [cse])\n"
          % (len(rows), nsplit, ncse))
    for calls, n, cond, name, s, haskin, dup in rows[:25]:
        print("  %2d calls %3di %2dbr %-28s %s%s%s%s"
              % (calls, n, cond, name, s,
                 "  [kin]" if haskin else "",
                 ("  [%s]" % dup) if dup else "",
                 "  <- NO GUARD" if cond == 0 else ""))
