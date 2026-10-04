# Batch 321 — seven agents over the verified frontier

**4,833 / 5,710 (84.6%)**, 877 remaining, 785 parked, 0 available.

Eight landings, six of them pin-free. Eleven parks refreshed with corrected
diagnoses. Two landings held for owner decisions. And **four claims the
coordinator had written into the briefs as settled were refuted by the agents**,
which is the batch's real content.

## Landings

| function | from | mechanism |
|---|---|---|
| `Func_8095938` | 3 of 129 | alias-set-0 union member — **the lever came from a landed sibling, not a park** |
| `Func_8095fcc` | 3 of 56 | the same lever by a second route: it closes a *register copy*, which no alias set can depend on, by raising the rival chain's priority |
| `Func_942e0` | 2 pinned / 15 pin-free | typing **one** of two byte stores + one insn of live range — lands **pin-free** |
| `Func_80df9d0` | 2 | the park's five hand-hoisted locals **were** the artifact |
| `Func_80b110c` | 1 | `_MSG_182` admitted on structural impossibility, mode checked |
| `OvlFunc_917_20092f4` | 4 of 207 | tail cross-jumping runs in jump2, **after** sched2 |
| `Task_BlitAnim` | 3 of 104 | one token: `typedef void (*CopyFn)` → `int` |
| `OvlFunc_948_2009308` | 4 of 59 | **deleting its own pin, nothing else changed** |

## THE GCC SOURCE WAS ON THIS MACHINE ALL ALONG

    ~/gs_project/camelot-gcc/gcc-2.96/gcc/

`docs/elevation.md` cites `haifa-sched.c`, `local-alloc.c`, `global.c`, `reload.c`
and `lcm.c` line numbers **in a dozen places and never gives the path**, so briefs
keep re-deriving the scheduler ladder from `-da` dumps.

One brief found the tree late in its run, read `rank_for_schedule`, and caught a
wrong sub-claim of its own **before shipping it** — then *withdrew its correction
of a park*, because the source tests the cost escape **before** the note kind (it
is kind-agnostic) and `arm_adjust_cost` returns 0 for `REG_DEP_ANTI`.

> **A park that cites the compiler outranks your inference from dumps.**

## Four propagated claims, refuted

1. **`qty_compare` has no `floor_log2`.** `allocno_compare`'s
   `floor_log2(R)*R/L` — written into many briefs as *the* allocation priority —
   is **global-alloc's only**; `local-alloc`'s `qty_compare` does not use it
   (verified twice, both directions). One park had imported the global formula
   into a **local-alloc** decision, turning a **1.3% margin into an apparent
   factor of three**, and that substitution is what kept it parked.
   **Free discriminator:** `.18.greg` prints `;; N regs to allocate:`; **`N = 0`
   means local-alloc**, so no `floor_log2`.
2. **The sched2 CLASS rung is not dead.** Recorded as "dead twice over".
   Counterexample plus instrument: priority ties 38/38, the dependent-count rung
   favours the ROM's insn **four to two**, and it still loses — and
   `register int base __asm__("r4")` flips the order to the ROM's.
3. **The HImode mechanism was wrong.** `*thumb_movhi_insn` **does** have an
   immediate alternative (alt 5, constraint `I`). HImode pools everything because
   of **alternative order**: alt 1's source is `mn`, `n` matches any `const_int`,
   and recog takes the first match. The conclusion survives; the distinction is
   load-bearing, because a symbol-table argument depends on *which* alternative
   matches first.
4. **Grouping by named pass is not grouping by deciding rung.** The frontier was
   bucketed by park-prose keywords and the batch assigned on it; **two briefs
   independently refuted their own bucket label**, one receiving four "reload"
   targets of which *none* was reload (three sched2, one combine's
   `simplify_comparison` plus global-alloc), another finding two of three residues
   decided **before any tie-break at all**.

## Read the landed siblings, not just the parks

The one lever that moved more than one target in a brief **did not come from a
park**. It came from a landed, byte-exact file in the same subsystem, whose header
documented the mechanism.

> A landed header records what the TU actually **wanted**. A park records what
> somebody could not make work.

## A park carrying a pin has not necessarily re-measured whether it needs it

`OvlFunc_948_2009308` landed at **0 by deleting its own pin** and changing nothing
else. An earlier batch had held it on debt with a diagnosis of declaration order —
and the depin works at *both* declaration orders, so the two are independent and
declaration order is exactly inert once the pin is gone. `Func_942e0` likewise
went from 2-pinned to **pin-free zero**.

