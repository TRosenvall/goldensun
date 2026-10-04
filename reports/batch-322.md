# Batch 322 — nine agents, grouped by bank

**4,839 / 5,710 (84.8%)**, 871 remaining, 779 parked, 0 available.

Six landings, seventeen parks improved or corrected. The grouping changed this
batch — by **bank** rather than by blocker keyword — and that change is the
batch's most reusable result.

## Landings

| function | from | mechanism |
|---|---|---|
| `SetDjinni` | 14 of 51 | **basic-block order** — 11 of the 14 |
| `Func_8077348` | 12 of 34 | one statement moved; a **0.8% priority margin** |
| `Func_8019908` | 9 of 27 | priority at a **1.2% margin**, from its twin's header |
| `Func_8099070` | 7 of 44 | — |
| `Func_8096b88` | 7 of 48 | two variables and **one** read, not two reads |
| `Func_8091254` | 7 of 29 | **`reload_cse_move2add`**, a pass never named here before |

All six pin-free. Also improved: `Func_80a1090` 16 → 7 (count, size and
relocations exact), `Func_801f730` 7 → 3, `Func_807808c` 5 → 4,
`Func_801fd34` 14 → 10 with relocations now clean, `Func_80216b4` 14 → 12.

## WHY BANK-GROUPING WORKED, AND WHERE IT STOPS

Batch 321 grouped by blocker keyword and two briefs refuted their own bucket
label. This batch grouped by bank so each agent had solved neighbours to read.
**Three of the six landings came from a landed sibling rather than from a park:**

- `Func_8019908`'s lever came from its **twin** `ClearCallbackTable`, same table
  pair at `+0x12bc`/`+0x12dc`, whose header handed over `REG_ALLOC_ORDER` and the
  local-before-global ordering. The twin's own two levers were measured **not
  needed** here — so what transferred was the **reasoning, not the edit**.
- `Func_8096b88`'s first step came from a corpus scan (denominator printed: 4,418
  generated `.s` with a sibling `.c`, 341 in that bank) finding eleven files with
  the ROM's register-reuse shape, three at the same `0x27` offset.

> **A landed header records what the TU actually wanted. A park records what
> somebody could not make work.**

And the boundary, measured: `Field_Whirlwind`'s levers do **not** transfer to
`Func_8096ddc` — its lever 4 hoists every `int` carrier (141-148 insns) and its
lever 5 is **exactly inert in all three positions**. The transfer is a hypothesis
to test, not a rule.

## FIVE INDEPENDENT READS OVERTURNED A CORRECTION FROM LAST BATCH

Batch 321 reported, and the coordinator propagated into `docs/elevation.md` **and
into nine agents' briefs**, that `local-alloc.c`'s `qty_compare` has no
`floor_log2`. **It does.** `local-alloc.c:1496` and `global.c:607` both call it,
and `local-alloc.c:1483-1488` says the sameness is deliberate.

Five briefs read the source independently and all five agreed. Two of them then
supplied what was missing:

- **The real difference is the denominator** — `death - birth` against
  `live_length`. And `death - birth` is **slot numbers at two per insn**
  (`local-alloc.c:1415`), so it is `2 × span ± 1`. The factor of two cancels in an
  ordering; **the ±1 parity term does not, and it is a sub-percent perturbation —
  exactly the size of margin these parks turn on.** This batch closed landings at
  0.8%, 1.2% and 0.87%.
  > **So compute local-alloc priorities as a RANKING and never quote the number.**
  > And verify your priorities reproduce `.18.greg`'s allocno order *in sequence*
  > before believing them — that check caught a park pricing an allocno at 6 refs
  > when `.17.lreg` says 8.
- **Batch 321's symptom finally has a mechanism.** Its observed "1.3% margin
  turned into an apparent factor of three" comes from substituting
  `REG_LIVE_LENGTH` for an in-block `death - birth`, which can differ by a large
  factor. **Dropping `floor_log2` could never have produced it** — that could only
  move a ratio by the ratio of two small integers.

> **When a correction explains an observation, check that the mechanism it names
> could produce the magnitude observed.** Batch 321's could not, and one read of
> one source file would have shown it.

