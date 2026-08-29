#!/bin/bash
# compile a two-function candidate, print pool words + whether the 0x38 bytes
# of Func_80c0e38 match the ROM (bl target masked out)
f=$1
/opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
  -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -Iinclude -S -o /tmp/c.s "$f" 2>&1|head -2
arm-none-eabi-as -mcpu=arm7tdmi -mthumb -mthumb-interwork -o /tmp/c.o /tmp/c.s 2>&1|head -2
arm-none-eabi-objcopy -O binary --only-section=.text /tmp/c.o /tmp/c.bin
python3 - "$f" <<'PY'
import sys
rom = open("baserom.gba","rb").read()[0xc0e38:0xc0e70]
our = open("/tmp/c.bin","rb").read()[:0x38]
words = [int.from_bytes(our[i:i+4],"little") for i in range(0x10,0x20,4)]
romw  = [int.from_bytes(rom[i:i+4],"little") for i in range(0x10,0x20,4)]
mask = list(range(0x28,0x2c))          # the bl displacement
same = all(a==b for i,(a,b) in enumerate(zip(rom,our)) if i not in mask)
print(("MATCH  " if same else "differ "),
      "pool:", " ".join("%08x"%w for w in words), " <-", sys.argv[1].split("/")[-1])
PY
