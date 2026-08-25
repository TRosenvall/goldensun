# Batch 61 — statement order, a lever with three outcomes, and a `make clean` that broke the build

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Task_BlitPreAnim` | `080c1438` | main ROM | [rom_c10e8_a_a_a_b.c](../src/rom_b5000/rom_c10e8_a_a_a_b.c) |
| `OvlFunc_908_20084c8` | `020084c8` | ovl_79c0c4 | [ovl_30_c_c_c_c_a.c](../src/overlays/rom_79c0c4/ovl_30_c_c_c_c_a.c) |
| `OvlFunc_932_200aa10` | `0200aa10` | ovl_7b9cb4 | [ovl_30_a_c_c_a_c_c_a_a_b.c](../src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_b.c) |
| `OvlFunc_968_2008558` | `02008558` | ovl_7f2f14 | [ovl_30_a_a_a_c_c_a_c.c](../src/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_a_c.c) |
| `OvlFunc_968_20085ac` | `020085ac` | ovl_7f2f14 | [ovl_30_a_a_a_c_c_c_a.c](../src/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_c_a.c) |

Nine functions were parked. Two of them sit at **2 and 3 instructions** of 24
and 25, which is close enough that they are listed as leads rather than dead
ends.

## Statement order, not declaration order

Three of the five matched because two assignments were **swapped in the source**.
Not reordered declarations — the declaration lever is a different thing and did
not help in any of these — but the order of the statements themselves.

`OvlFunc_908_20084c8` at 6 of 24 became an exact match when

```c
p = iwram_3001ebc;      k = 0xe0 << 1;
k = 0xe0 << 1;    →     p = iwram_3001ebc;
```

The ROM materialises the constant *before* it loads the global. Written the
other way round the two values land in swapped registers and every dependent
instruction inherits it — six instructions for nothing else.

`OvlFunc_968_2008558` at 2 of 26 became exact when the loop counter was assigned
before the table pointer, so that `mov r6, #8` precedes `add r5, #0x34`.

The rule that seems to hold: **when two independent values are set up before a
use, gcc materialises them in source order and assigns registers accordingly.**
That makes statement order a cheap first thing to try whenever a diff is nothing
but a register permutation in a straight-line prologue. It costs one screen.

## New lever: make the CONSTANT the destination of an AND/ORR

Thumb data-processing is two-operand and destructive. When the ROM has the
constant's register as the destination and gcc has the loaded value:

```c
m = 0xc & u;     →     m = 0xc;
                       m &= u;
```

**The reason this is worth more than the one instruction** is what happens to the
constant afterwards. With the value as destination, `0xc` stays live, and gcc
reuses it to build other nearby constants. `OvlFunc_957_200b610` needs the mask
`-13` four instructions later, and gcc emitted `sub r3, #0x19` (0xc − 0x19 =
−13) where the ROM has `mov r3, #0xd / neg r3, r3`. Making the constant the
destination kills it after the AND, and the derivation dies with it. It also
unblocked the instruction *order*: gcc had been hoisting two `ldrb`s together
and stopped. One spelling change took that function from **8 of 25 to 3**.

It is now documented in [docs/elevation.md](../docs/elevation.md) with all three
of its outcomes, because two of them are negative:

- **Needs a literal.** With two registers gcc canonicalises the AND regardless
  of which side the source names first. `Func_8006384` was **byte-identical**
  with and without the lever.
- **Hurts inside branch arms.** `OvlFunc_930_2009060` has the shape
  `mov r3, #0x2 / orr r3, r2` in each of two mutually exclusive arms; the
  compound form went from **11 of 25 to 13**.

Screen it, do not assume it. That caveat is the same one batch 57 recorded for
the read-modify-write spelling, and this is now a pattern worth naming: a lever
that works in straight-line code can be actively harmful in branch arms.

## The basic-block lever does not touch global CSE

Two functions this batch were blocked by gcc sharing a constant the ROM
materialises twice. The basic-block lever is the standard answer, and it did
nothing in either.

`Func_8096ab0`: a separate zero local, assigned *after* an early-return branch so
that it demonstrably sits in a different basic block from the other use. Output
**byte-identical**. `Func_80ad69c`: same result across three spellings.

The lever is grounded in `update_equiv_regs` in `local-alloc.c`, which runs late
and is block-local. This sharing is **global CSE**, which runs much earlier and
does not care about block boundaries. Two independent confirmations in
consecutive rounds is enough to treat this as settled rather than retested.

## Two parks worth returning to

