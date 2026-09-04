# Batch 211

Five elevated, four exact on the first screen. The batch produced one tool fix,
one red build I caused and corrected, and a confirmation that the hardest shape
this tree has met is now routine.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_931_20087b8` | `0x020087b8` | [ovl_30_…_a_c_c_b.c](src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_c_b.c) |
| 2 | `OvlFunc_888_2008360` | `0x02008360` | [ovl_30_c_c_a_a_a_a_c_a.c](src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_a_c_a.c) |
| 3 | `OvlFunc_930_2008924` | `0x02008924` | [ovl_30_c_c_a_c_c_c_a_c_c.c](src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_c_c.c) |
| 4 | `OvlFunc_909_200979c` | `0x0200979c` | [ovl_30_…_c_a_b.c](src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_a_b.c) |
| 5 | `OvlFunc_954_20096ec` | `0x020096ec` | [ovl_30_c_c_c_c_c_b.c](src/overlays/rom_7db0c8/ovl_30_c_c_c_c_c_b.c) |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## crossed.py WAS BLIND TO THE NEGATION FORM

The filter scanned for `lsl` only. But what orders the movs is **whichever
instruction consumes each one first**, and for a negative argument that is `neg`.

    OvlFunc_931_20087b8   reported CLEAN   actually 1 crossed site
    OvlFunc_909_200979c   reported 1       actually 2

Both were caught by reading the listing, so the gap cost nothing this round — but
it would have cost an attempt on any function where the neg-crossed site was the
only one. The scan now covers `lsl/lsr/asr/neg`.

Validation is the part worth stating: the three carriers the tool was built from
still flag, and `2008b68` now reports **two** sites where it reported one. The
second is the `__Func_8012330(-1, -1, 0xe666)` call — which that function's own
park had named in prose as a crossed negation site. **The tool now agrees with
what was already written down about it**, which is the strongest evidence the
widening is right rather than merely more sensitive.

## A "ONE FUNCTION" FILE CAN STILL HOLD DATA — I TOOK A RED BUILD ON IT

`20096ec`'s `.s` was vetted as alone in its file by counting `.thumb_func_start`,
and converted whole. It also carried a tail of `.incbin` blocks in an explicit
`.section .data`, exporting four `gOvl_…` symbols another TU links against. The
build went red on undefined references.

`tools/split_s.py` already knows the difference. Given a single function **with**
a data tail it splits code from data; it only says *"convert it directly, no
split needed"* when there is genuinely none. I bypassed it because my own count
had already answered the question it answers better.

**Run split_s.py on every target, including the ones that look alone. Its
no-split verdict is the check; a hand count of function starts is not.**

Fixed by giving the data its own `.s` and pointing the linker script's `.data`
entry at it, then realigning the names to the tool's own `_b` (code) / `_c`
(data) convention so the next reader finds what they expect.

## THE CROSSED SHAPE IS NOW ROUTINE

`2008924` carries **three** crossed sites. All three barriers went into the first
draft straight from the listing, and it was exact on the first compile.

That shape cost two functions two full rounds when batch 195 first met it, and
batch 206 had to correct a documented rule to reach it at all. Across this batch
there were **eight** crossed sites over four functions and not one of them cost an
attempt. The pre-filter locates them, the ROM says where the barrier goes, and
the first compile clears them.

## A DEFERRED SHIFT NEEDS n−1 BARRIERS TOO

`20096ec` builds a callee-saved constant across another call's argument fill,
with its shift **after** both argument shifts:

    mov r5, #0xfa / mov r1, #0xc0 / mov r2, #0xc0 / mov r0, #0 /
    lsl r1, #9 / lsl r2, #8 / lsl r5, #2 / bl __MapActor_SetSpeed

Written in exactly that source order it comes out **first** — a pinned
callee-saved register has no dependence holding its shift down. One barrier moves
it to the right place but pulls its `mov` ahead of the two argument movs.
Barriers on both are exact.

So batch 207's per-mov rule is not about argument setup: **what is being ordered
is materialisation**, and a long-lived local being built alongside a fill counts
the same as an argument.

## THREE LEVERS STACKED ON ONE STORE PAIR

`20087b8` is only 53 instructions and its opening pair of halfword stores wanted
all of: the zero **named**, both operands **pinned**, and the address **advanced
in place**. Written plainly it scores 31 of 55 against a 53-line ROM.

The reason all three are needed together is that two of them are the same defect
seen twice: gcc puts the zero in the pool because it never gave it a register,
and it never gave it one because the address expression claimed the low register
first.

That function also runs against a habit: **the actor pointer is a named local
here**, where elsewhere subscripting the call result directly is what keeps it in
r0. The ROM moves it out with `mov r1, r0` because r0 is needed for the next
call's slot number. Read the ROM's register, not the habit.

## SMALLER

**An unsigned range test has to be spelled unsigned.** `2008360` compares a value
loaded with `ldrh` against two bounds using `bcc`/`bhi`. An `unsigned short`
promotes to `int`, so the natural comparison emits *signed* branches; casting the
bound to `(unsigned)` gives the ROM's pair.

**Single exit, written in from the start.** `2008360`'s ROM reaches its teardown
from four places. Batch 209 measured what duplicating that call costs (86
instructions); here the nested form went in first and the function matched first
time.
