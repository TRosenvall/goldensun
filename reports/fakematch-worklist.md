# Fakematch worklist

*Functions matched by pinning a register with inline asm rather than by finding
the construct. Every one is a debt, listed here so a later pass has somewhere to
start.*

## Why these exist

The straight-line half of the arg-interleave class. The ROM materialises an
expensive operand in **two pieces** with another argument scheduled into the
gap; gcc emits it in one piece:

    rom    mov r1, #0x81 / mov r0, #0xe / lsl r1, #1
    ours   mov r1, #0x81 / lsl r1, #1   / mov r0, #0xe

Batch 37 found the lever for functions **with a branch** — assign the value to a
local in a different basic block from the call — and it needs a block boundary
that a straight-line function does not have. 509 functions are in that position.

Fakematching them was authorised as an interim measure, with a later pass
intended. This file is that pass's starting point.

## What is already ruled out — do not re-run these

**Twenty formulations, all producing the contiguous form:**

The literal at the call site; a named local assigned at its declaration; the
same assigned as a separate statement; both operands as locals in either order;
a nested block; a comma expression; `const`; `* 2` instead of `<< 1`; an extern;
a function parameter; the callee declared, undeclared, and with widened
parameters; eight different return types on the *preceding* callee.

**Eight optimisation flags, all byte-identical:** `-fno-schedule-insns`,
`-fno-schedule-insns2`, `-fno-peephole`, `-fno-force-mem`, `-fno-caller-saves`,
`-fno-expensive-optimizations`, `-fno-cse-follow-jumps`, `-O1`.

## The one positive result, and it is the lead

**`volatile` on the local produces the RIGHT ORDERING, in plain C, with no
inline asm:**

    volatile unsigned int w = 0x81;
    __MapActor_Surprise(0xe, w << 1);

    ->  mov r1, #129 / mov r0, #14 / lsl r1, r1, #1 / bl

It is unusable **only** because it also forces a stack slot — `sub sp, #4 /
str r1, [sp] / add sp, #4`, three instructions the ROM does not have.

That is worth stating precisely, because it changes what the open question is.
It is not *"can plain C produce this ordering"* — it can. It is **"what gives a
local `volatile`'s effect on scheduling without its effect on storage"** — a
register-level volatile, which is exactly what the `__asm__ volatile` barrier in
these files is standing in for.

Somewhere in gcc-2.96's handling of `volatile` locals, the decision to allocate
memory and the decision not to fold the shift into its consumer are separable.
A later pass should start by reading that code path rather than by trying more
C.

## The list

| function | overlay | file |
|---|---|---|
| `OvlFunc_967_2008030` | rom_7f21b8 | `src/overlays/rom_7f21b8/ovl_30_a.c` |
| `OvlFunc_973_200804c` | rom_7fc720 | `src/overlays/rom_7fc720/ovl_30_c_a_c_a_a.c` |
| `OvlFunc_921_20085dc` | rom_7a7298 | `src/overlays/rom_7a7298/ovl_30_c_c_c_c_a_a.c` |
| `OvlFunc_908_20081a8` | rom_79c0c4 | `src/overlays/rom_79c0c4/ovl_30_c_c_c_a_a_a_c.c` |
| `OvlFunc_882_2008398` | rom_77dd1c | `src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_b.c` |
| `OvlFunc_882_20083cc` | rom_77dd1c | `src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_c_b.c` |
| `OvlFunc_882_2008400` | rom_77dd1c | `src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_c_c_b.c` |

All seven were previously **parked**, so this is a net reduction in unconverted
assembly, not new debt on functions that were already fine.

They also carry the same note in their headers, so the debt is visible at the
point of use and not only here.

## The 97 inherited ones

`fakematch.txt` lists 97 more, all of which arrived with the tree in a single
commit and none of which we wrote. They are a separate question and are not
covered by this worklist — but several use the same register-pinning idiom for
the same shape, so whatever resolves these should be tried on those too.
