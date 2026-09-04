# Batch 192

Six elevated, **zero net parks** — the one park written this batch was withdrawn
when it turned out to be wrong.

That withdrawal is the batch's main result. It is not a new lever; it is a
correction to how the notebook decides that something *has no* lever.

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `OvlFunc_924_20094cc` | `0x020094cc` | [ovl_f84_…_a_c.c](src/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_a_c.c) | **`CSE_CFLAGS`**; one guard mixes signedness |
| 2 | `OvlFunc_901_2008d84` | `0x02008d84` | [ovl_314_…_a_a_a.c](src/overlays/rom_797990/ovl_314_c_c_a_c_a_a_a.c) | **fakematch**; an uninitialised pin moves its assignment |
| 3 | `OvlFunc_898_20092c0` | `0x020092c0` | [ovl_314_…_c_c_b.c](src/overlays/rom_793768/ovl_314_c_c_c_a_c_c_a_c_c_b.c) | one screen — kin of #2, slot changed |
| 4 | `Func_80b04dc` | `0x080b04dc` | [rom_b0070_…_c_a_b.c](src/rom_b0000/rom_b0070_a_a_c_a_c_a_b.c) | two pool loads and a subtract = symbol addresses |
| 5 | `OvlFunc_881_200b2f0` | `0x0200b2f0` | [ovl_30_…_c_c_c_c.c](src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_c_c.c) | **fakematch**; *previously parked by me, wrongly* |
| 6 | `OvlFunc_881_2009b5c` | `0x02009b5c` | [ovl_30_…_a_a_a_b.c](src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_b.c) | **fakematch**; the function that disproved the park |

Gated on a clean `make clean && make compare`, every address verified by
`tools/checkaddr.py` against `goldensun.elf` and the per-overlay `overlay.elf`.

## "N SPELLINGS TIE" IS ONLY EVIDENCE IF THE SPELLINGS DIFFER IN STRUCTURE

`OvlFunc_881_200b2f0` was parked at 4 of 99 with the claim that the `-1`
triple's `mov` order was unreachable. The evidence offered was **six spellings
tying at exactly 4** — which this notebook has treated as a reliable signal for
many batches.

It was wrong. All six kept the three assignments together and the three
negations together, and varied only the order *within* those groups:

    p0 = 1; p1 = 1; p2 = 1; p3 = 0;    /* six permutations of this half */
    p2 = -p2; p1 = -p1; p0 = -p0;      /* and of this one */

Interleaving them matches:

    p0 = 1; p0 = -p0;
    p1 = 1; p1 = -p1;
    p2 = 1; p2 = -p2;
    p3 = 0;

**Six permutations inside one shape are not six unrelated spellings — they are
one spelling tried six times.** The tie was real and measured; the inference
from it was not.

The mechanism is clear in hindsight. Grouped, the three `mov`s have no
dependence on anything — all three registers receive the same value — so gcc
orders them by scheduling and source order gives it nothing to act on.
Interleaved, each `mov` is pinned in place by the negation that immediately
consumes it. The dependence is what carries the order.

**The test to apply before writing a park: can you name the structural
assumption every attempt shared?** If you can, that assumption is the next thing
to vary, and the park is premature. A tie is evidence of a wall only when the
spellings differ in grouping, statement boundaries or variables — not merely in
order over one skeleton.

The sibling `OvlFunc_881_2009b5c` is what surfaced it: a function with the same
triple whose `mov`s were displaced differently, which made the grouped shape
visibly insufficient in a second, independent case.

## The `-1` triple is fully reachable

Batch 148 recorded it as an unbroken class. Batch 191 amended that to "reachable
by pinning" when the *negations* were reproduced, with the `mov` order still
believed out of reach. Both halves are now reachable — pin the four argument
registers and interleave each assignment with its own negation.

`pickable.py` rejects any function with three or more `neg` on the strength of
the original entry. That rejection now has **no basis beyond cost**.

## Other levers

- **One guard can mix signedness, and getting it wrong is a semantic bug.**
  `OvlFunc_924_20094cc` guards on `sub / cmp / bhi` — unsigned, the one-sided
  range check where a value below the low bound wraps and fails — immediately
  followed by two *signed* comparisons on a different field. Written with a
  plain `int` the first compiles to `bgt`, which never checks the low end at
  all: it builds, it looks right, and it is wrong. Read each condition code
  separately.
- **Two pool loads and a runtime subtract mean two symbol addresses.** gcc folds
  literal arithmetic, so `ldr =0xcc6 / ldr =0xc9b / sub` cannot be two integers.
  The message ids are absolute symbols in `message.sym` and the idiom is
  `(int)(&_MSG_xxx)`. The reading matters beyond the instructions:
  `msg += _MSG_cc6 - _MSG_c9b` is an *offset from a base line*, not an
  assignment, and `msg = 0xcc6` would compile to one pool load and lose both.
- **An uninitialised pin moves its assignment.** A `register` declaration *with*
  an initialiser pins its `mov` to the declaration point, which cannot be moved
  past an intervening statement by reordering declarations. Declaring the pin
  bare and assigning later places it. So a pin has two independent knobs:
  declaration position sets the register's place in the ordering, assignment
  position sets when the value is materialised.
- **The int-local escape for a halfword literal has a placement.**
  `*(short *)(p + 6) = 0xa0 << 8` pools as `=0xffffa000` (negative at short
  width) — ordinary blocker 1b. But naming it is not enough: with a call in the
  same statement the local must survive the call and gcc gives it a
  callee-saved register, widening the prologue. Compute it *after* the call.
- **Two signed byte reads, two instruction sequences, and the source picks.**
  Read straight into an `int`, gcc emits `ldrsb` (with its offset in a register,
  since `ldrsb` has no immediate form); assigned to a `signed char` local first,
  it emits `ldrb` plus an explicit `lsl`/`asr`. Both compare identically, so the
  difference is invisible in the semantics and shows up only as two
  instructions.

## The tool is feeding on its own output

`OvlFunc_898_20092c0` scored a perfect 1.00 against the file elevated in the
*previous round*, and the two turned out to be the same routine driving slot
`0x13` rather than `0x12`. Normalising the slot constant, the two disassemblies
diff to nothing. It matched on the first screen where the original had cost
eight.

That check was worth running rather than assuming — two cutscenes that look
alike usually differ somewhere in their marks or waits, and a normalised diff
costs one command. **Expect this compounding to continue**: each function landed
becomes a template for its kin, and `templated.py` surfaces them automatically.

## Selection is holding

Every function attempted this batch was chosen on the batch-191 criteria —
rank by shared-symbol count, then filter on zero `r8`–`r11` traffic. Six
attempts, six elevations, no abandonments. The one park was withdrawn on
further work rather than on a bad pick.
