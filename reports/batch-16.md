# Batch 16 — 11 functions, one family, one solve

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–15 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean build, unassisted. Every address read back from the linked ELFs.

## The functions

Eleven identical copies of one function — turn one step toward the target —
one per overlay, byte-for-byte the same body.

| `OvlFunc_884_2008030` | `0x02008030` | `src/overlays/rom_784360/ovl_30_a_a.c` |
| `OvlFunc_885_2008030` | `0x02008030` | `src/overlays/rom_78603c/ovl_30_a_a.c` |
| `OvlFunc_886_2008030` | `0x02008030` | `src/overlays/rom_786f0c/ovl_30_a_a_b.c` |
| `OvlFunc_887_2008030` | `0x02008030` | `src/overlays/rom_787e04/ovl_30_a_a.c` |
| `OvlFunc_896_2008314` | `0x02008314` | `src/overlays/rom_78ef88/ovl_314_a_a.c` |
| `OvlFunc_907_2008030` | `0x02008030` | `src/overlays/rom_79b154/ovl_30_a_a.c` |
| `OvlFunc_909_2008030` | `0x02008030` | `src/overlays/rom_79c738/ovl_30_a_a.c` |
| `OvlFunc_910_2008030` | `0x02008030` | `src/overlays/rom_79dd90/ovl_30_a_a.c` |
| `OvlFunc_911_2008114` | `0x02008114` | `src/overlays/rom_79e5c0/ovl_30_a_c_a_b.c` |
| `OvlFunc_921_20080d8` | `0x020080d8` | `src/overlays/rom_7a7298/ovl_30_a_a_b.c` |
| `OvlFunc_932_2008040` | `0x02008040` | `src/overlays/rom_7b9cb4/ovl_30_a_a_c.c` |

## What was load-bearing: the width of one local

The function takes the angle to its target with `atan2`, clamps the change to
±`0x1000` of a full circle, and applies it. The ROM zero-extends the `atan2`
result to sixteen bits **before** subtracting the current facing:

    lsl r0, #16 / lsr r0, #16 / sub r0, r3 / lsl r0, #16 / asr r0, #16

Written with the angle as an `int` and the cast in the expression —
`(short)((unsigned short)ang - cur)` — gcc drops the first pair. It is right to:
masking before a subtraction whose result is truncated to sixteen bits anyway
is redundant, and the output is two instructions shorter than the ROM's.

Declaring the angle as an `unsigned short` **local** forces the narrowing to
happen at the assignment, where the ROM has it.

Declaring `__atan2` as *returning* `unsigned short` matches equally well, so
this function does not settle which the original did. The local is used because
it claims less — `__atan2` is shared, and its signature should be pinned by a
function that actually determines it.

## A note on how the family was found

`tools/find_families.py`, seeded from a member I had **not** already elevated.

That detail is the whole reason batch 16 has no correction in it. Batches 12
and 13 reported three families complete and all three counts were wrong,
because the sweep's criterion had been derived from the first member I read and
silently excluded members that differed in an incidental way. Seeding from a
different member is what exposes that — you cannot see what a criterion
excludes by looking at the function it was built from.

Here the sweep found 11, and 11 matched. No revision.

## Two park notes corrected

- **`free` (0x08002df0) and `FindEntityAtPosition` are not the same problem.**
  Batch 14 recorded them as sharing a residue — exact instruction sequence,
  different register assignment — and cross-referenced them as "a better lead
  than either alone". `FindEntityAtPosition` is now solved, and its fix
  (indexing a base instead of walking a pointer) was tried on `free` in three
  forms and fails in all of them. They share a **symptom**, and that symptom
  has more than one cause.

- The `narrow_constant` note's claim that the residue is "register birth order"
  was already corrected in batch 12; batch 15 added that gcc-2.96 allocates by
  **priority**, not birth order, and that "birth order" in `docs/elevation.md`
  is a simplification that only holds for short functions.

## Still open, and still only answerable by you

- **Five ambiguous offsets in `actor.h`** (batch 03), documented rather than
  guessed.
- **`narrow_constant`**, 34 functions, down to one peephole: gcc folds the mask
  to `sub r3, #0x10` because a `3` is live and `3 - 0x10 == ~0xc`.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
