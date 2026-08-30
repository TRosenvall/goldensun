#!/usr/bin/env python3
"""dupfuncs.py -- find remaining functions that are DUPLICATES of each other.

Overlays in this ROM share a lot of code by copying it. Normalising away
function names, .L label numbers and pool operands, 101 of the 2110 remaining
THUMB functions fall into 27 identical groups -- so 74 of them would come free
the moment their representative is solved.

The distribution is very top-heavy. Three groups account for 50 functions:

    x18  OvlFunc_883_20080c4
    x17  OvlFunc_883_200834c
    x15  OvlFunc_883_20088c0

Two of those are already parked, one at SEVEN differing lines of 176.

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \\
        python3 tools/dupfuncs.py

WHAT "DUPLICATE" MEANS HERE. Bodies are compared after replacing every
`Func_`/`OvlFunc_` symbol with a placeholder, every `.L` label with a
placeholder, and every `=operand` pool reference with a placeholder. So two
members of a group differ only in which data tables and callees they name --
`OvlFunc_957_20088c0` and `OvlFunc_964_20088c0` differ in exactly two `ldr =`
operands. One C file per group, with the labels as parameters, matches all of
them.

Groups smaller than 8 instructions are skipped; short leaf functions collide
trivially and are not worth the churn.
"""
import collections
import hashlib
import os
import re
import subprocess
import sys

FSTART = re.compile(r"^\.(thumb|arm)_func_start\s+(\S+)", re.M)


def normalise(body):
    n = re.sub(r"(Ovl)?Func_\w+", "F", body)
    n = re.sub(r"\.L\w+", "L", n)
    n = re.sub(r"=\S+", "=X", n)
    return "\n".join(l for l in n.split("\n") if l.startswith("\t"))


def main():
    files = [f for f in subprocess.run(["git", "ls-files", "asm"],
                                       capture_output=True, text=True).stdout.split()
             if f.endswith(".s") and "/m4a" not in f and os.path.exists(f)
             and not os.path.exists("src/" + f[len("asm/"):-2] + ".c")]
    groups = collections.defaultdict(list)
    total = 0
    for f in files:
        t = open(f, errors="ignore").read()
        starts = [(m.start(), m.group(1), m.group(2)) for m in FSTART.finditer(t)]
        for i, (off, kind, name) in enumerate(starts):
            if kind != "thumb":
                continue
            end = starts[i + 1][0] if i + 1 < len(starts) else len(t)
            body = normalise(t[off:end])
            total += 1
            if body.count("\n") < 8:
                continue
            groups[hashlib.md5(body.encode()).hexdigest()].append((name, f))

    dups = {k: v for k, v in groups.items() if len(v) > 1}
    covered = sum(len(v) for v in dups.values())
    print(f"{total} remaining THUMB functions")
    print(f"{len(dups)} duplicate groups covering {covered} functions; "
          f"{covered - len(dups)} would come free")
    for v in sorted(dups.values(), key=len, reverse=True):
        if len(v) < 2:
            continue
        print(f"\n  x{len(v)}  {v[0][0]}")
        for name, f in v[:4]:
            print(f"        {name:<24} {f}")
        if len(v) > 4:
            print(f"        ... and {len(v) - 4} more")
    return 0


if __name__ == "__main__":
    sys.exit(main())
