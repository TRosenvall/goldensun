# Batch 157 — five elevated, and two filter rejects that were hiding the corpus

Clean `make clean && make compare` green from scratch. All five addresses
checked against the linked ELF.

| function | address | file |
|---|---|---|
| `Sprite_DeleteLayer` | `0x0800b93c` | `src/rom_9000/rom_b798_c_a_a_c_b.c` |
| `Func_800eba0` | `0x0800eba0` | `src/rom_9000/rom_ea54_c_c_b.c` |
| `Func_8011b00` | `0x08011b00` | `src/rom_9000/rom_11568_c_c_c_c_a_a_b.c` |
| `Func_80b9acc` | `0x080b9acc` | `src/rom_b5000/rom_b8228_c_a_c_c_c_c.c` |
| `Func_80cd418` | `0x080cd418` | `src/rom_c9000/rom_cd260_a_b.c` |

## The headline: two rejects were deleting classes from view

**`r8-r11` is not a wall.** `pickable.py` rejected any function touching a high
register, citing three named functions. Measured over the matching corpus: 232
translation units use one, of which **202 are genuine matches** and only 30 are
fakematches. Removing the reject took the filter from **5 candidates to 125**.

It also explains why the duplicate-group backlog never moved: all ten unparked
groups use high registers, so the 72 functions that follow from 25 group
solutions were invisible to the filter rather than hard.

**This was the third such reject.** "Fewer than 5 calls" previously hid the
loop-body class that produced four elevations. The check is one command — does
matching code already do the forbidden thing? — and it is now the rule before
trusting any reject in the doc.

## A process failure, also the third instance

A hand-written scan re-offered `Task_SpinCamera`, already parked at exactly the
residue it was re-derived to, with the same lever already written down. I
overwrote that park; `git status` showing `M` rather than `??` caught it and it
was restored unchanged.

Park files are named by ADDRESS; the ad-hoc filter matched the trailing
component of the SYMBOL NAME. **134 functions in `asm/` have an address
matching a park file.** `pickable.parked()` already handles this — filenames
and park contents both — and its docstring records two earlier rounds lost the
same way. Rule now in the doc: a throwaway scan is still a candidate filter;
import `parked()` rather than re-deriving exclusion.

## A new pre-screen: unreachable copies

Three functions stalled on the same shape — a ROM `mov rX, rY` where the copy
never diverges from its source, which no C local can produce because gcc
coalesces it (`Func_80a8b10`, `Func_80e38b8`, `HeightTile_B`).

That is mechanically detectable: count `mov rX, rY` where rY is never written
again. Of 51 candidates only 17 have none. It would have excluded both
functions that blocked the previous round, and the first zero-call candidate it
ranked matched in two screens (`Func_80cd418`).

## Levers

**Both directions of address/offset naming exist.** Naming the OFFSET restores
register-offset addressing `[base, index]` (`Func_8011b00`,
`Sprite_DeleteLayer`); naming the ADDRESS forces a separate `add`
(`Func_808bc44`). The ROM decides which is wanted.

**A single `bhi` does not make the variable unsigned.** On `HeightTile_A`,
declaring the parameter unsigned made the products unsigned too, so `/ 8`
became `lsr #3` and both four-instruction sign-correction sequences vanished —
the entire eight-line shortfall. `int` with the guard written
`(unsigned int)t <= 7` gives the unsigned compare AND the signed divisions.
Cast the comparison, not the declaration.

**Commutative pairs come out reversed.** `dx*dx + dy*dy + dz*dz` produces the
ROM's `dy` first; the same inversion appeared on `HeightTile_A`'s first
multiply. When a commutative pair is swapped, rewrite it rather than reaching
for a lever — it closed `Func_800eba0` in one edit.

**The offset declared INSIDE the loop body** moves the preheader below the
guard (`Sprite_DeleteLayer`). It does not stop `strength_reduce` CREATING an
induction pointer (`Func_80c1f50`) — different passes.

**gcc normalises a dead-counter loop to count down.** So a count-up loop in the
ROM whose counter is unused in the body means the counter was LIVE in the
original source (`Func_801e3c8`).

**Sibling levers transfer; allocation does not.** The `HeightTile` family is
three-for-three on register assignment — `A` at 4 of 39, `B` at 34 of 37, `4`
at 22 of 28 — every one with the interpolation, signed divisions and branch
structure exact, differing only in which register holds which local.

## The dominant blocker, stated plainly

Register assignment. Across this batch, spelling failed to reach it on
`Func_80f4100`, `Func_8029274`, `Func_80c0228`, `DecodeMetatileset`,
`Func_8079664`, `Func_80ae9f0`, `Func_8011fd8`, `Func_808bc44` and the three
`HeightTile` siblings. Where an addressing-mode or in-place-modify difference
sits DOWNSTREAM of a wrong register assignment, spelling the addressing mode
does not fix the rotation and usually costs lines.
