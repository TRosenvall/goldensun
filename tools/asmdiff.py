#!/usr/bin/env python3
"""Side-by-side diff of compiled C against the original ROM code.

The reference side is the ROM itself: either bytes lifted straight out of
baserom.gba, or an existing hand-disassembled .s (which is assembled and then
disassembled again, so what you compare is machine code, not source text).

Usage
-----
  tools/asmdiff.py Func_b684 rom_9000/src/f9_4_rom_b684.c \\
      --ref rom_9000/src/f9_4_rom_b684_old.s

  tools/asmdiff.py Func_b684 rom_9000/src/f9_4_rom_b684.c \\
      --rom-offset 0xb684 --rom-size 52

  # sweep compilers and flags to see which lands closest
  tools/asmdiff.py Func_b684 rom_9000/src/f9_4_rom_b684.c \\
      --ref rom_9000/src/f9_4_rom_b684_old.s --sweep

Exit status is 0 on an exact match, 1 otherwise, so it can gate a build.
"""

import argparse
import difflib
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

CPP = "arm-none-eabi-cpp"
AS = "arm-none-eabi-as"
OBJDUMP = "arm-none-eabi-objdump"
OBJCOPY = "arm-none-eabi-objcopy"

DEFAULT_CC = "tools/agbcc/bin/agbcc"


def makefile_var(name, fallback):
    """Read a flag variable straight out of the Makefile.

    Deliberately not hardcoded: if the build's flags and the differ's flags ever
    disagree, the differ is lying to you.
    """
    try:
        with open(os.path.join(ROOT, "Makefile")) as f:
            for line in f:
                m = re.match(r"^%s\s*:?=\s*(.*)$" % re.escape(name), line)
                if m:
                    return m.group(1).split()
    except OSError:
        pass
    return fallback


DEFAULT_CPPFLAGS = makefile_var("GBA_CPPFLAGS",
                                ["-Iinclude", "-nostdinc", "-undef", "-std=gnu89"])
DEFAULT_CFLAGS = makefile_var("GBA_CFLAGS",
                              ["-mthumb-interwork", "-O2", "-fhex-asm", "-fcall-used-r4"])

RED, GRN, YEL, DIM, RST = "\033[31m", "\033[32m", "\033[33m", "\033[2m", "\033[0m"


def sh(cmd, **kw):
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, **kw)


def compile_c(src, cc, cppflags, cflags, out_o):
    """cpp | cc1 | append .text align | as   -- the project's own pipeline."""
    pre = sh([CPP] + cppflags + [src])
    if pre.returncode:
        return pre.stderr
    comp = subprocess.run([os.path.join(ROOT, cc)] + cflags + ["-o", "-"],
                          input=pre.stdout, capture_output=True, text=True, cwd=ROOT)
    if comp.returncode:
        return comp.stderr
    asm_text = comp.stdout + ".text\n\t.align\t2, 0\n"
    a = subprocess.run([AS, "-mcpu=arm7tdmi", "-Iinclude", "-o", out_o],
                       input=asm_text, capture_output=True, text=True, cwd=ROOT)
    return a.stderr if a.returncode else None


def assemble_s(src, out_o):
    r = sh([AS, "-mcpu=arm7tdmi", "-Iinclude", "-o", out_o, src])
    return r.stderr if r.returncode else None


def text_bytes(obj):
    with tempfile.NamedTemporaryFile(suffix=".bin", delete=False) as t:
        binp = t.name
    sh([OBJCOPY, "-O", "binary", "--only-section=.text", obj, binp])
    with open(binp, "rb") as f:
        data = f.read()
    os.unlink(binp)
    return data


def rom_bytes(offset, size):
    with open(os.path.join(ROOT, "baserom.gba"), "rb") as f:
        f.seek(offset)
        return f.read(size)


RELOC = re.compile(r"^([0-9a-f]{8})\s+(\S+)\s+(\S+)\s*$")


def relocations(obj):
    """Byte offsets in .text that the linker will patch, with their symbols.

    An unlinked object holds 0 (or an addend) where a call target will go, while
    the ROM holds the resolved offset. Comparing those bytes directly reports a
    difference at every call site even when the code is correct, which is why
    any function that calls out could not be diffed before.
    """
    out = {}
    for line in sh([OBJDUMP, "-r", obj]).stdout.splitlines():
        m = RELOC.match(line.strip())
        if m and m.group(2).startswith("R_ARM"):
            out[int(m.group(1), 16)] = (m.group(2), m.group(3))
    return out


