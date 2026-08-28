# Batch 120 — three silent failures, and the levers that came out of them

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. All 18
symbols read back out of `goldensun.elf` / the linked overlay ELFs.

**18 elevated, 6 parked. 2332 → 2314 remaining.**

## A filter that filtered nothing

Round 6's worklists were built with a `blocked_cse.py` pre-filter and the four
agents were told so. **The filter did nothing.** The extraction was

    blocked_cse.py --list 4000 | awk '/insns/{print $4}'

and on a line like `104 insns  3 repeats  OvlFunc_955_2009424  asm/...` field 4
is the word **`repeats`**. The exclusion set was 113 identical copies of that
word. `sort -u` on it returns one distinct value.

Two agents independently reported spending a fifth of their budget on genuinely
blocked functions, and I assumed they were describing an older round until I
looked. One of them counted the cost precisely: **4 of its 5 non-matches** were
functions the filter should have removed.

This is the third silent-empty-set failure this session, after a verification
loop that read a list inside the container which had been written outside it
(batch 119), and a corpus detector whose regex matched nothing because generated
`.s` files use gcc's notation rather than the ROM's (also 119). All three
produced a confident pass over nothing.

> **A generated list is data. Look at three lines and the distinct count before
> trusting it.** None of these three failures announced themselves; the only
> symptom in each case was a result that looked like ordinary bad luck.

## Corrections to my own guidance

**The `.call_via` helper.** Mid-round I circulated "bind the callee symbol inside
the helper" as a general improvement. It is right for **one** call site and wrong
for several: binding costs a reload per site, and a three-site function emitted
`ldr r4, =F` three times (62 lines / 58 differing). The form that matches there
passes the callee as a plain argument constrained `"r" (f)` with `bx %1` in the
template, so gcc CSEs the address into one register held across all the sites.

**The clobber list.** I also said "never add `r2`/`r3`". Wrong as a blanket rule
— it is a per-function reading and it is measurable from the ROM. `"r2"` is
*required* on one function (the ROM's `mov r6, r2` prologue proves gcc believes
r2 dies), `"r3"` is required on another, and it is neutral on a third. The test:
look for a loop-carried or call-crossing value living in r2 or r3.

**`"lr"` should be dropped by default.** `mov r12, pc / bx rN` never writes lr;
two functions hold a live value in r14 across their call sites and cannot
reproduce it with `"lr"` clobbered. `"memory"` and `"r12"` are the only two that
are always right.

**A correctness trap, not a match failure.** Two call sites in one expression
*miscompiles*: both outputs bind `register __asm__("r0")`, so gcc treats them as
one value — `f(a,a) + f(b,b)` produced a single `bx` followed by `lsl r0, #1`,
doubling one result and dropping the other.

## A predictor I built, validated, and threw away

After parking two functions on the single-block pool-loads-first shape I wrote a
static detector for it and wired it into `script_candidates.py`'s ranking. Then I
validated it against four known outcomes:

| function | score | outcome |
|---|---|---|
| `200bd10` | 5 | parked |
| `2009490` | 3 | parked |
| **`200b130`** | **3** | **matched** |
| `2008328` | 1 | matched |

It does not discriminate. Folding it into the ranking would have hidden a
function that matched, and it cut the "clean" pool from 31 to 10 on a false
signal. Reverted to an advisory column with the measurement in the docstring.

## Levers

**Build a constant AFTER the call if the ROM does.** `OvlFunc_881_20081c4`
drafted with r5, r6 **and r7** against the ROM's r5, r6, because a constant was
written before a call and used after it. The ROM builds it after and keeps it in
**r4** — call-clobbered under this tree's `-fcall-used-r4`, therefore free. A
value live across a call cannot go there. Splitting the statement: 24 differing
(first diff at position 0) → 12 (first diff at 24).

**Naming one level too many costs a callee-saved register.**
`OvlFunc_932_200ad58` drafted at 72 of 69 at 74 lines. Not a missing lever — two
locals too many. The global's *address* wants a name and the byte *offset* wants
a name; the dereferenced *value* does not, and neither does a literal zero stored
through a byte pointer. Removing those two: **72 differing → 2**.

Both show up in the push list before they show up anywhere else: **if your
prologue saves a high register the ROM does not, count your locals before
reaching for a lever.**

**`-fno-strength-reduce` also fixes gcc reversing a loop the ROM does not.** The
documented use is a recomputed table offset. A different symptom: the ROM has
two independent induction variables (a plain counter plus a recomputed index)
where gcc emits one strength-reduced pointer. 65 differing → exact, and the
`while(1)`-increment rewrite did nothing.

**A symbol substitution can prove the instruction stream and still fail on the
pool.** gas dedupes two `ldr =K` into one word, while a symbol and a literal are
two entries — so for a *repeated* pool constant the symbol technique can never be
the answer. It remains an excellent diagnostic for isolating whether the CSE is
the only blocker.

## Build rules added

Four this batch: `STRENGTH_CFLAGS` for one TU, an explicit `-O2` rule overriding
an `O1_CFLAGS` wildcard that was wrong for its TU (34 differing at `-O1`, exact
at `-O2`), and two `CSE_CFLAGS` rules. That is the **second** mis-scoped wildcard
found this session; a sweep of the wildcard rules against their current members
is worth doing.
