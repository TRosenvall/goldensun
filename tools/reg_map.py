#!/usr/bin/env python3
"""reg_map.py -- is the register divergence a CONSISTENT permutation?

Register allocation is the terminal blocker: once a function's semantics are
right, what is left is which register each value lives in. gcc-2.96 allocates
from REG_ALLOC_ORDER (arm.h:989) = {3, 2, 1, 0, 12, 14, 4, 5, 6, 7, ...} --
caller-saved first -- and the ROM reaches for r4-r6 sooner. If that difference
is a fixed permutation, it is a rule we can exploit. If it is not, it is not one
blocker but many.

METHOD

For every parked .c whose screen comes out the SAME LENGTH as the ROM, compile
it, align the two streams instruction by instruction, and keep only the pairs
whose mnemonic and shape already agree. Every differing register in such a pair
is a correspondence: "the ROM put here what we put there". Collect them per
function.

  consistent : every ROM register maps to exactly one of ours and vice versa --
               a genuine permutation, and the function differs ONLY in naming
  conflicting: some ROM register maps to two different ones of ours -- the
               streams agree instruction for instruction but the VALUES are
               laid out differently, which is not a renaming and will not
               respond to a renaming fix

The distinction matters more than the counts. A consistent permutation says the
allocator started somewhere else in the same order. A conflicting map says the
two compilers made different decisions about what to keep alive.

    python3 tools/reg_map.py
"""
import collections
import glob
import os
import re
import subprocess
import sys

ROOT = "/work" if os.path.isdir("/work/asm") else \
    os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = re.compile(r"Source asm:\s*goldensun/(\S+\.s)")
ALT = re.compile(r"asm/\S+\.s")
REG = re.compile(r"\br(\d{1,2})\b")


def shape(ins):
    """The instruction with register numbers blanked -- its mnemonic and form."""
    return REG.sub("r?", ins)


def main():
    consistent, conflicting, other = [], [], 0
    perms = collections.Counter()
    pairs = collections.Counter()
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        txt = open(p, errors="replace").read()
        m = SRC.search(txt) or ALT.search(txt)
        if not m:
            continue
        ref = m.group(1) if m.re is SRC else m.group(0)
        if not os.path.exists(os.path.join(ROOT, ref)):
            continue
        rel = os.path.relpath(p, ROOT)
        r = subprocess.run([sys.executable, "tools/tryc.py", rel, "--ref", ref,
                            "--full"], capture_output=True, text=True, cwd=ROOT)
        out = r.stdout + r.stderr
        h = re.search(r"XX (\S+)\s+\(rom (\d+) lines, ours (\d+)", out)
        if not h or h.group(2) != h.group(3):
            continue
        mapping = collections.defaultdict(set)
        rev = collections.defaultdict(set)
        diffs = 0
        for line in out.split("\n"):
            d = re.match(r"^\s+(?:->)?\s*rom (.*?)\s{2,}ours (.*)$", line)
            if not d:
                continue
            a, b = d.group(1).strip(), d.group(2).strip()
            if a == b or shape(a) != shape(b):
                continue
            ra, rb = REG.findall(a), REG.findall(b)
            if len(ra) != len(rb):
                continue
            diffs += 1
            for x, y in zip(ra, rb):
                if x != y:
                    mapping[x].add(y)
                    rev[y].add(x)
        if not mapping:
            other += 1
            continue
        ok = all(len(v) == 1 for v in mapping.values()) and \
             all(len(v) == 1 for v in rev.values())
        flat = {k: next(iter(v)) for k, v in mapping.items()}
        entry = (h.group(1), rel, diffs, flat)
        if ok:
            consistent.append(entry)
            perms[tuple(sorted(f"r{k}->r{v}" for k, v in flat.items()))] += 1
            for k, v in flat.items():
                pairs[(f"r{k}", f"r{v}")] += 1
        else:
            conflicting.append(entry)

    print(f"same-length parks with register differences: "
          f"{len(consistent) + len(conflicting)}"
          f"   (consistent {len(consistent)}, conflicting {len(conflicting)})\n")
    print("=== CONSISTENT permutations -- the function differs only in naming ===")
    for fn, rel, d, flat in sorted(consistent, key=lambda e: e[2]):
        pm = " ".join(f"r{k}->r{v}" for k, v in sorted(flat.items(), key=lambda x: int(x[0])))
        print(f"  {d:3d} lines  {fn:26s} {pm}")
    if not consistent:
        print("  (none)")
    print(f"\n=== CONFLICTING -- same instructions, different value layout ({len(conflicting)}) ===")
    for fn, rel, d, flat in sorted(conflicting, key=lambda e: e[2])[:12]:
        print(f"  {d:3d} lines  {fn:26s} {rel}")
    print("\n=== which ROM register do we most often get wrong, and as what? ===")
    for (a, b), n in pairs.most_common(14):
        print(f"  rom {a} -> ours {b}   {n}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
