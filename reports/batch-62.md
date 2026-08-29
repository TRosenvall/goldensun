# Batch 62 — a blocker diagnosed out of the compiler source, and three levers

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_801ff14` | `0801ff14` | main ROM | [rom_1fe2c_b.c](../src/rom_15000/rom_1fe2c_b.c) |
| `Func_80a9cbc` | `080a9cbc` | main ROM | [rom_a8604_c_c_a_c_a_b.c](../src/rom_a1000/rom_a8604_c_c_a_c_a_b.c) |
| `OvlFunc_939_2008ac4` | `02008ac4` | ovl_7c460c | [ovl_314_a_c_c_a_c_a_a.c](../src/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a_a.c) |
| `OvlFunc_946_2009624` | `02009624` | ovl_7ced6c | [ovl_30_c_c_a_c_c_c.c](../src/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_c.c) |
| `OvlFunc_964_2009038` | `02009038` | ovl_7ed0a0 | [ovl_30_a_a_c_c_a_a_c.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_c_c_a_a_c.c) |

Fourteen functions were parked. Two of the five above were **previously stuck**
— `OvlFunc_964_2009038` had sat at 9 of 26 in an earlier round.

## The headline is a blocker that will not be fixed from C

Eleven screened functions, every one within six instructions of matching, are
held by a single difference: where gcc materialises a cheap `mov rN, #imm` among
the other argument registers.

```
rom   mov r1, #0xe0 / mov r0, #0x1 / lsl r1, #0x8 / mov r2, #0x0
ours  mov r1, #0xe0 / mov r0, #0x1 / mov r2, #0x0 / lsl r1, #0x8
```

Nine source spellings and eight compiler flags were byte-identical to each
other. Rather than keep guessing, the answer was read out of the compiler in the
build image, `/opt/camelot-gcc/gcc-2.96/gcc/`:

- **`calls.c:805`** `precompute_register_parameters()` copies any argument whose
  `rtx_cost` exceeds 2 into a pseudo **before any hard register is loaded**. Its
  guard is `SMALL_REGISTER_CLASSES && reg_parm_seen`, and `reg_parm_seen` is set
  for argument *i* **before** argument *i* is tested — so it is already 1 on the
  first register argument. The guard reduces to the cost test alone.
- **`arm.h:1061`** `SMALL_REGISTER_CLASSES` is `TARGET_THUMB` — always 1 here.
- **`arm.c:2042`** In Thumb, `ASHIFT`/`PLUS`/`MINUS`/`NEG`/`NOT`/`COMPARE` all
  cost 4; pool loads and synthesised constants also exceed 2.
- **`calls.c:1684`** `load_register_parameters()` then loops **forward**.
  `LOAD_ARGS_REVERSED` is not defined anywhere in this tree.

Expensive arguments are hoisted ahead of the register loads, and the cheap
constant is emitted afterwards — landing last. The ROM's compiler did not
precompute: its stream is plain forward load order with constant synthesis left
in place, scheduled afterwards.

### The diagnosis was tested, not asserted

It predicts exactly which calls fail: **a call misorders when its argument list
mixes cheap constants with two or more expensive values and a cheap one is not
last; an all-cheap call matches.**

`OvlFunc_921_20099bc` happens to contain one call of each kind. Both predictions
held on the first screen — `__MapActor_SetSpeed(0, 0x20000, 0x1999)` misorders,
`__Func_8092158(0, 0xe8, 0xcc)` is byte-identical. It also explains this batch's
successes after the fact: every call in `OvlFunc_946_2009624` and
`OvlFunc_932_200aa10` passes only cheap constants.

**These eleven parks are not source problems and should not be retried as such.**
They sit with the `-fno-rerun-cse-after-loop` count as the second concrete piece
of evidence that the reference toolchain differs from Camelot's.

### A correction this forced

`-fno-schedule-insns` was never a real experiment, here or in any earlier batch.
**`arm.c:634` force-disables `flag_schedule_insns` whenever `TARGET_THUMB` is
set**, silently, *"since it's on by default in -O2"*. The first scheduler never
runs for any file in this project. Only `-fno-schedule-insns2` does anything.
Any earlier note claiming sched1 was ruled out by flag should be read as ruled
out by construction.

## Three levers, each from a function that was stuck

**`do/while` puts the conditional on the back edge.** When the ROM's
continue-test jumps backward and an unconditional falls out (`bgt .L103e / b
.L1056`), a forward `goto` will not do it — gcc inverts `if (w > v) goto loop;
goto join;` into `ble join / b loop`. Same instruction count, opposite shape.
Spelling it `do { ... } while (w > v);` was the last difference on
`OvlFunc_964_2009038`.

