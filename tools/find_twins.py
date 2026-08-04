#!/usr/bin/env python3
"""find_twins.py -- group remaining asm functions by identical instruction body.

WHY

OvlFunc_898_2008938 was elevated by taking the .c of a function in a DIFFERENT
overlay, changing the name, and screening it. It matched first try, because the
two bodies are identical instruction for instruction. That was found by
accident, from noticing two candidates looked alike.

Doing it deliberately: 41 groups of duplicates remain, covering 74 functions
that could be elevated by solving ONE member of each group. That is a different
kind of work from picking candidates one at a time, and it is the main reason to
expect the rate to hold up rather than decay as the easy shapes run out.

WHAT COUNTS AS A TWIN

Bodies are compared after renumbering local labels, so two functions that differ
only in which `.L` numbers the disassembler assigned still group together. Any
difference in a register, an immediate, or a callee name makes them distinct --
this finds EXACT duplicates, not merely similar functions.

That is deliberately strict. tools/find_families.py already looks for similar
shapes and its docstring records what a loose substring match cost: nine false
positives, 450-1500 instruction functions grouped together because they happened
to share a few call names, and two splits that had to be reverted.

USING IT

    python3 tools/find_twins.py                 # all groups, largest payoff first
    python3 tools/find_twins.py --min 3         # only groups of 3 or more
    python3 tools/find_twins.py --show <name>   # print one group's members

A group is worth attacking in proportion to (members - 1) x instructions, which
is what it sorts by: solving a 27-instruction body that appears seven times is
worth more than a 139-instruction body that appears twice.

CAVEAT WORTH KNOWING BEFORE SPENDING A ROUND

A group being large says nothing about whether it is MATCHABLE. The seven-member
group headed by Func_809a44c is parked on a scheduling residue that neither
-O1 nor -fno-schedule-insns2 fixes. Check whether the representative is already
in src/non_matching/ before starting.
"""
import collections
import glob
import hashlib
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FUNC = re.compile(r"\.thumb_func_start\s+(\S+)[^\n]*\n(.*?)\.func_end", re.S)
LOCAL = re.compile(r"\.L[0-9a-fA-F]+")


def normalise(body):
    """Instruction text with local labels renumbered in first-appearance order."""
    seen = {}

    def sub(m):
        return seen.setdefault(m.group(0), f".L{len(seen)}")

    return "\n".join(l for l in LOCAL.sub(sub, body).split("\n") if l.strip())


def groups(min_insn=8):
    out = collections.defaultdict(list)
    for p in sorted(glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True)):
        rel = os.path.relpath(p, ROOT)
        for m in FUNC.finditer(open(p, errors="replace").read()):
            body = m.group(2)
            insn = len([l for l in body.split("\n") if l.startswith("\t")])
            if insn < min_insn:
                continue
            key = hashlib.md5(normalise(body).encode()).hexdigest()
            out[key].append((m.group(1), rel, insn))
    return {k: v for k, v in out.items() if len(v) > 1}


def parked(name):
    """True if some parked file names this function -- i.e. it is known hard."""
    for p in glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                       recursive=True):
        if name in open(p, errors="replace").read():
            return True
    return False


def main():
    a = sys.argv[1:]
    if "--show" in a:
        want = a[a.index("--show") + 1]
        for g in groups().values():
            if any(n == want for n, _, _ in g):
                for n, rel, insn in g:
                    print(f"  {n:26s} {insn:4d} insn  {rel}")
                return 0
        print(f"no group contains {want}")
        return 1

    lo = int(a[a.index("--min") + 1]) if "--min" in a else 2
    gs = [g for g in groups().values() if len(g) >= lo]
    gs.sort(key=lambda g: -(len(g) - 1) * g[0][2])
    print(f"{'dup':>4} {'insn':>5} {'payoff':>7}  representative")
    for g in gs:
        n, rel, insn = g[0]
        # check EVERY member, not just the representative -- the seven-member
        # group is headed by an overlay copy while the parked one, Func_809a44c,
        # sits sixth. Testing only the representative reported that group as
        # unexplored when it is the best-documented dead end in the tree.
        hit = next((x for x, _, _ in g if parked(x)), None)
        tag = f"  [PARKED: {hit}]" if hit else ""
        print(f"{len(g):4d} {insn:5d} {(len(g)-1)*insn:7d}  {n}  {rel}{tag}")
    print(f"\n{len(gs)} groups, {sum(len(g) - 1 for g in gs)} functions "
          f"reachable by solving one member each")
    return 0


if __name__ == "__main__":
    sys.exit(main())
