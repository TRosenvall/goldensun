#!/usr/bin/env python3
"""twin_families.py -- group the REMAINING functions by identical opcode stream.

WHY

Selecting candidates one at a time runs into diminishing returns: the easy
shapes get taken and what is left is register-allocation floors.  A family --
several functions whose instruction sequence is identical apart from immediates
and symbol names -- is different, because one solved C file becomes a template
and the rest are a search-and-replace.

A four-member DMA family was elevated this way in one round, and one of its
members had been parked for several batches at "21 lines against 22" with the
answer sitting in its three siblings.

The key is that this groups only UNELEVATED functions against each other.  An
earlier attempt compared them against already-elevated ones and returned zero,
because generated .s files use gcc's own `.thumb_func` directive rather than
the repo's `.thumb_func_start` macro -- the scan saw no elevated bodies at all.
If you extend this, check the positive control first.

    python3 tools/twin_families.py [--lo 12] [--hi 120] [--min 2]
"""
import argparse
import os
import re
from collections import defaultdict

START = re.compile(r"^\.thumb_func_start(?:_noalign)? (\S+)")
END = re.compile(r"^\.func_end")


def shape(buf):
    """The opcode sequence, with every operand discarded.

    Two functions with the same shape differ only in immediates, register
    numbers and symbol names -- exactly what a template substitution fixes.
    """
    return tuple(l.strip().split()[0] for l in buf
                 if l.startswith("\t") and not l.startswith("\t."))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--lo", type=int, default=12)
    ap.add_argument("--hi", type=int, default=120)
    ap.add_argument("--min", type=int, default=2)
    a = ap.parse_args()

    parked = set()
    for root, _, files in os.walk("src/non_matching"):
        for f in files:
            if f.endswith(".c"):
                parked.add(f[:-2])

    fams = defaultdict(list)
    for root, _, files in os.walk("asm"):
        for fn in files:
            if not fn.endswith(".s"):
                continue
            p = os.path.join(root, fn)
            cur, buf = None, []
            for line in open(p, errors="ignore"):
                line = line.rstrip("\n")
                m = START.match(line)
                if m:
                    if cur:
                        fams[shape(buf)].append((cur, p))
                    cur, buf = m.group(1), []
                    continue
                if cur is None:
                    continue
                if END.match(line):
                    fams[shape(buf)].append((cur, p))
                    cur = None
                    continue
                buf.append(line)
            if cur:
                fams[shape(buf)].append((cur, p))

    out = [(len(v), len(k), v) for k, v in fams.items()
           if len(v) >= a.min and a.lo <= len(k) <= a.hi]
    out.sort(reverse=True)
    covered = sum(c for c, _, _ in out)
    print(f"{len(out)} families of {a.min}+ in band {a.lo}-{a.hi}, "
          f"covering {covered} functions\n")
    for cnt, n, v in out[:25]:
        free = [x for x in v if not any(x[0].endswith(q) for q in parked)]
        print(f"  {cnt} members  {n:>3} insns  {len(free)} unparked")
        for nm, p in v:
            tag = "" if not any(nm.endswith(q) for q in parked) else "  [PARKED]"
            print(f"      {nm:<26} {p}{tag}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
