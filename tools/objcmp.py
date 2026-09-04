#!/usr/bin/env python3
"""objcmp.py -- compare a candidate .c against a reference .s AT THE OBJECT LEVEL.

    python3 tools/objcmp.py <candidate.c> <reference.s> [--func NAME]

Run it INSIDE the build container. Exits non-zero on any difference.

WHY THIS EXISTS, and when to reach for it instead of tryc.py.

tryc.py compares INSTRUCTION STREAMS. To stay robust against where a literal
pool happens to land, it normalises every PC-relative load to `=value`. That is
the right call for screening and it makes tryc blind to two real defects:

  1. POOL ORDER. Batch 218's Func_80982dc has 86 instructions against 86, 196
     bytes against 196, ZERO differing mnemonics -- and `make compare` fails.
     Its nine pool words are the ROM's nine, rotated: gcc emitted the last one
     first. The ROM's .s reaches constants with `ldr rX, =value`, so the
     ASSEMBLER pools them in instruction order; gcc emits its own `.word` list
     in its own order. Every `ldr [pc, #N]` then points four bytes further and
     tryc sees nothing.

  2. DUPLICATE LABELS. Where gcc puts two labels at one address (a pool-dump
     target and the epilogue, say), the ROM's disassembly can only show one, and
     tryc reports differing lines that are not real. Batch 218's
     OvlFunc_928_2008500 screens at "6 differ" and is byte-identical.

So the two tools answer different questions and neither replaces the other:
tryc tells you WHICH INSTRUCTION is wrong while you are still iterating; this
tells you whether you are actually done. USE THIS BEFORE TOUCHING THE BUILD
whenever tryc's own `!!` warning says the reference keeps its pool inside the
function -- that warning marks exactly the cases where its normalisation hides
things.

It is not a substitute for `make compare`, which remains the gate: this checks
one function in isolation and cannot see linker-script or layout mistakes.
"""
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

GCC = os.environ.get("GCC296_DIR", "/opt/gcc296")
AS = ["arm-none-eabi-as", "-mcpu=arm7tdmi", "-mthumb-interwork", "-I" + os.path.join(ROOT, "include")]
START = re.compile(r"^\s*\.(thumb|arm)_func_start(?:_noalign)?\s+(\S+)")
ENC = re.compile(r"^\s*[0-9a-f]+:\t([0-9a-f ]+?)\s*\t")


def cflags_for(ref):
    """Reuse tryc.py's per-file flag detection so both tools agree."""
    import tryc
    adjust = tryc.makefile_flags(re.sub(r"^asm/", "src/", os.path.relpath(ref, ROOT))[:-2] + ".c")
    flags = ["-O1" if (a == "-O2" and "O1" in adjust) else a for a in tryc.CFLAGS]
    if "no-sched2" in adjust:
        flags += ["-fno-schedule-insns2"]
    if "no-rerun-cse" in adjust:
        flags += ["-fno-rerun-cse-after-loop"]
    if "no-interwork" in adjust:
        flags = [a for a in flags if a != "-mthumb-interwork"]
    for a in sorted(adjust):
        if a.startswith("-f") and a not in flags:
            flags += [a]
    return flags, adjust


def one_function(ref, name):
    """Cut NAME out of a multi-function .s so the object holds it alone.

    Comparing a whole multi-function reference would be meaningless: the sizes
    would differ by the other functions. The includes are re-added because the
    macros (.call_via and friends) are what the body assembles against.
    """
    lines = open(ref, errors="ignore").read().splitlines(True)
    out, keep = [], False
    for l in lines:
        m = START.match(l)
        if m:
            keep = (m.group(2) == name)
        if keep:
            out.append(l)
        if keep and l.startswith(".func_end"):
            break
    if not out:
        sys.exit("objcmp: %s not found in %s" % (name, ref))
    return ['\t.include "macros.inc"\n', '\t.include "gba.inc"\n', "\n"] + out


def dump(obj):
    d = subprocess.run(["arm-none-eabi-objdump", "-d", obj],
                       capture_output=True, text=True).stdout
    enc = [m.group(1).strip() for m in (ENC.match(l) for l in d.splitlines()) if m]
    r = subprocess.run(["arm-none-eabi-objdump", "-r", obj],
                       capture_output=True, text=True).stdout
    rel = [l.split() for l in r.splitlines() if re.match(r"^[0-9a-f]{8}\s", l)]
    size = subprocess.run(["arm-none-eabi-size", obj],
                          capture_output=True, text=True).stdout.splitlines()[-1].split()[0]
    return enc, rel, int(size)


def main():
    argv = sys.argv[1:]
    name = None
    if "--func" in argv:
        i = argv.index("--func")
        name = argv[i + 1]
        del argv[i:i + 2]          # drop BOTH the flag and its value
    args = [a for a in argv if not a.startswith("--")]
    if len(args) != 2:
        sys.exit(__doc__.strip().splitlines()[2].strip())
    src, ref = args
    if name is None:
        names = [m.group(2) for m in (START.match(l) for l in open(ref, errors="ignore")) if m]
        if len(names) != 1:
            sys.exit("objcmp: %s holds %d functions; pass --func NAME" % (ref, len(names)))
        name = names[0]

    tmp = "/tmp/objcmp"
    os.makedirs(tmp, exist_ok=True)
    refs = os.path.join(tmp, "ref.s")
    open(refs, "w").writelines(one_function(ref, name))
    subprocess.run(AS + ["-o", os.path.join(tmp, "ref.o"), refs],
                   capture_output=True)

    flags, adjust = cflags_for(ref)
    cs = os.path.join(tmp, "cand.s")
    p = subprocess.run([os.path.join(GCC, "xgcc")] + flags + ["-S", "-o", cs, src],
                       capture_output=True, text=True)
    if p.returncode:
        sys.exit("objcmp: compile failed\n" + p.stderr)
    with open(cs, "a") as f:
        f.write("\n\t.text\n\t.align\t2, 0\n")
    subprocess.run(AS + ["-o", os.path.join(tmp, "cand.o"), cs], capture_output=True)

    if adjust:
        print("  (built with: %s)" % ", ".join(sorted(adjust)))
    a_enc, a_rel, a_sz = dump(os.path.join(tmp, "ref.o"))
    b_enc, b_rel, b_sz = dump(os.path.join(tmp, "cand.o"))

    bad = 0
    if a_sz != b_sz:
        print("  XX SIZE  ref %d bytes, ours %d" % (a_sz, b_sz)); bad = 1
    if a_enc != b_enc:
        n = sum(1 for x, y in zip(a_enc, b_enc) if x != y) + abs(len(a_enc) - len(b_enc))
        print("  XX ENCODINGS differ in %d place(s) (ref %d, ours %d)"
              % (n, len(a_enc), len(b_enc)))
        for i, (x, y) in enumerate(zip(a_enc, b_enc)):
            if x != y:
                print("     first at index %d: ref %s  ours %s" % (i, x, y)); break
        bad = 1
    if a_rel != b_rel:
        print("  XX RELOCATIONS differ"); print("     ref ", a_rel); print("     ours", b_rel); bad = 1
    if not bad:
        print("  OK %s -- %d bytes, %d encodings and %d relocations identical"
              % (name, a_sz, len(a_enc), len(a_rel)))
    sys.exit(bad)


if __name__ == "__main__":
    main()
