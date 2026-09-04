# Batch 212

Five elevated, one parked. The batch found a **second, cost-free cure for the
crossed case**, and parked a function whose own twin is elevated from the same C.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_948_200941c` | `0x0200941c` | [ovl_30_…_c_a_b.c](src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_c_c_c_c_c_c_a_b.c) |
| 2 | `OvlFunc_969_200cb28` | `0x0200cb28` | [ovl_314_…_c_c_b.c](src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_b.c) |
| 3 | `OvlFunc_890_2009264` | `0x02009264` | [ovl_30_…_c_a_b.c](src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_a_b.c) |
| 4 | `OvlFunc_899_2009a4c` | `0x02009a4c` | [ovl_30_…_a_c_b.c](src/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_a_c_b.c) |
| 5 | `OvlFunc_959_200a5f8` | `0x0200a5f8` | [ovl_9dc_c_c_a_a_a_c_b.c](src/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_b.c) |

Parked: `OvlFunc_948_2009308` at 4 of 58, length exact.

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## A CROSSED SITE CLOSED WITHOUT A BARRIER

`200a5f8`'s opening fill is crossed — the ROM's movs run r0, r1, r2 while its
shifts run r1, r2, r0, so only r0 is out of place. The documented cure is a
volatile asm after `q0`, and it does fix the movs.

**It also costs the frame adjustment.** This function passes two stack arguments,
so its prologue carries a `sub sp, #8` with **six body instructions hoisted above
it**. A volatile asm at the top of the body is a full scheduling barrier, the
hoist doesn't happen, and the score goes from 3 of 65 to 7. Moving the barrier
after all three movs doesn't help. The two defects are in tension and the barrier
cannot resolve both.

What works costs nothing — **write the shifts in the movs' order**:

    rom     mov r0 / mov r1 / mov r2 / lsl r1 / lsl r2 / lsl r0
    source  q0 = K; q1 = K; q2 = K; q0 <<= a; q1 <<= b; q2 <<= c;

The movs are slaved to the shift order *as written in the source*, so ordering
the shifts the way the movs need makes the movs come out right. sched2 then lands
the shifts in the ROM's order on its own, and nothing blocks `sub sp`.

**Try this before reaching for a barrier.** It can't reach every crossed site —
the barrier still closes the three-register fills where two movs are out of place,
and the negation forms — but it has no side effects. A `sub sp` in the prologue is
a positive reason to prefer it, and the tell for the conflict is a residue that
gets *worse* when the barrier goes in, with `sub sp` leading the diff.

## TWO SPELLINGS THAT ARE NOT OPTIONAL

`200941c` is a small guard function that needed both.

**A tile coordinate is a signed DIVISION, not a shift.** The ROM spends five
instructions per coordinate — `cmp #0 / bge / ldr =0xfffff / add / asr #20` —
which is exactly gcc's expansion of `x / 0x100000` for a signed int. Writing
`>> 20` gives the unbiased three-instruction form and cannot match. **The bias
constant in the pool is the tell.**

**A range test takes the unsigned-offset idiom.** `ty >= 0x10 && ty <= 0x12`
emits two compares and two branches; `(unsigned)(ty - 0x10) <= 2` gives the ROM's
`sub / cmp / bhi`. Its twin has **both** forms in one function — a three-value
range as the subtract, a two-value range as two compares — so neither is "the"
translation and the listing decides.

## A CHAIN OF FOUR DERIVED CONSTANTS THROUGH ONE REGISTER

`2009264` writes two words into the iwram block:

    ldr r1,[r3] / mov r3,#0xe0 / lsl r3,#1 / add r2,r1,r3 /
    sub r3,#0xc0 / str r3,[r2] / add r3,#0xc8 / add r2,r1,r3 /
    mov r3,#0x20 / str r3,[r2]

r3 is an offset (0x1c0), then the value stored there (0x100, by subtracting from
the offset), then the next offset (0x1c8, by adding to the value), then the next
value. Four constants each derived from the last, all in one register. Separate
variables per role make gcc materialise each independently; one variable stepped
through all four falls out.

All three registers of that block are pinned. Worth noting after last batch's
hazard: **pinning is safe here because there is no call inside the block**, so no
pinned live range crosses a `bl`.

## THE STORE-WIDTH POOLING KEEPS RECURRING

Three of the five wanted a stored constant assigned to a local as its own
statement to keep it out of the pool — values of `1`, `3`, `0x5c`, all of which
fit an 8-bit immediate several times over. It is the store *width*, not the
magnitude.

`2009a4c` adds the ordering half: the ROM advances the pointer **first** and
builds the value second, and writing those two statements the other way round
puts the `mov` first.

## THE PARK: A TWIN WHOSE SIBLING IS ELEVATED

`2009308` is `200941c` with five constants changed. The same C matches for one
and sits at 4 of 58 for the other, with the length exact and **nothing wrong but
which callee-saved register two locals get**.

Six forms measured. Declaration order, computation order and the bound's type are
all inert at 4. Assigning the pointer earlier gives 18. **The pin is actively
destructive**: pinning the coordinate to the ROM's register gives 24 and a
seven-instruction-short function, because that value is *computed* by a
bias-and-shift sequence rather than moved — naming the destination rewrites the
sequence instead of relocating its result.

That is the useful boundary. In `2009060` last batch, pinning two competing
callee-saved locals settled them; there each was a single `mov` of an immediate
with nothing upstream. **The pin relocates a value that is written; it cannot
relocate one that is built.**
