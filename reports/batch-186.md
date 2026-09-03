# Batch 186

Five elevated, no parks — and the round's largest result is not a function. The
selection filter had been reporting the pipeline as nearly exhausted, and it was
wrong by two orders of magnitude.

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `Func_80b874c` | `0x080b874c` | [rom_b8228_c_a_c_a_a_c_c.c](src/rom_b5000/rom_b8228_c_a_c_a_a_c_c.c) | naming a constant to reorder **global** allocation |
| 2 | `Func_8098294` | `0x08098294` | [rom_97b54_…_c_b.c](src/rom_8a000/rom_97b54_a_c_a_a_a_c_c_b.c) | first candidate — **only visible after recalibration** |
| 3 | `OvlFunc_899_20085bc` | `0x020085bc` | [ovl_30_a_c_a_c_c_a_b.c](src/overlays/rom_794ac0/ovl_30_a_c_a_c_c_a_b.c) | write the duplicate; let gcc cross-jump it |
| 4 | `OvlFunc_969_200b7c4` | `0x0200b7c4` | [ovl_314_…_a_b.c](src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_a_b.c) | transcribe a **dead statement**; boolean materialisation polarity |
| 5 | `OvlFunc_891_20094b8` | `0x020094b8` | [ovl_30_…_a_c_b.c](src/overlays/rom_78c76c/ovl_30_c_c_a_c_c_a_a_c_b.c) | the extra register was a **missing return value** |

## THE SELECTION FILTER WAS CALIBRATED ON THE WRONG FUNCTIONS

`tools/filtered.py` returned **four** candidates out of 1,251 unparked
functions. That reads like the work is nearly done. It is not — the filter's
thresholds had never been checked against this project's own record. Measured
over the 3,474 compiler-output `.s` files in the tree:

| the filter rejects | share of ALREADY-MATCHED functions it would have rejected |
|---|---|
| fewer than 8 calls | **85%** |
| outside 40–120 instructions | **77%** |
| uses r8–r11 | 8% |

Median matched size is **21 instructions** — below the filter's own floor of 40.
So `calls >= 8` would have discarded five sixths of this project's successes.
Those numbers describe whatever the filter's author was working on at the time,
not the functions that yield.

`--wide` keeps only the checks that survive contact with the record — hand-written
assembly excluded, a same-block repeated expensive constant excluded (proved
unreachable in batch 184), everything else ranked by size, smallest first, with
r8–r11 and the call count *reported* rather than rejected. **768 candidates
where the old filter offered four.** `Func_8098294` is the proof: 33
instructions, zero calls, matched on the first candidate, and structurally
invisible to the old rule.

The lesson is about tooling generally. **A screen whose thresholds come from the
cases in front of you will narrow to those cases and then report the work as
finished.** Check a selection rule against the outcomes it is meant to predict.

## HAND-WRITTEN ASSEMBLY IS NOT A CANDIDATE, AND THE TEST IS PER FILE

Recalibrating on size immediately surfaced a new hazard: the smallest remaining
functions are the MP2K sound driver, which ships as hand-written assembly and
was never C. Two patterns gcc-2.96 cannot emit identify it —

    mov r12, lr  ...  bx r12     link register saved in ip, not a push/pop frame
    bl .Lnnnn                    branch-and-link to a LOCAL label

**Zero** of the 3,474 compiler-output files contain either; 38 unparked
functions do.

**Test the FILE, not the function.** A `.s` builds one object and an object is
either compiled or assembled, never both — so one hand-written routine condemns
its whole translation unit. Per-function checking lets the small helpers
through, and those are exactly the ones that hurt: a five-instruction multiply
helper with no `r12` idiom of its own sorts to the very *top* of a
size-calibrated ranking while being just as unreachable as the driver around it.
File-scoping removed 28 such entries from the head of `--wide`.

## DUPLICATED ROM CODE MEANS DUPLICATED SOURCE

Three results from three different functions, all running against the instinct
to factor.

**A shared third block in a two-arm ROM is cross-jumping, not a shared source
block.** `OvlFunc_899_20085bc` is a two-level `if/else` with both inner arms
written out in full. gcc cross-jumps the two identical B copies and does *not*
merge the two A copies, though those are byte-identical too. One merge happened,
one did not. Hand-performing it with a `goto` is measurably worse.

**A `static` helper is not the same as duplicated source.** Factoring
`OvlFunc_969_200b7c4`'s repeated three-field test into a function called twice
went to **37** differing — gcc inlines it but shares the copies' structure
differently.

