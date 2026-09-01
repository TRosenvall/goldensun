# Batch 172

Eight functions elevated. The batch spans two distinct methods: the first five
came from the fuzzy-exemplar and twin-family rankings that carried batches
167-171, and the last three came after those rankings ran dry, from a widened
scan at the *small* end of the corpus that the standing filters had never
looked at.

## Function breakdown

| # | function | address | file | how it was reached | screens |
|---|---|---|---|---|---|
| 1 | `GiveItem` | `0x08078618` | [rom_78414_c_c_a_a_c_b.c](src/rom_77000/rom_78414_c_c_a_a_c_b.c) | fuzzy exemplar | 4 |
| 2 | `Func_80a14f0` | `0x080a14f0` | [rom_a1050_c_c_b.c](src/rom_a1000/rom_a1050_c_c_b.c) | fuzzy exemplar; corrected the narrowing-shift rule | 5 |
| 3 | `OvlFunc_915_2008aac` | `0x02008aac` | [ovl_30_c_c_a_a_b.c](src/overlays/rom_7a2bf0/ovl_30_c_c_a_a_b.c) | twin family | 3 |
| 4 | `OvlFunc_913_2008b1c` | `0x02008b1c` | [ovl_30_c_c_a_a_c.c](src/overlays/rom_7a04ac/ovl_30_c_c_a_a_c.c) | twin of #3, ported | 1 |
| 5 | `Func_8096cdc` | `0x08096cdc` | [rom_96cdc_a_a_a.c](src/rom_8a000/rom_96cdc_a_a_a.c) | twin family | 2 |
| 6 | `Sprite_DeleteLayerIndex` | `0x0800b9a4` | [rom_b798_c_a_a_c_c.c](src/rom_9000/rom_b798_c_a_a_c_c.c) | small-end scan; solved sibling `Sprite_DeleteLayer` supplied the whole tail | **1** |
| 7 | `Func_80b6378` | `0x080b6378` | [rom_b5a0c_c_c_a_a_a_a_c_c_b.c](src/rom_b5000/rom_b5a0c_c_c_a_a_a_a_c_c_b.c) | small-end scan | 4 |
| 8 | `Func_8098184` | `0x08098184` | [rom_97b54_a_c_a_a_a_c_b.c](src/rom_8a000/rom_97b54_a_c_a_a_a_c_b.c) | small-end scan | 2 |

Every address above was read back out of the linked ELF after a clean
`make clean && make && make compare`, and each is immediately preceded there by
a `.gcc2_compiled.` local symbol at the same address -- which is the check that
actually proves the bytes came from a C translation unit rather than from a
stale object beside a deleted `.s`. That check is not ceremony; see "A stale
object can make a wrong elevation look green" below.

## THE SELECTION FILTERS HAD DRAINED, AND THE POOL THEY DRAINED WAS NOT THE CORPUS

At the top of this round every ranking returned nothing usable:

| tool | result |
|---|---|
| `fuzzy_solved.py` | 4 leads at ratio >= 0.80, **all four flagged** DUP-CONST or FAKEMATCH |
| `pickable.py` | 99 candidates, and **every one of the 99 uses r8-r11** |
| `match_shapes.py`, `solved_twins.py`, `--near 1/2/3` | 0, as in the previous two rounds |

Batch 170 recorded "when the clean list empties, lower the ratio before changing
method." That advice was right then and is exhausted now -- the fuzzy pool is
dry at 0.60. The `pickable` result is the sharper signal: not that the filter
found little, but that its survivors are now **homogeneous**. Every remaining
candidate it admits carries the one wall it was never designed to score.

**The filters were tuned for 40-120 instructions and >= 5 calls, and that band
is worked out.** So the productive move was not another ranking over the same
band but a scan *below* it: 14-44 instructions, no r8-r11, no repeated expensive
constant, call count unconstrained. That returned 19 candidates, of which the
28-function `rom_f95e0.s` and `rom_f9ef8_a.s` audio bodies are a separate class.
Of the remainder, **three were tried and three matched**, at 1, 4 and 2 screens.

