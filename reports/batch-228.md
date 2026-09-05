# Batch 228 — a park that was wrong, and a tell that failed to fire

Six functions, one of them recovered from `src/non_matching/` where a previous
round had written **"NEXT: nothing source-level"**. That park is the batch's
main result, and the second-largest is the discovery that one of this project's
most-relied-on diagnostic tells has a silent failure mode.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `Func_80982dc` | `0x080982dc` | [rom_97b54_…_c_b.c](src/rom_8a000/rom_97b54_a_c_a_a_a_c_c_c_b.c) |
| 2 | `OvlFunc_885_2008be0` | `0x02008be0` | [ovl_30_…_c_a.c](src/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_a.c) |
| 3 | `OvlFunc_891_2008614` | `0x02008614` | [ovl_30_…_c_b.c](src/overlays/rom_78c76c/ovl_30_c_c_a_a_c_c_b.c) |
| 4 | `OvlFunc_952_2008ff8` | `0x02008ff8` | [ovl_30_…_a_b.c](src/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_a_b.c) |
| 5 | `OvlFunc_926_2009dbc` | `0x02009dbc` | [ovl_314_…_a_c.c](src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_c.c) |
| 6 | `OvlFunc_962_2008240` | `0x02008240` | [ovl_30_c_c_a.c](src/overlays/rom_7ec19c/ovl_30_c_c_a.c) |

## The park was wrong, and it was wrong in an instructive way

`Func_80982dc` had been parked on a literal-pool **rotation**: nine words, all
present, all correct, with `0x2090` moved from last to first. The park recorded
that it had tried three spellings of the final store and that all three produced
byte-identical pool orders — and concluded from that agreement that the order
was not driven by source at all.

The agreement was real. The inference was not. All three spellings left the
constant in the **same mode**, and mode is exactly what sets pool position.
`add_minipool_forward_ref` sorts by max_address, and a *narrow* reference has a
64-byte range where a word reference has a long one, so a narrow entry is forced
to sort early.

The ROM loads this constant with a word `ldr` and stores only its low halfword.
Written as a direct `short` store the constant is HImode, its reference is
narrow, and it sorts first — rotating the whole pool. Giving it an `int`
intermediate widens the reference and it sorts last: **18 differing encodings
down to 2.** The last two were an adjacent swap — the ROM forms the store's
*address* before loading the value — and binding the address to its own pointer
first closed it.

The control is what makes this safe to generalise: binding the address first but
writing the value as a **cast** rather than an `int` local goes straight back to
18 differing. The two levers are independent, and it is the local's mode doing
the work.

**A pool-order park is not automatically a dead end.** `tryc` cannot see this
class at all — it normalises PC-relative loads — so `objcmp` is the only screen
that measures it, and the mode of each pooled constant's reference is the first
thing to vary.

## The constant-CSE length tell has a silent failure mode

The standing procedure opens by reading the **length** difference: a constant
used twice across calls gets commoned into a callee-saved register, which adds
spill lines at entry, so plain C comes out longer than the ROM. Five batches have
leaned on that.

`OvlFunc_962_2008240` came out at **770 lines against the ROM's 770 — an exact
length match — with 710 of those 770 lines differing.** The widened prologue cost
precisely what the removed rematerialisations saved, and the two cancelled.

A length match therefore does **not** mean the CSE tell is absent. What
diagnosed it instead was the diff *text*: `push {r5,r6,r7,lr}` plus an r8–r11
staging push at entry, then thirty `mov rN, r5..r11` copies through the body.
**Counting those copies by destination register names the held values directly**
— here seven of them — which is a strictly better diagnostic than the length
difference because it survives the cancellation. This is now the recommended
first read.

## Six functions on `__Func_8092c40`, and the descending fill sharpened

`__Func_8092c40` wanting the descending fill is now **six functions** running.
`OvlFunc_962_2008240` sharpened it: there the *ascending* fill measured
**byte-identical to no pin at all**. Ascending at that callee is not a weaker
pin — it is a non-pin. That explains why the tell reads as binary rather than
graded.

## A sibling's cure can be actively wrong

`OvlFunc_891_2008614` closes with the same store block as its file-sibling
`OvlFunc_891_2008150` — same two words, same two offsets. The sibling needed a
scratch-register pin to land it. Here plain casts are exact, and **the sibling's
pin costs 14 differing**; an offset-clobber variant costs 12 and loses a line.

The reason is visible in the reference: the ROM's register roles are *inverted*
between the two. The sibling has r3 carrying the address; this one has r3
carrying the offset and then the value, with r2 as the address. Same source
shape, opposite allocation. **Read the block's register roles — do not copy the
neighbour's fix.**

## Pin-set size is a measurement, never an estimate

Four functions, four very different answers, all reached by greedy removal with
`objcmp` re-run after *every* drop and then a fixpoint pass:

| function | pins tried | pins shipped |
|---|---|---|
| `OvlFunc_891_2008614` | 111 | 33 |
| `OvlFunc_952_2008ff8` | 43 | 37 |
| `OvlFunc_926_2009dbc` | 69 | 68 |
| `OvlFunc_962_2008240` | 35 | **35 — none removable** |

`OvlFunc_926_2009dbc` is the interesting end: its constants are shared
*function-wide*, so dropping any single pin re-opens the same global CSE and
changes the push list. Where an overlay sibling strips a third to a half, this
one strips one.

And that one removal was not an inert pin — it was a **scheduling tie whose cure
was less source**. At the `__Func_80933f8` site the ROM's fill is ascending, but
r0's use is a trailing shift, so with pins in ROM order the scheduler sinks
`mov r0` past r1 and r2. The constant occurs nowhere else in the function, so
there is no CSE to destroy, and the plain unpinned expression call is exact.

Two further negative results worth keeping:

- **Once a pin exists, transcribing the ROM's emitted order into the fill buys
  nothing** — measured byte-identical at three sites in `OvlFunc_962_2008240`,
  including a `neg`, where the standing "the shift's position is source order"
  rule would have predicted otherwise.
- **The shifted-byte spelling is cosmetic inside a pin.** Rewriting every
  `0x80 << 9`-style constant as its flat literal is byte-identical; the `<<`
  form is kept only because it reads as the ROM's `mov`/`lsl` pair.

## Uniform fill continues to earn its default

`OvlFunc_952_2008ff8` went byte-exact on the **first** pinned pass: one
statement per argument, ascending, at all 43 sites, with no hand-ordered fill
anywhere. sched2 reproduced every one of the ROM's transposed emitted orders —
including a five-way interleave — from that single spelling. Three of the six
functions here needed no transcription at all.

## Housekeeping

Two `.s` files were split (`split_s.py`, which rewrites the linker scripts
itself). Both splits were verified with a **`make compare` on the layout change
alone, before any `.c` was written** — the tool's own advice, and it is worth
following, because a layout mistake and a bad decompilation are
indistinguishable at the end of a combined build.

No `const.sym`, `label.sym`, `message.sym` or `Makefile` entry was needed by any
of the six. Every pooled constant in all six functions was checked against the
shifted-byte test and every one is a bare literal.

Noted, not fixed: `fakematch.txt` and the in-file `// fakematch` marker have
drifted apart by three files (one marked and unlisted, two listed and unmarked).
Pre-existing, and small enough to leave until someone can establish which side is
right for each.
