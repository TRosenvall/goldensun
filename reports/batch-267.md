# Batch 267 -- five functions, a park retired, and a correction to batch 266

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All five addresses checked
against the linked ELF, each at exactly the address its name encodes, plus both
rehomed `.rodata` labels at exactly their `.incrom` addresses. A control symbol
not in the batch was checked alongside and correctly came back absent.

| | |
|---|---|
| elevated | **5** |
| parks RETIRED by elevation | **1** |
| whole-file conversions | 2 |
| splits | 3 (one of them a text/data rehome) |
| build-input changes | 0 |
| fakematch debt added | 1 function |
| published claims corrected | **1 (batch 266)** |

Solo throughout, no agents.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `Func_8079c5c` | `0x08079c5c` | [rom_79460_…_a_a_b.c](src/rom_77000/rom_79460_c_c_c_c_a_c_c_a_a_b.c) |
| 2 | `Func_8079c30` | `0x08079c30` | [rom_79460_…_a_a_a_b.c](src/rom_77000/rom_79460_c_c_c_c_a_c_c_a_a_a_b.c) |
| 3 | `Func_809a7f4` | `0x0809a7f4` | [rom_9a44c_a_c_b.c](src/rom_8a000/rom_9a44c_a_c_b.c) *(fakematch)* |
| 4 | `Func_809a738` | `0x0809a738` | [rom_9a44c_a_c_a.c](src/rom_8a000/rom_9a44c_a_c_a.c) |
| 5 | `Func_8005c68` | `0x08005c68` | [rom_56cc_c_c_a.c](src/rom_c0/rom_56cc_c_c_a.c) |

Two were exact on the **first** candidate (`Func_8079c30`, `Func_809a738`), both
immediately after their twin had paid for the finding.

## I corrected batch 266, and it was my own published claim

Batch 266 said four functions all ended on a local-alloc decision, citing
`;; 0 regs to allocate` in `.18.greg`. **That line was measured on one of them
and assumed for the rest.** Measured on all five:

```
Func_942e0    ;; 0 regs to allocate                        local-alloc
Func_8028ef0  ;; 0 regs to allocate                        local-alloc
Func_80a8578  ;; 5 regs to allocate: 37 33 36 35 32        global_alloc
Func_80cd52c  ;; 7 regs to allocate: 37 41 33 32 34 35 36  global_alloc
Func_80919d8  ;; 5 regs to allocate: 35 34 50 32 33        global_alloc
```

Three of five are **global_alloc**, so batch 266's "read local-alloc.c" pointed
at the wrong file for those three. Worse, `Func_8028ef0` -- the one offered as
the cleanest specimen -- has its two named values already in the ROM's
registers (r10, r8) per `.17.lreg`; its residue is argument setup and a reload
scratch, **not an allocator quantity at all**.

Corrected in four park files, in reports/batch-266.md (rewritten in place so the
wrong lead stays visible beside its correction), and in the HANDOFF row.

**The read was still worth doing.** It produced a real correction to the
notebook: **local-alloc runs two passes and the published priority formula is
only the second.** `block_alloc` sorts by `qty_sugg_compare` and allocates every
quantity carrying a hard-register suggestion first; those never reach the
formula. A copy between a hard register and a pseudo creates that suggestion
(`combine_regs`), so **every parameter, argument set-up and return value** is a
suggested quantity. The formula's only tie-break is qty number, assigned by
order of first appearance in the block scan.

**The working rule:** `.18.greg`'s "N regs to allocate" names every global
allocno; `.17.lreg`'s `;; Register N in M.` names every quantity local-alloc
placed. If the register under argument is in neither, it belongs to reload or to
argument set-up, and no amount of re-spelling locals will move it.

## A park that concluded "unreachable" was wrong, and how it was wrong is the result

`rom_77000/rom_79c30.c` (batch 56) diagnosed its blocker correctly -- Thumb
`mul rd, rm` is destructive, so the operand gcc puts first becomes the
destination -- measured four spellings at 4 of 19 each, and closed with:

> "NEXT: nothing at the expression level. This wants either a gcc flag that
> disables commutative canonicalisation -- none is known -- or ... a compiler
> difference rather than a source one."

It is a source one. The four spellings were `c * (a * f())`, the same with the
call named, the same with the inner product also named, and `a *= t; c *= a;`.
**Three of the four are the same expression tree**, and all four put the call's
result on the RIGHT of the inner multiply. On the LEFT of a flat left-to-right
product it is exact on the first try:

    r = Func_8079b24(b, 0);
    return r * a * c / 0x10000;