The lesson is about the filters, not the functions. `pickable.py` rejects under
40 instructions because "a tiny function gives the allocator nothing to act on"
-- and that reasoning is sound *for the levers the filter was built around*.
But it is a statement about which LEVERS apply, not about which functions
MATCH. A short function often needs no lever at all. **A filter tuned to
predict which functions need documented work will systematically hide the
functions that need none.**

## THE EPILOGUE TELL, REFINED -- AND A CORRECTION TO THIS REPORT

**Correction.** This section originally presented the epilogue tell as a new
finding. It is not. `docs/elevation.md` already recorded it twice, in batch 46
("Tell: `pop {r1}` in a function that looks void names a RETURN VALUE") and
again for `OvlFunc_971_20091bc` ("The epilogue register tells you the return
type"). The HANDOFF entry for this batch has been corrected too. What follows is
what the case actually adds.

`Func_80b6378` reached the ROM's exact length and exact sequence with a single
residue in the last two instructions:

```
    rom    pop {r1}    ours   pop {r0}
    rom    bx r1       ours   bx r0
```

Changing the declaration from `void` to `int` -- with no `return` statement
added, and no other edit -- matched.

That is the part the earlier entries do not cover. Both of them explain the r1
epilogue as the function returning *the value the last call left in r0*, and
both fix it by writing an explicit `return f(...)`. This function has no
`return`, no trailing call, and no value to return: its body ends in a store
inside a loop. So the mechanism sits one step earlier than those entries state.
gcc marks r0 live at exit from the **declared return type alone**, whether or
not any value ever reaches it, and the epilogue scratch falls to r1 as a
consequence. The tell reads:

> **`pop {r0}` / `bx r0` means declared `void`. `pop {r1}` / `bx r1` means
> declared non-void** -- not "returns something."

When the earlier entries' fix has no candidate call to apply to, the declaration
alone is still the whole lever. Their other advice stands unchanged: a two-line
epilogue residue looks exactly like the scratch-register-selection wall named in
batch 171, so **check the epilogue pair before adding a function to that park
class.**

## AN INDEX INTO A POINTER MUST BE NAMED TO STAY AN INDEX

The other residue in `Func_80b6378` was the store addressing:

```
    rom    add r2, #0x48          ours   add r2, r6, r2
    rom    strb r3, [r6, r2]      ours   add r2, #0x48
                                  ours   strb r3, [r2, #0x0]
```

The ROM builds the whole index in one register and stores with register-plus-
register addressing. Ours folds the base in first and ends up one instruction
longer. Measured spellings:

| spelling | result |
|---|---|
| `*(p + buf[i] + 0x48) = v;` with `char *p` | base folded in early, 28 lines (rom 27) |
| `*(char *)(buf[i] + 0x48 + p)` with `int p` | identical output; reassociated the same way |
| `k = buf[i] + 0x48; p[k] = v;` | **matches** |

This is the offset-first rule from batch 170 and 171, but it sharpens a detail
those entries got wrong. Both of them describe the lever as *operand order* --
"put the offset expression first." Operand order is inert here: the second row
above is the offset written first and it changes nothing, because `+` is
commutative and gcc reassociates before it selects addressing modes. **What
works is giving the index a NAME**, which creates a value that must exist as a
single quantity before the memory reference is formed, and therefore an index
register.

That is the same mechanism as the batch-170 global-read rule ("a global read
must be NAMED to keep its load above a following branch") pointed at addressing
instead of scheduling: a name is not a hint about placement, it is a
**materialisation point**. Where the earlier rule uses it to pin a load above a
branch, this one uses it to pin a sum ahead of an address computation. The
generalisation across both:

> Naming a value does not tell gcc where to *put* it. It tells gcc the value
> must *exist*. Every lever in this document that works by naming something
> works for that reason, and every one that fails -- see the constant-folding
> entry below -- fails because the value existed already.

## A REASSIGNED ACCUMULATOR MAY NEED A SECOND VARIABLE

`Func_8098184` came out at 21 lines against the ROM's 22, the extra ROM
instruction being a plain register copy inside the loop:

```
    rom    add r3, r2, r1     ours   add r2, r1
    rom    mov r2, r3
```

The natural `while (v <= 0xffff) v += 0x1000;` updates in place. The ROM
computes into a second register and copies back, so the source has two
variables, and the one that survives the loop is the one that is **stored
afterwards**:

```c
do {
    w = v + (0x80 << 5);
    v = w;
} while (w <= 0xffff);
*(int *)(a + 0x18) = w;
*(int *)(a + 0x1c) = w;
```

Matched. This is the split-versus-merge discriminator from batch 171 appearing
in a loop body rather than across a function, and it has its own tell: batch
171's discriminator is the **push list** (a callee-saved push the ROM has and we
lack says split). That tell is silent here -- both registers are caller-saved
and neither is pushed. **The loop-body tell is a bare `mov rN, rM` between the
update and the test**, with the stores after the loop reading the destination.
gcc will not coalesce the copy when the two variables have genuinely different
live ranges: `v` is dead at the loop exit and `w` is not.

The two discriminators are worth holding together, because they point the same
way from different evidence: a copy the ROM makes and we do not means the source
had two names where we wrote one.

## A STALE OBJECT CAN MAKE A WRONG ELEVATION LOOK GREEN

A process note that cost real time this round and would have cost much more if
it had gone the other way.

The tree compiles `src/<path>.c` to `asm/<path>.s` to `asm/<path>.o`, and it is
the **`.o` that the linker script names**. When a hand-written `asm/<path>.s` is
replaced by an elevated `src/<path>.c`, the old `.o` is still sitting there,
still newer than nothing, and `make` has no reason to rebuild it. Two of this
batch's three elevations built green in an incremental build while their `.o`
was still the one assembled from the **hand-written asm that had just been
deleted**. The C had never been compiled. A green `make compare` in that state
means only that the ROM is unchanged, which it trivially is.

It was caught because `asm/<path>.s` was missing for two functions and present
for the third -- an asymmetry with no innocent explanation, since all three had
been handled identically. The generated `.s` is a tracked build product and its
absence is the visible symptom of an object that was never regenerated.

The discipline that actually settles it:

> **After removing a hand-written `.s`, the elevation is not proven until a
> `make clean` build. And the proof is not the SHA -- it is the
> `.gcc2_compiled.` symbol at the function's address in the linked ELF.**

The existing rule "check every address against the linked ELF" was already in
the loop and would NOT have caught this on its own: the symbol is at the right
address either way. Reading the `.gcc2_compiled.` marker beside it is the part
that distinguishes a compiled translation unit from an assembled one, and it is
now part of the check.

## Method

Screens this round: 1 for `Sprite_DeleteLayerIndex`, 4 for `Func_80b6378`, 2 for
`Func_8098184` -- seven screens for three matches, against the four-to-seven
screens *per function* that batch 171 recorded for the hard tail. Some of that
is that short functions are easier. But `Sprite_DeleteLayerIndex` matched on the
first screen for a specific and repeatable reason: its solved sibling
`Sprite_DeleteLayer` in the same `.s` family is the same routine reached by
pointer instead of by index, and its entire second half -- the count loop, the
`i == 0` test, the byte store at `+0x27` -- transferred verbatim.

That is the batch-171 "elevations compound" finding, but through a channel the
ranking tools do not see. `fuzzy_solved.py` scores skeleton similarity across
the whole corpus and never surfaced this pair, because the two functions differ
in their entire first half. **Siblings inside a single `.s` family are worth
checking by hand even when no ranking offers them**, since the tree's `_a/_b/_c`
splits keep genuinely related routines adjacent and the rankings are blind to
that adjacency.

## Parks

None this round. Both parks written since batch 171 -- `rom_b0000/80b2720.c`
(32 of 32, 23 differing, scratch-register rotation) and `rom_15000/8019944.c`
(38 of 40, on a zero the ROM parks in r12) -- are recorded under the
constant-folding entry in `docs/elevation.md` and belong to the previous round's
accounting.
