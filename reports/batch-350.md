# Batch 350 — one landing, and three findings worth more

**5 briefs, 18 targets, 1 landing, 2 parks improved.** Park files **613 → 612**.

## The landing: a one-word edit

**`Anim_CriticalHit` 2 of 707 → 0** — `ClearFn`'s typedef returns **`int`**
instead of `void`. Isolated properly: the typedef alone is byte-identical, and
changing only the *callee's* declaration is bit-identical to the old body, so the
lever is the function **pointer's** return type.

**A void call carries a trailing `(use (const_int 0))`; a non-void call does
not.** Typing the pointer `int` drops those USEs and re-shapes the block's
dependence graph, satisfying a dependent-count bound
(`haifa-sched.c:4097-4110`) **without moving a statement.**

The **same asymmetry answered a different park's only open question**: a six-insn
tail that should cross-jump doesn't, because the else arm keeps its call's USE
and **neither `jump.c:1445` nor `:1448` skips a USE** — `:1468` breaks on
CALL-against-INSN at the first iteration. That park's two routes became one.

It connects to batch 344's `flow.c:645-665` finding: the same RTL object reached
two ways, from the call's *type* or the block's *shape*.

**The landed-twin instruction is what paid.** Both piece-mates were parked, so
there was no twin in the piece — but grepping the *landed* half of the bank for
the callee found three files, one stating this rule in its own header. **The
answer had been two files away for four batches**, and the park had never varied
that axis.

## A NEW CLASS: the stale POSITIVE

`FieldMove_NoTarget` improved 7 → 6, and the lever was **not** in its negative
list. It was a **positive that batch 340 measured, wrote "NOT installed" beside,
and told the next reader to start from.** Five batches passed.

**A stale negative wastes a search. A stale positive wastes the answer** — and
unlike a negative it is findable mechanically, because the park's own prose names
a smaller number than its own claim line.

`tools/unrealised.py` scans for it:

```
113 parks mention a smaller figure than they claim
 58 remain after excluding pin / device / flag / twin / instrument / not-a-distance
```

Those 58 are a read-first list. The tool prints the surrounding line and its
docstring is explicit that it ranks candidates for **reading**, not bugs — four
legitimate reasons for an unshipped smaller figure are enumerated there.

## A REAL CORRECTNESS BUG NO FIGURE COULD SEE

`i * 0xffff4000` — that constant is 4,294,918,144, **above `INT_MAX`**, so C makes
the expression `unsigned` and the body called **`__udivsi3` where the ROM calls
`__divsi3`**. It sat in the park's own printed relocation rows for **three
passes**, because `objcmp`'s encoding count cannot see it: the `bl` placeholder is
byte-identical either way. Fixed as `i * -0xc000`.

A defect class our primary instrument is blind to by construction.

## The switch-case-run mechanism transferred

All four `cmp #0xb` sites were verified **backward** before any variant was
written, so nothing was spent on the range-test shape.

- **`Field_Catch`** — transferred: both exits `cmp r7,#10 / ble` → `cmp r7,#11 /
  blt`, aligned 120 → 116.
- **`Field_Douse`** — **already matched**, from its own named bound. Third
  confirmation of the `Field_Halt` rule: the named bound reaches the immediate
  only when r11 is already taken, and this ROM saves r5–r11.

**Generalisation for the remaining three sites:** check `Hard regs used` in
`.18.greg` first — r11 free means only the case run works; r11 taken means either
route works and **they are bit-identical**.

## Censuses — fifteen parks, 318 rows → 90 distinct programs

| brief | rows rebuilt | distinct | measured nothing |
|---|---|---|---|
| A | 105 | 20 | 83% |
| B | 85 | 31 | 64% |
| C | 46 | 18 | — |
| D | 82 | 21 | 74% |

One park's "thirty-four spellings are now inert" is **one program**. Another's
thirteen rows are one program — terminal device-free. A third's 13 → 10 is what a
genuinely crossed list looks like, which is the useful contrast.

**And the reading rule for that rate**, reached independently by brief A and by
the coordinator on the anchor axis — both wrong first: **a 100% bit-identical
rate is a CLOSE, not an opening.** Near 0% means the negative list is
trustworthy; **moderate is where batch 349's three landings came from**; near 100%
means gcc's canonicalisation dominates that construct and no spelling will reach
the back end — change axis, not spelling.

**In the parks' defence:** every figure these parks ever wrote down reproduced
exactly on the rows that are genuinely distinct programs. The headers are honest.
They had no instrument that could tell a negative from a non-measurement until
batch 348.

## Mechanism closes

