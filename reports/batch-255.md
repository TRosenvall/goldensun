# Batch 255 — the residue was never at the differing site

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_956_2009f90` | `0x02009f90` | [ovl_30_c_c_c_c_a_c_c_a.c](src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c_a.c) |
| 2 | `OvlFunc_926_2009494` | `0x02009494` | [ovl_314_c_c_a_c_c_c_a_a_c_c_c.c](src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_c_c_c.c) |
| 3 | `OvlFunc_925_200856c` | `0x0200856c` | [ovl_314_c_c_c_a_a_b.c](src/overlays/rom_7b0400/ovl_314_c_c_c_a_a_b.c) |
| 4 | `OvlFunc_945_200d2f4` | `0x0200d2f4` | [ovl_30_…_a_a_c_b.c](src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_a_c_b.c) |
| 5 | `OvlFunc_953_2008710` | `0x02008710` | [ovl_30_c_c_c_a_a_a_c_c_c_a.c](src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_c_c_a.c) |
| 6 | `OvlFunc_909_20099b0` | `0x020099b0` | [ovl_30_c_c_c_c_c_c_c_c_c_c_c_a.c](src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_c_c_a.c) |

Four whole-file landings, two splits. **8.1 KB of code** against the previous
batch's 4.6 KB from nine functions — the remaining corpus is bigger per function
and the rate held anyway.

## Parked

| function | address | floor | blocker |
|---|---|---|---|
| `OvlFunc_943_200ab7c` | `0x0200ab7c` | **3 of 97** (one a benign linker alias) | uncharacterised, recorded as such |
| `OvlFunc_945_200d0e4` | `0x0200d0e4` | **4 of 206**, exact size/count/relocations | sched2 hoists a constant across two calls |
| `OvlFunc_890_2009ca8` | `0x02009ca8` | **5 of 456**, exact size, relocations identical | constant commoning with no block for cprop |
| `OvlFunc_943_200ac84` | `0x0200ac84` | **57 of 530**, exact length, 45 register swaps | `find_reg` conflict/preference pass |

## THE THEME: THE RESIDUE WAS NEVER AT THE DIFFERING SITE

Three independent results this batch, and all three say the same thing.

### A reload scratch is round robin over a set the source controls

Two functions stalled where **every** differing encoding was a reload scratch —
`movs r3` against the ROM's `movs r2`. No spelling of the differing site moves
it, because that is not where the decision happens.

From `reload1.c`: `spill_cost[]` is zeroed per insn so free low registers tie at
0; `find_reg`'s tie-break is `inv_reg_alloc_order` so **r3 wins every tie**; the
winners become `spill_regs[]` sorted ascending; `allocate_reload_reg` then picks
per insn by **round robin over that array**. All ties ⇒ singleton `{r3}` ⇒ every
scratch is r3. The ROM's `r3,r3,r2,r3,r2,r3` is round robin over `{r2,r3}`.

**The cure was one line, 500 bytes away** — a store written as a single
expression puts its offset out of thumb `str` range, forcing one extra reload
which takes r2. Same spelling closed both functions, in different overlays, with
different residues.

> **Count `Using reg N` in `.18.greg`. All one register where the reference
> alternates two means the spill set is too narrow — add a reload elsewhere and
> never touch the differing site.**

One of these had survived **182 measured perturbations at exactly 4 differing**,
70 of them ties, before the mechanism was read rather than searched for.

### A pin can be needed at the END of the ladder

`2008710`'s last two encodings sat at a site **inert to all six fill orders, both
widths, no pin, and every barrier**. What closed it was a pin at a *different*
site twenty instructions earlier.

Every minimisation routine here searches **downward** from "all pinned", so this
residue is structurally unreachable by the existing tooling. Its ordering
analogue turned up the same week: **three individually inert fill orders were
worth 7 together.**

### Control flow is the lever for a CSE problem; a barrier is not

`2009f90` came down to one lever worth **140 → 15**: a guarded `do{}while` with
the guard's address expression written out again. The **join** stops `cse_main`'s
extended basic block, so `loop.c` hoists the body's copy afterwards.

Against it, from the previous batch: `do { } while (0)` is **inert** against a
cse_main merge. A barrier orders; only a real join separates CSE classes.

And the named local is not a weaker version of the lever — it **is** the defect:
it commons the two expressions *and* loses r8 from the push mask.

## `hi=0 hiv=0` IS NOW FIVE FUNCTIONS DEEP

| function | gcc commons | ROM holds | plain differing |
|---|---|---|---|
| `2009494` | ~20 shifted constants | `push {r5, lr}` | 152 of 858 |
| `2008710` | 7 values | 3 | 600 of 665 |
| `20099b0` | 7 values | 2 | 754 of 807 |

On all three the **first differing encoding was the push mask**. Pinning
*everything* over-evicts (611 on one); the rule that worked is **pin every site
except the uses of values the ROM holds**.

`2009494` needed **233 pins, one per call site**, and was only tractable because
an agent wrote a generator emitting the pinned body from the `.s` — 233 of 233
generated, exactly 6 flagged for hand work.

## A PIN-INDUCED MISCOMPILE IS A LOCAL MINIMUM THAT CANNOT REACH ZERO

On the `2009ca8` park, pinning the split zero to r7 reaches **3 of 456 — the best
number any spelling produced — and emits `strb r7, [r3]`, storing the pointer
instead of the zero.** Another value is also in r7 and gcse commoned into the
pinned pseudo.

That is the **second pin-induced miscompile this week, both r7.** The earlier one
came out at the ROM's exact length with the count *improving* 123 → 78.

> A miscompiled candidate **cannot** reach zero differing — matching bytes would
> mean matching programs. So it is a local minimum with a floor above zero: it
> absorbs a round looking like the most promising branch, then stops. **A lower
> objcmp number is not evidence when a pin is involved.**

r7 specifically: callee-saved, low enough for Thumb everywhere, *and* gcc's frame
pointer expression — the register most likely to be doing two jobs.

## OTHER RESULTS NOW IN THE DOCS

- **The hole test needs a definition-side clause**, worth 291: a site is a hole if
  the ROM reads an argument out of a high register *or writes one into one*.
- **Held constants are found by evicting the EARLIEST occurrence** — complement of
  the recorded held-side rule, and cheaper.
- **A constant used only inside `if` conditions needs a statement-expression pin.**
- **A run of script-pointer uses wants a LOCAL, not pins.**
- **`adds rN, #1` is a live-across-a-call tell**, not `use_related_value`.
- **A backward `goto` hides a loop from `loop.c`** — and is not equivalent to
  `-fno-strength-reduce`.
