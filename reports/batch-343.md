# Batch 343

Five briefs, 20 targets, **1 landing** and **eight park improvements**. **633 →
632 park files**; census ROM functions landed 4,984 → **4,985**.

`Func_80b90ac` landed byte-identical (brief A). Improvements: `200a614` 21→19 and
**95 pins → 50**, `2008ca0` **depinned outright**, `200a3c4` 29→20, `2008e34`
25→20, `80165d8` 30→14, `80c0f98` 30→21, `200a6c0` 28→23, `20087f8` 32→31,
`80b0a20` 26→16.

## The landing

A **three-way crossing where none of the three works alone** — two pointer names,
`GCSE_CFLAGS`, and a flag byte reached through its own struct tag; all four
combinations measured. The park had recorded **seven spellings "exactly inert"**,
all taken at default flags where gcse pass 07 eats the copy whatever you call it.
Once the copy survives pass 07, r7 falls out free, so the lever is entirely
**upstream of the allocator** and none of the five allocno routes is what paid.

The last three encodings were an **alias set, not the allocator**: `_GetUnit`
returns `unsigned char *`, alias set 0 conflicts with everything, so the byte
store pinned a re-read behind it. That is this tree's "a `char *` lvalue pins it"
**run backwards** — a non-char lvalue *releasing* a pinned load.

## Route 3 pays a second time

`200a3c4` went 29 → 20 on one token — `e = a - 1;` instead of `a -= 1;` — closing
an r5/r6 coin flip and moving the first difference from index 6 to 15. That
refutes its park's diagnosis, which listed "the decrement in three positions and
two forms" as screened. **Second bank** where that route pays, so it is now the
first thing to try on a callee-saved coin flip.

## "positions line up" was wrong, and I had propagated it

I told brief C that `Func_8093af8`'s "116/115 gap 1" row has a 32-bit entry
absorbing the difference while stream lengths agree, "so positions line up" —
inherited from batch 334's report on the same function. Brief C measured it:

- the absorber is a **pad halfword**, not a 32-bit entry (those are 8 against 8);
- **equal stream length with unequal instruction counts does not line positions
  up.** A trailing pad cannot compensate for an instruction missing *mid-stream*:
  `aligncmp` puts the missing instruction at ref 97–103 and the pad at our index
  117, so everything between is shifted. The real residue is **110 aligned-equal
  against 22 in seven hunks**.

`parkcheck` printed that conclusion as a standing note on every such park, so it
had been telling agents the wrong thing since batch 334. Corrected to report what
it measured and to name `aligncmp`.

Brief C also refuted batch 334's "the missing instruction is independent of the
r2/r3 phase" on the same function — it is the **same** defect, one blocker not two.

## A stale claim line hid a park at 17 as 55

Brief B found `200a69c`'s top claim line reading 29 while its body measures 10:
batch 336's improvement *was* carried into the body, but the top line was never
updated — **and the top line is what `parkcheck` and `piece.py` parse.**

I swept the whole 15–60 band for that class: 121 parks, 112 OK, **two** stale
lines. The significant one is `overlays/200a718.c`, which **ranked as 55 for seven
batches while measuring 17**. Both corrected and re-verified.

## My briefing defects this batch

**All twenty piece-membership blocks shipped empty.** I added a filter to strip
`piece.py`'s new duplicate-park warning, and because that warning prints *first*
the filter cut the whole block. Brief A caught it; the others worked without the
data. Restored mid-batch, and brief B was messaged directly because its target is
**member 4 of a seven-function piece, all seven parked** — needing two cuts, not
one.

**I told brief A that batch 334's re-diagnosis applied to its target.** It applies
to the *sibling*; on `Func_80b90ac` cprop **is** the pass, and brief A recorded
that believing my brief would have cost the landing.

**And I described a tool in prose instead of pointing at it.** The shared brief
said autosweep sweeps "every **top-level** statement boundary" — stale since batch
338. **Three briefs read that, believed it, and reported it back as a tool
defect**; one hand-rolled four nested anchor placements it did not need, on a
function where **35 of 42 boundaries are at depth ≥ 2 and the deepest is 5**
(measured). The template now says to read the tool's docstring, which carries its
current behaviour, known defects and measured hit rate, because a brief is copied
forward and goes stale the moment the tool changes.

## The device class, refuted and re-audited

