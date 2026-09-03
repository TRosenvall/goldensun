#!/usr/bin/env python3
"""neighbour.py -- find the solved function that most resembles a target.

Reading a solved neighbour before writing anything is the single highest-yield
step in this project: it produced three ZERO-ITERATION byte matches in batch 187
alone, and a 26-of-123 opening screen in batch 185. This tool does the search
that was being done by hand.

THE REFINEMENT THAT MATTERS, and why this greps what it greps: CALLEE-SET
IDENTITY BEATS FILENAME ADJACENCY. On Func_80a3e28 the stem-sibling was LITERALLY
THE FUNCTION THE TARGET TAIL-CALLS, and it was less useful than a cross-bank file
that called BOTH of the target's callees over the same global -- because the
sibling walked the same array to CLEAR slots rather than to make the call. On
Func_8078af8 the same thing happened again: the literal neighbouring split of the
target's own parent, which also calls one of the two helpers, was worth nothing,
while a file in a different bank supplied the whole skeleton.

So the ranking is by SHARED CALLEES AND GLOBALS, and the filename is not
consulted at all. A stem-sibling that genuinely shares callees will rank on that
basis like anything else.

    python3 tools/neighbour.py Func_80a3e28
    python3 tools/neighbour.py Func_80a3e28 --top=10

Names are compared after stripping the overlay thunk prefix, so an overlay's
`__MapActor_GetActor` matches a main-ROM file's `MapActor_GetActor`. That is
deliberate: the two are the same routine and the solved main-ROM file is just as
useful a model.
"""
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import shapesib

CALL = re.compile(r"\bbl\s+([A-Za-z_.][\w.]*)")
POOL = re.compile(r"\bldr\s+r\d+,\s*=([A-Za-z_.][\w.]*)")
# helpers every function calls; they carry no information about what a function
# does, so counting them would rank the whole corpus equally
BORING = {
    "__divsi3", "__udivsi3", "__modsi3", "__umodsi3", "__aeabi_idiv",
    "_divsi3_RAM", "_udivsi3_RAM", "_modsi3_RAM", "_umodsi3_RAM",
    "_call_via_r0", "_call_via_r1", "_call_via_r2", "_call_via_r3",
    "_call_via_r4", "_call_via_r5", "_call_via_r6", "_call_via_r7",
    "memcpy", "memset",
}


def canon(name):
    """Strip the overlay thunk prefix so overlay and main-ROM names compare."""
    n = name.lstrip(".")
    while n.startswith("__"):
        n = n[2:]
    return n.lstrip("_")


def symbols_of(body):
    """The callees and pooled symbols a function body names."""
    out = set()
    for line in body:
        for m in CALL.finditer(line):
            out.add(m.group(1))
        for m in POOL.finditer(line):
            out.add(m.group(1))
    return {canon(s) for s in out
            if canon(s) and s not in BORING and canon(s) not in BORING}


def find_target(name):
    for s in glob.glob("asm/**/*.s", recursive=True):
        for fn, body in shapesib.functions(s):
            if fn == name:
                return s, body
    return None, None


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    if not args:
        print(__doc__)
        return 2
    name = args[0]
    top = 8
    for a in sys.argv[1:]:
        if a.startswith("--top="):
            top = int(a.split("=", 1)[1])

    path, body = find_target(name)
    if body is None:
        print("no such function in asm/: %s" % name)
        return 1
    want = symbols_of(body)
    if not want:
        print("%s names no callees or pooled symbols -- nothing to match on."
              % name)
        print("For a self-contained function the neighbour search cannot help;")
        print("fall back to tools/filtered.py --wide and read the body.")
        return 0

    print("%s  (%s)" % (name, path))
    print("names %d symbol(s): %s\n" % (len(want), ", ".join(sorted(want))))

    rows = []
    for c in glob.glob("src/**/*.c", recursive=True):
        if "non_matching" in c:
            continue
        try:
            with open(c, errors="ignore") as f:
                text = f.read()
        except OSError:
            continue
        hits = {w for w in want if re.search(r"\b_*%s\b" % re.escape(w), text)}
        if hits:
            rows.append((len(hits), sorted(hits), c))
    if not rows:
        print("no solved file names any of them.")
        return 0
    rows.sort(key=lambda r: (-r[0], r[2]))
    for n, hits, c in rows[:top]:
        print("  %d/%d  %s" % (n, len(want), c))
        print("        %s" % ", ".join(hits))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
