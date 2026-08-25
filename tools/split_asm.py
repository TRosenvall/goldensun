#!/usr/bin/env python3
"""split_asm.py -- cut one function out of a hand-written .s, safely.

    python3 tools/split_asm.py <asm/path/file.s> <FunctionName> [--apply]

Prints what it would do. `--apply` performs it.

WHY THIS EXISTS

Splitting a .s by hand has three failure modes, and all three have bitten this
project more than once:

  1. THE BASENAME COLLISION. A C file at src/<p>/X.c compiles to asm/<p>/X.o and
     writes gcc's assembly to asm/<p>/X.s -- overwriting the very .s being split.
     The link error reads like a stale object ("multiple definition" AND
     "undefined reference" together) and sends you looking in the wrong place.

  2. THE FILE-LOCAL .L LABEL. A `.L1234` referenced across the new object
     boundary does not resolve, because local labels do not cross objects. The
     error names the label and reads like a missing file:

         (rom_15000+0xb080): undefined reference to `.L73854'

     Hit three separate times. The fix is `.global` on the definition side;
     symbol binding is link-time metadata so the emitted bytes do not change.

  3. LOST DATA. A .s carrying .incbin/.incrom/.section survives only if the
     linker script keeps pointing at it for that section.

This checks all three before touching anything.

THREE-WAY SPLITS ARE CHECKED TOO. Cutting a function out of the MIDDLE leaves
two remaining pieces, and labels cross between THOSE as readily as across the
cut. The first version of this tool only checked the cut boundary; a three-way
split of `rom_f6008_c.s` linked cleanly there and failed on the other side,
because the twelve earlier functions referenced six `.L` labels in the `.rodata`
that stayed with the later half. It reported "none needed" for a split that
needed six. Every pair of resulting pieces is now reported.

WHAT IT DOES NOT DO

It does not edit the linker script or write the .c -- those need judgement about
piece names and slot order. It tells you exactly which lines to add.
"""
import os
import re
import sys

FUNC = r"\.thumb_func_start\s+{}\b"
LABEL_DEF = re.compile(r"^(\.L[0-9a-fA-Z_]+):", re.M)
LABEL_REF = re.compile(r"=(\.L[0-9a-fA-Z_]+)\b")
DATA = re.compile(r"^\s*\.(incbin|incrom|incdata|section)\b", re.M)


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    asm, fn = sys.argv[1], sys.argv[2]
    apply_it = "--apply" in sys.argv
    L = open(asm, errors="replace").read().split("\n")

    try:
        i_s = next(i for i, l in enumerate(L) if re.match(FUNC.format(re.escape(fn)), l))
    except StopIteration:
        print(f"ERROR: {fn} not found in {asm}")
        return 1
    i_e = next(i for i, l in enumerate(L) if l.startswith(f".func_end {fn}"))
    s = i_s
    while s > 0 and (L[s - 1].startswith("@") or L[s - 1].strip() == ""):
        s -= 1
    e = i_e + 1
    while e < len(L) and L[e].strip() == "":
        e += 1

    cut, rest = "\n".join(L[s:e]), "\n".join(L[:s] + L[e:])
    n_funcs = len(re.findall(r"^\.thumb_func_start", "\n".join(L), re.M))

    # (2) labels crossing ANY boundary the split creates.
    #
    # A middle cut yields three pieces -- head, cut, tail -- so there are three
    # pairs to check, not one. Checking only cut-vs-rest is what let a six-label
    # failure through in batch 68.
    head, tail = "\n".join(L[:s]), "\n".join(L[e:])
    need_global = sorted(set(LABEL_REF.findall(cut)) & set(LABEL_DEF.findall(rest)))
    reverse = sorted(set(LABEL_REF.findall(rest)) & set(LABEL_DEF.findall(cut)))
    head_needs = sorted(set(LABEL_REF.findall(head)) & set(LABEL_DEF.findall(tail)))
    tail_needs = sorted(set(LABEL_REF.findall(tail)) & set(LABEL_DEF.findall(head)))

    print(f"{asm}: {n_funcs} function(s); cutting {fn} (lines {s+1}-{e})")
    print(f"  carries data      : {'YES -- keep a .s and its section line' if DATA.search(rest) else 'no'}")
    print(f"  remaining funcs   : {n_funcs - 1}")
    if need_global:
        print(f"  MUST EXPORT (add `.global` beside each definition in the .s):")
        for l in need_global:
            print(f"      .global {l}")
    else:
        print("  label exports     : none needed")
    if reverse:
        print(f"  REVERSE refs (the .s would reference labels inside the cut): {reverse}")
        print("      -> the cut is not clean; pick a different boundary")
    if head and tail and (head_needs or tail_needs):
        print("  THREE-WAY SPLIT -- labels also cross between the two REMAINING")
        print("  pieces. If you keep them as separate objects, export these too:")
        for l in head_needs:
            print(f"      .global {l}      (defined after the cut, used before)")
        for l in tail_needs:
            print(f"      .global {l}      (defined before the cut, used after)")
    elif head and tail:
        print("  three-way split : no labels cross between the remaining pieces")
    print(f"  BASENAME WARNING  : name the .c something OTHER than "
          f"'{os.path.basename(asm)[:-2]}' unless this .s is deleted entirely")

    if apply_it:
        if reverse:
            print("REFUSING to apply: reverse label references")
            return 1
        out = rest
        for l in need_global:
            out = out.replace(f"\n{l}:", f"\n\t.global {l}\n{l}:", 1)
        open(asm, "w").write(out.rstrip() + "\n")
        base = asm[:-2]
        open(base + "_cut.s", "w").write(cut.rstrip() + "\n")
        print(f"APPLIED: {asm} rewritten, cut saved to {base}_cut.s")
    return 0


if __name__ == "__main__":
    sys.exit(main())
