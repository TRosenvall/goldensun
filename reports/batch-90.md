# Batch 90 — seven functions, one screen, and two refinements to the width rule

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_883_200db48` | `0200db48` | [ovl_30_c_c_c_c_c_a_c_c_c_a.c](../src/overlays/rom_780898/ovl_30_c_c_c_c_c_a_c_c_c_a.c) |
| `OvlFunc_910_2008974` | `02008974` | [ovl_30_c_c_c_c_c_a.c](../src/overlays/rom_79dd90/ovl_30_c_c_c_c_c_a.c) |
| `OvlFunc_914_2008cb4` | `02008cb4` | [ovl_30_c_c_c_c_c_c_c_a.c](../src/overlays/rom_7a1ff0/ovl_30_c_c_c_c_c_c_c_a.c) |
| `OvlFunc_927_200ac0c` | `0200ac0c` | [ovl_30_c_c_c_c_a.c](../src/overlays/rom_7b4558/ovl_30_c_c_c_c_a.c) |
| `OvlFunc_936_200ba3c` | `0200ba3c` | [ovl_30_c_c_c_c_c_c_a.c](../src/overlays/rom_7c097c/ovl_30_c_c_c_c_c_c_a.c) |
| `OvlFunc_939_2009840` | `02009840` | [ovl_314_c_c_a.c](../src/overlays/rom_7c460c/ovl_314_c_c_a.c) |
| `OvlFunc_946_200aed8` | `0200aed8` | [ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a.c](../src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a.c) |

Another `find_shape.py --clusters` family with nothing solved in it, and the
tightest yet: **all seven differ in nothing but the callback name**. Every id,
mask, offset and argument is identical across all of them, so one screen bought
seven functions.

## A trailing `& 0xf` can be a bitfield

Batch 71's rule reads the width of a mask to choose the spelling: a 32-bit
`mov / neg` pair means a bitfield, a bare byte `mov #0xNN` means hand-written
masking. The head of this function has three masks on two adjacent bytes:

    mov r2, #0xd / neg r2, r2 / ldrb r3, [r6, #9] / and r2, r3    ~0xc
    mov r3, #4 / ldrb r1, [r6, #5] / orr r2, r3
    mov r3, #0x21 / neg r3, r3 / and r3, r1 / strb r3, [r6, #5]   ~0x20
    mov r3, #0xf / and r2, r3 / strb r2, [r6, #9]

The third — `mov r3, #0xf` — *looks* hand-written by the width rule. It is not.
It is a four-bit field cleared to zero, and **gcc merges adjacent bitfield
writes to the same byte** into the single load and store the ROM has.

| spelling | result |
|---|---|
| ordinary masking, `int` locals to keep the constants wide | 77 differing of 98 |
| three bitfield assignments | **exact** |

So: when a byte-width mask sits next to 32-bit ones on the same field, try it as
a bitfield before assuming the width rule applies. Two loads and two stores for
four operations is the tell that the writes have merged.

## The order of bitfield assignments is the ROM's interleave

`f9_mid = 1; f5_b5 = 0; f9_hi = 0;` matches. The same three regrouped so both
`f9` writes sit together — which reads better, and is what anyone would write —
is **20 of 98**. gcc emits them in the order written and schedules the loads
around them, so the interleave in the assembly is source order, not scheduling.

## The same function wants a named zero and a bare one

It stores `0` to five fields and `1` to two.

- The **zero** needs a named `int z` assigned at the top, so its pseudo is live
  across the calls and lands in a pushed register — the pattern from batches 78,
  83 and 85.
- The **one** must be a plain literal in both places. gcc shares it into a
  callee-saved register by itself, and giving it a local puts the `mov #1` on
  the wrong side of a neighbouring store.

Same function, same kind of constant, opposite answers — which is the third time
in six batches that a lever has needed reading off the ROM rather than applying
because it worked last time.

## The last three lines

`__UploadSpriteGFX` is deliberately undeclared. With a prototype gcc fills its
argument registers r0 first; the ROM fills r1 then r0. That is the second
declaration lever, and it took the function from 5 differing to 3, with the
final 2 falling to writing the `1` as a literal.

## Where the shape clusters stand

Batch 88 measured 143 functions reachable through shape clusters. Batches 89 and
90 have taken 14 of them, and the four largest groups — 18, 17, 17 and 7 members
— are untouched. Those four alone are 55 functions behind four screens.
