# Batch 57 — six functions, and two levers given limits

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`;
`Field_Growth_Target`'s was also checked against the address in its `.s` comment,
since its name does not carry one.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Field_Growth_Target` | `080985a8` | main ROM | [rom_97b54_a_c_a_b.c](../src/rom_8a000/rom_97b54_a_c_a_b.c) |
| `Func_80a4754` | `080a4754` | main ROM | [rom_a1814_c_c_c_b.c](../src/rom_a1000/rom_a1814_c_c_c_b.c) |
| `OvlFunc_886_200855c` | `0200855c` | ovl_786f0c | [ovl_30_c_c_c_c_c_c_c_c_c_c_b.c](../src/overlays/rom_786f0c/ovl_30_c_c_c_c_c_c_c_c_c_c_b.c) |
| `OvlFunc_911_20081dc` | `020081dc` | ovl_79e5c0 | [ovl_30_c_a_a_a_a.c](../src/overlays/rom_79e5c0/ovl_30_c_a_a_a_a.c) |
| `OvlFunc_936_200964c` | `0200964c` | ovl_7c097c | [ovl_30_c_c_c_a_c_a_c_b.c](../src/overlays/rom_7c097c/ovl_30_c_c_c_a_c_a_c_b.c) |
| `OvlFunc_942_200886c` | `0200886c` | ovl_7c6bac | [ovl_30_c_c_a_c_c_c_c_b.c](../src/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_b.c) |

All six are 26–41 instructions. **The batch's two findings are both limits on
levers that were previously stated without one.**

## Do not apply the basic-block lever preemptively

The lever moves a constant gcc will **not** place where the ROM has it. If gcc
already places it correctly, hoisting perturbs what was right.

`Func_80a4754` looks like a textbook case: its last call interleaves —

```
mov r2,#1 / ldr r0,=0xb86 / neg r2,r2 / mov r1,#0
```

— and the call sits inside a guarded block, so the conditions hold on paper.
Applying it is **2 of 36**: hoisting `n = -1;` above the guards moves
`mov r1, #0` one instruction *early*. The plain literal `-1` at the call site
matches exactly.

**Screen the plain form first.** Otherwise the lever costs a screen to add and
another to discover it was the thing that broke you.

That function also confirms batch 56's constant-chain test from the other side:
its ROM derives a second offset with `sub r2, #0xa6`, and writing both offsets as
literals makes gcc produce that chain itself — so the chain is gcc's and the
literals are right.

## A fix that works in one function and hurts in another

`Field_Growth_Target`'s read-modify-write at `+0x23` has to be spelled out
completely — pointer, loaded value, constant and result each in their own local
and their own statement:

```c
r = a;  r += 0x23;
t = *r;
v = 2;  v |= t;
*r = v;
```

`a[0x23] = a[0x23] | 2;` is **2 of 34**, with the loaded byte and the constant in
each other's registers: the ROM has `ldrb r2, [r1] / mov r3, #2 / orr r3, r2` and
gcc emits the pair reversed. Naming *only* the loaded byte, or *only* reordering
the operands, changes nothing — it takes all four.

**And the same spelling makes a function with the identical residual worse.**
`src/non_matching/ovl_7ed0a0/2009458.c` has the same two-instruction difference,
and applying this form takes it from **3 of 36 to 7**.

Two functions, one residual, opposite responses to the same change. The
statement structure is not what decides the register pair. Both files record it,
so a reader who finds the fix in one does not assume it transfers.

## One local per independent operation

`OvlFunc_942_200886c` reads `gState` twice — different offsets, different tests.
Reusing one offset variable and one value variable across both is **6 of 39**;
gcc keeps the recycled locals in different registers than the ROM's two
independent ones. A fresh offset, pointer and value per read matches exactly.

The base pointer **is** genuinely held across both (`r5`, pushed), so that one
stays a single local.

> **A local is not free: it is a statement that one value spans both uses.** Read
> the ROM for which registers are held and which are rebuilt, then mirror it.

## The area-dispatcher family is closed

Enumerating it found exactly **one** member left — `OvlFunc_911_20081dc` — and it
needed only `_AREA_24`, the last missing id. That vein carried batches 45, 47, 52
and 57.

`OvlFunc_936_200964c` on the way is a **five-arm** dispatcher and needed nothing
the two- and three-arm members did not. The family scales; a longer one is not a
new problem.

## Also

- **`OvlFunc_886_200855c`**: a range check written as an **addition of a negative
  constant** with an unsigned compare — `t = h + 0xffff5fff; if (t <= 0x3ffe)`.
  The `bhi` is what lets one test do the work of two bounds.
- **`OvlFunc_942_200886c`**'s first compared value is `cmp r3, #0x5a`, an
  **immediate** — so the pool tell does not apply, it is not an area id, and
  `area.sym` was correctly left alone. Easy to miss surrounded by genuine area
  comparisons.

## Parked

`OvlFunc_956_2008ad4` at 9 of 36 — allocation on two constant builds, one a
straight-line interleave with no branch for the lever. Its offset **must** stay a
variable shifted in two statements: inlining `(0xfa << 1)` makes gcc fold it to a
single pool load and the function comes out three instructions short, the same
requirement as the GetEntrances family.