- **An argument move outranks any preceding store by exactly one**, for every
  non-const call in gcc-2.96: `arm.c:2429-2431` gives the +1, the store is linked
  `REG_DEP_ANTI` unconditionally (`haifa-sched.c:3230-3231` from `:3954`), and
  `arm_adjust_cost` zeroes anti/output (`arm.c:2425-2427`). `rank_for_schedule`
  returns at its **first** rung and never reaches the class test — which would
  have tied anyway.
- **local-alloc cannot suggest a register a copy never touched** —
  `find_free_reg`'s suggested phase (`local-alloc.c:2012-2018`) only reaches
  `qty_phys_copy_sugg`/`qty_phys_sugg`, which `combine_regs` fills **only** from a
  copy with a hard register on one side (`:1674-1708`).
- **A rematerialised constant is not a pool load** —
  `THUMB_LEGITIMATE_CONSTANT_P` (`arm.h:1807-1809`) admits any `CONST_INT` and the
  set matches `*thumb_movsi_insn`'s `K` alternative (`arm.md:3838-3841`), output
  `#`, so it splits to `mov`+`lsl`.
- **No zero-instruction construct can add a dependence edge** — a volatile asm
  takes `haifa-sched.c:3567-3597`'s total-order-anchor branch (and an asm with no
  outputs is implicitly volatile); a non-volatile `ASM_OPERANDS` is deleted
  before the back end.
- **`r2` can be local-alloc's copy suggestion rather than pressure** — on one
  park the reference has **zero branches**, so it is one basic block and there was
  never any pressure to reason about.
- **A park's central claim expired**: "declare the angle `unsigned short`" is
  bit-identical, because the value arrives zero-extended from the `ldrh` and the
  mode never reaches the allocator.
- **Another park's central question was already its state**: batch 337 closed it
  with "one question: a second biv on pass 1", and `.08.loop` shows that *is* the
  state. The gate was misread too — `check_dbra_loop`'s first disjunct requires
  `! loop_info->has_call` and the loop calls `GetUnit`, so the term sits in a dead
  disjunct.

## A LOWER FIGURE ON THE WRONG CODEGEN SHAPE IS NOT PROGRESS

Brief C declined a body at **4 of 39** over an installed **5**: it emits
`cmp/bgt/cmp/blt` where the ROM has `mov/sub/cmp/bhi`, buying one encoding by
trading away the one property the installed body shares with the reference. Right
call, now a standing rule; the candidate is kept because its r3 placement is real
and may compose. (And all five literal range-test spellings are bit-identical to
it, because `fold`'s `build_range_check` lowers them to exactly that body.)

**A fourth false-improvement shape** joined the list: `w = h = 8;` reads 2 against
a park at 3 and is a regression — it is `w = (h = 8)`, so `h` stores first,
writing the pair in the opposite order from the ROM. The four shapes are now a
phase artifact across a length change, a wrong program that compiles clean, the
wrong codegen shape, and a reversed store order.

## Corrections to the coordinator's own work

- **Route 3 needed two qualifications I recorded with neither**: it needs a **back
  edge** (no loop ⇒ the dead store is deleted; nine of ten spellings
  bit-identical) and **the two definitions must carry different values** (a
  duplicate of the same constant is cse'd back).
- **The fingerprint added in batch 348 was missing object size**, so a `static
  const` variant whose code stream matched while the object grew four bytes read
  as BIT-IDENTICAL — two objects called one program, inside the check built to
  prevent that.
- **`piece.py`'s `resolve_s` read only the first 9000 bytes**, reporting "could
  not resolve a reference .s that exists on disk" for a reference that exists and
  holds the function. Three parks affected; seventh narrow-window defect in this
  project's own instruments.
- **I told a brief `STRENGTH_CFLAGS` did not exist.** It is at `Makefile:855` with
  two consumers, so the 221-of-259 true-distance body that brief found needs no
  new flag group and no owner decision — a third explicit rule is the same
  practice as batch 346's `CSE_CFLAGS` rule. That park moved from "blocked on the
  owner" to "blocked on 221 encodings".

## Open

- **Two parks are at a verified 0 behind the same device question**:
  `Func_80a96d8` (ladder 8 / 2 / 0, needs a fifth `::: "memory"` fakematch row)
  and `Func_801cc50` (byte-identical candidate in `scratch_elev/b338/E/` since
  batch 338, one bare `__asm__ volatile ("")` from the installed body).
- The 58-park unrealised-figure read-first list.
- Three remaining backward hand-written `cmp #K / blt` sites for the case-run
  mechanism.
- Owner decisions unchanged: `_CONST_0`, the `__divsi3` alias, `:: "i"(0)`, the 23
  undefined-extern address parks.