def mask(data, offsets, width=4):
    """Zero `width` bytes at each offset so relocated operands compare equal."""
    b = bytearray(data)
    for off in offsets:
        for k in range(off, min(off + width, len(b))):
            b[k] = 0
    return bytes(b)


def disasm_bytes(data, func):
    """Disassemble raw Thumb bytes.

    objdump's raw-binary mode is used deliberately: round-tripping through an
    ELF loses the Thumb marking (GAS tags .incbin content as data, so objdump
    renders .word instead of instructions, even with -M force-thumb).
    """
    with tempfile.NamedTemporaryFile(suffix=".bin", delete=False) as t:
        t.write(data)
        binp = t.name
    out = sh([OBJDUMP, "-D", "-b", "binary", "-m", "arm7tdmi",
              "-M", "force-thumb", "--no-show-raw-insn", binp]).stdout
    os.unlink(binp)
    return parse_disasm(out, func)


LINE = re.compile(r"^\s*([0-9a-f]+):\s+(.*)$")


def parse_disasm(text, func):
    insns = []
    for raw in text.splitlines():
        m = LINE.match(raw)
        if not m:
            continue
        addr, body = int(m.group(1), 16), m.group(2).strip()
        if not body or body.startswith("..."):
            continue
        body = re.sub(r"\s*@.*$", "", body)          # drop objdump's @ comments
        body = re.sub(r"\s+", " ", body)
        insns.append((addr, body))
    # Rewrite absolute branch targets as signed instruction deltas, so a single
    # inserted instruction does not make every later branch look different.
    index = {a: i for i, (a, _) in enumerate(insns)}
    out = []
    for i, (addr, body) in enumerate(insns):
        def repl(m):
            tgt = int(m.group(1), 16)
            if tgt in index:
                return "->%+d" % (index[tgt] - i)
            return m.group(0)
        # raw-binary objdump renders branch targets as bare 0xNN
        body = re.sub(r"0x([0-9a-f]+)\b", repl, body)
        # ELF objdump renders them as NN <sym+0xNN>
        body = re.sub(r"\b([0-9a-f]+)\s*<[^>]*>", repl, body)
        out.append(body)
    return out


def render(ref, got, label_ref, label_got, width=44):
    sm = difflib.SequenceMatcher(a=ref, b=got, autojunk=False)
    rows, same = [], 0
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            for k in range(i2 - i1):
                rows.append((" ", ref[i1 + k], got[j1 + k]))
                same += 1
        elif tag == "replace":
            for k in range(max(i2 - i1, j2 - j1)):
                rows.append(("|",
                             ref[i1 + k] if i1 + k < i2 else "",
                             got[j1 + k] if j1 + k < j2 else ""))
        elif tag == "delete":
            for k in range(i1, i2):
                rows.append(("<", ref[k], ""))
        elif tag == "insert":
            for k in range(j1, j2):
                rows.append((">", "", got[k]))

    print("%s%-*s | %s%s" % (DIM, width, label_ref, label_got, RST))
    print(DIM + "-" * (width * 2 + 3) + RST)
    for mark, a, b in rows:
        if mark == " ":
            print("%s%-*s | %s%s" % (DIM, width, a, b, RST))
        else:
            col = YEL if mark == "|" else (RED if mark == "<" else GRN)
            print("%s%-*s |%s %s%s" % (col, width, a, RST + col, b, RST))
    return same, len(rows)


def build_got(args, tmp):
    obj = os.path.join(tmp, "got.o")
    cflags = args.cflags.split() if args.cflags else list(DEFAULT_CFLAGS)
    err = compile_c(args.source, args.cc, DEFAULT_CPPFLAGS, cflags, obj)
    if err:
        print(RED + "compile failed:" + RST)
        print(err.rstrip())
        sys.exit(2)
    return text_bytes(obj), relocations(obj)


def build_ref(args, tmp):
    if args.rom_offset is not None:
        if args.rom_size is None:
            sys.exit("--rom-offset needs --rom-size")
        return rom_bytes(args.rom_offset, args.rom_size)
    obj = os.path.join(tmp, "ref.o")
    err = assemble_s(args.ref, obj)
    if err:
        sys.exit("reference failed to assemble:\n" + err)
    return text_bytes(obj)


