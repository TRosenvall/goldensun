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
START = re.compile(r"\s*\.(?:thumb_func_start(?:_noalign)?|arm_func_start)\s+(\S+)",
                   re.IGNORECASE)


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

    # The preamble is copied into EVERY part, so it must contain nothing but
    # includes, comments and blank lines. If a function start goes unrecognised
    # the whole function lands here and gets duplicated across the split, which
    # the assembler accepts and the linker then rejects with `multiple
    # definition of ...` -- pointing at the parts, not at the cause.
    #
    # That is exactly what `.thumb_func_Start` (capital S, in two files) did
    # before the pattern above was made case-insensitive. This check is the
    # backstop for the next spelling nobody anticipated.
    stray = [l for l in preamble
             if l.strip()
             and not l.lstrip().startswith(("@", ".include"))]
    if stray:
        sys.exit(f"REFUSING to split {os.path.relpath(path, ROOT)}: the preamble "
                 f"holds more than includes and comments, and it is copied into "
                 f"every part.\n\n    " + "\n    ".join(x.strip() for x in stray[:6]) +
                 f"\n\nA function start was probably not recognised.")

    blocks = []
    for k, (j, name) in enumerate(bounds):
        end = bounds[k + 1][0] if k + 1 < len(bounds) else len(L)
        blocks.append((name, L[j:end]))
    return preamble, blocks


DEFINES = re.compile(r"^\s*(\.?[A-Za-z_][\w.]*):")
GLOBAL = re.compile(r"^\s*\.globa?l\s+(\S+)")


