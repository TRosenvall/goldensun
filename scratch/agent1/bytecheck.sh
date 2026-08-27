#!/bin/sh
# bytecheck.sh <candidate.c> <ref.s> <FuncName> [cflags...]
# Assembles the candidate and the ROM function standalone and diffs .text.
C="$1"; REF="$2"; FN="$3"; shift 3
cd /tmp || exit 1
python3 - "$REF" "$FN" <<'PY' > rom.s
import sys
t=open('/work/'+sys.argv[1]).read()
fn=sys.argv[2]
import re
m=re.search(r'^\\.thumb_func_start '+re.escape(fn)+r'\\s*(@.*)?$', t, re.M)
i=m.start()
j=t.index('.func_end '+fn)+len('.func_end '+fn)
print('\t.include "macros.inc"')
print(t[i:j])
PY
/opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
  -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -I/work/include "$@" \
  -S -o ours.s "/work/$C" || exit 1
printf '\n\t.text\n\t.align\t2, 0\n' >> ours.s
arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -I/work/include -o ours.o ours.s 2>/dev/null || exit 1
arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -I/work/include -o rom.o rom.s 2>/dev/null || exit 1
arm-none-eabi-objdump -d ours.o | sed 's/^ *//' | cut -f2- | tail -n +4 > o.txt
arm-none-eabi-objdump -d rom.o  | sed 's/^ *//' | cut -f2- | tail -n +4 > r.txt
echo "== $FN"
arm-none-eabi-size -A ours.o | grep -w .text | sed 's/^/  ours /'
arm-none-eabi-size -A rom.o  | grep -w .text | sed 's/^/  rom  /'
if diff -q o.txt r.txt >/dev/null; then echo "  TEXT IDENTICAL"; else diff o.txt r.txt | head -30; fi
echo "  relocs:"; arm-none-eabi-objdump -r ours.o | grep ABS32 | sed 's/^/    /'
