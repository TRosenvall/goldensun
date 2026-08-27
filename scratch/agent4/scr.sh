#!/bin/sh
# scr.sh <name-without-.c> <ref.s> [extra tryc args...]
C="$1"; R="$2"; shift 2
docker run --rm -v "$PWD:/work" -w /work goldensun-build \
  python3 tools/tryc.py "scratch/agent4/$C.c" --ref "$R" "$@"
