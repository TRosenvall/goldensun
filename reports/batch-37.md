# Batch 37 — five functions through a blocker that stood for 36 batches

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–36 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted — 96 overlays
compared byte-for-byte and `goldensun.gba: OK`. Every address read back from the
linked overlay ELF with `nm`.

## Read [reports/arg-interleave.md](arg-interleave.md) first

These five are the verification of a lever, not the point of the batch. The
lever retires **two** blocker classes and unblocks **516 functions**.

In one line: **assign an argument's constant to a named local in a DIFFERENT
BASIC BLOCK from the call.** Crossing a block boundary stops gcc-2.96 keeping
the value in a register, so it rematerialises at the call — and its rebuild of a
two-instruction constant is the split pair the ROM has, with the other argument
scheduled into the gap.

    int x;
    x = 0x88 << 18;              /* assigned here */
    a = __MapActor_GetActor(8);
    if (a != 0) ...;             /* <-- a basic-block boundary */
    __Func_8012078(0, x, ...);   /* used here, in a different block */

A **call does not create a boundary; only a branch does.** That is why the class
survived: every function it had ever been attempted on was straight-line.

## The two classes were one class

Filed separately in batches 32 and 36 because the symptoms look unrelated:

    arg-interleave      rom  mov r1,#imm / mov r0,#imm / lsl r1,#n
    pool-loads-first    rom  mov r0, r5 / ldr r1,=X / ldr r2,=Y

One displaces a shift, the other a pool load. They are the same mechanism, and
`OvlFunc_969_2009280` — **parked in batch 36 under the second class** — is
matched here by the lever found for the first. Both park notes are corrected.

## Functions

| function | address | overlay | was |
|---|---|---|---|
| `OvlFunc_949_2008224` | `0x02008224` | rom_7d4af4 | blocked, 2 differing |
| `OvlFunc_946_2009508` | `0x02009508` | rom_7ced6c | blocked |
| `OvlFunc_946_2009548` | `0x02009548` | rom_7ced6c | blocked |
| `OvlFunc_946_200958c` | `0x0200958c` | rom_7ced6c | blocked |
| `OvlFunc_969_2009280` | `0x02009280` | rom_7f6e64 | **parked, batch 36** |

## The limit, and it is a real one

**A straight-line function cannot use this** — there is no boundary to put
between the assignment and the call, and writing it that way makes things
*worse*: gcc keeps the value in a callee-saved register and pays a push/pop.
`OvlFunc_908_20081a8` goes from 2 differing instructions to 6;
`OvlFunc_882_20083cc` from 2 to 4.

Of 1,025 remaining functions carrying either shape, **516 have a basic-block
boundary before their first site** and 509 are straight-line. The straight-line
ones stay parked, and their notes now say why so nobody re-attempts them.

*(Corrected after publication: the first version of the site detector counted a
store's value as argument setup -- `mov r3,#0 / strh r3,[r0,#6]` before an
unrelated pooled load -- which inflated the population to 1,861 and the
reachable count to 417. The ratio was right; the absolute numbers were not.)*

## What the straight-line cases still need

A way to make gcc rematerialise a value **inside** a single basic block. That is
the same missing construct as:

* `OvlFunc_882_200c5b8` (batch 32) — gcc holds both masks where the ROM rebuilds one
* the `-1` triple in `src/non_matching/ovl_787e04/20093e4.c` — gcc builds `-1`
  once and copies; the ROM builds it three times

**Three parked shapes, one missing construct.** That is the next thing worth a
round, and the method that cracked this one applies: search gcc's own output for
places it already does the wanted thing, rather than generating variants.

## Counts

326 functions elevated in total. 2,969 hand-written functions remain in `asm/`
of 5,714. 92 parked functions plus the two large-function experiments, and 6
files documenting blocker classes — two of which are now marked solved for the
branching case.
