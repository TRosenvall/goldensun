# Batch 213

Five elevated across two rounds, three parked. The batch's result is a
**diagnosis method**: an unexplained callee-saved register is worth one
`xgcc -S` and a grep, and that beats guessing at register pressure.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_943_2009b58` | `0x02009b58` | [ovl_30_…_c_a_b.c](src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_b.c) |
| 2 | `OvlFunc_945_200c670` | `0x0200c670` | [ovl_30_…_a_c.c](src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_c_a_c.c) |
| 3 | `OvlFunc_917_2008158` | `0x02008158` | [ovl_30_c_c_a_c.c](src/overlays/rom_7a4370/ovl_30_c_c_a_c.c) |
| 4 | `OvlFunc_899_200cb2c` | `0x0200cb2c` | [ovl_30_…_c_c_b.c](src/overlays/rom_794ac0/ovl_30_c_c_c_c_c_c_c_b.c) |
| 5 | `OvlFunc_895_2008420` | `0x02008420` | [ovl_30_c_c_a_c_a.c](src/overlays/rom_78dee8/ovl_30_c_c_a_c_a.c) |

Parked: `OvlFunc_923_2009df8` (29 of 89), `OvlFunc_936_200b768` (66 of 99),
`OvlFunc_968_2009808` (23 of 94), `OvlFunc_943_200ab7c` (2 of 97).

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## THE SPILL WAS gcc COMMONING A ZERO

`2009df8` was parked last round on "it takes one more callee-saved register than
the ROM", with the next step written down as *read the generated `.s`*. Doing
that took five minutes and answered it:

    ldrh r3, .L7 / mov r8, r3 / ... / mov r1, r8 / ... / mov r2, r8

gcc fetched a **halfword zero from the pool**, cached it in r8, and fed it to a
byte store, a halfword store, and a third byte store in a later block. Holding
it across two calls is what forced the high register; the `mov r7, r8 / push {r7}`
pair and its teardown are the six-instruction excess.

Naming the zero for the adjacent pair alone removes it entirely — 100 lines to
88, 94 differing to 29. Torn down: drop the name and the spill comes straight
back. **The lesson is about diagnosis, not the lever.** Two rounds of reasoning
about pressure produced a wrong cause; one compile and a grep for the register
produced the right one.

The same function then gave up two more: the table index wants
`off = ...; off <<= 2; off += 0x14;` as statements, because the ROM adds the
offset *into* the scaled index and uses a register-offset load; and its store
pair needs both registers pinned. It sits at 29 of 89 on one remaining defect —
a zero the ROM takes from the pool where we emit an immediate — plus that
defect's displacement.

## AN EXPORT STEP THE SPLIT TOOL DOES NOT DO

`200cb2c` reads a four-byte cell declared `.lcomm .L64f8, 4`. The split put that
cell in a different object from its two references, and **the link failed before
any `.c` was written** — `undefined reference to .L64f8`, because a `.lcomm`
symbol is local to its object.

Adding `.global` beside the `.lcomm` fixes it and is byte-neutral: the cell is
`.bss` and contributes nothing to the ROM, so only the symbol table changes, and
`make compare` is green again before the `.c` goes in.

The tree already depends on this having been done elsewhere — an existing file
notes that *its* cell was "already `.global` in [another file], so no export step
was needed", which says the step exists and is sometimes owed. **Check for
`.lcomm` references crossing a split boundary.**

## THE BARRIER-FREE CROSSED CURE IS NOT A SPECIAL CASE

Batch 212 found that writing the shifts in the movs' order closes a crossed fill
without a volatile asm. This batch closed **four more sites** that way, including
a **four-register** fill in `200cb2c` (movs r0–r3 against shifts r3, r0, r1, r2)
— the widest crossing met so far. There is still no case where the reordering was
tried and a barrier was afterwards required.

## SMALLER, ALL MEASURED

**Two variables can share one pinned register when the types differ.** `2008158`
reuses r3 as the iwram base and then as the stored value. Where an earlier
function reused a single C variable for two roles, this one can't — a pointer and
an int — so it takes two `register` declarations naming the same register.

**A value stored after a call must be built after it.** `2009b58` builds a
halfword in a call-clobbered register; written as statements ahead of the store
gcc keeps it across the `bl` and the prologue widens, written inline it goes to
the pool. Naming both operands with the pointer pinned to the return register is
what reaches it — and the pin is safe because it's assigned *from* the call.

**Four blocks of the same shape are still not a loop.** `200c670`'s four guarded
blocks differ in which carry an extra field store, and its four tail calls fill
their registers two different ways. Fifth batch running.

## THE PARKS THAT REMAIN

`200ab7c` sits at 2 of 97 on something C cannot spell: gcc splits a global access
across a store, hoisting the address materialisation above it while the
dereference stays below. Those are one C expression with no boundary between
them, and a `do { } while (0)` is **exactly one instruction too strong**.

`2009808` is parked on where `sub sp` lands — the third function in that class.
Its TU builds at **-O1** through a mis-scoped wildcard; checking the flags before
screening is what kept the residue readable, and a plain `-O2` screen would have
sent the work the wrong way.
