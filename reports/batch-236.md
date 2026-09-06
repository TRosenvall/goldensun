# Batch 236 — a template lever that was not merely unnecessary but WRONG

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `Func_80b2f4c` | `0x080b2f4c` | [rom_b0070_c_c_a_c_c_b.c](src/rom_b0000/rom_b0070_c_c_a_c_c_b.c) |
| 2 | `OvlFunc_939_2008d30` | `0x02008d30` | [ovl_314_a_c_c_c_c.c](src/overlays/rom_7c460c/ovl_314_a_c_c_c_c.c) |
| 3 | `OvlFunc_939_2008eb0` | `0x02008eb0` | [ovl_314_a_c_c_c_c.c](src/overlays/rom_7c460c/ovl_314_a_c_c_c_c.c) |
| 4 | `OvlFunc_954_20093e4` | `0x020093e4` | [ovl_30_c_c_c_c_a_c.c](src/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_c.c) |
| 5 | `OvlFunc_956_2009c20` | `0x02009c20` | [ovl_30_c_c_c_c_a_c_a_c_c.c](src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c_c.c) |

Three of the five are whole-file landings needing no linker edit; entry 1 is a
three-way split, built and `make compare`d green with the function still in
assembly before any C was placed.

## SUFFICIENT-NOT-NECESSARY, CONFIRMED FROM THE HARMFUL SIDE

The rule that a template's levers are sufficient and not necessary has been
confirmed many times from the INERT side: you copy a lever, it measures zero,
you drop it. `Func_80b2f4c` is the first confirmation from the other side.

Its template's entire headline finding is that a byte-wide `-1` needs an
int-typed name to emit `sub` rather than `add r3, #0xff`. The sibling **does
not decrement at all** — it stores the poll result it just compared against
zero. Transplanting the lever costs 25 encodings and four bytes.

"Re-measure, don't transplant" is usually framed as avoiding waste. It is also
avoiding damage, and the damage does not announce itself: the transplanted
lever produces a plausible file that is simply wrong.

## A CALL-CLOBBERED PIN IS A THIRD REMEDY FOR THE COMMONED-CONSTANT TELL

The notebook records two remedies for a commoned constant widening the
prologue — the CSE flag group, and separate named locals — and guesses they
divide on WHERE the commoning happens. `2008d30` and `2008eb0` are one specimen
of each half, in one file, and the guess does not hold.

| function | shape | measured |
|---|---|---|
| `2008d30` | `GetFlag`/`SetFlag` across a branch — the flag-group shape | `-fno-rerun-cse-after-loop` exact, **AND an r0 pin exact** |
| `2008eb0` | commoning within a block — the separate-locals shape | separate locals 8 of 95; all three CSE flags 8 of 95; **only the r1 pin exact** |

So "the fix is the flag group, not the C" is too strong, and the
where-it-commons distinction does not predict the remedy.

Mechanism, which is why it should generalise: **naming a value says only that
the value exists**, and gcc may still keep one copy in a call-saved register.
Binding it to r0–r3 says WHERE it lives, and nothing in r0–r3 survives a `bl`,
so a later use must be rebuilt. That is ABI, not overlay-specific.

It is also what made `2008d30`'s file shippable whole. A TU takes one flag
group; the flag would have fixed the first function and not the second. Worth
one screen before parking a commoned-constant function on "neither recorded
remedy takes".

## A POOL-ORDER RESIDUE IS NOT ALWAYS A MODE QUESTION

Recorded: pool position is set by the MODE of the reference. That covers pooled
constants whose references differ in mode. When two pooled constants are
referenced in the SAME mode, `max_address` cannot separate them and the tie
breaks on **source statement order — reversed**, because sched2 swaps the
adjacent pair.

Two `int` stores to adjacent fields is the minimal case. In `2009c20` it is
worth 10 differing encodings at IDENTICAL size and encoding count, plus a whole
literal-pool rotation, and the fix is a one-line statement swap. `tryc` cannot
see this class at all.

Check the same-mode tie before varying types.

## THE FIRST-USE PIN RULE'S BOUNDARY IS ADJACENCY, NOT THE BASIC BLOCK

Recorded: two same-value calls in one basic block both need pinning. In
`20093e4` the two `-1` sites ARE in one basic block, with no branch anywhere
between them, and one pin suffices — because three calls intervene, and **a
call that clobbers the pinned hard register is as good as a branch**.

The same function bounds the rule from the other side. Pinning the SECOND site
is also exact, but needs a different spelling: whole-value `q1 = -1` fails there
(22 differing) and only the split build `q1 = 1; q1 = -q1;` works. Reading:
breaking the earliest occurrence destroys the CSE class outright, so any
spelling reaching r1 works; breaking a later occurrence must out-compete a
class that already exists.

## DISJOINT LIVE RANGES WANT TWO VARIABLES — ONLY IF THE VALUE CROSSES A CALL

In `20093e4` an actor is fetched twice, each fetch dying at its own
`__Actor_WaitMovement`. As one local it is a single long allocno; its priority
(`floor_log2(n_refs) * n_refs / live_length`) drops it one slot down
`REG_ALLOC_ORDER` and displaces every other callee-saved value — 44 differing.
Split into two, and all five registers snap to the ROM at once.

The other actor in the same function needs no split: it never crosses a call,
stays in r0, and splitting it is measured inert. Same in `2009c20`. A pointer
that dies before the next `bl` never competes for a callee-saved register, so
one variable suffices.

## Siblings are not twins

`20093e4` and `2009c20` share a skeleton, a prologue, a four-leg two-actor walk
and the same `0xcccc`/`0x6666` pair. They are still two jobs, because their
REGISTER ROLES differ — where one keeps a value in a low callee-saved register
the other keeps it in a high one, and the store that costs a `mov` in one is
direct in the other. One of them even wants the OPPOSITE source order for the
adjacent-field stores. No sed pair would have done it.

`solved_twins.py` reported zero for every function in this batch. A twin miss is
not a family miss: `20093e4` was solved off an idiom-grep that found a **parked**
function carrying the exact travel preamble. A park blocked on something
unrelated is still a correct idiom template.

## Flag groups

None of the five depends on one. Worth stating for `20093e4`: its `-1` residue
is exactly the shape `-fno-rerun-cse-after-loop` is normally reached for, and
that is not what cures it.
