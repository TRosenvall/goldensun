#!/usr/bin/env python3
"""split_s.py -- cut one function out of a multi-function .s so it can be elevated.

A .o is built from ONE source file, so a function living inside a
multi-function .s cannot be converted on its own. The established shape of this
tree (visible in the _a/_b/_c suffixes all over stage1.ld) is to split the .s
three ways -- everything before the target, the target, everything after -- and
list all three in the linker script where the original was. Order is preserved,
so the ROM layout does not move.

    python3 tools/split_s.py asm/rom_8a000/rom_925e0_a_a_c.s Func_8092848

leaves the target in <stem>_b.s ready to be replaced by src/<...>_b.c, with
<stem>_a.s and <stem>_c.s holding the rest. Empty parts are not written, and
the linker script is rewritten to match -- which is why the tree contains
things like rom_23178_a_a_c_b.o with no _a_a_c_a.o beside it.

Byte-neutral by construction: the same lines in the same order, only
distributed across files. `make compare` after running it should still be
green BEFORE any .c is written; check that first, because a layout mistake and
a bad decompilation look identical at the end.
"""
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
START = re.compile(r"\s*\.(?:thumb_func_start(?:_noalign)?|arm_func_start)\s+(\S+)")


def parse(path):
    """(preamble, [(name, [lines]), ...]).

    A block runs from the top of a function's annotation comment to the line
    before the next function's -- so the prose stays attached to the function
    it describes, and any data emitted after a function (its pool, its .ssize)
    stays with it rather than drifting to the next file.
    """
    L = open(path, errors="replace").read().split("\n")
    starts = [(i, m.group(1)) for i, l in enumerate(L) for m in [START.match(l)] if m]
    if not starts:
        sys.exit("no functions in " + path)

    bounds = []
    for i, name in starts:
        j = i
        while j > 0 and (L[j - 1].lstrip().startswith("@") or not L[j - 1].strip()):
            j -= 1
        bounds.append((j, name))

    preamble = L[:bounds[0][0]]
    blocks = []
    for k, (j, name) in enumerate(bounds):
        end = bounds[k + 1][0] if k + 1 < len(bounds) else len(L)
        blocks.append((name, L[j:end]))
    return preamble, blocks


def rewrite_ld(stem_rel, parts):
    """Replace the single .o reference with the parts that were written.

    Every section the original was listed under is handled, not just .text --
    a file with a .rodata line as well would silently lose its data otherwise.
    """
    old = stem_rel[:-2] + ".o"
    touched = []
    for ld in glob.glob(os.path.join(ROOT, "*.ld")) + \
            glob.glob(os.path.join(ROOT, "overlays", "*", "*.ld")):
        L = open(ld).read().split("\n")
        out, hit = [], False
        for line in L:
            m = re.match(r"^(\s*)" + re.escape(old) + r"\((\.\w+)\)(.*)$", line)
            if not m:
                out.append(line)
                continue
            hit = True
            indent, section, tail = m.groups()
            for p in parts:
                out.append(f"{indent}{p[:-2]}.o({section}){tail}")
        if hit:
            open(ld, "w").write("\n".join(out))
            touched.append(os.path.relpath(ld, ROOT))
    return touched


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    rel = os.path.relpath(os.path.abspath(sys.argv[1]), ROOT)
    target = sys.argv[2]
    path = os.path.join(ROOT, rel)

    preamble, blocks = parse(path)
    names = [n for n, _ in blocks]
    if target not in names:
        sys.exit(f"{target} not in {rel}; it has: {', '.join(names)}")
    k = names.index(target)
    if len(blocks) == 1:
        sys.exit(f"{rel} holds only {target}; convert it directly, no split needed")

    stem = rel[:-2]
    groups = [("_a", blocks[:k]), ("_b", [blocks[k]]), ("_c", blocks[k + 1:])]
    written = []
    for suffix, group in groups:
        if not group:
            continue
        body = []
        for _, lines in group:
            body.extend(lines)
        # trailing blank lines are noise; one newline at EOF
        while body and not body[-1].strip():
            body.pop()
        out = stem + suffix + ".s"
        open(os.path.join(ROOT, out), "w").write(
            "\n".join(preamble + body) + "\n")
        written.append(out)

    os.remove(path)
    touched = rewrite_ld(rel, written)

    print(f"{rel}  ->  {', '.join(written)}")
    print(f"linker scripts updated: {', '.join(touched) or 'NONE (check this)'}")
    print(f"\nnow: verify `make compare` is still green, THEN write "
          f"src/{stem[4:]}_b.c and delete {stem}_b.s")


if __name__ == "__main__":
    main()
