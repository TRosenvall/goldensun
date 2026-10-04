# Batch 323 — ten agents, grouped by bank

**4,853 / 5,710 (85.0%)**, 857 remaining, 765 parked, 0 available.

**Fourteen landings, all pin-free**, and eleven parks improved. The bank grouping
is now three batches old and earning its keep: landings keep coming out of solved
neighbours rather than out of parks.

## Landings

| function | from | mechanism |
|---|---|---|
| `Func_80bd7a4` | 18 of 25 | `DMA3_SET_RW` + a `_call_via_r0` declaration from a landed file |
| `Func_80b8000` | 19 of 45 | a named `int` intermediate, then `z = 0;` hoisted |
| `Func_800fa8c` | 20 of 28 | one expression instead of two names |
| `Func_80c1054` | 17 of 22 | misalignment, not distance |
| `Func_80be02c` | 16 of 29 | two causes: epilogue return type + `*q = 1; v = 1;` |
| `Func_80173f4` | 18 of 46 | six pointer locals, each assigned before its value |
| `Func_801b9a8` | 19 of 32 | `_CONST_1f` crossed with the park's own rejected edit |
| `Func_801b9ec` | — | the same body, riding on the split |
| `Func_801a7f4` | 17 of 134 | two edits, **one of them worth nothing alone** |
| `Func_80b8530` | 10 of 30 | the number of **exit points**, not register birth order |
| `Func_8011a84` | 7 of 40 | `DMA3_CLEAR_OFS` |
| `Func_80936a0` | 16 of 48 | from the park's **"keep this"** list |
| `MapActor_WaitAnim` | 18 of 26 | `*thumb_addsi3` has no CONST_INT alternative, so reload splits it |
| `Task_Thunder` | 19 of 117 | two byte stores as aggregate members |

Improved: 20→2, 17→4, 17→8, 14→5, 18→11, 18→16, 15→3, 12→8, 6→6 (corrected), 4→2.

## `.26.mach` EXPLAINED 29 ENCODINGS IN ONE COMPILE

The instrument found at the end of batch 322 did the batch's biggest single piece
of work. A halfword store of a literal makes a **HImode minipool fix at
`pool_range` 64** against SImode's 1020, and that does **two** things — which is
why parks read it as several independent blockers:

1. `add_minipool_forward_ref` (`arm.c:4817`) sorts the pool by
   `address + forwards`, so the HImode word jumps to the **front**, and every
   `ldr [pc,#imm]` offset in the function shifts.
2. Sixty-four bytes of reach **cannot** put the pool after the epilogue, so gcc
   **dumps it mid-function and branches over it** — +1 insn, +1 alignment `nop`,
   +4 bytes, every relocation shifted.

**14 of one function's 18 and 15 of another's 20.** One compile, no spellings.

### WHICH PUTS AN UNMATCHABLE ENROLLMENT IN DOUBT — follow-up

`Func_80b09fc` is enrolled `tu-pool` on the grounds that *"the ROM dumps its
literal pool MID-FUNCTION with a skip-branch before the epilogue … an original-TU
pool-pressure artifact a standalone TU cannot reproduce"*, naming
`Func_80b0a20` as having the identical tail.

**A HImode minipool fix produces exactly that shape from ordinary C.** This does
not prove the enrollment wrong, but **it removes its stated reason**, and an
unmatchable enrollment is the strongest closure this project makes. Re-measure
both with `.26.mach` before trusting either.

## TWO PARKS' "WHAT IS RIGHT AND SHOULD BE KEPT" LISTS WERE THE BLOCKER

One said **"NEXT: nothing source-level outstanding"** and listed its offset
variables as things to keep. The ROM's `add r3,#4` / `add r1,#2` are
**`reload_cse_move2add` products, not source arithmetic** — there is no offset
variable in that function at all, just four constant-offset accesses. Five
spellings all read 0, and the one shipped is the one a landed sibling in the same
`.s` family already uses.

> **A park's "do not change this" list deserves the same suspicion as its
> diagnosis** — it is the same act of inference, recorded with more confidence.
> **An `off` / `off +=` idiom in a park is a SUSPECT, not an asset.**

A third park's **"NEXT MOVE"** instruction was also refuted by measurement: it
told the next reader to install `int v`, claiming 32 is "14 plus a positional
shift"; on aligned distance the installed body is **9 in 6 hunks** and `int v` is
**10 in 4** — strictly worse, because it adds a `lsl#24`/`lsr#24` pair where the
ROM has `adds r2,r3,#0`.

## A FOURTH OUTCOME FOR DUPLICATE PARKS: TWO PARTIAL ANSWERS

Three were on record — true redundancy, a bodyless phantom, a stale pair. Brief I
found a fourth and it is the best: both parks held real definitions, **both claims
measured correctly** (20 and 28), and **the worse park held half the answer**. Its
early-return tail is the ROM's tail; the better park's head is the ROM's head.
**Crossing the two was worth 18 of the 20.**

> So the rule for a duplicate pair is not "find the better body and retire the
> other" — **cross them.** Nine more duplicate pairs remain.

Brief F settled another: both bodies real, md5s differ, **genuine alternatives** —
and the shared "18 of 26" was **misalignment in both and a distance in neither**.

## FOUR MECHANISMS READ FROM THE COMPILER, none previously documented here

- **local-alloc ties a derived address to a dying offset pseudo**
  (`local-alloc.c:1090-1178` → `combine_regs` 1593). Thumb's `add rd,rn,rm` has
  **no matching constraint**, so every dying operand is a tie candidate and
  `q = base + off` steals `off`'s register. Refusable only by making the offset a
  **global** allocno — so the lever is to have no offset variable.
