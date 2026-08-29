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
  branch-over-pool CERTAIN. The function BRANCHES OVER its own literal pool and
                   old_agbcc only emits one at .func_end. This file DEFERS to
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
                   rule held on every function tested, but the test here is
                   crude and OVER-counts. MEASURED: of the 3495 functions whose
                   C ALREADY MATCHES, this filter would flag 526 -- 15.1%. So
                   this line is an UPPER BOUND with a known false-positive
                   rate, not a count of unreachable functions, and it should
                   never be quoted as "N functions cannot be matched".
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
NEG = re.compile(r"\tneg\t(r\d+), (r\d+)")


def precompute(body):
    for m in BLOCK.finditer(body):
        lines = [l for l in m.group(1).split("\n") if l]
        if not any(l.startswith("\tlsl\t") or l.startswith("\tneg\t") or "=" in l
                   for l in lines):
            continue
        for l in lines[:-1]:
            if re.match(r"\tmov\tr\d+, #", l):
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