Brief E refuted the address-device class batch 342 added, on a target where it
mattered: `shimcount` called `Data_80ec8` an undefined extern **on a park whose
landed sibling takes the same address the same way** — which cannot be a device,
because the landed file links. It is defined by `.incdata`, which expands to
`.global \sym` plus `\sym:`.

**Second time the macro layer broke that check.** Batch 342 hit it with
`_FIXUP_RAM_CODE_SIZE` from `.func_end_emit_size` — which globalises its *second*
parameter — and I responded by restricting the class to parks only. **That hid the
defect rather than fixing it:** a park is exactly where an undefined extern
legitimately lives. `shimcount` now parses `macros.inc` for which parameter
positions become symbols. Audited count **26 → 23**, the three dropped all `Data_*`
from `.incdata`.

## Depin results

- **`2008ca0` depinned outright**: its 6 pins buy only 7 encodings, and
  `-fno-strict-aliasing` is worth 7 on *either* body, so pin-free + flag (23)
  **beats** the pinned park (24). Pin-free won the number as well as the policy.
- **`200a614` 95 pins → 50**, 21 → 19. Its inherited control was invalid as
  predicted: "all 48 mov-order permutations measure exactly 21" was taken with the
  pins in, **where the pins are the fill** — it swept that site's *order* and never
  tried *removing* it, which is worth 2 and retires the whole first cluster.
- **Compositionality is per-park.** One park's twelve mildly-load-bearing sites
  cost 2 each alone and **51 jointly**; another's seven cheap sites cost 2 each and
  **compose exactly**. The cliff is a **set, not a count**, and the
  `+4 bytes / +2 instructions` is the signal.

## Mechanisms worth promoting

- **`allocno_compare` as a working instrument.** Brief C predicted `.18.greg`'s
  `;; N regs to allocate:` line from `.17.lreg` for **three functions and 28
  allocnos exactly** (`global.c:1875-1881` confirms that line *is* the priority
  order). Where every competitor has `n_refs = 2` the price collapses to
  `20000/span`, which turned one store-order fix into a prediction that held first
  try.
- **Hoist the LAST stack argument, not the first.** Brief D's 28 → 23: expand
  emits the two argument sets and stores adjacently, so their ranges are
  **disjoint** and one register serves both. Hoisting arg6 makes it live across
  arg5's set, the ranges **overlap**, two registers are allocated, and both `str`s
  become byte-identical. Hoisting **arg5** instead is inert.
- **Thumb pools come from `arm_reorg`, not `output_constant_pool`**, and the dump
  point is the **last** natural barrier within `fix->address + pool_range`
  (`arm.c:5517-5534`). With one SImode fix at `pool_range` 1020 the ROM's position
  is unreachable; a corpus scan found **244 inserted against 145 natural**
  mid-function dumps, and every natural single-word case read by hand is driven by
  an **HImode** load at `pool_range` **64**.
- **`-fno-strength-reduce` is not corpus-inert.** `GetWeaponType` skipped it for 24
  batches on one unrelated function's negative from batch 63; it is the biggest
  lever in that file and autosweep found it in one command.
- **`cse.c` in gcc-2.96 has zero `MEM_ALIAS_SET` references** — a one-line
  re-checkable proof that no typed spelling reaches cse1's MEM commoning.
- **The `cse.c:5741-5745` barrier is not a class-wide lever**: 29 → 10 on one
  target and 25 → **27** on another in the same blocker class.
- **An unsigned loop counter can subsume `-fno-strength-reduce`** by undoing
  `check_dbra_loop`'s down-counter reversal — but that tell does *not* generalise:
  another target has the byte-identical construct and is genuinely signed on both
  sides.

## Open

- `parkcheck` verifies at **production flags**, so a flag-dependent regression is
  invisible. One installed body had silently regressed to one instruction *long*
  under the very flag its proof is written against.
- `Func_8011164`'s split is **refused** — needs `.global .L132cc` exported and
  gated alone first. Zero `.lcomm`/`.comm` in all four pieces, so the batch-339
  class does not apply.
- `autosweep` still cannot be aimed at a **candidate**, only an installed park;
  two briefs wrote helpers for it, and one confirms flag×source crossing *is*
  possible in-toolchain — `OBJCMP_EXTRA` just has to precede the image name.
- A committed compiler dump (`g5_200d64c.c.18.greg`) was sitting in the repo root
  since `a1d09e06`; removed.
