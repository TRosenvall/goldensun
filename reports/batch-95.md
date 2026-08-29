# Batch 95 — `pushal` is `push`, a load's operand order, and the discriminator I should have had five batches ago

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_801ef08` | `0801ef08` | main ROM | [rom_1de5c_c_c_c_c_a_a_a_c_b.c](../src/rom_15000/rom_1de5c_c_c_c_c_a_a_a_c_b.c) |
| `StartMenu_AddOption` | `080216e8` | main ROM | [rom_20198_c_c_c_a_a_c_a_b.c](../src/rom_15000/rom_20198_c_c_c_a_a_c_a_b.c) |
| `OvlFunc_956_2008714` | `02008714` | ovl_7e0928 | [ovl_30_c_a_c_c_b.c](../src/overlays/rom_7e0928/ovl_30_c_a_c_c_b.c) |
| `OvlFunc_953_2009c6c` | `02009c6c` | ovl_7d95dc | [ovl_30_c_c_c_c_a_b.c](../src/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_b.c) |
| `OvlFunc_common1_17c0` | `0200b058` | common1 | [common1_c_a_c_c_a_b.c](../src/overlays/common/common1_c_a_c_c_a_b.c) |

Two parked, and one of the parks carries the more useful result of the batch.

## `pushal` is `push`, and it was hiding a perfect match

`OvlFunc_common1_17c0` screened at **one differing instruction of thirty-five**,
and the instruction was the function's first:

```
	rom	pushal	{r5, r6, lr}
	ours	push	{r5, r6, lr}
```

There is no `pushal` macro anywhere in the tree, and `pushal` appears exactly
once in all of `asm/`. It is `push` with an `al` condition suffix — "always",
which is the default. Assembling both confirms it: `b560` either way.

So a byte-perfect function was reading as a near-miss because of one word in the
disassembly. `tools/tryc.py` now normalises it, with the assembler check written
into the comment so nobody has to re-derive it. This is the same class of bug as
the `ldrh`-versus-`ldr` normalisation that once cost eleven formulations and a
whole family: **the screen comparing text rather than encodings is a way to lose
functions that are already correct.**

## A register-offset load's operand order

`StartMenu_AddOption` came down to one instruction of forty-five:

```
	rom	ldrh r0, [r3, r2]      <- r3 is idx*2, r2 is the file base
	ours	ldrh r0, [r2, r3]
```

`LDRH Rd, [Rn, Rm]` computes `Rn + Rm` either way but encodes the registers in
fixed positions, so those are different bytes. Which you get is decided by how
the access is written:

```c
*(unsigned short *)(file + off)   ->  base first
((unsigned short *)file)[idx]     ->  scaled index first
```

Four spellings measured: `file + off` and `(int)file + off` give base-first;
`off + (int)file` and the array subscript give the ROM's index-first. The
subscript is what the file now uses, since it is also what the original plainly
meant and it removes the separate offset local entirely.

**A register-offset load whose first register holds a scaled index is a tell for
a subscript**, and the base-first form is what naive pointer arithmetic
produces. Worth checking before writing off a one-instruction residue as
allocation noise.

## Getting the branch back from a branchless constant choice

`Func_80b2ed8` is parked, but it defeated a blocker that has stopped several
functions before it.

It chooses between message ids 0xd2c and 0xd2d. Because they differ by one, the
plain `if (cond) id = 0xd2c; else id = 0xd2d;` gets if-converted — gcc emits the
`neg / orr / lsr #31` boolean-normalise idiom and subtracts, with no branch at
all, 26 differing of 46. The ROM branches.

Moving the **call** inside each arm blocks it:

```c
if (cond) s = Func_80b2884(0xd2c); else s = Func_80b2884(0xd2d);
```

gcc will not speculate a call, so the if-conversion cannot happen; it then
cross-jumps the two identical tails back into a single `bl`, which is exactly
the ROM's shape — two pool loads, a `b`, and one shared call. 26 differing to
19, and the whole control flow now matches.

**The rule: when the ROM branches over a choice of two constants that gcc
insists on making branchless, look for a call that can be moved inside the arms.
Cross-jumping puts it back.**

What is still missing there is one instruction — the ROM stages the constant
through a callee-saved register (`ldr r5, =0xd2c ... mov r0, r5 / bl`) where gcc
coalesces it straight into r0. Four spellings tried; that one is the allocator.

## The discriminator I should have had five batches ago

Batches 92–95 produced **six** file comments claiming that some register choice
in the ROM proved something about the source. **Four of them were false**, and I
found that out only because I started compiling the alternative before writing
the claim. This batch alone had two more:

* `OvlFunc_common1_17c0` writes zero to two fields from one register and returns
  a third zero from another. Named local, all literals, and all-one-local are
  the **same thirty-five instructions**.
* `OvlFunc_948_20099e8` last batch, `OvlFunc_964_200a52c` the batch before.

The ones that survived all have the same shape, and it is now written into
`docs/elevation.md` as a test I can apply instead of guessing:

> **Ask whether the spelling changes what has to be LIVE at some point.** If it
> does not, the assembly is showing you the allocator, not the source.

**Not evidence:** two stores sharing a register; a value sitting in a
callee-saved register; a constant provably constant inside its branch; which
operand of a commutative `and`/`orr` becomes the destination.

**Evidence:** a value that has to survive a **call** — `Func_801ef08` keeps a
zero in r10 across three calls, and spelled as a bare `0` it is 35 instructions
against 39, diverging at the first. And two **different** constants in two stack
slots at one call site, which need two registers at once.

`OvlFunc_953_2009c6c` shows both halves within one function, which is why it is
worth reading: its `__CopyMapTiles` call passes 2 and 2 and the ROM walks one
register through both stores (so: literals), while both `__Func_8010704` calls
pass two different values and the ROM builds each into its own register (so: two
named locals).

## The other park

`OvlFunc_954_200842c` sits at 9 differing of 43, all of it downstream of one
choice: the ROM shifts a loaded coordinate into a **different** register
(`asr r0, r3, #0x14`) where we shift in place. Six spellings tried, including
naming the loaded field first — the spelling that usually separates two values
into two registers — and all are byte-identical. Withholding the callee's
prototype makes it worse, which is the second function where the argument-move
rotation has a cause other than the declaration, consistent with batch 94
softening that rule.
