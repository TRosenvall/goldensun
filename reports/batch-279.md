# Batch 279 -- eighteen functions, the sched2 tie-break completed, and the x15 closed

Gated on a clean `make clean && make -j8 && make compare`, green at the target SHA1
`5c4695205413df7db52b9a184815a07783999971`. Sixteen of the eighteen verified at their exact ROM
addresses in the freshly linked ELF; the other two are named functions whose presence was checked
the same way. Three control symbols absent. `git status` clean.

| | |
|---|---|
| elevated | **18** |
| parked | 1 new, 1 closed as unreachable |
| `.sym` entries added | **3** (`_AREA_15`, `_MSG_cbe`, `_MSG_c76`) |
| splits | 8 |
| **fakematch debt added** | 6 |

**Six subagents, 18 targets.** Eighteen functions from sixteen solved targets.

| agent | assignment | landed |
|---|---|---|
| C | overlays 944 + 899 | **4** of 4 |
| E | `rom_b0000` / `rom_b5000` | **4** of 4 |
| F | named functions + the capital-F one | **4** of 4 |
| B | overlay 943 | **3** of 3 |
| A | overlay 927 (a x3 group) | **3** of 4 targets |
| D | the x15 group | 0 -- closed as unreachable |

## The selector, third refinement: score by CALL FAMILY

Batch 278's agents reported that stem adjacency was the wrong measure of the neighbour criterion, so
this round the targets were scored by **how much of each function's call family is already written in
elevated C** -- parse the `bl` targets out of the `.s`, check each against the solved corpus.

It held. The top-scoring targets landed: 25-of-27, 24-of-24, 23-of-23 and two 19-of-19 all converged,
and five of six agents reported the decisive donor was found by **callee**, not stem. Two better
search moves came out of it:

- **Grep generated `asm/` for a ROM instruction pattern, then read back to its `.c`.** One agent
  found its donor with `mov rN, #227`.
- **Diff a solved function's `-da` DUMP against yours.** That explained a schedule difference
  directly, where diffing its source would not have -- the donor had two trailing calls, so both
  scheduling candidates reached the same dependent count and LUID decided there.

## The sched2 tie-break, completed

The notes went straight from priority to `INSN_LUID`. **There is a term between them**, and three
functions hit it. `rank_for_schedule` compares priority, then **forward dependent count** (more
wins), then LUID.

Two corroborations worth copying as technique: **adding a trailing call raises BOTH counts by one and
is therefore inert**, and **gcc's Thumb pool is emitted in scheduled-insn order**, so a pool that
looks permuted is the schedule.

### Why an r0 argument fill can never win on dependent count

**A call SETS r0 and only USES r1/r2.** So r0-fills accumulate no anti-dependences while r1/r2 fills
accumulate one per later call -- one dump showed **10 against 3**. Three ways to move such a tie, in
ascending cost:

- **assignment order**, when the fills are in one region -- `q0 = N;` written textually first was
  worth **80 differing → 0** on one function with no other edit;
- **a memory dependence**, when there is a memory operand (below);
- **a basic-block boundary** (`do { } while (0);`), needed where an r0 fill cannot win at all.

## The alias set must change AT THE ACCESS, not globally

Batch 278 introduced `-fno-strict-aliasing` as the "ROM reloads a pointer field" lever. The flag and
a cast are **not interchangeable**.

On `OvlFunc_927_2008ae8` the last two encodings were a tie LUID structurally could not win --
`store_bit_field` always masks and shifts the value before reading the destination, so the value
load's LUID is always lower. Reading the byte through a **char pointer** puts it in alias set 0,
creates the dependence, and reaches **0**. `-fno-strict-aliasing` leaves **2** *and* breaks an
unrelated store pair.

Related, and it widens a recorded rule: a `union` on a table cures a missing **true** dependence, not
only an anti one. The rule is about alias **sets**.

## Check the block structure before moving live ranges

`OvlFunc_927_2008ae8` presented as **five** allocation defects at 56 differing -- wrong frame size,
two values swapped between high registers, three long-lived values in the wrong slots. The priority
formula had been computed and predicted greg's order exactly, and moving live ranges closed nothing.

**Duplicating a call into both arms of an `if`/`else`, rather than selecting its argument with a
ternary, dropped it to 8 in one step**, because cross-jumping then merges only the tail and stops
where the ROM's duplicated `mov r2, r6` sits. All five were one cross-jump defect upstream of
allocation. **The formula was right and the axis was wrong.**

