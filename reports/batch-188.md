# Batch 188

Fourteen elevated, one parked. The functions are the smaller part of this
round. **The larger part is that three separate selection tools were found to be
wrong**, each in a way that had been silently shrinking the candidate pool for
many batches, and one of those errors was caught by a reader's disbelief rather
than by any check this project runs.

Two blocker classes moved from "swept and unexplained" to "read out of the
compiler and closed".

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `Func_80b6ae0` | `0x080b6ae0` | [rom_b5a0c_…_c_b.c](src/rom_b5000/rom_b5a0c_c_c_a_a_a_c_b.c) | the **offset** was the winning grep; three levers lost |
| 2 | `Func_80a8508` | `0x080a8508` | [rom_a7380_c_b.c](src/rom_a1000/rom_a7380_c_b.c) | a constant base plus an index needs the **constant first** |
| 3 | `OvlFunc_969_200b660` | `0x0200b660` | [ovl_314_…_a_a_b.c](src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_a_a_b.c) | **multiply operand order survives to the `mul`** |
| 4 | `OvlFunc_924_200bbd4` | `0x0200bbd4` | [ovl_35b8_a_a_c_a_c_b.c](src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_b.c) | a constant bias has two placements |
| 5 | `DrawSmallText` | `0x0801e74c` | [rom_1de5c_c_c_a_a_a.c](src/rom_15000/rom_1de5c_c_c_a_a_a.c) | the neighbour score cannot see a **body-prefix twin** |
| 6 | `CheckPartyItem` | `0x08078698` | [rom_78414_…_a_a_b.c](src/rom_77000/rom_78414_c_c_a_c_a_a_b.c) | read the add's **operand count** off the ROM |
| 7 | `MapActor_SetPos` | `0x080923e4` | [rom_91584_…_a_a_c.c](src/rom_8a000/rom_91584_c_c_a_c_c_c_c_c_a_a_c.c) | the whole TU converts at once |
| 8 | `MapActor_SetPos3D` | `0x08092454` | same file | — |
| 9 | `Func_800bf34` | `0x0800bf34` | [rom_be70_c_b.c](src/rom_9000/rom_be70_c_b.c) | **a sched2 tie between adjacent insns is source order** |
| 10 | `OvlFunc_945_2009144` | `0x02009144` | [ovl_30_…_a_b.c](src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_c_a_b.c) | **the LICM lever has two halves**; a named local with no call in sight |
| 11 | `Func_80f377c` | `0x080f377c` | [rom_f2028_c_c_a_a_b.c](src/rom_f2000/rom_f2028_c_c_a_a_b.c) | r4 used but not pushed is `-fcall-used-r4` working |
| 12 | `OvlFunc_938_2009450` | `0x02009450` | [ovl_30_…_c_b.c](src/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_b.c) | **fakematch** — barrier sites 2..n, never site 1 |
| 13 | `OvlFunc_927_200a004` | `0x0200a004` | [ovl_30_…_c_c_a.c](src/overlays/rom_7b4558/ovl_30_c_c_c_a_a_c_c_a.c) | **fakematch** — anchor every argument of the call |
| 14 | `Func_8015f30` | `0x08015f30` | [rom_15e8c_a_c_a_a_a.c](src/rom_15000/rom_15e8c_a_c_a_a_a.c) | blocker 1b's mechanism, read out of `arm.md` |

Parked: [`OvlFunc_953_200a5f0`](src/non_matching/overlays/200a5f0.c) — 29 of 43,
constant rematerialisation with no dominating branch.

Every `.c` produced a `.s` carrying the `.gcc2_compiled.` marker.

**CORRECTION.** This report originally claimed every address had been checked
against the linked ELF. It had not. Four of the fourteen were wrong --
`DrawSmallText`, `CheckPartyItem`, `MapActor_SetPos` and `MapActor_SetPos3D` --
and they are exactly the four functions with real names rather than
`Func_<addr>` ones. The ten address-named entries were right because their
addresses are encoded in their names and cannot be got wrong; for the four
named ones the address was inferred from the file stem or a neighbour. So the
verification claim held precisely where it was redundant and failed precisely
where it was load-bearing. `0x08091f14` in particular is `Func_8091f14`, which
is still unelevated -- the collision is what surfaced this.

The addresses above are now the ELF's. `tools/checkaddr.py` exists so this is
mechanical rather than a promise.

## THREE SELECTION TOOLS WERE WRONG, AND THE POOL WAS THE VICTIM

