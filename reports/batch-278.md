# Batch 278 -- twelve functions, and five distinct allocation entry points

Gated on a clean `make clean && make -j8 && make compare`, green at the target SHA1
`5c4695205413df7db52b9a184815a07783999971`. All twelve addresses checked against the
freshly linked ELF, along with every function the splits left in assembly. Three control
symbols came back absent.

| | |
|---|---|
| elevated | **12** |
| parked | 8 new, 4 advanced, **4 retired** |
| `.sym` entries added | **1** (`_MSG_1197`, and it REPLACED a pin) |
| splits | 5 |
| **fakematch debt added** | **3** |

**Six subagents, 21 targets.** Twelve functions from ten solved targets -- the difference is
duplicate groups, where one solution converts two.

| agent | assignment | landed |
|---|---|---|
| B | `898`/`901`, `916`/`947`, `common1` pairs | **4** of 4 targets |
| C | highest-neighbour overlay singles | **3** of 4 |
| E | `rom_8a000` cluster | **3** of 4 |
| A | the `923`/`924` family | **2** of 4 |
| D | next overlay singles | 0 of 4 |
| F | the x15 group | 0 of 1 (44 -> 25) |

## Allocation now has five entry points, and they want different moves

This is the round's real output. Batch 277 gave the `global_alloc` priority formula; this
batch found four more places the decision can be made, and the triage matters more than any
one function.

| symptom | pass | move |
|---|---|---|
| registers permuted, same frame | `global_alloc` priority | raise/lower, or **make the value block-local** |
| frame LARGER than the ROM's | LICM / live ranges | keep fewer values live |
| a copy the ROM has is deleted | `local_alloc` | change which value claims the register first |
| an address pseudo cannot get r3 for no visible reason | **a dead insn from `combine`** | see below |
| only scratch registers rotate, structure exact | **`order_regs_for_reload`** | nothing source-level |

### A value can LEAVE the race instead of winning it

`local_alloc` runs first, so a quantity whose live range sits inside one basic block never
enters greg's priority list. On the x15 function that was the answer, because **winning was
arithmetically unavailable**: the competitor scored 0.771 against 0.643 and could not be
lowered -- all sixteen of its references were enumerated from the RTL and every one maps to an
instruction the ROM also has.

Moving one addition earlier made the value block-local, `local_alloc` gave it the ROM's
register, and the two neighbouring values were forced onto the ROM's registers too. **42
differing to 22.** The placement is load-bearing: the same split with the addition after the
intervening stores stays at 37.

### A `REG_UNUSED` insn still takes a hard register

