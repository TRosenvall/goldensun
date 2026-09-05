# Batch 224 — the cure depends on the prologue, and the loop depends on gcc

Six functions elevated, five of them from agent screening and all six
byte-exact. Two carry no register pins at all.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_953_200a668` | `0x0200a668` | [ovl_30_…_a_c_c_b.c](src/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_c_c_b.c) |
| 2 | `OvlFunc_895_20097c0` | `0x020097c0` | [ovl_30_c_c_c_a_c_c_c.c](src/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_c.c) |
| 3 | `OvlFunc_922_20092cc` | `0x020092cc` | [ovl_30_…_c_a_c.c](src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_a_a_b_c_a_c.c) |
| 4 | `OvlFunc_924_200ca08` | `0x0200ca08` | [ovl_35b8_a_a_c_a_c_c_b.c](src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_c_b.c) |
| 5 | `OvlFunc_953_200a964` | `0x0200a964` | [ovl_30_c_c_c_c_a_c_c_c.c](src/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_c_c.c) |
| 6 | `OvlFunc_943_200bc88` | `0x0200bc88` | [ovl_30_c_c_b.c](src/overlays/rom_7c7b9c/ovl_30_c_c_b.c) |

## The prologue rule, confirmed from the other side

Batches 222–223 established that a `push {lr}` ROM keeps nothing, so only pins
reach it. This batch produced the opposite cases and they behave as predicted:

- **`OvlFunc_895_20097c0`** pushes `{r5, r6, r7, lr}` — wide — and matched with
  **no pins at all**, on named locals. Naming the values the ROM *does* hoist
  destroys the hoist: doing it at all twelve marker sites narrows the push and
  costs 282 of 329 differing.
- **`OvlFunc_922_20092cc`** likewise, three named locals and nothing pinned.
- **`OvlFunc_943_200bc88`** is the interesting middle: its `push {r5, r6, lr}`
  holds **two bitmasks**, not script constants. So the masks must stay bare
  literals while every script constant needs a pin. Reading *what* the prologue
  keeps, not just how wide it is, is what made it tractable.

## Which local, not how many

The recorded stack-argument rule says the `str` operands tell you *how many*
values want names. `OvlFunc_922_20092cc` adds the harder half.

A C local gets one pseudo for the whole function, so its register is chosen once
and every use inherits it. That function needs r3 for the value stored to `[sp]`
in its first half and r3 for the value stored to `[sp,#4]` in its second, because
the ROM holds a different member of the pair in r5 on each side. **The split is
by rebuilt-versus-held, not by argument position.**

Splitting by position instead keeps the length exact and costs 26 differing —
*every one an r2/r3 swap, and all of them in the first half*, because in the
second half the two splits coincide. **That asymmetry is the diagnostic**: a
function exact in one half and register-swapped in the other is asking which
pseudo the value shares with the rest of the body.

## A ROM loop with the test at the top is not a `while`

`expand_end_loop` rotates a `while` into a bottom-test loop — one instruction
longer, and the whole tail shifts. `for (;;) { if (!c) break; }` lowers
identically and is no help. `do { if (c != 1) break; … } while (1);` suppresses
the rotation: 262 lines and 111 differing → exact on `OvlFunc_943_200bc88`.

A related shape on `OvlFunc_924_200ca08`: where the loop is a *re-ask* whose two
entries differ by a constant, `while (1)` with the break in the middle and a
variable carrying the differing id puts gcc's loop label where the ROM has it.
A duplicated preamble plus a `while` leaves the cross-jumper merging back only
as far as the call — several instructions short. 122 differing → 9 in one step.

## A commutative op ties its destination to whichever operand is written first

`OvlFunc_943_200bc88` ORs the same held constant into a byte at two sites, and
the ROM emits them with **opposite tied operands** — `orr r3, r6` where the
register is still live, `orr r6, r3` where it dies. `orr` is commutative with two
register operands, so gcc ties the destination to whichever is written first.
One shared variable, two source orders:

    r[0x5a] |= one;                    /* first site  */
    one |= r[0x5a]; r[0x5a] = one;     /* second site */

And it must be `register int one __asm__("r6")` — as a plain `int`, gcse's cprop
substitutes the constant back into the second block, exactly the mechanism found
last batch.

## Do not transcribe the ROM's shift order

`OvlFunc_953_200a668` shows this in its strongest form. Five `SetSpeed` sites
take the *same* constant pair, and the ROM emits three of them one way and two
the other — the shift and the slot `mov` swap places. Written to match each
site's emitted order, the two odd ones come out with their **movs** reversed.
Written with the same uniform source form as their siblings, all five are exact:
**sched2 produces both emitted orders from one spelling**, depending on context.

A site that looks different from its siblings in the ROM is not necessarily
different in the source.

## The one-at-a-time pin list is a set of candidates, not of removals

Third and sharpest measurement of "N pins is a size, not a set". On
`OvlFunc_953_200a668`, ten of twenty-two pins are individually inert, removing
all ten is 169 lines against 164, and a greedy pass takes out only **eight** —
the last two break once the other eight are gone, having been inert only while
those were present. `OvlFunc_943_200bc88` needed the same treatment (15
candidates, 14 removable) and converges on *different sets* depending on which
end of the list you sweep from.

So: strip one at a time to get candidates, then remove greedily with a re-test
after each drop, and stop at the fixpoint.

## When a function has a twin, diff the disassembly first

`OvlFunc_953_200a964` is byte-identical to `OvlFunc_953_200a668` in everything
but **two constants**. Taking the finished sibling and substituting them matched
on the first screen. Screening it independently would have re-derived fourteen
pins, a shift-order correction at three sites and a greedy minimisation over
twenty-two candidates to reach the same file.

The levers are recorded in one twin and the other points at it, so the two
records cannot drift apart.

## On trusting templated.py's 1.00 rows

Two functions here had a 1.00 neighbour built on only **three** shared symbols,
which the tool's own docstring calls near-coincidence. They went opposite ways:
`OvlFunc_895_20097c0`'s pick was genuinely weak and a shared-callee ranking found
a much better same-directory sibling worth 36 differing lines;
`OvlFunc_922_20092cc`'s pick was right, but for a reason the score does not
express — it is the immediately preceding sibling in the same split.

A three-symbol 1.00 is not evidence either way. Rank by shared-callee count
yourself, and check whether the neighbour is a split sibling.

## Where the project stands

3,707 functions elevated. The remaining work, counted carefully — an earlier
version of this section said "1,699 still in hand-written asm", which
**mislabelled** the number:

| | count |
|---|---|
| Functions still in `.s` | 1,699 |
| — genuinely hand-written asm (unreachable; `hand_written()` skips them) | 76 |
| **Attemptable** | **1,623** |
| — already attempted and parked | 425 |
| — **never attempted** | **1,198** |

"Hand-written asm" is a specific, small category: 76 functions authored in
assembly with no C to recover, which `tools/filtered.py` deliberately excludes.
Using that phrase for the whole remainder wrongly implies all 1,699 are
unreachable. The number that answers "left to attempt" is **1,198 never
attempted**, plus 425 parked ones that remain open — and parks do fall, as
batch 223's unpark of a specimen closed after thirteen flag measurements showed.

The 51 ARM functions sit inside the attemptable set and need a different
approach.

## Discipline

`tools/guard_generated.sh` fired on four of this batch's six commits, every one a
single-function `.s` converted whole. One `git add` aborted on a predicted split
suffix (`_c` where the product was `_a`), staging nothing — the recurring
reminder to **list a split's products before constructing the paths** rather
than guessing them.
