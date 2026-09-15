# Batch 268 -- five functions, one tool, and a symbol with a control

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All five addresses checked
against the linked ELF, plus `_MSG_b20` (absolute, resolving to exactly
`0x00000b20`) and both rehomed `.rodata` labels at their original addresses. A
control symbol not in the batch was checked alongside and correctly came back
absent.

| | |
|---|---|
| elevated | **5** |
| new tools | **1** (`tools/datacheck.py`) |
| `.sym` entries added | 1 (`_MSG_b20`, with an in-function control) |
| whole-file conversions | 1 |
| splits | 3 (one a text/data rehome) |
| build-input changes | 0 |
| fakematch debt added | 0 |

Solo throughout, no agents.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `InitAnimContext` | `0x080b9d34` | [rom_b9b30_a_b.c](src/rom_b5000/rom_b9b30_a_b.c) |
| 2 | `Sprite_AddLayer` | `0x0800b8ac` | [rom_b798_c_a_a_c_a.c](src/rom_9000/rom_b798_c_a_a_c_a.c) |
| 3 | `Func_80d6750` | `0x080d6750` | [rom_d6504_a_c_c_c_b.c](src/rom_c9000/rom_d6504_a_c_c_c_b.c) |
| 4 | `Func_80a153c` | `0x080a153c` | [rom_a1050_c_c_c_b.c](src/rom_a1000/rom_a1050_c_c_c_b.c) |
| 5 | `Func_80a15f0` | `0x080a15f0` | [rom_a1050_c_c_c_c_a.c](src/rom_a1000/rom_a1050_c_c_c_c_a.c) |

## The tool came first, out of batch 267's mistake

Batch 267 deleted a `.s` that carried a `.rodata` block along with its function
and lost 92 bytes of string literals. **Neither `tryc` nor `objcmp` can see
that** -- they compare one function -- and both reported a clean exact match;
the linker caught it. The hand grep that would have caught it was run and read
past.

`tools/datacheck.py` reports a `.s` file's data sections, functions and
`.global` exports, skipping generated `.s` (nothing to split) and data-only `.s`
(nothing to lose). Run over the tree it finds **79 hand-disassembled `.s`
carrying both code and data** -- roughly one target in fifteen, so not a one-off.

It earned its keep twice this batch: it cleared three targets before I touched
them, and it flagged `Func_80a15f0`'s file, which is why that one landed as a
text/data split instead of losing four exported labels.

`split_s.py` also earned its keep: asked to split `Func_80a153c` out of a file
whose `.rodata` the function references, it **refused**, named the three `.L`
symbols needing `.global`, and said to gate `make compare` on the export
*before* the split so the two changes stay separable. Done in that order --
EXPORT-ONLY GREEN, split, LAYOUT-ONLY GREEN, then the `.c`.

## Findings

**`s8` is UNSIGNED in this tree.** `include/gba/types.h` has `typedef char s8;`
and the tree is built with `__CHAR_UNSIGNED__`, so `s8` emits `ldrb` where the
ROM has `ldrsb`; a field needs `signed char` spelled out. Worth 35 differing of
70 on `InitAnimContext`. **A trap in the opposite direction to the usual one**,
because the type is *named* `s8` and reads as signed at every glance.

**Use the tree's structs, not hand-rolled offsets.** `Sprite_AddLayer` sat at 2
differing through every spelling of one statement -- it read as an unreachable
sched2 placement. A file-mate already defined the real `SpriteHost`,
`SpriteInfo` and `SpritePart`. My hand-rolled version had the right *offsets*
and the wrong *types* (`*(u16 *)(info + 2)` against `u16 unk_02`). **gcc
schedules a typed field load differently from a cast dereference.** Adopting the
existing definitions was exact.

**`i != N` rather than `i < N`** closed `Func_80d6750`, and it decides two
things: the exit test (`cmp #6/bne` against `cmp #5/ble`) and, less obviously,
whether gcc strength-reduces a derived value into a second induction variable.
The ROM's form *costs* an instruction per iteration, so it never looks like an
optimisation you are missing. Read the ROM's exit test before writing the loop.

