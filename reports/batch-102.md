# Batch 102 — working the jump-table class

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_8079d7c` | `08079d7c` | main ROM | [rom_79460_c_c_c_c_a_c_c_a_c_c_b.c](../src/rom_77000/rom_79460_c_c_c_c_a_c_c_a_c_c_b.c) |
| `Actor_SetBehavior` | `08093a6c` | main ROM | [rom_93304_a_c_c_c_b.c](../src/rom_8a000/rom_93304_a_c_c_c_b.c) |
| `Func_80a9dc4` | `080a9dc4` | main ROM | [rom_a8604_c_c_a_c_a_c_b.c](../src/rom_a1000/rom_a8604_c_c_a_c_a_c_b.c) |
| `OvlFunc_888_200814c` | `0200814c` | ovl_7892c8 | [ovl_30_c_c_a_a_a_a_a_a_b.c](../src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_a_a_a_b.c) |
| `OvlFunc_888_200b334` | `0200b334` | ovl_7892c8 | [ovl_30_c_c_a_a_c_c.c](../src/overlays/rom_7892c8/ovl_30_c_c_a_a_c_c.c) |

Nothing parked. Four of the five matched on the first screen; the fifth needed
one known lever.

## The class batch 101 opened is productive

All five are jump-table switches, taken straight off the list. **99 remain.**
The working method is now settled, and it is mostly transcription rather than
guesswork:

**Read the table before writing the switch.** Slot *i* is case `base + i`, where
`base` comes from the `sub` before the range check. Slots pointing at the
default label are values with no case — and they have to be written out anyway,
because they are what keeps the range dense enough for gcc to choose a table
over a decision tree.

**The block order gives the source's case order.** `OvlFunc_888_200814c` needs
its cases in the order 0xa/0xc, 0xb, 0x14/0x15/0x32, **0x20, 0x1d**, 0x23 —
0x20 before 0x1d, which is neither numeric order nor anything you would guess.
Transcribing the block order matched on the first screen. `Func_8079d7c` is
ordered by *return value* rather than by case value, for the same reason.

**The groupings are visible too.** `case 8: case 9: case 28: case 32:` sharing
one body is a different thing from four cases gcc happens to merge, and the
table shows which slots point at the same label.

`tools/find_jumptables.py` now lists what is left, smallest first, with the
table size and the number of distinct targets so a function can be sized up
before it is read. `slots - tgts` is roughly how much of the range falls to the
default.

## A switch with no default block

`Actor_SetBehavior` has no default arm at all. Its range check —
`sub r3, r1, #1 / cmp r3, #6 / bhi` — jumps **straight to the call site with r1
untouched**, so anything outside 1..7 is handed to `_Actor_SetScript` as-is.

That only makes sense if the second parameter is a script pointer that small
values overload as ids, and it is why the C assigns into the parameter rather
than into a separate local: a local would need a `default:` arm to initialise
it, and that arm would cost a block the ROM does not have.

## Smaller readings

**A named offset for a register-offset load.** `Func_80a9dc4` was two
instructions out until `off = i * 4 + 0xc8` became its own local — that gives
the ROM's `mov r3, r2 / add r3, #0xc8 / ldr r3, [r6, r3]`, the whole offset in
one register and a register-offset load off the base. Written inline gcc folds
the base in and uses an immediate-offset load. Batch 92's rule, still earning
its place.

The same function shows gcc **sharing the `lsl` between the jump table's index
scaling and that offset**, which is why the default arm carries its own copy of
the shift — the dispatch path computed it and the default path did not. That is
gcc's arithmetic, not the source's.

**A shared `neg` across three arms.** `Func_8079d7c` returns `-0x3c`, `-0x5a`
and `-0x64` from three different places, and all three compile to `mov r0, #k`
followed by a jump into one `neg r0, r0`. Writing the negative literals is
enough; gcc cross-jumps the tails itself.
