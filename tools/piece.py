#!/usr/bin/env python3
"""piece.py -- the FULL membership of a translation unit, with each member's state.

    python3 tools/piece.py src/non_matching/rom_c9000/rom_e3a3c.c
    python3 tools/piece.py asm/rom_c9000/rom_e3958_c_c_c_c_a.s
    python3 tools/piece.py --pairs        # every .s holding 2+ parked functions

WHY THIS EXISTS.  One `.c` is one TU, so a piece converts WHOLE OR NOT AT ALL,
which makes "how many of this piece's functions are still parked" the first
question before assigning any multi-park target.  The coordinator got it wrong
FOUR TIMES in batches 334-336, always the same way:

    a pool was built by grouping SHORTLIST parks (low figure, pin-free) that
    share a reference .s, and the piece was then described as having only those
    members.  A third parked member outside the filter -- at 488, at 653 -- was
    invisible, and two briefs spent part of a round on a pair that could not
    ship whatever they did.

Two parks have also been found naming an `.s` that NO LONGER EXISTS, because a
later split consumed it.  So the membership question cannot be answered from a
park header either; it has to come from the `.s` on disk.

## WHAT IT PRINTS

Every `.thumb_func_start` in the piece, and for each one whether it is LANDED
(a sibling `.c` exists beside the `.s`) or PARKED (a `src/non_matching/**` file
names it as its subject), with the park's current claim where there is one.

Then the verdict that matters:

  * **CONVERTS WHOLE** -- every member is parked, so one candidate holding all
    of them lands the lot.  This is the two-for-one (or three-for-one) shape.
  * **NEEDS A SPLIT** -- some members are landed, or some parked members are out
    of reach, so `split_s.py` has to cut the target out first.  `split_s.py`
    cuts before/target/after, so a member at either END of the stem splits out
    alone -- which is why "neither ships alone" is usually FALSE and was stated
    wrongly twice.
"""
import glob
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
START = re.compile(r"^\s*\.(?:thumb|arm)_func_start(?:_noalign)?\s+(\S+)", re.M)
CLAIM = re.compile(r"(\d+)\s+(?:differing\s+)?encodings?\s+of\s+(\d+)", re.I)
CLAIM2 = re.compile(r"(\d+)\s+of\s+(\d+)\s+encodings", re.I)


def park_index():
    """{function name: park path} from every non_matching .c, by SUBJECT not filename.

    Parks are address-named and landings are split-named, so a filename match
    finds every landing and no park -- the defect `upstream_module.py` documents.
    The subject is the first function this file DEFINES, or the name its header
    leads with.
    """
    idx = {}
    for p in glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"), recursive=True):
        try:
            t = open(p, errors="replace").read()
        except OSError:
            continue
        rel = os.path.relpath(p, ROOT)
        # STRIP COMMENTS FIRST.  The first draft of this index matched any
        # capitalised identifier followed by `(` anywhere in the file, so a park
        # claimed to define every function it merely CALLS or mentions in prose
        # -- and reported a three-function piece as all-parked-by-one-file.
        # Third tool in this project with that bug; see docs/elevation.md.
        code = re.sub(r"/\*.*?\*/", " ", t, flags=re.S)
        code = re.sub(r"//[^\n]*", " ", code)
        # A DEFINITION starts at column 0, has a return type, and its parameter
        # list is followed by `{` -- not by `;`, which is a declaration.
        for m in re.finditer(
                r"^[A-Za-z_][A-Za-z0-9_ \t\*]*?\b(\w+)\s*\([^;{]*?\)\s*\{",
                code, re.M | re.S):
            idx.setdefault(m.group(1), rel)
    return idx


def claim_of(path):
    try:
        t = open(os.path.join(ROOT, path), errors="replace").read(9000)
    except OSError:
        return None
    m = CLAIM.search(t) or CLAIM2.search(t)
    return f"{m.group(1)} of {m.group(2)}" if m else None


def resolve_s(arg):
    """Accept a park/landed .c or the .s itself; return the .s on disk."""
    if arg.endswith(".s"):
        return arg
    t = open(os.path.join(ROOT, arg), errors="replace").read(9000)
    for m in re.finditer(r"(asm/[A-Za-z0-9_/]+\.s)", t):
        if os.path.exists(os.path.join(ROOT, m.group(1))):
            return m.group(1)
    # a landed .c sits beside its own generated .s
    cand = "asm/" + arg[4:-2] + ".s" if arg.startswith("src/") else None
    if cand and os.path.exists(os.path.join(ROOT, cand)):
        return cand
    return None


def report(s_rel, parks):
    body = open(os.path.join(ROOT, s_rel), errors="replace").read()
    members = START.findall(body)
    print(f"{s_rel}  --  {len(members)} function(s)")
    landed_c = "src/" + s_rel[4:-2] + ".c"
    whole_landed = os.path.exists(os.path.join(ROOT, landed_c))
    nparked = 0
    for m in members:
        p = parks.get(m)
        if p:
            nparked += 1
            c = claim_of(p)
            print(f"    PARKED  {m:<28} {c or '(no claim)':<14} {p}")
        elif whole_landed:
            print(f"    LANDED  {m:<28} {'':<14} {landed_c}")
        else:
            print(f"    ??      {m:<28} {'':<14} no park found and no sibling .c")
    if nparked == len(members) and members:
        figs = []
        for m in members:
            c = claim_of(parks[m]) if parks.get(m) else None
            figs.append(int(c.split()[0]) if c else None)
        worst = max((f for f in figs if f is not None), default=None)
        print(f"  => all {len(members)} parked, so ONE candidate could land them all")
        if worst is not None and worst > 60:
            print(f"     BUT the worst member is at {worst}, so converting whole means"
                  f" closing that too.")
            print(f"     A SPLIT is the practical route: cut a reachable member out and"
                  f" ship it now rather")
            print(f"     than waiting for the whole piece. 'Converts whole' is about"
                  f" the TU, not the work.")
    else:
        print(f"  => NEEDS A SPLIT: {nparked} of {len(members)} parked")
        if members:
            print(f"     split_s.py cuts before/target/after, so {members[0]} and "
                  f"{members[-1]} each split out ALONE")
    return nparked, len(members)


def main():
    parks = park_index()
    if "--pairs" in sys.argv:
        hits = []
        for s in glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True):
            rel = os.path.relpath(s, ROOT)
            if os.path.exists(os.path.join(ROOT, "src/" + rel[4:-2] + ".c")):
                continue
            body = open(s, errors="replace").read()
            ms = START.findall(body)
            n = sum(1 for m in ms if m in parks)
            if n >= 2:
                hits.append((n, len(ms), rel))
        print(f"{len(hits)} pieces hold two or more PARKED functions "
              f"(of which {sum(1 for n,t,_ in hits if n==t)} convert WHOLE)\n")
        for n, t, rel in sorted(hits, key=lambda x: (-(x[0] == x[1]), x[1])):
            tag = "WHOLE" if n == t else "split"
            print(f"  {n}/{t} parked  [{tag}]  {rel}")
        return 0
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    if not args:
        sys.stderr.write("give a park .c, a landed .c, or an .s -- or --pairs\n")
        return 2
    for a in args:
        s = resolve_s(a)
        if not s:
            print(f"{a}: could not resolve a reference .s that exists on disk")
            continue
        report(s, parks)
    return 0


if __name__ == "__main__":
    sys.exit(main())
