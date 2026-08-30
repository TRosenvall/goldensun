# Batch 150 — the assembly names the type, the slot count, and when to stop trusting the screen

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every
address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `OvlFunc_888_20084e8` | `020084e8` | ovl_7892c8 | [ovl_30_c_c_a_a_a_a_c_b.c](../src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_a_c_b.c) |
| `Func_801b148` | `0801b148` | main ROM | [rom_1aeec_a_a_a_b.c](../src/rom_15000/rom_1aeec_a_a_a_b.c) |
| `OvlFunc_918_2008f58` | `02008f58` | ovl_7a5214 | [ovl_314_c_c_a_b.c](../src/overlays/rom_7a5214/ovl_314_c_c_a_b.c) |
| `OvlFunc_932_200a020` | `0200a020` | ovl_7b9cb4 | [ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_b.c](../src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_b.c) |
| `OvlFunc_935_2008aa0` | `02008aa0` | ovl_7bf5a8 | [ovl_2e0_c_c_a_c_c_c_b.c](../src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_c_c_b.c) |

Three parked. Two of the five matched on the first screen with no lever at all,
which is the point of the first two sections: the reference answers more
questions outright than the parked set assumes.

## A halfword read's addressing mode names its signedness

thumb has `ldrh rD, [rB, #imm]` but **no `ldrsh` with an immediate**. A signed
halfword field must therefore build its offset in a register first. So the
disassembly states the type:

```
ldrh  rD, [rB, #imm]   ->  the field is UNSIGNED
ldrsh rD, [rB, rO]     ->  the field is SIGNED
```

On `Func_801b148` I declared a linked-list node's flag `short` and got
`mov r2, #0xa / ldrsh r3, [r5, r2]` where the ROM has `ldrh r3, [r5, #0xa]` —
**58 differing lines from one character of the struct.** The same function's
field at `+0x12` genuinely is signed and keeps its register-offset form, so each
field has to be read separately.

A register-offset `ldrsh` is not a scheduling accident to hunt a lever for. It
is the only encoding available. This is the cheapest type check in the corpus
and it costs one glance at the operand.

## Every stack SLOT needs its own local, and it is all-or-nothing

Batch 149 established that the `str` operands say which stack arguments get
names — a store from a register the ROM already holds means a shared local, two
`mov`s into separate registers before either store mean that site wants its own
fresh pair. `OvlFunc_888_20084e8` generalises the count.

It makes an **eleven-argument call**: four in registers, **seven** on the stack.
The ROM materialises every stack value into its own register and only then
issues the stores. Written as eleven literals gcc reuses ONE register for all
seven — `mov r3,#3 / str r3 / mov r3,#7 / str r3 / …` — 22 differing, and it
never spends the callee-saved register the ROM pushes.

**Naming three of the seven is worse than naming none** — 28 differing against
22. All seven named: **2**. The rule is all-or-nothing because the register the
extra locals compete for is the one that decides the prologue; a partial naming
leaves gcc short of exactly one and it reshuffles everything.

Two of the seven were shared, read off the `str` operands: one value stored to
two slots from one register, and one that is both a register argument and a
stack slot. `OvlFunc_918_2008f58` then matched on the first screen with six
six-argument calls decided entirely by the same reading — no lever.

## The screen is not a verdict, and I proved it the hard way

`OvlFunc_935_2008aa0` screened at three differing lines. One of them was
`bl __umodsi3` against the ROM's `bl _umodsi3_RAM`, which really is only a
linker alias: every overlay script that needs it carries
`__umodsi3 = _umodsi3_RAM;`, and five already-elevated files link through
exactly that route. That reasoning was correct.

What was wrong was treating **three** differing lines as **one**. I installed
the file on "the only difference left is the alias" and `make compare` rejected
the overlay. Backing out and reading every line found the other two: `i = 0`
must be written **before** `t = 0xff << 16`, because the ROM interleaves them —
the counter's initialisation lands inside the constant's two-instruction build.
Swapping the statements closed it. Hoisting the constant into a block that
dominates the loop, the usual lever for that shape, is far worse here (17 and 24
differing).

`docs/elevation.md` already says tryc.py is a screen and `make compare` is the
authority. The failure mode it did not name is subtler than trusting an OK: it
is discounting one line of a diff correctly and then not reading the rest.

The same function also needed the modulo to be **unsigned** — `__Random`
declared returning `unsigned int`, or gcc emits `__modsi3` and calls the wrong
helper — and its data address held in a callee-saved register across the call,
which takes a named pointer assigned before it.

## What the parks establish

**The register coin flip has no rule to encode.** `OvlFunc_946_200a16c` is 72 of
72 lines where all thirteen differing lines are the same two values in swapped
callee-saved registers — the identical residue to `OvlFunc_946_2009c84` two
functions over. Thirteen spellings are now screened across the pair. The pair
rules out the obvious hypothesis: on `2009c84` the ROM puts the FIRST-computed
value in r5, here it puts the first-computed in r6, and **in both cases we
produce the opposite**. There is no "first one gets r5"; it is the allocator's
density ranking and the source has no handle on it. Both are clean two-pseudo
test cases for the `REG_ALLOC_ORDER` hypothesis.

**The shared-call-tail class has a third instance and a boundary.**
`OvlFunc_946_2009d2c` fails exactly as `OvlFunc_common1_4cc` and
`OvlFunc_971_2008e10` do. But `200a16c` has four arms branching into one call
block and is **not** an instance — gcc reproduces the shared call there and the
line count matches, because the arms set two argument registers rather than one
value it can hoist above the compare. Recorded so the class is not
over-applied.

**`StartEarthquake` parks with three independent residues**: gcc narrows
`w[0] >> 16` into an `ldrsh` of the high halfword where the ROM loads the word
and shifts (named locals and `volatile` both inert); the ROM writes three entry
fields through a post-incrementing pointer, and a single-register `stmia`
appears nowhere in the generated corpus, so there is no matching C to copy the
idiom from; and the `DMA3_CLEAR` expansion picks a different scratch register
for its zero. Its sibling `StartSnow` has the same shape.

## Tooling: one fix kept, one detector discarded

`tools/solved_twins.py` un-parked `OvlFunc_901_2008350` last batch and has now
been re-run to exhaustion — zero remaining functions have a solved twin.

I built a detector for the shared-call-tail class and **threw it away**. It
caught one of the three known instances, because the arms take different shapes
in each. A signal that weak is worse than none: it would have read "clean" on
two functions that are not.

## Numbers

3282 elevated / 2135 remaining / 397 parked.
