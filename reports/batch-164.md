# Batch 164

Six elevated. Verified after a clean `make clean && make compare`; SHA1
`5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked images.

| function | address | image |
| --- | --- | --- |
| Func_80935d4 | 0x080935d4 | goldensun.elf |
| Func_809b804 | 0x0809b804 | goldensun.elf |
| Func_80970f8 | 0x080970f8 | goldensun.elf |
| Func_80a10d0 | 0x080a10d0 | goldensun.elf |
| OvlFunc_899_200c7bc | 0x0200c7bc | overlays/rom_794ac0/overlay.elf |
| OvlFunc_899_200c7fc | 0x0200c7fc | overlays/rom_794ac0/overlay.elf |

## THE `multi` POPULATION WAS NEVER WORKED, AND IT IS ORDINARY WORK

Five of the six came from it. `tools/census.py` classifies 692 remaining
functions as `multi` -- they share a `.s`, so their blocker is UNKNOWN until a
split, and they are excluded from the `open` worklist for that reason. That is
not the same as being blocked, and after four consecutive thin rounds on the
worked-over pools it was where to look.

Selecting from it with the criteria that have been paying -- loop-free, no
r8-r11, no repeated expensive constant, 20 to 90 instructions -- leaves 33
functions. Six were tried and five matched; three of those were at exact length
on the first screen and one matched outright with no edits at all.

**The `multi` label costs a `split_s.py` run and nothing else, and the split is
byte-neutral by construction.** Treat that population as ordinary candidates.
The census presentation reads as though a split were an obstacle, and it is not.

## The merge lever is now the most reliable thing for register rotation

"One register for two unrelated values means ONE variable" -- the inverse of the
disjoint-live-range rule -- closed two more functions here and is three-for-three
overall:

    OvlFunc_954_2008490     38 differing to exact
    OvlFunc_896_200c260     78 differing to 6   (a park written off the round
                                                 before as unreachable)
    OvlFunc_899_200c7bc/fc  17 differing to exact

On the twins the ROM keeps one register for a coordinate and then for the
difference computed from it, so `ax = (ax - x) >> 16;` is right where a separate
`dx` is not. It reads badly and it is what the original did.

**Before recording a rotation park as unreachable, check whether the ROM REUSES
a register for two values the obvious C keeps apart.** That turns an allocator
question into a variable-count question.

## Three smaller readings, each measured

**A ROM `bhi`/`bls` on an `ldrb` value is a tell about the LOCAL'S TYPE.** A byte
load promotes to `int` and gcc picks a SIGNED compare unless the destination says
otherwise. On `GiveDjinni` the branch was `bgt` until the count was declared
`unsigned int`. Check the local before suspecting the comparison operand.

**Name the two multiplies before summing them.** Written inline, gcc fuses each
multiply with the subtraction feeding it; the ROM computes both differences and
then both squares. Naming the products took the twins from 36 lines to 34.

**`pop {r1} / bx r1` in a void-looking function wants `int` with NO return
statement.** Measured three ways on `Func_80935d4`: declared void is 4 differing
with the epilogue wrong; declared `int` with explicit returns at the early exits
is 74 and two lines long; declared `int` with no `return` anywhere is 2 and
exact. Explicit returns make gcc materialise a value the ROM never produces.

## The aliasing tell, and where it stops

`-fno-strict-aliasing` closed `Func_80935d4` (54 differing to 4) on the shape
recorded last batch: the ROM re-reads a field across a store of a different
width, and losing that one reload shifts everything downstream.

`tools/aliastell.py` makes the shape searchable, and two rounds of false
positives narrowed it twice. **A call between the store and the re-read means
nothing** -- gcc reloads across a call regardless. **A store through a CHARACTER
type does not qualify either** -- a character type aliases everything. With both
excluded the detector reports 5 candidates rather than 48.

Its boundary is also measured: on `OvlFunc_926_2008e94` the flag helps (82 to 65)
but the LENGTH IS WRONG IN BOTH DIRECTIONS -- 99 lines against 101 without it,
102 with. Some of the ROM's re-reads survive and some do not, so the source
distinguishes them by type and a whole-TU flag cannot. **When the flag moves the
line count the wrong way, it is the wrong class.**

## Parks worth returning to

`Func_808d828` at 4 of 94 and `GiveDjinni` at one instruction are both a single
unreachable detail from matching. `OvlFunc_947_2009938` at 36 of 91 has its
entire guard chain exact and is a twin, so it covers two.

`Func_80ae99c` records a boundary on the block-layout reading: gcc IF-CONVERTS a
two-arm constant selection that the ROM keeps as a real branch, and neither
ordering the arms to the ROM's fall-through nor spelling them with explicit
`goto`s changes it. That reading tells you which arm becomes the fall-through
WHEN GCC EMITS A BRANCH; it says nothing when gcc declines to emit one.
