# Batch 99 — the prototype lever was really a return-type lever

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_80a4db4` | `080a4db4` | main ROM | [rom_a47b4_a_c_b.c](../src/rom_a1000/rom_a47b4_a_c_b.c) |
| `Func_80b83b4` | `080b83b4` | main ROM | [rom_b8228_c_a_a.c](../src/rom_b5000/rom_b8228_c_a_a.c) |
| `OvlFunc_936_20082e8` | `020082e8` | ovl_7c097c | [ovl_30_c_c_a_c_a.c](../src/overlays/rom_7c097c/ovl_30_c_c_a_c_a.c) |
| `OvlFunc_943_200b950` | `0200b950` | ovl_7c7b9c | [ovl_30_c_a_a_c_c_a_b.c](../src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c_a_b.c) |
| `OvlFunc_964_2009a10` | `02009a10` | ovl_7ed0a0 | [ovl_30_a_c_c_c_c_c_c.c](../src/overlays/rom_7ed0a0/ovl_30_a_c_c_c_c_c_c.c) |

Six parked. This batch ran over two rounds; the first produced three and was
left open rather than padded.

## A rule I have been carrying since batch 92 was wrong

Batch 92 found that when the only remaining difference is the ORDER of the
argument-setup moves in front of a call, deleting that callee's `extern`
declaration fixes it. Batch 93 wrote it as a table with a direction. Batch 94
found a counterexample and softened it to "try both and measure".

All of that was chasing an artifact. **Deleting the declaration changes two
things**, and only one of them matters:

| declaration | result |
|---|---|
| `void f(int, int)` | r0 emitted first |
| `void f()` | r0 first |
| `int f(int, int)` | r0 **last** |
| `int f()` | r0 last |
| no declaration at all | r0 last — because the implicit return is `int` |

**The parameter list is irrelevant. The return type decides it.** Deleting the
declaration worked only because it also made the return implicitly `int`.

That explains the batch-94 "counterexample" too: adding a prototype to a callee
that had none moves r0 earlier *only if the prototype you add says `void`*.
There was never an unpredictable direction.

Two files that carried a deliberately-omitted declaration —
`src/rom_a1000/rom_a47b4_a_b.c` and
`src/overlays/rom_7ed0a0/ovl_30_c_c_c_c_c_a.c` — now have **full prototypes with
`int` returns and still match**. That is both honest and readable, and it
removes a piece of folklore from the tree.

**The batch-93 caveat is withdrawn.** It said the lever only bites when the r0
argument is a value in a register. `OvlFunc_936_20082e8` has `mov r0, #8` — a
small constant — and responds exactly.

**And there is a boundary.** `OvlFunc_943_2008a48` and its twin are two
instructions out with `ldr r1, =0x103` and `mov r0, #0x15` transposed, and
neither the return type nor deleting the declaration moves them. gcc issues pool
loads as early as it can; a rotation involving a **pool load** has a different
cause from one involving two register moves. Worth knowing before reaching for
the lever on sight.

## The named-constant type lever is a spelling to try, not a rule

Batch 97 found that `unsigned char two = 2;` put the constant in an `orr`'s
destination where `int two` did not, and phrased it as "a named constant of the
FIELD's type".

`OvlFunc_964_2009a10` has the same shape on an `and` and goes the other way: the
plain literal `*p &= 0xfd` matches, `int mask` matches, and a named
`unsigned char mask` **breaks** it. So the width of the named constant is not a
rule about matching the field; it is one of several spellings to measure. The
file records all four.

## Smaller readings

**`0x80000000` is a sentinel, not a coordinate.** `Func_80b83b4` tests both of an
actor's override fields against it before falling back to the live position —
four independent guards, which is why one compare appears four times.

**A magnitude computed twice is the source, not an artifact.** `Func_80a4db4` has
the `mov r3, r5 / cmp / bge / neg` sequence in full, twice. A local holding the
absolute value computes it once. Likewise its glyph position is computed inside
both arms; hoisting it loses two instructions. Both were measured.

**A signed `/ 2` is `lsr #31 / add / asr #1`.** Written `>> 1` it is a bare `asr`
and `Func_80b83b4` loses four instructions.

## Parks

* **`OvlFunc_907_2008f3c`** — a **third** member of the two-pointer-chain park,
  reaching exactly the same 7 differing positions as the other two despite an
  extra masked byte. That confirms the residue is the chains and nothing
  downstream.
* **`OvlFunc_933_2008344`** — batch 97's type lever runs **one way only**. It can
  put a constant in an `and`'s destination; nothing puts the loaded value there,
  and naming the loaded word makes it worse.
* **`Debug_TestEquipAndStatus`** — gcc is *cleverer* than the original build. The
  offset `0x11c` is live in a register at the store, and `0x11c` truncated to a
  byte is `0x1c`, the value being stored. gcc reuses the register; the ROM
  builds `0x1c` fresh. No spelling can un-coincide them. Fourth member of the
  constant-reuse class and the first where the reused value is a truncation of
  an address offset.
* **`OvlFunc_956_2008b30`** — 3 of 47, a pooled mask in the wrong register.
* **`Func_809b5dc`**, **`OvlFunc_943_2008a48`** — as above.
