# Batch 197

Five elevated, **all five withdrawn from `src/non_matching/`**, no new park.

The batch has one result and it is a generalisation rather than a lever: across
five different optimisation passes, the parks were right about the mechanism and
wrong that the site was unreachable, for the same reason each time. **A pin does
not argue with a pass — it avoids the pass.**

## Function breakdown

| # | function | address | file | pass it was parked behind |
|---|---|---|---|---|
| 1 | `OvlFunc_939_2008c74` | `0x02008c74` | [ovl_314_a_c_c_a_c_c.c](src/overlays/rom_7c460c/ovl_314_a_c_c_a_c_c.c) | the `neg` interleave |
| 2 | `OvlFunc_974_2008b10` | `0x02008b10` | [ovl_30_…_a_c.c](src/overlays/rom_7fcd20/ovl_30_c_c_a_c_a_c_a_c.c) | partial redundancy elimination |
| 3 | `OvlFunc_909_2009958` | `0x02009958` | [ovl_30_…_c_b.c](src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_b.c) | argument-setup ordering |
| 4 | `OvlFunc_952_20083b0` | `0x020083b0` | [ovl_30_c_a_a_c_a_a.c](src/overlays/rom_7d768c/ovl_30_c_a_a_c_a_a.c) | sched2 ready-list tie-break |
| 5 | `OvlFunc_959_200a1c4` | `0x0200a1c4` | [ovl_9dc_…_a_c.c](src/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_a_c.c) | duplicate-constant CSE |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## A PIN AVOIDS THE PASS RATHER THAN ARGUING WITH IT

`OvlFunc_952_20083b0` carried the most carefully argued diagnosis in the park
directory, and every particular of it is correct. Thumb cannot load `0xe000` in
one instruction, so the constant is `force_reg`'d in
`precompute_register_parameters`, which means the `lsl` **always** has a lower
LUID than `mov r0, r5`. The park read sched2's ready list, showed that an
identically-shaped call twenty instructions earlier comes out right because its
dependent counts are 3 against 4, and identified why this one ties at 4 — an
intervening call between the `bl` and the next explicit write of `r1`.

From that it concluded the shape *"rules out every reorder-the-source lever
before it is tried"*. **That is true.** Every lever it had in view reorders the
source, and the LUID order is fixed before source order can matter. A pin is not
one of them: it names the hard register, so the argument is never `force_reg`'d
into a pseudo and the path that creates the LUID ordering is never entered.

`OvlFunc_974_2008b10` is the same story against a different pass. Its park showed
that three named `int` locals holding `-0x64` — declared at the top, or
immediately before each use — are byte-identical to the bare literal at 55
differing, because partial redundancy elimination folds the three initialisers to
one rtx *before* the per-use-site naming lever can matter. Correct, and again a
pin does not engage with it. **55 differing to 7, length exact.**

**The check to apply:** when a park's argument ends in *"no ordering of the
source reaches this"*, ask whether a pin avoids the pass rather than arguing with
it. That is now seven parks across five batches — the dominating-branch rule
(193), the same-value rule (195), the basic-block prediction and the precompute
bind (196), and these.

## A QUALIFICATION TO BATCH 196'S DISCRIMINATOR

Batch 196 wrote, in the `2008120` park, that a `mov` with no consuming operation
has nothing to order it. That park's two zeros are genuinely unreachable and the
rule predicted two elevations correctly. It was still stated **too broadly**.

The last step of `OvlFunc_959_200a1c4` is a three-register argument fill the ROM
runs *backwards* — `mov r2 / mov r1 / mov r0`. All three are constants, two are
zeros, none has a consumer, and the pin orders them without difficulty.

The difference is what is being asked:

- In `2008120` the movs must be placed **inside another register's
  two-instruction build**, between its `mov` and its `neg`.
- Here they only have to be ordered **among themselves**.

A pin fixes where its own register is written relative to other pinned writes,
which is enough for the second and not for the first. **The rule is about
interleaving into a build, not about consumers in general**, and both files now
say so.

## Smaller results

**Only one of two adjacent records advances.** In `2008b10` the ROM does
`add r0, r2 / strb [r0]` in place for the first unit record — the pointer-advance
tell — and applying the same form to the second is 67 lines against 64. The two
blocks are written differently on purpose; read each store off the ROM rather
than making a pair consistent.

**`tools/crossed.py` earned its place.** `OvlFunc_959_200a06c` looked like an
ideal candidate — same CSE class, same overlay as #5 — and the filter rejected it
because its third call crosses mov order against shift order. That is the wall
from batch 195, and the filter cost one command instead of a sweep.

## Park hygiene, twice

`2008c74` **kept its measurements and not its code**, so the body was rebuilt
from the disassembly. The reconstruction screens at exactly the 2 of 53 the park
recorded, which is the only available evidence that it is the same candidate the
park was describing — and the reason to check that number before trusting a
rebuilt body. Second such park in two batches; the batch-193 audit counted 123
with no code.

`2009958`'s recorded `.s` path **no longer exists** — that file was split since,
and `tryc` reported "no asm counterpart" rather than a bad screen. Locate a
park's asm by grepping the function name, never by the path in its header.

Both are cheap to fix at write time and expensive to hit later.
