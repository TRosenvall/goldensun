#!/bin/bash
f=$1
/opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
  -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -Iinclude -S -o /tmp/m.s "$f" 2>&1|head -2
arm-none-eabi-as -mcpu=arm7tdmi -mthumb -mthumb-interwork -o /tmp/m.o /tmp/m.s 2>&1|head -2
arm-none-eabi-objcopy -O binary --only-section=.text /tmp/m.o /tmp/m.bin
python3 - "$f" <<'PY'
import sys
rom = open("overlays/rom_7ec19c/orig.bin","rb").read()[0x16c:0x16c+0x6c]
our = open("/tmp/m.bin","rb").read()
bl=set()
for i in range(0,len(our)-3,2):
    if our[i+1]&0xf8==0xf0 and our[i+3]&0xf8==0xf8: bl.update((i,i+1,i+2,i+3))
bad=[i for i in range(min(len(rom),len(our))) if rom[i]!=our[i] and i not in bl]
print("%-7s len %3d diffs %2d  first=%s" % (sys.argv[1].split("/")[-1], len(our), len(bad),
      [hex(x) for x in bad][:6]))
PY
