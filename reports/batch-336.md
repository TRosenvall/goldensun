# Batch 336

Eight briefs, 32 targets, **6 landings**. **689 → 682 park files.**

> **A note on the count, because the HANDOFF column has drifted.** The figure
> above is `tools/directory.py`'s `park FILES`, stated as such. The batch-335
> row records "675 parked", but at that batch's own publish commit (`aa135a58`)
> `directory.py` reported **689** park files — so the column's earlier numbers
> are on some other basis and are not comparable to this one. Rows from 336
> onward say "park files" explicitly. The derived counters that *are* consistent
> are `census ROM functions landed` (4,943 → **4,949**, +6, matching the six
> landings) and `definitions in landed sources` (5,069 → **5,073**).

Landings: `Field_Move` (E), `Func_80ab21c` (B), `OvlFunc_957_2008b30` +
`OvlFunc_957_2008bc8` as one TU (A), `Func_8011fd8` (G), `Func_80f0254` (C).

Targeting was the length-gap criterion again, fifth batch running on it. But the
batch's real subject turned out to be the **readiness barrier from batch 335** —
three briefs reached for it, two landed with it, and three separate limits on it
were measured. It is now a documented device with a known reach rather than a
general-purpose fence.

## The barrier is a scheduling device, and only that

Three spellings that are interchangeable-looking C reach three different passes,
decided by the RTX code of the insn's **pattern**:

| spelling | pattern | scheduler | combine | cse |
|---|---|---|---|---|
| `__asm__ volatile ("")` | `ASM_INPUT` | **yes** | no | no |
| `__asm__ volatile ("" ::: "memory")` | `PARALLEL` | yes | **yes** | **no** |
| `__asm__ volatile ("" :: "i"(0))` | `ASM_OPERANDS` | yes | yes | **yes** |

`cse.c:5742-5745` tests `GET_CODE (PATTERN (insn)) == ASM_OPERANDS`, and a
memory clobber wraps the asm in a `PARALLEL`, which fails the test. **A memory
clobber does not flush cse.** This corrected a write-up made earlier in the same
batch on brief A's wording; brief G measured it and the source agrees.

Two more limits:

- **Not free in a loop body.** `Func_80ab21c`'s first barrier fixed the readiness
  contest and left the park at 90 of 101 — **94 instructions against 98**. The
  `.18.greg` conflict graphs are byte-identical; only the allocno *order* swaps,
  because `allocno_compare` (`global.c:598-620`) divides by `live_length` and a
  barrier that emits nothing still lengthens every range crossing it by one
  insn. The fix is a second barrier inside the inner loop, after which all four
  placements are byte-identical — **if placement stops mattering, the lever is
  balancing lengths, not ordering.**
- **Unreachable against a loop hoist, and it reverses.** `loop.c` emits every
  movable with `emit_insn_before (..., loop_start)`, so a hoist is always above
  the whole body. On `Func_80b6a60`: barrier before → inert at 2, barrier after
  → **7**.

And where it *does* work, position is everything and there is no shortcut:
`haifa-sched.c:3589-3595` makes an `ASM_INPUT` a dependent of **every register's
last set, `sp` included**, without naming anything. `Field_Move` is **0 with the
barrier after the load and 3 with it before**.

## Other mechanisms

- **A new ifcvt gate, booked as gate 3.** `dead_or_predicable` scans the merge
  block (`ifcvt.c:1911-1925`) and **any MEM in it makes if-conversion refuse**.
  `Func_80ae99c` went 18 → 10 by moving a halfword load into each arm. It
  matters because gate 1 is unreachable in Thumb: `BRANCH_COST` is **1** at -O2
  (`arm.h:2413`) while the then block is one RTL insn in every spelling, so
  `count_bb_insns > BRANCH_COST` never fires — which is why that park had
  plateaued six ways.
- **Shift the parameter in place.** `fx >>= 16;` took `Func_8011f54` from
  **59 → 33**, where `x = fx >> 16` and `x = fx; x >>= 16` had both been screened
  and correctly measured inert. A fresh variable's pseudo is a copy and gets
  propagated; the parameter's own pseudo keeps `assign_parms`' prologue save.
  Both parks varied *where* the shift happened and *what it was assigned to*, and
  never *whose pseudo it modified*.
- **The rung above dependent count.** `haifa-sched.c:4068-4095` takes the highest
  **class** before dependent count and LUID. It ties across this tree for a
  target reason — `arm_adjust_cost` returns 0 for anti/output and `arm.md:254`
  gives every `core_cycles single` insn latency 1 — so it is dead in an ALU
  window and a first-class lever wherever the contested window holds a **load,
  multiply or multi-register store**.
- **`cse1`'s widening-load rule.** `cse.c:5048-5092` collapses "a MEM already
  loaded with a widening operation", reachable because `extendhisi2`
  (`arm.md:3187-3236`) always forces a Thumb operand through
  `copy_to_mode_reg (HImode, …)`. Brief A's landing came from **deleting the
  carrier** so the re-read lives where there is no `zero_extend` to find, after
  which gcse (pass 07) hoists the load back into block 0 — which is what gives
  the ROM's `ldrh`-before-`ldrsh` order.
- **`PROMOTE_MODE` makes an HImode local SIGNED here.** `arm.h:597-606` widens
  with `UNSIGNEDP = TARGET_MMU_TRAPS != 0`, which is **zero**, so a park's
  `unsigned short h` was held sign-extended and its `(short)h` was a no-op. The
  CSE it diagnosed never happened to its own body.

## Corrections to my own briefing

1. **"Neither ships alone" — written three times, and wrong every time.**
   `split_s.py` cuts before/target/after, so **any** member of a piece can be cut
   out alone. Three briefs rejected it; brief F found it wrong a second way (its
   targets were members 4 and 7 of seven, needing three cuts, not one).
