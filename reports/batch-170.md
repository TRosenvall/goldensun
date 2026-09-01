# Batch 170

Five elevated. Verified after a clean `make clean && make compare`; SHA1
`5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked images.

## The functions

| function | address | image | ratio | what closed it |
| --- | --- | --- | --- | --- |
| `Func_80a8b8c` | 0x080a8b8c | goldensun.elf | 0.823 | first screen, no iteration |
| `Func_80a5578` | 0x080a5578 | goldensun.elf | 0.763 | first screen, no iteration |
| `Func_80a9b94` | 0x080a9b94 | goldensun.elf | 0.765 | loop initialiser order |
| `OvlFunc_947_20091c4` | 0x020091c4 | rom_7d0e88 | 0.761 | matched at 1 differing, that one a linker alias |
| `Func_80a1cb0` | 0x080a1cb0 | goldensun.elf | 0.701 | named the global read |

All five came off `tools/fuzzy_solved.py`. Three needed no iteration.

`Func_80a8b8c` and `Func_80a5578` are two more members of the scroll-descriptor
family that share the exemplar `Func_80a6a00` — three elevated across two
rounds, each differing from the exemplar by one guard or one call.

## A global read must be NAMED to keep its load above a following branch

`Func_80a1cb0` selects a constant with an `if`, then walks a list based at a
global. The ROM loads *and dereferences* the global before the branch and keeps
the value across it:

    ldr r3, =0x3001f2c / mov r2, #0x38 / ldr r3, [r3] / mov r8, r2
    cmp r0, #1 / beq L0 / mov r2, #0x28 / mov r8, r2
    L0: mov r5, r3 / add r5, #0x48

Written with the global used only after the branch, gcc sinks the whole load
below it — 12 differing at exact length. Reading it into a named local first,
and forming the pointer afterwards, matches. The name pins the load above the
branch; the `+ 0x48` still happens after it, as the ROM has it.

> **This is the read counterpart of the address-local birth rule.** For a store
> the question is where the address is COMPUTED; for a global read it is where
> the value is FETCHED, and a branch between fetch and use makes it observable.

## Loop initialiser order, confirmed outside a `goto` loop

Batch 168 found on `Func_80aac84` that once a loop's initialisers are both
visible their ORDER is observable. `Func_80a9b94` confirms it on a plain
`do`/`while` with no rewrite involved:

    rom    mov r5, #0x0 / add r6, #0x48        counter first
    ours   add r6, #0x48 / mov r5, #0x0        pointer first

Swapping the two source statements is the whole difference between 2 differing
and an exact match. **When a diff is two lines and both are loop setup, try the
other order before anything else.**

## Two functions, one exemplar, two-line residues pointing OPPOSITE ways

`OvlFunc_928_2008d0c` and `OvlFunc_957_2008de8` are both variants of the solved
`OvlFunc_946_2009a44`, both come out at the ROM's exact length, and both sit at
exactly 2 differing — on the placement of one constant build relative to a
neighbouring memory operation. And they want opposite things: `2008d0c` wants
the constant EARLIER than gcc puts it, `2008de8` wants it LATER.

Six spellings were measured between them — operands swapped in the sum, the
constant named, the mask named, the expression inlined into the call argument,
the halfword read named first, and the whole statement moved above the vector
stores. Every one is inert or worse, in both functions.

**The pairing is the finding.** If a source construct controlled where an
independent constant build lands, one of the two would have yielded to it. So
this is the scheduler, not a missing lever, and a two-line residue of this shape
is worth one probe and then a park.

`-fno-schedule-insns2` on `2008d0c` shows the recorded "destroying the evidence"
signature exactly — first difference jumps to instruction 3, count multiplies to
21 — which is batch 168's rule doing its job.

Worth separating: `2008d0c`'s exemplar contains the *identical* angle expression
and matched with it. The spelling is not the variable; the surrounding register
pressure is.

## What the lead tags are actually filtering

At ratio >= 0.70 there are 26 leads and exactly **one** is fully clean; the rest
carry DUP-CONST, FAKEMATCH, or both. That is the tags earning their keep rather
than the pool being empty — dropping the threshold to 0.66 surfaced five more
clean leads, one of which is elevated here.

> **When the clean list empties, lower the ratio before changing method.** A
> clean 0.70 lead has been more productive than a flagged 0.90 one.

## Parks

Four, all carrying full measurement tables:
`ovl_7b6668/2008d0c.c` and `ovl_7e3e08/2008de8.c` (the paired scheduling
finding above), `ovl_77dd1c/2009498.c` (47 of 53 — the ROM spends r8 on a third
stack-argument constant and the C cannot demand it), and `rom_b5000/80c1014.c`
(31 of 33 — the ROM copies a buffer address into a second callee-saved register
inside a guard; three spellings, including one where the call argument and the
loop base share no variable at all, all produce one address and never the copy).

Those last two are the same wall stated twice: **the C has no way to demand a
particular callee-saved register, or a redundant copy into one.**
