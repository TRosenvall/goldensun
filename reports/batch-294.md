# Batch 294 — five functions, and seventeen functions removed from the corpus without touching them

Gated on `make -j8 && make compare` → `goldensun.gba: OK` at
`5c4695205413df7db52b9a184815a07783999971`. All 5 landings verified. 19 new parks and 3 park
updates, every one verified by `parkcheck`. 7 commits. `git status` clean.

| | |
|---|---|
| elevated | **5** |
| parked | **19 new + 3 updated** |
| unopened | **0** — second batch running |
| `.sym` entries added | 1 (`_AREA_3d`), **2 withheld** with their evidence recorded |
| doc corrections | **13**, in 256 lines added to `elevation.md` |
| functions retired as unelevatable | **17**, before any agent ran |
| agents | 8 briefs, 27 targets |

**5 + 19 + 3 = 27**, reconciled per brief. Twenty-five consecutive batches reconcile exactly.

| brief | band | targets | landed | parked |
|---|---|---|---|---|
| A | 140–168 | 5 | **2** | 3 |
| B | 220–228 | 5 | **2** | 3 |
| C | 251–253 | 4 | **1** | 3 |
| D | 285–296 | 4 | 0 | 4 |
| E | 352–355 | 3 | 0 | 3 |
| H | 402–406 | 3 | 0 | 3 |
| F | closing brief, 2 parks | 2 | 0 | 2 updates |
| G | `Debug_PaletteEditor` alone | 1 | 0 | 1 update |

All five landings came from the three briefs at 140–253 instructions. Everything at 285 and above
parked — the third batch in a row that band has produced nothing, which is now a settled fact
rather than a run of luck.

## Seventeen functions retired before the batch began

The largest single result came from answering a question batch 293 had flagged as "worth one
person-minute with objdump". `include/macros.inc` expands `.call_via reg` to an **inline**
interworking veneer — `.align 2,0` / `mov r12, pc` / `bx reg` — where gcc-2.96 with
`-mthumb-interwork` calls a function pointer as `bl _call_via_rN`. Four bytes each, completely
different encodings.

Settled against `baserom.gba` rather than argued from the disassembly, because a `.s` using a macro
is not evidence of what the bytes are. At `LoadMapActors`' site the ROM holds `fc 46 18 47`,
preceded by the `00 00` the macro's `.align` emits. Spot-checked in three banks, with per-function
site counts matching the annotations exactly (13 and 13, 6 and 6) and the `bx` register varying.
**The decisive half: 33 hand-written `.s` files contain `.call_via` and zero generated ones do**, in
4,300+ files.

So **17 of the 249 available functions cannot be elevated under this toolchain**, including
`Func_80ad6d4` at 1,274 instructions, `Func_8090a5c` at 806, `ActorCmd_Player` at 804,
`Func_80f3078` at 797, and `UpdateActors` at 683 with **thirty-one sites**. They were excluded from
this batch's allocation, so no agent paid for them.

## Thirteen corrections, and five of them were mine

Three batches running, this has been the most valuable output. This batch it reached my own briefs,
my own notes, and the Makefile's own comments.

### Corrections to things I wrote

1. **Spill-slot declaration order IS a lever.** I told all eight agents it was "not a lever,
   measured inert, don't spend budget there", quoting the doc. The doc's claim rests on **one**
   measurement on one function. It was worth 8 aligned instructions on one function here and
   decisive for all twelve slots on another. Mechanism confirmed: `alter_reg` walks pseudos in
   ascending number and `FRAME_GROWS_DOWNWARD` is 1. The scoping that reconciles both results —
   inert when the quantities end up register-resident, decisive when they are spilled for their
   whole lives.
2. **"A `goto` leaves `last_test_insn` NULL and the roll never fires" is wrong** — I wrote that last
   batch. The loop's *own* condition also jumps to `end_label`, so the roll fires either way. What a
   `break` changes is **which** jump is last, because `stmt.c:2482` keeps reassigning
   `last_test_insn` to every qualifying jump within thirty insns. Two agents converged on this
   independently. The usable rule is *how many* exits are spelled `break`.
3. **The mul lever's operator list was wrong.** I wrote "does not carry to other commutative
   operators". It does carry to `|` — a byte-field read-modify-write went 39 against 71 on operand
   order — while `and` and `add` are genuinely inert. The doc now has a measured table.
4. **"The bank runs backwards" is not a usable framing.** I told one agent the int-return lever
   reverses in `rom_15000`. One function there calls two function pointers and wants **opposite**
   return types. Per callee, never per bank.
