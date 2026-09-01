# Batch 171

Five elevated. Verified after a clean `make clean && make compare`; SHA1
`5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked ELF.

## The functions

| function | address | ratio | what closed it |
| --- | --- | --- | --- |
| `Func_80a68a8` | 0x080a68a8 | 0.714 | first screen, no iteration |
| `Func_80a3d24` | 0x080a3d24 | 0.710 | first screen, no iteration |
| `Func_80a9cf8` | 0x080a9cf8 | 0.656 | named global read, then initialiser order |
| `Func_80a9d3c` | 0x080a9d3c | 0.667 | `goto` loop + an `int` intermediate |
| `GiveItem` | 0x08078618 | 0.629 | first screen, no iteration |

All five off `tools/fuzzy_solved.py`. Three needed no iteration at all — and
all three of those came from exemplars elevated within the previous two rounds.

## Elevations compound: today's match is tomorrow's exemplar

`Func_80a68a8` was elevated one round, and the next round it appeared as the
EXEMPLAR for `Func_80a3d24`, which matched cold. `Func_80a9b94` (batch 170) was
the exemplar for `Func_80a9cf8` here.

The solved corpus grows every round, so a ratio computed today is not the ratio
that will be computed next round — functions below threshold rise as their
nearest neighbour gets solved.

> **Re-run the ranking every round rather than working from a saved list**, and
> do not write off a function because it ranked poorly before its family had a
> member.

## Initialiser order: three for three, now a first check

| function | shape | what moved |
| --- | --- | --- |
| `Func_80aac84` (batch 168) | `goto` loop | counter before base |
| `Func_80a9b94` (batch 170) | plain `do`/`while` | counter before pointer |
| `Func_80a9cf8` | plain `do`/`while` | counter before a held constant |

Not a `goto`-loop phenomenon, not specific to pointers. **When a small residue
sits in a loop's setup block, permute the initialisers before reaching for any
lever** — each permutation is one screen and there are usually two or three
worth trying.

`Func_80a9cf8` also needed the global read NAMED before the pointer was derived
from it (`p = iwram_3001f2c; … q = p + 0xc8;`), which took it 9 → 2. That is the
rule `Func_80a1cb0` established in batch 170: the name pins the LOAD, and the
displacement still happens where the source puts it.

## The `goto`-loop lever, fourth instance, fitting the recorded rule

`Func_80a9d3c`'s parallel flags access was strength-reduced by gcc into its own
walking pointer with a precomputed end address, where the ROM keeps the counter
and indexes with it. The `goto` rewrite recovered it — which is batch 170's
discriminator behaving as stated: **the rewrite pays when gcc applied a loop
TRANSFORMATION, not merely a hoist.**

That function also pooled a halfword store of `8` (`ldr r3, =0x8` where the ROM
has `mov r3, #0x8`), needing an `int` intermediate. That is a third exception to
the narrowed HImode rule, so check the halfword exception on any small constant
stored through a `short *`, not just on `0`.

## SCRATCH-REGISTER SELECTION is a distinct wall, three deep

Three parks this batch reach the ROM's exact length AND its exact instruction
sequence, differing only in which of r1/r2/r3 carries a value:

| function | lines | differing |
| --- | --- | --- |
| `OvlFunc_882_20090a4` | 80 of 80 | 8 |
| `OvlFunc_968_200c968` | 90 of 90 | 22 |
| `SystemMsgBox` | 79 of 79 | 23 |

Distinct from the register-ROLE swap (callee-saved allocation, visible in the
prologue) and from scheduling (which moves instructions). Here the prologue
matches, the order matches, the work matches.

Inert across all three: naming a value, naming a global's address, naming a
struct pointer, reordering the statements that produce the operands, operand
order within an expression, and every flag group tried. Those levers change
*what* is computed or *when*; none changes which scratch register receives it.

> **When a screen is at exact length with the instruction sequence aligned and
> the diff is a column of `mov rN` against `mov rM`, take one probe and park.**
> These three cost four to seven screens each to reach the same place.

## Two more findings

**A reassigned local sometimes has to be SPLIT, not merged.**
`OvlFunc_882_20090a4`'s ROM runs r5 through `0x35` then `0x36` — the merge
lever's usual shape. One reassigned local is 78 of 80 with 64 differing; two
separate locals is 80 and 8. gcc coalesces the single variable's live ranges and
then needs one FEWER callee-saved register than the ROM. **The push list is the
discriminator**: a push the ROM has and we lack says split, exactly as a push we
have and the ROM lacks says merge. That function also needed `-ffixed-r7`.

**Changing three things at once cancels signal.** `Func_80df9d0` screened at 5
differing with three fixes visible. Applied together: **9**, worse than
baseline. Individually: named loop bound **11** (wrong), offset-first pointer
**4**, split load/store **3**, the two good ones together **2**. The recorded
"change ONE thing at a time" rule has a second failure mode — a change that
helps and a change that hurts *cancel*, and the combined number says nothing
about either. A batch scoring worse than baseline is not evidence that all of it
is wrong.

## Parks

Six: `ovl_77dd1c/20090a4.c`, `ovl_7f2f14/200c968.c`, `rom_15000/80208e4.c` (the
scratch-register three), `rom_c9000/80df9d0.c` (2 of 38, on where gcc parks a
hoisted loop bound), `rom_b5000/80b90ac.c` (third instance of the
copy-into-a-callee-saved-register wall — and the copy goes to r7 here where the
earlier one went to r8, so it is not about the high-register bank), and
`ovl_77dd1c/2009498.c`.
