#!/usr/bin/env python3
"""templated.py -- rank UNSOLVED functions by how well a SOLVED file matches them.

    python3 tools/templated.py [--top=N]

NOT the same question as pickable.py, which rejects functions whose SHAPE hits a
known blocker. This asks the complementary one: which remaining functions
already have a worked example to copy from?

WHY. Targets were being chosen by SIZE, smallest first, on the theory that small
converges. Two consecutive rounds went 0-for-3 that way. With ~3,520 functions
already matched, the size-ordered head of the list is now uniformly structural
-- allocation-order disagreements and scheduling residues -- because the ones
that fall to a spelling are largely gone.

Meanwhile the heuristic that HAS worked, seven rounds running, is callee-set
identity: neighbour.py finds a solved file sharing a target's callees and
globals, and that file hands over the struct layout, the argument order and the
idiom. But neighbour.py was only ever run AFTER a target was chosen. This
inverts it -- score every candidate by its best available neighbour FIRST, and
work the ones that already have a template.

score = |shared symbols| / |target's symbols|, so 1.00 means every callee and
global this function touches already appears together in one solved file.
Division helpers and call veneers are excluded via neighbour.py's BORING set,
because sharing __divsi3 says nothing about body shape.

SIZE IS REPORTED BUT NOT RANKED ON. A 90-instruction function with a perfect
template is a better bet than a 50-instruction one with none -- that is the
whole premise, and if it stops holding this file should be struck rather than
tuned.
"""
import os
import re
import sys
import glob

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from neighbour import symbols_of
from filtered import hand_written, arm_functions

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
START = re.compile(r"^\s*\.(thumb|arm)_func_start(?:_noalign)?\s+(\S+)")
END = re.compile(r"^\s*\.func_end\b")


def solved_sets():
    """[(path, {symbols})] for each solved .c, read from its generated .s."""
    out = []
    for c in glob.glob(os.path.join(ROOT, "src/**/*.c"), recursive=True):
        if "non_matching" in c:
            continue
        s = c.replace("/src/", "/asm/", 1)[:-2] + ".s"
        if not os.path.exists(s):
            continue
        syms = symbols_of(open(s, errors="ignore").read().splitlines())
        if syms:
            out.append((os.path.relpath(c, ROOT), syms))
    return out


def main():
    top = 25
    for a in sys.argv[1:]:
        if a.startswith("--top="):
            top = int(a.split("=", 1)[1])
    solved = solved_sets()
    parks = "\n".join(open(p, errors="ignore").read()
                      for p in glob.glob(os.path.join(ROOT, "src/non_matching/**/*.c"),
                                         recursive=True))
    rows = []
    for s in glob.glob(os.path.join(ROOT, "asm/**/*.s"), recursive=True):
        lines = open(s, errors="ignore").readlines()
        if any(".gcc2_compiled." in l for l in lines[:40]):
            continue
        if hand_written(s):
            continue
        arm = arm_functions(s)
        starts = [(i, m.group(2)) for i, l in enumerate(lines) if (m := START.match(l))]
        for k, (i, name) in enumerate(starts):
            if name in arm or name in parks:
                continue
            stop = starts[k + 1][0] if k + 1 < len(starts) else len(lines)
            for j in range(i + 1, stop):
                if END.match(lines[j]):
                    stop = j
                    break
            body = lines[i + 1:stop]
            n = sum(1 for l in body
                    if l.strip()
                    and not l.strip().startswith((".", "@", "/*", "*"))
                    and ":" not in l.strip().split()[0])
            syms = symbols_of(body)
            if len(syms) < 2 or n < 8:
                continue
            best, bestf = 0.0, None
            for path, ss in solved:
                shared = len(syms & ss)
                if shared:
                    sc = shared / float(len(syms))
                    if sc > best:
                        best, bestf = sc, path
            if bestf:
                rows.append((best, len(syms), n, name, os.path.relpath(s, ROOT), bestf))
    rows.sort(key=lambda r: (-r[0], r[2]))
    print("%d candidates have a solved neighbour\n" % len(rows))
    print("score syms insns  function                      neighbour")
    for sc, ns, n, name, s, f in rows[:top]:
        print("%.2f  %3d  %4d   %-28s %s" % (sc, ns, n, name, f))


if __name__ == "__main__":
    main()
