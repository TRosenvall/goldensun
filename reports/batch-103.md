# Batch 103 — the jump-table class, second pass

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked overlay ELF with
`arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_953_200807c` | `0200807c` | ovl_7d95dc | [ovl_30_c_c_a.c](../src/overlays/rom_7d95dc/ovl_30_c_c_a.c) |
| `OvlFunc_881_200837c` | `0200837c` | ovl_77a7c8 | [ovl_30_c_a_c_a_c_a_b.c](../src/overlays/rom_77a7c8/ovl_30_c_a_c_a_c_a_b.c) |
| `OvlFunc_964_2008e20` | `02008e20` | ovl_7ed0a0 | [ovl_30_a_a_c_a_c_c_a.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_c_a_c_c_a.c) |
| `OvlFunc_959_200c638` | `0200c638` | ovl_7e7574 | [ovl_9dc_c_c_a_a_b.c](../src/overlays/rom_7e7574/ovl_9dc_c_c_a_a_b.c) |
| `OvlFunc_945_200c7cc` | `0200c7cc` | ovl_7cb2c0 | [ovl_30_c_c_c_c_c_c_a_a_a_c_b.c](../src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_c_b.c) |

Two parked. Four of the five matched on the first screen. **94 jump-table
functions remain**, down from 106 when the class opened.

## Three new readings

**An arm that sits AFTER the shared tail needs a `goto` to get there.**
`OvlFunc_959_200c638` has seven arms that just pick a message id and an eighth
that walks the actor across the room first. The ROM's layout is: the seven
id-blocks, then the shared `__MessageID / __ActorMessage` tail, then the long
block. Written as an ordinary `case 5:` with its body inline — even written
last — gcc places it *before* the tail and needs a `b` to jump over it, one
instruction too many. `case 5: goto five;` with the body after the tail
reproduces it.

That is recorded honestly rather than confidently in the file: it is what
reproduces the bytes, and the block order does say the body was out of line,
but a `goto` is only one of the shapes that could have put it there.

**The outer area test can be a symbol while the inner case values are
literals.** `OvlFunc_964_2008e20` compares against a pooled `0xac` — the symbol
tell, and `area.sym` has `_AREA_ac` — while its inner switch's case values
(3..13) are plain immediates in the range check. Both are area numbers; the
assembly distinguishes them. `OvlFunc_953_200807c` does the same with
`_AREA_8c` and `_AREA_8e` against sixty-six literal room values.

**`break` inside a case reaches the code after the switch.** The two gated arms
of `OvlFunc_881_200837c` drop out when their flag test fails, and the ROM sends
them to the same block the default uses. A `break` says exactly that, which is
why the fallback is written after the switch rather than in a `default:` arm.

## The case order keeps paying

Every one of these needed the cases transcribed in block order rather than
numeric order:

* `OvlFunc_945_200c7cc` — 0x13, then 0x12/0x14, then 0x16/0x17, 0x18,
  0x15/0x19, 0x1a.
* `OvlFunc_881_200837c` — the six-value group 0x42-0x45/0x4b sits between 0x49
  and 0x50.
* `OvlFunc_953_200807c` — a 66-slot table where 8, 21, 31, 64, 65 and 67 share
  one script, which is not a pattern anyone would guess from the room numbers.

## Parks

* **`Func_801c244`** — three switch arms compare against −1 and the ROM builds it
  fresh in each; gcc shares it, three instructions short. Third member of the
  `-1` rematerialisation family in `constant_reuse.c`; the three CSE flags give
  byte-identical output, matching the eleven-flag sweep already recorded there.
* **`UploadIcon`** — the ROM stages `AllocSpriteSlot`'s return through a second
  register and compares that; gcc compares the value still in r0. Two arms, two
  instructions. Naming the returned value in its own local — the spelling that
  usually forces the copy — is byte-identical.
