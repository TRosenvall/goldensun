# Batch 214

Five elevated across two rounds, three parked. Two of the five were **parks
unblocked by reading the generated assembly**, and the batch bounds two claims
this tree had been carrying.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_936_200b768` | `0x0200b768` | [ovl_30_…_a_c_b.c](src/overlays/rom_7c097c/ovl_30_c_c_c_c_a_c_b.c) |
| 2 | `OvlFunc_932_200af10` | `0x0200af10` | [ovl_30_a_c_c_a_c_c_c.c](src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_c.c) |
| 3 | `OvlFunc_890_2009380` | `0x02009380` | [ovl_30_…_a_c_b.c](src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_a_c_b.c) |
| 4 | `OvlFunc_955_2008b38` | `0x02008b38` | [ovl_30_c_c_c_c_a_b.c](src/overlays/rom_7ddb88/ovl_30_c_c_c_c_a_b.c) |
| 5 | `OvlFunc_943_20099c0` | `0x020099c0` | [ovl_30_…_c_a_b.c](src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c_a_b.c) |

Parked: `OvlFunc_947_200a384` (44 of 100), `OvlFunc_932_20086dc` (35 of 100),
`OvlFunc_935_20089c0` (61 of 94).

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## THE .s ANSWERED THE SECOND SPILL PARK TOO

`200b768` was parked on "one more callee-saved register than the ROM", with the
park noting that scoping the locals was byte-identical and so was not the cause.
One `xgcc -S` and a grep:

    ldr r6, .L9+16 / mov r0, r6 / ... / mov r0, r6
    ldr r5, .L9+28 / mov r0, r5 / ... / mov r0, r5

Both repeated flag ids cached in callee-saved registers where the ROM reloads
them — the ordinary rematerialisation lever wearing a pressure disguise. Four r0
pins took **66 of 101 to 10 of 99**, and pinning the flag-merge block's four
registers closed the rest.

That is the second park in two rounds where reading the generated assembly
answered in minutes what reasoning about liveness had got wrong. It is the
method now, not a one-off.

## TWO CLAIMS BOUNDED

**The barrier-free crossed cure is not always sufficient.** Batch 213 recorded
that no case had yet appeared where the reordering was tried and a barrier was
still required. `2008b38` is that case, and it is a clean two-state trap:

    shifts in the movs' order    movs right, shifts transposed   2 of 63
    the ROM's literal order      shifts right, movs transposed   2 of 63

Neither state is the ROM. A barrier on the first mov resolves the pair. The
reordering stays the cheap first try; where it reaches only one of the two
orders, the barrier is still what settles it.

**A constant reused in two *different* argument registers still gets cached.**
On `200af10`, `0x9999` is r0 at one call and r1 at another, and gcc hoisted it
anyway. The rematerialisation lever is usually described for a value reused in
the same register; the register is not the point, the reuse is.

## A POOLED CONSTANT THAT FITS AN IMMEDIATE IS A LINKER SYMBOL

`200af10`'s last differing instruction was `ldr r0, =0x56` against our
`mov r0, #0x56`. `0x56` fits an 8-bit immediate three times over, so cost cannot
explain the pool. `area.sym` has `_AREA_56 = 0x56`, and a sibling in the same
overlay already spells that call as `__Func_8091f90((int) (&_AREA_51), 0x63)` —
with its own note pointing at a third file for why it has to be a symbol.

**When a ROM pools a constant an immediate would hold, look for a symbol with
that value** before treating it as a gcc quirk. The `.sym` files are the place,
and the overlay's siblings will usually show the idiom.

## THE PIN HAZARD, MILDER FORM

Batch 210 recorded that a pin assigned before a call and used after it is
**dropped**. On `20099c0` the same setup merely **relocates** them: pins assigned
around a store whose expression contains a call get moved below the store, 46 of
79. Fetching the actor into its own statement first — so the pins' live range
holds no `bl` — gives the ROM's order. Same cause, two severities.

## WHERE A MACRO IS SAFER THAN TRANSCRIPTION

`2009380` has five identical do-while loops differing only in a limit and a wait.
A **loop** over a table would not match — the ROM emits all five bodies — but
writing fifty near-identical lines by hand invites a typo. A macro expanded five
times gives the ROM's five copies with the parameters visible.

The distinction is whether the repetition is in the **output** (a macro is fine)
or only in the **source** (a loop changes the output). This is the first case in
the tree where that line falls on the macro side.

## THE PARKS BRACKET WHAT A PIN CAN BUY

Two of the three parks are about the same tool from opposite ends, and they are
worth reading together.

On `20086dc` **the pin is necessary and costs a loop guard**: pinning the counter
is what puts it in the ROM's register, and it is also what stops gcc proving the
first iteration runs, so an entry test appears that the ROM does not have. Three
spellings measured; each fixes what the other breaks. The idea that the counter
might *inherit* the register from a pinned base was tried this batch and fails —
82 and 66 differing against the pinned form's 35.

On `20089c0` **the pin is harmful**: the ROM copies a fetched pointer with
`mov r5, r0` and stores through the copy, and a pin lets gcc store through the
return register and drop the copy, so the function comes out a line short —
70 differing against 63 unpinned. A pin says *where* a value lives; it cannot say
that it must be **copied** rather than used in place.

`200a384` refines an existing entry instead: a stack-argument pair needs two
named locals **pinned to the ROM's scratch registers**, not merely two named
locals. As ordinary locals they take callee-saved ones and widen the prologue.
That fixed the first of its two sites; the second commons a constant the ROM
builds twice, between a stack and a register argument of one call, which nothing
in the tree yet defeats.