**gcc-2.96 runs no flow pass between `combine` and allocation.** On `OvlFunc_923_2009bc8` the
chain is visible end to end -- expand emits a QImode subreg set, cse1 rewrites it to a
constant, combine folds the subreg into the store and leaves the insn `REG_UNUSED`, and it is
*still there* at `.17.lreg`. `local_alloc` gives it r3 (first in ARM's `REG_ALLOC_ORDER`), and
two independent address pseudos carry the identical conflict list including hard `3`.

If that one dead insn were gone the function would be exact. **The competitor is not a real
value at all**, which is why no spelling of the real values moves it. Diagnose by grepping
`.17.lreg` for `REG_UNUSED` against `.18.greg`'s conflict lines.

### And two corrections to how I read allocation residues

- **`allocno_compare` breaks ties on ALLOCNO NUMBER, lower first.** So a residue called "a
  coin-flip" is not one -- if the value you want has the lower number, **equality suffices**.
- **"A coin-flip in `allocno_compare`" turns out to be a description of not having computed
  the gap.** On the x15 function it was 0.771 against 0.643, not close, and a source change
  moved it.

## What landed

| | function | source |
|---|---|---|
| 1 | `OvlFunc_common1_1608` | [common1_a_c_c_c_b.c](../src/overlays/common/common1_a_c_c_c_b.c) *(park closed)* |
| 2 | `OvlFunc_948_200a0c4` | [ovl_30_..._c_a.c](../src/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c_c_a.c) *(twin)* |
| 3 | `OvlFunc_898_2008ef4` | [ovl_314_..._a_c.c](../src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_a_c.c) *(2 parks closed, fakematch)* |
| 4 | `OvlFunc_901_2008a80` | [ovl_314_..._c_c.c](../src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_c_c.c) *(twin, park closed, fakematch)* |
| 5 | `OvlFunc_923_2009ec8` | [ovl_1a3c_a_c_a_b.c](../src/overlays/rom_7aa430/ovl_1a3c_a_c_a_b.c) |
| 6 | `OvlFunc_924_200d458` | [ovl_35b8_a_c_a_c.c](../src/overlays/rom_7ac2d8/ovl_35b8_a_c_a_c.c) *(twin)* |
| 7 | `Task_Transition300` | [rom_8d9a4_..._c_a.c](../src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_a_c_a.c) |
| 8 | `Func_808ef70` | [rom_8d9a4_c_a_c_c_c_c_a_c.c](../src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_a_c.c) |
| 9 | `Func_80908e0` | [rom_8d9a4_..._c_b.c](../src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c_a_c_b.c) |
| 10 | `OvlFunc_884_2008248` | [ovl_30_..._a_a.c](../src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a_a.c) *(`_MSG_1197`)* |
| 11 | `OvlFunc_909_20086e0` | [ovl_30_..._a_a_a.c](../src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_a_a_a.c) *(3 pins)* |
| 12 | `OvlFunc_922_20097e4` | [ovl_30_..._a_a.c](../src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_c_c_a_a.c) *(7 pins)* |

## A `.sym` line that REPLACED scaffolding

`_MSG_1197` is the one entry added, and it is the first added to remove a pin rather than to
improve a count. Two routes reached the same bytes -- `register int m __asm__("r5")`, or a
message base symbol -- and I measured all three states myself:

- **with the literal, the function comes out a DIFFERENT LENGTH.** Every relocation offset
  after the first `__MessageID` shifts by two. The literal is not "a few encodings off", it is
  structurally wrong.
- with the symbol: 140 encodings against 140, every relocation identical, one pool word left
  for the assignment to fill.

The function reaches `__MessageID` three times as a base plus arithmetic (`m`, `m + 3`,
`m + 4`), which is the shape `_MSG_810`--`813` and `_MSG_c20` already describe. That satisfies
the batch-272 rule -- an entry is worth adding when it COMPLETES a function -- where
`_MSG_2080`, `_SIZE_80f0024` and `_TBL_7a828` remain unadded because they do not.

**A length difference is a stronger tell than a register difference**, and worth looking for
before reading a count.

## Three fakematch rows, and the one that is heavy

`OvlFunc_922_20097e4` takes **seven pins** -- the heaviest scaffolding in this run of batches.
It is recorded rather than argued away; the alternative was to park it at 2 differing. Its
greedy-drop ladder shows nothing is removable (dropping the r5 zero pin alone costs 10), and no
flag reaches it.

`OvlFunc_909_20086e0` takes three, and its mechanism is the recorded
CSE-shared-expensive-constants class **in its beatable form**: `.00.rtl` has four independent
`(set (reg N) (const_int 47710208))`, `.03.cse` collapses three to one, and `.17.lreg` prices
that pseudo at 2·5/30 = 0.33 against the branch-proven zero's 2·6/95 = 0.126 -- so it wins r6
and adds a push. No CSE flag disables cse1 and named locals do not survive it; only a
call-clobbered pin does, and the **ascending assignment order** is the second half of the lever.

> Two measured negatives came with it: the batch-182 "both split builds named" lever
> **backfires** at 59 differing (four call sites is not six), and
> `-fno-rerun-cse-after-loop` on that function is an **internal compiler error**
> (`decode_rtx_const`, `varasm.c:3421`) -- which matters because that flag is in the
> `CSE_CFLAGS` group several files already use.

## Levers that each closed a function

- **A bitfield insertion is not an expression store.** The rule that a HImode store target
  truncates a mask in the RHS is true of an *expression*; `store_bit_field` builds the
  read-modify-write in **SImode**, so a contiguous bit range written as a BITFIELD keeps the
  ROM's 32-bit mask. **This refuted a park** that had concluded the mask was "NOT reachable by
  spelling" -- 16 differing to exact in one step. Two independent instances this batch, and a
  single `(v & 0xf) & ~0xc | 4` store turns out to be **two adjacent bitfield writes to one
  byte**.
- **The "argument precompute" blocker class is retired.** Both its parks called it a compiler
  difference. It is a sched2 tie: `.23.sched2` shows two insns at identical priority 72 with
  the tie falling to `INSN_LUID`, and `expand_call` puts the r0 fill last where the ROM has it
  in the middle. Found by sweeping the **solved** corpus for the three-line window
  `lsl / mov r0,#0 / lsl` -- 23 hits, all with a `.c` beside them.
- **A caller-save `str rN,[sp]` / `ldr rN,[sp]` pair in the ROM is a SPEC.** It says that
  allocno is last in priority order, which pins the relative order of all the others -- and the
  move it implies is to **lengthen** a live range, not shorten it. One statement move, 29
  instructions.
- **The two-sign LICM rule used in BOTH directions inside one function**: a `goto` loop to
  suppress the pass (it was hoisting five invariants where the ROM hoists two, and `.18.greg`
  gave all five hard registers -- the surplus push), then the two the ROM *does* want written
  by hand into the preheader. 27 -> 13 -> 4.
- **A named local wrapping ONE of two identical constants** splits a CSE that survives
  allocation; the other is inert.
- **Reusing a variable as its own accumulator** picks which side of a commutative `and` is
  tied to the output. But for a *sum* feeding a store a FRESH variable is the lever and reusing
  one inverts the operands -- two functions in one batch, opposite answers.
- **Two similar blocks can want DIFFERENT statement order** (both symmetric orderings 17 and
  22; the asymmetric one 13).

## `-fno-strict-aliasing` is the "ROM reloads a pointer field" lever

Probed directly rather than inferred: with strict aliasing gcc **always** caches
`x->ptrfield` -- across a store to a `char` field, an `int` field, a pointer field of the
pointee, and a pointer field of the same parent struct. Without it, it reloads.

**No source-level type change reaches this.** That makes it genuinely different from the
recorded DO NOT CACHE A REPEATED READ rule, which is about scalar reads a source can re-write.
`ALIAS_CFLAGS` already exists for it.

## The loop-weighting cuts both ways

`REG_N_REFS` is weighted `+= loop_depth + 1`. `OvlFunc_883_200dd68` is **one instruction** from
exact and cannot get there, because the two routes cost each other exactly: the pointer-walk
gives the ROM's address chain but adds one **in-loop** reference to a pooled zero, taking it
12/92 = 0.130 to **24/92 = 0.261** and overtaking the loop index at 0.190. The two swap r7/r8,
costing exactly the instruction the walk saved. Eleven walk spellings all held the zero at 8
refs.

**Price a spelling that adds a reference inside a loop before adopting it -- at depth it counts
treble.**

## The selector, refined

The neighbour criterion held for a third round -- but two agents reported that **stem adjacency
was the wrong measure of it**:

- one target's best neighbour was *the function it calls on its first line*;
- another's 12-component stem neighbour was twelve lines of unrelated arithmetic, while the
  real source was found by **grepping for two callee names** and sat in a different directory;
- a third pair was solved from a park in an unrelated directory that was the *same function
  body*.

**Check the call family, not the stem.** Cheaper to compute, and it predicted the outcomes stem
distance got wrong. The clearest single data point: the one target of four that stalled in the
`rom_8a000` round was the **smallest** at 103 instructions and the only one under the old
cut-off -- and the only one whose stem-mates were 20-byte stubs.

## A correction I owe the log

I replaced `src/non_matching/ovl_7aa430/2009bc8.c` wholesale instead of appending, dropping its
existing header -- the exact failure I criticised in the x15 park one batch ago. Restored.

What was briefly lost: two levers still required by the current candidate, and **a third group
member**, `OvlFunc_907_2008f3c`, which reaches the same 7 differing positions with the same
spelling.

**That third member also means `dupfuncs.py` UNDERCOUNTS.** It groups on exact identity, so a
near-twin differing by one statement is invisible to it -- this one was known only because a
batch-99 park recorded it by hand. **The "26 functions would come free" figure is a floor, not
a total.**

## State

**4,461 from C / 1,249 in asm** against batch 277's 4,449 / 1,261 -- exactly **+12/-12**, the
**thirteenth consecutive batch** where the delta reconciles. `census.py` TOTAL 1,249, agreeing
with funcindex exactly (the cross-check added last batch). 485 park files (481 + 8 new - 4
retired). `fakematch.txt` 463 rows, **+3**.

`dupfuncs.py` now reports 12 groups covering 38 functions with 26 free, down from 16/46/30 --
and that is a floor, per the correction above. The x15 group is the largest single prize left
in the corpus and now sits at 25 differing at full length.

## Open, for the user

Unchanged from batch 277, and still grouped as one decision -- each has verified evidence, none
COMPLETES its function:

- **`_MSG_2080 = 0x2080`** -- strong structural argument (shiftable, so gcc builds it and the
  ROM pools it); worth 73 -> 67 on a function that stalls at 23.
- **`_SIZE_80f0024 = 0x230`** -- arithmetic verified, same class as the `_SIZE_8015430` added
  in batch 276. The strongest of the three.
- **`_TBL_7a828`** in `label.sym` -- `.L7a828` verified `.global`.

New this batch, and both cheap if you want them:

- **`OvlFunc_970_2008f80` wants a `-fcall-saved-r4` Makefile row** (153 -> 145). The precedent
  is `COMMON2_CFLAGS`, which makes exactly that substitution, and the one-grep test is
  `push {r4`. Not added because the function is not otherwise exact.
- **`OvlFunc_882_200c41c` wants `ALIAS_CFLAGS`** for the pointer-reload above. Same reason.

Also still open: the fakematch convention question (52 bare-pin files unregistered), and
`OvlFunc_948_2009308`'s one pin from batch 272.
