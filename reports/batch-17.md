# Batch 17 — 18 functions, one family, three load-bearing details

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–16 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean build, unassisted. Every address read back from the linked ELFs.

## The functions

Eighteen identical copies of `FillMapRectCollisionByte`, one per overlay,
byte-for-byte the same body. It writes one byte into every cell of a rectangle
of the map's cell array — rows `0x200` bytes apart, cells 4 bytes, the byte
landing at `+2` of the cell, which is the collision/attribute byte rather than
the metatile index. A null `[iwram_3001e70]` makes it a no-op.

This is how a log occupies space: the caller stamps `0xFF` at the destination
and `0` at the origin, on layers 0 and 2.

| `OvlFunc_883_2008244` | `0x02008244` | `src/overlays/rom_780898/ovl_30_a_a_a_c_c_b.c` |
| `OvlFunc_905_2008244` | `0x02008244` | `src/overlays/rom_799abc/ovl_30_a_a_a_c_a_c_b.c` |
| `OvlFunc_913_2008244` | `0x02008244` | `src/overlays/rom_7a04ac/ovl_30_a_a_a_c_a_c_b.c` |
| `OvlFunc_914_2008244` | `0x02008244` | `src/overlays/rom_7a1ff0/ovl_30_a_a_c_a_c_b.c` |
| `OvlFunc_915_2008244` | `0x02008244` | `src/overlays/rom_7a2bf0/ovl_30_a_a_a_c_c_b.c` |
| `OvlFunc_923_2008528` | `0x02008528` | `src/overlays/rom_7aa430/ovl_314_a_c_a_c_b.c` |
| `OvlFunc_924_2008528` | `0x02008528` | `src/overlays/rom_7ac2d8/ovl_314_a_c_a_c_b.c` |
| `OvlFunc_927_2008244` | `0x02008244` | `src/overlays/rom_7b4558/ovl_30_a_a_c_a_c_b.c` |
| `OvlFunc_934_2008528` | `0x02008528` | `src/overlays/rom_7bdeb0/ovl_314_a_a_a_c_c_b.c` |
| `OvlFunc_946_2008244` | `0x02008244` | `src/overlays/rom_7ced6c/ovl_30_a_a_a_c_c_b.c` |
| `OvlFunc_947_2008528` | `0x02008528` | `src/overlays/rom_7d0e88/ovl_314_a_c_a_c_b.c` |
| `OvlFunc_948_2008244` | `0x02008244` | `src/overlays/rom_7d30e0/ovl_30_a_a_a_c_a_c_b.c` |
| `OvlFunc_957_2008244` | `0x02008244` | `src/overlays/rom_7e3e08/ovl_30_a_a_a_c_a_c_b.c` |
| `OvlFunc_958_2008528` | `0x02008528` | `src/overlays/rom_7e636c/ovl_314_c_a_c_b.c` |
| `OvlFunc_959_2008244` | `0x02008244` | `src/overlays/rom_7e7574/ovl_30_c_a_c_b.c` |
| `OvlFunc_964_2008244` | `0x02008244` | `src/overlays/rom_7ed0a0/ovl_30_a_a_a_c_a_c_b.c` |
| `OvlFunc_965_2008244` | `0x02008244` | `src/overlays/rom_7ef4f4/ovl_30_a_a_a_c_a_c_b.c` |
| `OvlFunc_968_20084f4` | `0x020084f4` | `src/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_a_b.c` |

## Three things were load-bearing, all invisible in the C

**1. Branch polarity.** The ROM branches *away* for `layer > 2`, so the layer
lookup is the fall-through and therefore the `if` body:

    if (layer <= 2) { ... } else { p = gBuffer; }

Written the other way round, gcc inverts the test and the two arms swap.
`docs/elevation.md` records this as a general rule — where the ROM branches away
from the code that follows the test, that code was the `if` body — and this is
the second function it has decided.

**2. The layer offset must be a named local.** The ROM loads with a register
offset, `ldr r0, [r2, r3]`, where `r3` holds `layer * 0x30 + 0x130`. Written
inline, gcc folds the `0x130` into the address and adds twice. Same lever as the
byte offset in batch 12: naming an intermediate stops gcc folding it into its
consumer.

**3. Addend order.** The ROM computes `add r3, r6, r3` — x plus the shifted z.
Written `(z << 7) + x`, gcc emits the destructive `add r3, r6`: identical
arithmetic, different bytes. Written `x + (z << 7)`, it matches.

That third one was the last instruction to differ and reads like a compiler
quirk. It is not — operand order in a two-operand add is a real choice the C
makes, and writing it off would have parked eighteen functions.

## Method: seed a family sweep from a function you have not solved

Batch 16 introduced this and it held again here. The sweep was seeded from
`OvlFunc_905_2008244`, a member that had not been elevated, and returned 17 —
which with the head makes 18, agreeing with `find_families.py`'s independent
count before any work started.

Seeding from the function you just solved bakes its incidental properties into
the criterion, and you cannot see what that excludes by looking at the function
it came from. That is what produced six batches of wrong family counts, corrected
in batch 15.

## A member was nearly lost to a missing newline

Worth recording because nothing downstream would have caught it.

The family list was written to a file with no trailing newline, and a
`while read` loop silently dropped the last line — six members processed where
seven were asked for. No error, no warning.

It surfaced only by comparing the processed count against the size the sweep had
reported. That check is possible because the sweep states its member count up
front; without it, "I split six" and "there were six" are indistinguishable.

Same shape as the sweep-criterion bug: **silent omission**. The screen catches
wrong C. Nothing catches C you never wrote.

## Three families closed in sequence

| Family | Members | Instructions each |
|---|---|---|
| `FindEntityAtPosition` | 17 | 40 |
| turn-toward-target | 11 | 40 |
| `FillMapRectCollisionByte` | 18 | 46 |

**46 functions from three solves**, all real code rather than templated stubs.

## Still open, and still only answerable by you

- **Five ambiguous offsets in `actor.h`** (batch 03), documented rather than
  guessed.
- **`narrow_constant`**, 34 functions, down to one peephole: gcc folds the mask
  to `sub r3, #0x10` because a `3` is live and `3 - 0x10 == ~0xc`.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
