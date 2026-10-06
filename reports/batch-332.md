# Batch 332 — eight landings, and the 700 layer falls

Five briefs, 15 targets, **8 landings**. **707 → 699 parked.**
**4,911 → 4,919 of 5,710 (86.1%).** Gated green at every phase; SHA1
`5c4695205413df7db52b9a184815a07783999971` throughout.

## Every target was chosen by one criterion, and it worked

All 15 came from `reports/park-figure-audit.md` on a single test: **the two
sides' 16-bit instruction counts differ by 1 or 2.** Nothing about the
mechanism, the bank, or the park's diagnosis entered the choice.

| brief | targets | landed |
|---|---|---|
| A | 3 | **2** |
| B | 3 | **2** |
| C | 3 | **1** |
| D | 3 | **2** |
| E | 3 | **1** |

**8 of 15.** Every brief landed something, and **every brief found the missing
or extra instruction on every target** — the three that stayed parked did so for
reasons downstream of alignment, not because the gap was unfound.

### The direction of the gap is a different diagnosis, and briefs read it right

Splitting the batch by direction was worth doing. Five targets were "ours is
LONGER" — we emit an extra instruction, meaning gcc failed an optimisation the
original build made, so the search is *what lets gcc reuse a value* and never
*what statement do I add*. Brief E reported that framing "was the whole key on
all three", and both of brief A's long targets had park headers pointing the
lever at the wrong statement.

## Landings

| function | from | what closed it |
|---|---|---|
| `OvlFunc_888_200a750` | 12, ref +1 | a `signed char` field, not `unsigned` |
| `OvlFunc_880_2008384` | 13, ours +1 | the if-arms written **1/0** rather than `-(a != b)` |
| `OvlFunc_973_200871c` | 24, ref +1 | a `struct Blk` ported from a corpus hit |
| `OvlFunc_941_2008094` | 23, ref +1 | `struct Actor` from this tree's `include/actor.h` |
| `Func_8005868` | 29, ref +2 | `base + 0x40` at the use site; `return r != 0` |
| `OvlFunc_957_2008f10` | 18, ours +1 | the constant named **before** a call |
| `CanRemoveItem` | 39, ref +1 | `off = slot + 0xd8` as a new pseudo |
| `AddPartyMember` | 21, ref +2 | an if/**else** giving a second read a second path |

Parks improved: `Func_80a7440` 23 → **4**, `Func_80063bc` 27 → **3**,
`Func_8006408` 31 → **7**, `OvlFunc_common1_588` 25 → **9**, `Func_8079664`
28 → **14**, `Func_80788c4` 39 → **14** — and in every one of those six the
**lengths now agree**, so those figures are distances for the first time.

## The three mechanisms worth keeping

**`asm/` is a labelled corpus.** Every generated `.s` is byte-matching by
construction, so each instruction window is tagged with the C construct that
produced it. Brief B landed two targets by grepping the generated half for the
reference's window and reading the `.c` beside the hit, after spelling-level
search had failed on both. Written up in `docs/elevation.md` with three query
forms by cost.

**In Thumb, `local-alloc` can never hand out r7.** `HARD_FRAME_POINTER_REGNUM`
is r7 (`arm.h:898-899`) and `local-alloc.c:1978-1990` reserves it
unconditionally — *"It can move only regs made by global-alloc."* It is **not**
reserved: 1,010 generated files push it. So wherever the ROM holds a value in r7
and we do not, the question is **whether that value crosses a basic-block
boundary**, never what spelling asks for r7. Also established: the comparison
expanders `sne` and `abssi2` are `TARGET_ARM`, so in Thumb a boolean can only
come from `expr.c:7720-7738` (`foo != 0` intercepted directly) or
`expr.c:7745-7765` (the `TRUTH_ANDIF` fallback) — **and the two spellings of "is
it nonzero" produce different instructions.**

**A copy only survives when it is not a copy in the source.** `CanRemoveItem`'s
park had tried `off = slot; off += 0xd8;` and got byte-identical output, because
a copy whose source is dead is copy-propagated away. `off = slot + 0xd8` keeps
it, because Thumb-1's 3-bit three-operand add cannot encode `0xd8`. The park had
varied the shift for three rounds; the deciding statement was the add.

## Two parks are one answer away from three functions

**`asm/rom_c0/rom_5cf8_a_a_c_a.s` holds three functions and all three are
parked**, so none converts alone — brief E found this with the piece check and I
verified it. `Func_80063bc` is at **3** and `Func_8006408` at **7**, both with
lengths aligned, and `Func_8006384`'s 24 was never a distance: we emit
`R_ARM_ABS32 REG_SIOCNT` where the reference has the bare literal `0x04000128`.
**One cast plus three instructions takes three parks off in one landing.**
(`io.h:482`'s `vu16` macro is also the wrong width for that register — the
reference reads it 32 bits wide.)

`rom_78414_c_c_a_c_a_c_c.s` is the same shape at 5 and 14.

## Waiting on the owner — now two, both evidenced

1. **`Func_80a8f40`** is byte-identical with `(int)&_MSG_741`, which needs an
   entry in `message.sym`. Parked at its device-free 6; seven device-free
   spellings are exactly inert because cprop folds every one back.
2. **`YesNoMenu2`** reaches **1 differing encoding of 35 with every instruction
   identical** — the sole difference is the pool-word placeholder for
   `_CONST_24`, needing `_CONST_24 = 0x24;` in `const.sym`. Criterion 1 is met in
   its strong form (`arm.h:1096`), criterion 2 measured over eight literal
   spellings, and the internal control is **in the same `.s` file**.

One owner question was **withdrawn**: `Func_8006408`'s pooled zero needed no
symbol — gcc-2.96 pools it unprompted when the destination is byte-wide, because
the narrow-mode move has no immediate alternative.

## Carried forward

- **The audit shortlist has ~45 unworked parks left at gap ≤ 2**, and this batch
  went 8 for 15 on it. It is the best-priced list in the tree.
- **133 parks sit at a one-instruction gap** overall; only a handful have been
  touched.
- The `rom_c0` and `rom_78414` trios above, both worth three.
- `src/non_matching/rom_15000/8028df4.c`'s standing verdict — *"nothing in the
  source chooses which of five live ranges gets the high register"* — is
  refuted by the r7 finding and should be re-attacked.
