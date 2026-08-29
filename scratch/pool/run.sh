#!/bin/bash
f=$1
/opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
   -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -Iinclude -S -o /tmp/p.s "$f" 2>&1 | head -3
grep -A20 "^\.L[0-9]*:" /tmp/p.s | grep -E "^\s+\.word" | sed 's/[ \t]*\.word[ \t]*//' | tr '\n' ' '
echo "  <- $(basename $f)"
