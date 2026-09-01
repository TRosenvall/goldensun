#!/usr/bin/env python3
"""census.py -- classify every REMAINING function by the blocker that will
stop it, using the predictive rules already established, so the shape of the
work left is a measurement rather than an impression.

WHY

By batch 141 the easy end of the corpus is gone: solved_twins.py reports ZERO
remaining functions with a solved twin, and rounds were being spent screening
functions that the documented rules already predict cannot match. This counts
the population each rule covers, once, in priority order.

WHAT THE CLASSES MEAN, and how much to trust each line:

  audio            src/lib/m4a and the rom_f9000 engine. Deliberately left as
                   assembly; not a blocker.
  arm              .arm_func_start rather than .thumb_func_start. 51 functions
                   in 7 files, and they are the performance primitives: the LZ
                   decompressors, the BlitFade family, division and integer
                   sqrt, palette upload, FixupRamCode.

                   TWO REASONS THEY ARE HERE, and they need separating before
                   this line means anything:

                     * No ARM compile path exists. The Makefile has no
                       agbcc_arm or -marm rule, and tools/agbcc/bin/agbcc_arm
                       ships unused. That is fixable plumbing.
                     * SOME OF THEM ARE PROBABLY HAND-WRITTEN and have no C to
                       recover. BlitFade_Div2_ROM builds a mask by self-OR-shift,
                       moves four registers at a time with ldm/stm, and folds a
                       barrel shift into the AND operand; others use rrx, bxmi
                       and post-indexed loads. That is not compiler output. For
                       those, leaving the .s in place is CORRECT rather than a
                       gap -- the same stance this project already takes on the
                       39 m4a audio functions.

                   Nobody has read them one by one to say which is which, so do
                   not read this count as 51 recoverable functions. They also
                   assemble into the ROM today and block nothing.
  branch-over-pool NOT A BLOCKER -- CORRECTED. The function branches over its own
                   literal pool. This was counted as CERTAIN on the premise that
                   the compiler cannot emit a mid-body pool; that premise is
                   about old_agbcc, and gcc-2.96 emits them routinely (see
                   tools/poolblocked.py, which now demonstrates it). These are
                   candidates like any other -- expect to fight over pool
                   placement, which follows the pool's CONTENTS and is therefore
                   reachable from C. The original test is still This file DEFERS to
                   tools/poolblocked.py rather than reimplementing the test,
                   because two looser definitions were tried here and both were
                   wrong:

                     "contains .pool"            518  -- counts pools that sit
                                                        at the end, which is
                                                        exactly what gcc does
                     "data mid-body, code after" 562  -- FALSIFIED: 85 of the
                                                        3494 already-MATCHING
                                                        functions have data
                                                        mid-body, and it is a
                                                        switch JUMP TABLE.

                   gcc emits mid-body data for jump tables and never for
                   literal pools, so only the narrow test means anything.
  precompute       PREDICTED, from HANDOFF.md's argument-precompute rule: a
                   cheap `mov rN, #K` that is not the last setup line before a
                   `bl`, in a block that also builds something expensive. The
                   rule is implemented STRICTLY: two or more argument
                   registers built by an expensive operation, plus a cheap
                   `mov rN, #K` that is not the last setup line.

                   MEASURED against the corpus that already matches: 84 of 3495
                   -- 2.4% false positives. An earlier, looser version of this
                   test (any cheap mov not last, one expensive value enough)
                   flagged 526 of 3495, 15.1%, and inflated this line by about
                   130 functions. The loose version also flagged single-argument
                   calls like `__SetFlag(0xc0 << 2)`, where there is no ordering
                   question at all.

                   Even strictly, 84 matching functions have this shape, so it
                   is not absolute -- see OvlFunc_965_200919c, which passes four
                   arguments with two of them shifted and matches.
  const-remat      PREDICTED. The same built constant is passed in two or more
                   argument registers of one call; the ROM rebuilds it, gcc
                   builds once and copies. Checked BEFORE precompute, because
                   most of these calls also match the precompute shape and
                   would otherwise be invisible -- an earlier ordering reported
                   const-remat as ZERO for exactly that reason.
  data-tail        NOT a codegen blocker. The .s carries data after .func_end
                   that other translation units reference, so it needs a
                   hand-split before it can be elevated at all.
  multi            NOT a codegen blocker either -- the function shares a .s
                   and needs a split first.
  open             Nothing here predicts a blocker. THIS IS THE WORKLIST.

The classes are checked in that order and each function is counted once, so
`open` is a lower bound on what is reachable and every other line is an upper
bound on what is not.
"""
import os
import re
import sys
import collections

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import poolblocked

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FUNC = re.compile(r"\.(?:thumb_func_start(?:_noalign)?|arm_func_start) (\S+)")
BLOCK = re.compile(r"((?:\t(?:mov|lsl|neg|ldr|add|sub)\t[^\n]*\n)+)\tbl\t")
CHEAP = re.compile(r"\tmov\t(r[0-3]), #")
EXP = re.compile(r"\t(?:lsl|neg)\t(r[0-3])|\tldr\t(r[0-3]), =")
NEG = re.compile(r"\tneg\t(r\d+), (r\d+)")