def cross_references(groups):
    """Local labels one part defines and another part needs.

    A `.L` label is file-local: it does not survive into the object's symbol
    table, so a reference to it from a different .s cannot link. Splitting
    between a function and a data table it shares with its neighbours produces
    exactly that, and the failure surfaces only at the final link, as an
    `undefined reference to .Laebcc` with no indication of which split caused
    it.

    Found the hard way on rom_a5534_c_c_c.s: the target was the LAST function
    in the file, so the trailing data tables came with it into _b, and
    deleting _b.s after writing the .c took two tables that an earlier
    function still referenced.
    """
    defined, used = {}, {}
    for suffix, group in groups:
        d, u, exported = set(), set(), set()
        for _, lines in group:
            for l in lines:
                text = l.split("@")[0]
                m = DEFINES.match(text)
                if m:
                    d.add(m.group(1).lstrip("."))
                for tok in re.findall(r"\.L[\w.]+", text):
                    u.add(tok.lstrip("."))
                m = GLOBAL.match(text)
                if m:
                    exported.add(m.group(1).lstrip("."))
        # Apply the exports LAST. A `.global .Lfoo` directive precedes the
        # `.Lfoo:` definition it exports, so discarding as the lines are read
        # lets the later definition put the symbol straight back and the
        # export is ignored -- which made this refuse a split that was
        # perfectly legal.
        defined[suffix], used[suffix] = d - exported, u - exported

    bad = []
    for a, _ in groups:
        for b, _ in groups:
            if a == b:
                continue
            for sym in sorted(used[a] & defined[b]):
                bad.append((a, b, sym))
    return bad


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
        # "One function" is not the same as "nothing else in the file". A .s can
        # hold one function AND the data tables it selects between, and those
        # are .incbin blobs that C cannot carry into a translation unit. Delete
        # the .s in that case and the data goes with it -- the compile still
        # succeeds, because tryc.py does not link, and the failure surfaces as
        #
        #     undefined reference to `.L6c10'
        #
        # much later. That happened to ovl_e20_c_c_c_c_c_c_c.s, which holds one
        # function and FOURTEEN .incbin tables, on this tool's own advice.
        raw = open(path, errors="replace").read()
        data = len(re.findall(r"^\.L\w+:", raw, re.M))
        # COUNT EVERY BLOB DIRECTIVE, NOT JUST .incbin. This read `.incbin`
        # only, and rom_b6eb4.s carries its five tables as `.incrom` -- so the
        # tool reported "no data", the .s was deleted on its word, and the link
        # died with `undefined reference to '.Lc2a46'`. Batch 101. The same
        # class of blindness as the `.lcomm` one in batch 78.
        blobs = len(re.findall(r"^\s*\.(?:incbin|incrom|incdata)\b", raw, re.M))
        # A DATA SECTION AFTER THE CODE IS ITSELF DISQUALIFYING, whatever
        # directive fills it. Checked separately so a new blob spelling cannot
        # slip past the list above.
        tail = raw[raw.rindex(".func_end"):] if ".func_end" in raw else ""
        has_section = bool(re.search(r"^\s*\.section\b", tail, re.M))
        body_labels = set(re.findall(r"(\.L\w+)", "\n".join(blocks[0][1])))
        defined = set(re.findall(r"^(\.L\w+):", raw, re.M))
        # Labels the function does NOT mention at all are plainly data.
        stranded = sorted(defined - body_labels)
        # ... but a label can be BOTH referenced by the function and defined in
        # the file's data, which is exactly what a jump table's targets look
        # like. Those are data too, and `defined - body_labels` misses them, so
        # count anything defined after the last .func_end as well.
        after = set(re.findall(r"^(\.L\w+):", tail, re.M))
        if blobs or stranded or has_section or after:
            # A CODE/DATA SPLIT IS SAFE WHEN THE DATA IS STRICTLY TRAILING.
            #
            # This used to refuse unconditionally and say "split by hand". That
            # was over-broad: 31 single-function files in the tree carry their
            # data as a clean `.section .data` AFTER `.func_end`, and for those
            # the ordinary split path below already does the right thing -- it
            # cuts at `.func_end`, sends the function to _b and everything
            # after it to _c, and rewrite_ld repoints EVERY section the object
            # was listed under, not just .text. The linker scripts already list
            # (.text) and (.data) on separate lines, so nothing new is needed.
            #
            # What made the blanket refusal look necessary is real, but it is
            # about data BEFORE or INTERLEAVED WITH the code -- that cannot be
            # cut at a single boundary. So the test is not "is there data" but
            # "is any of it ahead of the function".
            #
            # Falling through rather than hand-rolling the split also means the
            # existing safeguards still apply: _b is checked for stray
            # definitions, and cross_references() catches a `.L` label that
            # would cross the new file boundary and tells you which `.global`
            # to add. Those are exactly the failures a hand split invites.
            fe = raw.rindex(".func_end")
            head = raw[:fe]
            ahead = (re.search(r"^\s*\.(?:incbin|incrom|incdata)\b", head, re.M)
                     or re.search(r"^\s*\.section\b", head, re.M)
                     or (set(re.findall(r"^(\.L\w+):", head, re.M)) - body_labels))
            if ahead:
                sys.exit(
                    f"{rel} holds only {target}, but ALSO {blobs} blob(s), "
                    f"{data} label(s), of which {len(stranded)} are not branch targets "
                    f"of the function and {len(after)} are defined after the code.\n"
                    f"At least some of that data sits BEFORE or INSIDE the function, "
                    f"so there is no single boundary to cut at.\n"
                    f"Split the function from its data by hand, or leave it as assembly.")
            print(f"{rel} holds {target} plus {blobs} blob(s) and {len(after)} "
                  f"label(s), ALL of it after the code.\n"
                  f"Splitting code from data: the function goes to _b, the data to _c.\n")
            # fall through to the ordinary split machinery below
        else:
            sys.exit(f"{rel} holds only {target} and no data; convert it directly, "
                     f"no split needed")

    stem = rel[:-2]

    # Cut the target's block at the end of the function itself. Anything after
    # it -- a literal pool, a .rodata section, an .incrom blob -- belongs to
    # the file, not to the function, and must NOT travel into _b: the whole
    # point of _b is that it gets deleted once the .c replaces it.
    #
    # rom_a5534_c_c_c.s is why this exists. The target was the last function
    # in the file, so a trailing .rodata section carrying two .global .incrom
    # blobs came with it, and deleting _b.s destroyed data that
    # rom_a5534_a_b.s references. The build stayed green until the final link,
    # which then reported `undefined reference to .Laebcc` with nothing to
    # connect it back to the split.
    name, lines = blocks[k]
    cut = len(lines)
    for i, l in enumerate(lines):
        if re.match(r"\s*\.(func_end|size)\b", l):
            cut = i + 1
            break
    target, trailing = lines[:cut], lines[cut:]

    groups = [("_a", blocks[:k]),
              ("_b", [(name, target)]),
              ("_c", ([(name + "_data", trailing)] if any(x.strip() for x in trailing)
                      else []) + blocks[k + 1:])]

    # Belt and braces: _b must hold exactly the one function and nothing else.
    body_defs = [l for _, g in [groups[1]] for _, ls in g for l in ls
                 if DEFINES.match(l.split("@")[0])
                 and DEFINES.match(l.split("@")[0]).group(1).lstrip(".") != name
                 and not DEFINES.match(l.split("@")[0]).group(1).startswith(".L")]
    if body_defs:
        print(f"REFUSING to split {rel}: {stem}_b.s would carry definitions "
              f"beyond {name}, which are lost when it becomes a .c:\n")
        for l in body_defs:
            print("   ", l.strip())
        sys.exit(1)

    # And no local label may cross a file boundary, since .L symbols do not
    # survive into the object's symbol table.
    bad = cross_references([(s, g) for s, g in groups if g])
    if bad:
        # Print the COUNT first and the fix last. This list was already
        # accurate when OvlFunc_960_2008e5c was parked for three rounds on a
        # note claiming the file needed "dozens" of exports -- it needed one,
        # and that one line was on screen at the time. A wall of detail with
        # no headline invites an estimate instead of a read.
        syms = sorted({sym for _, _, sym in bad})
        print(f"REFUSING to split {rel}: {len(syms)} local label(s) would "
              f"cross files.\n")
        for a, b, sym in bad:
            print(f"  {stem}{a}.s references .{sym}, defined in {stem}{b}.s")
        print(f"\nA `.L` symbol does not survive into the object's symbol table, "
              f"so the link would fail.\nIf these are data tables the function "
              f"needs, export them in the .s and re-run:\n")
        for sym in syms:
            print(f"    .global .{sym}")
        print(f"\nA .global emits no bytes. Verify `make compare` after the "
              f"export and BEFORE the split,\nso that the two changes stay "
              f"separable.")
        sys.exit(1)

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
