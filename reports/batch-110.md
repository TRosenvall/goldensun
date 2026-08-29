# Batch 110 — the eighteen-member family, and where the work actually is

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of its overlay's linked ELF with
`arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_910_200850c` | `0200850c` | ovl_79dd90 | [ovl_30_c_c_c_c_a_c_a_b_b.c](../src/overlays/rom_79dd90/ovl_30_c_c_c_c_a_c_a_b_b.c) |
| `OvlFunc_931_2008874` | `02008874` | ovl_7b8cb0 | [ovl_30_c_c_c_c_c_c_c_c_c_c_a_b.c](../src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_a_b.c) |
| `OvlFunc_883_2008eb4` | `02008eb4` | ovl_780898 | [ovl_30_c_c_c_a_a_a_c_c_c_a_a_b.c](../src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a_b.c) |
| `OvlFunc_945_2009a08` | `02009a08` | ovl_7cb2c0 | [ovl_30_c_c_c_c_c_c_a_a_a_a_a_b.c](../src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_b.c) |
| `OvlFunc_945_2009a60` | `02009a60` | ovl_7cb2c0 | [ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_b.c](../src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_b.c) |

**2475 functions remain in assembly, 231 parked.**

## Where the remaining work is, by size

| band | functions | instructions | share of remaining instructions |
|---|---|---|---|
| 1-10 | 27 | 200 | 0.0% |
| 11-20 | 78 | 1,289 | 0.3% |
| 21-40 | 311 | 9,873 | 2.3% |
| 41-70 | 562 | 30,867 | 7.1% |
| 71-100 | 385 | 32,195 | 7.4% |
| 101-150 | 401 | 49,779 | 11.4% |
| 151-250 | 293 | 56,666 | 13.0% |
| 251-500 | 236 | 81,485 | 18.6% |
| 501-1000 | 126 | 87,667 | 20.1% |
| 1001+ | 53 | 87,144 | 19.9% |
| **total** | **2,472** | **437,165** | |

**Batches 104-110 have worked almost entirely in the 41-100 band.** That is 947
functions and 14.5% of the remaining instructions. It is where the levers in
`docs/elevation.md` were derived and where they land first-screen most often.

The honest read of the table is that **58% of the remaining instructions are in
functions of 250+**, and 415 functions of 501+ hold 40% on their own. Small
functions are not where the mass is; they are where the *technique* is. Every
lever this project has (the basic-block lever, carried-vs-rebuilt, CSE_CFLAGS,
the argument-order table) was found on a function under 100 instructions and
then applied upward. Batch 104's scouting park reached 1025 of 1027 on a
1027-instruction function using exactly those.

## The eighteen-member family: 169 of 176, parked

`tools/prologue_families.py` (batch 109) reports three families of 17-18
identical per-overlay copies. This round attacked the largest, and it is parked
at [ovl_780898/20080c4.c](../src/non_matching/ovl_780898/20080c4.c) at **7
differing of 176 — across eighteen functions.**

It is the block-pushing routine: take the player's facing, index a sixteen-entry
table by its top nibble for a direction delta, probe in front for a pushable
actor, probe beyond and above it for obstructions, test collision, and walk both
actors one tile.

Three readings were solved and each is reusable:

* **The table entry packs two signed halfwords into one word**, and the two are
  extracted differently — `d & 0xffff0000` gives the X delta already at 16.16
  scale, `d << 16` gives the Z delta.
* **The shift is destructive**, so it is `d <<= 16;` as its own statement, not
  `d << 16` in the argument. Three sites, one instruction each.
* **The shorts at +0xa and +0x12 are the HIGH HALVES of the `int` x and z**, not
  fields of their own. The tail does `ldrsh [+0xa] / lsl #16 / str [+8]` — which
  is truncating a 16.16 coordinate to a whole tile by reading its own top half
  back. Declaring them as struct fields shifts every offset after +8 and costs
  32 instructions.

The residue is seven lines of tail scheduling: gcc slots the independent
`mov r3, #0x80 / mov r2, r8` in before two zero stores; the ROM does the stores
first. Same instructions, same registers. Seven flags and six source orderings
measured.

**Seven instructions from eighteen functions is the largest single lever left in
the tree**, and the park now contains everything except the last idea.

## Two smaller parks, both instructive

**`Func_80bf37c`** (three members) reaches 31 against the ROM's 32 — one
instruction SHORT. The missing one is a redundant `mov r3, r2` the ROM keeps
after `ldrb r2, [r5]`. Getting there needed the `goto`-the-shared-exit form and
`n += 0xff` rather than `n--`: on a value that is stored as a byte and then
tested for zero, `+0xff` and `-1` are the same modulo 256, and gcc picks
`add #0xff` only for the first spelling.

**`OvlFunc_948_2009838`** (three members) is the constant hoist with **no
boundary to lever against**. Two pool constants are passed to two consecutive
`__MapActor_SetSpeed` calls; gcc builds each once into a callee-saved register,
the ROM builds them twice. Six CSE-related flags measured, none of them reach
it — `-fno-rerun-cse-after-loop` included, which is the first case found where
the flag does not fix a constant hoist.

That is worth stating precisely, because batch 106's rule was "try the flag
first": **the flag reaches CSE across a call, not the hoist gcc performs when
building the same pool constant for two calls in one straight-line block.** The
lever reaches the second, and needs a branch.

## Five elevated, and one new reading

`OvlFunc_931_2008874` needed **`_AREA_4b` and `_AREA_4c`**. Both fit in a `cmp`
immediate and the ROM pools them — `ldr r3, =0x4b` — which only happens for a
linker symbol. Both were already in `area.sym`. A literal gives `cmp r2, #0x4b`
and the pool entry vanishes.

`OvlFunc_910_200850c` carries **eight levered constants**, the most in one
function so far, and two of them are the same value (`0x1070000`) passed to two
different calls. Each gets its own local, because the ROM rebuilds it at each
site — REBUILT, one per site, per the batch-107 rule.
