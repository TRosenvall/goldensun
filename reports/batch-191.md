# Batch 191

Five elevated, two parked, one attempted and set aside. Four of the five are fakematches and one needs a
flag group.

**The batch's result is that candidate selection now predicts outcomes.** Batch
190 established ranking by template quality; this one found the second, missing
half — a template predicts whether the *data model* comes free, and says nothing
about whether the *residue* is tractable. The two questions turn out to be
independent, and both are cheap to ask before writing a line of C.

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `OvlFunc_948_2009694` | `0x02009694` | [ovl_30_…_c_b.c](src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_c_c_c_c_b.c) | **fakematch**; four recorded levers, diagnosed from the prologue |
| 2 | `OvlFunc_882_200be48` | `0x0200be48` | [ovl_30_…_a_c_b.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_c_b.c) | **fakematch**; teardown found ONE pin where four were added |
| 3 | `OvlFunc_935_20088a8` | `0x020088a8` | [ovl_2e0_…_c_a_b.c](src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_c_a_b.c) | **`CSE_CFLAGS`**; found by screening on high-register use |
| 4 | `OvlFunc_934_20094ac` | `0x020094ac` | [ovl_1300_c_c_a_c.c](src/overlays/rom_7bdeb0/ovl_1300_c_c_a_c.c) | **fakematch**; the teardown gives a size, not just a verdict |
| 5 | `OvlFunc_932_2008c9c` | `0x02008c9c` | [ovl_30_…_a_a_b.c](src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_a_b.c) | **fakematch**; pin *declaration* order is argument order |

Parked this batch: [`OvlFunc_968_2008cc8`](src/non_matching/overlays/2008cc8.c)
(104 of 140, a register rotation plus three instructions I did not account for)
and [`OvlFunc_943_2009a98`](src/non_matching/overlays/2009a98.c) (28 of 75, the
`-1` triple below). `OvlFunc_955_2009898` was attempted and abandoned at 117 of
126 without a park, because I stopped on budget rather than on a diagnosis and a
park with nothing behind it would claim knowledge I do not have.

Gated on a clean `make clean && make compare`, every address verified by
`tools/checkaddr.py` against `goldensun.elf` and the per-overlay `overlay.elf`.

## SCREEN ON HIGH-REGISTER USE, NOT JUST TEMPLATE QUALITY

`templated.py` ranks by how well a solved file matches a candidate's callees and
globals. That reliably delivers the struct layout, the argument order and the
extern block — twice this batch a neighbour supplied an entire extern block
including overlay-local callees, so no signature had to be derived at all.

It says **nothing** about whether the residue will yield. Two of the strongest
templates on the list went nowhere before that was noticed.

Counting `r8`–`r11` references in the target's body separates them cleanly:

| function | syms | r8–r11 uses | outcome |
|---|---|---|---|
| `OvlFunc_955_2009898` | 9 | **27** | abandoned at 117 of 126 |
| `OvlFunc_968_2008cc8` | 11 | **17** | parked at 104 of 140 |
| `OvlFunc_935_20088a8` | 7 | **0** | **elevated, first candidate** |
| `OvlFunc_934_20094ac` | 13 | **0** | **elevated** |
| `OvlFunc_932_2008c9c` | 10 | **0** | **elevated** |

High-register traffic means a function needs more values live than the low
registers hold, which is precisely what the allocation-order parks are made of.
**Rank on the template, then filter on `hi == 0`.** It is now a column in the
tool.

## The teardown gives a SIZE, not just a verdict

Every fakematch this batch was built up lever-by-lever and then torn back down,
removing each piece from the *finished* file. That is the only way to know what
was needed rather than what happened to be tried, and the numbers say more than
a yes/no:

| function | piece removed | differing |
|---|---|---|
| `OvlFunc_882_200be48` | r1 + r2 pins, two-step | 2 |
| | **r0 pin alone** | **0 — everything else was habit** |
| `OvlFunc_934_20094ac` | the `PlaySound` pin | 40 |
| | the named zero | 70, one line long |
| | only the `r0` pin | 2 |
| `OvlFunc_932_2008c9c` | the `TravelTo` pin | 14 |
| | the paired stack locals | 5 |
| | only the `r0` pin | 3 |

`OvlFunc_882_200be48` is the sharp case: the first form that matched pinned all
three argument registers *and* used a two-step constant — four pieces — and the
teardown showed **one pin on r0 was the whole lever**. Everything else was
habit. Elsewhere an `r0` pin was worth exactly two instructions, the smallest
contribution any pin has made.

**"Load-bearing" is not the same as "important."** Building up tells you what
helped; tearing down tells you what is still needed, and the size tells you
which pieces are doing the work. Only the torn-down form belongs in the file.

## Reading the screen: a "60 of 60" can be three instructions

`OvlFunc_935_20088a8` reported *60 differing of 60* at plain -O2 — apparently a
total mismatch — when exactly **three** instructions disagreed. One extra
instruction near the top shifts every later line and difflib then aligns almost
nothing.

**Read the trailing `N instruction(s) in disagreeing regions` line; the header
count is an alignment artefact whenever the lengths differ.** That candidate was
correct on the first try. The lesson paid immediately: two later candidates
reported 93 and 72 differing when the itemised regions showed 5 and 2, and both
would otherwise have been discarded.

## The `-1` triple is NOT an unbroken class

`tools/pickable.py` rejects any function with three or more `neg`, on the
strength of batch 148 recording the shape as unbroken:

    rom     mov r0,#1 / mov r1,#1 / mov r2,#1 / neg r1 / neg r2 / neg r0
    plain   mov r6,#1 / neg r6,r6 / mov r1,r6 / mov r2,r6 / mov r0,r6

gcc builds `-1` once and copies it, because three arguments want the same value.
Plain C measures a total mismatch.

**Pinning the four argument registers and negating in place reproduces the
triple exactly**, taking `OvlFunc_943_2009a98` from 75-of-75 to 27 with the
negations in the ROM's own order. The class was recorded as unbroken because the
fakematch idiom had never been tried on it, not because it resists. That
rejection should read *expensive*, not *impossible*, and the batch-148 entry
wants amending. The function is still parked at 28 on a second, independent
hoist.

## Smaller results

- **Pin *declaration* order is argument order.** The recorded rule reads
  "declaration order is argument-setup order", which invites the wrong reading:
  it is the order of the **declarations**, not of the arguments in the call.
  Declaring the pins `r1, r2, r0` leaves `r0` three slots late; declaring `r0`
  first lands it, with the call site written identically either way.
- **The `gState` base wants a named local by default.** Fourth function to need
  it. Any access past offset 255 folds to `=gState+N` otherwise.
- **A hoisted constant and a named local, checked in both directions.**
  `OvlFunc_934_20094ac`'s shared zero *is* a named local — the ROM materialises
  it into callee-saved r6 and then stores it, and three literal zeroes measure 70
  differing. That is the exact inverse of `OvlFunc_927_2009c34` last batch, where
  the ROM stored the literal straight to its destination and naming it was
  wrong. **The order of the first use separates them**, and the rule has now been
  tested from both sides.
- **The anchor-every-argument rule keeps needing bounding** — three consecutive
  rounds. Anchor the argument that participates; the teardown finds which.

## A tool was overwritten and restored

`tools/pickable.py` already existed and is imported by `filtered.py`. A new tool
was written to that path with `cat >`, destroying it and breaking the candidate
filter. It was caught from the resulting circular-import error, restored with
`git restore`, the tree confirmed to match HEAD, and the new tool rewritten as
`templated.py` after checking the name was free. Nothing was lost. **Check
before writing.**
