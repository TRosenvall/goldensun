# Batch 198

Five elevated, all five from `src/non_matching/`, no new park — and a **class of
fourteen functions reopened**.

The batch's result is not a lever. It is that a class note which had closed
fourteen functions to a "compiler-level answer" was wrong, and the reason it was
wrong is the one this notebook has been finding for six batches running.

## Function breakdown

| # | function | address | file | note |
|---|---|---|---|---|
| 1 | `OvlFunc_949_2008260` | `0x02008260` | [ovl_30_c_c_a_a_c_b.c](src/overlays/rom_7d4af4/ovl_30_c_c_a_a_c_b.c) | a pin needs no second basic block |
| 2 | `OvlFunc_887_20081e0` | `0x020081e0` | [ovl_30_c_a_a_c_c_a_c.c](src/overlays/rom_787e04/ovl_30_c_a_a_c_c_a_c.c) | a pin needs no dominating block |
| 3 | `OvlFunc_883_2008dc0` | `0x02008dc0` | [ovl_30_…_a_c_c.c](src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_a_c_c_a_c_c.c) | **class member**, first screen |
| 4 | `OvlFunc_883_2008e54` | `0x02008e54` | [ovl_30_…_a_a_b.c](src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a_a_b.c) | **class member**, first screen |
| 5 | `OvlFunc_883_2008e84` | `0x02008e84` | [ovl_30_…_a_a_c.c](src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a_a_c.c) | **class member**, first screen |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## A CLASS OF FOURTEEN, CLOSED FOR THE WRONG REASON

`src/non_matching/overlays/arg_interleave_flat.c` is not a park but a class
note. It found the largest shape group in the unelevated corpus by grouping every
function under 30 instructions by its opcode set — fourteen members, all flat,
all ending in a call whose arguments interleave:

    mov r1, #0xcb / mov r0, #0 / lsl r1, #1 / ldr r2, =0x2d7

`r0` is written *inside* r1's two-instruction build. The note's analysis is
correct: the batch-43 basic-block lever moves an argument constant by assigning
it in a block that **dominates** the call, that needs a branch, every member is
straight-line, so `REG_BASIC_BLOCK (regno) < 0` can never hold.

Its conclusion was that these *"need a compiler-level answer, not a source-level
one"* and are *"the single largest argument for reading gcc's source rather than
generating more variants."*

**A pin does not go through basic blocks.** It names the hard register, so the
placement is decided at the assignment and that predicate never needs to become
true. Three members were taken this batch and **all three matched on the first
screen**. A fourth, `OvlFunc_945_200bdec`, was elevated back in batch 194 — the
class was already broken and nobody had connected it back to the note.

**Eleven members remain**, each a short job: the bodies are trivial and the only
work is reading each call's argument order off the ROM.

### The part worth keeping

The note says, in capitals, **"SCREENED TO CONFIRM RATHER THAN ASSUMED"** — and
it did screen. It ran three variants on a member and reported the numbers.

Every one of those variants changed the **source**, which is exactly what the
mechanism it had just correctly identified rules out. *Screening more variants of
the thing you have proved cannot work is not confirmation.* The discipline of
measuring was followed and it produced false confidence, because the measurements
were all drawn from inside the closed set.

That is the sixth batch running in which a conclusion true of one lever's
mechanism was recorded as a fact about C — and this is by far the most expensive
instance, because the note told the candidate ranker to keep refusing fourteen
functions for many rounds.

## THREE MEASURED NEGATIVES

The pin is not a universal solvent, and three parks were tested and left standing.

**`Func_80064b8` — the pre-header load merge.** The value pinned to `r3` is
byte-identical to the base form; pinned to `r0` it is one worse. The `r3` result
is the informative one: `r3` is call-clobbered and the loop body contains a call,
so if pinning could force a reload it would force one here. It cannot, because
gcc has already factored two loads reaching the same test into one at the merge
point. **A pin decides which register holds a value and where its own write sits;
it says nothing about where in the control flow a load happens.**

**`Func_808d828` — the load-interleave shape**, at 4 of 94. The ROM puts a load
*inside* the offset's build: `mov r3, #0xb8 / ldr r2, [r5, #8] / lsl r3, #1`.
That is the shape measured unreachable across seven forms in `2008de8` — a pin
places the `mov` of the register it names, and this needs a load into a
*different* register placed between that `mov` and its shift. Naming the store
address, the right lever for the other half of the residue, makes it worse (4 to
5): the two halves want opposite things. Also recorded there: this TU has no
`ALIAS_CFLAGS` rule, so finishing it needs one added.

**`OvlFunc_965_200a6fc` — branch polarity**, left untouched. Its park had already
established that gcc-2.96 normalises the branch pair in its jump optimiser after
the source shape is gone, and had measured three spellings against it. Nothing in
this batch's toolkit addresses a jump-optimiser normalisation, so it was not
re-swept.

## Smaller

`tools/crossed.py` cleared every candidate taken this batch and rejected three
others before any screen was run.

Two more stale paths: `2009958`'s recorded `.s` had been split away in the
previous batch, and this batch a file I had *just* split was gone from under a
later lookup. Both were found by grepping the function name, which is the only
reliable handle.