def precompute(body):
    """HANDOFF.md's rule, strictly: TWO OR MORE expensive argument values plus a
    cheap constant that is not last. The "two or more" matters -- a call with a
    single shifted argument is a different and smaller problem, and lumping the
    two together is what inflated this class."""
    for m in BLOCK.finditer(body):
        lines = [l for l in m.group(1).split("\n") if l]
        exp, cheap = set(), []
        for i, l in enumerate(lines):
            e = EXP.search(l)
            if e:
                exp.add(e.group(1) or e.group(2))
            if CHEAP.match(l):
                cheap.append(i)
        if len(exp) >= 2 and cheap and cheap[-1] != len(lines) - 1:
            return True
    return False


def const_remat(body):
    """The same value built into two or more argument registers of one call."""
    for m in BLOCK.finditer(body):
        blk = m.group(1)
        negs = NEG.findall(blk)
        argnegs = [d for d, s in negs if d == s and int(d[1:]) < 4]
        if len(argnegs) >= 2:
            return True
    return False


def main():
    counts = collections.Counter()
    blocked = {n for n, _ in poolblocked.scan()[0]}
    for root, _, fs in os.walk(os.path.join(ROOT, "asm")):
        for f in fs:
            if not f.endswith(".s"):
                continue
            p = os.path.join(root, f)
            rel = os.path.relpath(p, ROOT)
            if os.path.exists(os.path.join(ROOT, rel.replace("asm/", "src/", 1)[:-2] + ".c")):
                continue
            t = open(p, errors="ignore").read()
            starts = list(FUNC.finditer(t))
            if not starts:
                continue
            multi = len(starts) > 1
            for k, m in enumerate(starts):
                end = starts[k + 1].start() if k + 1 < len(starts) else len(t)
                body = t[m.start():end]
                if "rom_f9000" in rel or "/m4a" in rel:
                    counts["audio"] += 1
                elif m.group(0).lstrip().startswith(".arm_func_start"):
                    counts["arm"] += 1
                elif m.group(1) in blocked:
                    counts["branch-over-pool"] += 1
                elif const_remat(body):
                    counts["const-remat"] += 1
                elif precompute(body):
                    counts["precompute"] += 1
                elif not multi and re.search(
                        r"\.(section|incrom|word|byte|hword|space|align)\b",
                        body[body.rindex(".func_end"):] if ".func_end" in body else ""):
                    counts["data-tail"] += 1
                elif multi:
                    counts["multi"] += 1
                else:
                    counts["open"] += 1
    total = sum(counts.values())
    print(f"{total} remaining functions, by the blocker predicted to stop them\n")
    for k, v in counts.most_common():
        print(f"  {v:5d}  {100.0*v/total:5.1f}%  {k}")
    print("\n`open` is the worklist. Everything above it is predicted or")
    print("certain, and `multi` / `data-tail` need a split before anything else.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