**The lesson is about the sweep, not the multiply.** Varying how much is NAMED
and how it is BRACKETED changes neither the expression tree nor the operand
order. Before concluding "the source cannot control this", check that the probes
actually differ in the dimension being blamed.

## Findings

**`.call_via` is `include/math.h`'s macro, not gcc's veneer.** The tree has two
indirect-call sequences: `bl _call_via_rN` (what a C function pointer emits) and
`mov r12, pc / bx r5` (math.h's `fx32_multiply`, inline asm). **The relocations
tell them apart** -- the veneer carries an `R_ARM_THM_CALL`, the macro carries
none -- and `objcmp` printed that on the first screen. Do NOT grep for
`call_via` to distinguish them: the substring matches inside `bl _call_via_r3`,
and that scan reported 20 generated files as using the macro when none did.

**Compute both products before either store.** Two self-contained statements let
gcc store the first sum right after the first call and never spend a third
callee-saved register; the ROM carries the first product across the second call.
Worth 62 differing to 5. **The prologue push is the cheap tell for the whole
class: a register you do not spend is a value the ROM is carrying that your
reading has not found.**

**`DMA3_CLEAR` vs `DMA3_SET`: who owns the zero word.** `DMA3_CLEAR` declares its
own `u32 value;` inside the macro, so gcc writes it through `sp` and never holds
its address. A ROM that hoists that address into a callee-saved register came
from `DMA3_SET` with a caller-owned local instead. Switching macros alone is 63
of 65; adding the **pointer** (`q = &value;` hoisted, `*q = 0;`) is exact.

**A halfword compare shifts BOTH sides left 16.** `mov r1, #0xca / lsl r1, #15`
against `lsl r3, #16` is a comparison with **0x65**, not 0x650000.

**One `short` field, two load forms.** `ldrsh` for a `!= 0` test and `ldrh` for
the decrement is one field: the subtract's result is truncated by the `strh`, so
gcc takes the cheaper load there.

## Process error: I deleted a `.s` that carried data

`Func_8005c68`'s `.s` carried a `.rodata` section (`.global .L79b0` / `.L79b8`,
92 bytes of string literals) alongside the function, and I deleted it with the
function. **Neither `tryc` nor `objcmp` can see this** -- they compare one
function -- and both reported a clean exact match. The linker caught it:

    (rom_1b70+0x3c24): undefined reference to `.L79b0'

The stage1.ld grep I ran to check the `.text` line **printed the `.rodata` line
directly beneath it** and I did not act on it. Reading the stem's linker lines is
not the check; grepping the `.s` is.

Fixed by rehoming rather than emitting from C -- four string literals totalling
84 bytes is past batch 265's "a handful of words you can read" threshold, and
`.incrom` keeps the bytes exact by construction. Both labels now link at
`0x080079b0` and `0x080079b8`, their original addresses, which is the check that
the rehome is byte-neutral.

A failed landing also leaves an orphan: the first build generated `asm/<stem>.s`
from the `.c` at its original name, and after the `.c` moved to the `_a` stem
that file was left behind and committed. Removed in a follow-up.

## Parks: one retired, one sharpened

`HeightTile_A` is not closed but is now diagnosed to the pseudo. `.18.greg`
reports 6 global allocnos and **neither** swapped value is among them;
`.15.regmove` names them `reg 52 = t - 8` and `reg 53 = c - b`, both local
quantities whose priority ties, leaving qty number as the only tie-break. Five
spellings that genuinely reorder them -- RTL-confirmed -- all measure unchanged,
because the load heading `c` is scheduled ahead of the arm regardless of where
the statement sits. **Statement order is exhausted there** and the park says so.

## State

```
funcindex --stats   4353 from C, 1357 still in asm     (batch 266: 4348 / 1362)
tools/census.py     TOTAL 1355  (76 hand-asm, 14 ARM, 591 parked, 674 available)
matched .c files in src/ (excl. parks): 3987
park files: 472
```

**+5 from C and -5 still in asm, for exactly the five elevated** -- the second
consecutive batch where the delta reconciles. Batch 264's figure remains
unexplained and that entry stays open.

## Not done

* The three global_alloc parks now want `global.c` -- `allocno_compare`, the
  conflict graph, `find_reg` -- not local-alloc.c. Nobody has read it yet.
* `Func_8028ef0`'s residue is reload scratch selection plus argument-setup order,
  a class with no recorded levers at all.
* The remaining cold `.call_via` functions are all 115+ instructions;
  `Func_80123f4` is the smallest and the idioms now transfer.
* `8021390`'s `_MSG_1b` build-input question and
  `OvlFunc_968_200c048`/`200c520` behind `200c2bc`'s floor, both still held.
