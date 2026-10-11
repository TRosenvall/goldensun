#!/usr/bin/env python3
"""symrefs.py -- who actually references each `.sym` symbol?

    python3 tools/symrefs.py                 # every symbol, reference counts
    python3 tools/symrefs.py _CONST_1f       # just these
    python3 tools/symrefs.py --dead          # only the unreferenced ones

WHY THIS EXISTS.  A `.sym` entry is a CLAIM about the original source -- that it
named a symbol where we would otherwise write a literal -- and the owner admits
one only on structural evidence.  So "this entry no longer has a beneficiary" is
a retirement argument, and it has now been made WRONGLY TWICE:

  * batch 333 brief D: "_CONST_1f now has no beneficiary anywhere in the tree".
    It is referenced by the LANDED src/rom_15000/rom_23178_a_a_a_a_c_c_a_c_b.c
    and by a park.
  * batch 335 brief C: "_CONST_200 is no longer referenced anywhere".
    It is referenced by src/non_matching/rom_a1000/80a9aec.c.

Both briefs had closed their OWN function's use of the symbol and generalised
from one bank to the tree.  The coordinator relayed both without checking.

## THE TRAP THIS TOOL ITSELF FELL INTO FIRST

A plain `grep -r _CONST_200 src/` reports a hit in **park-header prose** --
including the very sentence claiming the symbol is unreferenced.  The first
draft of this check therefore reported ZERO unreferenced symbols out of 448,
which looked like a clean bill of health and was actually a tautology.

Comments are stripped before matching: C block and line comments, and in `.s`
both `@` comments and `/* */` continuation lines.  **That is the whole
difficulty of this check** -- the same defect `docs/elevation.md` records under
prose that looks like machinery, now on its third sighting in a measuring tool.

## WHAT COUNTS AS A REFERENCE

A use in code, in a landed `.c`, a park `.c`, an `include/` header, or a
hand-written `.s`.  A park counts: a park is work in progress and retiring a
symbol out from under one would silently change that park's meaning.
"""
import glob
import os
import re
import sys
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SYMDEF = re.compile(r"^(_[A-Za-z0-9_]+)\s*=")
IDENT = re.compile(r"_[A-Za-z0-9_]+")


def strip_c(t):
    t = re.sub(r"/\*.*?\*/", " ", t, flags=re.S)
    return re.sub(r"//[^\n]*", " ", t)


def strip_s(t):
    out = []
    for l in t.split("\n"):
        l = re.sub(r"@.*$", "", l)
        s = l.strip()
        if s.startswith(("/*", "*", "//")):
            continue
        out.append(l)
    return "\n".join(out)


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    dead_only = "--dead" in sys.argv

    syms = {}
    for f in sorted(glob.glob(os.path.join(ROOT, "*.sym"))):
        for l in open(f, errors="replace"):
            m = SYMDEF.match(l)
            if m:
                syms[m.group(1)] = os.path.basename(f)

    refs = defaultdict(list)
    for pat, strip in (("src/**/*.c", strip_c), ("include/**/*.h", strip_c),
                       ("asm/**/*.s", strip_s)):
        for f in glob.glob(os.path.join(ROOT, pat), recursive=True):
            try:
                body = strip(open(f, errors="replace").read())
            except OSError:
                continue
            rel = os.path.relpath(f, ROOT)
            for name in set(IDENT.findall(body)):
                if name in syms:
                    refs[name].append(rel)

    names = args or sorted(syms)
    unknown = [n for n in names if n not in syms]
    for n in unknown:
        print(f"  ?? {n} is not defined in any .sym")
    names = [n for n in names if n in syms]

    dead = [n for n in names if not refs[n]]
    if dead_only:
        print(f"{len(dead)} of {len(names)} symbols are UNREFERENCED in code:")
        for n in dead:
            print(f"  {n:<24} ({syms[n]})")
        return 1 if dead else 0

    for n in names:
        rs = refs[n]
        landed = [r for r in rs if r.startswith("src/") and "non_matching" not in r]
        parks = [r for r in rs if "non_matching" in r]
        asms = [r for r in rs if r.startswith("asm/")]
        hdrs = [r for r in rs if r.startswith("include/")]
        if not rs:
            print(f"  {n:<24} ({syms[n]})  *** UNREFERENCED")
            continue
        bits = []
        if landed: bits.append(f"{len(landed)} landed")
        if parks: bits.append(f"{len(parks)} park")
        if hdrs: bits.append(f"{len(hdrs)} header")
        if asms: bits.append(f"{len(asms)} asm")
        print(f"  {n:<24} ({syms[n]})  {', '.join(bits)}")
        if len(names) <= 8:
            for r in sorted(landed + parks + hdrs)[:6]:
                print(f"        {r}")
    if not args and not dead_only:
        print(f"\n{len(names)} symbols, {len(dead)} unreferenced")
    return 0


if __name__ == "__main__":
    sys.exit(main())
