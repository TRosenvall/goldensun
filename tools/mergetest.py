#!/usr/bin/env python3
"""mergetest.py -- is merging a module's landed pieces back into ONE TU byte-neutral?

    python3 tools/mergetest.py src/overlays/rom_7ac2d8/ovl_d58_a.c \
                               src/overlays/rom_7ac2d8/ovl_d58_b.c
    python3 tools/mergetest.py --module src/overlays/rom_7ac2d8/ovl_d58

Run it INSIDE the build container.  Exits non-zero if the merge changes a byte.

WHY PASS 2.5 NEEDS THIS

Pass 2 lands functions ONE AT A TIME, which means splitting upstream's objects
into pieces: 5,203 pieces against upstream's 521 `.s` files, a mean of 24.4
pieces per module.  The split was free -- a pure subdivision, ROM
byte-identical, at a cost of 1,543 local labels promoted to `.global`.

Pass 2.5 has to put them back, because:

  * a module is the unit upstream ships (one `.o` per object in `stage1.ld`);
  * it is the unit pass 4 renames to (4,462 split-named files);
  * and a promoted `.global` is only DEMOTABLE once every piece that references
    it sits in one TU -- which is exactly what the merge creates.

The question this tool answers is whether that merge is mechanical or whether it
is a THIRD MATCHING PASS.  It matters a lot: merging 24 pieces per module across
521 modules would cost as much as pass 2 did if each merge can break.

## THE EVIDENCE SO FAR, AND ITS LIMITS

Batch 334 ran the first case by hand: `ovl_d58_a.c` + `ovl_d58_b.c`, two landed
pieces adjacent in `overlays/rom_7ac2d8/overlay.ld`.  Compiled separately the
two objects hold 67 and 15 encodings; compiled as one TU the result is **82
encodings, byte-identical to the concatenation**.

A mechanism predicts that result, which is why one case was worth believing:
gcc-2.96's minipool is per-function (every input to `add_minipool_forward_ref`
is), and cse/gcse/local-alloc/global-alloc are all per-function too.  There is no
obvious pass whose behaviour depends on what ELSE is in the translation unit.

**It also contradicts an older worry that deserves naming.**  Batch 285 recorded
`OvlFunc_932_200b738` as matching alone and reading 43 differing "once pasted
beside its sibling".  Given this result, that was a statement having MOVED during
the paste, not a property of merging.  Do not cite it as evidence against the
merge.

### What has NOT been tested

The one clean case was 2 pieces and 57 lines whose preamble is nothing but
`extern` declarations.  Untested, in rough order of risk:

  1. **a piece carrying `.data` or `.rodata`** -- `ovl_d58_c` has a `.data`
     section and was deliberately left out of the test;
  2. **`static` helpers that collide** between pieces, or two pieces defining
     the same-named static;
  3. **`#include` ordering** where pieces pull different headers;
  4. **many pieces** -- the mean is 24.4, the test did 2;
  5. **pieces with different per-file Makefile flags.**  THIS ONE IS A HARD
     BLOCKER, not a risk: two pieces compiled with different flags cannot become
     one TU at all, and the tool refuses the merge below.

Only 9 multi-piece modules are fully landed today, so the bulk of this cannot be
tested until pass 2 closes.  Run this tool on each of those 9 first.

## WHAT IT COMPARES

The encoding stream only, offsets excluded -- objdump restarts addressing at 0
for each object, so including the address column reports every second piece as
differing.  That was the first draft's bug.
"""
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import tryc  # noqa: E402

AS = ["arm-none-eabi-as", "-mcpu=arm7tdmi", "-mthumb-interwork",
      "-I" + os.path.join(ROOT, "include")]
OBJDUMP = "arm-none-eabi-objdump"


