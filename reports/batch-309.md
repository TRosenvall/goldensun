# Batch 309 — reading two spill maps together, and the partition lever bounded

*Written after the fact, with batch 308's report — see the note there.*

Three agents, 7 targets — the last of the 401–800 band. Gate green throughout.

**0 landed, 7 parked.** State at close: **4,787 of 5,710 (83.8%)**, with the band down to 1.

## The best technique: read TWO spill maps together

A spill map gives the declaration order of the locals that **spill**. One that stays in a register is
**invisible in that map yet still numbered**, so it still occupies a position. Across the
`Func_8025200` / `Func_802592c` pair a single order satisfies **both** maps, because each spills a
different subset and the subsets interleave:

    s, top, prev_top, row, prev_row, gfx, [unit], boxA, nitems, keep, cy, cx

Neither map alone determines it. **Bounded in batch 310:** the technique needs the two functions to
spill *different* subsets — on a true twin pair whose streams are identical, both maps are identical
and the pair carries what one carries.

Two sharpenings: **a slot dates the DECLARATION, never the first assignment**, and **a gap in the
aggregate layout is a declaration**, not padding.

## The partition lever, bounded twice in one batch

- **A shared hard register is not evidence of one variable.** On `BaseAnim_RapidSlash`, r8 appears in
  all three loops *precisely because the ranges do not conflict* — **splitting** that counter was the
  largest step on the function (41.0% → 47.3%).
- **The direction is not fixed.** From identical r8 evidence: splitting paid **+6.3** on RapidSlash,
  unifying nine counters paid **+8.6** on `Anim_TitanBlade`, where it moved size, count *and* aligned
  toward the reference together. **Try both**; the discriminator is the reference's **reference count**
  on that register.
- **And splits are NOT ADDITIVE** — three pure *reorder* probes were inert on `Func_8025200` while the
  whole partition at once moved 69 encodings. A reorder changes ranking; a partition changes the allocno
  **count**.

## When two trusted rules conflict: access count beat slot order by 12

The ROM's slot map says a shifted value is a declared local — and **declaring it costs 12 encodings**,
because the ROM recomputes it at all nine sites. The resolution is not that the slot rule is wrong:
**a slot tells you a quantity existed, not that the source held it in one place.** When they disagree,
count the accesses.

## Parks

| function | figure | note |
|---|---|---|
| `Func_80bae40` | 789 of 830 | **size exact** |
| `Func_80a6ccc` | 736 of 774 | frame exact and fully explained |
| `Func_8025200` | 712 of 817 | 63.4% aligned |
| `Func_802592c` | 795 of 838 | partition partly **undone** vs its sibling |
| `BaseAnim_RapidSlash` | 740 of 778 | 50.3% aligned |
| `Anim_TitanBlade` | 731 of 816 | 63.0% aligned, family's best opening pass |

## Other findings

- **A partition does not transfer wholesale between near-siblings** — these two share ~265 identical
  instructions and still needed different partitions, one being two allocnos *long* rather than short.
  A required mask in one must be **dropped** in the other, discriminated by **provenance**: a call
  return versus a memory read.
- **Thumb `ldrsh`/`ldrsb` have no immediate-offset form**, so a `mov`+signed-load pair is **ISA-forced**
  and not a missing index-local lever. Third bound on that family.
- **Keep a change on encoding evidence when the figures disagree with it**: a pointer walk moved **both
  figures the wrong way** while making the addressing **exact**, and was correctly kept.
- **There is no `rowx2` variable** — the ROM's three `mov/lsl/str` blocks are gcse/PRE insertions on
  every edge that redefines `row`; writing them by hand makes gcc cross-jump them away.
- **A frame "hole" can be a third array** addressed `mov r0,sp / add r0,#K` rather than
  `add r2,sp,#K` — and a separate recon's "40-byte hole" was simply ten spill slots. **Two phantom
  holes in one batch**, both from reading a frame before the slot map.
- **Ranking demonstrated in both directions**: on one function candidate v1 was best on size and count
  while 28 aligned points worse; on another, the partition took **size to exact** while the aligned
  figure went *down*. Neither figure dominates — record both.
- **I rejected a recon's headline correction after measuring it.** It reported `UpdateActors` as 11
  veneer sites across three registers and recommended re-ranking it *up*; scoped to the function and
  anchored on the leading dot it is **31 sites across five registers, two of them high**. Those counts
  were of `bl _call_via_rN` — the freely-reachable form. My own first screen was also unreliable:
  **POSIX `awk` has no `\s` class**, so a `\s`-anchored range pipe silently matches nothing and prints
  0, which reads exactly like "clean".