## THE SOURCE SETTLED THREE DISPUTES IN ONE BATCH

`~/gs_project/camelot-gcc/gcc-2.96/gcc/` — found in batch 321, and this document
cites its file:line in a dozen places without ever giving the path.

1. The `floor_log2` question above.
2. **The sched2 dump.** One brief reported "there is no `.NN.sched2` dump;
   `-fsched-verbose=6` writes to stderr"; another reported the opposite.
   `haifa-sched.c:6846` is
   `dump = ((sched_verbose_param >= 10 || !dump_file) ? stderr : dump_file)` —
   **the threshold is ten.** Measured at N = 2, 5, 9, 10: the detail is in
   `.23.sched2` through 9 and gone at 10. So `-da -fsched-verbose=6` is correct and
   lands in the file.
3. **`reload_cse_move2add`** (`reload1.c:8840`), which no park had named. A park
   blamed constant CSE; `rtlanal.c:get_related_value` returns 0 for anything that
   is not a `CONST`, so **cse can never derive 0x2a02 from 0x2a01**.

## A FREE INSTRUMENT WE HAD NEVER USED

`push_minipool_fix` (`arm.c:5380`) prints **every pool fix** into the plain `-da`
dump `<base>.c.26.mach` — per pool word its **MODE**, the **referencing insn's
address** and its **`pool_range`** — emitted in ascending `addr + range` order
(`arm.c:4820`).

> **One compile replaces reverse-engineering pool order out of the `.s`.** Any
> pool-order park should start here. A brief last batch verified pool order out of
> `baserom.gba` by hand.

With it, the HImode pool mechanism is now complete: `*thumb_movhi_insn`
alternative 1 takes `mn`, so a HImode constant matches the **load** alternative at
`pool_range` **64** against `movsi`'s **1020** — hence a HImode fix sorts ahead of
every SImode one. `ldrh rN,.LC` **is not an `ldrh` in the object** (gas emits the
word form: `4b04 ldr r3,[pc,#16]`), and a signed short truncation of `0x8000`
dumps as `.word 0xffff8000`.

## New levers

- **Tail cross-jumping runs in jump2, after sched2** — but it does **not** fire on
  arms that do not end in calls (eight duplication variants, every one +4 bytes).
- **Manufacturing a hard-register conflict to delete a preference.** When an
  allocno wrongly takes an *argument* register, name the argument expression and
  assign it before the last other use of its input. **Two of three placements are
  exactly inert**, so it is invisible one-at-a-time.
- **Moving an `INSN_LUID`.** When all four rungs tie, RTL order decides — and
  `loop.c` emits every loop-invariant hoist **last** in the preheader, so a hoist
  always loses. Found independently by two briefs.
- **A constant apparently hoisted above the previous statement inside an `if` arm
  is evidence about which basic block that statement is in**, not about constant
  motion. Worth 11 of 14 on `SetDjinni`.
- **The ROM's countdown loop is bit-identical from an up-counting `for`** —
  `check_dbra_loop` reverses it, so loop form is one equivalence class.
- **To re-rank two call-crossing allocnos, move an initialisation across the call
  that births the rival** — it shortens the rival's live range by one insn.

## New bounds

- **A register pin is not a free diagnostic.** Pinning a call-crossing local
  changes the **prologue push set** and therefore the instruction count (106 of
  122 at 116 insns). Check the count before reading the figure.
- **A second name cannot change `REG_N_REFS`** — copy propagation rewrites the
  uses back; `.17.lreg` inputs bit-identical across three copy edits.
- **local-alloc's `fake_birth`/`fake_death` false-dependency avoidance never runs
  here** — gated `!SMALL_REGISTER_CLASSES`, and `arm.h:1061` defines that as
  `TARGET_THUMB`. Found independently by two briefs.
- **`local-alloc.c:360-366` short-circuits on `REG_N_DEATHS == 1`**, so where a
  pseudo dies twice, the gate one park blamed is never evaluated.
- **`DIFFERENT_ALIAS_SETS_P` can never fire against a reload spill slot**, and the
  alias lever **cannot reach a sched2 tie settled by LUID**.
