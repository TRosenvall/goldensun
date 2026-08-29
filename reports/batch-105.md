# Batch 105 — a lever that was already written down

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of its overlay's linked ELF with
`arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_948_2009fd8` | `02009fd8` | ovl_7d30e0 | [ovl_30_c_c_c_c_c_c_c_c_c_b.c](../src/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c_b.c) |
| `OvlFunc_888_20085cc` | `020085cc` | ovl_7892c8 | [ovl_30_c_c_a_a_a_c_a_a.c](../src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_a.c) |
| `OvlFunc_911_2008304` | `02008304` | ovl_79e5c0 | [ovl_30_c_a_a_c_a_a_a_c_b.c](../src/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c_b.c) |
| `OvlFunc_943_2008a48` | `02008a48` | ovl_7c7b9c | [ovl_30_c_a_a_c_a_a.c](../src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_a.c) |
| `OvlFunc_943_2008af0` | `02008af0` | ovl_7c7b9c | [ovl_30_c_a_a_c_a_a.c](../src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_a.c) |

**Every one of these five was already parked.** No new function was attempted
from assembly this round. 2504 functions remain in assembly, and 227 are parked.

## The finding: batch 104 was wrong, and the correction is procedural

Batch 104 parked three functions on what it called "the r0-against-a-shift
rotation", recorded it as a shape the argument levers do not reach, and put it
in the report as five functions on one unsolved class. Every one of those
functions closes with a two-line change, and the change is the **basic-block
lever** already written up at `docs/elevation.md` line 1329.

The reason the class looked new is worth being precise about. Batch 104 tried,
per function: an unprototyped callee declaration, the return-type lever,
`-fno-gcse`, `-fno-rerun-cse-after-loop`, `-fno-cse-follow-jumps`,
`-fno-force-mem`, `-fno-expensive-optimizations`, and naming values adjacent to
the call. **Every one of those is a property of the CALL SITE.** The lever is
about where the value is ASSIGNED — and the lever's own write-up says so, in
those words, including the sentence "the trigger is not at the call site at
all".

So the procedural lesson is not "read the docs". It is narrower and more useful:
**when a park's list of failed attempts is long and every entry varies the same
axis, that is evidence about the axis, not about the blocker.** Eight failed
call-site experiments should have been read as "stop varying the call site".

`OvlFunc_943_2008a48` makes the same point at closer range. Its park, written in
batch 96, records trying "naming 0x103 as an `int` local assigned **inside the
else-block**". That is the exact case the lever's table calls out as the one
that fails — same basic block as the use, so gcc keeps it in a register. The
right experiment was one line away and the park had already half-written it.

## What the lever did

    OvlFunc_948_2009fd8   12 differing  ->  0     (four SetPos calls at once)
    OvlFunc_911_2008304    2 differing  ->  0
    OvlFunc_888_20085cc   25 differing  ->  22    (then a goto for the rest)
    OvlFunc_943_2008a48    2 differing  ->  0     (two functions)
    Task_BlitAnim         29 differing  ->  3

Each is the same edit: assign the value to a named local in a block that
dominates the call and is not the call's own block, then use it once.

```c
case 12:
    x0 = 0xe8 << 16;              /* case 12's block */
    y0 = 0xda << 18;
    if (__GetFlag(0xee7) == 0)
        __MapActor_SetPos(8, x0, y0);   /* the if body: a different block */
```

gcc then rematerialises each coordinate at the call as a split `mov`/`lsl` pair
with the slot number scheduled into the gap, which is the ROM's shape.

## It reaches pool-loads-first, which the docs said it did not

`docs/elevation.md`'s "Pool loads come first" section lists this shape —

```
rom    mov r2, #0 / mov r0, #0x15 / ldr r1, =0x103 / bl __MapActor_Emote
ours   mov r2, #0 / ldr r1, =0x103 / mov r0, #0x15 / bl __MapActor_Emote
```

— as reachable "only by register pinning with inline asm". `OvlFunc_943_2008a48`
and `OvlFunc_943_2008af0` close on the lever with no asm. The section has been
corrected: **the lever reaches it whenever there is a boundary to use**, and the
inline-asm note applies to the straight-line members of that class, which is
what its four catalogued examples happen to be.

## The dosage is not monotonic

`Task_BlitAnim` has seven sites using the same `0x4000`. Levering them:

| locals | differing |
|---|---|
| 0 (literals everywhere) | 29 |
| 2 (case 1's two uses) | 5 |
| 3 (adding case 0's) | **3** |
| 7 (every site) | 11 |

That is the `REG_N_REFS == 2` clause in the lever's write-up biting from the
other side. Past some point CSE merges the locals back into one pseudo and the
pseudo is referenced too often for local-alloc to rematerialise it. **Apply the
lever to the sites that are wrong, not to every site that could take it.**

## Where the lever stops — two new negatives, both measured

`OvlFunc_929_2008598` (4 of 55) and `OvlFunc_956_2008b30` (3 of 47) both have
real boundaries and neither moves. Four placements were compiled on the first
and three on the second.

* **2008598**: the ROM defers its `lsl` past *all three* remaining argument
  moves, so it is the last instruction before the `bl`. Where the lever works,
  the ROM's gap holds one or two moves. A gap that swallows every other
  argument is the scheduler, not rematerialisation — and the park's own note
  agrees, because only the FIRST of two identically-written calls in that arm
  does it.
* **2008b30**: the blocker is *which register* holds a pooled mask, not where
  it is built. The lever decides where a value is rematerialised and does not
  touch register choice.

`Anim_UnleashIntro` is the third negative and the most informative. Three
placements, all 2 of 80, because gcc is **already** splitting the pair:

```
rom    mov r0, #0xa0 / ldr r3, =Func_8001af8 / mov r2, #0x80 / lsl r0, #19
ours   mov r0, #0xa0 / ldr r3, =Func_8001af8 / lsl r0, #19 / mov r2, #0x80
```

The lever's job is to make gcc rematerialise rather than hold, and it is doing
that. *Which* of the remaining arguments gcc schedules into the gap is a
separate question the lever does not answer. That is its other boundary,
alongside the straight-line-function one.

## One label can be the whole diff

`OvlFunc_888_20085cc` sat at 96 lines against 95 after the lever, with the extra
line being a **label**, not an instruction. The ROM's `.L6a0` does three jobs at
once: jump-table target for cases 0x1d/0x20/0x23, `b` target from the
0xa/0xb/0xc arm, and C fallthrough from case 0x14.

Ending the 0xa/0xb/0xc arm with its own `__ClearFlag(0x12f); break;` produces
the right instructions — gcc cross-jumps the duplicate away — but leaves a
second coincident label behind. A `goto` to the join makes it one label:

```c
    case 0xa: case 0xb: case 0xc:
        ...
        goto clear;
    case 0x14:
        ...
        /* falls through */
    case 0x1d: case 0x20: case 0x23:
    clear:
        __ClearFlag(0x12f);
        break;
```

Worth knowing because a one-line length difference reads as a missing or extra
instruction, and the tooling counts labels as lines.

## Operational: exporting a `.L` symbol to split a file

`OvlFunc_948_2009fd8` drives a frame counter at `.L2f80` that lives in the same
`.s`. `tools/split_s.py` refused the split rather than producing a link error,
and the fix is `.global .L2f80` — which emits no bytes. Done as its own commit
with `make compare` green before and after, so the export and the split stay
separable.