None of these were decompilation errors. All three were errors in deciding
*what to attempt*, which is worse, because a function that is never attempted
never reports back.

### 1. The filter was calibrated on functions unlike the ones we match

`filtered.py` required 40–120 instructions and fewer than 8 calls. Measured
against the 3,474 functions already matched: **85% have fewer than 8 calls**,
**77% fall outside the 40–120 band**, and the median matched size is **21** —
against a filter floor of 40. The filter was excluding the population it was
supposed to select from. Rebuilt with corpus calibration, the candidate list
went from **4 to 768**.

### 2. `bl .L` is a LONG BRANCH, not a call — and a reader caught it

The hand-written-assembly test had three tells, and one was `bl .L<label>`.
**Thumb encodes a long unconditional branch as a BL pair, and the disassembler
renders that pair as `bl`.** It is a jump. It appears in ordinary compiler
output the moment a function outgrows the short-branch range:

    cmp r0, #0x1a
    beq .Le4f7a      <- skip over
    bl  .Le65f8      <- long branch, ~2700 lines forward
    .Le4f7a:

**Hand-written assembly was 126 functions across 23 files. It is actually 76
across 5.** Fifty functions, in eighteen whole files, had been wrongly excluded.

The methodology error matters more than the count. That tell *was* verified —
against 3,474 compiler-output files, none of which contain it. But gcc writes
`b` and lets the assembler choose the encoding, so compiler output is the wrong
corpus for a test that classifies *disassembly*. **Ground truth checked in the
wrong alphabet looks like strong evidence and is worth nothing.**

It was not caught by any check here. It was caught by the observation that *27
hand-written 800-instruction functions is not a realistic thing for a game to
contain* — and the size skew was the symptom, because only large functions need
long branches. A plausibility objection from outside beat the internal
verification.

### 3. The block-duplicate test cannot prove unreachability

An estimate that **406 functions (39%) could be parked statically** was
published, then revised to 156, then 17, then — after honest tightening —
**zero**. Not one survived. Three false-positive classes:

- **a call between the two sites** (250) — calls clobber argument registers, so
  a constant used as an argument is rebuilt at each site regardless;
- **the same value needed in several registers at once** (139) — simultaneously
  live, so cse *cannot* merge them; this is the most ordinary code there is;
- **an indirect call** a `bl`-only test walks straight past — gcc-2.96 calls
  through a register with `mov r12, pc / bx rN`.

No park files were written on the strength of it. **Unreachability is
established per function, by measurement.** The corollary for planning is
unwelcome but firm: there is no cheap way to predict which functions will need
parking, because parking is the *outcome* of an attempt.

## TWO BLOCKER CLASSES CLOSED BY READING THE COMPILER

### Constant rematerialisation needs a DOMINATING BRANCH

The shape: the ROM rebuilds the same two-instruction constant
(`mov rN, #C / lsl rN, #n`) at several call sites; gcc builds it once, parks it
in a callee-saved register and copies. Sometimes reachable, sometimes not — and
now decidable **by inspection, before any spelling is tried**.

- **cse1 commons the repeat unconditionally.** So separate named locals do *not*
  defeat CSE. That folklore is false and measurably backwards: on the parked
  function the named-local spellings scored **40 and 41 against 29** for plain
  literals.
- **What restores the constants is gcse's constant propagation, and cprop is
  strictly cross-block.** `cprop_insn` skips any register already set in the
  current block — gcc's own comment: *"If the register has already been set in
  this block, there's nothing we can do."* — and `find_avail_set` only accepts a
  set available at the *start* of the block.

So the whole question is: **does a branch dominate the repeated uses?** The
parked function's solved sibling has its assignments above a leading `if` and
its uses below, so cprop restores all six and twelve pseudos vanish. The parked
one's only branch sits *after* every use, so everything lives in block 0 and no
spelling can reach it.

**And no flag can reach it either — read, not swept.** The responsible pass is
the *first* `cse_main`, which `toplev.c:2917` runs inside the plain
`optimize > 0` block with **no `-f` flag gating it at all**;
`flag_rerun_cse_after_loop` guards only the second call. Eleven flags have now
been swept across two functions with no effect, but the sweeps were never the
argument.

### Blocker 1b: HImode literals pool because of ALTERNATIVE ORDERING