- **regmove will not keep a commutative two-address op in place on a parameter
  pseudo.** `regmove.c:1199-1205` gates on `replacement_quality`: a pseudo copied
  from a hard register scores **1**, a fresh constant **3**, so `param &= K` is
  always retargeted off the parameter, costing two `REG_N_REFS`. Explains why a
  landed sibling *can* keep `lsl`/`asr` in place — those are not commutative.
- **A load's address register donates a hard-register preference to the load's
  destination** — `set_preference` strips `XEXP (src, 0)`, `prune_preferences`
  republishes it, `find_reg` ORs it into `used` (`global.c:1016`).
- **`*thumb_movhi_insn` operand 1 is `"l,mn,l,*h,*r,I"`** (`arm.md:4318`), so
  alternative 1's `n` matches any `const_int` before alternative 5's `I` — **the
  8-bit `mov` is unreachable for a HImode const_int**, even for the value 9.

## A STRONGER SYMBOL-TABLE ARGUMENT: THE SIGN OF THE POOL WORD

With a pool-forcing **literal**, gcc **negates** the pooled `const_int`
subtrahend and emits `adds r0,r0,r3` against `.word 0xfffffeff`. With the
**symbol** it emits the ROM's `subs r0,r0,r3` against a *positive* pool word.

> **`sub reg,reg,pool` is unreachable from any literal, and the SIGN of the pool
> word is the evidence.** Checkable against every existing entry.

That **validated `_CONST_1f`**, which `const.sym` carried as UNVALIDATED, twice
over. And the caution that comes with it: **the pool-forcing literal is a good
alignment instrument and a misleading screen** — one crossed edit read 14 with the
negated `add` and **1** with the real `sub`.

## Traps and bounds

- **`.12.life`'s `Register N used R times across L insns` does not decide
  allocation** — `reg_live_length` is recomputed before global-alloc. Screen on
  the printed `;; N regs to allocate:` order. `allocno_compare` ties **by allocno
  number ascending**.
- **Equal encoding counts can hide a length difference**, because `objcmp` counts
  the trailing `.short 0x0000` alignment pad as an encoding. Found independently
  by two briefs, on five functions between them (23-v-22 and 22-v-21 insns both
  reporting matched counts). `crossfire.py` now carries an **`INSNS`** flag for
  exactly this.
- **Two defects can cancel in the positional count** — one park's 9 aligned
  hunks were 8 + 1 and both streams measured 92.
- **`lang_get_alias_set` returns 0 for ANY char-precision reference**
  (`c-common.c:3348-3351`), so a baseline, a union and a struct member all have
  alias set 0 — and the bank's "lever 5" is **not** about alias set 0 and **not**
  directional in the way it was briefed. What is pinned: the aggregate needs
  **two or more members**. The pass was not identified, and that was said rather
  than guessed.

## `crossfire.py`'s BASE flag: answered, and it is informative

I had asked agents to report any flag on the BASE row. Two reported one, **both
true positives**: a park whose extra pool load really is one `ldr` over the
reference, exactly cancelled in the total count — which is why 117 tied 117.

> **A MEM flag at a tied instruction count localises the residue to an opcode
> class.** That is more useful than the figure.

## Corrections to the coordinator's own brief

- **"Quote local priorities as a ranking, never a number" is a LOCAL-alloc rule
  only.** Where `.18.greg` prints `;; 19 regs to allocate:` the denominator is
  plain `live_length` with no parity term, so global figures are exact and
  reproduce greg's printed order. I had stated it unconditionally.
- One brief reported a lever as **"not tested rather than measured inert"**,
  because no sched2 question survived into its residues. **Saying which it is
  matters** — an untested lever recorded as inert becomes a bound nobody revisits.

## Devices caught and withheld

- A device-free body reading **5** that is 42 insns with one fewer `ldr` than the
  reference — a better figure from doing less work. Kept as
  `docs/repro-b323/p3_rejected_vtop.c` so it is not re-found and believed.
- A device symbol giving **5 of 43** with count and size exact — **not proposed**,
  because four encodings remain and it fails the completion test. The
  `_FILE_e4`/`_e5` case: argument recorded, entry withheld.
- Four variants reading **7** that store the ROM's two values in the opposite
  order.

## Housekeeping

`DMA3_SET_RW` and `DMA3_CLEAR_OFS` promoted into `include/dma.h` rather than
shipped file-local, per `docs/elevation.md` — the second and third helpers of that
shape after `DMA3_FILL_OFS`. Both candidates re-verified against the header copy.
Seven splits, each gated byte-neutral before any `.c` was written, with
`tools/repoint_parks.py` run after each.

## Open

- **`Func_80b09fc`'s unmatchable enrollment** — stated reason removed, see above.
- **`Debug_WarpMenu_UI`** — 17 pin-free, **0 with one pin**, two independent
  one-pin routes. Parked under owner decision 3; the `r6` route is the better
  pass-3 candidate because r6 is callee-saved and the ROM genuinely keeps the
  pointer there, so the pin asserts something the bytes show.
- **`Func_8092b08`** — 17 pin-free, **0 with three pins**. Pass-3 candidate.
- **`HeightTile_4`** — two parks, both with real definitions; `8011d60.c` (19) is
  the keeper over `HeightTile_4.c` (20, count-misaligned).
- **`HeightTile_6`** — the `HeightTile_A` levers port but are **nearly inert**
  (19 → 18); its residue is its own.
