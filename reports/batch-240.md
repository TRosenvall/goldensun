# Batch 240 — eight for eight, and a rule that does not scale

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_963_20083c4` | `0x020083c4` | [ovl_30_c_c_a_c_d.c](src/overlays/rom_7ec968/ovl_30_c_c_a_c_d.c) |
| 2 | `OvlFunc_956_200a330` | `0x0200a330` | [ovl_30_c_c_c_c_c_a.c](src/overlays/rom_7e0928/ovl_30_c_c_c_c_c_a.c) |
| 3 | `OvlFunc_930_2008c30` | `0x02008c30` | [ovl_30_c_c_a_c_c_c_c_a.c](src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_a.c) |
| 4 | `OvlFunc_930_2008b2c` | `0x02008b2c` | [ovl_30_c_c_a_c_c_c_c_a.c](src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_a.c) |
| 5 | `OvlFunc_924_200b860` | `0x0200b860` | [ovl_35b8_a_a_c_a_c_a_b.c](src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_b.c) |
| 6 | `OvlFunc_924_200b948` | `0x0200b948` | [ovl_35b8_a_a_c_a_c_a_b.c](src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_a_b.c) |
| 7 | `OvlFunc_966_20087c4` | `0x020087c4` | [ovl_30_c_c_c_a_a_c_b.c](src/overlays/rom_7f148c/ovl_30_c_c_c_a_a_c_b.c) |
| 8 | `OvlFunc_938_2008360` | `0x02008360` | [ovl_30_c_c_c_c_c_c_c_a_a.c](src/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_a_a.c) |

Seven targets, eight functions, **no parks**. Entry 4 was not a target — it
was the other function in a file opened for entry 3, the fourth such this
week.

Entry 8 is the largest function elevated in this project so far: 1649
instructions, 4268 bytes, 138 pins.

Five of the seven targets were chosen for **zero r8–r11 traffic**, and all
five matched. That predictor continues to earn its place at the front of
target selection.

## THE FAMILY-NOMINATION RULE DOES NOT SCALE UNAIDED

Batch 239 recorded that nominating pin candidates by call-site family —
same callee AND same argument shape — made a 630-instruction function exact
**with no residue read at all**. This batch is where that bound gets found.

On entry 8, at 1649 instructions, the rule nominates 4 extras. All 4 survive
minimisation and each costs 2–3 when dropped, so the rule is genuinely
right. But the family set **alone** leaves 39 differing, and 29 after the
descending fills.

It is a strong first pass, not a substitute for the residue loop. Recorded
as a qualification rather than a retraction — but the honest version is
uncomfortable: at the size where it would save the most, it saves least.

## A FAMILY WHOSE ARGUMENTS ARE ALL CHEAP IS NOMINATED BY NEITHER RULE

On entry 7 the entire residue after the family set was **one site** whose two
arguments are bare `mov #imm8` — no repeated constant to trigger the CSE
rule, no expensive argument shape to trigger the family one. It needs an
ordering pin all the same.

Widening to EVERY multi-argument site — 136 pins, uniform ascending — leaves
exactly the same two encodings and nothing else. That is the strongest
evidence yet that the uniform ascending fill is **correct rather than merely
cheaper**: simultaneously right at 135 of 136 sites.

## THE RELOCATION PARTITION, ON THE LARGEST SAMPLE YET

Batch 239 found it on 48 pins. Entry 8 has 138, and the partition has zero
overlap and no middle:

| | count | encodings | SIZE silent |
|---|---|---|---|
| ORDERING (relocations silent) | 52 | **exactly 2 or 3** | 52 of 52 |
| CSE loss (relocations differ) | 86 | 10 – 1633 | 47 of 86 |

Two refinements. The CSE floor is **10**, not the 23 recorded. And the
ordering band is tighter than "small" — a residue of 4 to 9 belongs to
neither band and **did not occur once** across 138 drops.

It held on every function in this batch that has pins: 17 drops on entry 3
split 5 CSE against 12 ordering with only 2 of the 5 moving SIZE at all; 27
drops on entry 1 split 4 against 23 with 26 of 27 at size zero.

## A TWO-SITE CSE CLASS IS BROKEN FROM EITHER END

Recorded: pin the FIRST use, because pinning later sites while the first is
open buys nothing. On entry 1, a value with exactly two uses separated by a
join:

| pins | result |
|---|---|
| neither | 130 differing, relocations differ |
| first only | 2 differing, relocations silent |
| **second only** | **exact** |
| both | exact — so the FIRST pin is the inert one |

A call-clobbered destination dead across the next `bl` cannot be fed by a
commoned pseudo either, so killing the class's LAST end kills it as
thoroughly as its first. What is asymmetric is the fill order: the second
site independently wants a particular argument order, and once that pin
exists for ordering it does the CSE job free.

First-use remains the safe default. The point is that a first-use pin **can
be the redundant one**, so the sweep must be allowed to remove it — which it
only is if it runs to a fixpoint rather than stopping at the first exact
candidate.

