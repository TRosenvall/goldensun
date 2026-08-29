#!/bin/sh
# r7.sh <scratchname> <ref.s>  -- screens default and with -fno-rerun-cse-after-loop
cd /Users/timothyrosenvall/gs_project/goldensun
C="scratch/agent4/$1.c"; R="$2"
echo "--- default"
docker run --rm -v "$PWD:/work" -w /work goldensun-build python3 tools/tryc.py "$C" --ref "$R" --full 2>&1 | tail -3
echo "--- nocse"
docker run --rm -v "$PWD:/work" -w /work goldensun-build python3 tools/tryc.py "$C" --ref "$R" --full --cflags "-fno-rerun-cse-after-loop" 2>&1 | tail -3
