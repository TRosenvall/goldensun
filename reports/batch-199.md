# Batch 199

Five elevated, all five members of the `arg_interleave_flat` class, all five
matched on the **first screen**.

This batch is follow-through, not discovery. Batch 198 established that the
class is reachable by pinning; this one works the list. The only new observation
is at the end, and it is about how the class was *found* rather than how it is
solved.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_883_2008f5c` | `0x02008f5c` | [ovl_30_…_a_a_c_b.c](src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a_c_b.c) |
| 2 | `OvlFunc_883_2008f8c` | `0x02008f8c` | [ovl_30_…_a_a_c_c.c](src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a_c_c.c) |
| 3 | `OvlFunc_884_200881c` | `0x0200881c` | [ovl_30_…_c_c_a.c](src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_c_c_c_a.c) |
| 4 | `OvlFunc_884_20088ac` | `0x020088ac` | [ovl_30_…_c_c_c.c](src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_c_c_c_c_c.c) |
| 5 | `OvlFunc_921_20099bc` | `0x020099bc` | [ovl_30_…_c_a_b.c](src/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_a_b.c) |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## The class, worked

The shape is unchanged from batch 198 — `mov r0` written inside another
register's two-instruction build:

    mov r1, #0xa3 / mov r0, #0 / lsl r1, #1 / ldr r2, =0x466

Pinning the argument registers and assigning them in the ROM's order places it.
Four of this batch's five differ from each other only in three constants and a
`.L` table label; the C is the same eight lines each time.

**Eight of fourteen are now done, every one on its first screen** — `200bdec`
(batch 194, before the note was re-read), three in batch 198, five here. **Six
remain**, and there is no reason to expect them to behave differently.

Worth stating plainly: nothing was learned about the compiler in this batch. The
work was reading five ROM listings and writing down what they say. That is what
a resolved class should look like, and it is the argument for spending the effort
to resolve one rather than picking functions off a ranked list.

## Shape grouping found a real class without knowing what the functions do

`OvlFunc_921_20099bc` is not like the others. It opens a cutscene and runs a map
transition; the rest play a sound and blit a table. Its body has nothing in
common with them.

The class was defined by its **opcode set** — `{bl, bx, ldr, lsl, mov, pop,
push}` over functions with no branches — and says nothing whatever about
behaviour. It still grouped this function correctly, because the blocker is a
property of the instruction sequence and not of what the sequence is for.

That is a point in favour of the grouping method the class note used, and it is
worth keeping now that the note's *conclusion* has been struck: **the way the
class was found was sound, and only the verdict on it was wrong.**

## Housekeeping

`src/non_matching/overlays/arg_interleave_flat.c` now carries the running tally
and the six remaining names. The original member list is preserved in place
rather than edited away, so the note still reads as the record of what was
believed when it was written.

Two functions in one `.s` were both class members, so that file was split once
and both halves became `.c` — the first time in these batches that a split
produced two elevations rather than one plus a remainder.
