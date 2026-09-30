# Batch 307 — twelve parks, two briefing errors of mine, and the 800+ band opened

Seven agents, 28 targets — intended to close the 401–800 band and open the 800+ band. Gate green at
`5c4695205413df7db52b9a184815a07783999971` at every commit.

**0 landed, 12 parked, 8 recons.** State: **4,785 of 5,710 (83.8%)** — 76 available, 758 parked.

Reconciliation, exact: available 88 → 76 (**12 left the pool**), parked 746 → 758 (**+12**), and
0 + 12 = 12.

**The band did not close: 13 remain in 401–800.** Two causes, both worth stating. Agents were told
to prefer depth over breadth and did, leaving 8 targets at recon rather than shallow parks — which
was the right trade. And **four targets were dropped on a screen of mine that was wrong** (below).

## The closest results

| function | figure | exactness | aligned |
|---|---|---|---|
| **`Anim_Unused_Fizz`** | **118 of 770** | **size AND count both exact** | **91.3%** |
| `Anim_Ragnarok` | 296 of 584 | **size exact, all 49 relocations exact, frame exact** | 83.0% |
| `BuildDraw2DFuncEx` | 718 of 781 | all 27 relocations exact | 89.6% |
| `BattleMain` | 618 of 684 | **size exact** | 67.8% |
| `Anim_Nereid` | 619 of 707 | all 63 relocations exact on candidate 1, frame exact | 56.6% |

All pin-free except `BuildDraw2DFuncEx` (4 pins, removable — see below) and `ActorCmd_Player`.

## The strongest lever: a global-derived pointer used across a call

**Worth 630 → 139 of 770 on `Anim_Unused_Fizz`, and it made size and count both exact.**

The tell is a proof rather than a heuristic. The ROM reuses `view` and `view + 0xc` *across an
intervening call*. **cse can never do that for a global's load — a call clobbers memory — so only a
pseudo can hold it.** A global-derived pointer that survives a call in the reference *was a source
local*, necessarily.

**And it must be declared inside each loop body, one per loop.** Loop 1 keeps it in r5, loop 2
spills it at sp+0x28, and one pseudo cannot be both — so a single function-level local cannot
reproduce the frame. Four earlier candidates had been stuck 8 bytes short with every spill offset
off by 8; this supplied the missing slot and `view + 0xc` supplied the second.

It also **bounds the named-offset lever**: naming the blit's *source pointer* is worth 7, while
naming the *offset* instead is worse. And `p->fc += dx >> 8` field-first beats `(dx >> 8) + p->fc`
by 9 — those are different instructions, not a formatting choice.

## My two briefing errors

**1. I screened 28 targets against a retracted rule and told a brief to abandon three viable
functions.** I read `elevation.md`'s "`.call_via` IS AN INLINE VENEER gcc NEVER EMITS — A STRUCTURAL
BLOCKER CLASS" and acted on it. That section is **retracted in full further down the same
document**, which records that 51 functions had already been written off on it, that the machine
description bounds the *code generator* and not the source language, and that `tryc.py` had been
silently dropping the `.call_via` line so every user screened two instructions short per call site.
`Func_8097a10` was later elevated **byte-exact** through the helper.

The agent disproved me three ways: it found the retraction, compiled its own candidate to
`mov r12, pc / bx r4` twice aligned-equal with the ROM, and observed that `ActorCmd_Player_World` —
which I had parked at 86.5% an hour earlier — carries the same two sites. It kept its target and
parked it at 795 of 833.

**The retraction had never struck the original heading.** That is the third recorded instance of the
same failure, and it cost work twice in one day: batch 302's agent also declined a figure for
`Func_808bec0` on that section's authority. Struck in place now. **`UpdateActors` and
`Func_8090a5c` are viable targets and should be reassigned.**

**2. I told two briefs a shared-file split constraint was symmetric. It is not.**
`split_s.py --dry-run` cuts `rom_e3958_c_c_c_c_a.s` three ways: cutting for `BaseAnim_Attack` leaves
`Anim_CriticalHit` **alone** needing no split, while cutting for CriticalHit first buries Attack.
Both agents measured it and corrected me independently. **Dry-run both orders before sequencing.**

## A contradiction I helped create

I relayed brief B's `int`-carrier finding to brief A mid-run. Brief A measured it **3.6 aligned
points negative** — for the same constant `0x1010`, which is **unshiftable on both sides**, so the
proposed shiftability precondition does not separate the cases. Thumb-1 has no PC-relative `LDRH`,
so the printed mnemonic cannot settle it either. Recorded as genuinely unresolved, with instructions
not to apply it on either function's authority.

**The process lesson is mine:** relaying a fresh single-function result between concurrent briefs
propagates it faster than it can be validated. Mechanisms and split shapes are facts worth relaying;
a one-function lever should travel with *"measure it, it may not hold."*

## The 800+ band, first systematic work

Full write-up in [docs/band-800plus.md](../docs/band-800plus.md). **Branch density, not instruction
count, is the axis.** Three of four targets were 800+ instructions in **one to six basic blocks**;
`Anim_Ramses` has 54 branches and behaves ordinarily. They need different levers and should not
share a brief — **triage the remaining 63 by branch count.**

For the straight-line population **one mechanism dominates: cse pass 1 commons repeated constants
across calls.** A call does not invalidate cse's constant table, so a 150–800-instruction block
gives it seven repetitions where 200-instruction code gives two; the overflow lands in r8–r11 and
costs the six-instruction high-save prologue. Twelve flag settings measured, none reaches it.

