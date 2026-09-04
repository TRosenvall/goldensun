# Batch 193

Six elevated. **Five of the six came out of `src/non_matching/`** — parks
written in earlier batches, re-screened and withdrawn. Only one was a fresh
target, and that one is why the other five fell.

The batch's result is a **correction to a documented blocker class**. Batch 188
closed "constant rematerialisation" by reading the compiler, concluded it
requires a *dominating branch*, and instructed the reader to park immediately
where there is none. The reading of the passes was right. The conclusion was
wrong, and it had been sending functions to the park directory unattempted ever
since.

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `OvlFunc_943_2009a98` | `0x02009a98` | [ovl_30_…_a_a_c_b.c](src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c_b.c) | **fakematch**; *previously parked*, withdrawn to the interleaved form |
| 2 | `OvlFunc_919_200805c` | `0x0200805c` | [ovl_30_…_c_c_b.c](src/overlays/rom_7a67d8/ovl_30_a_c_c_c_a_c_c_c_b.c) | **fakematch**; *previously parked*; pins are not only for constants |
| 3 | `OvlFunc_930_2008870` | `0x02008870` | [ovl_30_…_c_a_b.c](src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_c_a_b.c) | **fakematch**; *previously parked* as "not reachable from the source" |
| 4 | `OvlFunc_963_2008334` | `0x02008334` | [ovl_30_c_c_a_c_c.c](src/overlays/rom_7ec968/ovl_30_c_c_a_c_c.c) | **fakematch**; the function that corrected the rule |
| 5 | `OvlFunc_902_200811c` | `0x0200811c` | [ovl_30_…_c_a_a.c](src/overlays/rom_7987ac/ovl_30_c_c_a_a_c_c_c_a_a.c) | **fakematch**; *previously parked*; closed its own stated test case |
| 6 | `Anim_UnleashIntro` | `0x080ccaec` | [rom_cc5d8_a_a_b.c](src/rom_c9000/rom_cc5d8_a_a_b.c) | **fakematch**; *parked twice*, at 2 of 80 both times |

Gated on a clean `make clean && make compare`, every address verified against
`goldensun.elf` and the per-overlay `overlay.elf`.

## THE DOMINATING-BRANCH RULE WAS WRONG, AND IT WAS LOAD-BEARING

`OvlFunc_963_2008334` is sixty instructions of straight-line cutscene setup
with **not one conditional branch anywhere**. The ROM rebuilds
`mov #0x80 / lsl #9` at each of two calls, and three more constants at four
others. By the recorded rule this is unreachable and should be parked on sight.

The plain spelling behaves exactly as the rule predicts — 35 of 60, length
already exact, the constants hoisted into `r5`/`r6`, the prologue widened to
`push {r5, r6, lr}`, every later call fed by `mov r1, r5`. Writing the literals
inline instead of naming locals is **byte-identical**, which confirms the cse1
half of the batch-188 analysis: cse1 commons the repeat unconditionally and no
source spelling defeats it.

Pinning the argument registers matches on the first try:

    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");
    p1 = 0x80; p2 = 0x80; p0 = 8; p1 <<= 9; p2 <<= 8;
    __MapActor_SetSpeed(p0, p1, p2);

**Why the rule mis-stated its own boundary.** What the shape needs is for the
constant to be *dead* at the next use, so nothing can be copied forward. A
dominating branch is one way to arrange that, and it was the only way anyone had
tried — so it got written down as the requirement. It is not. `r0`–`r3` are
call-clobbered: a constant pinned there cannot survive a `bl`, so gcc has no
choice but to rebuild it, and no branch is involved anywhere.

The inspection test keeps its value with its verdict flipped: **no dominating
branch means the ordinary spellings are hopeless — go straight to a
call-clobbered pin, do not sweep.** Where a dominating branch *does* exist the
plain form still works and costs no fakematch, so prefer it.

The batch-188 scan that found "three instances, all spill cases, no solved
precedent" was searching *generated* asm and could not have found this, because
until now no such function had been solved. **A scan over solved output cannot
discover a class that has never been solved** — it can only ever confirm the
current frontier.

## FIVE PARKS FELL, AND THEY ALL FAILED THE SAME WAY

Every one of the five withdrawn parks reasoned carefully and reached a wrong
conclusion by generalising a lever's boundary into a wall.

`OvlFunc_902_200811c` is the sharpest, because it named itself as the test case
for its own release: *"If the interleave class is ever reached for
straight-line sites, this function is a two-instruction test case."* It had
checked for a dominating branch, correctly found the only conditional sits after
the call, and parked on the rule as written. It was defeated by the
documentation, not by the compiler.

