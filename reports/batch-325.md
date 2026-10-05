# Batch 325 — the reload register, settled

**7 landings, 13 parks improved, 0 pins, 0 devices, 0 new flag groups.**
Progress **4,860 → 4,867 of 5,710 (85.2%)**; remaining functions 850 → 843,
parked 758 → 751. Every installed park figure independently re-measured by
`parkcheck`.

Eight briefs at three targets each. Unlike batch 324, which swept a band, this
batch aimed at three specific openings 324 had created plus four banks it never
touched.

| brief | targets | result |
|---|---|---|
| A | 2, 2, 3 — the shelf 324 created | `Func_801d9d4` **2 → 0** |
| B | the copy-collapse family | no landing; three mechanism corrections |
| C | `HeightTile_4/6/A` | `_4` **19 → 0**, `_6` **19 → 0** |
| D | parks blaming `REG_ALLOC_ORDER` | `Func_80a1090` **7 → 0** |
| E | `rom_8a000` | `Func_808fe38` **11 → 0** |
| F | `rom_a1000` | `Func_80ad69c` **17 → 0** |
| G | `rom_77000` | `Func_8079bf8` **15 → 0** |
| H | `rom_c9000` | no landing; `Task_SpinCamera` 23 → 18 |

`HeightTile_*` is now **15 of 16**.

## The batch's real content: I was wrong, twice, about something already on file

Batch 324's headline was the **reload round-robin cursor**, found by two agents
independently in different banks with no contact. I promoted it to `HANDOFF.md`,
to `reports/batch-324.md`, and into five briefs of this batch, with the
instruction *"reopen any park blaming `REG_ALLOC_ORDER` for a coherent
rotation."*

**Brief D refuted it from source.** I verified every line, corrected the record,
and messaged all four still-running agents. **Brief E then refuted my
correction**, with an observable I could not argue with.

The settled mechanism has **three** layers:

| layer | where | what it does |
|---|---|---|
| 1 | `find_reg`, `reload1.c:1588` | reserves per-insn registers; minimises `spill_cost`, ties broken on `inv_reg_alloc_order` (`:1645-1662`). Owns the **only** `Using reg` printf (`:1664`) |
| 2 | `finish_spills`, `reload1.c:3609-3627` | **GROWS** the set to (every hard reg not holding a pseudo live across that insn) ∩ (global spill set), with an `abort()` asserting it only enlarges |
| 3 | `allocate_reload_reg`, `:4962` | round-robins from `last_spill_reg` (`:5003`) **inside the grown set** |

**Layer 2 is what both of my statements missed.** So:

- `REG_ALLOC_ORDER` **is** genuinely read, and parks citing it are not
  automatically wrong — batch 324's instruction was wrong as written.
- *"One reload on an insn means one bit set, which means zero freedom"* — my
  correction — is also wrong. **The cursor's freedom is the number of FREE
  REGISTERS at the insn, not the number of reloads on it.**

Brief E's decisive observable, on a single-reload insn:

	.18.greg    Spilling for insn 47. / Using reg 2 for reload 0
	.19.flow2   (insn 135 (set (reg:SI 1 r1) (const_int 165)))
	            (insn 47 (set (reg 3) (plus (reg 4) (reg:SI 1 r1))))

**`find_reg` printed r2; the emitted register is r1.** Across four reloads,
printed r2,r2,r1,r2 against emitted r1,r2,r1,r2. **A `Using reg` line is not the
register you get** — confirm in `.19.flow2`.

### And `elevation.md` already held all of it, at batch 255

The entry *"A RELOAD SCRATCH REGISTER IS ROUND ROBIN OVER A SET THE SOURCE
CONTROLS"* carries `find_reg`'s tie-break, the spill set, the `.18.greg`
discriminator **and** the cure. `CLAUDE.md` says of that file: *"It is long and it
is the point; grep it before writing anything up as a new finding."* Neither the
agents nor I did.

> **Two independent agents reaching the same conclusion is strong evidence the
> FRAGMENT is real and no evidence that it is COMPLETE.** "New" is the dangerous
> word, because it is what licenses overwriting an older, better entry. One grep
> for `allocate_reload_reg` would have found batch 255's entry.

### Every version agreed on the action, which is the usable part

**Change which pseudos are LIVE at the insn, not the spelling of the differing
site.** Batch 255's cure was one line 500 bytes from the defect. `Func_808fe38`
landed 11 → 0 by naming an address as its own statement so a pseudo left an
insn's live set, which stopped `order_regs_for_reload` (`:1534`) banning r3
there. And it is why 112 spellings were inert on one function and 182
perturbations on another.

Triage is **per insn, not per function** — brief H measured 2+-reload insns at
0–10% of blocks across 15 animation functions, median ~2%, with zero reload
inheritance, so a function-level count hides the cases that matter.

## Two more classes refuted

**The `tu-pool` enrollment is wrong AS A CLASS**, and brief G says why for all 30
of them: **every input to `add_minipool_forward_ref` is per-function.**
`push_minipool_fix`'s address is `insn_addresses` within the function and
`arm_reorg` walks only this function's chain, so **a standalone TU has every
lever the original had, whatever the residue.** `Func_8077f70` went 9 → 3 on the
strength of it. That follows batch 324 retiring an `unmatchable.txt` enrollment
of the same kind.

