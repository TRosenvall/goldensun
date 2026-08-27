#!/bin/sh
cd /Users/timothyrosenvall/gs_project/goldensun
f="$1"; shift
docker run --rm -v "$PWD:/work" -w /work goldensun-build /opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -Iinclude "$@" -S -o - "$f"
