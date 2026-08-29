# Batch 98 — the width the arithmetic happens at, and a theory refuted

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked overlay ELF with
`arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_970_2008100` | `02008100` | ovl_7fa4ec | [ovl_30_c_c_c_a_a_a_b.c](../src/overlays/rom_7fa4ec/ovl_30_c_c_c_a_a_a_b.c) |
| `OvlFunc_951_2008d70` | `02008d70` | ovl_7d6418 | [ovl_30_c_c_c_a_c_b.c](../src/overlays/rom_7d6418/ovl_30_c_c_c_a_c_b.c) |
| `OvlFunc_945_2009280` | `02009280` | ovl_7cb2c0 | [ovl_30_c_c_a_a_c_a_c_c_b.c](../src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_c_b.c) |
| `OvlFunc_932_200a428` | `0200a428` | ovl_7b9cb4 | [ovl_30_a_c_c_a_c_c_a_a_a_b.c](../src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_b.c) |
| `OvlFunc_899_200c840` | `0200c840` | ovl_794ac0 | [ovl_30_c_c_c_c_c_c_a_b_b.c](../src/overlays/rom_794ac0/ovl_30_c_c_c_c_c_c_a_b_b.c) |

Six parked. This batch ran over two rounds; the first produced two functions and
was deliberately left unpublished rather than padded.

## The width the arithmetic happens at decides add against sub

Two of the five came down to this, and it unifies a note that has been sitting
in the tree as two separate observations.

```
	rom	sub r3, #6              /  sub r3, #1
	ours	ldr r1, =0xfffa / add   /  ldr r1, =0xffff / add
```

Writing `(unsigned short)(x - 6)` or `(*p)--` on a halfword does the arithmetic
at **halfword width**, where subtracting is the same as adding the wrap-around
value — and gcc pools that addend. Reading the value into an `int` local and
subtracting there gives the ROM's `sub`.

Batch 89's `Func_80bf37c` park records the mirror: there the ROM wanted the
**add** and `v--` produced a sub. It is one rule, not two coincidences, and the
rule is about the width, not about which operator you type.

`OvlFunc_970_2008100` needed two more decisions on top: the counter is read
twice through two types (`ldrsh` for the test, `ldrh` for the value), and the
0x14 it stores needs both a named `int` **and** the destination pointer computed
first — either alone leaves two instructions transposed.

## Which return value gets the shared block

Batch 96 found that turning `if (p == 0) return 0;` into the positive form stops
gcc hoisting the return constant. `OvlFunc_945_2009280` shows the other side:
there the trailing `return 1` has to be the fall-through, and two early
`if (...) return 0;` guards are what produce it.

So the lever is not "write the test positive". It is: **the value reached by
more exits gets the shared block, and the other one falls through.** Two zeros
and one 1 here; one zero and one 1 in `common0_18`. Writing it the other way
round cost eleven differing positions of forty-four.

## `__modsi3 = _modsi3_RAM;`

`OvlFunc_951_2008d70` was one instruction from matching, and the instruction was
the modulo helper. gcc-2.96 emits `__modsi3` for `%` and has no flag to rename
it; overlay code calls the RAM-resident copy. The alias emits no bytes and is
the same fix batch 96 applied for `__divsi3`. This is the first `%` in an
overlay to need it, so both helpers now have the same treatment.

## A standing theory, refuted

Batch 96 named the r2/r3 exchange as a class and offered a lead: the one
matching function with that instruction shape,
`src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_c.c`, feeds its `and` result into an
`orr` before storing, while the parked ones store immediately — so perhaps the
allocator splits on live-range length.

`OvlFunc_922_2008ed8` has the `orr` **and still splits**, at 8 of 43. The theory
is wrong and `docs/elevation.md` now says so rather than leaving a plausible
dead end for the next person.

What the class does respond to is narrower than hoped: a named constant of the
field's width, and statement order, and only where the store is in the same
statement. Six functions have been through it; two yielded.

That park is still worth reading, because getting it to 8 needed **three**
levers from three different batches stacked — the named `int` mask, the named
`unsigned char` for the OR'd constant, and the positive null test.

## The `_call_via_rN` lever transfers

`Func_80b84c0` is the second use of the indirect-call lever found in
`src/rom_c9000/rom_e0524.c`, and it does produce the `_call_via` form: assigning
the callee to a function-pointer local is enough, and gcc-2.96 does not
constant-propagate it back into a direct call.

It lands in r10 where the ROM has r5, which costs an extra push/pop pair and
four moves around it — six instructions. The ROM reuses two registers that are
dead by that point and we do not. So the class is reachable in principle and
this function is not blocked on the lever itself.

## Other parks

* **`Func_80b8000`** — batch 97's two-pointer lever takes it from 33 differing
  to 16, but the ROM shares a register between a zero and the second pointer and
  nothing at the statement level asks gcc to run out of registers.
* **`OvlFunc_958_2008fd0`** — a third member of the message-base-in-a-register
  class, and the smallest: a single `+ 8` rather than three consecutive ids.
* **`YesNoMenu`** — the logic screens correct; what differs is which of four
  parameters lands in which high register.
* **`ClearFlag`** — its residual is now characterised as ONE decision rather than
  two: the ROM computes the bit before touching `gFlags`, which frees r3 for a
  three-operand shift.
