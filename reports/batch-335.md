# Batch 335 — ten landings, and a four-batch bound that was one rung too high

Eight briefs, 32 targets, **10 landings**. **684 → 675 parked.**
**4,934 → 4,943 of 5,710 (86.6%).** Gated green; SHA1 unchanged.

Fourth batch on the length-gap criterion. Rate across the four: **8/15, 7/25,
8/25, 10/32.**

| brief | landed | parks improved |
|---|---|---|
| **A** | **3** | 2 |
| **E** | **3** | 1 |
| C | 1 | 3 |
| G | 1 | 3 |
| H | 1 | 2 |
| B | 0 | 4 |
| D | 0 | 4 |
| F | 0 | 4 |

**Six briefs got a TU pair**, assigned after computing which shortlist parks
share a translation unit. Two of those pairs landed whole; the rest produced the
clearest result available — *why* the piece cannot convert.

## Landings

| function | from | what closed it |
|---|---|---|
| `Func_80063bc` + `Func_8006408` | 3, 7 | **one TU** — a readiness barrier, an alias-set split, a closing barrier |
| `Func_8078870` | 5 | a one-member struct mask, device-free |
| `Func_80b5864` | 59 | reusing the dividend to flip `allocno_compare`'s sort |
| `OvlFunc_947_200a384` | 37 | `CSE_CFLAGS`, and **0 pins where the park had 14** |
| `OvlFunc_935_20089c0` | 57 | `CSE_CFLAGS`, and **0 pins where the park had 7** |
| `Func_801b424` | 55 | a `COMPONENT_REF` base, not a pointer |
| `OvlFunc_886_2008368` | 60 | a function-scope `int` carrier killing an HImode pool entry |
| `Func_80a51d0` | 43 | `_MSG_182` + a one-member union + assignment order |

Parks improved: `Func_8006240` 134 → **3**, `Func_80a9f10` 48 → **19**,
`Func_80a68ec` 47 → **23**, `DecodeMetatileset` 48 → **24**, `Func_8011b54`
48 → **14**, `Func_80a40ac` 36 → **14**, `Func_80c0a24` 47 → **34**.

## The finding: a priority bound is only valid if readiness is fixed

Batch 334 bounded `Func_80063bc` structurally and **every statement in it
reproduces** — `arm_adjust_cost` (`arm.c:2425-2427`) zeroes anti/output
dependences so the priority gap is fixed at one, priority falls monotonically
along the store chain, and `rank_for_schedule`'s class test
(`haifa-sched.c:4068-4095`) prefers the independent insn. The arithmetic was
right and the bound was wrong.

**`rank_for_schedule` only ranks READY insns**, and the `mov r3,#0x80` was ready
at t=0 solely because it had no predecessor. A bare `__asm__ volatile ("")`
gives it one, and the park went **3 → 0 on one line** — landing the TU partner
that could not ship alone.

Generalised in `docs/elevation.md`: **when a park proves an ordering, priority,
cost or tie-break cannot be won, check the PRECONDITION of the comparison
rather than its arithmetic.** Four preconditions with levers: *readiness*
(predecessors), *allocno membership* (a block-local pseudo is absent from
`.18.greg`), *pass applicability* (no `sched1` here; the register-pressure rung
is gated on `! reload_completed`), and *pattern matching* — a `define_peephole`
matches only **consecutive** insns, so brief H found one whose four conditions
were all satisfied and which still never fired because sched2 wedged an insn
between the pair. **That is a new blocker class: a peephole defeated by
scheduling.**

## Pins can be actively wrong

Brief E's two overlay landings came from **removing** pins, not adding them. On
`20089c0`, two of its seven had **aliased live values** — the stored zero pinned
to r7 while the unpinned actor pointer also took r7, so the body emitted
`str r7,[r7,#0x48]`, **storing the pointer instead of the zero**, and the
missing `mov r7,#0` was the entire length gap. And `200a384`'s whole stated
residue doesn't exist pin-free.

## Six corrections to my own briefing

1. **"Pin-free" was wrong on two targets** (14 and 7 pins) — the *third* such
   error in two batches, and systematic: the audit data has no pin column and I
   supplied the claim from nowhere. **Run `shimcount` on every target.**
2. **The brief tables' "lengths" were 16-bit counts, not stream lengths** —
   caught independently by briefs B, D and F. A 32-bit entry can absorb an
   instruction-count difference while the streams still agree, in which case
   positions line up. I fixed `parkcheck` after batch 334's version of this and
   **kept using the instruction-count numbers anyway**; the audit report's title
   and framing now say so.
3. **A piece I called two-member holds three** (`rom_20198`), with the third
   parked at 488 — so it was unlandable regardless.
4. **A piece I called four-member holds four with three parked**, and my
   "neither ships alone" was false: `split_s.py` cuts the last function out.
5. **A figure I gave was stale** — `Func_80788c4` is 14 of 62, not 39, taken
   from a four-day-old cached sweep.
6. **D and F got the same two filler targets.** F deferred to D's results, as
   asked.

## Two symbols are now retirement candidates

- **`_CONST_200` is no longer referenced anywhere** — `Func_80a40ac` is
  device-free, and `const.sym`'s stated reason for the entry ("no HImode
  expression for gcc to pool into") is **refuted**: a `(u16)` cast on the wider
  value makes the mask an HImode pool entry.
- **`_CONST_1f`** lost its last beneficiary in batch 333.
- Fixed in passing: **`_MSG_182` was defined twice** in `message.sym` (same
  value); the bare duplicate is replaced by a note.

## Carried forward

- **`_CONST_24` is now worth a whole function, not one encoding.** Batch 332's
  "one differing encoding" was an artefact of measuring the symbol as an
  unresolved extern; measured properly, parameter-reused + symbol is
  **byte-identical, 0 of 35**. Device-free is closed by proof:
  `calls.c:841-857` needs `rtx_cost > 2` and `arm.c` returns 0 below 256, so
  `0x24` can never get a pseudo.
- **Two `CSE_CFLAGS` members added by hand** with their reasons as Makefile
  comments, on the `owner-decisions.md:47-52` precedent that adding to an
  existing group is routine. Reversible.
- `Func_80a7440` (4) + `Func_80a7478` (178) **is not a two-for-one**: scheduling
  is per-function, so closing the big one still leaves the small one's 4.
- `shimcount.py:42` matches `.equ` only, so `__asm__(".set ...")` reports zero
  pins — a third blind spot, currently harmless.
- Empty-asm debt is now **12 files, 18 barriers**, and the new `rom_c0` pair
  holds four of them.
