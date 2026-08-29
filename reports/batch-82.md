# Batch 82 — three levers, one of them a correction to batch 80

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_882_200a0fc` | `0200a0fc` | [ovl_30_c_c_c_c_a_a_a_c_c_b.c](../src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_b.c) |
| `OvlFunc_953_200ab1c` | `0200ab1c` | [ovl_30_c_c_c_c_b.c](../src/overlays/rom_7d95dc/ovl_30_c_c_c_c_b.c) |
| `OvlFunc_909_20084ec` | `020084ec` | [ovl_30_c_c_c_c_c_c_b.c](../src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_b.c) |
| `OvlFunc_953_200960c` | `0200960c` | [ovl_30_c_c_c_a_a_c_a_b.c](../src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a_b.c) |
| `OvlFunc_963_2008730` | `02008730` | [ovl_30_c_c_a_c_b.c](../src/overlays/rom_7ec968/ovl_30_c_c_a_c_b.c) |

Every one of the five keeps its literal pool **inside** the function — the class
`pick_candidates.py` refused to show until batch 79 — and every one went to
`make compare` rather than being trusted from the screen.

## Batch 80's label technique was heavier than it needed to be

`OvlFunc_882_200a0fc` reads a four-byte `.bss` slot the overlay declares as

    .global .L57fc
    .lcomm  .L57fc, 4

whose name is not a C identifier. I renamed it and updated all five references,
the way batch 80 renamed a data label — **and the build said no.** Two
already-elevated files in the same overlay reference it as

    extern unsigned int L57fc __asm__(".L57fc");

gcc's asm-label extension gives a declaration a link name that need not be a
valid identifier, and reaches the symbol **without touching any other file**.
The tree was already using it before batch 80 wrote its rename note; I did not
look.

`docs/elevation.md` now leads with the asm label. The rename is kept as the
heavier alternative — worth doing when a symbol has exactly one reference and a
real name is genuinely more informative, **not** worth doing to make a reference
possible. Batch 80's two files carry a pointer to the correction.

## A named local used ONCE can cost the preferred register

The standing lever says naming an intermediate forces the three-operand form and
generally helps. **The converse is also real**, and it shows up as a clean
register transposition rather than as extra instructions:

```c
base = *(char **)iwram_3001ebc;
*(int *)(base + (0xe0 << 1)) = 0x201;                      /* 6 of 43 differ */

*(int *)(*(char **)iwram_3001ebc + (0xe0 << 1)) = 0x201;   /* matches */
```

`REG_ALLOC_ORDER` (arm.h:989) starts `{3, 2, 1, 0, ...}`, so r3 goes to whichever
pseudo the allocator ranks first. Giving the base pointer its own named local
raises the offset constant's rank instead, and the ROM's r2/r3 assignment
inverts.

Four other spellings of the same store — operand order swapped, the destination
named, the offset named, the offset written as the folded literal `0x1c0`, an
`int *` base indexed `[0x70]` — all give the identical six differences, and so
do `-fno-gcse`, `-fno-rerun-cse-after-loop`, `-fno-schedule-insns2` and `-O1`.
**Only deleting the local moved it.**

The corollary is in the note too: **do not delete a local the ROM's own code
implies.** The same function reads `iwram_3001ebc` a second time and there the
local stays, because the ROM holds `&iwram_3001ebc` in r5 across every call and
re-loads the pointer — two separate reads of a global, not one cached value.

## The second declaration lever, used in anger

`OvlFunc_963_2008730` came out 47 against 47 with two differing: the argument
fill order of `__Func_8092c40(9, 0)`. The ROM fills r1 then r0.

This is the **second** lever from `elevation.md` — leaving the *mismatching*
call implicit puts r0 last in that call's own argument block — and the
distinction earned its keep, because both first-lever moves were measured and
neither helps:

| change | result |
|---|---|
| `__MessageID` given an `int` return type | 2 of 47 |
| `__MessageID` left implicit | 2 of 47 |
| **`__Func_8092c40` left implicit** | **match** |

The prototype is deliberately absent and the file says so, so nobody adds it
back to silence the warning.

Two other readings held in that function. The three `__ActorMessage(9, 0)` calls
really are three calls in the source — the ROM has two because gcc cross-jumps
two arms into a shared tail, and writing a `goto` would be transcribing the
optimiser rather than the source. And both `if`s are in the ROM's fallthrough
order, the same trap batch 81 recorded: `beq .L748` puts the else-arm at the
branch target.

`OvlFunc_909_20084ec` needed the same fallthrough reading — `cmp r0, r3 / beq`
with r3 built as `-1` means the not-taken path is the success arm.

## A split piece that holds only `.include` is not a piece

Cutting at the head of a `.s` leaves a "head" containing the file's
`.include "macros.inc"` line, so a naive `if head.strip()` writes an empty
`_a.s`, an empty `_a.o`, and a linker-script line for it. **It links and
`make compare` passes** — an object contributing nothing to `.text` changes
nothing — so it is invisible until someone reads the script and wonders what
`_a` is.

Decide on functions and sections, not on whether the text is empty. Caught here
on `ovl_30_c_c_c_c_c_c` and now in `elevation.md`.

## One thing that just worked

`OvlFunc_953_200ab1c` is thirteen calls in a straight line assigning palettes
3, 0, 4, 1, 5, 2, 6 to slots 0xc–0x12 and then staggering five animation speeds
ten frames apart. The palette ordering is the ROM's and is transcribed as found
rather than tidied. It matched on the first screen with nothing to say about it,
which is worth recording because the reports skew toward the ones that fought.
