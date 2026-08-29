# Batch 44 — eight functions

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`,
and every one sits at exactly the address its name claims.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `OvlFunc_888_200827c` | `0200827c` | ovl_7892c8 | [ovl_30_c_c_a_a_a_a_a_b.c](../src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_a_a_b.c) |
| `OvlFunc_926_200c1c4` | `0200c1c4` | ovl_7b2078 | [ovl_314_c_c_c_c_c_c_c_b.c](../src/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_c_b.c) |
| `OvlFunc_939_2008c10` | `02008c10` | ovl_7c460c | [ovl_314_a_c_c_a_c_b.c](../src/overlays/rom_7c460c/ovl_314_a_c_c_a_c_b.c) |
| `OvlFunc_948_20090b8` | `020090b8` | ovl_7d30e0 | [ovl_30_c_a_c_c_a_a_c_a_b.c](../src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_a_b.c) |
| `OvlFunc_955_20082c0` | `020082c0` | ovl_7ddb88 | [ovl_30_c_c_c_a_a_a_c_c.c](../src/overlays/rom_7ddb88/ovl_30_c_c_c_a_a_a_c_c.c) |
| `OvlFunc_964_2008fe8` | `02008fe8` | ovl_7ed0a0 | [ovl_30_a_a_c_c_a_a_b.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_c_c_a_a_b.c) |
| `Func_8079c8c` | `08079c8c` | main ROM | [rom_79460_c_c_c_c_a_c_c_a_b.c](../src/rom_77000/rom_79460_c_c_c_c_a_c_c_a_b.c) |
| `Func_809b648` | `0809b648` | main ROM | [rom_9ad70_c_c_b.c](../src/rom_8a000/rom_9ad70_c_c_b.c) |

Two of the eight are main-ROM rather than overlay code, which is worth noting
only because the batches before 40 were almost entirely overlay: the levers
below are not an overlay property.

## The findings, in the order they matter

### A third clause on the basic-block lever

Batch 43's `reports/arg-interleave.md` states the lever as two conditions, read
straight out of `update_equiv_regs` in `local-alloc.c`:

```c
if (REG_N_REFS (regno) == 2
    && REG_BASIC_BLOCK (regno) < 0
    && rtx_equal_p (XEXP (note, 0), SET_SRC (set)))
  reg_equiv_replace[regno] = 1;
```

**There is a third clause, and it is not visible in that code.** Every repeated
use of the value must be in a block *different from the assignment's own block*.
One use sitting in the assignment's block defeats the lever.

`OvlFunc_936_20095b4` is the case that shows it. It passes `0x80 << 2` to
`__GetFlag` in an `if` condition and to `__SetFlag` inside the body. Two
*separate* locals assigned above the `if` satisfy both printed conditions on
paper — and change nothing. CSE runs before local-alloc, merges the two pseudos
into one, and that single pseudo is then referenced three times, so
`REG_N_REFS == 2` fails on the merged register rather than on either original.

The functions where the lever *does* work — `OvlFunc_892_2008054`,
`OvlFunc_959_2008ce0`, and `OvlFunc_939_2008c10` in this batch — all have every
repeated use inside the conditional block, none in the assignment's block.

So the rule as it should be applied: **assign in a block that dominates the call
and contains none of the uses.** `936_20095b4` is parked with that reading.

### Whether a call result goes through a named variable decides its register

`OvlFunc_964_2008fe8` calls `__MapActor_GetActor` four times. The first three
assign to a local; the fourth is used **inline**, inside the expression that
reads `+0x10`. Assign that fourth one to the same local and gcc puts the joined
value in `r0` where the ROM has `r3`, and five positions differ. Nothing else
about the function changes.

This is a smaller sibling of the declaration lever: both are about controlling
which register a value lands in by changing how the source *names* it, not by
changing what it computes.

### The offset variable reused as the stored value — and a park corrected

`Func_809b648` needs one variable to do two jobs:

```c
off = 0x91 << 2;
p = g + off;
off = 0;
*(int *)p = off;
```

because the ROM overwrites the offset register with the value it is about to
store:

```
mov r3,#0x91 / lsl r3,#2 / add r2,r1,r3 / mov r3,#0 / str r3,[r2]
```

A separate `z = 0` gives six differing positions — gcc materialises the zero
before the address and the two registers swap.

**This matters beyond this function.** It is exactly the construct the
three-member indexed-store family had parked as *"the one that should work and
is the worst result"*. It is not wrong. It is right, and it is confirmed here on
a function that matches with it. What breaks in that family is that those
functions have **two** stores sharing one offset variable, so the reuse that
fixes the second store disturbs the first. `Func_809b648` has one store and no
such interaction.

All three park notes were updated, which narrows the next attempt from "find a
different construct" to "keep the reuse and stop it disturbing the first store".

That family's own body was also replaced, and **its score went up on purpose**:
3 of 30 → 5 of 30. The old body was 3 with one of the three a *wrong instruction
form*; the new one is 5 with all five the same instruction and two registers
swapped. Five is the better starting point — a wrong form says the C is not what
Camelot wrote, a register difference says the C is right and the allocator
disagreed. (Reusing one variable for both stores, the obvious reading of the ROM
since it reuses `r3` throughout, is 10: it disturbs the prologue.)

### Branch polarity: the ROM always says which arm falls through

`Func_8079c8c`. Written `if (r == 0) return 4; return r->f14;` gcc emits `bne`
and the two arms swap — three positions out. Written
`if (r != 0) return r->f14; return 4;` it emits the ROM's `beq`.

Not a new lever, but the first time it has been the *only* difference in a
function, so it is worth stating plainly: read the fall-through off the
reference before writing the condition.

### A repeated constant is only "shared" if the ROM reuses the register

`OvlFunc_955_20082c0` passes `0x20` as both its first argument and its `[sp]`
value. That looks like the shared-value case in the stack-arg-pair lever, where
one named local feeds both. It is not: the ROM builds `r0` fresh with its own
`mov` rather than reusing the stack register. So the argument stays a literal
and only the stack value is named. Same reading as `OvlFunc_935_2008410` in
batch 31.

## A ninth `.global`, and why the ordering was followed

`OvlFunc_926_200c1c4` could not be split out until one line was added to
`asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_c.s`:

```
	.global .L51d8