def compile_to_o(src_text, flags, workdir, stem):
    """Compile text as the build does: -S, append the .text/.align tail, assemble."""
    c = os.path.join(workdir, stem + ".c")
    s = os.path.join(workdir, stem + ".s")
    o = os.path.join(workdir, stem + ".o")
    open(c, "w").write(src_text)
    r = subprocess.run([os.path.join(tryc.GCC, "xgcc")] + flags + ["-S", "-o", s, c],
                       capture_output=True, text=True, cwd=ROOT)
    if r.returncode:
        return None, (r.stderr.strip().splitlines() or ["cc failed"])[-1]
    # the build appends this to every generated .s; see the Makefile's src->asm rule
    with open(s, "a") as f:
        f.write("\n\t.text\n\t.align\t2, 0\n")
    r = subprocess.run(AS + ["-o", o, s], capture_output=True, text=True, cwd=ROOT)
    if r.returncode:
        return None, (r.stderr.strip().splitlines() or ["as failed"])[-1]
    return o, None


def encodings(obj):
    """The encoding column of objdump -d, WITHOUT offsets (see the docstring)."""
    out = subprocess.run([OBJDUMP, "-d", obj], capture_output=True, text=True).stdout
    enc = []
    for l in out.splitlines():
        if not re.match(r"^\s+[0-9a-f]+:", l):
            continue
        parts = l.split("\t")
        if len(parts) > 1:
            enc.append(parts[1].strip())
    return enc


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    if "--module" in sys.argv and len(args) == 1:
        stem = args[0]
        args = sorted(p for p in
                      __import__("glob").glob(stem + "*.c")
                      if re.fullmatch(re.escape(stem) + r"(_[abcd])*\.c", p))
        if not args:
            sys.stderr.write(f"no landed pieces match {stem}*\n")
            return 2
        print("pieces: " + ", ".join(os.path.basename(a) for a in args))
    if len(args) < 2:
        sys.stderr.write("need at least two landed .c pieces, in LINK ORDER\n")
        return 2

    rel = [os.path.relpath(os.path.abspath(a), ROOT) for a in args]

    # A HARD PRECONDITION, not a risk: pieces with different per-file flags
    # cannot become one TU.  Refuse rather than produce a meaningless verdict.
    flagsets = {tuple(tryc.makefile_flags(p)) for p in rel}
    if len(flagsets) > 1:
        print("REFUSED: these pieces do not share per-file Makefile flags, so they")
        print("cannot be one translation unit.  Flag sets seen:")
        for f in flagsets:
            print(f"    {list(f) or '(tree default)'}")
        return 3
    adjust = list(flagsets.pop())
    flags = ["-O1" if (a == "-O2" and "O1" in adjust) else a for a in tryc.CFLAGS]
    flags += [a for a in adjust if a not in ("O1",)]

    with tempfile.TemporaryDirectory() as wd:
        sep = []
        for i, p in enumerate(rel):
            o, err = compile_to_o(open(os.path.join(ROOT, p)).read(), flags, wd, f"p{i}")
            if err:
                print(f"XX {p}: {err}")
                return 1
            e = encodings(o)
            print(f"  {os.path.basename(p):<44} {len(e):>5} encodings")
            sep += e
        merged = "\n".join(open(os.path.join(ROOT, p)).read() for p in rel)
        o, err = compile_to_o(merged, flags, wd, "merged")
        if err:
            print(f"XX merged TU failed to compile: {err}")
            print("   (a static collision or a duplicate definition would land here)")
            return 1
        m = encodings(o)
        print(f"  {'MERGED TU':<44} {len(m):>5} encodings")

    if m == sep:
        print(f"\nOK  merge is BYTE-NEUTRAL -- {len(m)} encodings identical")
        return 0
    print(f"\nXX  MERGE CHANGES THE OUTPUT: separate {len(sep)}, merged {len(m)}")
    for i, (a, b) in enumerate(zip(sep, m)):
        if a != b:
            print(f"    first difference at encoding {i}: separate {a}  merged {b}")
            break
    return 1


if __name__ == "__main__":
    sys.exit(main())
