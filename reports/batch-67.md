# Batch 67 — naming as elevation, and two rules I had over-generalised

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.
3068 sources checked, every elevated `.c` has a tracked `.s`; 0 orphans.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_946_2008d48` | `02008d48` | ovl_7ced6c | [ovl_30_c_c_a_a_a_a.c](../src/overlays/rom_7ced6c/ovl_30_c_c_a_a_a_a.c) |
| `OvlFunc_953_2008238` | `02008238` | ovl_7d95dc | [ovl_30_c_c_a_b.c](../src/overlays/rom_7d95dc/ovl_30_c_c_a_b.c) |
| `OvlFunc_957_2008c2c` | `02008c2c` | ovl_7e3e08 | [ovl_30_c_c_a_c_c_c_c_c_c_a_b.c](../src/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a_b.c) |
| `OvlFunc_963_200808c` | `0200808c` | ovl_7ec968 | [ovl_30_c_c_a_a_a.c](../src/overlays/rom_7ec968/ovl_30_c_c_a_a_a.c) |
| `OvlFunc_968_200af8c` | `0200af8c` | ovl_7f2f14 | [ovl_30_c_c_a_a_c_a_c_b.c](../src/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_c_b.c) |

## Eight symbols added — naming as the elevation

`_AREA_7e 86 8d 8f 90 92 a9 b6` were added to `area.sym`. Each is **compared
against the halfword at gState+0x1C0**, which is `area.sym`'s own stated
criterion for the namespace. Eleven functions were blocked on them.

**Each was previously defined only in `file_table.sym`.** Not a contradiction —
a file id and an area id may share a number, and 95 small values already collide
across the four `.sym` files. The **consumer** distinguishes them.

This was found by reading `OvlFunc_common1_588`, where `0x8f`/`0x90` are compared
against the area field while the message ids passed to `__MessageID` are
`0x2076`/`0x2078`/`0x207a`. A value-based reading would have called them file
ids — precisely the error the consumer rule exists to prevent.

The addition asserts the **namespace, not the meaning** — the same "named by
value, pending semantic names" convention `message.sym` already uses.

### Three independent lines of support, none of them proof

1. **A flag run.** `OvlFunc_946_2009494` computes `0x8c8 + (area - 0x7e)` — a
   per-area flag base. `(int)(&_AREA_7e)` reproduces it exactly, subtract
   operand order included. A run based at an unrelated *file* id would be a
   coincidence. This also shows an `_AREA_` symbol works in **arithmetic**, not
   only comparison.
2. **A consecutive triple.** `OvlFunc_953_2008238` uses `_AREA_8c/8d/8e`; only
   the middle one was missing.
3. **A consecutive run of six.** `OvlFunc_968_200af8c` uses `_AREA_b5`…`_AREA_ba`
   with the added `_AREA_b6` among them.

### And one thing that does not reconcile

`area.sym`'s header states *"52 unused values inside the range"*. A direct count
of the definitions gives **57** before this change and 49 after. The two may
count different things — defined ids versus ids some function actually compares
— or one may be stale. **Recorded unresolved rather than smoothed over**; my
first draft of the comment cited the header's 52 and claimed the change filled
eight of them, and that arithmetic does not hold.

## Two rules I had over-generalised

Both were derived from real evidence and then applied past its conditions.

**The signed lower-bound floor applies only to IMMEDIATE comparisons.** Batch 55
established that gcc rewrites every signed lower bound to `cmp #(K-1) / ble`
where the ROM has `cmp #K / blt`. True against an immediate. When the bound is a
**symbol**, `(int)(&_AREA_7e)` is not a constant gcc can decrement, so the
comparison is register-to-register and `blt` comes out exactly as the ROM has
it. `OvlFunc_946_2008d48` range-tests `_AREA_7e <= area <= _AREA_86` and matches.

Two rounds earlier I had narrowed candidate selection to exclude **every**
function containing a signed range branch, and reported that as sensibly
avoiding a known floor. It was too coarse. Correcting it took the candidate list
from 26 to **40**, and `OvlFunc_968_200af8c` is one of the 14 recovered.

**The consumer rule counted only comparisons.** After the flag-run finding above,
`sym_candidates.py` gained a WEAK tier: functions that read the area field but
use their pooled values some other way are now listed for reading rather than
silently dropped.

## Smaller things worth keeping

- **Two out-of-range paths need a shared `goto out;`**, not two `return`
  statements. gcc merges two returns into a block it places differently — six
  instructions on `OvlFunc_946_2008d48`.
- **`_AREA_b6` routes to the default**, not to a table of its own, so its arm is
  a `goto` and not a return. Reading the branch targets rather than assuming
  symmetry.
- **Five identical calls are written out, not looped** (`OvlFunc_957_2008c2c`) —
  the ROM has them unrolled and a loop would add an induction variable and a
  branch.

## State

40 area candidates remain under the corrected filter. The `_FILE_` and `_MSG_`
pools are still untouched and need their consumers identified — `sym_candidates.py`
deliberately refuses to classify them from the value.
