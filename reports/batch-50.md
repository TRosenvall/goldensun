# Batch 50 — seven functions, one question closed, two classes parked

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`
and sits at exactly the address its name claims.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_8091858` | `08091858` | main ROM | [rom_91584_c_a_c_c_c_a_b.c](../src/rom_8a000/rom_91584_c_a_c_c_c_a_b.c) |
| `OvlFunc_890_20081ec` | `020081ec` | ovl_78b2ac | [ovl_30_c_c_a_a_c_b.c](../src/overlays/rom_78b2ac/ovl_30_c_c_a_a_c_b.c) |
| `OvlFunc_891_200901c` | `0200901c` | ovl_78c76c | [ovl_30_c_c_a_c_c_a_a_b.c](../src/overlays/rom_78c76c/ovl_30_c_c_a_c_c_a_a_b.c) |
| `OvlFunc_936_200958c` | `0200958c` | ovl_7c097c | [ovl_30_c_c_c_a_a_c_a_b.c](../src/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_b.c) |
| `OvlFunc_937_2008308` | `02008308` | ovl_7c3044 | [ovl_30_c_c_c_c_c_c_b.c](../src/overlays/rom_7c3044/ovl_30_c_c_c_c_c_c_b.c) |
| `OvlFunc_938_2008230` | `02008230` | ovl_7c37ac | [ovl_30_c_c_c_c_c_b.c](../src/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_b.c) |
| `OvlFunc_941_200833c` | `0200833c` | ovl_7c5efc | [ovl_30_c_a_c_c_c_a_b.c](../src/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_b.c) |

## A question closed after several batches of re-attempts

`src/non_matching/rom_8a000/rom_9a44c.c` has been picked up and put down
repeatedly. Its own note set the deciding test:

> *checkable against the other functions in the same .s: if they need the
> scheduler ON, the flag is not the answer and this is a genuine compiler
> difference.*

**That test has now been run.** A rule giving the whole `rom_9a44c` stem
`-fno-schedule-insns2`, then a clean rebuild: **the ROM checksum fails.** Four of
the five already-elevated siblings change under the flag —

```
rom_9a44c_a_a_b.s   5 instructions
rom_9a44c_a_b.s     5 instructions
rom_9a44c_b.s       3 instructions
rom_9a44c_c_c_b.s   1 instruction
```

— and every one of them was byte-matching at `-O2`.

So the translation unit needs the post-reload scheduler **on**. What remains is
a source form nobody has found, or a real difference between gcc-2.96 and the
compiler Camelot used. **It is not a per-file flag, and no future round should
spend time re-testing that.** The Makefile change was reverted.

Worth seven functions if it is ever solved — the body appears verbatim in six
overlays plus the main ROM.

### Two Makefile comments corrected

Both asserted that `-O1` is *"equivalently `-O2 -fno-schedule-insns2`"*. It is
not: `-O1` also changes register allocation and expression ordering, the two
diverge sharply on `Func_809a44c`, and for one stem the scheduler flag breaks
four matching files. The claim was never verified for the TUs it was written
about — only assumed, and then inherited.

## The basic-block lever, twice in one function

`OvlFunc_891_200901c` is the clearest demonstration of the batch-43 lever so
far, because it needs it **twice**, on the two shapes that batch argued are one
mechanism:

| Local | Retires |
|---|---|
| `n = 0xfc << 1;` | an **arg-interleave** — the ROM writes `r0` between `mov r1, #0xfc` and its `lsl r1, #1` |
| `s1`, `s2` | **pool-loads-first** on a *different* call in the same body — the ROM does `mov r0, #9` before two `ldr =` loads |

Fixing only the first leaves **3 of 21**. Both calls sit inside the `if` body and
all three constants are assigned in the entry block, which dominates the body
and contains none of their uses — the conditions from batch 43 as amended in
batch 44.

**Also checked and dropped:** `OvlFunc_891_20095d4` has the same interleave but
its call is in the **entry block**, so there is no earlier block to assign from
and the lever cannot reach it. That is the distinction to check first.

## A recognition rule for constant-CSE

Two more TUs need `-fno-rerun-cse-after-loop`, taking the count to **eleven**.
Both have the same shape, and it is worth stating as a rule rather than a list:

> **A flag id read in a guard and written in the body is constant-CSE.**

gcc hoists it into a callee-saved register across the call, spending a push and
a pop to save one pool load; the ROM loads it twice. `OvlFunc_890_20081ec` is 22
instructions against 20 without the flag; `OvlFunc_941_200833c` is 25 against 23.

`OvlFunc_936_200958c` is the same class, and there the **basic-block lever was
tried first and made it longer** — 13 of 17 against a 16-instruction ROM —
because the named local is exactly what CSE merges. Constant-CSE and
arg-interleave look alike in a listing and are not the same class.

All three rules were added as **explicit targets, not patterns**, per batch 45.

## `tools/shape_groups.py`

Batch 49 elevated nine functions by enumerating a shape already solved. This
generalises it: group every remaining function under N instructions by its
**set of opcodes**, rank the groups by size.

The docstring leads with the caveat this batch earned:

> **A large group may be large because it is BLOCKED.** The candidate rankers
> refuse those functions round after round and they accumulate. Screen one
> member before spending a round.

The first run put a **14-member group at the top and every member is blocked** —
straight-line arg-interleave, unreachable per batch 42. That group is now parked
as a class in `src/non_matching/overlays/arg_interleave_flat.c` with all
fourteen members listed, so nobody re-derives it.

The `branch` column is the field that matters: the basic-block lever needs a
branch, so a flat group whose diff is an argument-order problem is dead on
arrival. Picking branch groups is how the three lever functions above were
found.

## gcc-2.96 narrows a mask that feeds a byte store

The batch's other finding, and it explains two separate parks.

```c
s[9] = (s[9] & ~0xc) | ((f & 3) << 2);
```

gcc emits a single `mov r3, #0xf3`, because the result is stored to a byte and
the top 24 bits provably cannot matter. The ROM materialises the full
`0xfffffff3` with `mov r3, #0xd / neg r3, r3` and comes out one instruction
longer. **No spelling of the constant changes this** — `-0xd` and `~0xc` give
identical output.

What *does* change it is forcing the mask through an `int` local negated in its
own statement — but **only** as `v &= m`. Writing `m &= v` collapses it straight
back to the narrowed single `mov`. That asymmetry cost four screens.

The same narrowing parks `OvlFunc_931_2008c0c` at **1 of 24**, where the ROM
builds the identical constant by *subtracting from a zero it already held*. Two
ROM spellings, one gcc behaviour, one class.

## Parked

- **The sprite-flags setter**, five identical copies, at **4 of 11** with the
  tail exact — the mask narrowing above, plus a load position and an r2-vs-r3
  choice that are allocation rather than expression shape.
- **`OvlFunc_931_2008c0c`** at **1 of 24** — same narrowing, different ROM
  spelling.
- **`Func_8079c30`** at **4 of 19** on **multiply operand canonicalisation**.
  Thumb `mul` is destructive, so whichever operand gcc puts first becomes the
  destination, and it does not take that from the source. Four spellings tried;
  the one that says "a is the destination" outright (`a *= t`) is the **worst**
  of them at 18 lines against 19, because it lets gcc reuse the parameter
  registers differently.
- **The 14-member flat arg-interleave group**, as a class note.

## Housekeeping

A tenth `.global` was added to split `OvlFunc_937_2008308` out, verified
byte-neutral before the split as `tools/split_s.py` insists. `HANDOFF.md`'s
count is updated.
