#!/bin/sh
# scr.sh <candidate.c> <ref.s> [extra args...]
C="$1"; REF="$2"; shift 2
docker run --rm -v "$PWD:/work" -w /work goldensun-build \
  python3 tools/tryc.py "$C" --ref "$REF" "$@"
