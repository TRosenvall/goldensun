# Batch 305 — two landings, and sched1 does not exist

Five agents across the twelve closest parks. I ranked the parked set by claimed distance rather
than by recollection, which itself turned up parks nobody was tracking.

**2 landed, 7 parks improved or corrected, 19 park files repaired.** Gate green at
`5c4695205413df7db52b9a184815a07783999971` throughout.
State: **4,785 of 5,710 elevated (83.8%)** — 91 available, 743 parked.

## Landed

| function | from | how |
|---|---|---|
| `OvlFunc_945_200c8e8` | 6 of 696 | **one line** |
| `Func_80f62b8` | 17 of 191 | two levers, then a text split |

**Both parks had their blocker wrong**, which is the pattern of this batch.

`OvlFunc_945_200c8e8`'s park blamed `global_alloc` and proposed declaration-order work. Case
0x13 is straight-line — one basic block — so its values go through **local-alloc**, and gcc says
so itself: `.18.greg` lists the eight allocnos it places and these pseudos are not among them.
`allocno_compare` never ranked them, so the proposed work could not have moved anything. With
that corrected, `.17.lreg` shows `t10` carrying a fourth reference only because `t10 - 0x60` was
its own insn; spelling that argument as a literal drops it to three refs and the order flips to
the ROM's. The park had excluded the whole family on a predicted +1 that does not occur — cse
reaches the constant in one insn from a scratch already holding a neighbour.

`Func_80f62b8`'s park called its residue a `global_alloc` floor with "nothing source-level". It
was source-level twice: an **r0 donor** (the `__divsi3` first argument, whose range dies at the
divide) took 17 → 6, and the last six were `fold` canonicalisation — **fold inverts a ternary
when one arm is simpler**, so writing both arms as subtractions removes its reason to invert.

## sched1 does not run in this build

Verified directly with `-da` at production flags. The dump sequence is

    … 17.lreg  18.greg  19.flow2  20.ce2  23.sched2  25.jump2  26.mach

**There is no sched1 dump** — `flag_schedule_insns` is off at `-O2` here, so only the post-reload
scheduler ever runs. Consequences:

- **13 park files attribute a residue to "sched1"**, a pass that never executes. All 13 are now
  annotated in place rather than silently rewritten; the residues are real, the pass named is not.
- **`-fno-schedule-insns` being "inert" is evidence of nothing.** It appears in 36 park files,
  usually as a ruled-out alternative. It controls a pass that never runs, so its inertness was
  guaranteed a priori.
- The inference *"`-fno-schedule-insns2` is worse, therefore not sched2"* is **invalid**.
  Disabling the only scheduler makes the block worse; that says nothing about the specific order.

**What actually decides a sched2 tie**, from `rank_for_schedule`: priority → **dependent count
(more wins)** → `INSN_LUID` (lower wins) — and LUID is set at expand, so the last tie-break
*preserves expand order*. `precompute_register_parameters` hoists any argument with
`rtx_cost > 2` before any hard argument register is written, walking arguments forward, and
`load_register_parameters` then walks from argument 0 up. **So argument 0 is always emitted
first and no spelling reorders it.**

That single mechanism explains three separate "unreachable" residues: `Anim_Froth`'s last 4,
`OvlFunc_881_200b9fc`'s last 2, and `OvlFunc_959_200d0e4`'s last 3. **The escape, where one
exists, is the dependent-count term** — a landed sibling inverts the identical tie only because
its callee *returns a value*, so the `call_value`'s own `set r0` intercepts r0's chain and wins
3 dependents to 2. A `void` callee cannot. Corpus calibration: the ROM's order occurs 199 times
against 335 for the opposite across 4,371 generated `.s` files, so both are reachable in
principle.

## Anim_Froth is now the closest non-matching function in the tree

**10 → 4 of 529**, with size, count *and* relocations exact, 99.6% aligned, pin-free. One
statement plus two locals: assigning `tbl` **inside** the `while` condition makes the guard copy
and the loop-bottom copy one declared variable and therefore one hard register — the ROM's r2 —
while keeping a pool load per copy so the count survives. An index local keeps the `+1` in the
index register where combine cannot fold it into the load's immediate. Comma order is
load-bearing: `n` first 4, `tbl` first 10.

Its last 4 are **argued unreachable** by the sched2 mechanism above, across 21 spellings, 9
source positions and 9 flags all reading exactly 4 at the same four indices. This park should
ship at 4 rather than absorb more rounds.

