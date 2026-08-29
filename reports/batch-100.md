# Batch 100 — an unsigned switch, a duplicated tail, and a rule weakened again

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked overlay ELF with
`arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_936_2008040` | `02008040` | ovl_7c097c | [ovl_30_a_c_a_a.c](../src/overlays/rom_7c097c/ovl_30_a_c_a_a.c) |
| `OvlFunc_956_2008274` | `02008274` | ovl_7e0928 | [ovl_30_a_c_c_a_c_c_c_b.c](../src/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_b.c) |
| `OvlFunc_939_20083f4` | `020083f4` | ovl_7c460c | [ovl_314_a_c_a_a_c_a_a.c](../src/overlays/rom_7c460c/ovl_314_a_c_a_a_c_a_a.c) |
| `OvlFunc_932_200840c` | `0200840c` | ovl_7b9cb4 | [ovl_30_a_c_c_a_a_a_a_c_c_c.c](../src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_c_c_c.c) |
| `OvlFunc_942_2008af8` | `02008af8` | ovl_7c6bac | [ovl_30_c_c_c_a.c](../src/overlays/rom_7c6bac/ovl_30_c_c_c_a.c) |

Four parked. Two rounds; the first produced four and was left open.

## `bcc` and `bhi` in a switch mean the value is unsigned

`OvlFunc_936_2008040` dispatches on a random roll:

```
	cmp	r0, #1 / beq  <case 1>
	cmp	r0, #1 / bcc  <case 0>
	cmp	r0, #4 / bhi  <default>
	cmp	r0, #3 / bcc  <default>
	b	<cases 3 and 4>
```

An `int` switch value gives the signed forms (`bgt`, `blt`) and 46 differing of
55. Making it `unsigned` takes that to 11. The four case labels are separately
what produce the decision tree rather than an equality chain — batch 91's rule,
which needs three or more.

## When there is only one return value, the lever is a duplicated tail

Batch 96 established that with two return values, the one reached by more exits
gets the shared block. This function returns **1 from both exits**, so there is
nothing to choose between — and the layout is still decided by the source.

Written with an early `return 1` and a single decrement after the `if`, gcc
hoists `mov r0, #1` above the test. Duplicating the two-line decrement into both
branches gives gcc two identical tails to cross-jump, and the constant stays at
the tail where the ROM has it. Eleven differing to zero.

So the rule generalises: **what decides the exit layout is how many identical
tails the source offers gcc to merge**, not how the test is written.

## Necessary is not sufficient

Batch 95 said a value that must **survive a call** is genuine evidence the
source named it. `OvlFunc_956_2008274` passes that test completely — two pooled
constants live in two callee-saved registers across a call and are re-stored
afterwards, both registers pushed.

Named as locals, the two registers come out **exchanged**, 6 differing of 51,
and no ordering of the declarations or the assignments fixes it. Written as
plain literals at both sites it matches, and gcc discovers the shared registers
itself.

That is the fourth function where this family of over-reading has cost a
spelling. `docs/elevation.md` now marks the survives-a-call test as **necessary
but not sufficient**: it raises naming from "no reason to think so" to "worth
trying", and both spellings still have to be measured.

## The return-type lever's boundary, mapped

Batch 99 corrected the "delete the prototype" folklore to a return-type lever.
This round found where it stops. It moves r0 relative to other **register
moves**. It does not move r0 against:

* a **pool load** — `OvlFunc_943_2008a48`, `ldr r1, =0x103` against
  `mov r0, #0x15`, two instructions out and unmoved by either the return type or
  deleting the declaration;
* **shifts** — `OvlFunc_930_2008870`, `mov r0, #0xe` against two `lsl`s,
  byte-identical with an `int` return.

Both are recorded in their parks so nobody reaches for the lever on sight.

## Two functions, one C, opposite requirements

`OvlFunc_939_20083f4` writes bytes at +0x55 and +0x23, and the ROM **derives**
the second address (`add r2, #0x55 / strb / sub r2, #0x32 / strb`) because
0x55 − 0x32 is 0x23. Plain field writes give exactly that.

The parked `OvlFunc_923_2009bc8` has the same two writes and the ROM keeps **two
independent pointer chains**, where plain field writes are wrong and both
pointers have to be computed before the first store.

Same C, opposite requirement. Only the assembly says which, and it is worth
checking rather than assuming the derived form is always what gcc will do.

## Parks

* **`OvlFunc_898_2008a4c`** — a **fourth** member of the cutscene-bookend family.
  That shape now has four distinct outcomes: one matches, one is off by one
  instruction of pool placement, one by scheduling, and this one merges a branch
  the ROM shares between an `if` exit and a pool skip.
* **`OvlFunc_955_200862c`** — the same shifted-field-read residue as
  `200842c.c`, three times over rather than once. Anything that moves one moves
  both.
* **`OvlFunc_897_200ac1c`** — two parameters in the wrong two callee-saved
  registers; same class as `YesNoMenu`.
