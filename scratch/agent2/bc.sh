#!/bin/sh
# bc.sh <candidate.c> <ref.s> <FuncName> [cflags...]
C="$1"; REF="$2"; FN="$3"; shift 3
cd /tmp || exit 1
python3 - "$REF" "$FN" <<'PY' > rom.s
import sys, re
t=open('/work/'+sys.argv[1]).read().split('\n')
fn=sys.argv[2]
i=next(k for k,l in enumerate(t) if l.startswith('.thumb_func_start '+fn) and (l[18+len(fn):19+len(fn)] in ('',' ','\t')))
j=next(k for k,l in enumerate(t) if k>i and l.startswith('.func_end '+fn))
print('\t.include "macros.inc"')
print('\n'.join(t[i:j+1]))
PY
/opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
  -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -I/work/include "$@" \
  -S -o ours.s "/work/$C" || exit 1
printf '\n\t.text\n\t.align\t2, 0\n' >> ours.s
arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -I/work/include -o ours.o ours.s || exit 1
arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -I/work/include -o rom.o rom.s || exit 1
arm-none-eabi-objdump -d ours.o | sed 's/^ *//' | cut -f2- | tail -n +4 > o.txt
arm-none-eabi-objdump -d rom.o  | sed 's/^ *//' | cut -f2- | tail -n +4 > r.txt
echo "== $FN"
arm-none-eabi-size -A ours.o | grep -w .text | sed 's/^/  ours /'
arm-none-eabi-size -A rom.o  | grep -w .text | sed 's/^/  rom  /'
if diff -q o.txt r.txt >/dev/null; then echo "  TEXT IDENTICAL"; else diff o.txt r.txt | head -40; fi