Same finding as batch 320's free-depin sweep, arriving from the other direction:
there it was 14 of 25 *landed* files; here it is *parked* functions whose pin was
the entire blocker. **Delete a park's pins and measure first. That is one compile.**

## New levers and bounds

- **Tail cross-jumping runs in jump2, after sched2**, so a hand-written
  fall-through is **not** equivalent to a duplicated tail plus `break`. Spelling
  each switch arm's final call explicitly gives sched2 one longer block and jump2
  merges the tail back into the ROM's fall-through.
- **sched2 transposes every argument pair except the last one in its basic
  block** — so an argument-order residue can be a question about *where the block
  ends*, not about registers.
- **The alias-set lever is directional and aimable.** Typing *one* of two byte
  stores works; typing **both cancels** — which is why an earlier batch filed
  typing as inert. And it extends past memory: where the residue is a register
  copy, the same edge still closes it by raising the rival's priority.
- **`packed` is for structs whose layout gcc chooses** — 552 of 319 on a
  pad-array struct, which it cannot move.
- **`DIFFERENT_ALIAS_SETS_P` can never fire against a reload spill slot** (it
  prints alias set 0). The lever is live in general; it cannot reach a spill-slot
  competitor.
- **An inline-asm clobber list cannot narrow a volatile asm's scheduling deps.**
- **You cannot give a reload-materialised constant an extra dependent from C** —
  every expression relating two compile-time constants folds before sched2.
- **Screen allocation edits by `.17.lreg`, not by the figure.** Sixteen variants
  read a dead-flat figure with **bit-identical allocator inputs** — the flatness
  meant *the edit never happened*, not *the lever is inert*.

## Symbol table

**`_MSG_182 = 0x0182` admitted.** SImode; `0x182 = 0xc1 << 1`, so constraint `K`
matches at alternative 3 of `*thumb_movsi_insn` while the pool path `mi` is
alternative 6 — **an SImode `const_int` 0x182 can never reach the pool**, and the
ROM pool-loads it. Completion test passes. Corroborated by eight reference `.s`
files across five ROM regions. Required a clean build, which also rewrote 117
tracked generated `.s` files by label renumbering only.

## Two bugs in the tools built last batch, both found by agents

- **`crossfire.py`'s MEM screen fired on every row including BASE.** It profiled
  the candidate by grepping the `.s` while profiling the reference by
  assemble-and-objdump — and gcc spells a HImode pool reference `ldrh r5, .L20`,
  which **assembles to a plain `ldr`**. Grepped `ldr=34 ldrh=2`; objdumped
  `ldr=35 ldrh=1`. That is the identical-encoding trap this project has recorded
  since batch 71, built into the tool written to enforce it. Both sides now
  objdump.
- **`install_batch.py` refused a correct batch after its own splits phase**, because
  the manifest describes the pre-split tree while the install phase runs
  post-split. **A phase tool has to know which phase it is in.** It now detects
  the post-split state and warns instead of failing.

## OPEN — owner decisions

- **`_MSG_b24`** for `Func_80a9a5c`. Unlike `_MSG_182`, a literal `0xb24` **can**
  reach the pool (it is neither 8-bit movable nor shiftable, so gcc pools it
  anyway) and the ROM's pool word carries **no relocation** — so the bytes do not
  single out a symbol. The argument is the hoist tell, upgraded to a mechanism
  (a Thumb `const_int` stays a `const_int` to final, while a `symbol_ref` goes
  through `force_const_mem` and becomes a real MEM, and only the MEM form carries
  the sched2 dependence on the call). **Sufficient but not necessary.** Without
  the entry the park sits at 9 pin-free; with it the function lands.
- **`Field_Halt`**. Byte-identical under a per-file `-ffixed-r11`, or **1 of 191**
  flag-free from a spelling none of the park's eleven literals tried. The honest
  argument against the flag is in the candidate's own header: the landed twin
  `Field_Whirlwind` is in the same bank and uses fp freely.

## Also open, recorded not acted on

- `iwram_3001f30` is declared **eleven different ways** across the tree, against
  an unused `struct MapState` that does not fit the offsets its users need.
- The `Func_80b9554` TU has **no tool-backed figure**: `objcmp` cannot score
  nested functions, `parkcheck` cannot check it, and its "66 / 40" comes from a
  recipe no tool implements. An independent harness read 104 / 62 — not a
  refutation, but the one number steering that TU is the one nothing can
  recompute. Its park also **contradicts itself**, calling the live-length routes
  "arithmetically out of reach" and then concluding the ROM's compilation "must
  have differed in LIVE LENGTH".
- `OvlFunc_887_2008578` carries **45 pins** and has **no pin-free figure at all** —
  every depin changes the instruction count.