**Three of our best levers measure negative there, and the reasons transfer:** reuse-to-inherit is
*worse* because its first precondition (count already exact) fails; one-variable-per-region is
**byte-identical**, because region-scoping cannot reach a pseudo the compiler invented; declaration
order is inert for the same reason. **Both halves of the allocation pair are useless when the
competitor is not a source variable.** What moved a candidate was changing the *set* of long-lived
quantities.

**Ranking refinement: rank size-and-count first even while both are inexact.** On one candidate the
+4/+1 version is 2.3 aligned points *worse* than the +20/+10 one.

## A retraction the agent made itself

`-ffixed-r8..r11` takes one candidate from +44/+19 to **+4/−2**, which looks like evidence that the
original toolchain excluded the high register bank — the `REG_ALLOC_ORDER` question in a new form.
**It is false.** The sibling function's own reference *has* the high-save prologue and 45
high-register mentions, and I verified independently that **414 of 600 sampled asm files using high
registers already have landed `.c` siblings**. `-ffixed` was masking the commoning excess cse1
creates. No Makefile row, and **these figures must not be cited as `REG_ALLOC_ORDER` evidence.**

The guard that caught it was **measuring a second function before writing the first up as a band
finding.**

## One allocation class, two victims

`Anim_Frost` and `Anim_DragonCloud` share a residue: **the ROM spills a whole-function-lived
variable** where we keep it in r11. That one allocno explains the frame word, the entire count
deficit, and every hunk — the register *roles* are already right, only the names rotate.

Two facts make it a class: the ROM's choice is measurably **worse** than gcc's, so its allocator was
*forced*; and **the two instances have different victims**, so the spilled variable is whoever is
left over after the per-loop allocnos claim the seven call-saved registers. **No per-variable
spelling will reach it** — closing it means changing how many long-lived quantities exist, which is
exactly the lever that worked in the 800+ band. Fifteen flags measured, none moves it.

## The switch lever that transfers to 36 functions

**Every switch has a `case 0` sharing the `default` arm, and that arm comes first.**
`balance_case_nodes` splits three nodes at the middle; **four** nodes take the bisect branch, the
root becomes node 1, and node 0 being an unsigned minimum collapses its test into the ROM's `bcc`.
**So a `bcc` entry test on an unsigned selector says there is a fourth, lowest case you have not
written.** Read the mnemonic first — `bgt` means signed and may be an ordinary three-node split.

Corollary: **cases 1 and 2 need separate arms even with identical bodies**; merging them makes one
range node and emits `bls`, and `jump.c` cross-jumps the bodies back for free.

**45 occurrences already exist in generated output and 46 across 33 hand-written files covering 36
distinct functions**, many already parked. Re-screening those is likely worth more than any single
new reconstruction.

## Traps and refinements

- **A wrong program that is right on every other axis.** Four call offsets derive from `x = timer<<2`
  as `x-0x14…x-0x11`; the first reading compiled to `subs r3,#37` where the ROM has `#17` — one
  hunk, a different program, plausible on size, count, relocation sequence and aligned figure alike.
  **Found by reading the immediates inside the differing hunks.**
- **A grep filter that hides a diagnostic manufactures a false claim.** Filtering objcmp through
  `grep -E 'SIZE|ENCODINGS|first at'` drops `RELOCATIONS differ`, which produced a published "all
  138 relocations match" where the truth was 133 of 138 symbols and 134 of 138 offsets differing.
  The agent caught its own error. **The absence of a line in a filtered transcript is an absence of
  the filter.**
- **`ldmia rX!` plus a countdown** is the walking-pointer tell; with an ascending compare against a
  bound it is a strength-reduced subscript instead. Worth 18 encodings, measured both ways.
- **Copy coalescing is the two-variables lever run backwards** — the ROM emits a copy twice where gcc
  coalesces `j = i` because `i` is dead. The probe is keeping the donor *live past the copy*.
- **A compiler temp dates the declaration order**: it cannot outrank a function-level declaration, so
  when its slot falls between two declared locals', everything below must be block-scoped. Exact
  frames on two functions.
- **Two quantities in one loop can need opposite spellings** — three as `j *` expressions become
  givs, the fourth must be an accumulator or `fold` associates it and loop.c eats the whole address.
  **Both uniform choices are worse.**
- **A `*/` in park prose silently closes the comment** and breaks compilation (`iwram_*/gData`). Every
  park installed today was screened; all clean.
- **zsh word-splitting, sixth occurrence.** The fix has not changed: literal arguments or `while read`.
- **Sixth converse-in-one-family pair**: the walker split is wrong in `Anim_Unused_Fizz` (411/411/407
  against 118) and right in `Anim_Ray`.

## Owner decisions

- **Promote `DMA3_COPY_RW` to `include/dma.h`.** `BuildDraw2DFuncEx`'s 4 pins are all the register
  lines of one *local* dma.h-style helper — it is pin-free the moment the helper is shared, exactly
  as batch 299 did for `DMA3_COPY16_RW` (byte-neutral at the gate). `Func_80c02a4` wants the same,
  so **two functions now satisfy the standing condition.**
- **`UpdateActors` and `Func_8090a5c` return to the pool** — my screen was wrong.
- `Anim_TitanBlade` is the last of eight in `rom_e7320_c_c.s`; doing all four export sets of that
  file in one gated change makes the later splits free.

## Open

- **13 still in 401–800**, plus the 2 I wrongly dropped.
- **63 in 800+**, to be triaged by branch count before assignment.
- `Anim_Unused_Fizz` at 118 of 770 with both axes exact is the best prospect in this batch.
- ~30 frontier parks still carry stale or missing recipes; each is a disabled test.
- Host disk ~1.5 GB free. Unresolved.
