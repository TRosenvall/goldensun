# Batch 68 — a second form of the pool tell, and the levers that are about operand order

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `Func_801ee68` | `0801ee68` | [rom_1de5c_c_c_c_c_a_a_a_b.c](../src/rom_15000/rom_1de5c_c_c_c_c_a_a_a_b.c) |
| `Func_80f7db4` | `080f7db4` | [rom_f6008_c_b.c](../src/rom_f6000/rom_f6008_c_b.c) |
| `Func_80a17c4` | `080a17c4` | [rom_a172c_a_c_c.c](../src/rom_a1000/rom_a172c_a_c_c.c) |
| `Func_80b2884` | `080b2884` | [rom_b0070_c_c_a_a_b.c](../src/rom_b0000/rom_b0070_c_c_a_a_b.c) |
| `Func_8012038` | `08012038` | [rom_11ce0_c_a_b.c](../src/rom_9000/rom_11ce0_c_a_b.c) |
| `Func_8012078` | `08012078` | [rom_11ce0_c_a_b.c](../src/rom_9000/rom_11ce0_c_a_b.c) |
| `Func_801a7c0` | `0801a7c0` | [rom_1a66c_a_b.c](../src/rom_15000/rom_1a66c_a_b.c) |

## The pool tell has a second form

The tell as recorded until now: gcc-2.96 never pools a constant it can build
with `mov #imm8`, so a pooled small value was a **symbol**. `Func_80b2884`
showed a different shape of the same thing. Three times it spends four
instructions to add ten:

```
ldr r3, =0xd2e / ldr r2, =0xd24 / sub r3, r2 / add r0, r3
```

**No compiler leaves `0xd2e - 0xd24` unfolded.** Both operands were symbols.

What makes this form worth naming separately is how it fails. Written as
literals, gcc folds the difference and emits `add r0, #0xa` — so our stream came
out **eight instructions shorter than the ROM's**. A stream that is short around
some arithmetic reads like an optimiser-proved-it floor, which is a blocker
class we do have and cannot fix. Here it was the opposite: information missing
from the C, and recoverable.

### Finding the namespace: follow the result, not the value

`0xd24`, `0xd2e`, `0xd38`, `0xd42` were in none of the four `.sym` files, so
the park deliberately stopped short of naming them and recorded where the
evidence would be: *"this function only adds the result to its argument and
returns it; the caller is where the evidence would be."*

It was. Three lines converged:

1. The already-elevated caller
   [rom_b0070_c_c_a_b.c](../src/rom_b0000/rom_b0070_c_c_a_b.c) passes the result
   to `_Func_8017658`'s **first argument**.
2. Elevated code elsewhere
   ([rom_1de5c_…_b.c:28](../src/rom_15000/rom_1de5c_c_c_c_c_a_a_c_c_a_b.c))
   passes `(int)&_MSG_14` in that same position.
3. `_MSG_d21` was **already defined** in `message.sym` — one of the same run.

Five ids added on that basis: `_MSG_d24 d2d d2e d38 d42`.

This is the consumer rule doing the work it exists for. 95 small values collide
across the four `.sym` files; a value-only lookup would have had nothing to go
on here, since none of the four values existed anywhere. The **consumer** is
what identifies the namespace, and the consumer was one call away.

## Four operand-order levers

None of these change what the C means. All four decided whether the output
matched.

**A named index local forces register-offset addressing.** Written inline,
`ctx + (layer & 3) * 0x30 + 0x130` gets reassociated by gcc into
`(ctx + index) + 0x130` — two adds and a `ldr r2, [r3]`. Naming the whole index
keeps it in one register and produces the ROM's `ldr r2, [r0, r3]`.

**`x + (y << 7)`, not `(y << 7) + x`.** The ROM's three-operand
`add r3, r1, r3` wants the shifted term second. Swapping the source operands
gives the two-operand `add r3, r1` instead.

**`p += …; return p[2];`, not one indexing expression.** The pointer-typed
operand decides which register the sum lands in: `add r2, r3` (into the pointer)
versus `add r3, r2` (into the index).

**A named temporary can decide the allocation — the reverse of the usual
advice.** In `Func_80b2884`, `d = X - Y; base += d;` puts the two pooled loads
in the opposite registers from the ROM (12 of 30). `base += X - Y;` matches
exactly. Naming a subexpression is normally the safe move; here it was the
difference between a match and a park.

## Describing the layout beats writing the offsets

`Func_801a7c0` appends a pair to two parallel sixteen-entry `short` arrays.
Written with raw offsets:

```c
*(unsigned short *)(blk + (0x354 + n)) = a;
```

gcc reassociates to `(blk + n) + 0x354`, folds the index into the base, rewrites
the pointer for each store and needs an extra callee-saved register — 25 lines,
`r5` pushed.

Declared as two `short [16]` members at their real offsets inside a struct, gcc
keeps the block pointer as the base, computes `n * 2` **once** and adds each
member's own constant to it. That is the ROM's `mov r12, r3` and its
register-offset `strh r0, [r2, r3]`, and it pushes only `lr`.

