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

### …and it is a dead end. Corrected by reading the compiler.

The gcc-2.96 **source** is in the build image, at
`/opt/camelot-gcc/gcc-2.96/gcc/`. `expand_decl` in `stmt.c` decides whether a
local gets a register or a stack slot:

    else if (DECL_MODE (decl) != BLKmode
             && !(flag_float_store && TREE_CODE (type) == REAL_TYPE)
             && ! TREE_THIS_VOLATILE (decl)          <-- here
             && ! TREE_ADDRESSABLE (decl)
             && (DECL_REGISTER (decl) || optimize)
             && ! current_function_check_memory_usage)
      { /* Automatic variable that can go in a register. */ }

**A `volatile` local never gets a register.** It is forced to memory at
declaration expansion, before any optimisation pass runs. So the two effects are
not separable: the ordering we saw is a *consequence* of the operand being a
`MEM`, not an independent scheduling property.

I wrote this section as an open lead before checking. It is not one. The
question a later pass should ask instead is the one the whole class actually
turns on:

> **What makes gcc-2.96 rematerialise a register-allocated constant inside a
> single basic block, rather than computing it once?**

That is the same question `OvlFunc_882_200c5b8` and the `-1` triple in
`src/non_matching/ovl_787e04/20093e4.c` turn on, and it is now answerable by
reading `local-alloc.c` and `reload1.c` rather than by probing.

## SETTLED, batch 42: this is not a search problem

The worklist above assumed a construct existed and had not been found. **It does
not exist.**

gcc-2.96 rebuilds a constant at its use instead of keeping it live in exactly
one place, `update_equiv_regs` in `local-alloc.c`, and only when both hold:

    REG_N_REFS (regno) == 2        set once, used exactly once
    REG_BASIC_BLOCK (regno) < 0    the pseudo spans MORE THAN ONE basic block

The second condition is a property of the control-flow graph, not of the source.
A straight-line function has one basic block, so no C can satisfy it. The only
other pass that could rebuild the value is `combine`, and combine folds a
constant into its consumer only when the target has an instruction taking it as
an immediate — which a constant needing two instructions does not.

**So the fakematches below are not a debt that better C will pay off.** They are
the only way to reach these functions in this compiler, and a future pass should
either accept them or accept the assembly. What a future pass CAN still do is
check the reverse: whether the original build used a compiler whose
`update_equiv_regs` had different conditions, which would make these functions
evidence of a toolchain difference rather than of anything about the source.

That also settles three parks that were filed as sharing "one missing
construct": `src/non_matching/ovl_77dd1c/200c5b8.c`,
`src/non_matching/ovl_7c7b9c/200c218.c`, and the `-1` triple in
`src/non_matching/ovl_787e04/20093e4.c`.

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
