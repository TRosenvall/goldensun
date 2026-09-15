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

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SECTION = re.compile(r"^\s*\.section\s+(\.[\w.]+)", re.M)
FUNC = re.compile(r"^\s*\.(?:thumb|arm)_func_start(?:_noalign)?\s+(\S+)", re.M | re.I)
GLOBAL = re.compile(r"^\s*\.global\s+(\S+)", re.M)
DATA_SECTIONS = (".rodata", ".data", ".bss")


def inspect(path):
    """(data_sections, exported_syms, funcs) for one .s, or None if generated."""
    text = open(path, errors="ignore").read()
    if ".gcc2_compiled." in text:
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
            print("    EXPORTS       : %s" % ", ".join(syms))
        print("    -> converting a function here needs a TEXT/DATA SPLIT; the "
              "data must keep its own object.")
    if args[0] == "--all":
        print("\n%d .s files carry both code and data." % bad)
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
