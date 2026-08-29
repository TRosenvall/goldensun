# Batch 85 — a filter that pays for itself, and a lever that is not a rule

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_940_200808c` | `0200808c` | [ovl_30_c_c_a_c_a_b.c](../src/overlays/rom_7c5974/ovl_30_c_c_a_c_a_b.c) |
| `OvlFunc_928_2008968` | `02008968` | [ovl_314_c_c_a_c_c_a_c_b.c](../src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_c_b.c) |
| `OvlFunc_884_20083b4` | `020083b4` | [ovl_30_c_a_a_a_c_c_a_c_a_b.c](../src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a_b.c) |
| `OvlFunc_884_2008444` | `02008444` | [ovl_30_c_a_a_a_c_c_a_c_a_b.c](../src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a_b.c) |
| `OvlFunc_907_2008d10` | `02008d10` | [ovl_30_c_a_c_c_b.c](../src/overlays/rom_79b154/ovl_30_c_a_c_c_b.c) |

## Batch 83's lever is a spelling to try, not a rule to apply

That batch found that naming a constant in a local **of the width it is combined
with**, and writing it first, decides which operand becomes the destination of a
two-operand `orr`. `OvlFunc_928_2008968` is the counterexample, and it matters
because the lever is now three batches old and reads like a rule.

Its `and` needs the constant in the destination — `mov r3, #0xfd / and r3, r2` —
and applying the lever gives the opposite:

| spelling | result |
|---|---|
| `unsigned char m = 0xfd; *p = m & *p;` | **wrong way round** |
| `*p &= 0xfd;` | right |
| `int m = 0xfd; *p = m & *p;` | right |
| `0xfd & *p` with no local | right |
| `v = *p; *p = m & v;` | wrong way round |

So the plain form was already correct and the lever broke it. **When the
operands are the wrong way round, try the narrow local — do not reach for it on
sight.**

## Three functions on one blocker, so the tool now sees it

`OvlFunc_887_2008e34`, `OvlFunc_921_20087a4` and `OvlFunc_959_2008e80` all fell
to the same thing in a single round: gcc **builds** a constant once and keeps it
in a callee-saved register, paying a push and a pop, where the ROM builds it
twice.

    rom    mov r0, #0xc0 / lsl r0, #2   ...   mov r0, #0xc0 / lsl r0, #2
    ours   mov r5, #0xc0 / lsl r5, #2   ...   mov r0, r5    ...   mov r0, r5

That is the same decision as the repeated-pool-load blocker `pick_candidates.py`
already screens for, with `mov + lsl` in place of `ldr =`. Its docstring used to
say the filter could not see it and to check by eye. It sees it now, keyed on
(value, shift) so that different shifts of the same immediate stay different
constants. 143 candidates become 112.

**The first version of the filter had a hole and it cost a screen immediately.**
It required the `mov` and its shift or negate to be adjacent, but gcc emits all
of a call's `mov`s and then all its `neg`s:

    mov r0, #1 / mov r1, #1 / mov r2, #1 / mov r3, #0 /
    neg r1, r1 / neg r2, r2 / neg r0, r0

— which is `-1` built three times. `OvlFunc_881_20097fc` slipped through and was
offered anyway. The filter now matches each `neg rN, rN` back to the
`mov rN, #imm` that last set that register, and reports it as `[repeats -1]`.

## The subtraction has to be wide

`(unsigned short)(gState.f1c2 - 3) <= 1` reads naturally and is wrong. With the
member an `unsigned short`, gcc keeps the whole expression in sixteen bits:

    ours   ldr r2, =0xfffd / add r3, r2      a pooled -3, ADDED
    rom    sub r3, #3

Reading the member into a named **`int`** first widens the subtraction to SImode
and the `sub` comes back. An explicit `(int)` cast on the member does **not** do
it — the local does. Three of 43 lines, and the last three.

This is the same family as batch 84's `int` local for a mask: the constant's
mode is decided by the mode of the operation around it, and a local is how you
set that mode.

## The rest

`OvlFunc_940_200808c` is the last function in the tree with the 32-bit arc test,
and it needed both readings batch 84 recorded: the comparison is unsigned over
the whole word because there is no `lsl #16 / lsr #16` pair, and the compound
condition is **nested** rather than `&&` because the ROM tests twice to the same
target.

`OvlFunc_884_20083b4` needed `__Func_8092c40` left implicit — the second
declaration lever, and the same call that needed it in batch 82. Its shared tail
at `.L40e` is gcc cross-jumping two arms, so both arms are written out in full;
a `goto` would be transcribing the optimiser.

`OvlFunc_928_2008968`'s stored zero is a variable assigned **before** the first
call, so it lands in a pushed r5 as the ROM has it — the same pattern as batches
78 and 83.

## Parks

| function | state | blocker |
|---|---|---|
| `OvlFunc_887_2008e34` | 47 of 72 | one built constant hoisted; five flags measured |
| `OvlFunc_921_20087a4` | 48 of 85 | two hoisted, plus a separate arc-test lowering worth its own note |
| `OvlFunc_881_20097fc` | 43 vs 45 | `-1` built three times |
| `OvlFunc_959_2009ab0` | 34 of 41 | pool load scheduled three calls early — **second** member of that class, after `OvlFunc_883_20091d8` |

`OvlFunc_921_20087a4`'s second difference is worth recording separately: the ROM
shifts first and subtracts a pre-shifted constant, `lsl r3, r5, #16 / add r3,
=0x5fff0000`, where gcc narrows the subtraction to sixteen bits and shifts
afterwards. The test appears twice in that function, so it costs four lines on
its own and would still be there with the hoist fixed.
