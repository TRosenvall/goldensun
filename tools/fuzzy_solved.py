#!/usr/bin/env python3
"""fuzzy_solved.py -- rank remaining functions by SIMILARITY to a solved one.

WHY THIS EXISTS

tools/match_shapes.py matches a remaining function against the solved corpus by
EXACT skeleton, and tools/solved_twins.py does the same job from the other
side. Both are now exhausted: match_shapes reports 0 leads, and `--near 1`,
`--near 2` and `--near 3` all report 0 as well. That is a real result -- every
remaining function that is a constants-only variant of something already
elevated has been taken.

But "no exact twin" is not "no useful template". Two rounds were lost picking
candidates on size and call count alone and hitting register-allocation walls,
while the rounds that produced elevations came from a solved NEIGHBOUR:
DeleteActor matched on the first screen because its own .s already held the
solved Actor_SetAnimAndSpeed with the same opening. That signal is worth
ranking on directly, and it survives constant and structural drift that an
exact skeleton match does not.

So this ranks every remaining function by the best difflib ratio between its
skeleton and any solved function's, and prints the exemplar to read. A ratio
around 0.8 means "same shape, some statements differ"; 0.95+ is usually a
near-twin that exact matching missed only because of one extra instruction.

    python3 tools/fuzzy_solved.py                 # top leads
    python3 tools/fuzzy_solved.py --min-ratio 0.85
    python3 tools/fuzzy_solved.py --min-insn 30 --max-insn 120

WHAT IT DOES NOT DO

A ratio is a lead, never a proof. The exemplar's .c has to be read before it is
copied -- match_shapes.py's docstring records what a genuinely loose match cost
when that step was skipped. This is deliberately a RANKING, not a filter.

FAKEMATCH EXEMPLARS ARE MARKED. Some elevated files match only because they
force gcc's register allocation with `__asm__ volatile ("" : "+r" (x))` on
every local, and are labelled `// fakematch` at the top. Copying one produces a
match and propagates the hack into a function that may not need it -- this tool
offered exactly that for OvlFunc_959_200a06c, whose target turned out to need
no such thing. Those exemplars now print with a FAKEMATCH tag; read the target
on its own terms before borrowing anything from them.
"""
import collections
import difflib
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import match_shapes
import filtered
import shapesib


def dup_expensive(path, name):
    """Expensive constants the TARGET repeats -- the duplicate-constant hoist.

    Reads the real instruction text, not the skeleton: match_shapes collapses
    every immediate to one letter, so the skeleton cannot see this at all.
    """
    body = None
    for n, b in shapesib.functions(os.path.join(match_shapes.ROOT, path)):
        if n == name:
            body = [l for l in b if l.strip()
                    and not l.strip().startswith((".", "@", "/*"))]
            break
    if body is None:
        return []
    vals = filtered.expensive_constants(body)
    seen, dup = set(), set()
    for v in vals:
        if v in seen:
            dup.add(v)
        seen.add(v)
    return sorted(dup)


def is_fakematch(src):
    """True if the exemplar's .c forces its match with inline-asm register pins."""
    try:
        head = open(os.path.join(match_shapes.ROOT, src), errors="replace").read(4000)
    except OSError:
        return False
    return "fakematch" in head.lower()


def flatten(m):
    out = []
    for skel, entries in m.items():
        lines = skel.split("\n")
        if not lines or lines == [""]:
            continue
        for name, path, n in entries:
            out.append((name, path, n, lines))
    return out


def main():
    a = sys.argv[1:]
    def opt(flag, default):
        return type(default)(a[a.index(flag) + 1]) if flag in a else default
    min_ratio = opt("--min-ratio", 0.80)
    min_insn = opt("--min-insn", 25)
    max_insn = opt("--max-insn", 130)
    top = opt("--top", 25)

    solved_map, unsolved_map = match_shapes.scan()
    solved = flatten(solved_map)
    unsolved = flatten(unsolved_map)
    if not solved or not unsolved:
        sys.exit("one corpus is empty (%d solved, %d unsolved) -- parser broke"
                 % (len(solved), len(unsolved)))

    # Bucket the solved side by length so the quadratic part stays small.
    by_len = collections.defaultdict(list)
    for s in solved:
        by_len[len(s[3])].append(s)

    rows = []
    for name, path, n, lines in unsolved:
        if not (min_insn <= len(lines) <= max_insn):
            continue
        span = max(3, int(len(lines) * 0.20))
        cands = []
        for L in range(len(lines) - span, len(lines) + span + 1):
            cands.extend(by_len.get(L, ()))
        if not cands:
            continue
        # cheap prefilter: mnemonic multiset overlap, keep the best 40
        mine = collections.Counter(l.split(None, 1)[0] for l in lines)
        def overlap(c):
            theirs = collections.Counter(l.split(None, 1)[0] for l in c[3])
            return sum((mine & theirs).values())
        cands.sort(key=overlap, reverse=True)
        best = (0.0, None)
        for c in cands[:40]:
            r = difflib.SequenceMatcher(None, lines, c[3]).ratio()
            if r > best[0]:
                best = (r, c)
        if best[1] and best[0] >= min_ratio:
            rows.append((best[0], name, path, len(lines), best[1]))

    rows.sort(reverse=True)
    print("%d remaining functions at ratio >= %.2f\n" % (len(rows), min_ratio))
    print("ratio insn  function / file")
    print("            exemplar to read")
    for r, name, path, n, ex in rows[:top]:
        tag = "  <- FAKEMATCH, do not copy" if is_fakematch(ex[1]) else ""
        dup = dup_expensive(path, name)
        warn = ("  <- DUP-CONST %s" % ",".join(dup)) if dup else ""
        print(" %.3f %3d  %-28s %s%s" % (r, n, name, path, warn))
        print("            %-28s %s%s" % (ex[0], ex[1], tag))


if __name__ == "__main__":
    main()