`OvlFunc_930_2008870` claimed its placement was *"not reachable from the
source"* after five spellings. All five varied order among **ordinary locals**,
for which the claim is true — gcc fixes argument setup after those choices are
made. A pinned register is not an ordinary local: it has two independent knobs,
declaration position and assignment position, and the second reaches placements
ordering cannot.

`Anim_UnleashIntro` had the diagnosis exactly right and still could not act.
Its second park observed that the basic-block lever was *already* splitting the
pair, and that *"which of the remaining arguments gcc schedules into the gap is
a separate question the lever does not answer."* That is correct, and it is
precisely why a pin was needed: seven spellings had been compiled by then, and
every one still left the choice to gcc.

**The test that would have caught all five: name the structural assumption every
attempt shared.** Batch 192 wrote that rule after one park; this batch is five
more instances of it. In each case the shared assumption was *"the value is an
ordinary local and gcc picks the register."*

## THE PIN'S BOUNDARIES, MEASURED IN BOTH DIRECTIONS

Two parks were re-screened, failed, and were **re-parked with sharper
diagnoses**. They matter as much as the withdrawals, because a lever this
productive invites over-reading.

`src/non_matching/ovl_7ac2d8/200cf44.c` — the pin *reaches* the instruction the
old park despaired of, and pays for it elsewhere:

    ROM        ldr r3 / mov r1,#210 / mov r2,#150 / lsl r2 / mov r0,#11 / lsl r1
    baseline   ldr r3 / mov r1,#210 / mov r2,#150 / lsl r2 / lsl r1 / mov r0,#11
    any pin    ldr r3 / mov r2,#150 / mov r1,#210 / lsl r2 / mov r0,#11 / lsl r1

The baseline gets the head right and the tail wrong; any pin on `r0` gets the
tail right and flips the head. Both are 2 of 28; the ROM is a third state
neither reaches. **Seven structurally distinct forms tie** — pin counts of one,
two and three, different registers pinned, initialised against uninitialised,
three spellings of the arguments. So: the pin's knobs move *its own* register's
`mov`, and do **not** order two other movs the post-allocation scheduler may
swap.

`src/non_matching/overlays/common1_148.c` — the pin lands on **exactly the same
4 differing** as a plain `int` local. Landing on the identical number is the
informative part: the pin is not being ignored, it is paying the same cost,
because the cost was never about which register or how it was requested. The
value must be live across the store and this function has nothing to spare.
**A pin is not a way around register pressure.**

## TOOLING AND TREE

`tools/split_s.py` **silently overwrote an existing destination.** Its suffixes
are hard-coded `_a`/`_b`/`_c`, but once a neighbouring function in a stem has
been elevated, a *generated* `<stem>_b.s` sits in `asm/` under exactly the name
the tool wants. It replaced one holding the already-elevated
`OvlFunc_963_2008730`, and `make compare` then failed at char 57 of the overlay
— which reads exactly like a bad decompilation. I suspected the candidate C
first. The tool now refuses, naming what is in the way and whether it is
generated, since a generated one is the build input for a function already done.
That split was completed by hand into free suffixes `_c`/`_d`.

**A park's filename is not an address.** `src/non_matching/rom_c9000/cc5d8.c`
and `asm/rom_c9000/rom_cc5d8_a_a.s` both say `cc5d8`; the ELF puts
`Anim_UnleashIntro` at **`0x080ccaec`**. `cc5d8` is where the *file's region*
starts. This is the batch-189 lesson in a new disguise — that batch found four
wrong addresses, all of them real-named symbols, because an address-named symbol
carries its own answer. `Anim_UnleashIntro` is real-named, and the wrong address
came from the filename rather than from a neighbour.

**A park keeps its measurements but not always its candidate code.** An attempt
to resume `ovl_79b154/2008ed8.c` reconstructed the body from the ROM and landed
at 32 differing against a recorded 1 of 44 — the park had no code to resume
from. An audit: **423 parks carry candidate code, 123 do not**, and 22 of the
latter have small recorded residues that cannot be picked up cheaply. Check for
code before quoting a park's number as a starting point.

**Where the tracked generated `.s` files come from.** Two commits this batch
briefly contained one. The mechanism: `git rm` the hand-written `.s`, then the
build **regenerates a file at that same path** while compiling the new `.c`, so
the next `git add` re-adds it as a modification. This is almost certainly how
the **~315 tracked `.s` files carrying `.gcc2_compiled.`** got there, in
contradiction of BRANCH.md. Still awaiting a decision; no `.gitignore` entry
covers them.

## The pool of remaining work in this class

**59 parks** reference the dominating-branch / constant-rematerialisation
blocker. Three of this batch's withdrawals came from that list, all matching on
the first pinned screen. The remainder is the most promising target list the
project currently has, and the ones with small recorded residues that still
carry code should be taken first.
