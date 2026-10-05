# Batch 328 — organised by upstream module, and the verdict is "not quite"

**8 landings, 20 parks held or improved, 0 pins added, 0 devices shipped, 0 new
flag groups.** Progress **4,875 → 4,883 of 5,710 (85.5%)**; remaining 835 → 827,
parked 743 → 735. Every installed park figure independently re-measured.

Seven briefs at four targets each. **The best landing count of any batch this
session**, and the organising idea that produced it is the thing most worth
arguing about.

| brief | grouping | result |
|---|---|---|
| A | `overlays/ovl_30.s` (2,261 landed) | 4 parks; an **exact body at 2 pins**, parked on policy |
| B | `overlays/ovl_314.s` (454 landed) | **`OvlFunc_918_2009424` → 0**; 6→2, 4→3 |
| C | `ovl_7ac2d8` **by piece** | **two landings from one body** |
| D | `rom_15000/rom_1aeec.s` (34 landed) | **`Func_801c154` 8 → 0** |
| E | close `rom_15000` | **three landings**: 3→0, 3→0, 4→0 |
| F | `rom_a1000/rom_a8604.s` (25 landed) | **`Func_80a9d84` 33 → 0** on the first variant |
| G | `rom_15000/rom_23178.s` (26 landed) | `Func_8029274` 6 → 2 |

## Did module-grouping work? Three briefs, three different answers

This batch was the first organised by **upstream module** rather than by bank,
which only became possible after batch 327 fixed `upstream_module.py` (it had
reported `our parks: 0` for modules holding nine). The verdicts:

- **Brief B: it won decisively.** Three of its four results came from
  module-mates and none from the bank. Its example is unanswerable: `200db90`
  (sitting at 2) **already held the solved lever** for exactly what `200b600` was
  stuck on, and the two parks **share no path prefix at all** — a bank-organised
  brief would never have paired them.
- **Brief G: a cheap hypothesis generator, not a better bank.** Its landed
  module-mate supplied the **frame** and not the **edit**: the rule *"the
  variables **are** the allocation"* is what sent the round at allocation order
  instead of spellings, but **every one of the three concrete facts that sibling
  suggested measured inert or refuted.** Its own improvement came from crossing
  the park's rejected rows.
- **Brief A: for the overlay banks, group by RESIDUE SHAPE instead.**
  `overlays/ovl_30.s` **is not a module in the useful sense** — it is the whole
  overlay-30 cutscene bank, and the tool returns the identical 2,261-landing
  listing for all four targets, so it gave no shortlist at all. What beat it was
  a **generated-vs-hand shape regex over `asm/`**, which also yields a
  reachability bound: one residue shape has **0 hits in gcc-generated `.s`
  against 4,721 in hand-written `.s`.**

> **The synthesis is G's, sharpened by A: "same module" was a weaker predictor
> than "SAME BLOCKED QUANTITY".** Three parks now want the identical thing — a
> zero-instruction extra reference or dependent on one allocno — and only two are
> module-mates. That is the axis for the next batch.

Module-grouping is **worth keeping as a lookup, not as a partition.** It found a
landing nothing else would have (brief B), and it mis-sized a whole brief
(brief A).

## Two landings from one body

Brief C's second landing cost nothing. `dupfuncs` said one overlay piece copied
another; brief C **checked it properly** — a normalised diff is **empty** after
mapping 2 function names, 3 data names and every `.L<hex>` label — so the body
ported **by rename alone**, with identical figures *and* identical first
differing indices.

With **272 overlay functions parked** and `dupfuncs` reporting **74 of 101**
group members free once the representative lands, this is a shape rather than an
anecdote.

Also established: **whole-piece figures are additive** (one TU read exactly the
sum of the two `--func` figures, no cross-function interference), a whole-piece
`.c` can only be **installed** once both functions match, and **`--func` cannot
see a prototype conflict** between two parks of one TU — these two declared the
same three callees incompatibly.

## The mechanisms

- **`promote_mode` (`explow.c:895-902`) applies `PROMOTE_MODE` only to
  `INTEGER/ENUMERAL/BOOLEAN/CHAR/REAL/OFFSET` types — a `RECORD_TYPE` is not in
  that switch.** `arm.h` forces `UNSIGNEDP=1` for QImode, so every *scalar* char
  local becomes an SImode `zero_extend` with `nonzero_bits ≤ 0xff` and
  `simplify_comparison` kills **both** shifts; reading the same byte as a
  **struct field** keeps it QImode and the ROM's `lsl r3,#0x18` survives. Closed
  `Func_801f730`. **The axis ~45 recorded bodies never varied was the TYPE
  CONSTRUCTOR of the lvalue.**
- **A bitfield's alias set comes from the field's DECLARED TYPE, not the access
  width** — so a park that closed `DIFFERENT_ALIAS_SETS_P` on "both references are
  char-precision" was right about what it tried and wrong as a bound. Four-way
  confirmed, with `-fno-strict-aliasing` returning the figure to exactly 3 at
  exactly index 35. Closed `Func_801d014`.
