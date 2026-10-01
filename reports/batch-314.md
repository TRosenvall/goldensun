# Batch 314 — pass two opens with seven landings, and every single park had its blocker wrong

Five agents, 21 of the closest parks, ranked by distance rather than size. Gate green at
`5c4695205413df7db52b9a184815a07783999971` at every commit.

**7 LANDED**, 2 parks closed by measurement, 1 park depinned 93 → 45, 2 tools corrected.
State: **4,795 of 5,710 (84.0%)** — 915 remaining, 824 parked, 0 available.

First movement on the matched count in many batches. Pass one attempted everything; pass two is
landing it, and the opening result is that **the parks were the obstacle, not the compiler**.

## Landed

| function | from | what actually did it |
|---|---|---|
| `Func_8018efc` | 2 of 119 | a `*(unsigned short *)` cast — **one token** |
| `Func_809b450` | 2 of 145 | a real struct shape instead of `unsigned char *` |
| `OvlFunc_882_2009b18` | 2 of 545 | **one word**: `extern void` → `extern int` |
| `OvlFunc_969_200cbec` | **2 of 1068** | a union across nineteen accesses |
| `Anim_Froth` | 4 of 529 | **one word** |
| `Func_80b0fa4` | 3 of 138 | the `PLUS` operand order |
| `OvlFunc_common1_1ecc` | 3 of 100 | **one word**, on a *callee* |

## All seven parks had their blocker wrong

That rule now has the standing of a measured mechanism — batch 305 (both landings), 310 (all
three), 314 (all seven). What the parks said:

> *"an exact sched2 priority tie falling to `INSN_LUID`"* · *"sched2 tie. **UNREACHABLE FROM
> SOURCE — see PROOF below**"* · *"pre-reload scheduling"* · *"expand-order, no spelling reaches
> it"* · *"reload's output-reload register, no reach from source"* · *"not reachable from source"*

None was the cause.

> **A park's FIGURE is evidence. Its DIAGNOSIS is a hypothesis — usually written at the moment the
> author ran out of ideas, which is exactly when a wrong attribution gets recorded as fact.**

## The lever: the dependent count is source-controllable through ALIAS SETS

The batch's most productive finding, and new. The tree recorded sched2's tie-break as
*priority → dependent count → `INSN_LUID`*, with **the only escape** being r0 interception by a
`call_value`. True, and far too narrow:

> **Whenever one competitor in the tie is a MEM, the dependent count is source-controllable
> through its ALIAS SET — in BOTH directions.**

- **Adding** a dependent: `*(unsigned short *)&win->w` at one site widens the load's alias set, it
  picks up anti-dependences on the block's stores, reaches 3 against 2, and **lands** the function.
- **Removing** one: a park's own `unsigned char *f28` put that MEM in **alias set 0, which conflicts
  with everything**, manufacturing a dependence onto a store **it cannot alias**. Giving it a real
  struct shape removes the false dependence and **lands** it.

**`alias set 0` is the thing to look for.** A `char *` or `void *` walk conflicts with every other
MEM in the function, inventing dependences sched2 then obeys. That is why 26 probes and 11 flags
had been inert on one of these: every attempt tried to separate a priority dominated by a shared
path.

**Structural bound, measured:** the lever reaches none of brief E's four, because every residue
there is a reload-stage decision or a tie between two *constant register sets* — **never a MEM
against a non-MEM**. One look at the two competing insns settles whether it can apply.

### And the DEPENDENCE TABLE is a rung below the positional figure

    size → instruction and pool counts → opcode histogram → call multiset
      → positional figure → **the dependence table** (`-fsched-verbose=6`)

Two landings came straight off it. The positional figure says two encodings are swapped; **only
the dependence table says which key decided it.** On one function `-fsched-verbose=8` showed 4
dependents against 5 at equal priority — **never a LUID tie at all**, settled one key earlier, and
LUID would have picked the other way.

### A wrong `extern` is invisible to every figure we carry

Two landings came from **one word**, where the tree's own `include/task.h` declares as `s32` what
the parks declared `void`. A value-returning callee intercepts the r0 chain through its `call_value`
set, which moves the dependent count — so **the escape was available all along and the declaration
hid it.** One park's 21-spelling failure list contains **three** such declarations and **every one
returns void**: the return type was the one axis never varied.

It hides because a wrong return type changes **size, count, opcode histogram, call multiset and
relocation sequence not at all** — the call is emitted either way.