**Thumb cannot `mov` an immediate into r8..r11.** A constant gcc parks in a high
callee-saved register costs an extra `mov` pair *at every site*.
`Func_80a15f0`'s three per-block stack arguments as ONE reused local came out
four bytes long; three separate locals keep each in r7. 130 differing to 52.
Counting instructions does not spot this -- you have to look at *which* register.

**Name every value a call takes when the ROM loads it somewhere else first.** A
load into a register that is then only moved to r0..r3 means the source named
it. `Func_80a153c` had three such sites and was four bytes short until all three
were named.

**Read past the formatting when diffing streams.** On `InitAnimContext` eleven
of twelve diff lines were hex-against-decimal and two-operand-against-three, and
accounted for *none* of the differing encodings; the twelfth (`lsr` against
`asr`) was the whole residue.

## `_MSG_b20`, and the control that makes it defensible

gcc builds a Thumb constant inline when it is a **shifted byte** and pools it
otherwise. `Func_80a15f0` passes three ids to the same callee:

```
0xb1c, 0xb1d   not shiftable   gcc pools them   -> literals reproduce
0xb20          0xb2 << 4       gcc builds it    -> mov r0,#178 / lsl r0,#4
                               the ROM pools it -> ldr r0, =0xb20
```

**The two literal neighbours in the same function are the internal control.**
They reproduce as plain literals in the same three-block sequence, so the claim
is "this one constant gcc CAN build and the ROM chose not to", not "this id
space wants symbols". That is the difference between a `.sym` entry and a
convenient explanation, and `const.sym`'s own header asks for exactly this kind
of check.

## A measured `dma.h` finding I deliberately did not ship

`Func_80d67dc` came out ONE instruction short: the ROM reloads the DMA count
from the pool for each transfer, while gcc loads it once, because `DMA3_SET`
binds the count to r2 as an **input** and clobbers only `"memory", "r0"`. Adding
`"r2"` reproduces the ROM and takes it from 30 differing to 2.

**Tested tree-wide: 29 files use `DMA3_SET`, and rebuilding all of them with the
widened clobber leaves `make compare` GREEN.**

It is not in the tree. It completes no function on its own -- that function
still ends two encodings out -- and declaring an *input* register clobbered is a
fiction about the asm, not a fact about the hardware. Shipping a shared header
on that basis for no landing is not worth it. The measurement is recorded in the
park so whoever closes the last two can make the change with the evidence in
hand.

## Parks

`Func_80d67dc` at **2** of 72, length exact -- the residue is which frame base
gcc picks for a store (`[sp]` against a register already holding sp).

`Func_942e0`'s park gained a **negative**: batch 268's new "use the tree's
structs" rule is the obvious thing to try there (its residue is also two
encodings, and it was written with cast byte stores), and it is **inert** --
15 unpinned and 2 pinned, both unchanged, same two instructions. Recorded so the
new rule does not send anyone on that round trip.

## State

```
funcindex --stats   4358 from C, 1352 still in asm     (batch 267: 4353 / 1357)
tools/census.py     TOTAL 1350  (76 hand-asm, 14 ARM, 589 parked, 671 available)
matched .c files in src/ (excl. parks): 3992
park files: 473
```

**+5 from C and -5 still in asm, for exactly the five elevated** -- the third
consecutive batch where the delta reconciles. Batch 264's figure remains
unexplained and that entry stays open.

## Not done

* The three global_alloc parks (`Func_80a8578`, `Func_80cd52c`, `Func_80919d8`)
  want `global.c` -- `allocno_compare`, the conflict graph, `find_reg`. Nobody
  has read it yet.
* `Func_8028ef0`'s residue is reload scratch selection plus argument-setup
  order, a class with no recorded levers.
* The remaining cold `.call_via` functions are all 115+ instructions.
* `8021390`'s `_MSG_1b` build-input question and
  `OvlFunc_968_200c048`/`200c520` behind `200c2bc`'s floor, both still held.
