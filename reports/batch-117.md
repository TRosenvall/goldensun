# Batch 117 — a new compiler flag, a new selection tool, and two rounds that produced nothing

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
All five addresses read back out of the linked overlay ELFs with
`arm-none-eabi-nm`.

**5 elevated, 7 parked. 2402 → 2397 remaining.**

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_967_20080c8` | `020080c8` | rom_7f21b8 | [ovl_30_c_c_a_c_a.c](../src/overlays/rom_7f21b8/ovl_30_c_c_a_c_a.c) |
| `OvlFunc_927_2009520` | `02009520` | rom_7b4558 | [ovl_30_c_c_a_c_c_b.c](../src/overlays/rom_7b4558/ovl_30_c_c_a_c_c_b.c) |
| `OvlFunc_943_2009d0c` | `02009d0c` | rom_7c7b9c | `ovl_30_…_a_c_b.c` |
| `OvlFunc_959_200a134` | `0200a134` | rom_7e7574 | `ovl_9dc_…_c_a_b.c` |
| `OvlFunc_945_200d6dc` | `0200d6dc` | rom_7cb2c0 | `ovl_30_…_a_c.c` (needs `-ffixed-r7`) |

## `-ffixed-r7`

`OvlFunc_945_200d6dc` needs three callee-saved registers. gcc takes r5, r6, r7;
the ROM takes r5, r6 and **r8**, which in thumb is not free — it costs
`mov r6, r8 / push {r6}` at entry and `pop {r3} / mov r8, r3` at exit. Ours was
55 lines against 59 and those four instructions were the whole gap.

`-ffixed-r7` reserves the register and nothing else: **55 → 59 lines, 41
differing → 9**, and the residue was a plain r5/r6 swap that two separate locals
for the two script pointers closed. `-fno-omit-frame-pointer` also reserves r7
and is the wrong tool — it adds frame setup and gives 61.

It is now a `FIXEDR7_CFLAGS` group with one explicit rule. **It is not a class
key**, and I checked rather than assuming: `StartThunder2` stays at 32 of 74 and
`Func_80f7df0` at 18 of 30 under the flag. The tell that it is worth trying is
mechanical — the ROM saves a high register and you do not, and the line-count
gap is about four.

## The no-prototype lever

`OvlFunc_927_2009520` makes five 6-argument calls where gcc emits `mov r0, #2`
first and the ROM emits it last. `-fno-schedule-insns` (13 of 76),
`-fno-schedule-insns2` (37), `-fno-rerun-cse-after-loop` (13), and a carried
local for the first argument (13) all failed. **Deleting the callee's `extern`
declaration matched exactly** — without a prototype gcc has no parameter types
to convert against and expands the arguments in a different order.

Also not a class key. `OvlFunc_960_2008d24` goes 8 → 14 (worse) and
`OvlFunc_948_2009df8` stays at 18.

## Two rounds of nothing, and the tool that came out of them

Between the two useful rounds I attempted four functions and elevated none:
`Func_80c1ebc` (66 of 75, four independent blockers), `StartThunder2` (32 of 74,
though the whole `DMA3_CLEAR` block is exact), and then two straight-line call
scripts that failed **on the same thing as each other**.

That last pair is the useful part. `OvlFunc_882_200bc48` uses `0xb3 << 1` at two
call sites twenty calls apart; `OvlFunc_881_2009c08` uses `0x16f` and `0x171` at
two sites each. In every case the ROM rebuilds the constant and gcc hoists it
into a callee-saved register. The functions are straight line — no labels at all
— so the control-flow boundary the constant-CSE rule requires does not exist and
cannot be made. Seven flags and the symbol-address technique all leave the
instruction *count* wrong, which is the tell that none of them touched the hoist.

**This was visible in the assembly before I wrote a line of C**, so
`tools/script_candidates.py` now ranks straight-line call scripts by repeated
expensive constant. In band 30–70 there are 68 such scripts and **40 have none**.
The first two I picked off the clean list matched on the first screen.

Two refinements the specimens forced:

* **gcc hoists a pool load too**, not just a multi-instruction build — even
  though the reload and the replacing `mov` cost the same one instruction. I
  predicted the opposite and was wrong; the filter counts `ldr rN, =V`.
* **One symbol used twice is hoisted like an integer.** `(int)&_CONST_16f` at
  both sites changes nothing. The doc's "two DISTINCT symbols of equal value
  reload" is about two different symbols, and this is the measurement that
  separates the two readings.

The filter's first version counted every `mov rN, #imm8` and reported 40 of 41
functions as blocked — the opposite of useful. Bare small immediates are
rematerialised for free, which is why every script in the corpus passes 0 and 1
at a dozen call sites without trouble. Only constants costing more than one
instruction to build are hoisted.

## Also this batch

`_AREA_a5` — `OvlFunc_960_2008d24` compares a `gState` halfword against `0xa5`
and the ROM **pools** it, though thumb's `cmp` immediate covers 0–255. Spelling
the test `== (int)(&_AREA_a5)` took it from 62 differing to 17, and the
stack-arg-pair lever took it to 8, where it is parked. That sharpens the
batch-116 correction: `ldr rN, =0` is genuinely not a symbol tell, but a pooled
*non-zero* small constant still is.

Seven parks were written this batch, six of them with full measurement tables:
[2008ce4](../src/non_matching/ovl_7eaf28/2008ce4.c) (1 of 27),
[2008d24](../src/non_matching/ovl_7eaf28/2008d24.c) (8 of 65),
[80f7df0](../src/non_matching/rom_f6000/f7df0.c) (18 of 30),
[common2_41c](../src/non_matching/overlays/common2_41c.c),
[c1ebc](../src/non_matching/rom_b5000/c1ebc.c) (66 of 75),
[StartThunder2](../src/non_matching/rom_8a000/95290.c) (32 of 74),
[200bc48](../src/non_matching/ovl_77dd1c/200bc48.c) and
[2009c08](../src/non_matching/ovl_77a7c8/2009c08.c).