**But it is NOT systematic, and I measured that before briefing it.** A sweep of all 19 parks
carrying that declaration found it **inert on 18** and one encoding **worse** on the nineteenth.
Two one-word landings do not imply nineteen.

### And the lever's direction was backwards in our own record

The doc framed the dependent-count escape as something *your* insn gains from a value-returning
callee. Measured, it is the opposite operation: a void call does **not** intercept an output
dependence on r0, so the move is to **shorten the COMPETITOR's list** by making a callee
value-returning. Reusable form: **an argument-register setter's dependent count includes every
later WRITE of that hard register in the same block**, and a call's **CLOBBER never becomes a last
setter** — only a SET does. So a callee's declared return type is a real codegen lever on r0
chains, free whenever the value is unused.

## Other mechanisms

**The `PLUS` operand order sets the destination register.** `*thumb_addsi3` emits `add %0,%1,%2`, so
rd follows operand 1, which follows the PLUS's canonical order: `arr + base` gives rn = r7,
`base * 2 + (int)arr` gives rn = r3 — the ROM. The park blamed reload's spill rotation; **reload was
never choosing freely, because `rd == rn` in both streams.** Seven recorded inert spellings all kept
the pointer on the left, and `base + arr` only *looks* like the flip — C pointer arithmetic and
`fold` normalise it straight back. **The flip has to happen in integer space.**

**A blocker can live one pass earlier than the park says.** `global_alloc` was blamed for an
allocation **local-alloc** had already decided: a halfword live across `__udivsi3` is single-block,
so local-alloc owns it, and the call-crossing exclusion leaves only r4–r7 with r7 taken as the frame
pointer. It grabs r5, and the quantities needing r5/r6 inherit the conflict. **Read the conflict
SETS, not just the allocation order** — the order was recorded correctly and the reason was still
wrong.

**The cheapest tell for a bad attribution:** one park's blocker *line* said "pre-reload scheduling"
while its own analysis, further down the same file, worked post-reload. **A park whose headline
contradicts its own body is self-refuting**, and checking that costs nothing.

## The interacting-pins law — the key result for pass THREE

Batch 312's pin pass reported a "fixpoint" at 233 → 157 using cumulative greedy, caveated only that
a *pair* might be jointly **removable**. The gap runs the other way too, and that direction is worse:

> **A PAIR OF PIN SITES CAN BE JOINTLY LOAD-BEARING WHILE EACH IS INDIVIDUALLY INERT.**

20 of 34 sites individually inert; dropping all 20 cost **65 encodings**; bisection isolated it to a
**single pair** — each free alone (4), **together 65**, every subset containing both at 65 and every
subset missing either at 4.

**The mechanism makes it predictable:** those two were **the only two sites materialising the same
constant**. cse1 unifies two pseudos holding the same `CONST_INT`, the survivor crosses calls and
takes a callee-saved register — so **a pin defeats the unification only in company**, because
unification needs **two unpinned peers**. Control: break the shared value and 65 collapses to 5. A
**second independent instance** was found the same way (inert at 32 of 34 sites, one pair costing 15,
again the only two sharing a constant).

**Why greedy cannot see it:** offered one of such a pair first it **accepts** it, **rejects** the
second, and reports a fixpoint **one pin short** with no sign the two were related.

**The guard:** group pin sites **by the value they materialise** and remove each group as a unit.
`tools/pinmin.py` now carries it, and the landed function's header is qualified in place — **157 is
a lower bound, not a minimum.**

**Delivered early:** `OvlFunc_887_2008578` goes **93 pins → 45** with no loss of figure, 19 shim
blocks replaced by ordinary C.

## An objcmp "encoding" can be a POOL WORD

A seventh instrument finding, and it **inflates** figures rather than hiding defects. Two parks were
overstated this way: one tracked as *4 of 261* is **2 of 261** once the reference side's missing
`_AREA_` spelling is supplied, which also caused an undocumented relocation difference.

**Every lower rung is blind to it** — size, instruction count, opcode histogram and call multiset all
miss it, because a pool word is not an instruction. When a figure will not shrink, check whether some
of it is **pool content rather than code**.

## Two parks closed by measurement rather than left hopeful

A park that still says "maybe the counts can be equalised" costs a future batch a brief.

- One has sched2 **inert by measurement** (`-fno-schedule-insns2` gives the same order) **and unable
  ever to help**: all three preheader moves have consumers inside the next loop, so each has zero
  dependents and priority 1, and `rank_for_schedule` falls to LUID = emission order. There is no tie
  to win. Corroborated by the clean half of the same function.