- **Local arrays lay out in reverse declaration order.**
- **`__Func_8092a1c` wants its pooled pointer first** — third site, promoted to a
  callee rule.

## TOOLING: THE PARK CORPUS IS NO LONGER 86% INVISIBLE

`tools/funcindex.py` resolves a function to the file that currently defines it,
by name rather than by a remembered path. `close_parks.py` went from screening
**75 of 521 parks to 521 of 521**.

The cause of the old blind spot was **prose format**, not stale paths as
previously reported: it required one exact English phrase, which 141 parks
matched and 75 still had a live path for. Overlays sharing address space was the
subtlety — one park address has **thirteen** candidates, and the park's directory
is what disambiguates.

First result from the wider view: **close_parks still reports zero parks within
six instructions**, now over the whole corpus rather than a 14% sample. That
confirms distance does not predict tractability — the parks that fell this week
were at 7, 17, 21, 25 and 67 differing, while the one that will not move is at 2.

Second result: **22 parks name a subject that is already elevated**, candidates
for retirement, listed but not acted on.

## TWO HARNESS TRAPS

- **`b .L… / .pool_aligned / .L…:` is a pool jump, not control flow.**
  `draft_script.py` reads it as a join and split one call in half.
- **A generator whose knobs replace a one-line call with a multi-line block
  renumbers every later site.** Symptom: a lever that works in one base and
  inverts in another. Cause was an off-by-six index.

## Verification notes

One agent reported an instruction-stream match plus a pool check **instead of** an
objcmp verdict. Its reasoning was sound and objcmp confirmed it independently at
2,236 bytes — but the authority was run here rather than the substitute accepted.

An agent's landing notes also gave a function's overlay incorrectly in a previous
round; every path in this batch was resolved with `funcindex` before landing
rather than taken from the report.

## Gate

`make clean && make -j8 && make compare` green, `goldensun.gba: OK`. Every
address checked against the linked ELF. `--orphans` 0; `--unlinked` unchanged at
10.
