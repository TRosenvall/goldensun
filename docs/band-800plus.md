# What is different about working at 800+ instructions

Batch 307, brief G.  First work in the band.  Everything below is measured on
this batch's four targets (OvlFunc_881_2008c28 834, OvlFunc_945_2009f3c 807,
OvlFunc_959_200a7b0 810, Anim_Ramses 818), two of them reconstructed and
measured end to end.  Where a claim rests on one function only, it says so.

## 1. THE BAND IS TWO POPULATIONS, AND MIXING THEM IN ONE BRIEF IS A MISTAKE

Instruction count is not the useful axis up here.  BRANCH DENSITY is.

| function | insns | branches | labels | high-reg mentions in the REFERENCE |
|---|---|---|---|---|
| OvlFunc_881_2008c28 | 834 | 5 | 5 | 0 |
| OvlFunc_945_2009f3c | 807 | 1 | 1 | 45 |
| OvlFunc_959_200a7b0 | 810 | 5 | 6 | 0 |
| Anim_Ramses | 818 | 54 | 50 | 65 |

Three of the four are **straight-line script** -- 800+ instructions in one to
six basic blocks, i.e. blocks of 150-800 instructions.  The fourth averages
~15 instructions per block, which is ordinary.  These two populations have
different dominant blockers and want different levers, and a brief should not
put them side by side.  The band's 66 available functions should be triaged by
`grep -c` on branch mnemonics before any are assigned.

## 2. FOR THE STRAIGHT-LINE POPULATION, ONE MECHANISM DOMINATES EVERYTHING

**cse.c pass 1 commons repeated multi-instruction constants across call
boundaries, and the residue is which constants it picks.**

Traced in the `-da` dumps on 2008c28.  `00.rtl` holds four independent
`(set (reg:SI N) (const_int 26214))`, one per `__MapActor_SetSpeed` site.
`03.cse` leaves ONE real set -- insn 137, `(set (reg:SI 56) (const_int 26214))`
`{*thumb_movsi_insn}` -- and rewrites the other three sites as
`(set (reg:SI 2 r2) (reg:SI 56))` carrying `(expr_list:REG_EQUAL (const_int
26214))`.  Reg 56 then lives across four calls.

Why this is a BAND effect and not a general one: **a call does not invalidate
cse's constant table**, and cse1's window is the basic block.  At 200
instructions a block is small and a constant repeats twice; at 834
instructions in six blocks, a constant repeats seven times inside one block.
2008c28's reference reloads `=0x6666` 7 times, `=0x19999` 6, `=0x2008` 7,
`=0x16d80000` 5, `=0xcccc` 5.  Every one of those is a commoning opportunity
that simply does not exist lower down.

The consequence is register pressure, and with `-fcall-used-r4` only r5/r6/r7
are call-saved low registers.  Overflow goes to r8-r11, which Thumb-1 cannot
use as operands, so each one costs a `mov rlo, rhigh` **and** the six-
instruction Thumb high-save prologue and its matching epilogue.

**No flag reaches it.**  cse pass 1 is unconditional at -O1, -O2 and -Os.
Measured on 2008c28, high-register mentions in the generated `.s`:

    -O2 baseline 47 | -fno-rerun-cse-after-loop 43 | -fno-gcse 47
    both 47 | -fno-cse-follow-jumps 47 | -fno-cse-skip-blocks 43
    -fno-expensive-optimizations 47 | -fno-force-mem 47 | -fno-caller-saves 47
    -fno-omit-frame-pointer 45 | -O1 43 | -Os 47

The two `-fcse-*` switches tune cse's reach ACROSS blocks; the commoning here
is INTRA-block, which is why both are inert.  **Do not write a CSE_CFLAGS row
for a straight-line function in this band on the strength of the shape alone**
-- lever 5's precondition (a save-flag id with one use dominating another) is
absent, and on 2008c28 `-fno-rerun-cse-after-loop` measured WORSE (74.9% ->
74.5% aligned).

## 3. THE ESTABLISHED LEVERS: WHICH SURVIVE, WITH FIGURES

**Lever 1, REUSE TO INHERIT A REGISTER -- measured WORSE.**  2009f3c's
reference carries 0x400b and then 0x8008 in two disjoint ranges of r5, the
batch-304 signature exactly.  Merging them into one declared variable is
+8 size / +3 count against +4 / +1 for not merging, and loses 2.7 aligned
points.  **The precondition that fails is the FIRST one -- the count is not
already exact.**  Up here you are still arguing about HOW MANY registers there
should be, and merging removes one quantity from the very set the size and
count are measuring.  Reuse is an endgame lever; it is not a band-entry lever.

