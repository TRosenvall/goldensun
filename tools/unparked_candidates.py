#!/usr/bin/env python3
"""unparked_candidates.py -- small remaining functions that are neither
pool-blocked nor already parked.

WHY THIS EXISTS

A candidate list that does not subtract the parks re-offers work that was
already investigated and written up. That is not a hypothetical: a throwaway
version of this script offered OvlFunc_899_2008428 and OvlFunc_883_2008dc0 as
fresh, both of which had detailed park notes recording the exact attempts that
were then repeated -- and the round overwrote both notes before noticing. One
of them documented a family of SEVEN functions and the mechanism read out of
the gcc sources. tools/pool.py had this same leak once and it cost a day.

So the park subtraction is the point of this tool, not a detail of it.

A park is matched on the FUNCTION NAME appearing anywhere in any file under
src/non_matching/, not on the file's path or its header format. Park headers
use at least five different layouts, and matching the header is what let the
earlier leak through.

With --safe-args, also excluded: functions whose call argument setup matches
the ARGUMENT PRECOMPUTE shape that HANDOFF.md documents as not reachable from
C. gcc-2.96 hoists any argument whose rtx_cost exceeds 2 -- a shifted or
pool-loaded value -- ahead of the register loads, and emits cheap `mov rN, #K`
arguments afterwards so they land LAST. A call whose ROM setup interleaves a
cheap mov among the expensive work, or puts the cheap one first, will misorder
and no source spelling fixes it.

The filter is deliberately crude: it drops any function where a `mov rN, #K`
appears between an `lsl` and the `bl`, or where a `mov` of a small constant
precedes an `lsl` in the same argument block. That over-rejects -- some of
those calls would match -- but a screen costs more than a skipped candidate,
and the rule exists precisely so these are not screened one at a time.

Excluded, in order:
  * files holding more than one function   -- these need a split first
  * files with DATA after the last .func_end -- the .o carries a .rodata or
    .data section other translation units reference, so replacing the .s with
    a .c deletes symbols and the LINK fails, not the compare. Elevating one of
    these needs a hand-split into a text half and a data half first, with both
    listed in every linker-script section the original appeared in.
  * functions with an elevated .c already  -- mirrored src/ path exists
  * functions branching over .pool         -- the 312-function toolchain
                                              ceiling, see poolblocked.py
  * functions named in any park file       -- already investigated

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \\
        python3 tools/unparked_candidates.py [--min N] [--max N] [--all]
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def parked_names():
    """Every identifier appearing anywhere under src/non_matching/.

    Deliberately EVERY identifier, not names matching a Func_/OvlFunc_ shape.
    The first version of this matched `(?:Ovl)?Func_\\w+` and so was blind to
    every function the ROM annotations gave a real name: GetWeaponType was
    parked, with the same strength-reduction analysis, and was offered as a
    fresh candidate anyway -- the round rediscovered the whole finding before
    spotting the park file. DecompressIcon, TextBox and ActorCmd_Loop were in
    the same list and are the same risk.

    Over-matching is the safe direction here. A candidate wrongly suppressed
    costs one function; a park wrongly re-offered costs a round.
    """
    names = set()
    base = os.path.join(ROOT, "src", "non_matching")
    for root, _, fs in os.walk(base):
        for f in fs:
            if not f.endswith(".c"):
                continue
            t = open(os.path.join(root, f), errors="ignore").read()
            names.update(re.findall(r"[A-Za-z_]\w+", t))
    return names


BLOCK = re.compile(r"((?:\t(?:mov|lsl|neg|ldr|add|sub)\t[^\n]*\n)+)\tbl\t")


def precompute_risk(body):
    """True if any call's argument setup shows the precompute shape."""
    for m in BLOCK.finditer(body):
        lines = [l for l in m.group(1).split("\n") if l]
        has_shift = any(l.startswith("\tlsl\t") or l.startswith("\tneg\t")
                        or "=" in l for l in lines)
        if not has_shift:
            continue
        # a cheap immediate mov that is NOT the last setup line
        for i, l in enumerate(lines[:-1]):
            if re.match(r"\tmov\tr\d+, #", l):
                return True
    return False


def main():
    lo = int(sys.argv[sys.argv.index("--min") + 1]) if "--min" in sys.argv else 10
    hi = int(sys.argv[sys.argv.index("--max") + 1]) if "--max" in sys.argv else 45
    parked = parked_names()
    out, skipped, risky = [], 0, 0
    for root, _, fs in os.walk(os.path.join(ROOT, "asm")):
        for f in fs:
            if not f.endswith(".s"):
                continue
            p = os.path.join(root, f)
            t = open(p, errors="ignore").read()
            starts = re.findall(r"\.thumb_func_start (\S+)", t)
            if len(starts) != 1:
                continue
            rel = os.path.relpath(p, ROOT)
            if os.path.exists(os.path.join(ROOT, rel.replace("asm/", "src/", 1)[:-2] + ".c")):
                continue
            body = t[t.index(".thumb_func_start"):]
            if ".pool" in body:
                continue
            # data after the function: .rodata/.data/.incrom that other TUs
            # reference by label. Replacing the .s with a .c drops it and the
            # link fails. Cost a round when rom_1aeec_c_c.s turned out to hold
            # two .incrom blocks after .func_end.
            tail = body[body.rindex(".func_end"):] if ".func_end" in body else ""
            if re.search(r"\.(section|incrom|word|byte|hword|space|align)\b",
                         tail):
                continue
            if starts[0] in parked:
                skipped += 1
                continue
            if "--safe-args" in sys.argv and precompute_risk(body):
                risky += 1
                continue
            n = len([l for l in body.split("\n") if l.startswith("\t")])
            if lo <= n <= hi:
                out.append((n, starts[0], rel))
    out.sort()
    for n, name, p in (out if "--all" in sys.argv else out[:25]):
        print(f"{n:4d}  {name:28s} {p}")
    print(f"\n{len(out)} candidates ({lo}-{hi} insns); "
          f"{skipped} suppressed as already parked"
          + (f"; {risky} suppressed as argument-precompute risk" if "--safe-args" in sys.argv else ""))
    return 0


if __name__ == "__main__":
    sys.exit(main())