**`allocno_compare` has no loop-depth term, but its input is already
loop-weighted.** Batch 324's reading of the formula was right and I repeated it in
this brief; brief G found all four `REG_N_REFS` increment sites in `flow.c`
(`:4435`, `:4948`, `:5115`, `:5556`) add `pbi->bb->loop_depth + 1`. **So a bound
computed from raw reference counts is wrong by that factor per reference** — one
park's real priority ratio is **1.93** where it had computed 1.25, making the
bound *harder*, not easier. Read `n_refs` out of `.17.lreg`; never count
references in the C.

**And a documented bound was wrong as stated**: *"`const` is not the cure for an
alias-set-0 memory edge … bit-identically inert."* It is inert through a
**pointer** (`expr.c:6512` ands in `TREE_STATIC`) and **decisive on the object**
(measured 3/3/0/0/3). On `gState` that is a **device**, since the object is
mutable global state, so the 0 is a figure about the blocker.

## Family rules amended, both against my own brief

- **The `HeightTile` writeback must be on the SUBTRACTION, not the bias.**
  `t = b - a; a = t + 0xf` lands; `a = b - a; a += 0xf` — **which my brief
  predicted** — is three instructions short, as are two other phrasings, because
  `a` and `b` then both die at the sub and the save pair plus `push {r5}` go with
  them.
- **The multiply's operand order only matters where md operand 1 is not already
  in the destination.** `*thumb_mulsi3` alternative 2 ties operand 1 to the dest
  and emits `mul %0,%0,%2` **with no `mov`** — so **the `mov`'s presence is what
  makes a site live**, and a park claiming "the two sites want opposite
  spellings" was wrong (the high arm is exactly inert either way).

## Tooling, both fixes earning their place immediately

**`objcmp` could not see a pad absorbing a length difference.** gas appends a
`.short 0x0000` to word-align, and that pad is an encoding, so:

| | instructions | pad | encodings | bytes |
|---|---|---|---|---|
| reference | 25 | 1 | 26 | 52 |
| ours | 26 | 0 | 26 | 52 |

Equal size, equal count, one instruction apart. It made a park read **19** when
aligned it was **3**. This is the **third** variant of the padding trap and the
first invisible to a careful reader, so it now lives in the tool as an
`INSTRUCTION COUNT` line. Only trailing zeros are stripped — `0x0000` is a legal
Thumb encoding (`lsls r0,r0,#0`).

**A landing that splits a `.s` silently orphans every other park's recipe in that
file.** Batch 324's two `HeightTile` landings left **all four** remaining family
recipes naming paths that no longer existed, so `objcmp` exited
`FileNotFoundError` and **no figure in that family had been checkable since.**
`install_batch.py`'s splits phase now repoints them, and on its first three real
runs it fixed **ten** parks — including two in `rom_a1000` and two in `rom_8a000`
that no brief this batch was working on. It refuses to guess when a symbol is
ambiguous: **a recipe pointing at the WRONG reference is worse than one pointing
at nothing.**

A tree-wide scan found one further orphan, `rom_c9000/80cd358.c`; repointed, it
measures **67** and its claim verifies.

## Things I got wrong in the briefs themselves

1. **The reload-cursor instruction** (above) — propagated into five briefs. Two
   agents had reached the correct reading independently before my correction
   arrived, so it cost them nothing; brief E says the cursor reading "paid for the
   landing."
2. **`rom_c9000`'s premise was false.** I told brief H to read its landed `Anim_*`
   siblings. **Zero `Anim_*` or `BaseAnim_*` sources have landed** — all 155
   landed files in that bank are split-named, and all 46 named animations are
   parked. The brief's primary instruction could not be carried out.
3. **The `HeightTile_6` index spelling I predicted** was three instructions short.
4. Parallel briefs collided in the shared scratchpad: one found another's
   `dump.sh` with different semantics, costing it a confusing dump. **Namespace
   scratchpad helpers per brief.**

## Carried forward

- **`HeightTile_A`** at 2 of 36 — bound re-confirmed on a 32-body cross, but its
  *mechanism* corrected: the observable is that the incoming copy of `t` is
  `NOTE_INSN_DELETED` in the good body and survives as `mov r4,r1` in the flipped
  one. One open question stated honestly with evidence rather than filled in.
- **The copy-collapse family** — brief B corrected its attribution (it is
  `cse_insn`'s gated transform at `cse.c:5959-6014`, whose gate is a **liveness**
  test at `:5986` via `make_regs_eqv` `:1397`, **not** `delete_trivially_dead_insns`
  as `elevation.md` says). **That is why fourteen bodies measured flat — they
  varied names, types and order, never which live range leaves the block.** A
  block-scoped `unsigned char` intermediate achieves the ROM's three insns for the
  first time. Seven candidate siblings listed, unverified.
- **`SystemMsgBox`** held at 2, but its "two fixes are mutually exclusive through
  the spill set" claim is **refuted** — a crossed pair reads 8 of 85 with
  `Using reg 1` restored; a third defect then appears.
- **`Func_801d014`** at 3 device-free, **0 with a `const` on the object** — an
  owner ruling would settle whether that is acceptable. It is a device by the
  standing definition.
- **`Task_SpinCamera`**: best production-flag body is 18, best `ALIAS_CFLAGS` body
  is **10**, and they are *different bodies*. `ALIAS_CFLAGS` already exists on 20
  objects and the reference `.s` holds one function, so no sibling can contradict
  the assertion. **Owner-facing.**
- **30 `tu-pool` parks** to reopen on brief G's per-function argument.
- **86 parks citing `REG_ALLOC_ORDER`** to triage with the per-insn rule.
