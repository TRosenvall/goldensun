# Batch 341

Five briefs, 20 targets, **0 landings** — and the most productive batch of the
week for everything else. Park files 642 → **641**.

Nine parks improved, one 96-pin park depinned to 20 at an equal figure, one park
retired as a duplicate, **five autosweep defects found**, and two standing bounds
refuted.

## The pool-load hypothesis transferred, and the floor it was tested against was not a floor

Brief A was given batch 340's finding — an anchor after `queue = &gDMATaskCount;`
*inside the static inline* took `AnimEnd` 14 → 2 — and told to re-measure the
class. It held on both parks named for it: worth **5 each** on `AnimStart` and
`AnimStart2`, and on `AnimStart` it moves the `gDMATaskCount` pool word onto the
ROM's `+0x7c`, which is that park's own named cause-1 symptom.

**But only the FIRST of the four inlines matters.** All 15 subsets measured on
both parks: any subset containing the first is 66/78, every subset without it is
71/83. The address is built once and copied, so N inlined queue copies have **one**
anchor site, not N. Do not sweep the copies.

`Anim_CriticalHit` **10 → 2**, and the brief I wrote was wrong about why it could
not move. I told brief A that 10 was "a floor for its dimension across a 39-row
crossed sweep". That sweep crossed five block shapes against eight **barrier**
placements and never varied the *instrument*:

| body | figure |
|---|---|
| no device | 13 |
| barrier | 10 |
| anchor30 | **6** |
| anchor30 + 83 | 4 |
| + `p` as an index | **2** |

So the barrier was costing **2 encodings and the pass-3 debt**, and the park now
reports no device at all. Also `Anim_Venus` 20 → 16 with its "1 pin" retired — a
plain `void *tf;` in the same block measures identically; the park had tested
"without the pin" by deleting the whole block and credited the pin for the
carrier.

## Five autosweep defects, all false-clean-negatives

Found independently by briefs A and C, and they matter because this tool is now
run before every batch:

1. **`--max` defaults to 40 and nothing said the axis was truncated.** Brief C had
   four targets over the cap (88, 94, 124, 179 boundaries) and **two improved past
   boundary 40** (`Func_80a112c` at 58, `Anim_Djinni` at 44). Brief A lost an
   anchor worth 4 on `Anim_Venus` (97 boundaries, winners at 56 and 58) and 3 each
   on the AnimStart pair. The tool now prints the denominator and **never calls a
   capped axis closed**.
2. **The anchor axis could not reach a `static inline` at all** — `boundaries()`
   saw the subject's body only, which is precisely where batch 340's `AnimEnd`
   answer and both AnimStart levers live. It now also sweeps inline helpers the
   subject calls (12 extra boundaries on `AnimEnd`).
3. `resolve()` took the first function defined rather than the subject (fixed in
   340; **61 parks** have a first-definition that is not their subject).
4. The relocation warning was false for absolute `.sym` symbols (fixed in 340).
5. `boundaries()` walked brace-depth-1 only (fixed in 338).

**And that invalidates a claim I made to the owner.** I reported the batch-338 and
batch-340 pre-passes as closing the anchor axis on 32 and 14 parks. Those ran at
`--max 24` and `--max 22`, and **412 of the tree's parks have more than 24
boundaries** — so for any long park among them the axis was truncated, not closed.
The two zeros and two improvements those passes found are real; the "closed"
verdicts are not.

## Two standing bounds refuted

**The `REG_BASIC_BLOCK < 0` escape sits downstream of a gate that always closes
first.** Batches 339/340 recorded the pin-free escape as `local-alloc.c:884-889`,
noting none of their parks had a basic-block boundary. Brief E's target **has**
one, and five pin-free spellings — three straddling it — all measure *exactly* 42
of 184. `.00.rtl` holds the slot pseudos above the branch test, so the boundary is
real in RTL; by `.03.cse` those pseudos **do not exist**, because cse1 folds the
sub-256 constant back (`cse.c:509-514`) and `update_equiv_regs` never sees a
pseudo. So the next question is not "is there a boundary" but **"what carrier for
a sub-256 constant survives cse1"**.

