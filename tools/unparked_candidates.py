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

Excluded, in order:
  * files holding more than one function   -- these need a split first
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


def main():
    lo = int(sys.argv[sys.argv.index("--min") + 1]) if "--min" in sys.argv else 10
    hi = int(sys.argv[sys.argv.index("--max") + 1]) if "--max" in sys.argv else 45
    parked = parked_names()
    out, skipped = [], 0
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
            if starts[0] in parked:
                skipped += 1
                continue
            n = len([l for l in body.split("\n") if l.startswith("\t")])
            if lo <= n <= hi:
                out.append((n, starts[0], rel))
    out.sort()
    for n, name, p in (out if "--all" in sys.argv else out[:25]):
        print(f"{n:4d}  {name:28s} {p}")
    print(f"\n{len(out)} candidates ({lo}-{hi} insns); "
          f"{skipped} suppressed as already parked")
    return 0


if __name__ == "__main__":
    sys.exit(main())
