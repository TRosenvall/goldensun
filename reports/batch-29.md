# Batch 29 — 5 functions, and the parked set as a work queue

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–28 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted. Every address
below read back from the linked ELFs and every path confirmed to exist.

## Read this if you inherit the parked set

**Three of the five functions here were parked with C that was simply wrong**,
under diagnoses that read as plausible codegen problems. That brings the running
total to six.

| function | filed as | actually |
|---|---|---|
| `OvlFunc_911_20081ac` | "logic faithful… endgame permuter seed" | compared against `_AREA_38`; the ROM wants `0x26` |
| `HeightTile_7` | "reg-alloc/scheduling divergence; logic correct" | index written `param_2 * 16 + param_3`; the ROM transposes them |
| `OvlFunc_907_20080dc` | "scalar `.L` pool" class | **two** wrong symbols — an invented `SpecialExitTag` for an area id, and a `.L` label where the ROM returns a real global |

**Why they survived:** a wrong constant produces a well-formed
single-instruction diff that looks exactly like allocation noise. The diagnosis
explains the diff, so nobody looks further.

**The check that finds them**, now automated in `tools/audit_parks.py`: on a
short diff, ask whether the differing operand is a **value or a name** rather
than a register. Registers differing is noise; a value differing is a bug in the
C.

That check needed two guards, and both were wrong on the first attempt — worth
knowing if you extend it:

* A **transposition** also produces differing operands at the same index — both
  instructions exist on both sides, just swapped. That was six of the first
  eight hits. Only report an operand appearing **nowhere** on the other side.
* That cross-check was then written testing each operand against the stream it
  came *from*, which is vacuously true, so the tool reported nothing. **A filter
  that rejects nothing looks exactly like a filter that works.**

## Ranking the parked set

Screening every park and sorting by how many positions differ turned out to be
the highest-yield thing available. Three sat at a **single** differing
instruction; two of those were the semantic bugs above.

The parked set has now out-produced the fresh-candidate list for four
consecutive batches. A "tried, did not work" line in a park records an
*experiment*, not a proof — several have turned out to be the right idea applied
in the wrong place.

## Functions

| function | address | where | note |
|---|---|---|---|
| `OvlFunc_911_20081ac` | `0x020081ac` | rom_79e5c0 | **unparked**, wrong constant |
| `HeightTile_7` | `0x08011e2c` | rom_9000 | **unparked**, transposed index |
| `OvlFunc_907_20080dc` | `0x020080dc` | rom_79b154 | **unparked**, two wrong symbols |
| `OvlFunc_926_200a508` | `0x0200a508` | rom_7b2078 | sanctum attendant |
| `OvlFunc_937_20081fc` | `0x020081fc` | rom_7c3044 | sanctum attendant |

## Operand order decides the destination register

`HeightTile_7` had **two bugs stacked**, and fixing the first exposed the
second. With the index transposition corrected, the sum still accumulated into
the wrong register — and the operand order of the addition decides that:

    param_3 * 16 + param_2    ->  add r2, r1
    param_2 + param_3 * 16    ->  add r1, r2

gcc writes the result into whichever operand it evaluated first. The same holds
for `&` (shown on the `20089dc` family), so it is a general rule and not a quirk
of `add`. **Try both spellings before calling a one-register difference
"allocation".**

## The facing-test family, catalogued

Five functions now share a facing range check, in three distinct spellings. The
ROM says which the original used:

| spelling | ROM shows |
|---|---|
| `f - 0xa001 <= 0x3ffe` on a `u32` | no shift at all |
| `(u16)(f - 0x6001) <= 0x7ffe` | one `lsl #16` against a **pre-shifted** constant |
| `(u16)(f + 0x5fff) <= 0x3ffe` | same, but a small positive constant pooled |

A pre-shifted constant in a comparison is a tell for a narrowing cast; no shift
means the test was at full width.

## Progress on two families that did not close

* **`20089dc` (five clones)** went from 9 differing positions to 4, with the
  instruction count now matching, on three rules already in the tree that had
  never been applied to it — a named `int` mask for `narrow_constant`, an `int`
  local for the field read, and the `&` operand order above. The park's own TODO
  had proposed the first and it was never tried. What remains is scheduling.
* **`OvlFunc_881_20082cc`** went from 10 instructions against 13 to 13 against
  13, by fixing a missing **dereference** (it took the address of a symbol that
  holds a pointer) and an `ldrsh`-versus-`ldrh` trap already documented. What
  remains is a **pooled zero** — the ROM loads `0` from the literal pool where a
  `mov` encodes, so that operand was a symbol whose value is zero. That is now a
  two-member class with `rom_b09fc.c`, and unlike the area ids there is no
  second signal saying what the name should be. Recorded rather than guessed.

## Counts

276 functions elevated in total. 3,023 hand-written functions remain in `asm/`
of 5,714. 92 parked functions, of which 4 document blocker classes rather than
individual functions.
