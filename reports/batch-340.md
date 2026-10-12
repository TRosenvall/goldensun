# Batch 340

Five briefs, 20 targets, **6 landings**. **648 → 642 park files**; census ROM
functions landed 4,972 → **4,978**.

Landings: `OvlFunc_946_2009c84` and `OvlFunc_946_200a16c` (A),
`OvlFunc_897_200a84c` (B), `Func_8078aa0` (C), `OvlFunc_931_20086f0` (D),
`Func_80a33d4` (E).

## To win an allocation race, take the value out of it

Brief A landed **both** surviving members of the four-function callee-saved coin
flip on one token:

```c
a -= 1;        /* parked */
d = a - 1;     /* lands */
```

Same `sub r5,#1` emitted either way. What changes is the **allocno set**: the
long-lived value drops to two references, **dies at the new pseudo's definition
so it stops conflicting with it**, and both end up in r5; the short dense pseudo
sorts first in `allocno_compare` (`global.c:598-620`) and takes r5, leaving the
other long-lived value uncontested on r6.

> **The lever is not "shorten the range so it wins the sort" — it is "add a
> definition so the range SPLITS and leaves the race."**

On both parks shortening was arithmetically unavailable: `200a16c`'s verdict
proves its carrier needs `live_length < 27` against an actual 50, and **that
verdict is correct for the question it asked.** The question was wrong.

Three distinct, non-interchangeable routes into `allocno_compare` are now known:

| route | what it does | found |
|---|---|---|
| delete/split a reused local | makes a value **block-local**, so `local-alloc` takes the register first | 339 brief B |
| move a definition before its first use | shortens `live_length` directly | 339 brief D |
| **add a definition so the range splits** | takes the value **out of the contest** | 340 brief A |

Brief A tested the first two here first: the no-local route is **inert at
identical allocno priorities**, and shortening is unavailable.

## The other landings

- **`OvlFunc_897_200a84c`** — its park said "nothing in the source reaches the
  allocator's priority order". Right observation, **wrong allocator**: the
  carrier is referenced in one basic block, so `local-alloc` takes r5 before
  `global-alloc` ranks anything, and all three declaration orders it screened
  were arguing with `global.c`.
- **`Func_8078aa0`** — residue reading and `allocno_compare` citation both right,
  but with `n_refs` and `size` equal **that sort reduces to live length alone**,
  and the park spent all 28 of its bodies on the `n_refs` term. The edit is
  deleting one initialiser so the accumulator stops living across the guard.
- **`OvlFunc_931_20086f0`** — four measured steps: `-fno-strict-aliasing`
  (17 → 12, from autosweep's flag axis), split the reused `s` (→ 11), split the
  reused `v` (→ 3), name `0xe666` (→ 0). The two splits are a **crossed** result:
  `v` alone inert, `s` alone worth one, together worth nine. Needs an
  `ALIAS_CFLAGS` rule — added explicitly, third file in the tree to take it.
- **`Func_80a33d4`** — the park said "naming it makes it worse". One local
  reassigned per loop *is* worse at 42; **three distinct locals are 0**. The park
  had recorded both rows as the same figure.

## Parks moved

`AnimEnd` **14 → 2** on a device-free anchor inside the static inline,
immediately *after* `queue = &gDMATaskCount;`. Its park proves a pool load cannot
win a sched2 priority contest and **that proof is correct** — the anchor does not
raise priority, it **empties the ready list**. Placement is not interchangeable:
before the load 10, after 2, both sides 5. Brief E noted the correction was never
applied to the pool-load class, which became brief A of batch 341.

Also `OvlFunc_943_2009684` 16 → 3, `OvlFunc_965_200a660` 19 → 4,
`OvlFunc_924_200b6ac` 16 → 4, `Func_8094154` 16 → 8, `OvlFunc_964_20090c4`
16 → 12.

## Three more autosweep defects, all false-clean-negatives, all found by briefs

1. **`resolve()` took the first function the park file defines**, not its subject
   — so a park with `static inline` helpers above its subject **swept the wrong
   function**. `AnimEnd` swept as `SetRegAnimDest` and reported "BASE DOES NOT
   MEASURE": a clean-looking "too far gone" on a target where the anchor axis was
   worth 12. Now prefers the header subject, validated against the reference's own
   `.thumb_func_start` list. **61 parks have a first-definition that is not their
   subject**, so every pre-fix sweep of those was about a different function.
2. **The relocation warning added in 339 is false for absolute `.sym` symbols.**
   Brief D found three of four such banners wrong: `_CONST_2` (`const.sym:53`) and
   `_AREA_36` (`area.sym:71`) are absolute symbols in files `stage1.ld` INCLUDEs,
   so an `R_ARM_ABS32` against them links to the literal — and landed
   byte-identical files spell the same pooled constants the same way. Reworded to
   a check with the test to apply. Related: an extra `R_ARM_ABS32` pool word costs
   one encoding and an extra `R_ARM_THM_CALL` costs **zero**.
3. A winning anchor row printed only `anchorNN`, so brief D had to import
   `boundaries()` to recover the placement. It now prints the source text it
   follows.

## Two bounds of mine corrected by briefs

- The shared brief's lever 3 says the class rung "is a first-class lever wherever
  the contested window holds a load". Brief D's window holds a load and **that
  rung is never reached**, because `INSN_PRIORITY` (`haifa-sched.c:4040-4043`)
  already separates the load from the `mov`s by one — `arm.md:254-294` gives
  `load` latency 2.
- **"N distinct locals beat gcse" does not generalise.** It works for a repeated
  *constant* and fails for a repeated *expression over a live base* (62 of 142,
  8 bytes shorter) — indistinguishable in source, opposite in outcome.

Also booked: **an autosweep anchor win is a hypothesis, not a result** — brief E's
`anchor03` was 16 → 15 on the park and **11 → 13** after the statement split.

## A free tell worth having

`thumb_exit` (`arm.c:8288-8319`) proves a **`void`** Thumb function cannot emit
`pop {r1} / bx r1` — `{r0,r1,r2}` leaves r0, `{r1,r2}` leaves r1. So that
epilogue shape is itself evidence about the return type, readable off any
reference `.s`. A park declaring `void` against it was wrong; returning the inner
call's result took it 19 → 17.

## Open

- **`__asm__ volatile ("" :: "i"(0))` is booked as an owner question.** It is the
  only asm form that flushes `cse`'s table (`cse.c:5741-5745`) and takes a park
  17 → 8 where the bare and `::: "memory"` forms reach 16. The asymmetry is the
  question: the bare barrier is in **34 landed files**; this form is in **13, all
  of them parks**.
- `Func_80270ac` should be scheduled as a **parent-function job** with
  `rom_15000/8022a7c.c`, which three briefs have now independently concluded.
- `OvlFunc_932_2009770` is named by `20086f0`'s park as covered by the same
  four-step route — untried.
