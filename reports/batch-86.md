# Batch 86 — one function that settles the argument about batch 83's lever

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_903_2008d04` | `02008d04` | [ovl_314_c_a_c_b.c](../src/overlays/rom_798dc4/ovl_314_c_a_c_b.c) |
| `OvlFunc_898_200913c` | `0200913c` | [ovl_314_c_c_c_a_c_a_c_c_b.c](../src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_c_c_b.c) |
| `OvlFunc_939_20091d0` | `020091d0` | [ovl_314_c_a_c_c_b.c](../src/overlays/rom_7c460c/ovl_314_c_a_c_c_b.c) |
| `OvlFunc_898_2008fb4` | `02008fb4` | [ovl_314_c_c_c_a_c_a_c_a_b.c](../src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_c_a_b.c) |
| `OvlFunc_898_2009010` | `02009010` | [ovl_314_c_c_c_a_c_a_c_a_b.c](../src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_c_a_b.c) |

## Both answers, four instructions apart

Batch 83 found that naming a constant in a local of the width it is combined
with, and writing it first, decides which operand becomes the destination of a
two-operand `orr`. Batch 85 found a counterexample on an `and`. **This batch has
both cases inside one function**, which settles what kind of thing the lever is.

`OvlFunc_898_200913c`:

    rom   ldrb r2, [r5]     / mov r3, #0xfe / and r3, r2      the CONSTANT is rd
    rom   ldrb r3, [r6, #9] / mov r2, #0xc  / orr r3, r2      the VALUE is rd

The plain `*p &= 0xfe` and `m[9] |= 0xc` give exactly those, with nothing named.
Naming either constant would move the wrong one.

Meanwhile `OvlFunc_903_2008d04` in the same batch **does** need the narrow local
for its `orr` — the plain `*p |= 2`, `*p = 2 | *p` and an `int` local all put the
loaded byte in the destination where the ROM has the constant.

So: same lever, same instruction shape, opposite answers. **When the operands
are the wrong way round, try it. Never apply it on sight.**

## The built-constant filter is now register tracking

Batch 85 added a repeated-built-constant filter to `pick_candidates.py` matching
text pairs, then patched it once for the separated `neg` form. This round
`OvlFunc_947_200a4cc` slipped through anyway:

    mov r0, #0x81 / str r3, [r5, #0x14] / str r3, [r5, #0xc] / lsl r0, #2

— unrelated work between the `mov` and its shift, which no pair pattern catches.

It now **tracks registers**: remember the last `mov rN, #imm`, record the value
when that register is later shifted or negated, and forget a register when
anything else writes it. That is the shape of the problem rather than a shape of
its text, and all four known members of the class are filtered. 82 candidates.

Three patches to one filter in two batches is worth naming: the failure mode was
matching what the assembly *looks* like instead of what the compiler *did*.

## Where the local goes decides whether it matches

`OvlFunc_939_20091d0` stores `0x5b` into a `u16`, which pools it as a HImode
constant (`ldr r3, =0x5b`) where the ROM has `mov r3, #0x5b`. Batch 84's rule
says widen it with an `int` local — and that is only half the answer:

| local | result |
|---|---|
| `int v = 0x5b;` next to the store | 10 of 40 |
| `int v = 0x5b;` at the top of the function | **match** |

Assigned at the top it is live across the calls, which is what puts it where the
ROM has it. The same distinction decided `OvlFunc_901_2008864` in batch 83 and
`OvlFunc_928_2008968` in batch 85 — **the width says which type, the position
says where** — and both halves have to be read off the ROM.

## Two parks

`OvlFunc_898_20087ec` and its twin are 44 lines against 44 with the pool sitting
one 4-byte `bl` too early. The three spellings measured put it in three
different places:

| source | pool lands |
|---|---|
| `*p \|= 2` | after `ldrh r2, [r5]` |
| `two = (u16)(int)&_CONST_2` | before `bl __CutsceneEnd` |
| **rom** | **between them** |

The dump point is `create_fix_barrier`'s and it moves with the mode of the
pool's first entry — a wider reference lets gcc scan further before
manufacturing the barrier. There is no third mode to try. The symbol is still
right: it is the only spelling that gives both the pool load and the ROM's `orr`
operand order, which is what batch 83 established.

`OvlFunc_963_2008288` is 44 of 46 on argument precompute — `mov r2, #0x10 /
mov r1, #3 / neg r2, r2` against our `mov / neg / mov` — with seven spellings
recorded. Its two `2`s that feed both `__CopyMapTiles` calls **are** a named
local, because the ROM keeps the value in r5 across both; that half worked.
