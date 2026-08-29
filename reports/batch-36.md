# Batch 36 — three rules of ours that turned out to have exceptions

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–35 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted — 96 overlays
compared byte-for-byte and `goldensun.gba: OK`. Every address below was read
back from the linked ELF with `nm`.

## Read this first: a fourth spelling in the screen

`tools/tryc.py` normalises three spellings that differ between gcc's output and
the ROM's disassembly without any difference in machine code. There is a
**fourth**, and unlike the others it is a typo rather than a convention: three
lines in the inherited disassembly have **no space after the operand comma**.

    asm/rom_c9000/rom_d2d98.s                     ldr r0,=.Lee1f5
    asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c.s    ldr r5,=0xffff0000
    asm/overlays/rom_7a8c8c/ovl_30_c_a…c_c_a.s    ldr r0,=.L3058

Collapsing *runs* of whitespace cannot fix that — there is no whitespace to
collapse. So a byte-exact translation reports **one differing position in the
middle of an otherwise clean diff**, which reads exactly like a wrong symbol,
and the natural next move is to go hunting for a right one that does not exist.
`OvlFunc_922_2008f30` is 53 instructions and this was its only difference.

Three lines in the whole tree, and each one is worth a round. The screen now
normalises comma spacing on both sides.

## Three rules with exceptions

`OvlFunc_964_200970c` breaks two of this tree's own findings in twenty-two
instructions, and both would have been applied mechanically.

**The same callee can get different argument orders in one function.**
`__MapActor_SetAnim` is called twice:

    mov r0, #0x14 / mov r1, #1    / bl __MapActor_SetAnim
    mov r1, #2    / mov r0, #0x14 / bl __MapActor_SetAnim

A declaration is a property of the **file**, not the call site, so no setting of
the declaration lever can produce both — and gcc produces both anyway, from one
declaration, because the order also depends on what is live around the call.
**A differing argument order is not by itself evidence that a lever is needed.**
Write the obvious C and screen it before reaching for one.

**The narrow mask is sometimes written inline.** Every previous member of that
class needed `int m = ~2;` as a named local, because gcc narrows an inline mask
to a byte immediate and loses the ROM's `mov`/`neg` pair. Here the ROM *has* the
byte immediate, so the narrowing is what is wanted and the named local would
break it.

| the ROM has | write |
|---|---|
| `mov rN, #0xfd` | the mask inline |
| `mov rN, #3 / neg rN, rN` | a named `int` local |

The discriminator is in the ROM, not in the source.

**The declaration lever pulls both ways inside one function.**
`OvlFunc_883_2009244` needs `__Func_8010704` **declared**, and the local
four-argument callee on the very next line **not** declared, because the ROM
fills that one's `r0` last. The lever is not "declare the callees"; it is
*declare the ones whose `r0` comes first*.

Its near-twin `OvlFunc_883_2009280` does the same three operations in the
opposite order, and shows that the stack-arg-pair naming is adjacent to **the
call**, not to a position in the body — there the pair is named at the end of
the function rather than the start.

## `match_shapes.py --near`

The exact matcher drains: once every shape with a matched exemplar is elevated,
it reports nothing until more work is done. `--near N` pairs an unsolved
skeleton with a solved one of the **same length** differing in at most N lines,
and prints which lines differ, so a bad lead costs two lines of reading rather
than a round. It produced two of this batch.

**It cannot see the arg-interleave blocker**, and that is worth knowing before
trusting it. The skeleton collapses registers, so

    mov r1 / mov r0 / lsl r1        (the ROM, blocked)
    mov r1 / lsl r1 / mov r0        (what gcc emits)

are the same shape. A near lead can land on a known wall — one did, and cost two
screens rather than a round only because the class was already written up.

## An overlay/main-ROM pair that supports the `__divsi3` alias

`Func_809a65c` is the main-ROM original of `OvlFunc_918_200985c`, which has five
byte-identical copies across five overlays. Batch 29 aliased `__divsi3` to
`_divsi3_RAM` in each overlay's linker script, flagged FOR REVIEW as an
assumption about the original build.

This function **needs no alias**: main-ROM code calls `__divsi3` directly, which
is what gcc emits unaided. The same source resolves to the ordinary helper in
the main ROM and to the RAM-resident one in an overlay — which is exactly what
the alias asserts. Not proof, but it is independent of the reasoning that
produced the alias.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `Func_809a65c` | `0x0809a65c` | main ROM | integrator; no `__divsi3` alias needed |
| `OvlFunc_922_2008f30` | `0x02008f30` | rom_7a8c8c | GetEntrances 8-way, the widest yet |
| `OvlFunc_883_200d928` | `0x0200d928` | rom_780898 | first `--near` find |
| `OvlFunc_948_2009a70` | `0x02009a70` | rom_7d30e0 | second `--near` find; pointer walk |
| `OvlFunc_883_2009244` | `0x02009244` | rom_780898 | lever pulling both ways |
| `OvlFunc_883_2009280` | `0x02009280` | rom_780898 | its undo, in reverse order |
| `OvlFunc_964_200970c` | `0x0200970c` | rom_7ed0a0 | the two exceptions above |

`OvlFunc_922_2008f30` needed no new `.global` — the sibling `.s` that defines
its seven tables already exported all of them.

## Parked

`OvlFunc_908_20081a8` (rom_79c0c4), a fourth member of the arg-interleave class.
Nineteen against nineteen, two positions differing. Three declaration variants
and two optimisation flags all came out **worse** than doing nothing, which
matches the other three members exactly.

## Tooling

`tools/pick_candidates.py` now skips functions named only in a park's **header
comment**. A park that documents a *class* lists its members as bare names, and
the filter matched on "name followed by `(`" — so three of
`pool_load_first.c`'s four members were never skipped, and two came back to the
top of the candidate list the round after they were parked. That is precisely
what the filter exists to prevent, and it had been silently half-working.

## Counts

321 functions elevated in total. 2,974 hand-written functions remain in `asm/`
of 5,714. 95 parked functions, plus 6 files that document blocker classes rather
than individual functions.
