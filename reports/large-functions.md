# Do large functions break the method?

*Not a batch. An experiment, run because the answer changes what to work on.*

## Why this was run

Everything elevated in 36 batches is small: **median 25 instructions, maximum 65.**
The remaining work is not.

|  | functions | instructions |
|---|---|---|
| elevated on this branch | 321 | 8,564 |
| remaining | 2,920 | 458,896 |

**45% of the remaining instructions are in 244 functions of 400+**, and nothing
had ever been attempted at that size. Two extrapolations were possible and they
differ by an order of magnitude, so the question was worth one round.

Two functions were attempted, both ~140 instructions, chosen at opposite ends of
the shape spectrum: one straight-line with 29 calls and no branches, one with 30
calls, 8 branches and a five-way nest.

## The result

**Length is not the problem.** Neither function needed anything the small ones
did not, and both came out mostly correct on a first transcription:

| | instructions | disagreeing | first pass |
|---|---|---|---|
| `OvlFunc_887_20093e4` (straight-line) | 140 | **28** | 80% right |
| `OvlFunc_942_200851c` (branching) | 147 | **35** | 76% right |

Neither matched. But every remaining disagreement is either an **already-parked
blocker class** or ordinary scheduling — no failure mode appeared that does not
also occur in twenty-instruction functions.

## What actually stops them: blocker DENSITY

Blockers are per-call-site. A shape that appears at one call site in twenty is
nearly certain to appear somewhere in a function with twenty-nine of them.

Measured across all 2,920 remaining thumb functions, for three blocker shapes
that can be detected mechanically — arg-interleave, a pooled constant loaded
twice (constant-CSE), and two or more `neg`s (the rematerialisation class):

| size | functions | arg-interleave | dup pool | 2+ neg | **any of the three** |
|---|---|---|---|---|---|
| 1–20 | 180 | 14% | 8% | 1% | **23%** |
| 21–40 | 539 | 7% | 10% | 2% | **19%** |
| 41–80 | 837 | 14% | 22% | 7% | **39%** |
| 81–160 | 687 | 26% | 36% | 22% | **60%** |
| 161–400 | 433 | 47% | 76% | 35% | **89%** |
| **401+** | **244** | **76%** | **97%** | **69%** | **99%** |

**Essentially every large function contains at least one shape we already cannot
solve at any size.** `OvlFunc_887_20093e4` contains five instances of two of
them.

That reframes the problem. The binding constraint is not the number of
unelevated functions — it is **the number of unretired blocker classes**.
Retiring arg-interleave alone would clear a blocker from 76% of the 244 largest
functions. No amount of small-function work does that, because small functions
mostly do not contain the blockers, which is precisely why they match.

## One new lever, worth having

`OvlFunc_942_200851c` gave up 36 of its 78 disagreeing instructions to a single
change: **`goto` the ROM's join points instead of using early `return`s.**

The ROM has two exits — one that runs `__CutsceneEnd` and one that does not —
and five paths reaching them. Written naturally, gcc emitted `__CutsceneEnd`
three times and laid the blocks out differently. Written with two labels
mirroring the ROM's, that halved.

A short function has one exit and the question never arises. **This is the first
finding that is specific to size**, and it is a lever rather than a blocker: one
screen to find, mechanical to apply. Any function with multiple exits should be
written this way from the start.

## One tooling fix, forced by the experiment

`tryc.py` reported **"132 differ"** for a function whose streams disagreed in six
places totalling 36 instructions — and reported the same for every variant tried
afterwards, so the number could not rank them.

The count was positional: instruction *i* against instruction *i*. One extra
instruction shifts everything after it. That is survivable at twenty
instructions, where the listing can be read whole, and useless at 140.

`tryc.py --align` now reports disagreeing **regions** via `difflib`, and a count
of instructions inside them, which does fall as a candidate improves. Every
number in this document comes from it. **This should be the default lens for
anything over about fifty instructions.**

## Measured afterwards: what would and would not accelerate this

Three ideas were costed rather than argued about. Two are worth nothing and one
is worth a quarter of the remaining project.

**A mechanical C generator for straight-line call scripts — NO.** Most of what
has been elevated by hand is cutscene script: a run of calls with constant
arguments, which a generator could emit from the ROM. Measured across the
remainder: **110 functions, 5,758 of 458,542 instructions — 1%.** The population
that looks automatable has already been consumed by hand.

**Reading the compiler instead of probing it — YES, and it is free.** The
gcc-2.96 source is in the build image at `/opt/camelot-gcc/gcc-2.96/gcc/`. See
docs/elevation.md; the first question put to it took ten minutes and corrected a
conclusion that twelve hand-written probes had got wrong.

**A FRAGMENT matcher — measured YES, then measured again and NO.** The tool was
built (`tools/find_fragments.py`) and the first ranking was wrong. Both numbers
are kept below because the difference between them is the lesson. `match_shapes.py`
compares whole functions, which is why it only ever finds small ones: a
400-instruction function will never have the same whole-function skeleton as
anything solved. Splitting at labels and branches and matching BLOCKS instead:

| block coverage from the solved corpus | functions | instructions |
|---|---|---|
| **≥80%** | **104** | **70,795** |
| 60–80% | 92 | 44,741 |
| 40–60% | 52 | 17,745 |
| 20–40% | 87 | 27,002 |
| <20% | 343 | 152,225 |

43% of all blocks in large functions have an **exact** skeleton match in the
solved corpus, and the distribution is bimodal rather than flat: 343 functions
are genuinely novel, but **104 are four-fifths built out of blocks somebody has
already written C for.**

That looked like 70,795 instructions — 15% of everything remaining.

**It is not.** The ranking counted a block as covered if ANY solved function had
a block of the same shape, and 16% of the distinct block skeletons in the solved
corpus come *only* from fakematches — inline-asm register pinning, which is not
C anyone can learn a block from. Worse, the correlation runs the wrong way: a
function scores high on that metric largely by being full of the arg-interleave
shape, which is exactly the shape whose only "solutions" are fakematches.

Counting only blocks with a **real** exemplar (`--real`):

| block coverage, real exemplars only | functions | instructions |
|---|---|---|
| ≥80% | **1** | 589 |
| 60–80% | 5 | 1,768 |
| 40–60% | 17 | 4,188 |
| <40% | 655 | 305,963 |

Six functions, 2,357 instructions. **Half a percent of the remaining work, not
25%.** The tool is worth keeping as a "have I seen this block before" lookup,
and it is not the lever I said it was.

THE LESSON IS ABOUT THE METRIC, NOT THE TOOL. A measurement over a corpus that
contains 104 fakematches will silently measure the fakematches. Every future
count over the solved corpus should exclude them by default -- and this was
caught only because the first `--show` output was visibly full of
`[FAKEMATCH]` tags, which is luck rather than method.

## What this says to do next

Not "keep elevating small functions" — that is real value per round and it
converges on nothing, because the population it draws from is the one without
blockers in it.

The work with leverage is **retiring a blocker class**, and the density table
says which one: arg-interleave, present in 76% of the largest functions and in
five parked functions across two classes. It has resisted every lever in the
tree, and one member is known to be reachable only by pinning registers with
inline asm.

`src/non_matching/ovl_787e04/20093e4.c` and
`src/non_matching/ovl_7c6bac/200851c.c` carry the full attempt logs, including
what was tried and rejected, so neither has to be re-derived.