`OvlFunc_957_200b610` — **3 of 25, all one register.** The pointer read from
`actor + 0x50` is r4 in the ROM and r0 for us. The ROM's prologue is
`push {r5, lr}`: it **uses r4 without saving it**, clobbering its caller's
callee-saved register. gcc will not do that from any spelling — forcing the
value live across a call does get a callee-saved register, but gcc pushes it
too, and 3 becomes 7. Not the scheduler; `--no-sched2` is byte-identical.

`OvlFunc_930_2008870` — **2 of 24, argument-setup scheduling.** The ROM slots
`mov r0, #0xe` between two argument bases and their two shifts. Five spellings
were tried, including source order matching the ROM exactly, the declaration
lever, and compound shifts. **All five are byte-identical to each other.** gcc
fixes argument-setup order after the source has any say. `--no-sched2` makes it
*worse* (6 of 24), so the scheduler is wanted here.

Both are register-level residue on otherwise exact streams. They are the kind of
thing a compiler-version difference would explain, and they belong with the
`-fno-rerun-cse-after-loop` count in HANDOFF.md as evidence for that.

## Operational: `make clean` breaks the build, and the fix is on the host

**This is the important finding of the batch for anyone else running this loop.**

`make clean` deleted objects that **cannot be rebuilt inside the Docker image**.
`tools/agbcc/bin/` holds `agbcc`, `agbcc_arm` and `old_agbcc`, and they are
**Mach-O x86_64** — macOS host binaries. The Linux container runs them as shell
scripts and reports:

```
tools/agbcc/bin/old_agbcc: 1: Syntax error: "(" unexpected
```

Five objects need them: `src/lib/m4a/m4a.o`, `m4a_tables.o`, and
`src/lib/agb_flash/{agb_flash,agb_flash_mx,agb_flash_at}.o`. Their rules are
`gcc -E` → `old_agbcc` → `arm-none-eabi-as`, and the macOS host has all three.

**Recovery**, which is now the documented procedure:

```sh
make src/lib/agb_flash/agb_flash.o src/lib/agb_flash/agb_flash_mx.o \
     src/lib/agb_flash/agb_flash_at.o          # on the macOS HOST
for f in src/lib/m4a/*.c; do b=${f%.c}
  gcc -E -nostdinc -Itools/agbcc/include -Iinclude -D PLATFORM_GBA=1 \
      -D M4A_SIGNED_CHAR $f -o $b.i
  tools/agbcc/bin/old_agbcc -Wimplicit -Wparentheses -fhex-asm \
      -mthumb-interwork -O2 -o $b.s $b.i
  printf '\n\t.text\n\t.align\t2, 0\n' >> $b.s
  arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $b.o $b.s
done                                            # on the macOS HOST
```

then the Docker build completes and `make compare` passes. The `m4a` loop is
spelled out because on macOS `make` picks the generic `%.o: %.c` rule for those
targets rather than the `src/lib/m4a/%.o` one, and reaches for
`tools/gcc296/xgcc`, which only exists inside the container.

The compare passing is the proof the host-built objects are right — the output
is byte-compared against `baserom.gba`, so there is nothing to take on trust.

**The standing advice changes:** the report gate says "only after a clean
`make clean && make compare`". That is still right, but it is a *five-minute
recovery*, not a no-op. Do not run `make clean` casually mid-round.

## Splitting a `.s`: the basename must be unique

A C file at `src/<path>/X.c` compiles to `asm/<path>/X.o` **and writes gcc's
assembly to `asm/<path>/X.s`** (Makefile rule `asm/%.o: src/%.c`). Naming an
elevated piece after the `.s` it was split from makes gcc **overwrite that `.s`
mid-build**. The symptom is misleading — it reads as a stale object:

```
multiple definition of `OvlFunc_968_20085ac'
undefined reference to `OvlFunc_968_20085e4'
```

Both halves come from one cause: the asm file now holds gcc's version of the
first function and nothing else. Give every split piece a fresh name, delete the
original, and reference all pieces under `asm/` in the linker script.
`asmfacts.py --orphans` cannot catch this; only the build does. Now in
[docs/elevation.md](../docs/elevation.md).

## Tooling

`tools/pool_candidates.py` replaces the scratch script that had been driving
candidate selection. It ranks unelevated functions **every one of whose callees
already appears in elevated C**, which removes wrong-signature diffs — the most
expensive kind of wrong answer — from a round entirely.

It now also excludes functions that are **themselves already parked**. The old
version filtered parked *callees* but not parked *targets*, and handed me
`Func_80bd7a4` this batch despite its park note (dma.h register binding) being
three batches old. 121 parked functions excluded; 58 candidates remain in the
23–30 instruction window.
