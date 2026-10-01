# Batch 315 — five more landings, the sched2 ladder corrected, and the tree's two closest parks turn out to be one function

Six agents over the 24 parks within 12 encodings, grouped by distance band, with one brief
deliberately adversarial. Gate green at `5c4695205413df7db52b9a184815a07783999971` at every commit.

**5 LANDED** (plus 2 carried over from batch 314's unprocessed work = **7 this session**).
State: **4,802 of 5,710 (84.1%)** — 908 remaining, 817 parked, 0 available.
**Pass two now stands at 14 landings.**

## Landed

| function | from | what actually did it |
|---|---|---|
| `OvlFunc_881_200b9fc` | 2 of 579 | **one word** — which the park had derived and then ruled out |
| `CheckSpecialExits` | 10 of 106 | **two levers that are each exactly inert alone** |
| `OvlFunc_934_2009984` | 4 of 261 | **two of the four were pool words** + one word |
| `Func_80a524c` | 7 of 134 | moving three statements |
| `OvlFunc_930_20091b0` | 5 of 302 | a union pun — **retiring a parked owner decision** |

Carried over from batch 314, verified but unprocessed: `Func_801b664` (200 encodings) and
`SomethingSaveHeader` (156 encodings). That brief reported three landings and only one had been
taken — my oversight.

## Every park this session had its blocker wrong

Fourteen landings across two batches, fourteen wrong diagnoses. The parks said *"UNREACHABLE FROM
SOURCE — see PROOF below"*, *"pre-reload scheduling"*, *"reload's output-reload register, no reach
from source"*, *"not reachable from source"*, *"no source spelling moved it"*. One proposed **a
compiler feature that does not exist** (a fourth allocation entry point) and was on its way into
this document.

The sharpest case: a park **derived the exact landing mechanism in its own header** — that a
value-returning call's `SET` of r0 intercepts the output-dependence chain — noted its callee was
declared `void`, and concluded *"neither half of that asymmetry is available here."* **The asymmetry
was in our own declaration.** Fourteen recorded failed spellings rested on that sentence.

## The sched2 ladder: four rungs, not three, and one is dead

Four briefs worked scheduling residues and between them the ladder is now properly specified:

    priority → CLASS vs last_scheduled_insn → dependent count → INSN_LUID

- **The class rung was missing from this project's record entirely** (3 independent, 2 anti/output,
  1 data; higher wins). Found via a trace where an insn with *four* dependents ranked **below** one
  with three, because it anti-depends on the store just scheduled.
- **`INSN_REG_WEIGHT` is dead at sched2**, gated `!reload_completed`. Register pressure is never a
  key here.
- **The class rung is skipped at t=0**, since `last_scheduled_insn` is 0. So a block's *first*
  decision has one fewer rung than its later ones — which is why a preheader contest and a body
  contest behave differently with identical-looking tables.

I had propagated the three-rung version into every brief this batch and messaged all five running
agents mid-flight when it was corrected.

### And the alias-set precondition I sent was wrong

I told five agents the aliasing store must be *later* in the chain than the load. Corrected:
**store-before-load** gives a true dependence moving **priority**; **load-before-store** gives an
anti-dependence adding a **dependent**. Order picks **which rung** the lever acts on, not whether it
works — one function landed with the store *preceding* the load.

### A hard bound that retires a whole family of attempts

**No pool load is ever a MEM for scheduling**, because **the literal pool is built after sched2** — a
pool load is `(set (reg) (const_int))` at `.19.flow2` and only becomes a MEM at `.26.mach`. In a tie
between two pool loads there is nothing to widen. Out of scope **by dump, not by assertion**.

Two more scope facts, each one look: **a hard-register destination is never a PRE candidate**
(`hash_scan_set` requires a pseudo dest; at -O2 `expand_expr` discards non-pseudo targets; and
`force_operand` is unconditional for `ADDR_EXPR`, so `&local` as an argument always lands in a pseudo
whatever a pin says) — and **`precompute_register_parameters` never touches an address argument under
Thumb**, since `arm_rtx_costs` returns exactly 2 for a PLUS against a `> 2` gate. Two parks inherited
that wrong.

