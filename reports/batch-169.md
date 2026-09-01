# Batch 169

Five elevated. Verified after a clean `make clean && make compare`; SHA1
`5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked images.

## The functions

| function | address | image | ratio | what closed it |
| --- | --- | --- | --- | --- |
| `LoadItemIconID` | 0x08019fcc | goldensun.elf | 0.889 | one statement move |
| `OvlFunc_922_20095dc` | 0x020095dc | rom_7a8c8c | 0.864 | named `gState` offset, then an int-vs-`unsigned short` reading |
| `Func_80929d8` | 0x080929d8 | goldensun.elf | 0.811 | first screen, no iteration |
| `OvlFunc_927_2009880` | 0x02009880 | rom_7b4558 | 0.907 | first screen, no iteration |
| `OvlFunc_927_200a078` | 0x0200a078 | rom_7b4558 | 0.876 | constant left INLINE, not named |

All five came off `tools/fuzzy_solved.py`. Two matched with no iteration at all.

## The headline: naming a repeated constant can be WRONG

`OvlFunc_927_200a078` passes `0xc0 << 11` to two calls, and the ROM builds it
once into r8 and holds it across the intervening work. That is the shape which
normally means a named local, and its own solved exemplar names it.

Naming it here is **2 differing** at exact length, and the difference is
argument-setup order:

    rom    mov r2, #0x86 / mov r3, #0xc0 / lsl r3, #11
    ours   mov r3, #0xc0 / mov r2, #0x86 / lsl r3, #11

Writing the literal at **both** call sites matches outright. gcc's own CSE
produces the single build in r8 by itself; the named local additionally fixes
*when* that build happens, and fixes it wrongly. Moving the assignment earlier
is worse again — 125 lines and 122 differing, both placements tried.

> **A constant held in a callee-saved register across calls is not by itself
> evidence for a named local.** gcc will common two plain literals into exactly
> that. Name it only when the plain spelling fails.

This is the inverse of the three parks below, where gcc's hoist is precisely
what has to be prevented. Same mechanism; whether it is right depends on the
ROM.

## Three parks, one class, and why the recorded remedy misses

`OvlFunc_948_200938c` and `_200949c` (parked together — same cutscene over
different slots), `OvlFunc_959_200a1c4`, and `OvlFunc_959_200a06c` all lose to
the duplicate-constant hoist: gcc builds a repeated expensive constant once into
a callee-saved register where the ROM rebuilds it at each site.

The recorded remedy is separate locals per use site. Copying its worked instance
is **exactly inert** — byte for byte identical to plain literals — and
`-fno-rerun-cse-after-loop`, `-fno-gcse` and `-fno-cse-follow-jumps` are inert
too.

The reason is guard placement. The working instance has three `if` blocks
*between* the assignments and the uses; all four of these are straight-line
across the repeats. **So "separate locals per use site" is the dominating-block
mechanism under another name**, joining the argument interleave and constant-CSE
levers in needing a branch to rematerialise across.

`200a06c` adds a presentation the earlier descriptions would not catch: gcc
commons the **pre-shift** eight-bit base, `mov r5, #0xb0`, underneath two
*different* shifted results. The repeated thing need not be an argument, or even
a value the ROM writes twice.

## Two more readings

**`(unsigned short)(v - 1) <= 1` is not `unsigned short w = v - 1; w <= 1`.**
`OvlFunc_922_20095dc` guards on a halfword minus one. The ROM subtracts as a
full word and narrows only at the comparison. Declaring the intermediate
`unsigned short` makes gcc materialise the wrap instead —
`ldr r2, =0xffff / add r3, r2` — three differing at exact length. A narrow
*type* asks gcc for a narrow value; a cast inside a comparison only asks for a
narrow *comparison*. Same family as batch 168's `x <<= 16; x >>= 2;`.

**The `gState` offset build, twice more.** `OvlFunc_922_20095dc` screened at
135 differing purely because `gState + (0xe1 << 1)` folded to one pooled symbol.
Naming the offset took it to 3. Batch 168 recorded this; it is now the single
highest-yield first thing to check when a differing count approaches the line
count.

## Tooling: warn before the C is written

`tools/fuzzy_solved.py` now flags two failure modes on the lead list itself.

**FAKEMATCH exemplars.** Some elevated files match only by pinning locals with
`__asm__ volatile ("" : "+r" (x))`. Copying one *works*, which is the problem —
it propagates the hack into a target that may not need it. The tool offered
exactly that for `OvlFunc_959_200a06c`, whose target is a nine-call
straight-line cutscene needing nothing of the kind.

**DUP-CONST targets.** `tools/filtered.py` already computed the notion of
"expensive constant" that predicts the hoist, so the detector reuses that
function verbatim. It has to read the real instruction text — `match_shapes`
collapses every immediate to one letter, so the skeleton the ranking runs on
cannot see repeated constants at all.

Of eleven leads at the start of the round, two carried a fakematch exemplar and
three were flagged DUP-CONST. This batch took its work from the clean six.

## Where the leads stand

Eight remain above 0.80, of which five are clean. `fuzzy_solved.py` has now
produced fourteen elevations across batches 167–169.