## An unparseable recipe disables the only check on the figure it hides

Six parks carried a **placeholder** in the recipe's candidate slot — literally `<this file>` — so
`parkcheck` could not parse it, reported UNCHECKABLE, and **never re-measured them**.

Filling in the six paths immediately exposed a park lying about its own body:
`DisplayMenuArrowCursor` claimed **6 of 133** and measures **16**. Its header narrated the
improvement — "WAS 16; batch 297a took it to 6 with one lever below" — and the lever was never
written into the C. That is the batch 282/283 failure mode for the **third** time, and it hid
indefinitely because the check that catches it could not run. The claim is corrected to 16 with
the lever marked UNAPPLIED.

The same pass surfaced **`ovl_7bdeb0/2009984.c` at 4 encodings**, invisible to every ranking
pass for the same reason.

So: an UNCHECKABLE park should be triaged like a failing test, not like missing polish.

## Other results

- **`Func_8018efc` 17 → 2 of 119**, pin-free, every flag inert. `base` and `q` **are one variable
  in the ROM** — worth 14 of the 17. The park had blamed an "r6/r7 role swap"; it was two
  variables where the ROM has one, which is why its three recorded-inert probes were inert: none
  changed the allocno *count*.
- **The reuse lever gained a fourth precondition: the donor needs a use of its own.** On
  `OvlFunc_968_2009af0` two dead-before-loop variables sat in exactly the wanted registers and
  reuse was byte-identical in four placements — gcc coalesces the assignment into the argument
  store and discards a donor whose only use is that argument.
- `200cda0` **15 pins → 9** at the identical 19; pin-free measured at 31, so the pins cost 12 and
  work precisely *because* they block constant propagation. Its 3 barriers are irreplaceable —
  six spellings of one constant all come out two instructions short, because gcc commons the
  value however it is written. Its recorded `-fno-*` figures of 146–174 were **inflated by a
  pool-offset cascade** and the real residue there is 2–4 instructions.
- `80a112c` **20 → 18**, and the reuse lever is excluded for its cluster B **with a reason**: every
  early local is spilled, so there is no r0 or r4 donor in the function at all.
- `OvlFunc_923_200a030`: a header claim **does not reproduce** — a hand-written division form
  recorded as "14, inert, in four placements" measures **20** across seven spellings, retiring
  that park's stated next plan, which used the form as a schedule-neutral control it is not.
- `OvlFunc_924_200d5c0` reported MISMATCH (claims 16, measures 14): its body was improved in
  batch 300 and the line-1 claim never updated — 282/283 **inverted**. It also carries two copies
  of its own header.
- `Anim_Whirlwind` holds at 26 with two routes closed. Four source shapes for the row giv are
  **byte-identically** inert, which says loop.c derives givs from the counter's own uses in its
  canonical order, not from source order. And the `while`-condition comma device **does not
  transfer** — LICM hoists an invariant assignment straight back out, so the device bites only
  where the value must be *rematerialised* at each copy.
- `200a094` holds at 2 with its flag rung closed (19 flags) and the cause made exact: loop.c
  hoists the invariant and rewrites the address insn into a copy, so the residue is a **LUID
  difference produced by loop.c**, not a scheduling preference.
- A `diff=0` diagnostic exists for `200b9fc` and was **correctly withheld** — it is a pin in
  disguise that lies about a real callee's arity. It proves every other byte is right.

## Process

I hit the same first-block hazard a **third** time: annotating 13 parks by prepending a note
pushed their recipes out of the first comment block and turned all 13 UNCHECKABLE. Caught by
`parkcheck`, re-merged into the first block. I also overwrote two park headers with body-only
files — the delta-versus-replacement mistake — and my first repair cut at the wrong boundary and
duplicated declarations. Both rounds caught by `parkcheck`, which is the argument for running it
on every park you touch, including ones you only annotated.

## Open

- **`Anim_Froth` at 4, `2009984` at 4, `200b9fc` at 2, `200d0e4` at 3, `Func_8018efc` at 2.**
  Four of the five are argued to be at a floor by the sched2 mechanism; the honest move is to
  ship them as parks and stop spending rounds.
- **All six placeholder recipes are fixed**, but ~30 frontier parks still carry stale or missing
  recipes. Each one is a disabled test, so that queue is worth clearing before more park work.
- **Host disk recovered slightly to ~1.6 GB free.** Still unresolved.