**`allocno_compare`'s formula was misstated in this document.** `global.c:600-618`
is `floor_log2(n_refs) * n_refs / live_length * 10000 * size` — seven write-ups
omitted the `* 10000 * size`, and line 6005 claimed it "weights priority by
basic-block frequency", directly contradicting the tree's own section saying it has
no frequency term. Frequency reaches the sort only through `n_refs`
(`flow.c:4948`). Both corrected.

## The pin instrument was under-reporting

`shimcount.py` expanded a pin macro's arity only when the macro was literally named
`PIN<digit>`, so a file naming its macros `R0..R3` reported **4 pins while carrying
96** across 36 blocks — I briefed it as "4 pins". Pin macros are now matched by
*shape*. Tree-wide: **14,517 → 14,774** pins, so 257 were uncounted; none of the
eleven parks briefed as pin-free this batch actually carries pins.

Brief D then took that file **96 pins → 20 at an identical 14 of 273** — same
instruction counts, same byte size, same first differing index. A single-drop
sweep reproduced the park's load-bearing set and all 29 inert blocks dropped
jointly.

> **Removing the pin first and only then sweeping is a different search space, not
> a handicapped one.** On `OvlFunc_897_200aeb0` the pin-free body sits at 120 of
> 148, and `p += 0xf;` as its own statement takes that **120 → 12**, with one
> anchor → 10. That park's own control — "the walk spelling never mattered" — was
> measured *with the pin in*, where the spelling is worth 108 encodings.

One useful exception to the week's pattern: that park's barrier **cannot** be
replaced by an anchor (33, same as removing it). Its value is the two extra
*references*, not the fence.

## A park filed twice

`ovl_7b0400/2009af0.c` was a **copy** of `ovl_7f2f14/2009af0.c` — code-identical
at 2,985 characters, its own subject line naming the other overlay's function and
its own recipe naming the other overlay's `.s`, dating to batch 316's repoint pass.
The tell was in the brief: `piece.py` returned the same block for both paths, so
`TARGETS.md` printed it twice.

The function actually at that address in `rom_7b0400` is a ~1,900-line cutscene
routine with **no park at all**, and `OvlFunc_926_2009af0` — which my brief named
— does not exist anywhere in the tree. The better revision was merged into the
survivor, the duplicate retired, and `frontier.md`'s stale row removed.

## Mechanisms worth promoting

- **Raising a priority competes; ending a region does not.** On one site the
  barrier buys 3, the anchor buys 7, and both together are worse than the anchor
  alone.
- **The preheader LUID tie has two handles.** Batch 327's rule is the handle on the
  *hoisted* quantity and fails for any function-wide constant — which `0` always
  is, explaining one park's long-standing `msk`/`zero` asymmetry. The second handle
  is on the *competing* insn: make it an induction variable so it leaves the
  preheader.
- **A memory read pays one forward dependent per later call in its block; a pool
  load pays none** (`haifa-sched.c:3553-3556`, `:3955`, `:3217-3223`) — so the
  property that stops a pool load *gaining* a dependent also loses it the
  dependent-count rung at `:4097-4110`.
- **Three LO reload scratch registers one step round the cursor**
  (`reload1.c:4937/4962/5003`): for one insn `find_reg` printed **r2, the ROM's
  register**, and r1 was emitted. Same door as `ovl_7aa430/2009a3c.c` — two parks,
  one mechanism.
- **The deferred-store class runs both directions in the battle bank**: one park
  has the ROM storing immediately where we defer (an anchor fixes it), another has
  the ROM deferring where we do not (no placement helps).

## Open

- Seven parks now found quoting the **stream count on the instruction line**,
  across three batches.
- `Anim_CriticalHit`'s last 2 is 4 against 5 with LUID on our side, so a tie is a
  win; the bound is "get `GetBattleActorPos2` out of block 25".
- `OvlFunc_954_2008a3c` at 11 and `ovl_7aa430/2009a3c.c` share one door.
- `_FILE_e4` is comment-only at `file_table.sym:159`, so it is a genuinely
  unresolved symbol — **not** the ".sym defined by value" case.