## Three `.sym` entries, each completing a function, one on a new kind of evidence

- **`_AREA_15 = 0x15`** -- `area.sym`'s own stated criterion, literally: 0x15 fits an eight-bit `mov`
  so gcc builds it, and the ROM pools it. The `_AREA_05` shape exactly. Seven literal spellings
  measured, all failing.
- **`_MSG_cbe = 0x0cbe`** -- the same shape and the same module as `_MSG_d1c`, whose entry already
  records it: a message base held in a callee-saved register and reached with `add r0, #K`. Here the
  **pass trace** is on file, which is stronger than this class usually gets: `.03.cse` already emits
  the ROM's shape and `.07.gcse` undoes it with three `CONST-PROP` lines, because `cprop_insn` skips
  only uses in the def's own basic block.
- **`_MSG_c76 = 0xc76`** -- **a DUPLICATE POOL WORD is a symbol tell.** `force_const_mem`
  deduplicates SImode `const_int` per function, so two words holding one value mean two rtx objects:
  one `const_int`, one `SYMBOL_REF`. `DataTransferMenu`'s pool is seven words holding six distinct
  values, with `0xc76` twice -- decoded out of `baserom.gba` at landing rather than taken on report.
  **That reaches the unshiftable case the shiftability argument cannot**, and `tools/pool.py` would
  never flag it.

> And the anti-tell, from the same batch: **a pooled SHIFTABLE constant can be a HImode tell rather
> than a symbol tell.** `& (short)0x800` makes the AND HImode so the mask pools, printing as the
> ROM's `ldr rN, =0x800`. Since shiftability is the strongest symbol argument, check the MODE first.

## The x15 group is closed as unreachable

Three rounds took `OvlFunc_883_20088c0` from 101 differing to 42 to 25 at full length. This round
takes it no further, and the result is a decisive negative.

**The entry point the batch-278 park named does not exist, and I wrote that park.** It said the
residue was `local_alloc`'s choice for the loop's `ldrsh` zero. That zero is a `(clobber
(scratch:SI))` -- a bare scratch with no register number, so no allocno, and `local_alloc` never sees
it. It is filled by **reload**, which runs *after* `global_alloc`; `.18.greg`'s trailing log prints it
from strings that exist only in `reload1.c`. **The zero is downstream of the index's register, not
upstream** -- I had the causality backwards and a full round went at a consequence.

The checkable lesson: when a residue's register belongs to a `scratch`, look for an actual
`Register N used R times` line in `.17.lreg` before treating it as source-addressable.

**And a sixth thing that decides an allocation:** `find_reg` runs **two passes**. Pass 0 considers
only registers already handed out and skips any register a later *conflicting* allocno prefers; pass
1 drops both and walks `REG_ALLOC_ORDER`. Priority decides who reaches a register first; **pass 0
decides whether a fresh one is taken at all.** Read `;; N preferences:` beside `;; N conflicts:`.

The negative is properly built: the mechanism was **validated by construction** first -- forcing the
base's priority above the index's does put it in r1, does move the index off, and does stop the pool
load hoisting, giving the ROM's exact line order -- and only then priced. Three simultaneous
live-length requirements of 47, 47 and 101 against current 32, 28 and 40, where the index dies at
instruction 40 of 142 and the object is already positionally aligned. Plus a second independent
blocker: the walker and base share a cse equivalence class, so an added base reference is attributed
to the walker (measured: walker 7 → 9 refs, base unchanged at 3).

**Those fifteen copies are not available this way.** It needs a differently configured compiler, and
that claim is now backed by `find_reg`'s own arithmetic rather than asserted.

## Scaffolding, checked against convention rather than instinct

Thirteen pin sites on one function felt like too much, so I measured the convention instead of
guessing: **181 landed files already use the `PIN2`/`PIN3`/`PIN4` macros**, the great majority booked
in `fakematch.txt`, and the `do { } while (0);` barrier also has precedent in landed files. This is
the tree's established form at scale.

It also quantifies the standing convention question: **about twenty of those 181 are not booked.**

> And a process lesson: on one function, getting a *mask* right -- `neg r3, r3` is −13, and
> −13 == `~12`, not `~13` -- made **four separate pieces of scaffolding inert**: two pin pairs, a
> barrier and two register pins. **Re-run the greedy drop after every structural change**, not just
> at the end.

## Other levers that each closed a function

- **An inner scope is the only handle on spill-slot order.** `expand_decl` numbers function-scope
  locals before any statement, so one always gets a pseudo number below a compiler-generated temp's;
  a nested block pushes it past. That turns a recorded non-lever into a lever.
