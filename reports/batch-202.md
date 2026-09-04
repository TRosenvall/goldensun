# Batch 202

Five elevated, from shape groups 0 and 2. Every candidate screened this round
matched on its **first screen**, and every one was pre-cleared by
`tools/crossed.py` before being read.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_891_20095d4` | `0x020095d4` | [ovl_30_…_a_a_a_b.c](src/overlays/rom_78c76c/ovl_30_c_c_a_c_c_c_c_a_a_a_b.c) |
| 2 | `OvlFunc_891_20095fc` | `0x020095fc` | [ovl_30_…_a_a_a_c.c](src/overlays/rom_78c76c/ovl_30_c_c_a_c_c_c_c_a_a_a_c.c) |
| 3 | `OvlFunc_954_2008158` | `0x02008158` | [ovl_30_c_c_a_a_a_c_a.c](src/overlays/rom_7db0c8/ovl_30_c_c_a_a_a_c_a.c) |
| 4 | `OvlFunc_936_2009ed8` | `0x02009ed8` | [ovl_30_…_c_c_a.c](src/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_c_c_a.c) |
| 5 | `OvlFunc_966_2008078` | `0x02008078` | [ovl_30_c_c_a_c_a.c](src/overlays/rom_7f148c/ovl_30_c_c_a_c_a.c) |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## The interleave, one argument wider

The `891` pair are the first functions in these batches to need **r3 pinned**.
Their four-argument call is the familiar shape with one more register on the
end:

    mov r1, #0xd0 / mov r2, #0xe0 / mov r0, #2 / lsl r1, #16 / lsl r2, #15 / mov r3, #0

`mov r0` inside the two shift builds, `mov r3` after both. Nothing about the fix
changes — pin all four, assign in the ROM's order. The two functions are the
same code with different constants and a different tail callee, written from one
template.

Their guard needed no lever at all: `if (call(...) != 0) tail();` produces the
ROM's `cmp r0, #0 / beq` over a single tail call directly.

## Two blockers wanting one fix

`OvlFunc_936_2009ed8` carries both at its `__MapActor_SetSpeed` sites:

    mov r0, #0x14 / ldr r1, =0x19999 / ldr r2, =0xcccc

That is the **precompute bind** — both pool loads exceed the `rtx_cost`
threshold, so gcc precomputes them and the cheap `mov r0` lands last — *and* a
**repeated pair** of pool constants, `0x19999` and `0xcccc`, at both call sites,
which gcc would otherwise load once and copy.

Pinning all three argument registers answers both at once: r0 is placed first,
and r1/r2 being call-clobbered forces the reload the ROM performs. Two separate
classes, one construct.

## The counter-case, and a tell that reads both ways

`OvlFunc_966_2008078` is worth more for what it did **not** need. The ROM holds
`0x80 << 9` in r5 across two stores to two separately-fetched actor pointers —
`mov r5, #0x80 / lsl r5, #9` once, then two stores — and an ordinary `int v`
reproduces that exactly.

That is the **opposite** of the rematerialisation cases this notebook has been
closing for ten batches. Here the ROM hoists too, so naming the value is right
and a pin would be wrong.

The discriminator is the ROM's own prologue. It pushes r5 and r6 because it
*intends* to hold values across calls. Where the ROM rebuilds instead, its
prologue is **narrower** than ours — that is the batch-190 marker, and the point
worth recording is that it reads in both directions: a wider ROM prologue is
permission to name a value, a narrower one is instruction to pin it.

## Method

`shape_groups.py` was re-run and its top three groups taken in order.
`crossed.py` cleared all six candidates screened; nothing was rejected this
round, which is itself informative after batch 201 rejected one of six.

No tool was written or changed.