Same addresses, same semantics; the struct tells gcc which part is the base and
the arithmetic does not.

A detail worth not mistaking for sloppiness: the trailing `p->n = p->n + 1`
**reloads** the count rather than reusing the value already in hand. That is
correct — the two `short` stores may alias the `unsigned short` count, so
gcc-2.96 has to re-read it. The ROM re-reads it too.

## Two facts about this compiler, recorded because they look like bugs

**It writes r4 without saving it.** All three functions in the last round do,
and so does our output. That is this compiler's behaviour, not a disassembly
artifact — worth knowing before treating a missing push as evidence that a
function was hand-written.

**`strh r2, [r2]` on `REG_IME` is deliberate.** It writes `0x0208`, the low half
of the register's own address. Only bit 0 of `REG_IME` is live and `0x208` has
it clear, so this disables interrupts exactly like a zero would, one instruction
cheaper — no `mov r3, #0` is needed because the address is already in hand.
`*ime = (unsigned int)ime;` reproduces it. `REG_IME = 0;` does not, and
`REG_IME = REG_ADDR_IME;` does not either: gcc materialises the truncated
constant from a fresh literal rather than reusing the address register.

## REG_ALLOC_ORDER: the largest park class now has a named cause

The census in this batch put register allocation at the top — 37 of 177 parks —
with the recurring detail that the ROM reaches for r4–r6 where gcc uses r0–r3.
That has a cause, read out of the compiler source in the build image rather than
guessed:

```
config/arm/arm.h:989
  #define REG_ALLOC_ORDER { 3, 2, 1, 0, 12, 14, 4, 5, 6, 7, 8, 10, ... }
```

gcc tries r3, r2, r1, r0 **first** — caller-saved before callee-saved — then r12
and r14, then r4 onward. There is no Thumb-specific override.

**What this does not establish:** that the original toolchain had a different
`REG_ALLOC_ORDER` is the most economical explanation, not a proven one.
Different register pressure, a different pass order, or a different
`CALL_USED_REGISTERS` would all look the same from here.

`Func_80063bc` is this batch's example and is parked on it. Thirty lines against
twenty-nine; every instruction matches but one extra move at entry. The ROM
leaves argument 0 in r0 for its whole life and spills argument 1 to a
callee-saved register; we do the opposite. Both are forced out of r1 by the IME
read landing there — the only question is which keeps r0, and gcc-2.96 gives it
to the value with the **earlier first use**, while the ROM gives it to the later
one. Six variants were tried; the park lists them. The store order cannot be
permuted to fix it, because the store order is the part that already matches.

## The park census

A keyword pass over all 177 park notes at the time:

```
 37  register allocation        35  scheduling
 19  constant-CSE               12  optimiser proved something
  7  argument precompute         4  pool tell / naming
  4  basic-block placement       3  cross-jumping / CSE of two loads
 56  unclassified
```

Two caveats belong with the numbers: the classifier is **first-match-wins**, so
a park naming two causes is counted once; and the 56 unclassified are mostly
**older** parks written before the vocabulary settled, not parks that resist
classification.

## A hole in my own tool, found by hitting it

`tools/split_asm.py` checked **one** cut: the function being removed against
everything else. Cutting from the **middle** leaves *two* remaining pieces, and
labels can cross between those two — which it never looked at. A three-way split
in this batch linked cleanly on the boundary the tool checked and failed on the
other, because twelve earlier functions referenced six `.L` labels in `.rodata`
that stayed with the later half.

The tool reported *"label exports: none needed"* for a split that needed six.
It now checks **every pair** of resulting pieces. Recording this because a tool
that is confidently wrong is worse than no tool, and this one had been trusted
for several batches.

## Checked and negative

Recorded so it is not repeated: **none of the levers added since batch 64
unlocks an existing park.**

- **`DMA3_FILL`** — the four dma-related parks do not use the fill shape.
  `Func_80bd7a4` passes a literal 0 as source, not a stack address, so no helper
  produces it. `rom_198dc` and `rom_251d4` are copies, not fills.
- **Guard inversion** — all three block-placement parks were retried when the
  lever was found. `ovl_7cb2c0/20080fc.c` is unmoved at 19 of 28, and
  `ovl_7f2f14/2008e88.c` is byte-identical under it.

That closes "re-screen the parks with the new levers" rather than leaving it
open as a standing suggestion.

## Parks added this batch

| Function | Class |
|---|---|
| `HeightTile_4` | elided register save — the ROM's save exists only as a consequence of its own register choice |
| `Func_8019908` | register allocation |
| `Func_80c24b0` | register allocation |
| `Func_800fa8c` | register allocation — structure exact, registers shifted by one throughout |
| `Func_80063bc` | register allocation — one extra move; see above |

`Func_80b2884` was parked in this batch and **unparked in the same batch**; its
park file is deleted.
