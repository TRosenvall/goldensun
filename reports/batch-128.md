# Batch 128 — the interleave lever, sharpened three ways

Verified on a clean `make clean && make compare` — `goldensun.gba: OK` — with
every address read out of the linked ELF.

## Elevated (2241 → 2230)

| function | address |
|---|---|
| `OvlFunc_953_200a820` | 0x0200a820 |
| `OvlFunc_953_200a3e0` | 0x0200a3e0 |
| `OvlFunc_960_2008adc` | 0x02008adc |
| `OvlFunc_891_2009624` | 0x02009624 |
| `OvlFunc_886_20081e8` | 0x020081e8 |
| `OvlFunc_908_200835c` | 0x0200835c |

Two needed `CSE_CFLAGS` and have explicit Makefile rules; two matched on the
first screen.

## The detector was asking for more than the mechanism requires

Batch 127 sized the interleave class by looking for `mov r0, #0` inside a split
build. The single-instruction argument can be **any** constant — `mov r0, #0xc`,
`#0x14`, `#0x15` all appear. Widening the detector took the fully-guarded pool
from **97 functions to 194**, and brought the smallest member down from 85
instructions to **18**.

Both first-screen matches this batch came from the part of the pool the narrow
detector could not see. When a sizing looks small, check whether the detector is
asking for something more specific than the mechanism needs.

## Both split-build arguments must be named, not just the repeated one

`OvlFunc_953_200a820` has six calls of the form `f(slot, X << 2, 0x93 << 2)`
behind a guard. It took four applications of the same lever:

| step | screen |
|---|---|
| all inline | 61 of 86 |
| name the two **pooled** SetSpeed constants separately per call site | 43 |
| name the repeated `0x93 << 2` six times | 12 |
| name the **varying** `X << 2` too | **exact** |

After step 3 the second argument was named and the third inline, and the
interleave did not appear at all. **A call with two split-build arguments needs
both named** — naming one is not a partial win. Twelve named locals in a
forty-line function looks absurd and is correct.

## The commoned-constant tell has two remedies, and they are not interchangeable

An added push holding a constant used more than once is a reliable tell. What
fixes it is not:

- `OvlFunc_886_20081e8`, `OvlFunc_908_200835c` (and batch 127's
  `OvlFunc_883_2008ba8`) — `CSE_CFLAGS` is exact, separate named locals do
  nothing.
- `OvlFunc_953_200a820` — separate named locals are exact, the flag does
  nothing.

Both are one screen. Try both before concluding anything. I offer a guess at the
distinction in `docs/elevation.md` — flag id across a branch versus pooled
argument pair in one block — and label it as a guess, three cases against one.

## Three behaviours for the constant zero

Storing a constant through a narrow pointer, the same value needs different
spellings:

- `*(unsigned short *)p = 0;` **pools** the zero. An int intermediate gives the
  ROM's `mov r3, #0`.
- `*(short *)p = 0x80 << 8;` pools `0xffff8000` by sign extension. An
  **unsigned short** destination gives `mov r3,#0x80 / lsl r3,#8`.
- A bare literal `0` into a **byte** store is pooled — which is what
  `OvlFunc_common1_78` wants.

Two sibling functions in one `.s` needed the first two, which is how the pair
came up. Decided by destination width and spelling together, not either alone.

## Parked

- `OvlFunc_common1_78` — 90 of 90 lines, structure exact. The ROM stores a
  halfword and then genuinely re-reads it with `ldrsh`; gcc sign-extends the
  register it already holds, four times over. **Not** an aliasing problem: the
  intervening store is at a provably distinct offset, so there is nothing for
  alias analysis to get wrong. The original source expressed the re-read in some
  form gcc cannot relate to the store, and I do not know what it is.
- Seven from the preceding round, all small-band register permutations —
  including `GetFlag`, at 3 of 13 with 180 call sites. Its useful half:
  gcc will not generate its branchless is-non-zero idiom from a comparison, and
  writing `neg`/`orr`/`lsr #31` longhand is what produces it.

## A round with no matches, reported as one

The round before this produced **zero** elevations across eight functions, best
screens 3, 3, 10, 11, 12, 13 and 15 differing. Six came from the small-function
pool and five stalled on which register a value lands in.

That was a selection error worth naming: a short function has few enough pseudos
that one allocation choice decides the whole screen, and there is no structure
left to vary. **A 13-instruction function that is 3 lines off is not nearly
done — it is out of moves.** The guarded pool has run at roughly four in five
across this session.

## Still owed

- 12 not-yet-elevated `.s` TUs inside Makefile wildcards.
- ~3,300 lines of duplicate Makefile rule blocks.
- 276 parks, several written under reasoning that batches 123–128 have changed.