## THE LEVER THAT MATTERED ON ENTRIES 5–6 WAS REMOVING ONE

Both match with **no pin at all**. Every pin added was paying for the wrong
loop shape. The template's headline `goto` inner loop was transplanted, which
then needed a pin to fix a swapped high-register pair — reaching 6 encodings
with SIZE and RELOCATIONS silent, a textbook ordering signature, and a trap.
The residue was two permuted `mov rHIGH` copies that **no source order can
flip**, because one was a source assignment and the other an LICM hoist.

Writing the loop as an ordinary `do`/`while` and deleting the pin matched
next try.

Mechanism: with a real loop, LICM hoists BOTH constants itself, in
loop-discovery order, which is the ROM's order. Naming one converts it from a
hoist into a preheader assignment emitted BEFORE the hoist — so naming does
not order the pair, it **inverts** it.

Two consequences worth keeping:

**A single-use loop invariant is not a `goto`-loop tell.** gcc-2.96 declines
to hoist an invariant used once, since rematerialising a shift beats spending
a callee-saved register — so a single-use invariant is rebuilt in an ordinary
loop too. The converse is the useful test and was unrecorded: **a live giv in
the ROM proves the loop is ordinary**, because strength reduction is exactly
what a `goto` loop turns off.

**Naming a constant can REMOVE a callee-saved register, not add one.** Naming
a bias dropped the push set and rematerialised its pool load twice inside the
loop. The bound is the constant's rematerialisation cost: a single `ldr` from
the pool loses to a register the moment it has a pseudo, while a
two-instruction build does not.

## SUBSCRIPT THE ARRAY AS DECLARED

Two escapes from the `symbol+offset` pool fold are recorded — change the
extern's element type and index it, or let the index carry a loop variable.
Neither covers a **declared element type with a constant index**, and there
the two C-equivalent spellings are not equivalent to gcc-2.96:

    gState[0xf9 << 1] = 1;          exact
    *(gState + (0xf9 << 1)) = 1;    55 differing, one insn SHORT

`gState[498]` and `gState[0xf9 * 2]` also tie at zero, so it is the tree
shape and not the literal. **And the cast form is not a subscript** — casting
the array before indexing folds exactly like the pointer form, because the
cast decays the array before the index applies.

Entry 2 carries both polarities, which is what makes it convincing: its
halfword guard NEEDS a local base (147 differing without), and its byte store
must NOT go through that base — carrying it across the body takes a fifth
callee-saved register, 156 differing and four bytes longer.

## PROTOTYPES ARE PER-SITE, NOT PER-FILE

On entry 2, two callees in one family want their prototypes WITHHELD because
the ROM puts r0 last; a third, overlay-local, puts r0 in the middle and
REQUIRES one. The template's r0-last list is right about the two it shares
and silent about the third.

## A SECOND DESCENDING CALLEE BY NAME

`__Func_8093054` joins `__Func_8092c40`. It is CSE-nominated, so it is easy
to pin and get wrong: pinned ascending measures exactly the same as unpinned.

And `__Func_8092c40` wanted descending **four times in one function** on
entry 8, against the recorded lone site — while entry 7 proved the same
callee is a SITE property *internally*: two calls 106 apart, one wanting no
pin and one wanting descending.

## A CORRECTION TO BATCH 237

That batch added a blockquote claiming the typed field and the named
destination pointer are one lever for `orr`. They are not. On entry 3, same
single call and same address computation:

| spelling | differing |
|---|---|
| `->f5a \|= 1` / `e->f5a \|= 1` | **0** |
| `bp = &e->f5a; *bp \|= 1;` (batch 237's form) | 2 |
| four other pointer spellings | 2 |
| the recorded narrow-local remedy | 4 |

The discriminator is **aggregate member reference versus dereferenced
pointer** — not address computation, not value width. Batch 237's result
stands for its own case, where the plain form cost a second call to recompute
the object. What does not stand is the generalisation, and the doc banner is
amended in place.

## Landing notes

**A manual split, because `split_s.py` cuts around one function.** Entries
5–6 are the leading contiguous half of a FOUR-function `.s`, so the two had
to be kept together by hand. Instruction lines conserved exactly, 393 = 233 +
160, and the layout proved byte-neutral with both pieces still in assembly
before either `.c` went in.

One detail worth recording: the `.s` header comment describes the FIRST
function, so copying the file head into the tail piece would have left a
wrong description at the top of a file that no longer contains it. Copy the
`.include` lines, not the head.

**A flag-group near-miss.** Entry 3 matches at the tree default `-O2`; at
`-O1` the combined object is 309 of 447 differing. Its immediate neighbours
in the same overlay ARE `-O1`, under a pattern whose own Makefile comment
records that it used to read a prefix which **would have captured this
stem**. Do not re-broaden it.

**A `.data` line for a section that does not exist**, twice more — entries 2
and 8. Both keep their `.o` name, so neither needed an edit, but both had to
be checked rather than assumed.