5. **A single-target brief should name the dumps to read, not rank the park's items.** I built G's
   whole brief around a question already answered at `elevation.md:14841`, and the actual largest
   lever was the item the park had deprioritised.

### Corrections to the docs and the Makefile

6. **`if_convert` is a blocker class that had been hiding inside register allocation.** It
   speculatively moves a block, which spills, which changes the frame and shifts every `[sp,#N]`. No
   flag reaches it. Two source-reachable gates, and the first is a genuine gcc quirk: `ifcvt.c:1763`
   reads `/* ELSE is small. */` immediately above `if (count_bb_insns (then_bb) > BRANCH_COST)` — it
   counts **THEN**. So you make the THEN block bigger.
7. **A flag recorded as inert ALONE can be load-bearing in a PAIR.** The Makefile records
   `-fno-expensive-optimizations` as inert in two separate comments. Byte-exact on one function: none
   225, `-fno-gcse` 89, `-fno-expensive-optimizations` **280 (worse)**, **both 22** at the ROM's
   exact size. The coupled-lever warning extends from source constructs to flags.
8. **`MOVE_RATIO` on this target is 2, not the widely-quoted 15** — `arm.md:5084` defines
   `movstrqi`, so the `#else` branch never applies. Any 8-byte block move goes through `movstrqi` and
   never `move_by_pieces`.
9. **`--align` is blind to the literal pool** (`tryc.py:363` drops `.word`), so it can read **zero
   while bytes differ** — the pool is ordered by the constant's **mode**, and a HImode word goes
   first, rotating every later relocation. And it can **invert** a verdict: one change read 119 → 121
   aligned while encodings went 265 → 261. When the two disagree in sign, take the one that moved the
   length.
10. **gcse PRE's join-point form: the source must NOT name the value.** The existing note covers PRE
    lifting to a *dominating* use; the join case is the inverse. Deleting two hand-written locals was
    **273 → 124** on one function — five times any other lever there. `-fno-gcse` is not the
    diagnostic; read `.07.gcse`'s `PRE/HOIST: edge` lines and count them against the ROM's entry
    points.
11. **`const.sym`'s halfword exception needs a pool-*placement* test**, because a HImode entry splits
    gcc's single end-of-function pool. A HImode literal can reproduce the `ldr =K` tell and still be
    badly wrong: 190 of 284 against 2 for the symbol form.
12. **`precompute_register_parameters` (`calls.c:850`) is the argument-fill lever** and explains a
    whole residue class — any argument with `rtx_cost > 2` is copied to a pseudo first, so a cheap
    argument's `mov` gets the largest LUID and sched2 leaves it last. Two levers follow, together
    worth 8 → 0.
13. **The destination-naming lever has a ceiling** — splitting further into named accumulators was a
    regression at all six spellings tried (9, 10, 11, 12, 16, 16).

## Defeating `nonzero_bits`: a family, now with three routes

A large residue class is gcc knowing a narrow value's high bits are clear and deleting a shift, mask
or compare the ROM has. `set_nonzero_bits_and_sign_copies` **unions over every SET** of the pseudo,
which is the handle:

1. **A one-member struct or union carrier.** `promote_mode` widens only the six scalar type codes, so
   a `RECORD_TYPE` keeps its mode and the pseudo is genuinely HImode, failing `nonzero_bits`' fast
   path (`combine.c:7992`). Took `Func_807a664` **31 → 8** at zero cost, after its park had concluded
   no spelling could reach it.
2. **Variable reuse, to add a set gcc cannot bound.** Reusing the variable that had held
   `(unsigned int)&gState` adds a SYMBOL_REF set and a HImode compare's `lsl/lsl/cmp` returns
   byte-exact. A clean `int` carrier fails *because* both its sets are `ldrh`s that union to `0xffff`.
3. **A genuinely unknown producer** — the route the corpus already used.

**A declared `unsigned short` carrier is not on the list and cannot be**, because `PROMOTE_MODE`
makes it `ldrsh`.

## The closing brief worked; the deep dive answered a different question than asked

**F, two parks blocked on arithmetic, both questions answered yes:**
`Func_807a664` **31 → 8 of 143**, now the closest park in the tree, with size, count and all ten
relocations exact and zero shims. `ScreenTransitionIn` **147 → 60**, with size and count now exact —
the 28-byte gap its earlier park dismissed as an artefact is closed, via three coupled mechanisms
(`*thumb_movhi_insn`'s `pool_range` of 64, a HImode carrier, and `local-alloc.c:886` deleting a
pooled constant with exactly two references).

