# Batch 206

Seven elevated. Five came from an unparked survey; the last two are the
functions batch 195 parked on a rule this batch measured wrong. That correction
is the batch.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_916_20087e0` | `0x020087e0` | [ovl_30_…_a_a_b.c](src/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_a_b.c) |
| 2 | `OvlFunc_905_200915c` | `0x0200915c` | [ovl_30_c_c_c_c_b.c](src/overlays/rom_799abc/ovl_30_c_c_c_c_b.c) |
| 3 | `OvlFunc_926_200902c` | `0x0200902c` | [ovl_314_…_a_c_b.c](src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_c_b.c) |
| 4 | `OvlFunc_926_2009160` | `0x02009160` | [ovl_314_…_a_c_c.c](src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_c_c.c) |
| 5 | `OvlFunc_960_2008838` | `0x02008838` | [ovl_314_…_c_a_b.c](src/overlays/rom_7eaf28/ovl_314_c_a_c_c_c_c_c_c_c_c_c_c_c_a_b.c) |
| 6 | `OvlFunc_939_2008ff0` | `0x02008ff0` | [ovl_314_c_a_c_a.c](src/overlays/rom_7c460c/ovl_314_c_a_c_a.c) |
| 7 | `OvlFunc_948_2008b68` | `0x02008b68` | [ovl_30_…_c_a_c_a.c](src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_a_c_a.c) |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## A DOCUMENTED WALL WAS NOT A WALL

Batch 195 measured that gcc emits the `mov`s feeding an argument fill in the
order their consuming shifts appear, and that where the ROM wants those two
orders CROSSED, the source cannot express it. That measurement is correct and
nothing here disturbs it. The conclusion drawn from it was not:

> If they are CROSSED, no arrangement of pins, barriers or statement order
> reaches it, and the function should be parked on that basis rather than swept.

`tools/crossed.py` was built from that sentence to reject candidates carrying
the shape. **Three functions in this batch carry it and all three match.**

The lever is one line, after the first `mov` the ROM issues:

    q1 = 0xdc; __asm__ volatile ("" : : "r" (q1)); q2 = 0x9d; q2 <<= 3; ...

It consumes the register, so the `mov` must be materialised before it, and it
produces nothing, so gcc has no value to copy forward in place of rebuilding the
immediate. `2008ff0` went 2 of 157 → exact on that line alone.

**Why the earlier sweep could not find it.** The park on `2008ff0` recorded
seven structurally distinct forms, all scoring 2 — pin presence, pin scope,
assignment order, and how the two mov/shift chains interleave. Every one of them
varied something the *source* controls. The mov order is not decided there; it
is decided in the post-reload scheduler, exactly as that park worked out. An
operand rewrite cannot say "materialise this one first". A scheduling barrier
can, trivially.

**The park asked for precisely this and was not answered.** Its closing
paragraph:

> whether anything makes gcc pick the other mov first when both shifts are
> pending — a construct that reverses the scheduler's preference without
> introducing a dependence, since a dependence emits a register copy instead of
> the immediate

Both properties are named, both are correct, and the construct satisfying them
is three tokens long. The park had done the analysis; the round after it
inherited the verdict rather than the question. **A park's NEXT paragraph is
work already scoped, and it should be read as a task rather than as a summary.**

`docs/elevation.md` and `crossed.py` are both amended. The tool now prints
`BARRIER` instead of `AVOID` and says in its own docstring that the verdict is a
route to the lever, not a reason to skip.

### The tool had also stopped being checkable

`crossed.py` resolves a function name by scanning `asm/`. All three functions it
was validated against are now elevated and their listings are gone, so a bare
name now reports `NOT FOUND` for every one of its own test cases — silently, and
with exit 0. Its docstring already warned that *"a filter that passes the cases
it exists to catch is worse than no filter; check any change here against those
two names"*, and following that instruction had become impossible.

It now accepts a `.s` path, and the three `git show` commands that recover the
listings are in the file beside the code that needs them. All three still report
`BARRIER`. **Elevating a function deletes the fixture any tool validated against
it was using**, which is worth a check whenever a tool names specific functions
as its test set.

## A NAMED POINTER COSTS THE RETURN REGISTER — AND SO DOES REUSING ONE

Two functions hit this from opposite directions in the same batch.

On `200915c`, the obvious spelling

    p = __MapActor_GetActor(0xa);
    p[0x23] &= 0xfd;

gives `mov r1, r0 / add r1, #0x23` where the ROM does `add r0, #0x23`, and that
single copy drags the block out of step — 45 of 72. Subscripting the call result
directly, `__MapActor_GetActor(0xa)[0x23] &= 0xfd;`, takes it to 2. The address
becomes a temp that dies inside the statement, so nothing wants it off r0.

On `2008b68`, naming was *not* the problem. One `unsigned char *a` served two
stores far apart, the second of which must stay live across three calls. One
pseudo, so the second range forces callee-saved and the **first range inherits
it**: `mov r5, r0` and a store through r5 where the ROM stores through r0.
Splitting the two ranges into separate locals costs nothing and took 84 of 139
to 2, length exact.

**Read the live ranges, not the spelling.** The question is never "is this
pointer named" but "how long does this particular value have to live".

