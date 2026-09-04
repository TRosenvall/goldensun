# Batch 201

Five elevated. With `arg_interleave_flat` closed in batch 200, the shape-group
method was re-run to find what had moved to the top of the list — and the answer
was a six-member group with the same blocker.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_959_2008c78` | `0x02008c78` | [ovl_9dc_a_c_c_a_a_a_a_b.c](src/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_a_a_b.c) |
| 2 | `OvlFunc_905_20089dc` | `0x020089dc` | [ovl_30_a_a_a_c_c_c_c_a_b.c](src/overlays/rom_799abc/ovl_30_a_a_a_c_c_c_c_a_b.c) |
| 3 | `OvlFunc_909_2009984` | `0x02009984` | [ovl_30_…_c_c_b.c](src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_c_b.c) |
| 4 | `OvlFunc_922_2009ad0` | `0x02009ad0` | [ovl_30_…_c_a_b.c](src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_c_c_a_b.c) |
| 5 | `OvlFunc_926_200a68c` | `0x0200a68c` | [ovl_314_c_c_c_c_c_a_a.c](src/overlays/rom_7b2078/ovl_314_c_c_c_c_c_a_a.c) |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## The method, re-run

`tools/shape_groups.py` already existed — it is the tool the closed class was
found with, and its docstring still names that class as the cautionary example
of a group that is large *because* it is blocked. Re-running it after the class
was cleared puts a new group first:

    group 0    6 members    flat, opcodes = {bl, bx, lsl, mov, pop, push}

That is the closed class's signature **minus `ldr`** — the same interleave with
no pool loads anywhere. Five of the six matched on the first screen, using the
fix the closed class established: pin the argument registers, assign them in the
ROM's own order.

**The sixth was rejected before any screen ran.** `tools/crossed.py` flags
`OvlFunc_933_2009874`: its first call crosses `mov` order against shift order,
which is the batch-195 wall. Reading its listing by hand had suggested the same
thing, and the tool confirmed it in one command.

So group 0 is 5 elevated, 1 genuinely blocked — a much better ratio than the
14-member group, and the tool's own warning is worth restating: a large group can
be large because it is blocked, and the only way to know is to screen one.

## Argument order is not uniform within a single function

`OvlFunc_922_2009ad0` calls `__MapActor_SetAnim` twice, and the ROM fills its
arguments in **opposite orders**:

    mov r1, #7 / mov r0, #0        <- reversed, needs a pin
    mov r0, #0 / mov r1, #6        <- natural, written plainly

Four of its six calls need a pin and two do not. This is why each call gets read
off the listing separately rather than being made consistent with its
neighbours — pinning the second `SetAnim` would break it, which is the failure
measured in batch 195.

## A twin pair

`OvlFunc_922_2009ad0` and `OvlFunc_926_200a68c` are **instruction-for-instruction
identical** — same calls, same constants, same argument orders — in two
different overlays. One body was written; the other is the same text with the
name changed. Both matched on the first screen.

Also of note: `OvlFunc_959_2008c78` is ten instructions, the smallest function
elevated in these batches.

## Nothing new was built

`shape_groups.py` and `crossed.py` both already existed and both did their jobs
unmodified. The round's only judgement calls were which group to take and which
member to skip.