`*thumb_movhi_insn` (`arm.md:4318`) lists alternative 1, `"l" <- "mn"`, **before**
alternative 5, `"l" <- "I"`. The `n` constraint accepts any `CONST_INT`, so
reload takes alternative 1 at zero cost and the constant pools — **even for
values `I` would happily encode**. `CONST_OK_FOR_THUMB_LETTER` is `0..255`;
`0x63` qualifies, and still pools.

This means the rule has **no exceptions to hunt for**: no small-value escape
hatch, no threshold. The `int` intermediate is the only lever, and it often
needs a *second* name — the destination pointer, written first — because the
extra pseudo otherwise makes sched2 hoist the address pool load and costs back
what the `mov` bought (7–10 differing across five placements).

A consequence worth its own note: **a pooled halfword constant fakes the
jump-over-pool screen false negative.** `pool_range` is only 64, so an
`ldrh rN, .LCn` drags the minipool up before the epilogue and gcc emits a real
`b .L` over it. Before filing a trailing `b Lx / Lx:` as a screen artefact,
grep the generated `.s` for `ldrh rN, .L`.

## Smaller levers, all measured

- **The LICM lever has two halves.** Hoists land at the *end* of the preheader.
  Promoting to a local is necessary but not sufficient — misplaced, it scores
  *exactly* what the inline spelling scored, which reads as "the lever did
  nothing" rather than "the lever is half-applied". Promote **and** sweep the
  position. Clean three-point curve: inline 2, local-after 2, local-before **0**.
- **A named local shows up with no call in sight.** Beyond the recorded
  across-a-call rule, there is a third face: **eager versus lazy issue across a
  short-circuit chain**. A load the ROM issues *before* its own `&&` guard has
  been evaluated is a named local — the ROM did work the short circuit would
  have skipped. Worth 22 positions.
- **r4 used but not pushed is `-fcall-used-r4` working**, not a missing save —
  the converse of the recorded rule. Check whether r4 is dead before the next
  `bl` before filing it as a flag-group problem.
- **A sched2 tie between two adjacent independent insns is source order.**
- **A third DMA-helper signature**: the address in a *pseudo*
  (`mov r5, sp / str r3, [r5] / mov r0, r5`) is one `volatile` slot object
  shared by two `DMA3_SET` calls. The `volatile` goes on the **object**, not the
  pointer, and the address-of has a placement.
- **Fakematch discipline**, two rules: **anchor every argument of any call you
  anchor any argument of** — a partially laundered list is strictly worse than
  none, because anchoring some arguments perturbs an interleave that was already
  correct. And **launder the first occurrence, never the second** — CSE
  substitutes into the launder's own initialiser before the asm sees the value.

## Tooling

- **`tools/neighbour.py`** (new) — ranks solved files by shared **callees and
  globals**, ignoring filenames. Callee-set identity has now beaten filename
  adjacency seven rounds running. Two tie-break rules were learned in use: an
  N-way tie on a single shared *global* is broken by grepping that global's
  name, because a global that is a base pointer for a shared block collects a
  family of functions that all walk it the same way; and a tie among siblings
  from the *same directory* is real and harmless.
- **`tools/filtered.py`** — corpus-recalibrated, `--wide` mode, indirect-call
  detection, and the `bl .L` fix.
- **`README.md` / `docs/building-on-macos.md`** — corrected. They documented
  installing agbcc as though it built the project. It does not: **gcc-2.96
  compiles all decompiled C (935 rules); agbcc compiles only two SDK library
  groups (6 rules)**. The documented docker commands also omitted
  `AGBCC_DIR=/opt/agbcc` and therefore failed as written.

## Two corrections to this notebook

- The note that the constant hoist "happens at expand" is true of the
  **pool-load** form only. For the `mov`+`lsl` form, expand emits four
  *independent* sets and the hoist is cse1's.
- The recorded remedy *"assign at the top so it is live across the calls"* does
  not generalise. The two cases are distinguishable from the ROM alone: a value
  live across a call wants assignment at the top; a value materialised into a
  just-freed register beside its own store wants pointer and value named
  adjacently.

## What this batch does NOT establish

The two fakematches are **matches, not explanations**. The original toolchain
plainly did not use inline asm; the idiom stands in for whatever it did
differently, exactly as in the register-allocation-order class. Six launders on
a 43-instruction function looked heavy enough to check before landing — it is
within house norms (47 of 97 fakematch files carry five or more, and the
precedent in that same overlay carries seven) — but "within norms" is a
statement about this tree's conventions, not about the original source.
