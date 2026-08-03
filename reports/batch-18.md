# Batch 18 — 14 functions, a mirrored pair of effect families

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–17 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean build, unassisted. Every address read back from the linked ELFs.

## The functions

Two families of seven, laid down as a **pair** in each of seven overlays.

| `OvlFunc_881_200bfb4` | `0x0200bfb4` | `src/overlays/rom_77a7c8/ovl_30_c_c_c_c_b.c` |
| `OvlFunc_881_200c004` | `0x0200c004` | `src/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_b.c` |
| `OvlFunc_882_200c378` | `0x0200c378` | `src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_c_c_c_b.c` |
| `OvlFunc_882_200c3c8` | `0x0200c3c8` | `src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_c_c_c_c_b.c` |
| `OvlFunc_883_200dcc4` | `0x0200dcc4` | `src/overlays/rom_780898/ovl_30_c_c_c_c_c_c_c_b.c` |
| `OvlFunc_883_200dd14` | `0x0200dd14` | `src/overlays/rom_780898/ovl_30_c_c_c_c_c_c_c_c_b.c` |
| `OvlFunc_884_200a39c` | `0x0200a39c` | `src/overlays/rom_784360/ovl_30_c_c_c_a_a_c_b.c` |
| `OvlFunc_884_200a3ec` | `0x0200a3ec` | `src/overlays/rom_784360/ovl_30_c_c_c_a_a_c_c_b.c` |
| `OvlFunc_887_20095e8` | `0x020095e8` | `src/overlays/rom_787e04/ovl_30_c_c_a_a_b.c` |
| `OvlFunc_887_2009638` | `0x02009638` | `src/overlays/rom_787e04/ovl_30_c_c_a_a_c_b.c` |
| `OvlFunc_897_200ae0c` | `0x0200ae0c` | `src/overlays/rom_791794/ovl_30_c_c_c_a_a_c_b.c` |
| `OvlFunc_897_200ae5c` | `0x0200ae5c` | `src/overlays/rom_791794/ovl_30_c_c_c_a_a_c_c_b.c` |
| `OvlFunc_969_200a15c` | `0x0200a15c` | `src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_a_a_c_b.c` |
| `OvlFunc_969_200a1ac` | `0x0200a1ac` | `src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_a_a_c_c_b.c` |

## What they do

A 32-frame effect actor riding on a parent. Each frame it advances a counter in
`goalFacing`, deletes itself once past `0x1f`, and otherwise takes `sin()` of
the counter scaled to a full circle, using it for both the `0x18`/`0x1C` pair
and a height offset above the parent. x tracks the parent exactly; y climbs a
fixed `0x10000` per frame.

The second family is the **mirror** of the first — three signs changed:

    rotY  = -sin                            rather than +sin
    z     = parent - (0x10000 - sin) * 5    rather than +
    offset  0x100000                        rather than 0x80000

Both halves appear in all seven overlays that have either, which is what a
symmetric effect — one on each side of the parent — looks like.

## The one load-bearing detail

The counter is read back through a **signed 16-bit narrowing after the
increment**. The ROM does `lsl #16 / asr #16` on the *stored halfword* rather
than using the value it just computed:

    a->goalFacing++;
    n = (short)a->goalFacing;     /* re-read, then narrow */

Keeping the incremented value in an `int` does not produce that.

## What this batch does NOT settle

`include/actor.h` records `0x18`/`0x1C` as genuinely undecided — "a scale pair,
or a rotation pair" — with the draw path reading them one way and the movement
path the other, and a note that both readings cannot be true.

These fourteen functions apply a 32-frame sine to **both members at once**,
which fits either reading equally well. The fields are named `rot*` here
following the header, and each file says explicitly that this function does not
decide it.

That is deliberate. A tentative name used without comment hardens into an
assertion — which is exactly how `MapEntrance_ARRAY_*` came to be counted as
evidence in batches 13 and 14 before being corrected in batch 15.

## Method note: two families that were one thing

`tools/find_families.py` reported these as separate groups, at 38 and 39
instructions. That is correct — `neg` and `sub` really are different
instructions from `add`, and a shape comparison should not smooth that over.

The signal worth following was that **both groups appeared in the same seven
files**. Reading the second as "the first with three signs changed" rather than
as an unrelated 39-instruction function made it a first-try match.

## Five families closed in sequence

| Family | Members | Instructions each |
|---|---|---|
| `FindEntityAtPosition` | 17 | 40 |
| turn-toward-target | 11 | 40 |
| `FillMapRectCollisionByte` | 18 | 46 |
| sine effect | 7 | 38 |
| sine effect, mirrored | 7 | 39 |

**60 functions from five solves**, all real code rather than templated stubs.

## Still open, and still only answerable by you

- **Five ambiguous offsets in `actor.h`** (batch 03), documented rather than
  guessed. The `0x18`/`0x1C` pair above is one of them, and this batch adds
  usage without resolving it.
- **`narrow_constant`**, 34 functions, down to one peephole: gcc folds the mask
  to `sub r3, #0x10` because a `3` is live and `3 - 0x10 == ~0xc`.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
