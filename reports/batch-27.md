# Batch 27 — 5 functions, and two rules narrowed by their exceptions

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–26 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted. Every address
below read back from the linked ELFs and every path confirmed to exist.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `OvlFunc_934_20096f0` | `0x020096f0` | rom_7bdeb0 | pool tell reuses `_AREA_5d` |
| `OvlFunc_956_2008a44` | `0x02008a44` | rom_7e0928 | one declaration |
| `OvlFunc_956_200858c` | `0x0200858c` | rom_7e0928 | two shapes that only look like blockers |
| `OvlFunc_883_200da94` | `0x0200da94` | rom_780898 | do **not** tidy the source |
| `OvlFunc_959_2008b4c` | `0x02008b4c` | rom_7e7574 | pointer derived by subtraction |

## Write the ROM's arithmetic, not the tidy form

Two functions in this batch fail if the C is written the way a person would
naturally write it, and they fail in **opposite directions** — which is why both
are worth having.

**`OvlFunc_883_200da94` — do not hoist.** Its two arms each recompute the same
address and only the store is shared:

    .L5acc:  mov r2, r0 / add r2, #0x23 / mov r3, #1
    .L5ad2:  strb r3, [r2]

Written tidily — one `u8 *p = (u8 *)b + 0x23;` above the branch and a ternary —
gcc hoists the add, the arms collapse to a `mov` each, and it is two
instructions short. Writing the whole store out redundantly in **both** arms
produces the ROM's shape: gcc cross-jumps the common `strb` on its own and
leaves the address arithmetic where the source put it.

That is the pre-header load merge (`src/non_matching/preheader_load_merge.c`),
normally a blocker, running in our favour. The job is to not pre-empt gcc's
tail-merging.

**`OvlFunc_959_2008b4c` — do not recompute.** The ROM walks one pointer
backwards to reach the second field:

    add r2, #0x55 ... sub r2, #0x32     (0x55 - 0x32 = 0x23)

Written as two independent `(u8 *)a + 0x55` and `+ 0x23` expressions, gcc emits
two adds from the base and the `sub` never appears. `p -= 0x32;` on the live
pointer is what produces it.

## The area namespace was reused, twice, rather than extended

`OvlFunc_934_20096f0` compares with `ldr r3, =0x5d` where `cmp r2, #0x5d` encodes
fine — the pool tell, so that operand was a symbol. It reads from
`gState + 0x1c0`, the same halfword `GetEntrances` compares, so it is an area
id — **and `_AREA_5d` was already defined in `area.sym`** from an earlier batch.
`OvlFunc_922_2009750` (parked, below) does the same with `_AREA_34`.

This is the first independent support the namespace has had since it was
adopted. Symbols defined for one overlay turned out to be exactly what unrelated
functions in *other* overlays needed, at the same struct offset. That is not
proof, but it is a prediction that would have failed if the inference were
wrong.

## A counter-example that narrows the CSE-flag rule

Batch 26 recorded that `-fno-rerun-cse-after-loop` applies when a repeated
constant's uses are separated by a **call**. `OvlFunc_922_2009750` is exactly
that shape — the offset `0x1c0` built twice with `__GetFlag` between them — and
**the flag does nothing**, byte-identical with and without it. It is parked at
30 instructions against 33.

So the rule is narrower than stated. The lead, recorded as a lead and not a
finding: every flag-responsive case so far repeats a value used as a **call
argument**, while this one repeats a value used in **address arithmetic**. Three
cases against one.

`src/non_matching/overlays/constant_reuse.c` carries the correction.

## Two shapes that look like blockers and are not

From `OvlFunc_956_200858c`, both worth having as screening heuristics:

* **Two stack arguments filled before the register ones** is just what gcc does.
  The parked stack-arg class is about the *order* of the slots relative to the
  registers, not their presence.
* **A constant materialised twice inside one argument block** is not
  constant-CSE. There is no call between the uses, so gcc rebuilds it exactly as
  the ROM does. The value in the same function that *is* shared across a call
  needs a named local.

## Tooling

`tools/pick_candidates.py` now skips functions that are already parked. On the
first run after a break, four of the top five rows were functions already
attempted and characterised, each costing a read to recognise.

## Counts

263 functions elevated in total. 3,036 hand-written functions remain in `asm/`
of 5,714. 96 parked functions, of which 4 document blocker classes rather than
individual functions.
