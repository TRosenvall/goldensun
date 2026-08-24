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
