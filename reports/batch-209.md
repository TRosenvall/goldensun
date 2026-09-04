# Batch 209

Five elevated, four of them exact on the first screen. The batch spent its first
effort closing the source axis on a park and got a negative result; the rest was
selection paying off.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_943_20092f0` | `0x020092f0` | [ovl_30_…_a_a_b.c](src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_a_b.c) |
| 2 | `OvlFunc_882_2008f38` | `0x02008f38` | [ovl_30_…_c_a_b.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a_b.c) |
| 3 | `OvlFunc_942_2008328` | `0x02008328` | [ovl_30_c_c_a_c_a_c.c](src/overlays/rom_7c6bac/ovl_30_c_c_a_c_a_c.c) |
| 4 | `OvlFunc_881_2008a8c` | `0x02008a8c` | [ovl_30_…_a_a_b.c](src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_a_b.c) |
| 5 | `OvlFunc_924_200a684` | `0x0200a684` | [ovl_22c4_c_c_c_a_b.c](src/overlays/rom_7ac2d8/ovl_22c4_c_c_c_a_b.c) |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## A SINGLE EXIT WAS WORTH 86 INSTRUCTIONS

`2008328`'s ROM reaches `bl __CutsceneEnd` from four places by branch and skips
it entirely on one early-out. Written the natural way — `__CutsceneEnd()` at the
end of each of the two outer arms — the function comes out **152 lines against
150 with 86 differing** and the whole label structure shifted. One call at the
bottom, with a bare `return` for the path that skips it, is exact:

    if (flag) { ... } else { if (test) return; ... }
    __CutsceneEnd();

The single-exit lever is on file — `src/non_matching/ovl_7ced6c/2008f70.c`
measured it at 152 to 98 on five duplicated `return`s. This is the same thing
with a **call** rather than a return, which is worth adding because the tell is
different: a **length overshoot plus a shifted label structure**, not a
scheduling residue. A residue that large normally means the source is wrong
somewhere structural, and here it was.

## WHEN THE PINNED-MESSAGE-BASE LEVER IS *NOT* NEEDED

Batches 206–208 kept finding that a message id used twice needs
`register int m __asm__("r5")`, because constant propagation otherwise folds
`m` and `m + 1` into two separate pool entries. `2008328` uses one three times —
`m`, `m + 1`, `m + 2` — and needs **no pin at all**.

The difference is the spelling of the id. Here it is a linker symbol,
`extern int _MSG_1d20;` with `(int)&_MSG_1d20`, and a symbol address is not
something cprop can fold into distinct constants. The siblings that needed the
pin had plain integer bases.

**The spelling of the id decides whether the pin is needed** — so read what the
ROM pools before reaching for the lever.

## THE PARK'S SOURCE AXIS IS CLOSED

`2008d5c` sits at 2 of 139 on a load transposed against an immediate `mov`.
Three more spellings this batch — the OR's operands swapped, the expression split
into `&=` then `|=`, and the negation named in a second local — all score 2,
bringing it to **seven structurally distinct forms tied**. They vary declaration,
assignment, operand order and statement count, which is this tree's own standard
for a tie meaning something.

The flags then said which pass owns it:

    --no-rerun-cse        2 differing   (CSE is not involved)
    --no-schedule-insns2  27 differing from instruction 16

So it is sched2, and **turning sched2 off does not put the pair back** — it
breaks correct work elsewhere. The ROM's stream is not gcc's unscheduled stream.
That is the same conclusion `src/non_matching/rom_8a000/809802c.c` reached from a
completely different residue (where the prologue's `sub sp` lands), and it puts
the two in one category: a sched2 decision with no source handle and no flag
that isolates it.

## THE CROSSED VERDICT USED PROSPECTIVELY

`tools/crossed.py` reported `BARRIER` on two of the five during **selection**,
and in both cases the barrier was written into the first draft from the listing
rather than discovered by iterating. Both screened exact first time.

That is the verdict working as batch 206 rewrote it — a route to the lever, not a
reason to skip the candidate. It is also the difference between `2008f38` and its
parked sibling `2008d5c`: same overlay, same family, same guarded tail, and the
one whose crossed site was anchored before the first compile is elevated.

## SELECTION IS COMPOUNDING

Every candidate was picked on the same two filters as the last two batches —
`tools/templated.py` ranked on template quality, filtered to `hi == 0`. Four of
five were exact on the first screen.

Worth noting *what* the templates now are: the neighbours `templated.py` matched
against are files written in batches 207 and 208. `2008f38` is the third member
of a family already worked twice, and every lever it needed came from the two
siblings. The corpus is training itself.

## SMALLER, ALL MEASURED

**A call-clobbered register can force a build's order for free.** `200a684`
stores `0x80 << 9` through a fetched actor and gets the ROM's `mov r3 / lsl r3`
from the inline expression, with no need to split it into statements — because
r3 cannot survive the `bl`, so the build has to follow the call anyway. That is
the one place the usual shifted-constant cure is unnecessary.

**A statement duplicated in both arms is not a tidying opportunity.** `200a684`
increments the same halfword at the end of the true arm and the start of the
false one; hoisting it after the `if` would not match. Same class as the
duplicated call recorded in batch 208.

**Three copies of a block beat a loop, again.** `2008a8c` walks three actors to
the player with the identical six-statement sequence and only the slot number
changing. Fourth batch running that a transcribed sequence is what matches.

**Stack-argument pairs still need two named locals.** `20092f0`'s
`__CopyMapTiles` passes two on the stack and both must be live at once. Its ROM
also builds `0x6c` twice rather than copying it between registers, which two
plain literals give.
