#!/bin/sh
# Compile a probe .c with production flags and print the thumb asm. FAILS LOUDLY.
[ -f "$1" ] || { echo "FATAL: no such file: $1" >&2; exit 2; }
out=$(docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build \
  /opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
    -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -Iinclude -S -o - "$1" 2>&1) || {
  echo "FATAL: compile failed for $1" >&2; echo "$out" >&2; exit 3; }
case "$out" in *"	"*) ;; *) echo "FATAL: no asm emitted for $1" >&2; echo "$out" >&2; exit 4;; esac
echo "$out"