**The pointer-typed operand comes first in `[rA, rB]`.** Reversing the addition
in the source does **nothing** — gcc canonicalises pointer-plus-integer and the
output is byte-identical. What decides it is the *types*: making the walking
offset the pointer and the loaded base a plain `unsigned int` swaps the
operands. That was the whole difference on `Func_801ff14`. The opposite
direction — naming the sum so gcc computes an address and stores at offset zero
instead of using register-offset form — was needed by `OvlFunc_939_2008ac4`.

**A derived initialiser forces the pointer copy gcc coalesces away.** `q = p;`
followed by `q += 0x48;` gets coalesced and the ROM's `mov r5, r3` disappears;
`q = (unsigned char **)(p + 0x48);` in one statement makes the copy real.
`Func_80a9cbc`, 2 of 28 to exact.

Its limit is recorded with it, from a failure in the same round: `Func_8078550`
needs the identical copy and cannot get it, because there the second pointer
holds the *same* address rather than a derived one — the attempt made it worse
(8 of 27 to 10). **The initialiser must be an expression, not an alias.**

## A tool that was nearly a mistake

The precompute diagnosis went into `tools/pool_candidates.py` as a filter that
would have dropped 19 of 49 candidates. Before trusting it, it was validated
against 11 confirmed-blocked and 10 confirmed-matching functions — the matches
read out of their generated `.s`, which is byte-equal to the ROM:

| rule | blocked caught | matches **wrongly** flagged |
|---|---|---|
| ≥ 2 expensive args | 10 of 11 | **1 of 10** (`Task_BlitPreAnim`) |
| ≥ 1 expensive arg | 11 of 11 | **2 of 10** |

Right about 90% of the time and **wrong about 10%, in both directions**.
`Task_BlitPreAnim` would have been discarded, and it matched exactly.

A filter that hides a matchable function is worse than no filter, so it now
**ranks flagged candidates last and labels them** rather than removing them,
with those numbers in the docstring and the flag reading *"screen this later"*,
never *"impossible"*.

## `make clean` cannot be recovered inside Docker

Found the hard way while running this batch's own report gate.
`tools/agbcc/bin/{agbcc,agbcc_arm,old_agbcc}` are **Mach-O x86_64** — macOS host
binaries. The Linux container runs them as shell scripts:

```
tools/agbcc/bin/old_agbcc: 1: Syntax error: "(" unexpected
```

Five objects need them: `src/lib/m4a/{m4a,m4a_tables}.o` and
`src/lib/agb_flash/{agb_flash,agb_flash_mx,agb_flash_at}.o`. Rebuild those on
the macOS **host** and the container build completes; the exact commands are in
[HANDOFF.md](../HANDOFF.md). The `m4a` pair must be run by hand because macOS
make picks the generic `%.o: %.c` rule for them and reaches for
`tools/gcc296/xgcc`, which exists only inside the container.

`make compare` is byte-exact against `baserom.gba`, so a passing compare after
this recovery proves the host-built objects are right — nothing is taken on
trust. But the report gate's clean build is a five-minute recovery, not a no-op.

## Splitting a `.s`: the basename must be unique

A C file at `src/<path>/X.c` compiles to `asm/<path>/X.o` **and writes gcc's
assembly to `asm/<path>/X.s`**. Naming an elevated piece after the `.s` it was
split from makes gcc overwrite that `.s` mid-build. The symptom is misleading —
`multiple definition` and `undefined reference` in one message, which reads as a
stale object. Give every split piece a fresh name. `asmfacts.py --orphans`
cannot catch this; only the build does.

## Negative results worth not repeating

- **The basic-block lever does not reach global CSE.** Two functions were
  blocked by gcc sharing a constant the ROM materialises twice, and a separate
  local in a demonstrably different basic block produced **byte-identical**
  output both times. That lever acts on `local-alloc`'s `update_equiv_regs`;
  this sharing is global CSE, which runs earlier and ignores block boundaries.
  Two independent confirmations — treat it as settled.
- **The back-edge lever unlocks nothing already parked.** All 125 parks were
  swept for the ROM signature (a backward conditional immediately followed by a
  forward unconditional). **Zero** have that shape.
- **gcc does not assign registers in source order.** Swapping two loads to test
  it was byte-identical (`OvlFunc_969_200d9f0`).
- **The constant-as-destination lever is conditional in two ways.** It needs a
  literal operand — with two registers gcc canonicalises anyway
  (`Func_8006384`, byte-identical) — and it *hurts* inside branch arms
  (`OvlFunc_930_2009060`, 11 of 25 to 13). It applies to `MUL` as well as
  `AND`/`ORR`.