```

`tools/split_s.py` refused the split and diagnosed it: `.L51d8` is referenced
from the half above the cut and defined in the half below, and a `.L` symbol
does not survive into the object's symbol table, so the link would fail. The
tool also insists on an ordering, which was followed exactly — **export, verify
`make compare` is still green, and only then split** — so that the assembly edit
and the elevation stay separable in review.

A `.global` emits no bytes. **HANDOFF.md previously said eight such lines across
four `.s` files; it is now nine across five**, and the count has been corrected
there.

## The catalogue is now preventing work, not explaining it

`OvlFunc_923_2008f48` and `OvlFunc_946_2009494` were both skipped **without
being screened**. Each is straight-line and carries a pool load interleaved into
another argument's construction — the shape batch 42's reading of
`local-alloc.c` says is *unreachable in plain C*, because the rematerialisation
path requires `REG_BASIC_BLOCK (regno) < 0`, which a straight-line function
cannot produce.

That is the third and fourth time the settled mechanism has stopped work before
it started rather than explaining a failure afterwards. It is the main return on
batch 42: the population measurement there put **512 functions** in the
straight-line-blocker class, and every one of them is a screen not spent.

## Parked this batch

- **`OvlFunc_936_20095b4`** — the third-clause case above. Constant-CSE, but
  with a precise reason rather than a name.
- **`YesNoMenu2`** (`Func_80288a8`'s caller) — a **pool tell with no
  namespace**, and it is worth reading as a caution about the tell itself.

  The ROM loads `0x24` from the pool and keeps it in `r8` across three calls.
  `0x24` fits in an eight-bit `mov`, and gcc never pools what it can `mov`, so
  the operand was a **symbol** — the same tell that identified the area, message
  and file ids. As a literal the function is 16 of 34, because the whole
  prologue's allocation shifts with it.

  The park named reading `Func_80288a8` as the next step. **That read was done
  and did not settle it.** The fourth argument is stored as a **halfword** into
  a menu structure at `+0x92`, so `0x24` is a sixteen-bit UI field — 36 is a
  plausible pixel width, and a pixel width is not the sort of thing that is a
  symbol.

  So the pool tell and the semantics now point in opposite directions, and the
  park states both readings — including the possibility that **the tell has an
  exception for values STORED into a struct** rather than compared or dispatched
  on. Every confirmed instance so far has been the latter. Worth knowing,
  because the tell is load-bearing for three namespaces.

  `Func_80288a8` has exactly **one** caller in the whole ROM, so the parameter
  cannot be triangulated the way `__Func_8091f90`'s area id was from two
  elevated files. Only one other site pools `0x24` and it is unknown whether it
  is the same kind of value. Inventing a namespace for one function would be
  worse than leaving it parked — this tree has already been burned once by
  adopting names on thin evidence (see the area-id discussion in `HANDOFF.md`).

## One tooling change

`tools/tryc.py` gained a **fifth normalisation**: `ldrb r3, [r3]` and
`ldrb r3, [r3, #0]` are the same instruction — the zero-offset form is an alias
— and the ROM's disassembly writes one while gcc writes the other. Folded, the
same way the destructive Thumb form, immediates, literal pools and comma spacing
already are.

Only the bare single-register form is folded. `[r3, r2]` is a register offset
and a genuinely different instruction.

## One thing deliberately not done

`OvlFunc_882_2008030` uses `%` in an overlay, which needs
`__umodsi3 = _umodsi3_RAM;` in the linker script — exactly as batch 29 added for
division. **The alias was not added.** With the rest of that function still
unmatched it would be an unused linker change with nothing to justify it. Its
park note records the requirement and says to add it in the same commit that
solves the remaining difference.
