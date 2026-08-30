# Batch 146 — thirteen rounds of failure, turned into a selection filter

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every
address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_808e0b0` | `0808e0b0` | main ROM | [rom_8d9a4_a_c_a_a_a_a.c](../src/rom_8a000/rom_8d9a4_a_c_a_a_a_a.c) |
| `GetJupiterDjinni` | `08095a44` | main ROM | [rom_944ec_a_c_a_a_c_a_b.c](../src/rom_8a000/rom_944ec_a_c_a_a_c_a_b.c) |
| `Func_80c23a0` | `080c23a0` | main ROM | [rom_c1a34_a_a_c_c_a_a_b.c](../src/rom_b5000/rom_c1a34_a_a_c_c_a_a_b.c) |
| `OvlFunc_927_20095d0` | `020095d0` | ovl_7b4558 | [ovl_30_c_c_a_c_c_c_a_a.c](../src/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_a_a.c) |
| `OvlFunc_927_20096f0` | `020096f0` | ovl_7b4558 | [ovl_30_c_c_a_c_c_c_a_b.c](../src/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_a_b.c) |
| `OvlFunc_927_2009de0` | `02009de0` | ovl_7b4558 | [ovl_30_c_c_c_a_a_c_a_b.c](../src/overlays/rom_7b4558/ovl_30_c_c_c_a_a_c_a_b.c) |
| `OvlFunc_927_2009ef0` | `02009ef0` | ovl_7b4558 | [ovl_30_c_c_c_a_a_c_b.c](../src/overlays/rom_7b4558/ovl_30_c_c_c_a_a_c_b.c) |
| `OvlFunc_948_20095f0` | `020095f0` | ovl_7d30e0 | [ovl_30_c_a_c_c_a_a_c_c_c_c_c_c_b.c](../src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_c_c_c_b.c) |

Between batch 145 and these eight there were **thirteen consecutive rounds with
no elevation at all.** That stretch is the subject of this report, because what
came out of it is worth more than the eight functions.

## What thirteen rounds of failure produced

Every round parked a function one to seven instructions from matching, and every
park recorded the spellings tried and their scores. Read together they are not
thirteen separate problems; they are five conditions, each measured:

  * **Under 40 instructions.** Every lever in `docs/elevation.md` works by
    changing what the register allocator does. A function with a
    two-instruction body gives it nothing to act on. Four parks demonstrate it:
    `OvlFunc_959_2008c78` (3 of 10, three namings byte-identical),
    `Func_80a3ce4` (a range test gcc folds), `SetTextColor` (5 of 8, both AND
    operands dead so the destination is free), `Func_8019d0c` (the explicit
    pointer lever that fixed the identical residue one function over does
    nothing).
  * **Uses r8-r11.** Allocation priority, read out of the compiler with `-dg` on
    `OvlFunc_883_200d64c`: `global.c`'s `allocno_compare` explains our order and
    not the ROM's, which is why no reordering reaches it.
  * **Repeats an expensive constant.** CSE when the uses are close, and PRE
    hoisting when one dominates another -- a distinction probed in five variants
    and clean: *rebuilds* when every use sits in a branch no other dominates,
    *hoists to the top of the function* as soon as one dominates. Distance and
    intervening call count make no difference.
  * **Over 120 instructions.** Too many independent residues to converge.
  * **Fewer than 8 calls.** Arithmetic bodies hit instruction selection instead.

`tools/pickable.py` encodes all five. **104 of the remaining functions pass.**

## The result

The first candidate the filter produced, `GetJupiterDjinni`, matched after one
documented lever. The next four matched **on the first screen with no lever at
all** — the structure alone was enough.

Five consecutive first-screen matches, against thirteen rounds of none from
targets picked by size or by hand. The levers were never the bottleneck;
choosing functions that would fail for reasons no lever reaches was.

## The filter caught a bug in itself

Its first constant detector required a `mov` *immediately* followed by an `lsl`.
On that reading it admitted `OvlFunc_952_200be40` — the function whose PRE
hoisting motivated writing the filter, and where the ROM puts `mov r0, #8`
between the pair. Fixed to pair a `mov` with a later `lsl` of the same register,
which drops the candidate list from 151 to 104.

A filter that silently admits the case it exists to reject is worse than no
filter, and the docstring says so.

## Other findings from the stretch

**`whodoesthis.py`** searches the *generated* assembly of already-matching
functions for a residue shape and prints the C that produced it. A count over
*remaining* assembly tells you what the original authors wrote; a count over
*generated* assembly tells you what this compiler will emit. Confusing the two
produced four wrong "unreachable" claims in six rounds, all since corrected. The
tool closed three functions, including `Func_808e0b0` in this batch — parked
across four rounds, and fixed by struct types plus a loop guard written
`i = 0; if (i < o->f27)` rather than `!= 0`, found by reading a matching
function at the same field offset.

**`dupfuncs.py`** — 101 of the remaining functions are byte-identical to another
after normalising symbols, labels and pool operands. 27 groups, so 74 come free.
Three groups carry fifty functions: `OvlFunc_883_20080c4` (×18, parked at 7 of
176), `OvlFunc_883_200834c` (×17), `OvlFunc_883_20088c0` (×15, length exact at
142/142). Those parks carry mapped residues.

**One unresolved contradiction, recorded rather than smoothed over.**
`OvlFunc_952_200be40`'s ROM rebuilds a constant at a dominating use, which the
probe says gcc will not do under our flags. The likeliest reading is two
distinct symbols coinciding in value, but that is inference from a
contradiction, not a measurement, and this tree has no symbol space to write it
with.

## Parked during the stretch

Nineteen, all with measured spellings rather than "scheduling". The closest are
`Func_80974d8` (2 of 48, with a four-way controlled zero behind it),
`OvlFunc_932_20082cc` (2 of 74), `CheckEquipmentCritBoost` and
`Actor_SetAnimAndSpeed` (2 of 51 each), and `OvlFunc_923_20091b4` (2 of 28).
