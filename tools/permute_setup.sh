#!/bin/bash
# Set up a decomp-permuter working directory for one function.
#
#   tools/permute_setup.sh Func_b074 rom_9000/src/f9_1_rom_b074.c \
#                                    rom_9000/src/f9_1_rom_b074.s
#
# then:
#   python3 decomp-permuter/permuter.py nonmatching/Func_b074
#
# The permuter mutates base.c (reordering temporaries, swapping equivalent
# idioms) looking for the variant whose compiled output matches target.o. That
# is the search that has to be done by hand otherwise, and it is what stalls
# progress once the instruction sequence is right but the registers are wrong.
set -e

FUNC="$1"
CSRC="$2"
ASMSRC="$3"

if [ -z "$FUNC" ] || [ -z "$CSRC" ] || [ -z "$ASMSRC" ]; then
    echo "usage: $0 <Func_name> <source.c> <reference.s>" >&2
    exit 1
fi

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

OUT="nonmatching/$FUNC"
mkdir -p "$OUT"

# Flags come from the Makefile so the permuter cannot compile differently from
# the build. Keep this in step with tools/asmdiff.py, which does the same.
CPPFLAGS=$(sed -n 's/^GBA_CPPFLAGS[[:space:]]*:\{0,1\}=[[:space:]]*//p' Makefile)
CFLAGS=$(sed -n 's/^GBA_CFLAGS[[:space:]]*:\{0,1\}=[[:space:]]*//p' Makefile)

cat > "$OUT/compile.sh" <<EOF
#!/bin/bash
# invoked as ./compile.sh input.c -o output.o
set -e
cd "$ROOT"
IN="\$1"; OUT="\$3"
arm-none-eabi-cpp $CPPFLAGS "\$IN" \\
  | tools/agbcc/bin/agbcc $CFLAGS -o - \\
  | cat - <(printf '.text\n\t.align\t2, 0\n') \\
  | arm-none-eabi-as -mcpu=arm7tdmi -Iinclude -o "\$OUT"
EOF
chmod +x "$OUT/compile.sh"

# target.o: the reference disassembly, assembled. Built from the .s rather than
# from raw ROM bytes on purpose -- GAS marks .incbin content as data, so an
# object built that way will not disassemble as Thumb and the permuter cannot
# score against it.
arm-none-eabi-as -mcpu=arm7tdmi -Iinclude -o "$OUT/target.o" "$ASMSRC"

# base.c: preprocessed, with every other function stripped out.
arm-none-eabi-cpp $CPPFLAGS "$CSRC" > "$OUT/base.c"
if [ -f decomp-permuter/strip_other_fns.py ]; then
    python3 decomp-permuter/strip_other_fns.py "$OUT/base.c" "$FUNC" || true
fi

# sanity check: does base.c compile through the same pipeline?
if "$OUT/compile.sh" "$OUT/base.c" -o "$OUT/base.o" 2>/dev/null; then
    echo "  base.c compiles"
else
    echo "  WARNING: base.c does not compile -- fix it before permuting" >&2
fi

echo "  wrote $OUT/{compile.sh,base.c,target.o}"
echo
echo "  run:  python3 decomp-permuter/permuter.py $OUT"