- **A `REG_UNUSED` insn still takes a hard register**, because it is live in
  `.12.life`, combine substitutes it away, and **gcc-2.96 runs no flow pass
  between combine and allocation.** The fix was never a re-spelling of the real
  values: `int zi = 0; u8 z = zi;` — **both** an SImode and a QImode zero had to
  exist as real pseudos.
- **`regmove.c:1200-1205`** skips a two-address commutative rewrite only when
  `replacement_quality(comm) >= replacement_quality(src)`, and
  `replacement_quality` (`:341-368`) scores **a parameter's pseudo 1** against a
  constant's 3 — so regmove **always** moves a two-address destination off a bare
  parameter, which explains why one park's two `and`s were wrong in *opposite*
  directions.
- **`loop.c:1803` gates a hoist on `threshold * savings * lifetime >=
  insn_count`** with every term fixed, so the only reachable quantity is whether
  the set is **a movable at all**. `Func_80a9d84` landed by reusing a local
  already carrying another constant's copy: two sets in the loop disqualify the
  pseudo. The park had varied that constant's **spelling** four ways and never its
  **identity**.
- **`arm_adjust_cost` never raises a cost** and returns **0 for anti/output**, so
  a store whose only dependent is an anti dep has **priority 0**.
- **There is no `sched1` in this build at all** (`.13.combine` → `.23.sched2`), so
  any reasoning about the register-pressure rung concerns a pass that does not run.
- **`_CONST_1f` IS VALIDATED.** `const.sym`'s own entry has read *"UNVALIDATED,
  AND THIS ENTRY SAYS SO … `make compare` still FAILED"* since batch 276. The
  inline reference landed a **different** function here and the gate is green, so
  the symbol was always real and its earlier failure was specific to that other
  function. The extra `R_ARM_ABS32` is the **phantom relocation** class
  `const.sym` already documents.

## A question answered, and the answer splits

Brief G found three parks wanting "a zero-instruction extra reference or
dependent" and brief E tested it:

> A zero-instruction **ordering** exists — a bare `__asm__ volatile ("")` sets
> `reg_pending_sets_all` and totally orders the block, so the ready list holds the
> insn **alone** and `rank_for_schedule` is never consulted. But it supplies **no
> reference** to any named pseudo, which is exactly why `shimcount` ignores it, so
> it **cannot** raise a `REG_N_REFS` count.

**So the two parks wanting a DEPENDENT are reachable and the one wanting a
REFERENCE is not.** Naming the operand buys the reference and costs the
fakematch.

Also from that work: **a device that perturbs the allocation is not an
isolation** — one volatile store forced a value out of r8 into r7, because Thumb
`str` cannot encode a high register, swapping two variables function-wide.

## Information that exists, indexed under a different name — the third instance

The mechanism that landed `OvlFunc_924_200d158` **was already in
`docs/elevation.md`**, worked end to end — under the address of its `dupfuncs`
**twin**. The park never connected them **because parks are filed by their own
address**, and it cost that function two batches.

That completes a set:

| | the information existed | indexed as |
|---|---|---|
| batch 327 | nine parks in a module | landings split-named, parks address-named |
| batch 327 | 148 landed `Anim_*` sources | a filename scan stood in for a definition scan |
| batch 328 | a solved mechanism | filed under the twin's address |

> **When a park resists, search for its DUPLICATE's symbol as well as its own.**

## Prose that looks like machinery — four instances, now guarded

Both deferred tool fixes are the same defect as the comment-terminator trap.

- **`parkcheck`'s `header_of` returned only the first `/* … */`**, so a brief that
  prepends a *separate* block strands the recipe in block 2 and the park reports
  `UNCHECKABLE` **with a correct figure sitting in it**. Four parks this batch; a
  recurrence of batch 311, fixed then only for leading `//` lines. It now takes
  the leading **run** of comment blocks, stopping at the first non-comment line
  so a recipe quoted in the body cannot be mistaken for the park's own.
  Corpus-checked: **0 parks lose a visible recipe**, `HEADERCUT` still fires.
- **`install_batch`'s `park_subject` took the FIRST `--func`** and captured the
  word `and` from a park reading "`--func and --whole:`" — after which its caller
  correctly **refused** to retire that park. The guard working, on a fiction. It
  now prefers a function-shaped capture and falls back to the **last** `--func`,
  because a real recipe sits at the end of a header after the prose discussing
  it. Corpus-checked: **four** subjects change and **all four are corrections**
  (three captured `AND`, one captured `2`). My own manual prose-rewrite earlier
  was one instance of a four-instance bug.

## Carried forward

- **Organise 329 by RESIDUE SHAPE**, not module or bank — the one axis all three
  verdicts point at. Brief A's generated-vs-hand shape regex is the tool.
- **Check `dupfuncs` before working any parked overlay function.** 272 are
  parked; 74 of 101 group members come free with their representative.
- **`OvlFunc_927_2009818` is exact at two pins**, and the control shows **the
  ORDER is the lever, not the pins** — a pass-3 item with a known answer.
- **Three parks share one blocked quantity**; two are now known reachable.
- `OvlFunc_924_200cfcc` at 5 with route (c) found and kept as an instrument;
  `Func_801bcd4` at 13 with both cse2 sub-conditions binding from both sides;
  `DisplayMenuArrowCursor` at 6 with the bound now **symmetric**.