- **`goto` into a `do/while` is a SCHEDULING lever**, not only a loop shape: the loop's
  `NOTE_INSN_LOOP_BEG` rides on the `b` into the test and acts as a barrier, reordering the code
  *before* it. It flipped two argument fills at exactly the call sites followed by such a loop, while
  three sites without one were right from the start.
- **The ldrh/ldrsh blocked sub-class is narrower than recorded** -- blocked when the SAME variable is
  read twice, not when ONE SIGNED READ is MASKED THREE WAYS, which reproduces the ROM's `ldrsh` *and*
  `ldrh` (one signed plus one unsigned variable CSEs to a single load, 54 differing).
- **A per-case block-local beats both an inline expression (94 differing) and one function-scope
  local (88, and sixteen instructions short)** -- the function-scope one spends a callee-saved
  register in every arm. And gcc's cross-jumping merges two cases into a third's tail, which is why a
  ROM jump-table entry can point *inside* another case's block.
- **The struct-field spelling, not an area symbol, is what drags a pool up.** Writing a ROM's pooled
  zero as a symbol reproduces the assembly TEXT and loses the pool position, because an SImode
  symbol's 1020-byte `pool_range` moves everything past the epilogue. `store_bit_field`'s HImode
  insert mask at `pool_range` 64 is the real producer -- confirming both halves of the `80b0a20`
  park at once.
- **A four-point measurement of the assignment-position axis**: the same masked value named at four
  points measures 18, 17, 6 and 0, with block 0 the *worst*.

## The one target parked

`OvlFunc_927_200a2c0`, 61 differing of 193 -- and it is a clean specimen rather than a stall. Its
whole back half was exact on the FIRST screen, and **every differing encoding is a repeated-constant
build**. That is new information about the straight-line repeated-constant class: it is confined to
the constant builds, and nothing else in a 193-instruction cutscene resists.

It also states the three-named-locals precondition's failure mode as cleanly as it has been stated:
**the whole pre-loop region is ONE basic block across about twenty `bl`s**, because calls do not end
a basic block. Not a weak lever -- no branch to dominate from. Fifteen launder rows still leave four
real instruction placements wrong, which is why it is parked.

## Two corrections I owe the log

- **I overwrote a park's header last batch**, dropping two still-required levers and a third
  duplicate-group member. Restored, and it produced a finding: **`dupfuncs.py` UNDERCOUNTS**, because
  it groups on exact identity and a near-twin differing by one statement is invisible to it. Its
  "would come free" figure is a **floor**.
- **Two commits in this batch were incomplete.** The splits rewrote `stage1.ld` and one
  `overlay.ld`, and I staged only `src/` and `asm/`. Every gate was green because the build reads the
  **working tree**, so nothing failed -- but a fresh clone of either commit would not have built.
  Fixed in a follow-up. **A green `make compare` does not prove a commit is complete; `git status`
  does, and it belongs beside the address check at the end of a batch.**

## State

**4,479 from C / 1,231 in asm** against batch 278's 4,461 / 1,249 -- exactly **+18/-18**, the
**fourteenth consecutive batch** where the delta reconciles. `census.py` TOTAL 1,231, agreeing with
funcindex exactly. 486 park files. `fakematch.txt` 469 rows, +6.

`dupfuncs.py` now reports 9 groups covering 32 functions with 23 free -- **and that is a floor**. With
the x15 closed, the largest *reachable* group is the x3 this batch landed, so duplicate work is
mostly spent; the call-family score is now the primary selector.

## Open, for the user

The three long-standing entries, still grouped as one decision -- each has verified evidence, none
COMPLETES its function, which is why the three added this batch went in and these have not:

- **`_MSG_2080 = 0x2080`** -- strong structural argument; worth 73 → 67 on a function that stalls at 23.
- **`_SIZE_80f0024 = 0x230`** -- arithmetic verified, same class as `_SIZE_8015430`. Strongest of the three.
- **`_TBL_7a828`** in `label.sym` -- `.L7a828` verified `.global`.

Also open: **the fakematch convention question, now quantified** -- about 20 of the 181 `PIN`-using
files are unbooked. And from batch 278: `OvlFunc_970_2008f80` wants a `-fcall-saved-r4` row,
`OvlFunc_882_200c41c` wants `ALIAS_CFLAGS`, and `OvlFunc_948_2009308` has one pin from batch 272.
