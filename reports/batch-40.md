# Batch 40 — the lever reaches constant-CSE, and the parked set gets sorted

*Status: ready to port.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–39 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare` — 96 overlays compared
byte-for-byte and `goldensun.gba: OK`. Every address read back from the linked
ELF with `nm`.

## The basic-block lever defeats constant-CSE

Batches 37 and 39 described it as fixing argument *order*. It also breaks the
CSE that makes the ROM's redundant form unreachable.

Where the ROM builds the **same value twice** and gcc builds it once and copies,
giving each occurrence **its own named local in a dominating block** makes gcc
rematerialise both:

    rom    mov r0,#0xc0 / mov r1,#0xc0 / mov r2,#0x80 / lsl r0,#10 / lsl r1,#10
    ours   mov r1,#0xc0 / lsl r1,#10 / mov r0,r1 / ...       (two literals)
    ours   ... identical to the ROM ...                      (two locals)

**`OvlFunc_922_2009750` is the proof, and it was a parked counter-example.** Its
note recorded that `-fno-rerun-cse-after-loop` — the flag this tree carries per-file
for that class — was **byte-identical** on it, and guessed that the flag only
reaches values used as call arguments and not values used in address arithmetic.
Two locals holding the same offset match it outright, **with no flag**. The guess
was wrong in a useful way: the discriminator is not what the value is used for,
it is whether the uses sit in a different basic block from the assignment.

**Try separate locals before reaching for `CSE_CFLAGS`.** The flag is a
build-system change carried in `HANDOFF.md` as a standing question for you; this
is one line of C.

The same construct took `src/non_matching/ovl_7f2f14/20087d8.c` from 19
disagreeing instructions to 6, including the `-1` triple it was parked on.

## Name the pointer to move a load's base and offset

`Func_8095b8c` had been parked for two batches on **one instruction**:

    rom    ldr r3, [r3, r1]        index is the base, table is the offset
    ours   ldr r3, [r1, r3]        table is the base, index is the offset

Its note carried three restructurings that all made it *worse* — index as the
pointer base, index named, table cast to `unsigned char *` — because each moved
the whole expression. What works is smaller: `unsigned int *t = L9f0a4;` and
index **that**. Only which operand gcc treats as the base moves.

## `tools/rank_parks.py` — the parked set was never sorted by anything

A hundred files, flat. Picking one to re-attempt meant opening files until one
looked close, and **the file that looks closest is the one with the best-written
note, not the one with the smallest diff.**

Screened with `--align` and sorted, the top entry was one instruction out and
matched the same round. Nothing about it had changed since it was parked.

It also found **five stale parks** — files claiming a function is unconverted
when it has since been matched elsewhere in `src/`. One was for a function
elevated seven batches earlier. Nothing had checked, because matching a function
and deleting its park are two separate acts and only the first is gated on
`make compare`. The ranker now reports them, and eight parks with a dangling
`Source asm:` line have been repointed at the `.s` their function moved to.

Parked-file count is 85 screened / 8 unreferenced, against 79 / 18 before.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `OvlFunc_945_2008670` | `0x02008670` | rom_7cb2c0 | lever + undeclared callee |
| `OvlFunc_943_2008c28` | `0x02008c28` | rom_7c7b9c | `switch`, then `mov`/`neg` |
| `OvlFunc_959_2008ce0` | `0x02008ce0` | rom_7e7574 | lever breaks CSE |
| `OvlFunc_922_2009750` | `0x02009750` | rom_7a8c8c | **unparked**, no flag needed |
| `Func_8095b8c` | `0x08095b8c` | main ROM | **unparked**, named table pointer |
| `OvlFunc_964_20093e0` | `0x020093e0` | rom_7ed0a0 | stack-arg-pair, shared form |
| `OvlFunc_884_20085e8` | `0x020085e8` | rom_784360 | one id a symbol, one a literal |
| `OvlFunc_968_200aee4` | `0x0200aee4` | rom_7f2f14 | stack-arg-pair twice |

`OvlFunc_884_20085e8` is worth a glance: `ldr r0, =_MSG_eb0` in one arm and
`ldr r0, =0xeb1` in the other, so the C is written inconsistently **because the
ROM is**.

## Parked, and negatives worth not repeating

`OvlFunc_903_2008d68` (rom_798dc4), 2 of 22 — and it is filed specifically so the
mask-operand-order rule is not tried on it twice. The `orr` is identical on both
sides; what differs is which register holds the loaded byte and which the
constant. That rule decides the combine's *destination*, which is already right
here. Four spellings tried; gcc allocates the same way in all of them.

Three more re-attempted from the ranking and recorded in their own notes:

* `rom_c5b4`, 2 of 23 — gcc pools two constants as **halfwords** where the ROM
  pools them as words. Looks like inverted `narrow_constant`; that lever does
  not apply, because the narrowing is of the whole computation rather than of a
  store. Naming the result: no change in four spellings. Naming the operands:
  **worse**, 7.
* `2008078`, `2008704`, `2009874` — all arg-interleave, all **straight-line**, so
  the basic-block lever has nowhere to put the value. Confirmed, not assumed.
* `cd488`, 2 of 13 — a volatile register walk gcc merges into `stmia r1!, {r3}`.

## Counts

349 functions elevated in total, of which 7 are fakematches. 2,946 hand-written
functions remain in `asm/` of 5,714. 89 parked functions and the two
large-function experiments.
