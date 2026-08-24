# Batch 30 — 5 functions, a calling convention, and a false positive in the screen

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–29 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted. Every address
below read back from the linked ELFs and every path confirmed to exist.

## Read this first: `tryc.py` has a third false-positive class

Two functions screened **clean at 39 instructions against 39**, were split,
written in, and **failed `make compare`**. Both reverted.

The instruction streams were genuinely identical. The **bytes** differed in 36
places, every one a `ldr rN, [pc, #imm]` or a branch displacement, because **the
ROM keeps its literal pool INSIDE the function body** behind a `.pool_aligned`
while gcc puts the pool after the epilogue in a single-function translation
unit. Every PC-relative offset shifts.

`tryc.py` normalises pool loads to `ldr rD, =value` on both sides — which is
what makes the pool-tell class readable at all — so a pool at a different
*distance* compares equal.

**This is the third false-positive class in that tool**, after dropped label
definitions (batch 20) and the keyhole listing (batch 24). Two mitigations:

* `tryc.py` now **warns on an OK verdict** when the reference has an inline
  pool. 336 of 4,730 overlay `.s` files qualify, so it is specific rather than
  constant noise.
* `pick_candidates.py` now **rejects** such references outright.

`make compare` caught this, not the screen. The gate is what stopped two wrong
functions landing.

## The 24-byte struct-by-value shape

Four of the five functions here are one family, and the family is worth
recognising because its assembly reads like hand-written pointer juggling:

    mov r2, sp / add r3, sp, #0x18 / ldmia r3!, {r0, r1} / stmia r2!, {r0, r1}
    ldr r0, [r5] / ldr r1, [r5, #4] / ldr r2, [r5, #8] / ldr r3, [r5, #0xc]
    bl <callee>

That is the ARM calling convention for a **24-byte aggregate passed by value**:
first sixteen bytes in r0–r3, remaining eight copied to the outgoing stack area
with an `ldmia`/`stmia` pair. Written as `f(s)` with a six-word struct, gcc emits
exactly this. Written as a pointer plus explicit copies, it does not.

**The tell is `sub sp, #0x20` with the local at `sp+8`** — the local sits at an
offset because the 8-byte outgoing slot is at `sp`, which falls out of the struct
being an *argument* rather than anything the source arranges.

`OvlFunc_914_20089f8` had been passed over **twice** as "complex struct
handling" before a twin made the shape obvious. Sweeping for it finds **53
functions** in `asm/` still carrying this shape.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `OvlFunc_930_20088e0` | `0x020088e0` | rom_7b7f1c | sanctum attendant |
| `OvlFunc_947_200a53c` | `0x0200a53c` | rom_7d0e88 | struct-by-value, with else arm |
| `OvlFunc_914_20089f8` | `0x020089f8` | rom_7a1ff0 | struct-by-value |
| `OvlFunc_946_2009740` | `0x02009740` | rom_7ced6c | twin |
| `OvlFunc_957_2008eac` | `0x02008eac` | rom_7e3e08 | twin |

## The twin-copy hazard

`OvlFunc_930_20088e0` was written by copying its twin and changing constants.
**The copy was wrong on the first screen**: the actor id appears twice — in
`__UI_Sanctum` and in `__ActorMessage` — and only one got changed. One differing
instruction, `mov r0, #0xf` against `mov r0, #0xd`.

That is the same class of bug six parked functions were sitting on, caught here
in one screen by the check that finds them: **on a short diff, ask whether the
differing operand is a value before assuming register allocation.** Copying a
twin is the cheapest way to manufacture one, so check every constant that
appears more than once.

## Parked

`OvlFunc_962_200816c` and its twin `OvlFunc_967_2008234` — the pool-placement
pair above. The C is **correct** and is parked with both levers it needed:

* the mask is a **named `int` local** (the `narrow_constant` sub-case that has a
  lever — a narrow value inside a wide expression);
* the mask is assigned **after** the addition, so gcc reuses the register the ROM
  reuses. Assigned before, it is four positions out.

That second point is the same lesson as the stack-arg-pair lever: **being live
earlier is not the same as being live at the right moment.**

## Counts

281 functions elevated in total. 3,018 hand-written functions remain in `asm/`
of 5,714. 93 parked functions, of which 4 document blocker classes rather than
individual functions.