2. **Brief C believed it, which is the expensive direction.** It measured
   `Func_80f0254` to **0**, byte-identical, then wrote *"THIS IS NOT A LANDING
   AND MUST NOT BE COUNTED AS ONE … it CANNOT SHIP."* It ships: the first member
   cut out alone into a 47-line piece and landed, leaving its mate parked at 3.
   **A landing was nearly lost to a premise I supplied.**
3. **Root cause, and the fix.** I had grouped *shortlist* parks sharing a
   reference `.s` and then described the piece as though those were its only
   members; the real pieces held three and seven functions. `tools/piece.py` now
   prints full membership with each member's park state and claim, the brief
   template requires running it, and `--pairs` reports **86 pieces holding two or
   more parked functions, 75 wholly parked.**
4. **`tools/shimcount.py` counted one barrier form of three**, so any body
   shipping an operand form reported zero shims — which is how all four of brief
   G's bodies read as clean. Now counts the three separately. Tree-wide this
   surfaces 117 landed files with a bare `ASM_OPERANDS` form and 3 with the
   clobber form, all already booked via their register pins.

## Bookkeeping

- Two unbooked fakematch-class shims cleared (`OvlFunc_968_200c520`,
  `BaseAnim_Spasm`), both predating this batch. `unbooked fakematch-class` is 0.
- A stale duplicate park (`80e3a3c.c`, header-only, naming a pre-split `.s` and a
  vanished scratch file) retired — its one unique claim carried forward first:
  this function's coin flip is in the **scratch** set, not the callee-saved
  r5/r6 flip parked on four others, so a lever fixing those cannot reach it.
- `Func_8011f54`'s installed body had **missing braces**, making `src = gBuffer`
  dead — which is exactly its park's own unexplained "relocations differ in their
  symbols — gBuffer" line, standing for two batches directly beneath its cause.
- A damaged comment fixed: commit `6098e247`'s annotation port had overwritten a
  symbol name in `asm/rom_a1000/rom_ae88c_c_c_c.s:5` with the literal string
  `.gcc2_compiled.`, which made `grep -rl gcc2_compiled` misclassify a
  hand-written file as generated. Not recoverable from the body, so marked
  `<SYMBOL LOST>`.

## Two greps that read as clean negatives

- `grep -rl gcc2_compiled asm/` does **not** identify generated `.s` (see above).
- Matching pool loads as `ldr rX, =sym` finds **zero** generated hits: gcc emits
  `ldr rX, .L5`. Measured **16,146** label-form occurrences against not one `=`
  form in a generated file. `asm/` is a labelled corpus of **two dialects**, and
  an instruction-level sweep has to be written for the one it is aimed at.
- Third instance, from brief F: an escaping frame address has **three** encodings
  (`add rX,sp`, `mov rX,sp`, `adds rX,rY,sp`); matching only the first produced a
  false precedent and swung a hit list from 9 to 233.
- Fourth, from brief C: `objcmp --func NAME` on a multi-function candidate
  compares the **whole object** against one reference function and reported 124
  differing of 87 on nine good variants. **Use `--whole` for a merged TU.**

## Open for the owner

1. **Is a never-read local array that only reserves frame bytes a device, and is
   it admissible?** `Func_80175c0` closes at **0 of 44 in one line** with one.
   Brief F declined to ship it on an unbooked precedent; the precedent is larger
   than it thought — **22 landed, byte-matching files already carry one**, and
   while seven appear in `02-depin.md` it is for *other* pins, because that
   taxonomy has no category for a never-read local at all. Ruling it
   inadmissible leaves 22 landed files carrying an uncosted device. Booked in
   `docs/owner-decisions.md`.
2. **`Func_8011fd8`'s device choice.** It landed with the booked clobber-form
   barrier. An equivalent 0-of-45 body with no asm exists using a `volatile` cast
   on the single read — ruling 4's *device* test is about "one of several reads
   of the same lvalue", which this is not, but its *lever* branch needs the width
   or ordering to be a genuine property of the data, and the lvalue is ewram. So
   both options are devices; the barrier is consistent with 120 landed
   precedents. Reversible in one file.
3. Carried: `_MSG_741`, `_CONST_24`, `_CONST_200`.

## Open work

- `Func_80ae9f0` + `Func_80ae99c` both read **0 with one pin each** — a complete
  two-function TU whose only artifact is one register name repeated. Parked under
  the `Func_80979a4` precedent; strong pass-3 candidate.
- `LoadGS1CreditsBG` at **3** (2 with `_SIZE_80f0024` resolved), now the only
  thing between its piece and a second landing.
- `Func_80b2ed8` at 3 is a **`jump.c`** question, not a C one: `.23.sched2` shows
  the insns are in different blocks so `rank_for_schedule` never compares them,
  cross-jumping runs at `.25.jump2` *after* sched2, and the ROM's order is
  reachable in one block. What remains is a cross-jump merge that does not
  happen though `find_cross_jump`'s own test should accept it.
- `Func_8011f54`'s remaining 17 is one allocator decision: the ROM spends eight
  hard registers where max tail liveness is seven on both sides. Naming
  `ewram_202c001` early reaches **10** but reverses the pool order; naming the
  callee fixes the order but costs `_call_via_r3`.
- `tools/piece.py --pairs`: **86 pieces with two or more parked members, 75
  wholly parked** — an unworked targeting pool, with the caveat that the useful
  question is the **worst** member's figure.
- The one park at figure **0** (`Func_80b9554`) is correctly parked and is the
  real exception to "any member splits out alone": it and `Func_80b9604` are gcc
  **nested functions** whose static chains reach parent `Func_80b9724`'s frame
  slots, so they are one TU **by the language**, not by the split tool.
