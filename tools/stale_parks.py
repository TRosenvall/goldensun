#!/usr/bin/env python3
"""stale_parks.py -- find parked files whose functions have since been elevated.

WHY

A park is a record of a blocker. When the blocker is later solved the park has
to go, and it is easy to miss: the function is elevated in one round, its .s
disappears, and the .c under src/non_matching/ stays behind describing a problem
that no longer exists. Nothing in the build notices -- parked files are not
compiled.

That matters more than tidiness. The park census in reports/batch-68.md counted
177 files by blocker class and used the totals to decide what to work on next.
The FIRST run of this script found SIXTEEN fully-stale files, including
overlays/narrow_constant.c, which described what docs/elevation.md called "the
single highest-value problem in the project" -- for functions that were by then
matched.

WHAT IT DOES

For every .c under src/non_matching/, pulls the ROM symbol names out of its
comment header and checks each against the set of `.thumb_func_start` names
still present in asm/. A name that is gone has been elevated.

  fully stale : no named symbol is still in asm -- delete the file
  partly stale: some named symbols are elevated and some are not -- the file
                covers a group and needs editing, not deleting

Run it after any round that elevates something, and before trusting a census.

    python3 tools/stale_parks.py
"""
import glob
import os
import re
import sys

ROOT = "/work" if os.path.isdir("/work/asm") else \
    os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SYM = re.compile(r"\b(OvlFunc_[0-9a-z_]+|Func_[0-9a-f]{4,})\b")


def asm_symbols():
    out = set()
    for p in glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True):
        for line in open(p, errors="replace"):
            if line.startswith(".thumb_func_start"):
                out.add(line.split()[1])
    return out


def main():
    live = asm_symbols()
    full, part, n = [], [], 0
    for p in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                              recursive=True)):
        n += 1
        head = open(p, errors="replace").read().split("*/")[0]
        names = sorted(set(SYM.findall(head)))
        if not names:
            continue
        gone = [s for s in names if s not in live]
        stay = [s for s in names if s in live]
        rel = os.path.relpath(p, ROOT)
        if gone and not stay:
            full.append((rel, gone))
        elif gone and stay:
            part.append((rel, gone, stay))

    # DUPLICATES: two park files describing the SAME function. The stale check
    # cannot see these -- both name a function that is still in asm/ -- but they
    # inflate the park census and split the notes, so the second reader finds
    # half the history. Found by hitting one: Func_80ad69c had two.
    bysym = {}
    for p2 in sorted(glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                               recursive=True)):
        head2 = open(p2, errors="replace").read().split("*/")[0]
        # Only the FIRST symbol -- the file's subject. Matching on every symbol
        # mentioned flags cross-references and the deliberately grouped park
        # files, which is 46 false positives against 2 real ones.
        found = SYM.findall(head2)
        if found:
            bysym.setdefault(found[0], []).append(os.path.relpath(p2, ROOT))
    dupes = {k: v for k, v in bysym.items() if len(v) > 1}

    print(f"screened {n} parked files\n")
    print(f"=== DUPLICATE parks -- one function, several files ({len(dupes)}) ===")
    for k, v in sorted(dupes.items()):
        print(f"  {k}")
        for f in v:
            print(f"      {f}")
    if not dupes:
        print("  (none)")
    print()
    print("=== FULLY STALE -- every named function is elevated; delete ===")
    for rel, gone in full:
        print(f"  {rel}\n      {', '.join(gone[:6])}")
    if not full:
        print("  (none)")
    print(f"\n=== PARTLY STALE -- edit, do not delete ({len(part)}) ===")
    for rel, gone, stay in part:
        print(f"  {rel}\n      elevated: {', '.join(gone[:4])}"
              f"\n      still parked: {', '.join(stay[:4])}")
    if not part:
        print("  (none)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
