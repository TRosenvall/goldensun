# Batch 338

Five briefs, 25 targets, **10 landings** — eight from the briefs and two from a
zero-token post-pass. **669 → 660 park files**; census ROM functions landed
4,955 → **4,965**.

Landings: `Func_80a7440` (A), `OvlFunc_897_200ac1c` and `OvlFunc_946_200add0`
(B), `Func_8079664` and `Func_80b0070` (C), `Func_80a5fe0` and `Func_80b8824`
(E, both from **prose** parks), `Func_80c0130` (the batch-337 hold, lifted), and
`OvlFunc_915_20089f8` + `OvlFunc_927_2009d04` by sweep alone.

## The prose pool is not the expensive pool

`tools/shortrefs.py` had ranked the 133 bodyless parks by reference length —
nothing under 21 instructions, 40 over 300 — and I concluded they were expensive
ground. **That assumed the body still had to be written.**

Brief E drew five and found **all five name a candidate in `scratch/` that still
exists and still compiles**, then landed three in one round. Swept tree-wide:

> **70 of the 133 prose parks name a live `scratch/` candidate.** The work was
> done, measured, and left in a gitignored directory instead of installed, so the
> park beside it read as unattempted prose and every instrument was blind to it.
> Exactly **one** of the 70 failed to compile.

`tools/scratchfill.py` finds them. 36 produced an encoding figure and 33 more a
*length* verdict; all 69 are now installed as real park bodies with the figure
first, a verify recipe, and the original prose preserved beneath.

| | before | after |
|---|---|---|
| parked bodyless | 138 | **65** |
| parked *with a figure* | 516 | **557** |
| unverifiable (no recipe) | 114 | **38** |
| the ≤10 band | 44 | **66** |

**Eighteen of the recovered bodies sit at ≤8 differing, seven of them at 2**, and
**nine of the length-gap ones are one or two instructions off** — which is this
tree's most productive criterion (8/15, 7/25, 8/25, 10/32 across four batches).

## Two landings by sweep alone, no agent

After fixing the anchor axis (below), a sweep of the 35 clean parks at 1–5
differing found **two zeros and one improvement**:

- `OvlFunc_915_20089f8` 2 → **0** on one `do { } while (0);` at boundary 16
- `OvlFunc_927_2009d04` 2 → **0** at boundary 17

Both had been prose parks that morning. The full route was
**scratchfill → measure → autosweep**, three zero-token steps. That is a 1-in-17
landing rate for a pass costing only wall-clock, which makes it worth running
*before* every batch rather than instead of one.

`OvlFunc_896_200c260` has an anchor taking it 3 → 2; **recorded, not shipped** —
one encoding is not worth shipping ahead of understanding the residue.

## `do { } while (0)` is not a stronger barrier — it is a loop note

Batch 337 cited `haifa-sched.c:3744-3757` as though an anchor reached it the way
an `ASM_INPUT` does. Brief A corrected it and the source agrees: those lines sit
inside `if (loop_notes)` (`:3714`) and then `if (schedule_barrier_found)`, which
is set only by `NOTE_INSN_LOOP_BEG`/`_LOOP_END`/`_EH_REGION_*`/`_SETJMP`. A
`do { } while (0)` emits loop notes; a bare `__asm__ volatile ("")` emits none and
never enters that branch.

Two consequences: it is **not** a basic-block split (block count 3 on both sides,
`.19.flow2` block 0 identical), and **its placement is predictable** —
`:3735-3736` hangs the notes on the *following* insn, so it fences immediately
before the next insn. `anchor03` reached 0 where adjacent `anchor02` measured 4.

## Three autosweep bugs, all found by briefs, all false-clean-negatives

1. **`boundaries()` walked brace-depth-1 only**, so nothing inside an `if` or loop
   was ever anchored — and a nested residue got the full "both cheap axes are
   CLOSED" verdict on half an axis. Brief D found it the expensive way: **both**
   of its anchor wins came from placements the sweep could not generate
   (`20087b0` inside `if (v < 0)`, 14 → 4; `8096ddc` inside its `for`, 11 → 10).
2. Its `CLAIM` regex carried one word order, so it read **four of five** of brief
   D's targets as having no figure.
3. **Relocations were not reported**, and brief D showed `20087b0`'s "14 of 71"
   came with `RELOCATIONS differ` — two pool words transposed — so that 14 was
   never a plain distance.

Also fixed after the fact: it no longer inserts after a nested `{` (a brace
opening an initializer is indistinguishable by text, and it silently produced 3
no-rows from 8 variants), it skips boundaries that would put a statement among
declarations in C89, and it now **reports how many variants produced no row** —
because measuring fewer variants than you wrote reads exactly like a clean sweep.

**`directory.py` was the fourth tool with the same claim-regex defect** —
requiring `differing` adjacent to `encodings` — so all 36 freshly measured parks
landed in its "No parseable figure" section while `parkcheck` read them fine.

## Mechanisms

- **A value whose uses lie in ONE basic block is allocated by `local-alloc`**, so
  splitting a reused local can reassign a callee-saved register no priority lever
  can reach (`local-alloc.c:1341-1420`). `OvlFunc_946_200add0` 13 → **0** on
  `d = b - 1;` in its own local. Its park had correctly proved `allocno_compare`
  can never prefer `b` over `a` (3934 vs 2069, confirmed by `.18.greg`'s own
  line) — **the order never needed to change.**
- **`allocno_compare` is computable, and a tie is a win** (`global.c:617-619`
  breaks on allocno number). Its `floor_log2` makes the threshold non-linear, so
  compute it before spending a round — on one park, dropping `slot` from 5 refs to
  4 is useless and to 3 wins.
- **`j = i;` is a FREE insn** when its pseudo sorts after its source, because
  flow2's `delete_noop_moves` (`flow.c:2622-2624`) deletes it. That is how
  `Func_8079664` went 8 → 0.
- **On a function with a stack array, an added local is not a null edit** — so
  several inherited "temp/carrier" negatives were measuring frame layout.
- **The multiply operand copy is decided at expand** (`*thumb_mulsi3`,
  `arm.md:1116-1129`, with `expand_binop`, `optabs.c:647-669`), closing the
  16-member `HeightTile_0..F` family after two batches aimed at the allocator —
  both registers were already the ROM's.
- **A byte access through a struct member does not get alias set 0**; it gets the
  struct's set, even for a plain `unsigned char`.

## `Func_80c0130`: the batch-337 hold, withdrawn rather than ruled

It needed a bare barrier inside `include/dma.h`'s inline helper — new for a
shared header, since every caller would inherit a scheduling device. Substituting
an anchor was measured in all four combinations and **every one matches**, so it
landed with **no device anywhere** and the header carries none.

> **When a hold is about a DEVICE, test a device-free construct before asking for
> a ruling.** A precedent for devices in shared headers was nearly set for a
> function that did not need one.

## Bookkeeping

`Func_801cc50` is parked at 3 rather than landed at 0: its three pins are the
`.call_via` class, structural to an indirect call, and both candidate bodies carry
them identically — so the question is the barrier, and the anchor substitution is
untried. Six parks whose subject is **already landed** were retired, and one
mis-assignment during the scratchfill (a park whose displaced line 1 made the
resolver pick up its *twin*) was caught by validating all 36 against their
filename addresses, reverted, and the check now runs before anything is written.