## The tree's two closest parks are the same function

`2009fa4` at 1 of 199 and `200a094` at 2 of 199 are **the same 192-instruction routine in two
overlays**, identical after name/label/pool normalisation — and **neither park referenced the other**.
One carried a pin the other lacked; **that was the entire difference between them.** Porting it brings
both to 1, which also confirms they share one residue.

**So I checked why `dupfuncs.py` missed it.** The tool slices each function to the *next function
start*, dragging in trailing `.global` directives for data symbols (`gOvl_…`, `gScript_…`,
`gTable_…`). Those lines begin with a tab so they survive its filter, and they aren't placeholdered —
only `Func_`/`OvlFunc_`/`.L` are. Two copies of one routine therefore differ in ~39 lines of pure
symbol names and never group. Ending the slice at `.func_end` fixes it: **7 groups / 14 functions →
9 / 18**, with the twin pair now reported.

My first hypothesis was the literal pool words. **It was wrong** — dropping every `.word` line left
the bodies still unequal. Recorded, because a wrong guess is cheap to repeat.

## Read a flag probe at the INSTRUCTION, not at the figure

The method correction that broke the closest closure. A `gcse (PRE)` attribution was doubted because
`-fno-gcse` measures **71 of 199 and +4 bytes** — far worse. That refutes nothing: **`gcse_main` runs
cprop and PRE in one loop**, so the flag removes a pass group the whole function depends on.

Read at the site, the expression survives — the attribution **holds**, and a **second blocker is
exposed behind it**. And the landing mechanism is a pass earlier than either: `local-alloc.c`
`update_equiv_regs` substitutes a single-set/single-use pseudo's `REG_EQUIV` into its one use, which
is exactly **why every reload-level probe was inert**. Third blocker-one-pass-earlier this batch.

## Testing one change at a time is the dominant failure mode

Four distinct shapes of one gap are now measured, and this batch supplied the two cleanest:

| shape | evidence |
|---|---|
| inert because a **prerequisite** is missing | byte-identical alone, worth 2 once another landed |
| pins **jointly load-bearing**, individually inert | 20 inert singly, 65 encodings together, bisected to one pair |
| edits each a **clear regression**, jointly a gain | 69, 112 and 142 individually; **57 → 40** together |
| two edits each **exactly inert**, jointly the whole residue | each reads 10; together **10 → 0** |

`CheckSpecialExits` is the clearest: 24 permutations measured, two reach zero, gradient monotone in
how late one value is read. Its park had **sixteen** inert spellings — because it moved one half at a
time, sixteen times.

> **A park with a long inert list is evidence of one-at-a-time testing, not of a hard floor.**

## Measurement devices caught before shipping

Three in two batches, all sharing one shape: **the device improves the figure by changing what is
measured rather than what is compiled.**

- a **fictitious three-argument alias** of a real callee, reaching diff=0;
- a **never-read union member**, present only to widen an alias set (the union-free body at 42 is kept
  beside the 40 one, with the choice flagged as deliberate);
- an **`.equ` resolving a symbol at assembly time**, which made objcmp read 0 where the real extern
  reads 1. Removed in favour of the shipping form.

> **A figure is only a figure if the body that produced it is one we would ship** — and that belongs
> *at* the figure, not further down.

## Two closures upgraded, one rewritten

- **Rung 4 pinned equal for any source shape**: both competitors are constant materialisations with
  no input registers, so neither can ever carry a data dependence, and both carry the same
  `REG_DEP_OUTPUT`. With the pool-load bound, no route remains.
- **The ROM's own order violates a true dependence** at one site — an alias-set-0 spill store before
  the alias-set-19 load it produces — so **sched2 cannot reach that order at all**.
