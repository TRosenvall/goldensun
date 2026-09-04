# Batch 208

Five elevated, two parked. One of the five was a park closed by its own `NEXT:`
paragraph; closing it then exposed a **screening tool that was rejecting correct
work**, which is the batch's most important result.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_952_2008108` | `0x02008108` | [ovl_30_c_a_a_a_c_b.c](src/overlays/rom_7d768c/ovl_30_c_a_a_a_c_b.c) |
| 2 | `OvlFunc_890_2008c00` | `0x02008c00` | [ovl_30_…_b_a_b.c](src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_b.c) |
| 3 | `OvlFunc_890_2009a58` | `0x02009a58` | [ovl_30_…_a_c_b.c](src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_b.c) |
| 4 | `OvlFunc_895_200961c` | `0x0200961c` | [ovl_30_…_c_c_b.c](src/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_b.c) |
| 5 | `OvlFunc_956_2009df8` | `0x02009df8` | [ovl_30_…_a_c_b.c](src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_b.c) |

Parked: `OvlFunc_958_2008df0` at 12 of 136 and `OvlFunc_882_2008d5c` at 2 of 139,
both length-exact.

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## THE SCREEN REJECTED A CORRECT FUNCTION

`2008108` finished, and `tryc.py` refused it:

    !! instructions match but OUR POOL HAS 5 entries and the reference needs 4.
       ... two `.word`s for one value is 4 bytes larger and WILL fail make compare.

The build disagreed. The check computed the reference's requirement as a
`set()` of `=value` operands **across the whole file**, but a function can hold
more than one literal pool — a mid-body `.pool_aligned` plus the implicit one
inside `.func_end` — and **Thumb PC-relative loads are forward-only**, so a
value used on both sides of a pool boundary needs a slot in *each*. `0x969` is
used once before the boundary and once after. The ROM spends five slots too.

Now counted per pool. The change is **monotone** — the sum over segments is
never less than the whole-file distinct count — so it can only remove
rejections, never add one. Across the 1090 hand-written `.s` left in the tree it
raises 372 counts and leaves 718 alone.

This tree has now had this failure in both directions: batch 205 landed a
function on a screen that passed it and `make compare` failed; this batch nearly
abandoned one the screen failed and `make compare` passed. **The screen is a
cheap filter with two failure modes, and the build is the only verdict.** Both
episodes are recorded next to the code that caused them.

## A PARK CLOSED BY ITS OWN CLOSING PARAGRAPH

`2008108` was parked last batch with this as its last line:

> the question is how to spell "this value's live range begins exactly where
> that one's ends" when the two are separate source variables. A single variable
> reused for both is the obvious try and was not made.

That is the entire fix. The ROM refills r6 one instruction after its last read;
as two source variables it cannot be reached. Hoisting the last read into a
local of its own and assigning the second value back into the **first** variable
gives it with no lever at all:

    t = d << 16;
    d = 0x80 << 7;
    if (t == (0x80 << 23)) { ... }

Second batch running that a park's `NEXT:` paragraph turned out to be work
already scoped rather than a summary.

**And the count went UP.** 36 of 136 became 45 of 135 — while the first
divergence moved *ninety instructions forward*. Last batch recorded that a
correct fix downstream reads as a regression; this is the same rule from the
other side: **a lever that moves the first divergence has worked even when the
total rises.**

## TWO CURES FOR CONSTANT NARROWING, AND THEY ARE NOT INTERCHANGEABLE

Last batch found gcc narrowing an AND mask when the result is truncated, and
that `~0x3fff` blocked it where `0xffffc000` did not. `2008d5c` has the same
family and **the `~` form does not work**:

    s[9] & -0xd     ->  mov r3, #0xf3        (one instruction; ROM spends two)
    s[9] & ~0xc     ->  mov r3, #0xf3        (identical — no help)
    int z = 0xd; s[9] & -z  ->  mov r3, #0xd / neg r3, r3    (exact)

