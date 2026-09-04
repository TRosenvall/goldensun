# Batch 196

Five elevated, **all five withdrawn from `src/non_matching/`**, one new park, and
one prediction made in advance and then confirmed.

No new lever was invented this batch. The work was recognising which existing
lever a park's residue wanted, and the batch's result is a **discriminator that
tells the two sub-cases apart before any screen is run**.

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `OvlFunc_931_20086a4` | `0x020086a4` | [ovl_30_…_a_c_b.c](src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_b.c) | **fakematch**; *parked by prediction* at 2 of 28 |
| 2 | `OvlFunc_923_2008f48` | `0x02008f48` | [ovl_e90_c_c_a_a_c_c.c](src/overlays/rom_7aa430/ovl_e90_c_c_a_a_c_c.c) | **fakematch**; one pin beats a precompute bind |
| 3 | `OvlFunc_963_2008288` | `0x02008288` | [ovl_30_c_c_a_a_c_b.c](src/overlays/rom_7ec968/ovl_30_c_c_a_a_c_b.c) | **fakematch**; *the predicted case* |
| 4 | `OvlFunc_946_2009494` | `0x02009494` | [ovl_30_…_a_a_c.c](src/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_a_a_c.c) | **fakematch**; both halves already solved |
| 5 | `OvlFunc_936_2008504` | `0x02008504` | [ovl_30_…_a_a_c_b.c](src/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_a_c_b.c) | **fakematch**; predicted again |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## THE DISCRIMINATOR, AND A PREDICTION THAT HELD

Three parks carried what looked like one residue — a `mov` that must land inside
another register's `mov`/`neg` build. They do not behave the same way:

    20086a4   mov r2,#0x10 / mov r1,#0x2 / neg r2       interleaved arg 2        MATCHED
    2008288   mov r2,#0x10 / mov r1,#0x3 / neg r2       interleaved arg 3        MATCHED
    2008504   mov r2,#8    / mov r1,#0   / neg r2       interleaved arg 0        MATCHED
    2008120   mov r2,#0x10 / mov r0,#0 / mov r1,#0      BOTH args 0, no neg between   PARKED

`2008120` resisted four pin arrangements. Comparing it against the site that had
just fallen gave the rule, which was written into its park **before** the
remaining candidates were attempted:

> the interleaved argument needs an operation nearby whose order the source can
> set.

Two functions were then taken on that basis and both matched on the first
screen. Note what the third row corrects: the rule is **not** "the value must be
non-zero" — `2008504`'s interleaved argument is a zero and it yields, because a
single zero sits against a `mov`/`neg` pair on a distinct value. What defeats
`2008120` is that *both* interleaved arguments are zeros with no consuming
operation anywhere between them.

This is the batch-195 rule seen from its other side: **mov order follows the
order of consuming operations, and a mov with no consumer has nothing to
follow.**

## FOUR MORE PARKS RIGHT ABOUT THEIR MECHANISM AND WRONG AS CONCLUSIONS

`OvlFunc_931_20086a4` called itself *"the first function PARKED BY PREDICTION
rather than by discovery"*. It reasoned from the basic-block lever — rebuilding
needs `REG_BASIC_BLOCK < 0`, that needs more than one block, this function has
no branch — and concluded "unreachable in plain C". Every step is correct *about
that lever*. It fails because a pin does not go through basic blocks at all: it
names the hard register, so placement is decided at assignment rather than by
liveness.

`OvlFunc_923_2008f48` diagnosed a `precompute_register_parameters` bind
precisely — `calls.c:805` copies every argument whose `rtx_cost` exceeds 2 into a
pseudo before any hard register is loaded, so both pool loads are precomputed and
the cheap `mov r0, #0` lands last. It was also **right** that no C expression
separates the two pool loads; they are the same cost by construction. One pin on
`r0` closes it, because a pin does not answer that question — it takes the
argument out of the precompute path entirely.

`OvlFunc_946_2009494` needed nothing new at all: one half was the precompute
bind, the other the mov/neg split, both closed earlier in this same batch.

**A prediction from a mechanism is only as wide as the mechanism.** That is now
six parks across four batches whose reasoning was sound about the lever it named
and was then recorded as a fact about C — after the dominating-branch rule
(193), the same-value rule (195), and these.

## PINS ARE STILL NOT FREE

The fifth measured case of a pin costing rather than reaching, on the parked
`2008de8`. Its residue is a **three-instruction rotation**, which is new:

    rom   str r3, [r6, #8] / mov r1, #0xf0 / ldrh r3, [r5, #6] / lsl r1, #8

A `do { } while (0)` barrier after the vector stores fixes the `mov r1`
placement the park was written about — and the residue becomes the `ldrh`
instead, still 2 of 84. `__asm__ volatile("")` is byte-identical to it. One
barrier buys the first instruction and loses the second.

The pin is the wrong tool there for a reason that follows from the rule: a pin
places the `mov` of the register it names, and this site needs a **load into a
different register** placed between that `mov` and its shift. Seven forms
measured, none better than the 2 already recorded.

The teardown removed a pin from three of the five elevations — every one was a
site where a smaller form was byte-identical.

## Park hygiene

`OvlFunc_936_2008504`'s park kept its measurements and **not its candidate
code**, pointing instead at a path under `scratch/`. That file happened to still
exist. A park that keeps its numbers but not its source cannot be resumed once a
scratch directory is cleared — the audit in batch 193 counted **123 parks with no
code**, and this is what that costs in practice. The body now lives in the
elevated file.

## New park

[`200c9a0`](src/non_matching/ovl_7e7574/200c9a0.c) at **165 lines against 166**.
gcc holds two shared pool constants across the body — `0x1999` in r6 and
`0x3333` in r8 — and spills a high register to afford them, where the ROM
reloads at all six argument positions and pushes only r5.

Pinning to force the reload **overshoots to 158 lines, eight short**. Pinning
only the register that actually holds the shared value at each site is
**byte-identical** to pinning the whole argument triple, so the overshoot is not
over-pinning: forcing the reload is itself what removes the instructions, and
the ROM's extra eight come from somewhere else. One instruction of the gap is a
branch-distance artefact — the ROM spells two early tests as `beq/b` pairs, the
form gcc emits when a conditional branch cannot reach — and that follows from
the body's length rather than from anything in the source.

## Correction to batch 195's park text

Batch 195 parked `2008ff0` and `2008b68` on the "crossed mov/shift order" wall
and both are stamped with the batch-195 correction. Nothing in this batch
changes that reading; the sub-case rule above is about a *different* residue —
an argument interleaved into a `mov`/`neg` build — and does not apply to the
crossed-shift shape, which remains unreachable.
