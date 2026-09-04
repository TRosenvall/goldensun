# Batch 205

Five elevated. The batch spent two rounds finding nothing and then changed how
candidates are chosen, which produced three first-screen matches in a row. Both
halves of that are the report.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_948_20091d8` | `0x020091d8` | [ovl_30_…_c_a.c](src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_c_a.c) |
| 2 | `OvlFunc_883_2008fec` | `0x02008fec` | [ovl_30_…_c_a_a.c](src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a_a.c) |
| 3 | `OvlFunc_890_2009be8` | `0x02009be8` | [ovl_30_c_c_a_c_b_b.c](src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_b.c) |
| 4 | `OvlFunc_881_200955c` | `0x0200955c` | [ovl_30_…_a_a_b.c](src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_b.c) |
| 5 | `OvlFunc_881_2009680` | `0x02009680` | [ovl_30_…_a_c_b.c](src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_b.c) |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## THE PARKED BAND IS EXHAUSTED; THE UNPARKED ONE IS NOT

Two rounds produced no elevations. Four parks were worked and every one reduced
to the same thing — **which register the allocator parks a value in**:

    2008ef4    the ROM puts parameter 2 in r6 and parameter 1 in r5; gcc reverses
    200811c    the zero for an ldrsh offset lives in r4; gcc uses r1
    2009424    two argument movs want r1 first; gcc emits r0 first
    200842c    the ROM shifts a field into a different register than it read it into

The pin is inert against all four. It decides which register a value is
**written into** and where that write sits among other pinned writes; it does not
override the allocator's choice for a value that is not being written where the
pin names.

**That also bounds a claim from batch 197.** That batch ordered three argument
registers by pinning and concluded "a pin orders independent movs". On
`2009424` pinning is inert at the two differing sites *and* with all eight calls
of both long arms pinned. The claim is too strong as stated; the 197 case should
be read as three-register argument setup, not as a general rule.

### The change

Everything at 60 instructions or fewer in these shape groups is parked, and the
parks are where the hard residues have accumulated. So the survey was widened to
**120 instructions** and filtered for functions with **no park at all**.

Three came out of that filter and all three matched on the first screen —
`2009be8` at 72 instructions, and the `200955c`/`2009680` twins at 103 each.
None needed a new lever.

**Size was never the blocker; shape is.** A 103-instruction function with no
accumulated park is easier than a 20-instruction park, because the park exists
precisely to record that the easy approaches were already spent on it.

## A NEAR-MISS THAT WOULD HAVE COMMITTED A RED BUILD

`Func_801edec`'s park stated that one of its three differing lines — the ROM's
`ldr r5, =0x214` against our `ldr r5, =_FUNC_80158E8_SIZE` — was "not a real
difference at all", since the symbol resolves to `0x214` at link time, and said
**"Do not spend attempts on it."**

The other two closed: splitting the DMA helper's `_src` declaration from its
assignment and pinning the fill value to r3 restored the ROM's staging order,
and declaring that pinned temporary `unsigned int` rather than `u16` stopped
`0xe0e0` reaching the pool sign-extended as `0xffffe0e0`. (**The pin decides the
register; the type decides what lands in the pool.**)

With those applied `tryc` reported exactly the one line the park called
cosmetic. It was landed on that basis and **`make compare` failed at
compare-rom.** Reverted, tree restored green, park corrected.

The general point, now recorded there: a park can *assert* that a difference is
cosmetic, and `tryc` cannot check such an assertion because it compares
disassembly text. Only the build can, and it is cheap to ask.

## A MATCHING FORM, DELIBERATELY NOT TAKEN

`OvlFunc_881_200811c` reached **1 of 16**. A form that matches at 16 of 16
exists: writing its second access as `*(unsigned short *)(p + off - off)`.

It was rejected. That expression exists only to hold a register live while still
yielding the immediate addressing form, and it is what
`src/non_matching/ovl_780898/2008fec.c` called "inventing code to fit output"
when it rejected the same kind of move.

The distinction worth keeping: **a register pin is a declared fakematch**, listed
in `fakematch.txt` and legible as scaffolding. A fabricated expression is a
fakematch disguised as ordinary C, and the next reader has no way to tell.

## Housekeeping

**16 fully-stale parks removed**, of 524, at the batch boundary — nine from
batches 200–204 that were my own leavings, including a duplicate pair parking one
function twice. Batch 204 concluded this sweep belongs at a boundary rather than
when something looks wrong; run first, it keeps every grep-built candidate list
clean for the rest of the batch.

## Smaller

**The ROM builds a repeated value both ways.** `2009be8`'s final `SetPos` passes
the same value for x and y and the ROM builds it once and copies (`mov r2, r1`),
so `q2 = q1` reproduces it. `20091d8` has the identical situation and the ROM
builds it **twice**, which needed pins to force apart. Only the listing says
which.

**Not every constant in a repeated sequence is pooled.** In the `881` twins, four
of twelve calls build their third argument with `mov`/`lsl` because the value is
a shifted byte, and the other eight cost a pool entry. Plain literals get both
right unaided. And the twelfth call loads `r0` early where the other eleven load
it last — one exception in an otherwise identical sequence, and the reason the
call list was transcribed from the listing rather than written as a loop.

**Two process slips**, both mine and both caught before they mattered: a `.c`
written to a path derived from a *guessed* `.s` name, caught by looking the `.s`
up by function name; and a `*/` inside a header comment that closed the block
early.