**A dead statement is source too.** That same function opens with a compare, a
branch, and one load whose destination is redefined on the next instruction —
the residue of a complete test whose result is overwritten. gcc does not delete
the dead non-volatile load. **An isolated compare-and-branch whose only guarded
instruction is a load into an immediately-redefined register means a whole
statement was dead in the source.** Transcribe it.

## THE EXTRA REGISTER WAS A MISSING RETURN VALUE

My own first diagnosis of `OvlFunc_891_20094b8` was wrong, and the correction
generalises. I read a third pushed register as the loop's zero wanting a
caller-saved home. It was the sprite slot id.

The ROM ORs *into the argument register* after the call. Written as separate
statements, the id is live across the second call and must take a call-saved
register — a third push, a copy per block, everything rotated. Written as a
nested call it is the callee's return and stays in r0 for free. **43 differing →
17**, fixing the push list, the rotation and the length in one step.

> **An `orr`/`and`/`add` on the argument register immediately after a `bl` says
> the operand is that call's RETURN value, not something the caller kept alive.**
> Re-read any candidate that pushes one register too many at such a site before
> reaching for an allocation lever.

## Branch polarity has a THIRD face

For one predicate gcc emits three shapes that are not interchangeable:

    ok = (A && B && C);                    false-first — 0 hoisted to the TOP of
                                           the chain, 1 out of line
    ok = 1; if (!(A && B && C)) ok = 0;    a third shape again
    if (A && B && C) ok = 1; else ok = 0;  true-first — 1 INSIDE the last
                                           compare's block  ← the ROM

**Which literal is out of line tells you whether the source was an
expression-assignment or an `if/else` statement.** Two measured non-levers on
this shape: De Morgan rewriting is byte-identical, and the flag's type is free.

## Two corrections to recorded rules

**Every literal stored through a halfword lvalue pools.** The 1b table says
values in roughly 1..0x7fff need no local. Measured: a literal `5` through a
`short *` pools and costs four bytes of pool; only `int five = 5;` gives the
ROM's `mov`. The recorded counter-examples were `u16` struct members of a wider
object, which may reach a different pattern — flagged for a corpus re-check
rather than edited blind.

**"Same constant, distinct variables cannot separate them" is true of GCSE
only.** Read from the dumps on `OvlFunc_891_20094b8`: cse2 rewrites the second
and third blocks' `mov rN, #0` into a *copy* of the loop counter's zero and
leaves the first block alone — path-dependent, not uniform. Distinct per-block
variables stop it. The split **does** separate a cse2 copy and does **not**
separate a gcse hoist.

## Other mechanisms worth keeping

**`bls` is a signedness statement about the counter.** A signed `int i` gives
gcc's reversed countdown; `unsigned int i` with `i <= 8` blocks the dbra rewrite
and produces the ROM's increment-and-compare. Read the condition code before
writing a `goto` loop.

**Return type, not prototype, is the declaration lever.** With the prototype
held constant, `void` against `int` on a callee flips the argument fill order by
itself. It is not "drop the declaration" — it is "the callee returns `int`".

**A large early deficit is NOT diagnosed by branch polarity.** My hypothesis on
`OvlFunc_899_20085bc` was that a 55-of-88 deficit meant the arms were ordered
wrong; reordering bought one instruction. Polarity errors show up as isolated
single-instruction replacements; a large contiguous deficit means a structural
misreading.

**`tryc.py` has a fourth label false-negative.** When a jump-over-pool's target
coincides with a join label, gcc emits two labels at one address and a
disassembly can only show one. Two candidates reported at "2 differing" were
byte-identical. When the entire residue is a `b Lx / Lx:` pair with nothing
between the labels, assemble both sides and `cmp` before believing the diff.

## State

- **1,862 functions remain in assembly** — 639 unparked and 293 parked in the
  main ROM, 607 unparked and 323 parked across the overlays. 3,494 elevated
  `.c` files.
- `make clean && make -j8 && make compare` green; SHA1
  `5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
  linked ELF, `.gcc2_compiled.` present in each object.
- Five splits, each verified byte-neutral before any `.c` landed. No new
  `CSE_CFLAGS` rules — every match this batch is at stock `-O2`.
- `Func_80a1a40` was attempted to 37 of 49 and set aside rather than parked;
  one spelling is not enough measurement to justify a park.
