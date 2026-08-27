#!/bin/sh
# scr.sh <cfile> <ref.s> [extra args...]
C="$1"; R="$2"; shift 2
docker run --rm -v "$PWD:/work" -w /work goldensun-build python3 tools/tryc.py "$C" --ref "$R" "$@"
