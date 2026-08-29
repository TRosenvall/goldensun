#!/usr/bin/env python3
"""script_candidates.py -- rank straight-line CALL SCRIPTS by whether the
constant-CSE blocker can fire on them.

WHY

Functions with many `bl`s and no labels are the easiest class left: the C is a
transcription and tools/draft_script.py writes the first draft.  85 of them
remain in the 35-60 instruction band alone.

But two of the first two I tried failed on the SAME thing, and it was
predictable from the assembly before writing a line of C.  When a script uses
the same constant at two call sites, gcc-2.96 hoists it into a callee-saved
register and reloads it with a `mov`, where the ROM rebuilds it at both sites.
The function is straight line, so there is no control-flow boundary, and the
documented rule says that combination is unreachable.  Measured on
OvlFunc_882_200bc48 (one repeat) and OvlFunc_881_2009c08 (two repeats):

    -fno-rerun-cse-after-loop, -fno-gcse, -fno-expensive-optimizations,
    -fno-cse-follow-jumps, -fno-force-mem, -fno-thread-jumps, -O1,
    and spelling the constants as const.sym symbols

all leave the instruction COUNT wrong.  So a repeat is not a hint to try
harder, it is a reason to pick a different function.

This ranks the class by repeat count.  Zero repeats first -- those are the ones
worth a screen.

    python3 tools/script_candidates.py [--lo 35] [--hi 60] [--min-calls 6]
"""
import argparse
import os
import re

START = re.compile(r"^\.thumb_func_start(?:_noalign)? (\S+)")
END = re.compile(r"^\.func_end")
LABEL = re.compile(r"^\.L\w+:")
MOVI = re.compile(r"^\tmov\t(r\d+), #(0x[0-9a-f]+|\d+)$")
LSL = re.compile(r"^\tlsl\t(r\d+), #(0x[0-9a-f]+|\d+)$")
NEG = re.compile(r"^\tneg\t(r\d+), (r\d+)$")
LDRE = re.compile(r"^\tldr\t(r\d+), =(0x[0-9a-f]+|\d+)$")
ANYMOV = re.compile(r"^\tmov\t(r[0-3]), #")
ANYEXP = re.compile(r"^\t(?:lsl\t(r[0-3]), #|ldr\t(r[0-3]), =)")
BL = re.compile(r"^\tbl\t")


def r0_before_expensive(lines):
    """Count call sites where the ROM finishes a CHEAP argument before an
    EXPENSIVE one -- `mov rA, #imm` followed by `lsl rB, #n` or `ldr rB, =V`
    for a DIFFERENT argument register, with no `bl` between.

    gcc-2.96 does the opposite: it completes the expensive argument first and
    emits the cheap `mov` last.  In a function with a block that DOMINATES the
    call the basic-block lever fixes it.  In a straight-line function nothing
    does -- measured on OvlFunc_945_200bd10 (13 of 77) and OvlFunc_883_2009490
    (6 of 70), both parked, after plain literals, the return-type lever, no
    prototype at all, and hoisting the constants to top-of-function locals
    (which is much WORSE in one block: 77 lines becomes 97).

    MEASURED NEGATIVE -- this does NOT predict the outcome, and it is reported
    as an advisory column only, never folded into the ranking:

        OvlFunc_945_200bd10   0 labels  score 5   PARKED (13 of 77)
        OvlFunc_883_2009490   0 labels  score 3   PARKED (6 of 70)
        OvlFunc_881_200b130   0 labels  score 3   MATCHED
        OvlFunc_950_2008328   0 labels  score 1   MATCHED

    A matched function and a parked one both score 3.  Folding this into the
    "clean" score would have hidden OvlFunc_881_200b130, which matched on the
    second screen via the return-type lever.  What the count IS good for is
    knowing, before you start, that you will probably need that lever.
    """
    n = 0
    pend = None
    for ln in lines:
        if BL.match(ln):
            pend = None
            continue
        m = ANYMOV.match(ln)
        if m:
            pend = m.group(1)
            continue
        m = ANYEXP.match(ln)
        if m and pend:
            reg = m.group(1) or m.group(2)
            if reg != pend:
                n += 1
                pend = None
    return n


def constants(lines):
    """Every EXPENSIVE constant the function materialises, duplicates kept.

    Only constants that cost more than one instruction are counted: a pool
    load (`ldr rN, =V`) and a shifted build (`mov`+`lsl`).  A bare
    `mov rN, #imm8` is NOT counted -- gcc rematerialises those for free, which
    is why every script in the corpus can pass 0 and 1 at a dozen call sites
    without trouble.  Counting them reported 40 of 41 functions as blocked,
    which is the opposite of useful.
    """
    out = []
    pend = {}
    for ln in lines:
        m = MOVI.match(ln)
        if m:
            pend[m.group(1)] = int(m.group(2), 0)
            continue
        m = LSL.match(ln)
        if m and m.group(1) in pend:
            out.append(pend.pop(m.group(1)) << int(m.group(2), 0))
            continue
        m = NEG.match(ln)
        if m and m.group(2) in pend:
            # `mov rN, #K` + `neg rN, rN` is a two-instruction build of -K, and
            # gcc commons it exactly like a shifted build or a pool load.  It is
            # also the shape gcc will NOT reproduce: 0 of the generated .s files
            # in the tree contain two consecutive `neg rN, rN`, so a call taking
            # two negative constants is unreachable rather than merely unreached.
            out.append(-pend.pop(m.group(2)))
            continue
        m = LDRE.match(ln)
        if m:
            out.append(int(m.group(2), 0))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--lo", type=int, default=35)
    ap.add_argument("--hi", type=int, default=60)
    ap.add_argument("--min-calls", type=int, default=6)
    ap.add_argument("--max-labels", type=int, default=0)
    a = ap.parse_args()

    parked = set()
    for root, _, files in os.walk("src/non_matching"):
        for f in files:
            if f.endswith(".c"):
                parked.add(f[:-2])

    rows = []
    for root, _, files in os.walk("asm"):
        for fn in files:
            if not fn.endswith(".s"):
                continue
            path = os.path.join(root, fn)
            cur, buf = None, []
            for line in open(path, errors="ignore"):
                line = line.rstrip("\n")
                m = START.match(line)
                if m:
                    cur, buf = m.group(1), []
                    continue
                if cur is None:
                    continue
                if END.match(line):
                    rows.append((cur, path, buf))
                    cur = None
                else:
                    buf.append(line)

    out = []
    for name, path, buf in rows:
        if any(name.endswith(p) for p in parked):
            continue
        insns = sum(1 for l in buf if l.startswith("\t") and not l.startswith("\t."))
        calls = sum(1 for l in buf if l.startswith("\tbl\t"))
        labels = sum(1 for l in buf if LABEL.match(l))
        if not (a.lo <= insns <= a.hi and calls >= a.min_calls
                and labels <= a.max_labels):
            continue
        cs = constants(buf)
        repeats = len(cs) - len(set(cs))
        r0first = r0_before_expensive(buf) if labels == 0 else 0
        out.append((repeats, -calls, insns, name, path, repeats, r0first))

    out.sort()
    clean = sum(1 for r in out if r[0] == 0)
    print(f"{len(out)} straight-line scripts in band {a.lo}-{a.hi}; "
          f"{clean} with NO repeated constant\n")
    for score, negcalls, insns, name, path, repeats, r0first in out[:40]:
        tag = "clean" if repeats == 0 else f"{repeats} repeat"
        note = f"  [r0-first x{r0first}]" if r0first else ""
        print(f"  {insns:>3}i {-negcalls:>2}calls  {tag:<9} {name:<26} {path}{note}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
