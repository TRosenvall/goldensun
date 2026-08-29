# Batch 55 — six functions, and two new ways to read a diff

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_801d980` | `0801d980` | main ROM | [rom_1ca1c_c_c_a_b.c](../src/rom_15000/rom_1ca1c_c_c_a_b.c) |
| `Func_80284dc` | `080284dc` | main ROM | [rom_23178_a_a_a_a_c_b.c](../src/rom_15000/rom_23178_a_a_a_a_c_b.c) |
| `Func_80c08a8` | `080c08a8` | main ROM | [rom_bffb8_a_c_a_b.c](../src/rom_b5000/rom_bffb8_a_c_a_b.c) |
| `OvlFunc_964_20092b0` | `020092b0` | ovl_7ed0a0 | [ovl_30_a_a_c_c_c.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_c_c_c.c) |
| `OvlFunc_965_2008fac` | `02008fac` | ovl_7ef4f4 | [ovl_30_a_a_c_c_c.c](../src/overlays/rom_7ef4f4/ovl_30_a_a_c_c_c.c) |
| `OvlFunc_967_2008084` | `02008084` | ovl_7f21b8 | [ovl_30_c_c_a_b.c](../src/overlays/rom_7f21b8/ovl_30_c_c_a_b.c) |

**Two of the six were unparked**, and neither needed anything new — only a
different reading of what their diff meant.

## A shorter stream is a signature, not a curiosity

If a screen reports **fewer** instructions than the ROM, gcc found something
cheaper than the original compiler did. That is almost always a **rewrite of the
source shape**, not allocation noise — so it points at a cause, and the parked
set can be sorted by it.

Sweeping every park this way found **16 where ours is shorter**. Two came out on
the first thing tried.

`OvlFunc_964_20092b0` and `OvlFunc_965_2008fac` both held:

```c
if (area == X) return script;
return 0;
```

14 lines against 15. The ROM sets the default **before** the compare and
overwrites it in the matching arm:

```
ldr r3, =<area id> / mov r0, #0 / cmp r2, r3 / bne <out> / ldr r0, =<script>
```

So the source assigns `p = 0;` unconditionally and then replaces it. **Longer
usually means a blocker; shorter usually means the source said something more
clever than the original did.**

## A compound condition fuses — split it into statements

Two range tests written as one condition become a single unsigned comparison:

```
if (v > 0x11 || v < 0xf)   ->   sub r3,#0xf / lsl r3,#16 / cmp r3, 0x20000 / bls
```

Two instructions longer than the ROM's obvious pair of `cmp`s. Split into two
`if`s with a `goto`, `OvlFunc_899_2008048` goes from **16 of 22 to 2 of 22**, and
`Func_80a3ce4` from 11 lines to 12.

This is the **same lever** batch 53 found for the non-zero idiom, where `!= 0`
gets rewritten to a shift and only a statement-level `if`/`return` produces gcc's
branchless sequence. Two independent findings now point at it, so it is stated in
`docs/elevation.md` as a cue: *"gcc replaced my arithmetic with something
cleverer"* means **add a statement boundary**, not rename an intermediate — which
is the reflex the rest of the doc trains.

## New blocker class: gcc rewrites a signed LOWER bound

Both of those functions then stop at exactly **2 lines**, on the same thing:

```
rom    cmp r0, #0xc4 / bgt <out>     upper bound -- MATCHES
       cmp r0, #0xc1 / blt <out>     lower bound
ours   cmp r0, #0xc4 / bgt <out>
       cmp r0, #0xc0 / ble <out>
```

gcc-2.96 canonicalises **every** signed lower-bound test to `cmp #(K-1) / ble` —
`v < K`, `v <= K-1`, and an inverted `v >= K` all give identical output, and the
operand's type does not change it. It leaves **upper** bounds exactly as the ROM
has them.

**That one-directionality is what makes it a class rather than noise.** It
predicts a 2-line floor: if the only remaining difference is a lower bound, check
for `cmp #(K-1) / ble` against `cmp #K / blt` before spending another round.

## `StartTask` left undeclared, three times in two batches

`Func_801d980`, `Func_80284dc` and `Func_80958a8` (batch 54) all leave it
undeclared. Declared, gcc emits its `ldr r0, =<task>` one instruction early,
ahead of the pooled priority rather than after it.

Three instances makes it a property of that callee's call shape — both arguments
are pooled or shifted, so declaring it moves `r0` ahead of `r1` — rather than
something to rediscover per function.

## The `dma.h` experiment, and where it landed

Batch 54 parked two functions on `include/dma.h` binding `r0`–`r3` by name, and
suggested a helper taking its registers as parameters. **That was tried.**

Declaring the destination a read-write operand (`"+l"`) **does** remove the spill
that costs `Func_80a22f4` an instruction — 13 lines to 12, with the ROM's
`add r1, #0x1c` in place. But the remaining three differences are constant
strength-reduction, untouched by it.

The variant was **reverted rather than left unused in a shared header**, with the
exact change written into the park so it is one step for anyone who wants it. The
sibling park was **corrected**: it had speculated the same variant would help, and
it does not — `OvlFunc_914_2008c0c` needs gcc's *partial tail merge*, where the
count of `ldr r3, =REG_DMA3SAD` is the count of `DMA3_COPY` calls, so C can
express 21 lines or 24 but not the ROM's 22.

## Also

- `OvlFunc_967_2008084` loads its flag id **once**, so it is not the constant-CSE
  shape and needed no rule — unlike its neighbour `OvlFunc_967_20084b0` from
  batch 52, which loads the same id twice on mutually exclusive arms and also
  needed no rule, for a different reason.
- `Func_80c08a8` reads its iwram pointer **before** the DMA, kept in a pushed
  callee-saved register — the batch-49 tell.