- **A `volatile` MEM adds no sched2 dependence the alias set does not already
  give** — six variants bit-identical with the edit verified applied.
- **`qty_compare_1` breaks a priority tie by QTY NUMBER**, and qty numbers follow
  first use in the insn stream — so the declaration lever cannot move such a tie
  (all 24 permutations identical).

## Two of the coordinator's own artifacts corrected

- **The batch-319 backfill stamped 178 parks with "RELOCATIONS ALSO DIFFER — this
  figure is NOT a distance."** Re-measured: **137 of 176 are the SAME symbols at a
  shifted offset**, which this project treats as a consequence of a length
  difference, not a blocker. Only 38 are genuinely different symbols. So ~17% of
  the parked frontier was labelled unreachable when it was not. Each banner now
  names which case it is, and the real ones list the differing symbols — several
  of which (`__umodsi3` vs `_umodsi3_RAM`, a missing `_AREA_3c`) are
  **symbol-table** questions rather than codegen ones.
- **`crossfire.py`'s MEM screen fired on every row including BASE**, because it
  grepped the candidate's `.s` while objdumping the reference — and gcc spells a
  HImode pool reference `ldrh r5, .L20`, which assembles to a plain `ldr`. The
  identical-encoding trap from our own method notes, built into the tool written to
  enforce it. Fixed; agents confirmed BASE clean afterwards.
- **`install_batch.py` refused a correct batch after its own splits phase**,
  because the manifest describes the pre-split tree. A phase tool has to know
  which phase it is in. Fixed.

> **A diagnostic that is true but undiscriminating is worse than no diagnostic,
> because it is believed.** Both tool corrections were signals that fired
> correctly and meant two different things.

## Two parks whose strongest claims were refuted by counting

- One concluded **"the toolchain differed"**, resting on "zero mixed
  symbol+constant pools" (there are **1,540 mixed runs** across 4,418 files) and
  "nothing in the source controls pool entry order" (the HImode mechanism above
  contradicts it directly). *The strongest claim a park can make needs the
  strongest evidence; this one rested on two uncounted assertions.*
- Another's park body **was a wrong program** — 7 with a MEM flag, `ldrsb` where
  the ROM has `ldrb`. Its own header said "residue is now 3" and named a scratch
  file **that was never installed**. Now installed at 3.

## Devices caught

- A **ternary** reading 4 of 30 with the pool order correct **is a wrong
  program** — it stores twice. At four encodings from zero that looks like the
  answer; the memory screen is what catches it.
- A **volatile cast on one of three reads of the same lvalue** is a device, not
  the documented lever. Its body was better *only with* the device and **worse
  than the existing park** without it, so the existing body was kept and the 2 was
  recorded as a figure about the blocker.
  > **A volatile cast applied to one of several reads of the same lvalue is a
  > device. Applied consistently to an access whose width or ordering is a genuine
  > property of the data, it is a lever.**

## Process

Brief F died to an API timeout at "now let me write the deliverables", but had
checkpointed — all three candidates and a 7KB `FINDINGS.md` survived and only the
manifest was lost. That instruction has now paid off in batches 314, 316 and 322.

The pin policy worked as intended: `Func_80979a4` reads **0 of 47** with one pin
and was **reported as a park**, because it would be a fourth pin on a veneer that
already carries three.

## OPEN — owner decisions

Four now, two carried from batch 321. See the end of `reports/batch-321.md` for
the first two.

1. **`_MSG_b24`** — a symbol-table entry that would land `Func_80a9a5c`.
   *Sufficient but not necessary*: a literal `0xb24` can reach the pool and the
   ROM's pool word carries no relocation, so the bytes do not single out a symbol.
2. **`Field_Halt`** — byte-identical under a per-file `-ffixed-r11`, or **1 of
   191** flag-free. Its landed twin is in the same bank and uses fp freely.
3. **`Func_80979a4`** — **0 of 47** with a fourth pin on a three-pin veneer, held
   under the prefer-pin-free policy.
4. **`Func_8078870`** — 2 of 40 with a volatile cast classed as a device, against
   5 device-free. Recorded as a figure about the blocker, not shipped.