**G, `Debug_PaletteEditor` alone: `--align` 380 → 124**, now converging rather than misaligned. Its
park's stated blocker had its polarity backwards — `giv_count == 0` is the *enabling* condition, and
the dump shows four bivs and zero givs. Third batch running that a park's own blocker was wrong.

## Discipline enforced

**Two parks would have recorded numbers nothing can reproduce.** `OvlFunc_924_20099b8` reaches 4 of
229 under a proposed flag rule and measures **61** under the tree's actual flags; `Func_80ab314`
reaches 22 under two non-default flags and measures **227**. objcmp and parkcheck derive the flag
group from the Makefile, so the headers claim 61 and 227, with the better figures marked
flag-conditional and undecided.

**Two `.sym` proposals withheld despite good evidence.** `_CONST_50` meets both `const.sym` criteria
but a HImode carrier does reproduce the pool and only loses the register to a two-reference deletion
rule, so the symbol may stand in for a source reference nobody found. `_MSG_c30` has the strongest
control seen — the same function pools one shiftable constant and *builds* another — but does not
**complete** its function, and its residue depends on an undecided flag group. Both recorded in their
parks for whoever closes them.

**My own two mistakes, both caught by tooling.** I wrote the comment describing `_AREA_3d` into
`area.sym` and never the definition line; the build failed with `undefined reference`. Beside it a
trap worth knowing: that failed run's `sha1sum` printed the **target hash**, because the previous
good `.gba` was still on disk — a stale artifact reports success next to a `BUILD_FAIL`. And a park
header of mine contained `overlays/*/overlay.ld`, whose `*/` **closed the C comment four lines
early**, which is why parkcheck read that park as having no recipe.

## Three owner decisions, one of them newly strong

- **A `CSE_CFLAGS` row for overlay `rom_7e7574`, as a PAIR.** `OvlFunc_959_2009150` is byte-exact
  under `-fno-rerun-cse-after-loop`, and it is the **second** function in that overlay with the
  identical blocker and identical fix — its twin `2009528.c` was already parked. Two functions, one
  row.
- **`include/dma.h`'s helpers as macros rather than `static inline`.** `integrate.c:743/748` forces an
  inline argument into a register before any body insn, so the argument order the ROM has is
  unreachable; rewriting `DMA3_CLEAR` as a macro removes a residue completely. dma.h's own comment
  already guesses "maybe they were macros" — this is the first measurement bearing on it, and it needs
  a tree-wide screen before any edit.
- **A group for `-fno-expensive-optimizations`**, which only pays beside `-fno-gcse` (see correction 7).

## State

`census` TOTAL and `funcindex` agree at **988**.

| | batch 293 | batch 294 |
|---|---|---|
| functions elevated | 4,717 of 5,710 (82.6%) | **4,722 of 5,710 (82.7%)** |
| still in `asm/` | 993 | 988 |
| — parked | 654 | 673 |
| — **unattempted and attemptable** | 249 | **225** |
| — hand-written assembly | 76 | 76 |
| — ARM | 14 | 14 |

Available fell by exactly 24 = 5 + 19. **Of the 225, seventeen are the `.call_via` class and cannot
be elevated at all**, so the real pool is ~208 — and only **20** remain in the 101–200 band that
produces landings, against 64 / 75 / 65 above it.

## Open

- **The productive band is nearly exhausted.** Two obvious shapes remain: keep harvesting the last 20
  small functions, and run closing briefs on the parks that are already within a handful of
  encodings. The closing brief is the better bet on this evidence — it moved two parks from 31 and
  147 to 8 and 60 in one batch, where four fresh 400-instruction targets produced nothing.
- **The closest parks, all with named blockers:** `Func_807a664` at 8, `SomethingSaveHeader` at 2
  (blocked on the dma.h decision), `OvlFunc_895_2008a24` at 3, `OvlFunc_966_200920c` at `--align` 25,
  `Func_80ab314` at 22 under flags, `ScreenTransitionIn` at 60, `Debug_PaletteEditor` at 124.
- **`Func_80ad6d4` and the other sixteen `.call_via` functions** should be folded into `census.py` as
  a separate column, the way ARM and hand-written assembly already are — they are a structural fact,
  not a difficulty.
- **Six stale reference comments found across two batches** (four this batch). They name the wrong
  function or the wrong size, and each was noted in a park rather than edited.
- Unchanged: the `make clean` churn; ~469 parks with no `Verify with:` recipe; 161 with stale prose
  `Source asm:` lines; and the standing withheld symbols.
