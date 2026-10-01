#!/usr/bin/env python3
"""datacheck.py -- does this .s carry DATA as well as code?

Converting a function deletes its hand-written .s. If that .s also carried a
data section, the data goes with it and every symbol it defined disappears.
NEITHER tools/tryc.py NOR tools/objcmp.py CAN SEE THIS: both compare a single
function, and both report a clean exact match on a candidate that will fail to
link. The linker is what catches it, with an "undefined reference" from a
completely unrelated object.

That is how batch 267 lost .L79b0 and .L79b8 -- 92 bytes of string literals
sitting under the function in asm/rom_c0/rom_56cc_c_c.s.

    python3 tools/datacheck.py asm/rom_c0/rom_56cc_c_c.s
    python3 tools/datacheck.py --all

Run it on the .s BEFORE deleting it. Reading the stem's lines in stage1.ld is
not the check -- the .rodata line sits directly under the .text line and is easy
to read past, which is exactly what happened.

WHEN IT FIRES, the fix is the text/data split: the .c takes an _a stem, the data
moves to an _b.s holding only the data section, and the linker script's two
lines point at them in their original positions. Prefer that to emitting the
blob from C whenever it is more than a handful of readable words -- .incrom
keeps the bytes exact by construction.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from filtered import generated       # noqa: E402  -- the ONE correct "is this gcc's output?" test

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SECTION = re.compile(r"^\s*\.section\s+(\.[\w.]+)", re.M)
FUNC = re.compile(r"^\s*\.(?:thumb|arm)_func_start(?:_noalign)?\s+(\S+)", re.M | re.I)
GLOBAL = re.compile(r"^\s*\.global\s+(\S+)", re.M)
DATA_SECTIONS = (".rodata", ".data", ".bss")
LABEL_DEF = re.compile(r"^(\.L\w+):")
# A data symbol can also be DEFINED BY `.lcomm`, with no `LABEL:` line at all.
# Matching only `LABEL:` under-reported the export set and the recipe it
# printed would FAIL TO LINK: on ovl_30_c_c_c_c_a.s it named .L2054 and
# .L2057 while .L20d0 -- read twice, by the target and by its sibling -- is
# `.lcomm` with no `.global` and was omitted entirely.  The correct export set
# there is THREE symbols.  Every non-global `.lcomm` symbol in the tree was
# exposed to the same miss; 28 files carry the directive.  (Batch 312.)
LCOMM_DEF = re.compile(r"^\s*\.l?comm\s+(\.L\w+)")
LABEL_REF = re.compile(r"\.L\w+")
FUNC_END = re.compile(r"^\s*\.(?:thumb_|arm_)?func_end\b", re.I)
# Asm line comments start with `@`; a label named in prose is not a read.
COMMENT = re.compile(r"@.*")
START = re.compile(r"^\s*\.(?:thumb|arm)_func_start(?:_noalign)?\s+(\S+)", re.I)


def split_requirements(path):
    """Per function: which DATA labels it reads, and which of those lack `.global`.

    WHY THIS IS NOT THE `EXPORTS` LINE ABOVE. That line lists the labels the file
    ALREADY exports, which is not the set a split REQUIRES -- and the two sets can be
    disjoint. Measured in batch 292 on asm/rom_c9000/rom_c91dc_c_c_c_c_c_c.s: EXPORTS
    named `.Leded6` and `.Lededc`, both already global and NEITHER read by
    BaseAnim_SonicWave, while the two labels it does read (`.Ledee8`, `.Ledefc`) are
    not global and are exactly what the split must export. Reporting the first set
    where a reader needs the second is worse than reporting nothing.

    Three files in that one batch needed exports the EXPORTS line did not name (five,
    one and five respectively), and in two other files the assigned function needed
    ZERO exports while a file-mate needed one -- so the answer is per FUNCTION, not per
    file. Getting it wrong costs a failed link with N undefined references, which is
    the same class of failure this tool exists to prevent: batch 285 shipped a rehome
    that exported the two labels its own function read and needed five, because three
    more were read by the function that stayed in assembly.

    A label defined inside the function's own body is a branch target and disappears
    with the conversion; only labels defined in the DATA region count here.
    """
    lines = open(path, errors="ignore").readlines()
    dstart = None
    for i, l in enumerate(lines):
        m = SECTION.match(l)
        if m and any(m.group(1).startswith(d) for d in DATA_SECTIONS):
            dstart = i
            break
    if dstart is None:
        return None
    exported = set(GLOBAL.findall("".join(lines)))
    data_labels = {m.group(1) for l in lines[dstart:] if (m := LABEL_DEF.match(l))}
    data_labels |= {m.group(1) for l in lines[dstart:] if (m := LCOMM_DEF.match(l))}
    starts = [(i, m.group(1)) for i, l in enumerate(lines) if (m := START.match(l))]
    out = []
    for k, (i, name) in enumerate(starts):
        # Stop at this function's own `.func_end` when it has one. Running to the
        # NEXT .thumb_func_start swallows the following function's `@` doc-comment
        # block, and since LABEL_REF matches inside comments, prose that merely
        # NAMES a label was reported as a read. Brief A's Func_80f0678 was asked
        # for three exports where the body references exactly one: the extra two
        # were named in Func_80f07f0's comment. Over-exporting is the safe
        # direction, so this never broke a link -- it wasted agent rounds, because
        # the output was being handed to briefs as "the exact export list".
        limit = starts[k + 1][0] if k + 1 < len(starts) else dstart
        stop = limit
        for j in range(i, limit):
            if FUNC_END.match(lines[j]):
                stop = j + 1
                break
        # Strip `@`-to-end-of-line before matching, for the same reason: a label
        # discussed in a comment is not a label the code reads.
        body = "".join(COMMENT.sub("", l) for l in lines[i:stop])
        reads = sorted({r for r in LABEL_REF.findall(body) if r in data_labels})
        need = [r for r in reads if r not in exported]
        out.append((name, reads, need))
    return out


def inspect(path):
    """(data_sections, exported_syms, funcs) for one .s, or None if generated."""
    text = open(path, errors="ignore").read()
    # NOT `".gcc2_compiled." in text`. That is a SUBSTRING search, and hand-written
    # disassembly discusses the string in its `@` prose ("text through UIDrawText and
    # .gcc2_compiled."), so the substring test called 19 hand-written files "generated"
    # and SILENTLY PASSED THEM -- including files carrying .rodata, which is the one
    # thing this tool exists to catch. filtered.py had already found and fixed exactly
    # this trap (see its `generated` docstring: 72 files, 126 functions, three tools);
    # the fix did not reach here. Reuse that function rather than re-derive the test.
    if generated(path, text.splitlines(True)):
        return None                      # generated from a .c; not ours to split
    secs = [s for s in SECTION.findall(text)
            if any(s.startswith(d) for d in DATA_SECTIONS)]
    if not secs:
        return None
    return secs, GLOBAL.findall(text), FUNC.findall(text)


def main():
    args = sys.argv[1:]
    if not args:
        sys.exit(__doc__.strip().splitlines()[0] +
                 "\n\nusage: datacheck.py <file.s>... | --all")

    if args[0] == "--all":
        paths = []
        for root, _d, fs in os.walk(os.path.join(ROOT, "asm")):
            paths += [os.path.join(root, f) for f in sorted(fs) if f.endswith(".s")]
    else:
        paths = args

    bad = 0
    for p in paths:
        got = inspect(p)
        if not got:
            continue
        secs, syms, funcs = got
        if not funcs:
            continue                     # data-only file: nothing to lose
        bad += 1
        print("%s" % os.path.relpath(p, ROOT))
        print("    data sections : %s" % ", ".join(sorted(set(secs))))
        print("    functions     : %s" % ", ".join(funcs))
        if syms:
            print("    EXPORTS       : %s  (already global -- NOT the set a split needs)"
                  % ", ".join(syms))
        print("    -> converting a function here needs a TEXT/DATA SPLIT; the "
              "data must keep its own object.")
        req = split_requirements(p)
        for name, reads, need in (req or []):
            if not reads:
                print("    %-28s reads no data label -> split needs NO new export"
                      % name)
            elif need:
                print("    %-28s reads %s" % (name, ", ".join(reads)))
                print("    %-28s *** SPLIT MUST EXPORT: %s" %
                      ("", " ".join(".global " + r for r in need)))
            else:
                print("    %-28s reads %s (all already global) -> no new export"
                      % (name, ", ".join(reads)))
    if args[0] == "--all":
        print("\n%d .s files carry both code and data." % bad)
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