- The other is closed **at a compile-time constant**: `SMALL_REGISTER_CLASSES` is `TARGET_THUMB` and
  `*reg_parm_seen` is set at `i == 0` before any cost test, so the gate's first clause is **always
  true** — which is also **why** that park's `-fno-expensive-optimizations` probe was byte-identical
  (the flag can only reach the short-circuited clause). An inert flag with a cause is worth more than
  an inert flag in a list.

## Bounds and negatives

- **A lever can measure inert only because a prerequisite lever is missing.** One spelling was
  byte-identical alone and worth two encodings once another landed — which is why its park filed it
  as inert, *honestly*. **An inert entry is conditional on the body it was measured in; re-run the
  list against a new baseline.**
- **A false improvement, rejected:** a variant measured **63 against the 66 kept** and reads a
  pointer **once where the ROM reads it twice**, with its instruction count falling below the
  reference. **A better figure obtained by doing less work than the ROM is a wrong program.**
- **`volatile` is not a scheduling barrier for memory** in this gcc — a store with `MEM_VOLATILE_P`
  set took no dependence on an earlier store in a non-conflicting alias set, because **the alias
  check runs first**. Three placements, all inert.
- A **`long long` return** does intercept an r1 chain, but the declaration is **TU-wide**: it broke
  two other sites and cost 402 encodings on a 24-site callee.
- **A remaining residue, priced rather than asserted:** the frame pointer can reach the ROM's
  register only if another quantity is allocated first, and on gcc's own
  `floor_log2(n_refs)*n_refs/live_length` it outranks that quantity **threefold** — closing it needs
  five references where the ROM reads two.
- **`-fno-if-conversion` and `-fno-cprop-registers` do not exist in this cc1.**
- **gcse attributions are now 0 for 3 on their own flag test** — one was cse1, one cse2, and one
  rests on dumps alone.

## My own errors

- **My pin counts were wrong on three of four parks** in brief E: I used an ad-hoc regex and
  reported 7/15/9/4 where `shimcount.py` says **93/15/6/2**. The regex misses every
  **macro-expanded** PIN site, so the heaviest-pinned park in the batch read as the lightest —
  understated by a factor of thirteen. **`shimcount.py` is the authority for pins the way `objcmp.py`
  is for figures.**
- **The dependent-count lever's direction** was recorded backwards in the doc, and I repeated it in
  two briefs.
- **A harness trap I should add to the discipline list:** BSD `sed` does not expand `\t` in a
  replacement, so a sweep applied **not one substitution** and reported all four spellings inert at
  the baseline. Nothing failed — four plausible measurements of a file never edited. **Every
  generator must assert its edit landed**, because an unapplied edit measures as a clean inert result
  and is indistinguishable from a real bound.

## Housekeeping

- **`parkcheck.py` gains `ALTVERIFY`**: a nested-function whole-TU candidate is unscorable by objcmp
  *by construction* (nested functions emit as local symbols) and carries its own recipe. That was
  sitting in `UNCHECKABLE`, the bucket reserved for figures that can never be re-measured, and a
  permanent false entry there would mask the real ones. Third such split, after `NOFIGURE` and
  `NOBODY`.
- **A park with a figure and no recipe** was found by the sweep that could not test it; its claim of
  44 measures **53**, a figure kept from a body that was edited afterwards.
- **The `.L4`/`.L5` owner decision is settled**: no rename needed. An alias **adds** a name, so `.L5`
  keeps working for the still-asm sibling. What looked fatal — 26 to 44 objects per overlay defining
  `.L5:` — is harmless, because those are assembler locals that `as` discards; **exactly one file
  tree-wide carries `.global .L5`**, and the park named the wrong one.
- **`tools/sweep_variants.py`** installed: 35 variants in ~3 seconds in one container, which is what
  made 218 measurements and two bisections affordable. Pass three's group sweeps need it.

## Next

- **`2009fa4` at 1 of 199** is the closest park in the tree — residue is reload rematerialisation.
- The **nested-function TU** is a clean three-function landing (no split, **zero pins**) the moment
  one allocation fact yields; one of its three is already **81 of 81 exact**.
- **Re-read the four parks** resting on "the only escape is r0 interception" with the MEM case in mind.
- **Sweep parks for a headline that contradicts their own body** — self-refuting attributions are free
  to find.
