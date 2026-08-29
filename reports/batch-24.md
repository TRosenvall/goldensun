# Batch 24 — 11 functions, and a change in how they are found

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–23 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted. Every address
below read back from the linked ELFs.

## Read this first if you are porting selectively

**Five overlay linker scripts gain a line**, and it is the only change in this
batch that is not a `.c` replacing a `.s`:

    __divsi3 = _divsi3_RAM;

added to `overlays/rom_7a5214`, `rom_7ced6c`, `rom_7ed0a0`, `rom_7ef4f4` and
`rom_7f2f14`. It is an absolute symbol assignment and emits no bytes. See "The
divide helper" below for why it is necessary and what assumption it encodes.

## The find: seven of eleven came from duplicate detection

`OvlFunc_898_2008938` was elevated by taking the `.c` of a function in a
*different overlay*, changing the name, and screening it — it matched first try,
because the two bodies are identical instruction for instruction. That was
noticed by accident.

`tools/find_twins.py` does it deliberately: it groups remaining assembly
functions by normalised instruction body (local labels renumbered; any
difference in a register, immediate or callee makes them distinct). The result:

**41 groups covering 74 functions that can be elevated by solving ONE member
each.** One 139-instruction body appears **13 times**.

This is worth flagging because it changes the expected shape of the remaining
work. Picking candidates one at a time yields roughly one or two per session and
skews harder over time as the tractable shapes get consumed. Solving a body once
and applying it N times does not decay the same way. Camelot evidently shipped
the same source into many overlays.

**Caveat, and the tool reports it:** a large group says nothing about
matchability. The seven-member group headed by `Func_809a44c` is parked on a
scheduling residue (below). The tool flags a group when *any* member is parked —
it originally checked only the representative, which reported that group as
unexplored when it is the best-documented dead end in the tree.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `OvlFunc_936_20083d8` | `0x020083d8` | rom_7c097c | shifted range check |
| `OvlFunc_940_2008224` | `0x02008224` | rom_7c5974 | near-twin of batch 23 |
| `OvlFunc_899_2008378` | `0x02008378` | rom_794ac0 | ROM caches a zero across four calls |
| `OvlFunc_898_2008938` | `0x02008938` | rom_793768 | byte-identical twin of the above |
| `OvlFunc_881_200b448` | `0x0200b448` | rom_77a7c8 | unsigned switch selector |
| `OvlFunc_943_200b464` | `0x0200b464` | rom_7c7b9c | twin |
| `OvlFunc_918_200985c` | `0x0200985c` | rom_7a5214 | per-frame integrator |
| `OvlFunc_946_2008da4` | `0x02008da4` | rom_7ced6c | twin |
| `OvlFunc_964_2009068` | `0x02009068` | rom_7ed0a0 | twin |
| `OvlFunc_965_2008cf0` | `0x02008cf0` | rom_7ef4f4 | twin |
| `OvlFunc_968_200896c` | `0x0200896c` | rom_7f2f14 | twin |

## The divide helper — a link question, not a source one

The five integrator copies screen with exactly one difference:

    rom    bl _divsi3_RAM
    ours   bl __divsi3

Those are **different functions at different addresses**. 348 call sites in this
ROM use `__divsi3`; 104 use `_divsi3_RAM`, which is the stub every overlay's
`imports.s` generates from `.export_func divsi3_RAM`. Overlay code calls a
RAM-resident copy of the divide routine.

gcc-2.96 emits `__divsi3` for `/` and has no flag to rename it, so the fix is
the linker alias above.

**Calling `_divsi3_RAM` directly from C also emits the right instruction and is
wrong.** It makes gcc treat the division as an ordinary call rather than a
libgcc helper; the scheduler then hoists a load above a store two statements
earlier and the function stops matching. The alias keeps `/` in the source,
which is almost certainly what was written.

**The assumption, stated plainly for review:** that overlay translation units
resolved gcc's integer-division helpers to the RAM-resident copies. It is
consistent with every overlay's `imports.s` exporting `divsi3_RAM` and
`udivsi3_RAM`, and with no overlay calling `__divsi3` anywhere in the ROM. If
that is wrong, the alias is the thing to revisit.

## Three smaller tells

**`bcc` in a switch chain means an UNSIGNED selector.** gcc lays a small switch
out as an if-chain and the chain's shape reports the type — signed gives
`cmp/bgt/cmp #0`, unsigned gives `cmp/bcc`. With a signed parameter the chain
also grows four instructions handling negatives the unsigned form rules out. So
this is not a detail of one comparison; it changes the whole switch.

**A pre-shifted constant in a comparison means a narrowing cast.** gcc does not
emit `lsl/lsr` to narrow and then compare against `0x7ffe`; it shifts once and
compares against `0x7ffe0000`. So `(u16)(f - 0x6001) <= 0x7ffe` produces the
ROM's form, while the same test written on a `u32` emits no shift at all.

**In-place versus temp is visible.** The integrator's velX update is in place
(`vx -= vx / 0x12`) and its velZ update is not (`a->velZ = vz - vz / 16`). Write
both the same way and the second stops matching.

## Blockers this batch

**arg-interleave widened.** `OvlFunc_924_2008ffc` puts `mov r0` between two
independent pooled argument loads — the two earlier members had it inside a
single argument's `mov`/`lsl` construction. So the class is about r0's *position*
in the argument block, not any instruction pair. Neither declaration lever
reaches it.

The filter in `tools/pick_candidates.py` was **deliberately not widened to
match.** Generalising it to "r0 with another argument register written both
before and after" was implemented and rejects three functions that actually
matched, because r0–r3 are ordinary scratch registers and nothing in the text
separates argument setup from a range check using r2 and r3. A filter that
rejects good candidates is worse than one that lets bad ones through.

**`Func_809a44c`'s scheduling residue is not a flag.** This corrects a comment in
the Makefile, which describes the `O1_CFLAGS` rules as matching at -O1
"(equivalently `-O2 -fno-schedule-insns2`)". For this function they are not
equivalent and neither matches:

| | |
|---|---|
| `-O2` | 26 of 27, tail hoisted |
| `-O2 -fno-schedule-insns2` | tail fixed, four earlier pairs now wrong |
| `-O1` | diverges at instruction 4, worst of the three |

`tools/tryc.py` now takes `--no-sched2` so the middle option is checkable
without hand-compiling. Worth seven functions if solved.

## A tooling correction

Over 40 lines `tryc.py` printed only a keyhole around the first divergence. On a
45-line function that showed a single mismatched `bl` and read as a
symbol-naming problem; two register-allocation differences sat nine lines below
the window and only surfaced when `make compare` failed. The screen was right
and the reading was not. It now reports the **total** number of differing
positions in its header.

## Counts

247 functions elevated in total. 3,052 hand-written functions remain in `asm/`
of 5,714. 95 parked.