101 of 138 to 5, length exact. The two block different things: the `~` form
blocks combine's backward truncation through an AND; naming blocks the folding
of the negation. **Try both.**

## TWO READ-MODIFY-WRITES THAT INTERLEAVE

Also from `2008d5c`. The ROM loads the second byte *before* storing the first:

    ldrb r3,[r6,#9] / mov r2,#0xc / orr r3,r2 / ldrb r2,[r7] /
    strb r3,[r6,#9] / mov r3,#1 / orr r3,r2 / strb r3,[r7]

Two sequential `|=` statements do not produce that. Splitting both into explicit
loads and stores, with the second load written between the first accumulate and
the first store, gives it exactly. Note the two sites want **opposite** forms —
the first has the value in the destination and the second the constant — which
is the accumulate-names-its-destination rule from batch 207 applied in both
directions inside one expression pair.

## A SECOND, DIFFERENT LIMIT ON THE BARRIER

Batch 207 bounded the barrier by register pressure: unavailable where the ROM
itself uses r8–r11. `2008d5c` has **zero** high-register instructions and the
barrier is still harmful — a `do { } while (0)` before the site gives 8
differing and moves the first divergence *backwards* to instruction 27, breaking
a pointer copy that was already correct.

So the pressure test is necessary, not sufficient. On the other parked function,
`2008df0`, the barrier is the one thing that *does* land, closing a site where a
pin is provably inert. Both measurements are in the parks.

## THE TWO PARKS SHARE ONE UNNAMED CLASS

`2008df0` (12 of 136) and `2008d5c` (2 of 139) are both stuck on the same thing:
**an immediate build scheduled ahead of, or interleaved into, an addressing
computation.** `docs/elevation.md` records that a pin orders two movs of
immediates and does *not* order two independent loads; this is the mixed case
and it behaves like the loads.

Two negative results from `2008df0` make the usual cures unavailable rather than
merely unsuccessful, and they are in tension:

- **Naming the offset switches the addressing form.** Any spelling that makes
  the offset a variable gives `strh r3, [r1, r0]` in place of the ROM's
  `add r2, r1, r0 / strh r3, [r2]` — one instruction shorter, 57–62 differing.
- **Naming the value commons the two addresses**, letting gcc strength-reduce
  the second offset off the first.

The two functions should be worked together; a lever for one is very likely a
lever for the other.

## AT THIS SIZE THE WORK IS TRANSCRIPTION

Three of the five are large and branchy — 175, 163 and 152 instructions — and
all three were **exact or near-exact on the first screen** with no lever that
was not already written down. What they needed was accuracy: every call site
read off the listing rather than copied from the previous one that worked.
`2009a58` calls one helper five times and the ROM fills its registers four
different ways.

`2009df8` is the useful one to read, because it contains **both** forms of the
same four-argument fill:

    mov r0 / mov r1 / mov r2 / mov r3 / neg r0 / neg r1 / neg r2    not crossed — plain pins
    mov r0 / mov r1 / mov r2 / lsl r2 / mov r3 / neg r1 / lsl r0    crossed — two barriers

They look alike. Compare the two orders before reaching for the lever.

## SMALLER

**gcc derives a constant from an address offset unaided.** `200961c` opens with
`mov r2,#0xe0 / lsl r2,#1 / add r3,r2 / add r2,#0x44 / str r2,[r3]` — the stored
value 0x204 built by adding 0x44 to the address offset 0x1c0 already in the
register. Two plain literals produce it. The reflex on seeing a value derived
from an unrelated-looking quantity is to reach for a lever; here the cost model
does it.

**A call duplicated down one arm is not a decompilation error.** `2008c00` calls
one helper with identical arguments in the true arm *and* at the join, while the
false arm calls `__GetFlag` and discards the result. Written literally it
matches; "simplifying" it to one call after the `if` would not.

**A deferred shift is source structure.** `2009df8` sets `m = 0x8c`, makes two
calls, and only then shifts. The gap has to be in the source, and `m` pinned, or
cprop folds the pair into a single `mov`.
