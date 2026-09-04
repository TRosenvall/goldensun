# Batch 203

Five elevated, four parked across two files. The batch has one new lever and one
newly-bounded wall, and they point in opposite directions.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_899_20099a4` | `0x020099a4` | [ovl_30_a_c_c_c_a_c_a_a.c](src/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_a_a.c) |
| 2 | `OvlFunc_938_200940c` | `0x0200940c` | [ovl_30_…_c_a_b.c](src/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_a_b.c) |
| 3 | `OvlFunc_939_2008b0c` | `0x02008b0c` | [ovl_314_a_c_c_a_c_a_b_a.c](src/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a_b_a.c) |
| 4 | `OvlFunc_953_200a904` | `0x0200a904` | [ovl_30_…_c_c_b.c](src/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_c_b.c) |
| 5 | `OvlFunc_909_2008150` | `0x02008150` | [ovl_30_c_c_a_a_c_a.c](src/overlays/rom_79c738/ovl_30_c_c_a_a_c_a.c) |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## AN UNSIGNED COUNTER BLOCKS GCC'S LOOP REVERSAL

The palette-rotation pair parked this batch gave up a lever on the way. Written
with a signed `int` counter, gcc **reverses the loop** — `mov r2, #6` and
counting down with `sub` — where the ROM counts up from zero and compares:

    rom     mov r0, #0 ... add r0, #1 / cmp r0, #6 / bls
    int     mov r2, #6 ... sub r2, #1
    uint    mov r2, #0 ... add r2, #1 / cmp r2, #6 / bls     <- matches

Changing only the counter's **type** restores the ROM's direction. Everything
else was inert:

- five source spellings — `do/while`, `for`, `while`, `while (++i <= 6)`, and
  `i < 7` instead of `i <= 6`
- four flags — `-fno-strength-reduce`, `-fno-unroll-loops`,
  `-fno-rerun-loop-opt`, `-fno-expensive-optimizations`

Nine measurements against the loop's *shape* and its *flags*, all identical, and
a one-word type change removed the reversal. **The lever is the type, not the
loop.**

> ### Correction, batch 204
>
> **The unsigned-counter finding above is not new, and this section overstated
> what was done.** `src/non_matching/ovl_7ac2d8/200adcc.c` — a park for the
> twin of the function this was measured on — has existed since August and
> already records it, with a better statement of the tell: the ROM's `bls` is
> itself the evidence, because an unsigned branch on a loop counter means the
> counter is unsigned. It was derived here a second time without looking.
>
> That park is also **better**: it screens at 5 of 24 where the batch-203
> candidate screened at 9 and 10, because it additionally knows that assigning
> the counter *before* the source pointer is worth four instructions. And its
> diagnosis of the remaining defect is different and correct — **constant
> derivation**, gcc deriving the source pointer from the save target rather
> than taking a third pool entry, not the register-role rotation claimed below.
>
> The batch-203 park has been rewritten to carry the better body and defer to
> the older one. The claim that it "parks both functions" was also wrong: a
> separate park for the twin already existed.
>
> The cause was a process failure, not a compiler one — these two were triaged
> out of `shape_groups.py` and `src/non_matching` was never grepped for their
> names first. The nine spellings and four flags reported above were real
> measurements, but they were spent re-deriving something already written down.

## A SECOND PLACE THE PIN DOES NOT REACH

Both parks come down to the same thing, and it is not placement:

- `2008ef4` (4 of 30) — the ROM puts the second parameter in r6 and the first in
  r5; gcc does the opposite, and four lines follow from two swapped movs.
- `200a648` (9 of 24) — the ROM keeps the source pointer in r2 and the counter in
  r0; gcc reverses that, and the whole loop body follows.

These are **allocation-order** questions. Pinning was tried on `2008ef4` in four
forms and is completely inert: gcc coalesces the pinned local with the incoming
parameter and then allocates as it pleases.

That is consistent with the boundary already recorded in
`src/non_matching/rom_c0/rom_64b8.c`, and worth stating now that the pin has
carried ten batches: **a pin decides which register a value is written into and
where that write sits among other pinned writes. It does not override the
allocator's choice for a value that is not being written where the pin names.**

## ARGUMENT ORDER VARIES WITHIN A SINGLE CALL SITE'S NEIGHBOURS

`OvlFunc_939_2008b0c` calls `__MapActor_SetAnim` three times:

    mov r1, #1 / mov r0, #0      <- reversed, pinned
    mov r0, #0 / mov r1, #2      <- natural, written plainly
    mov r1, #1 / mov r0, #0      <- reversed, pinned

The first and third are the *same call with the same arguments*. The middle one
is filled the other way and would be broken by a pin. `OvlFunc_909_2008150` has
the same in another form: its two `__Func_8093040` calls take gcc's natural
order while the neighbouring `__Func_809280c`, same shape and mostly the same
values, is filled r1, r2, r0.

The inconsistency is in the ROM, so each call is read off the listing rather
than made consistent with its neighbours.

## Two twins recorded, not re-screened

`OvlFunc_901_2008a80` is instruction-for-instruction identical to the parked
`2008ef4`, and `200adcc` to `200a648`. Both were read and deliberately not
attempted — the same C gives the same residue, and screening would only
re-measure the wall. Each park says so and notes that solving one solves both.

## Process note

The `make compare` and address check for the fifth function were issued in the
same command as the clean rebuild, which was backgrounded — so they were not
*observed* before that commit was made. Both passed, and the clean rebuild that
followed is a strictly stronger check, so nothing is wrong with the result. But
the gate is meant to be read before committing, not after, and batching it with
a long-running job defeated that.
