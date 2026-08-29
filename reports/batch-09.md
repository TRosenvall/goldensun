# Batch 09 — 9 functions; the GetEntrances family finished to 17 of 18

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–08 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean build, unassisted. Every address read back from the linked overlay
ELFs.

## The functions

All nine are `GetEntrances` members and all nine came from splits, so the
sibling `.s` files must travel with them.

| Function | Address | New source |
|---|---|---|
| `OvlFunc_906_20083e4` | `0x020083e4` | `src/overlays/rom_79aad8/ovl_314_c_a_c_c_b.c` |
| `OvlFunc_909_2008100` | `0x02008100` | `src/overlays/rom_79c738/ovl_30_c_c_a_a_a_b.c` |
| `OvlFunc_911_2008284` | `0x02008284` | `src/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_b.c` |
| `OvlFunc_921_2008130` | `0x02008130` | `src/overlays/rom_7a7298/ovl_30_a_b.c` |
| `OvlFunc_930_2009180` | `0x02009180` | `src/overlays/rom_7b7f1c/ovl_30_c_c_c_c_b.c` |
| `OvlFunc_938_20080a4` | `0x020080a4` | `src/overlays/rom_7c37ac/ovl_30_c_c_a_b.c` |
| `OvlFunc_945_2008340` | `0x02008340` | `src/overlays/rom_7cb2c0/ovl_30_a_c_c_b.c` |
| `OvlFunc_951_2008044` | `0x02008044` | `src/overlays/rom_7d6418/ovl_30_c_c_a_a_a_b.c` |
| `OvlFunc_964_200a370` | `0x0200a370` | `src/overlays/rom_7ed0a0/ovl_30_c_c_c_c_b.c` |

`unknown_id.sym` gains `_ID_21`, `_ID_27`, `_ID_67`, `_ID_6f`, `_ID_ac`,
`_ID_bd`.

Two `.s` files gain `.global` declarations — see below.

## The family, finished

| | |
|---|---|
| Members | 18 |
| Elevated | **17** |
| Parked | 1 (`OvlFunc_960_2008e5c`, split refused — see below) |

Constants across the family: `0x1d 0x21 0x22 0x27 0x33 0x4a 0x67 0x6a 0x6f
0x8b 0xa6 0xac 0xbd`. They look like map ids. **They are not the namespace of
`__Func_8091eb0`'s first argument**, which the ROM builds with a plain `mov` —
so there are at least two id spaces here that both look like map ids, and that
is worth knowing before anyone names either.

## When a split refusal costs two lines, and when it costs a restructure

`tools/split_s.py` refuses when a local label would cross the boundary it is
about to create. `.L` symbols do not survive into an object's symbol table, so
a label defined in one part and referenced from another is invisible to the
linker — and the failure surfaces much later, looking like a bad
decompilation. The guard exists because that happened.

Three family members hit it. **Two cleared, one did not, and the difference is
the useful part.**

**Cleared (two lines).** The function returns one of two `.incbin` tables
defined in the same `.s`. C cannot carry an `.incbin` into a translation unit,
so the tables stay in assembly and the labels are exported instead:

    .global .L1b10
    .global .L1c9c

A `.global` emits no bytes. This is not a new practice — in `rom_7b7f1c` four
sibling tables were **already** exported for this function's own elevated
neighbours; these two had simply never been needed. Verified in two
deliberately separable steps: `make compare` green after the export and
**before** the split, then green again after.

**Not cleared (a restructure).** `rom_7eaf28/ovl_314_c_c.s` still refuses after
the same treatment. That file holds nine functions and 54 local labels with
only 8 exported, and cutting at the target strands references belonging to the
*other* functions. The target itself needs only its own two tables and its own
branch labels. So the fix is not two exports but dozens, restructuring nine
functions' worth of data to land one 15-instruction stub.

The question to ask is: **does the target reference labels across the cut, or
does the cut land in the middle of someone else's references?** First case, two
lines. Second case, leave it.

A splitter that cut on a label-closed boundary rather than a function boundary
would clear the whole class. Noted as the obvious next tool, not built yet.

## A note on the .global additions

These are the only changes in this batch that touch existing assembly rather
than replacing it, so they deserve to be called out explicitly for review:

| File | Added |
|---|---|
| `asm/overlays/rom_7b7f1c/ovl_30_c_c_c_c.s` | `.global .L1b10`, `.global .L1c9c` |
| `asm/overlays/rom_7ed0a0/ovl_30_c_c_c_c.s` | `.global .L3c0c`, `.global .L3ef4` |

Both files already exported sibling labels in exactly this way. If you would
rather these functions stayed as assembly than have the labels exported, the
change reverts cleanly — drop the four lines, restore the two `.s` files, and
delete the two `.c` files.

## Still open, and still only answerable by you

- **Semantic names for the id namespaces**, now with the extra wrinkle that
  there are demonstrably at least two of them.
- **Five ambiguous offsets in `actor.h`** (batch 03), documented rather than
  guessed.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
