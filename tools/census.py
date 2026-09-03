#!/usr/bin/env python3
"""census.py -- how many functions are left, by size and by status.

    python3 tools/census.py            # the table
    python3 tools/census.py --list 1 20   # name the AVAILABLE ones in a band

WHY THIS IS A TOOL AND NOT A ONE-LINER. This count was got wrong three times in
one day, each time by a fresh throwaway script, and each time the number was
plausible enough to publish. The three failures are all encoded below as tests
the code must not regress:

1. COUNT FUNCTIONS BY `.thumb_func_start`, NOT BY `.func_end`. Hand-written .s
   files do not reliably close their functions -- asm/rom_9000/rom_92b8.s has 9
   starts and 6 ends. A parser that emits a function only when it sees an end
   silently drops the rest, which undercounted hand-written assembly as 59 when
   it is 76.

2. A PARK IS NOT ONE FILE PER FUNCTION. Three shapes break the obvious mapping:
   CLASS parks (arg_interleave_flat.c, tiny_reg_order.c) cover many functions at
   once; some parks are named for a different address than their subject
   (Func_80f0008's neighbourhood is parked in 20095d4.c); and some are named for
   a source-file stem (rom_79c30.c). Matching park FILENAMES to addresses found
   464 parked functions when the true figure is 659.

3. MATCH THE NAME AS A SUBSTRING. An overlay park cites its callees with the
   thunk prefix, `__Func_808b868`, and `\bFunc_` does not match that -- `_` is a
   word character, so there is no boundary before `F`. A word-boundary regex
   also misses every park whose subject has a real name (CanRemoveItem,
   UpdateScreenShake) rather than an address.

   Substring matching can in principle over-count, by treating a park's citation
   of some OTHER unsolved function as if that function were parked. Measured
   against a hand check of the 1-20 band, it does not: it predicted 1 available
   and the hand check found exactly 1. Prefer it, and re-run the hand check
   below if you change anything here.

VERIFY BEFORE QUOTING. `--list` prints the available functions in a band; for a
small band, grep each name in src/non_matching/ by hand. If any listed function
turns up there, this file has a bug -- fix it here rather than in a new script.
"""
import os, re, sys, glob

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from filtered import hand_written

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
START = re.compile(r"^\s*\.(?:thumb_func_start(?:_noalign)?|arm_func_start)\s+(\S+)")
END = re.compile(r"^\s*\.func_end\b")
DIRECTIVE = re.compile(r"^\s*\.")
LABEL = re.compile(r"^\s*\S+:")
BUCKETS = [(1, 20), (21, 40), (41, 60), (61, 100),
           (101, 200), (201, 400), (401, 800), (801, 10 ** 9)]


def n_insn(lines):
    n = 0
    for l in lines:
        s = l.strip()
        if not s or s.startswith(("@", "/*", "*")):
            continue
        if DIRECTIVE.match(l) or LABEL.match(l):
            continue
        n += 1
    return n


def survey():
    """[(insns, name, hand_written, parked)] for every function still in asm/."""
    parks = "\n".join(open(p, errors="ignore").read()
                      for p in glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                                         recursive=True))
    rows = []
    for root, _, files in os.walk(os.path.join(ROOT, "asm")):
        for fn in sorted(files):
            if not fn.endswith(".s"):
                continue
            path = os.path.join(root, fn)
            lines = open(path, errors="ignore").readlines()
            # a .s sitting beside a solved .c is compiler OUTPUT, not a target
            if any(".gcc2_compiled." in l for l in lines[:40]):
                continue
            hw = hand_written(path)
            starts = [(i, m.group(1)) for i, l in enumerate(lines)
                      if (m := START.match(l))]
            for k, (i, name) in enumerate(starts):
                stop = starts[k + 1][0] if k + 1 < len(starts) else len(lines)
                for j in range(i + 1, stop):
                    if END.match(lines[j]):
                        stop = j
                        break
                rows.append((n_insn(lines[i + 1:stop]), name, hw, name in parks))
    return rows


def main():
    rows = survey()
    if "--list" in sys.argv:
        k = sys.argv.index("--list")
        lo, hi = int(sys.argv[k + 1]), int(sys.argv[k + 2])
        sel = sorted(r for r in rows if lo <= r[0] <= hi and not r[2] and not r[3])
        for c, name, _, _ in sel:
            print(f"{c:4d}  {name}")
        print(f"\n{len(sel)} available in {lo}-{hi}")
        return
    lab = lambda a, b: f"{a}-{b}" if b < 10 ** 9 else "800+"
    print(f"{'size':>9} | {'total':>6} | {'hand-asm':>8} | {'parked':>6} | {'AVAILABLE':>9}")
    print("-" * 9 + "-+-" + "-" * 6 + "-+-" + "-" * 8 + "-+-" + "-" * 6 + "-+-" + "-" * 9)
    t = [0, 0, 0, 0]
    for a, b in BUCKETS:
        s = [r for r in rows if a <= r[0] <= b]
        hw = sum(1 for r in s if r[2])
        pk = sum(1 for r in s if r[3] and not r[2])
        av = len(s) - hw - pk
        t[0] += len(s); t[1] += hw; t[2] += pk; t[3] += av
        print(f"{lab(a, b):>9} | {len(s):6d} | {hw:8d} | {pk:6d} | {av:9d}")
    print("-" * 9 + "-+-" + "-" * 6 + "-+-" + "-" * 8 + "-+-" + "-" * 6 + "-+-" + "-" * 9)
    print(f"{'TOTAL':>9} | {t[0]:6d} | {t[1]:8d} | {t[2]:6d} | {t[3]:9d}")
    solved = (len(glob.glob(os.path.join(ROOT, "src/**/*.c"), recursive=True))
              - len(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"), recursive=True)))
    print(f"\nmatched .c files in src/ (excl. parks): {solved}")


if __name__ == "__main__":
    main()
