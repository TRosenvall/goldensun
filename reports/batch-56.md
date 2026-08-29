# Batch 56 — six functions, and the size distribution turns over

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `GetSpriteVoice` | `080915ac` | main ROM | [rom_91584_a_c_a_a.c](../src/rom_8a000/rom_91584_a_c_a_a.c) |
| `LoadUIIcon` | `0802875c` | main ROM | [rom_23178_a_a_a_a_c_c_b.c](../src/rom_15000/rom_23178_a_a_a_a_c_c_b.c) |
| `OvlFunc_939_2008fa0` | `02008fa0` | ovl_7c460c | [ovl_314_c_a_b.c](../src/overlays/rom_7c460c/ovl_314_c_a_b.c) |
| `OvlFunc_947_200a63c` | `0200a63c` | ovl_7d0e88 | [ovl_2580_a_c_b.c](../src/overlays/rom_7d0e88/ovl_2580_a_c_b.c) |
| `OvlFunc_968_2009a50` | `02009a50` | ovl_7f2f14 | [ovl_30_c_a_c_c_c_a_c_b.c](../src/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_a_c_b.c) |
| `OvlFunc_common1_1fb4` | `0200b84c` / `0200bae4` / `0200c57c` | overlays/common | [common1_c_a_c_c_b.c](../src/overlays/common/common1_c_a_c_c_b.c) |

`OvlFunc_common1_1fb4` resolves at three addresses because `common1` is linked
into several overlays. All three were checked.

## The size distribution has turned over

Of **2,745** unelevated functions (m4a and generated excluded):

| Size | Count |
|---|---|
| ≤ 15 | **34** |
| 16–25 | 144 |
| 26–40 | **381** |
| 41–60 | 465 |
| 61–100 | 608 |
| > 100 | 1113 |

The small-function era is over. The round before this one was thin — one
elevation — because it kept fishing in a band that is essentially fished out,
and then spent its time reworking parks instead.

**So this batch worked the 26–40 band deliberately, and it went fine.** Four
functions from it, 28 to 36 instructions, all with accumulated levers and no new
ideas. `OvlFunc_968_2009a50` at 36 is the largest single function elevated in
several batches and needed three known levers applied together:

- the fallback call goes **last**, not as an early return — the ROM's `bne` jumps
  forward to it
- the mask is the AND's **destination**, giving the full 32-bit `~0xc` via
  `mov`/`neg` rather than gcc's narrowed `mov #0xf3`
- one zero, three stores, kept in `r5` across two calls

The band is not meaningfully harder. It just has more going on per function.

## Batch 48's constant-chain rule now has a test

That rule says an `add`/`sub` chain on a constant is usually gcc's own strength
reduction, so **try the literal form first**. Useful as a prior — but it reads
like a discriminator, and it is not one.

`OvlFunc_common1_15b8` settles it. Its ROM derives the second constant from the
first with `asr r3, #1`. Written as two literals, gcc emits a **fresh**
`mov`/`lsl` rather than the chain — so the shift belongs to the *source*, and
writing `v >>= 1;` takes it from 8 of 34 to 6.

> **The test: write the literals and see whether gcc produces the chain.**
> If it does, the chain is gcc's. If it emits fresh constants instead, the chain
> was in the source.

The mnemonic is not what decides it; who generates it is. `docs/elevation.md` now
leads with the test and demotes the `add`/`sub` observation to the prior it is.

## An over-broad claim narrowed

`sprite_flags_setter.c` (batch 50) states that `m &= v` collapses the mask to a
narrowed `mov` and only `v &= m` preserves `mov`/`neg`. **`OvlFunc_968_2009a50`
matches with exactly the `m &= f` form that fails there.**

The asymmetry is real in that function and is not a general law. The park now
says to try both forms, and records the untested hypothesis that the difference
is whether the OR'd-in value is computed or constant. Better to flag it as
unexplained than leave a rule that reads as universal.

## Smaller findings

- **`LoadUIIcon`**: the ROM uses the **index** as the load's base and the file
  pointer as its offset — the reverse of how `f + (idx << 1)` reads. One
  instruction of 31. Batch 48's base/offset lever used in the opposite
  direction: there an array-of-structs declaration made the *table* the base.
- **`OvlFunc_common1_1fb4`**: its stored `1` must be a named `int`, or gcc pools
  the constant and needs a `b` over the pool — 30 against 28. Third instance of
  the inverted narrow-constant tell, and the first where the pooled value is `1`
  rather than `0`. Its zero, separately, is `__GetFlag`'s **return value**
  reused.
- **`OvlFunc_939_2008fa0`**: shares its **second** stack slot across three calls,
  read off the `str r5` offset being `[sp, #4]` where batch 49's members shared
  `[sp]`. The `0xb` appearing as both an argument and the shared value is not one
  value — the ROM rebuilds the argument each time.
- **`OvlFunc_947_200a63c`**: the tree's first stack-arg pair whose values are
  **computed** rather than constant.
- **`GetSpriteVoice`**: two arms return zero and the ROM shares one block for
  both, placed **before** the non-zero arm. Naming the join is not enough — the
  block order is what `bne` against `beq` reports.

## Parked

- **`OvlFunc_964_2009458`** (+ twin `_20094ac`) at 3 of 36. Two fixes took it
  from 13: the `+0x62` store is a **destructive walk** (`p[0x62] = 0` keeps `p`
  live and costs two instructions), and the loaded byte needs its own local.
  What remains is the allocator choosing `r2`/`r3` the other way round.
- **`OvlFunc_common1_15b8`** at 6 of 34 — instruction order around one byte
  store, with the constant-chain finding recorded there.
- **`Func_80bd7a4`**, third member of the `dma.h` binding class: three identical
  transfers where the ROM rebuilds every argument and gcc sets them once. The
  asm operands are *inputs*, not clobbers, so gcc is correct about liveness and
  there is no source-level handle.
- **`Func_80198dc`**, the tree's first single-register `stmia` loop. The skeleton
  is right at 18 against 18; both reorderings tried made it *dramatically* worse,
  which says gcc schedules the loop body as a unit rather than following
  statement order.