- A third **survives as a conclusion but failed as an argument**: four load-bearing claims wrong,
  including never naming `REG_ALLOC_ORDER` (the only reason its central assertion holds) and crediting
  the wrong pass for a re-base. **Keeping a right conclusion for wrong reasons is how a park survives
  scrutiny it should not.** Also closed there by **C89 rather than the compiler** — the one promising
  shape is a wrong program, because a `short` aligns to 0x10 and not 0x0f.

## Symbol-table work, held to the file's own standard

- **`_AREA_58`** admitted on the **structural** argument: `0x58` is 8-bit movable, so gcc emits a
  `mov` and the value can never reach the pool as a `const_int` — a pool word holding it can only be a
  relocation.
- **Two reference pool loads respelled** `=0x5e`/`=0x5f` → `_AREA_5e`/`_AREA_5f` (symbols already
  existed). **Two of one park's four "encodings" were pool words**, and the same omission caused its
  relocation delta.
- **`_MSG_ad4`/`_MSG_b2c`** admitted — but **not on the argument proposed.** The park claimed both
  shiftable; they are not (`0x2b5` and `0x2cb` both exceed 8 bits), so a pool word holding them is
  *not* the structural tell `message.sym`'s other entries rest on. Admitted on the weaker **hoist**
  argument that file explicitly reserves, which its own measurement supports (+6 encodings each as a
  literal), with the wrong claim corrected at the entry.

## A parked owner decision retired rather than taken

One landing needed a per-file `ALIAS_CFLAGS` row, parked for batches because a flag row is a claim
about how the original was compiled. **There was no claim to make:** `-fno-strict-aliasing` was a
global way of spelling a local fact, and one union-punned access does the same work. `.19.flow2`
prints each MEM's alias set — store 7, load 10, no conflict — and diffing the sched2 tables against
the flag shows the **dependence sets byte-identical with only priorities moving**. **A struct member
is inert; only a union works**, because `c_get_alias_set` answers 0 for a `COMPONENT_REF` based on a
union.

## My own errors

- **The sched2 ladder** was three rungs in every brief I wrote this batch.
- **The alias-set precondition** I messaged to five agents was wrong as stated.
- **I told several briefs to start with the per-opcode histogram.** Where length is exact and the
  instruction multiset identical it is **identically zero** — true for four parks in one brief, which
  needed positions plus an allocator dump instead.
- **I propagated a price off by ~2×** from a park that guessed live lengths, where `.17.lreg` prints
  them: `Register N used R times across L insns`, exactly the inputs to the priority formula. Reading
  the real numbers also moved the question — the ROM has the *same* reference counts as us, so it
  differed in **live length**.
- **Batch 314's brief C reported three landings and I processed one.**
- **Bookkeeping drift**: doc sections sat uncommitted across five commits while landings were
  processed, and some of those commit messages referenced findings the document didn't yet hold. The
  user caught it.

## Depinning, ahead of pass three

- One park **15 → 5**, with sites inert singly, as value-groups **and** jointly.
- Two others **re-derived as minimal under value-grouping**, not merely greedy fixpoints.
- And a **grep census proved one group check complete**: the two recorded interacting pairs sit in the
  only groups large enough to hold one, with nine singletons — so **no third pair can hide.** That is
  the difference between finding nothing more and there being nothing more to find.

## Next

- **`2009fa4` / `200a094` — both at 1 of 199, one function, work them as one.** The closure is broken
  and the ROM's instruction has been produced by a probe (kept at `docs/repro-2009fa4-vp/`); the
  remaining need is to defeat cse1 without moving the frame.
- The **nested-function TU** is at **106** from 123, with one member still **81/81 exact**, no split
  and zero pins.
- **A park at "8 short" is already solved nested** and blocked on a sibling at 366 of 417 — it must
  not be reissued as a small-residue target.
- `-fno-dce`, `-fnew-ra`, `-fno-loop-optimize`, `-fno-if-conversion` and `-fno-cprop-registers` **do
  not exist in this cc1**.