### A false lead worth recording

The first hypothesis on `200915c` was that the function-scope
`register int p0 __asm__("r0")` argument pin was making r0 unavailable across
the body. Re-screening with every pin group moved into its own block scope gives
the **identical diff, byte for byte**. A pin reserves the register at the sites
that use it, not across the function, and that hypothesis should not be reached
for again.

## THE DESTINATION FOLLOWS THE TIE, SO BOTH OPERANDS HAVE TO BE NAMED

The last two instructions of `200915c` were an `orr` on a byte field with the
register roles swapped against the ROM. The existing entry lists four cures —
`|= K`, `K | x`, `x | K`, and an `int` temporary. **All four score 2**, which is
the tie that entry already predicts.

What closes it is naming the destination of the accumulation as its own
statement:

    t = q[0x23]; k = 2; k |= t; q[0x23] = k;

`k |= t` spells which register the result lands in; `q[0x23] = 2 | t` leaves
that to the allocator and it picks the other one. This was first reached with
both operands pinned to r2 and r3 — and **the pins then came out with no
change**, so they were never part of the match. Pinning *one* operand does not
do it either: value-only and constant-only both score 2 and both leave the
accumulation running the wrong way.

The `and` site four statements earlier, the same shape with a different
operator, needs none of this and gives the ROM's form first time. That asymmetry
is unexplained and is recorded as unexplained.

## ONE BARRIER MOVES THE PROBLEM; TWO FIX IT

With the message base pinned to r5 on `200902c`, gcc scheduled `ldr r5, =0x183a`
two statements above where the ROM issues it. A `do { } while (0)` before the
intervening `__CutsceneWait` moved it exactly one hole later. A barrier placed
*after* that call instead fixed the load but dropped `mov r6, r0` out of the
prologue. **Bracketing the call with both** is exact.

A pinned hard register has no data dependence holding its assignment down, so
the only thing bounding it is the region walls — and one wall only decides which
way it falls.

That pin is itself worth the entry. Written as a plain `int m = 0x183a`,
constant propagation folds `m - 1` into its own pool entry before liveness is
considered, `m` dies at its first use, and the parameter takes r5: 104 of 113,
and **one pushed callee-saved register where the ROM pushes two**. The prologue
width said a second value had to be named before a single instruction was
compared. `register int m __asm__("r5")` takes it to 25 in one step and the
parameter moves to r6 unaided.

## A DIFFERING COUNT IS ONLY MEANINGFUL FOR THE FIRST DIVERGENCE

Applying the two volatile-asm barriers to `2008b68` **first**, while the pointer
residue at instruction 52 was still open, scored **87 against the park's 84**.
The lever that finishes the function read as a regression.

Everything after the first difference is shifted, so a correct fix downstream
lands against misaligned ROM text and counts as noise. With the earlier residue
closed, tearing those same two barriers back out gives 5 differing at precisely
the two sites they serve — they were load-bearing the whole time.

**Fix the earliest divergence, then judge anything applied after it.** A total
count is a summary statistic over a comparison that is only aligned up to the
first mismatch.

## SMALLER, ALL MEASURED

**A shifted constant has to be shifted in the source.** `0x80 << 8` written
inline in a halfword store became `ldr r3, =0x8000` against the ROM's
`mov r3, #0x80 / lsl r3, #8`; splitting it into `v = 0x80; v <<= 8;` gives the
ROM's build. Its two neighbouring stores of `0xc0 << 12` and `0x80 << 24` need
none of this. Writing it as an r3 *pin* instead is wrong and cost an attempt —
the address load lands in r3 too and overwrites the pinned value, which `tryc`
shows as `strh r3, [r3, #0x1e]`.

**The type still decides what lands in the pool.** The same store as `short`
rather than `unsigned short` pools the value sign-extended to `0xffff8000`.

**Store address before store value.** Assigning the destination pointer ahead of
the value gives the ROM's r2/r3 split; leaving it a nested dereference reverses
them. Two spellings of that are byte-identical, so only the order is real.

**One exception in an otherwise identical run, again.** `2009160` calls
`__Func_809280c` five times in a row; the ROM fills r0/r1/r2 ascending for four
and descending for the fifth. Plain literals get four right and the fifth wrong.
This is the second batch running where a transcribed call list beat a loop over
a table for exactly this reason.

**Same callee, same arguments, two orders.** `__MapActor_SetSpeed(0xf, 0xcccc,
0x6666)` appears in both `200902c` and `2009160` — same overlay, same values —
and the ROM fills the registers in different orders at the two sites. Read every
call site off the listing; do not reuse the last one that worked.

## SELECTION

Batch 205 ended by widening the survey to unparked functions and got three
first-screen matches. That held: `tools/templated.py` ranked on template quality
and filtered on `hi == 0` (no r8–r11 traffic) produced five candidates and all
five landed, at 76, 72, 111, 141 and 150 instructions.

`crossed.py` — the pre-filter that exists to reject candidates before work goes
into them — **was not run during selection**, and running it would have skipped
`2008838`, which matches. That is an argument for running the screen and reading
the residue rather than trusting a pre-filter, and it is the reason the rule
behind the filter got measured at all.
