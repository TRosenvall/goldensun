#!/bin/sh
# usage: t.sh <file.c> <ref.s> [extra tryc args...]
cd /Users/timothyrosenvall/gs_project/goldensun
f="$1"; r="$2"; shift 2
docker run --rm -v "$PWD:/work" -w /work goldensun-build python3 tools/tryc.py "$f" --ref "$r" "$@"