**Lever 2, ONE VARIABLE PER REGION -- inert, contrary to the brief's
expectation.**  The brief predicted this would matter MORE at 800+ with many
regions.  On 2009f3c it does not matter at all: naming the reference's seven
parked constants and placing each assignment at the reference's own parking
point compiles to a BYTE-IDENTICAL object to leaving them all as literals --
same size, same count, same 672 aligned, same 144 hunks.  The reason is
mechanical and worth carrying forward: **region-scoping has no purchase on a
value cse1 already commons.**  Lever 2 works when a variable YOU declared has
an inflated `live_length`; it cannot reach a pseudo the compiler invented.

**What DID move 2009f3c was changing the SET, not the scoping.**  Naming eight
constants and assigning them ALL AT THE TOP went +20/+10 -> +4/+1 on size and
count, because it creates the reference's NUMBER of long-lived quantities.
That is the band-entry lever for the straight-line population: **count the
reference's parked constants, then make your candidate hold that many.**

**Declaration order -- inert FOR LOCAL-ALLOC-PLACED CONSTANTS ONLY; it DOES set
the order of RELOAD SPILL SLOTS.**  Permuting the eight declarations and
assignments into the reference's own r5..r11 order is byte-identical.
Consistent with batch 305: values placed by LOCAL-alloc were never ranked by
`allocno_compare`.

NARROWED IN BATCH 311 (`Anim_Gaia`).  The measurement above was taken on a
function whose quantities LOCAL-alloc placed in registers -- it says nothing
about a function that SPILLS, and the unqualified heading it used to carry was
read as covering both.  On `Anim_Gaia` a declaration reorder moved the aligned
figure by two encodings -- noise, and on the figures alone it would have been
written off as inert a second time -- while *correcting a seven-encoding spill
slot misassignment*, after which the slot map matched the reference entry for
entry.  Two consequences:

  * **For a spilling function the ranking instrument is the spill-slot
    access-count table, not the aligned figure.**  The aligned figure averages
    a slot correction away; the slot map shows it directly.  This is the same
    lesson as rung 2 of the figures-that-lie ladder, reached from the other
    side: here the figure did not lie about a WORSE candidate, it stayed silent
    about a BETTER one.
  * The spill-slot lever has **no purchase on a function with no spills**, which
    is why putting it in all eight briefs of batch 310 on two functions'
    evidence was wrong.  Read the frame first (the three greps); only then is
    the slot map a lever at all.

**Lever 3, THE OFFSET AS A NAMED LOCAL -- still pays, unchanged.**  On 2008c28
`h = 0xf0 << 8; *(short *)(q + 0x1e) = h;` keeps 0xf000 out of the pool, and
`v = 0x1999;` with two stores gets the reference's single pool load.  One
region of 2008c28 (19 instructions) comes out exactly right on the strength of
those two namings.

**The batch-306 POOLED-ZERO defect is worth checking first, and it is the
single best-value edit found.**  `*(short *)p = 0` puts the ZERO in the
literal pool (`ldrh r3, .L12`) because `*thumb_movhi_insn` has no immediate
form; the reference has `mov r3, #0`.  An int carrier -- `z = 0; *p = z;` --
was worth **16 bytes and 5 encodings** on 2008c28 in one line.

## 4. IS objcmp's COUNT USABLE? NO. SATURATION IS THE NORMAL CASE -- BUT
##    SIZE-AND-COUNT IS STILL THE RANKING AXIS, NOT THE ALIGNED FIGURE

Neither reconstructed function reached a true distance, so every objcmp figure
in this batch is saturated.  Lead with aligncmp, as the brief said.

But the sharper finding is that **the two instruments disagree, and the
discipline's own priority order is what resolves it.**  On 2009f3c:

| candidate | size | count | aligned | hunks |
|---|---|---|---|---|
| all literals | +20 | +10 | **80.0%** | 144 |
| region-scoped naming | +20 | +10 | 80.0% | 144 |
| eight named at top | **+4** | **+1** | 77.7% | 166 |
| same, reference's declaration order | +4 | +1 | 77.7% | 166 |
| same, 0x400b/0x8008 merged | +8 | +3 | 75.0% | 169 |

