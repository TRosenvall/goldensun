# Batch 149 — reading the register class off the assembly, and a park that was wrong about itself

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every
address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_80b010c` | `080b010c` | main ROM | [rom_b0070_a_a_c_a_a_a.c](../src/rom_b0000/rom_b0070_a_a_c_a_a_a.c) |
| `OvlFunc_941_20080d4` | `020080d4` | ovl_7c5efc | [ovl_30_c_a_c_c_a_c_a_b.c](../src/overlays/rom_7c5efc/ovl_30_c_a_c_c_a_c_a_b.c) |
| `OvlFunc_922_2009050` | `02009050` | ovl_7a8c8c | [ovl_30_c_a_c_c_c_c_a_a_a_b_b.c](../src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_a_a_b_b.c) |
| `OvlFunc_927_2009454` | `02009454` | ovl_7b4558 | [ovl_30_c_c_a_c_c_a.c](../src/overlays/rom_7b4558/ovl_30_c_c_a_c_c_a.c) |
| `OvlFunc_901_2008350` | `02008350` | ovl_797990 | [ovl_314_a_a_c_a.c](../src/overlays/rom_797990/ovl_314_a_a_c_a.c) |

One park deleted, five added.

## Stack arguments: the `str` operands say which get names, the statement position says which register class

The existing rule said each six-argument call site needs its own pair of locals.
It never said **which** sites need a pair at all, and the assembly answers that
directly — look at what the `str` reads from:

```
str r6, [sp]          <- a HELD register: the ROM spent a callee-saved
                         register on this value, so it is a SHARED local
mov r3, #0x18
mov r2, #0x3e         <- BOTH built fresh into separate registers, then
str r3, [sp]             stored: this site wants its OWN pair
str r2, [sp, #4]
```

Written as literals gcc reuses one register for both and interleaves the stores
(`mov r3 / str / mov r3 / str`), one register short of the ROM every time.
`OvlFunc_941_20080d4` has eight six-argument calls and needs **both** answers —
two values shared in r5 and r6 across the whole body, two sites with their own
pairs — and went 6 differing to exact. `OvlFunc_922_2009050` is seven sites, one
shared and five pairs, and matched on the first screen from the same reading.

**And where the assignment goes picks the register class.** A named stack
argument lands in a callee-saved register only if its live range **crosses a
call**. `OvlFunc_927_2009454` passes `(4, 0)` at its last site and the ROM holds
the zero in r5 across the whole register-argument setup, storing it after r0–r3
are loaded. Declared beside the call — exactly what the per-site rule asks for —
it gets a *scratch* register and is stored at once: 6 differing. Moving the
assignment up so it spans the preceding call makes gcc spend r5 and the function
matches. Hoisting it further, to the top of the function, is wrong again.

`Func_8020198` is the strongest case and it is parked for an unrelated reason.
The ROM spends a **third** callee-saved register there; written beside its call
the value takes a scratch register and the parameter is displaced, which
cascades into 17 differing lines. One assignment moved up: **17 → 2**. Moving
both such assignments up is worse. One crossing is enough and two is too many.

## A park that was wrong about itself

`OvlFunc_901_2008350` had been parked on **register pressure**. The note read
the r8–r11 spills off the prologue, observed that the ROM keeps four pointers
live where we kept three, and recorded that "the structure is believed right and
is not the problem."

The structure was the problem. The ROM's guard is

```
cmp r0, r10 / blt .L384          <- angle work
mov r3, r11 / cmp r3, #0 / beq .L3da   <- the anim-2 arm
```

which is `if (n < lim || force != 0) { angle } else { anim2 }`. The park had
written the contrapositive — same predicate, arms swapped — and **every** later
difference, the four-versus-three pointer allocation included, followed from
that. Written the ROM's way round it matches exactly, r8–r11 and all.

It was found by `tools/solved_twins.py` in seconds: `OvlFunc_898_2009674`, in a
different overlay, is the same function and had been elevated earlier. This file
is that `.c` with the callee renamed and a third argument added.

**The generalisation worth acting on:** a park that names a register-allocation
blocker and asserts the structure is fine has usually had no second opinion to
test the structure against. Run `solved_twins.py` over the parked set, not only
over fresh candidates.

## A correction to batch 148, from batch 148's own advice

Batch 148 said explicit gotos are how you control block order. That has a
boundary and I walked past it. On `OvlFunc_971_2008e10` I screened **five** goto
arrangements before trying the plain `for (;;) { … if (c) break; … }` at all,
and the plain form beat every one of them — 41, 40, 79, 35 against **29**.

The reason is worth keeping: **a two-instruction block reached by `goto` is
duplicated inline by gcc unless it happens to sit adjacent to the branch.**
Moving the label to where the ROM has the block makes gcc copy the body to the
branch site instead of jumping to it — 79, worse than the start. An ordinary
`break` produces the out-of-line block and the branch to it for free.

So: reach for gotos when the ROM's block order cannot be expressed with ordinary
control flow. Not before trying the structured form and reading its diff.

## A new class: call tails gcc will not cross-jump

Two independent functions park on the same shape. The ROM branches several arms
into **one shared call**, loading only the differing argument in each arm:

```
cmp r2, r3 / bne .L4f4 / ldr r0, =0x2076 / b .L500
.L4f4: ...   ldr r0, =0x2078 / b .L500
.L4fe:       ldr r0, =0x207a
.L500: bl __MessageID
```

Written as separate calls gcc emits a `bl` per arm — two lines too many.
Written as an id assigned per arm with one call after the join, which is the
ROM's own shape, gcc **if-converts** it: each `ldr r0, =id` is hoisted above its
compare and a `beq` falls into the call — four lines too few. Explicit gotos
collapse identically. `OvlFunc_common1_4cc` and `OvlFunc_971_2008e10` fail the
same way, and on `2008e10` forcing the merge with a shared local made it worse
for the same reason.

gcc-2.96 as invoked here does not cross-jump identical call tails that differ
only in a pooled constant, and the original build did. That is a class, and it
deserves a flag sweep.

## The candidate filter, again

`pickable.py` was serving a function that was already parked. It skipped parks
by **filename**, but a park covering a *class* is named for the class and lists
its members inside — `message_base_register.c` parks two functions and neither
address appears in any filename. Same failure as batch 147's filename fix,
reached the other way. It now reads park contents for the two conventions notes
use, and 34 candidates → 32.

Also new this batch: the filter rejects any function with three or more `neg`
— the `-1` triple of `constant_reuse.c`, which batch 148 confirmed on a function
with a *single* such call, so there is nothing at the call site to change.

## Numbers

3277 elevated / 2140 remaining / 392 parked.