def main():
    p = argparse.ArgumentParser(description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("func")
    p.add_argument("source", help=".c file to test")
    p.add_argument("--ref", help="reference .s (assembled, then compared as code)")
    p.add_argument("--rom-offset", type=lambda s: int(s, 0),
                   help="compare against baserom.gba at this offset instead")
    p.add_argument("--rom-size", type=lambda s: int(s, 0))
    p.add_argument("--cc", default=DEFAULT_CC)
    p.add_argument("--cflags", help='override CFLAGS, as one quoted string, '
                                    'e.g. --cflags="-mthumb-interwork -O -fcall-used-r4"')
    p.add_argument("--no-reloc", action="store_true",
                   help="do not mask relocated operands (compare raw bytes)")
    p.add_argument("--sweep", action="store_true",
                   help="try every compiler/flag combination and rank them")
    args = p.parse_args()

    if not args.ref and args.rom_offset is None:
        sys.exit("need --ref or --rom-offset/--rom-size")

    with tempfile.TemporaryDirectory() as tmp:
        ref_bin = build_ref(args, tmp)
        ref = disasm_bytes(ref_bin, args.func)

        if args.sweep:
            combos = []
            for cc in ("tools/agbcc/bin/agbcc", "tools/agbcc/bin/old_agbcc"):
                for opt in ("-O", "-O2"):
                    for extra in ([], ["-fcall-used-r4"], ["-fcall-used-r4", "-fcall-used-r5"]):
                        combos.append((cc, ["-mthumb-interwork", opt, "-fhex-asm"] + extra))
            results = []
            for cc, cf in combos:
                obj = os.path.join(tmp, "s.o")
                if compile_c(args.source, cc, DEFAULT_CPPFLAGS, cf, obj):
                    continue
                got_bin = text_bytes(obj)
                rl = relocations(obj)
                rb, gb = (mask(ref_bin, rl), mask(got_bin, rl)) if rl else (ref_bin, got_bin)
                got = disasm_bytes(gb, args.func)
                refd = disasm_bytes(rb, args.func)
                sm = difflib.SequenceMatcher(a=refd, b=got, autojunk=False)
                same = sum(n for _, _, n in sm.get_matching_blocks())
                results.append((same / max(len(refd), 1), len(got_bin), cc, cf, rb == gb))
            results.sort(reverse=True)
            print("%-22s %-46s %6s %6s %s" % ("compiler", "flags", "bytes", "match", ""))
            for score, size, cc, cf, exact in results:
                print("%-22s %-46s %6d %5.0f%% %s"
                      % (os.path.basename(cc), " ".join(cf), size, score * 100,
                         GRN + "EXACT" + RST if exact else ""))
            return 0 if any(r[4] for r in results) else 1

        got_bin, relocs = build_got(args, tmp)

        # Relocated operands cannot agree until link time, so blank them on both
        # sides before comparing. Offsets come from the candidate object; if the
        # instruction layout has diverged they will not line up, but in that case
        # the diff already shows the real problem.
        ref_cmp, got_cmp = ref_bin, got_bin
        if relocs and not args.no_reloc:
            ref_cmp = mask(ref_bin, relocs)
            got_cmp = mask(got_bin, relocs)

        ref = disasm_bytes(ref_cmp, args.func)
        got = disasm_bytes(got_cmp, args.func)

        same, total = render(ref, got,
                             "ROM  (%d bytes)" % len(ref_bin),
                             "yours (%d bytes)" % len(got_bin))
        exact = ref_cmp == got_cmp
        print()
        if relocs:
            print("%s%d relocation site(s) masked: %s%s"
                  % (DIM, len(relocs),
                     ", ".join(sorted({s for _, s in relocs.values()})), RST))
        print("%s%d/%d instructions aligned%s   %s"
              % (GRN if exact else YEL, same, total, RST,
                 GRN + "*** EXACT MATCH ***" + RST if exact
                 else RED + "not matching" + RST))
        if exact and relocs:
            print(DIM + "match is modulo relocations; confirm with a full "
                        "`make compare` once the .s is removed" + RST)
        return 0 if exact else 1


if __name__ == "__main__":
    sys.exit(main())