The candidate that is 16 bytes and 9 encodings closer is 2.3 aligned points
WORSE, and has 22 more hunks.  Ranking on the aligned figure alone would have
picked the structurally worse candidate.  **Rank on size-and-count first even
while both are inexact; use aligned only to break ties between candidates with
the same size and count.**  A candidate at +4/+1 is one pool word from a true
distance; a candidate at +20/+10 is not, however well its bytes happen to line
up.

## 5. THE CHEAPEST AND MOST VALUABLE EARLY SIGNAL IS THE CONSTANT SET

Confirmed on both functions and it is better than the relocation sequence.
Diff the DISTINCT pooled values -- the generated `.s`'s `.word` list against
the reference's `ldr rX, =` set.  On 2008c28 the two sets are IDENTICAL: all
30 numeric literals and all 15 symbols, nothing in one and not the other.
That single comparison proves every constant, actor slot, script pointer and
call in an 834-instruction reconstruction is correct, and it costs one
`grep | sort | uniq -c`.  It also surfaced the pooled zero as the one entry
present on our side and absent on the reference's.

At this size that check should come FIRST, before any hunk is read.  The
relocation sequence is still worth reading and also matched entry for entry on
both functions, but it cannot see numeric constants, which is where the whole
residue lives for this population.

## 6. IS `-da` DUMP READING STILL TRACTABLE? YES BY grep, NO BY READING

2008c28's dumps run 145 KB (`00.rtl`) to 445 KB (`23.sched2`), 21 files.
Reading one is not an option.  Greppping them is, and the workflow that worked
was: take a constant from the generated `.s`, count its occurrences per dump
to find the pass that changes it, then print the ~30 lines around the hit.

    26214 per dump: 00.rtl 13  01.sibling 13  02.jump 13  03.cse 21
                    07.gcse 33  08.loop 23  09.cse2 21  ... 18.greg 12

The 13 -> 21 step at `03.cse` located the pass in one command.  **Dump reading
is tractable at 800+ instructions if and only if you already have a specific
quantity to follow.**  Go in with a constant, a pseudo number or an insn
number; do not go in to look around.

## 7. ONE RETRACTION MADE INSIDE THIS BATCH -- AND THE GUARD THAT CAUGHT IT

On 2008c28, `-ffixed-r8 -ffixed-r9 -ffixed-r10 -ffixed-r11` takes size from
+44 to +4 and count from +19 to -2.  I read that as evidence that the original
toolchain's Thumb register pool excluded r8-r11, i.e. the REG_ALLOC_ORDER open
item in a new form.  **That is false, and the batch's own second target
disproves it.**  2009f3c's REFERENCE has the six-instruction high-save
prologue and 45 high-register mentions, parking constants in r9 and fp exactly
as our 2008c28 candidate does.  Tree-wide, **467 hand-written `.s` files carry
that prologue and 210 already have a landed `.c` sibling** -- so the shape is
demonstrably reachable from C with the flags we have.

`-ffixed` is therefore a MASKING flag here: denying the high bank forces
reload to rematerialise commoned constants from their `REG_EQUIV` notes, which
cancels an excess of commoning rather than preventing it.  No
`FIXEDHIGH_CFLAGS` row should be written, and these numbers must not be cited
as REG_ALLOC_ORDER evidence.  2008c28's park has the section struck in place.

The guard that caught it was simply **measuring a second function in the band
before writing the first one up as a general finding.**  A one-function
explanation of a band is a hypothesis about that function.

## 8. WHAT TO BRIEF NEXT IN THIS BAND

1. **Triage the 66 by branch count first.**  Straight-line (< 10 branches) and
   branch-dense (> 30) are different work.
2. **For the branch-dense ones, the existing 500-instruction lever set should
   apply essentially unchanged** -- their blocks are ordinary sized, so none of
   section 2 applies.  Anim_Ramses is the test of that and is untouched.
3. **For the straight-line ones, the open question is narrow and stated:**
   which source shape denies cse1 the cross-call commoning.  The target is one
   grep -- any candidate that leaves N separate `(set (reg) (const_int K))`
   insns in `03.cse` has solved it.  Everything else about these functions is
   already reachable.
4. **Do not spend budget re-sweeping constant spellings.**  `0xc8 << 3` and
   `0x640` are the same CONST_INT after parsing; whether a constant is pooled
   or built with `mov`/`lsl` is the thumb splitter's decision on the VALUE, not
   the spelling's.  The constant-set check in section 5 settles correctness in
   one command.
