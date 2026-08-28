# Batch 129 — density is the selector, and a detector bug that cost a round

Verified on a clean `make clean && make compare` — `goldensun.gba: OK` — with
every address read out of the linked ELF.

## Elevated (2230 → 2224)

| function | address | notes |
|---|---|---|
| `OvlFunc_898_2009238` | 0x02009238 | `CSE_CFLAGS` |
| `OvlFunc_948_2008ad0` | 0x02008ad0 | `CSE_CFLAGS` |
| `OvlFunc_934_2009258` | 0x02009258 | `CSE_CFLAGS` |
| `OvlFunc_934_20091a0` | 0x020091a0 | `CSE_CFLAGS` |
| `OvlFunc_882_2009a64` | 0x02009a64 | first screen |
| `OvlFunc_882_200ad28` | 0x0200ad28 | first screen |

## `GetFlag(id)` guarding a block that ends `SetFlag(id)` means `CSE_CFLAGS`

Five of seven functions elevated from this pool needed
`-fno-rerun-cse-after-loop`, and they share one shape:

    if (__GetFlag(id) == 0) { ... __SetFlag(id); }

The id is materialised twice and the rerun-CSE pass commons the two into a
callee-saved register, adding a push the ROM does not have. Separate named
locals defeat it in none of them. The shape is visible in the ROM listing before
writing a line of C, so these are now screened with `--no-rerun-cse` from the
start.

That also settles the guess offered in batch 128 as untested. It now stands at
six cases to one: **commoning across a branch wants the flag; commoning within a
block wants separate locals.** The lone counter-example is a pooled argument
pair reused at two calls in the same block, which fits.

## Density, not size, is the selector

Two rounds in the middle of this batch produced **zero** matches between them,
eight functions and then three, all stalling on which register a value lands in.
The cause was selecting by size: I had been taking the smallest candidates.

A short function has few enough pseudos that one allocation choice decides the
whole screen. A call-dense script is the opposite — it is mostly argument setup
and `bl`, so the levers act on nearly every instruction and there are few locals
for allocation to differ over. Filtering the guarded pool by

    calls * 4 >= instructions    and    memory-ops * 4 <= instructions

gives 11 functions in the 30–90 band, and **three of the four tried matched**.
A 72-instruction script with 23 calls is easier than an 18-instruction function
with three.

## A detector bug, and the lesson that is not "check for tabs"

Classifying the remaining functions by the flag shape reported **0 of 2227**,
twice, through two rounds of fixing the wrong thing:

1. The operand pattern only accepted `ldr r0, =<id>`, missing ids built as
   `mov r0,#K / lsl r0,#n`. Widened it. Still zero.
2. The actual bug: the call was matched with
   `x.strip() in ("bl __GetFlag", …)` and the assembly separates mnemonic from
   operand with a **tab**, so `strip()` yields `bl\t__GetFlag`.

With that fixed: **191 functions**. The zero was never plausible — five had been
elevated from that exact shape in the preceding rounds.

The lesson is that when a detector returns zero and the first fix does not move
it, the second hypothesis should be about the **harness**, not another
refinement of the pattern. Both wrong versions were about the ROM's spelling;
neither was about mine. This is the fifth over-narrow detector this session and
the first where the flaw was on my side of the comparison.

## Two new levers

**Consume the pointer, do not index it.** `a[0x62] = 0;` makes gcc copy the
pointer before adding (`mov r3, r0 / add r3, #0x62`) where the ROM consumes it.
Writing `a += 0x62; *a = 0;` took one function from **28 differing to 7**. Only
arises above offset 31, where thumb has no immediate form. Pairs with the
earlier "two call results need two variables" rule as opposite halves of one
problem — one keeps a pointer alive that should die, the other kills one that
should live.

**Name the stored constant.** `z = 0; *p = z;` instead of `*p = 0;` at three
stores took another function **15 → 6**; a named mask took a third **7 → 4**. It
shifts which register the pointer gets, but only moves the allocation, it does
not choose it. Neither function reached exact.

## The interleave lever, bounded from both sides

- `OvlFunc_899_20099a4` — a split build exists but no dominating block precedes
  it. The lever needs a branch to make rematerialising cheaper than keeping the
  value live.
- `OvlFunc_921_20082b8` (2 of 74) — a dominating block exists but **neither
  argument is a split build**. The lever moves arguments around a
  two-instruction sequence; where every argument is one instruction there is
  nothing to move them around.

The lever needs a split build AND a preceding branch. Missing either, park it.

## Parked

Six this batch. Beyond the two above: `OvlFunc_881_20097a4` is the first
commoned-constant case where neither remedy works — the repeated value is a
comparison operand and a stored value, not a flag id consumed by two calls, so
the commoning survives the pass the flag disables.

## Still owed

- 12 not-yet-elevated `.s` TUs inside Makefile wildcards.
- ~3,300 lines of duplicate Makefile rule blocks.
- 281 parks. Several were written under reasoning batches 123–129 have changed,
  and nothing systematically re-checks them.
