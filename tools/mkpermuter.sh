#!/bin/bash
# mkpermuter.sh <Func_name> <source.c> [outdir]
#
# Builds a decomp-permuter-agbcc working directory for one function.
#
# The permuter's own README describes a project-wide import that splits every
# .s file and rebuilds -- destructive, and aimed at bulk setup. We only ever
# want a handful of functions at a time, so this does the manual per-function
# setup from USAGE.md instead and touches nothing in the repo.
#
# The directory it produces contains:
#   compile.sh      our exact build pipeline, invoked as ./compile.sh in.c -o out.o
#   base.c          the C source, preprocessed and reduced to this one function
#   target.o        the bytes from baserom.gba that the C must reproduce
#   settings.toml   func_name + compiler_type
#
# target.o is built from the ROM rather than from our .s, so a permuter match
# is a match against the real thing and not against our own transcription.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
FUNC="${1:?usage: mkpermuter.sh <Func_name> <source.c> [outdir]}"
SRC="${2:?usage: mkpermuter.sh <Func_name> <source.c> [outdir]}"
OUT="${3:-$ROOT/../permuter-work/$FUNC}"

command -v arm-none-eabi-objcopy >/dev/null || { echo "need arm-none-eabi binutils"; exit 1; }

# --- where does this function live in the ROM? ---
# (python, not awk: BSD awk on macOS has no strtonum)
read -r ADDR SIZE < <(arm-none-eabi-nm -S "$ROOT/goldensun.elf" | python3 -c '
import sys
f = sys.argv[1]
for line in sys.stdin:
    p = line.split()
    if len(p) == 4 and p[3] == f:
        print(int(p[0], 16) & 0xFFFFFF, int(p[1], 16)); break
' "$FUNC")
[ -n "${ADDR:-}" ] || { echo "$FUNC not found in goldensun.elf -- build first"; exit 1; }

mkdir -p "$OUT"

# --- compile.sh: the project's real pipeline, nothing simplified ---
cat > "$OUT/compile.sh" <<EOF
#!/bin/bash
set -euo pipefail
IN="\$1"; OUT="\$3"
arm-none-eabi-cpp -I$ROOT/include -nostdinc -undef -std=gnu89 "\$IN" \\
  | $ROOT/tools/agbcc/bin/agbcc -O -mthumb-interwork -fhex-asm -fcall-used-r4 -o - \\
  | cat - <(printf '.text\n\t.align\t2, 0\n') \\
  | arm-none-eabi-as -mcpu=arm7tdmi -o "\$OUT"
EOF
chmod +x "$OUT/compile.sh"

# --- base.c: preprocess, then keep only the target function ---
# The register pins we use as matching aids -- `register int v asm("r3")` --
# are a GCC extension that pycparser cannot parse, so the permuter chokes on
# them outright. import.py's convention is to strip them (-D__asm__(...)=), so
# we do the same: the search runs over UN-PINNED source.
#
# That is a real limitation, not a workaround. The permuter rearranges source
# to influence register allocation; it will never invent a pin. So it can only
# help where a pin-free match exists. Anything it does find is more idiomatic
# than a pinned match, which is a fair trade.
arm-none-eabi-cpp -I"$ROOT/include" -nostdinc -undef -std=gnu89 \
    -D'__attribute__(x)=' -D'asm(x)=' -D'__asm__(x)=' "$SRC" > "$OUT/base.c"
python3 "$ROOT/../decomp-permuter-agbcc/strip_other_fns.py" "$OUT/base.c" "$FUNC" \
    2>/dev/null || echo "  (strip_other_fns declined; leaving base.c whole)"

# --- target.o -----------------------------------------------------------
# Both sides must be UNLINKED objects. base.o's literal pool holds relocation
# placeholders, so comparing it against final linked ROM bytes fails on the
# pool even when every instruction is identical -- which is exactly what the
# first version of this script did.
#
# So the target is assembled from the function's own .s, which `make compare`
# already proves is byte-identical to the ROM. If the function has no .s left
# (already converted to C), a reference .c can be given instead with
# --target-from-c, which is how a known-good function is used to validate the
# harness itself.
if [ "${4:-}" = "--target-from-c" ]; then
    "$OUT/compile.sh" "${5:?--target-from-c needs a file}" -o "$OUT/target.o"
else
    ASM=$(grep -rl "^\.thumb_func_start $FUNC\$" "$ROOT"/rom_*/src/*.s 2>/dev/null | head -1)
    [ -n "$ASM" ] || { echo "no .s defines $FUNC; pass --target-from-c <ref.c>"; exit 1; }
    python3 - "$ASM" "$FUNC" "$OUT/target.s" <<'PY'
import sys, re
src, func, out = sys.argv[1:4]
L = open(src, errors="replace").read().split("\n")
# keep the .include lines, then just this function
head = [l for l in L[:6] if l.lstrip().startswith(".include")]
i = next(k for k, l in enumerate(L) if l.strip() == f".thumb_func_start {func}")
j = next(k for k in range(i, len(L)) if L[k].startswith(".func_end"))
open(out, "w").write("\n".join(head + [""] + L[i:j+1]) + "\n")
PY
    (cd "$ROOT" && arm-none-eabi-as -mcpu=arm7tdmi -I"$ROOT" -I"$ROOT/include" \
        "$OUT/target.s" -o "$OUT/target.o")
fi

cat > "$OUT/settings.toml" <<EOF
func_name = "$FUNC"
compiler_type = "gcc"
EOF

printf 'ready: %s\n  %s at 0x%x, %d bytes\n' "$OUT" "$FUNC" "$ADDR" "$SIZE"
