#!/usr/bin/env python3
"""Find an ALREADY-MATCHING function whose shape resembles a candidate's.

Transferring a matching sibling's idiom has been the single highest-yield move
in recent batches -- OvlFunc_881_20080d4 went from 26 differing to 1 that way,
and HeightTile_6 came out at the exact length on its first attempt because
HeightTile_A's levers were applied before writing.

Ranking candidates by how many matching .c files sit in the same DIRECTORY was
tried in batch 159 and is a bad proxy: it measures how well-worked the bank is,
not whether any of those functions share a shape.  This ranks by SHAPE.

Each function is reduced to its mnemonic sequence with operands dropped, then
compared by difflib ratio.  Registers, constants and symbols are deliberately
discarded -- what transfers between siblings is the C idiom (a walking pointer,
a signed division, a switch chain), and that survives in the mnemonics while
the operands are exactly what differs.
"""
import difflib, glob, os, re, sys

MNEM = re.compile(r"^\t(\w+)")


def shape(lines):
    return [m.group(1) for l in lines if (m := MNEM.match(l))]


# Two formats, and missing the second gave a corpus of ZERO on the first run:
#   hand-written asm  ->  `.thumb_func_start NAME`
#   gcc OUTPUT        ->  `.thumb_func` / `.type NAME,function` / `NAME:`
# Elevated functions are only present in the second form, which is exactly the
# corpus this tool exists to search.
TYPEDECL = re.compile(r"^\s*\.type\s+(\S+?),\s*function")


def functions(path):
    L = open(path).read().split("\n")
    idx = []
    for i, l in enumerate(L):
        if ".thumb_func_start" in l:
            idx.append((i, l.split()[1]))
        elif (m := TYPEDECL.match(l)):
            idx.append((i, m.group(1)))
    for k, (i0, name) in enumerate(idx):
        i1 = idx[k + 1][0] if k + 1 < len(idx) else len(L)
        yield name, L[i0:i1]


def matched_corpus():
    """Functions whose .s has a .c beside it -- i.e. already elevated."""
    out = []
    for s in glob.glob("asm/**/*.s", recursive=True):
        c = s.replace("asm/", "src/", 1).replace(".s", ".c")
        if not os.path.exists(c):
            continue
        for name, body in functions(s):
            sh = shape(body)
            if 20 <= len(sh) <= 140:
                out.append((name, c, sh))
    return out


def best(cand_shape, corpus, k=3):
    scored = []
    for name, c, sh in corpus:
        if abs(len(sh) - len(cand_shape)) > max(12, len(cand_shape) // 3):
            continue
        r = difflib.SequenceMatcher(None, cand_shape, sh).ratio()
        scored.append((r, name, c))
    scored.sort(reverse=True)
    return scored[:k]


if __name__ == "__main__":
    target = sys.argv[1]
    corpus = matched_corpus()
    print(f"{len(corpus)} already-matching functions in the corpus\n")
    for s in glob.glob("asm/**/*.s", recursive=True):
        for name, body in functions(s):
            if name != target:
                continue
            for r, n, c in best(shape(body), corpus):
                print(f"  {r:.3f}  {n:24s} {c}")
            sys.exit(0)
    print("candidate not found")


def parent_stem(path, levels=1):
    """Strip `levels` split suffixes to reach an ancestor filename stem.

    split_s.py names halves by appending _a/_b/_c, so `ovl_30_c_c_c_a_c_a_a.s`
    and `ovl_30_c_c_c_a_c_a_b.c` are the two children of
    `ovl_30_c_c_c_a_c_a`.  Stripping ONE suffix finds true siblings; stripping
    to exhaustion just yields the bank (`ovl_30`), which is the directory proxy
    batch 159 measured as useless.
    """
    stem = os.path.basename(path).rsplit(".", 1)[0]
    for _ in range(levels):
        if len(stem) > 2 and stem[-2] == "_" and stem[-1].isalpha():
            stem = stem[:-2]
    return os.path.dirname(path), stem


def kin(path, levels=1):
    """Already-matching .c files cut from the same parent .s as `path`.

    Ranks ABOVE shape similarity.  Batch 160: OvlFunc_916_2008b3c's best shape
    match scored 0.683 -- below the ~0.7 threshold where a sibling stops being a
    usable template -- but the function beside it in the same file shared the
    record struct exactly, and taking that struct made the first screen 2 of 43.
    Shape similarity compares CONTROL FLOW, which is what neighbouring functions
    differ in; what they share is the DATA they walk.
    """
    d, stem = parent_stem(path, levels)
    d = d.replace("asm/", "src/", 1)
    return [c for c in glob.glob(d + "/*.c")
            if "non_matching" not in c and parent_stem(c, levels)[1] == stem]
