# Batch 163

Five elevated. Verified after a clean `make clean && make compare`; SHA1
`5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked images.

| function | address | image |
| --- | --- | --- |
| OvlFunc_968_2008b98 | 0x02008b98 | overlays/rom_7f2f14/overlay.elf |
| OvlFunc_954_2008490 | 0x02008490 | overlays/rom_7db0c8/overlay.elf |
| OvlFunc_946_20092b4 | 0x020092b4 | overlays/rom_7ced6c/overlay.elf |
| OvlFunc_903_2008d68 | 0x02008d68 | overlays/rom_798dc4/overlay.elf |
| Actor_SetAnimAndSpeed | 0x0800c388 | goldensun.elf |

## THE PARK CORPUS IS A CANDIDATE POOL THAT REFILLS

Three of the five came out of `src/non_matching/`, not from the candidate
scanners. That is the batch's finding, and it changes where a round should start.

A park is a snapshot of what was known the day it was written. The lever
inventory grows every batch, so parks quietly become reachable without anyone
touching them. `tools/park_retry.py` ranks parks that are close (<=20 differing)
and whose notes never mention a lever whose residue shape they carry; 112
qualify.

The three recoveries:

  * `OvlFunc_946_20092b4` -- parked at TWO differing on the ORR-destination
    residue, with two spellings listed as tried. `unsigned char m = 2;` matched
    outright. The park predated the lever.
  * `OvlFunc_903_2008d68` -- same residue, and its park concluded "it is
    register allocation, not operand order, and nothing in docs/elevation.md
    reaches it". It listed "the constant as a named local" among four attempts
    and **did not record the type**. It was an `int`, and the doc's own
    sharpening is that an `int` local here is folded and is *no lever at all*.
  * `Actor_SetAnimAndSpeed` -- parked at 2 differing on argument fill order,
    with the park itself observing that "the order tracks the CALLEE, not the
    argument expressions". That is the no-prototype lever's signature exactly,
    and the park never tried it. Deleting `Sprite_SetAnim`'s declaration
    matched.

**Operational rule: when a park says a named local was tried, check whether it
records the TYPE. If it does not, the lever has not been tried.**

## The boundary, so this is not read as a blanket instruction

`OvlFunc_947_200a1ac` carries a residue that looks identical to the two that
closed -- `mov r4, #0x8 / and r3, r1 / orr r3, r4` against ours -- and
`unsigned char` is MUCH worse there: 47 differing and one line long, against 9
for the `int`. The difference is what the ROM wants from the constant. In the
two that closed, the constant is the ORR DESTINATION. In 947 the requirement is
that it stay LIVE across two flag updates, and a narrow local will not do that.
Same-looking residue, opposite remedy. Recorded in that park.

And a thorough park stays parked: `OvlFunc_947_2009fd4` is also at 2 differing
on an `orr` residue and HAS tried the narrow local, at 18 differing, with five
other spellings.

## A tool bug that was hiding matches

`tools/protolever.py` scored a MATCH as the worst possible result. `tryc` prints
`  OK <name> (N lines)` indented; the tool did `out.strip()` and then tested for
`" OK "` with a leading space, which the strip had removed. So an OK row scored
10**9.

It printed two OK rows for `Actor_SetAnimAndSpeed` and then reported
`best: 4 differing, as written`. The match was visible in the transcript and
invisible to the summary. Fixed to match on the token rather than on surrounding
whitespace, and unit-tested against four output shapes.

Worth generalising: a tool that RANKS results can hide the thing it was built to
find, and it fails silently because the rows it prints are still correct. Any
sweep whose summary disagrees with its own output is reporting a bug, not a
result.

## Two levers found on fresh candidates

**One register for two unrelated values means ONE variable.** The ROM computes a
field into r6, tests it, then reuses r6 for an unrelated result;
`OvlFunc_954_2008490` written with two variables rotates the whole dispatcher at
38 differing, and with one variable it is exact. This is the inverse of "a
variable with disjoint live ranges should be two variables", and it also took
`OvlFunc_896_200c260` from 78 differing to 6 -- a park that had been recorded the
previous round as an unreachable register rotation. Before writing off a rotation
park, check whether the ROM REUSES a register: it turns an allocator question
into a variable-count question.

**The entry-block naming lever has a budget.** `OvlFunc_968_2008b98` with six
constants named held all six live and cost high registers -- 98 lines against 85,
89 differing. Naming only the constant whose build the ROM actually splits gave
85 lines and 4 differing. Name the interleaved one and nothing else; the failure
shows in the PROLOGUE before any diff is read.

## Also this cycle

Twelve stale parks were swept -- they name only functions since elevated,
including one for `Func_80b86ec`, elevated from a solved twin two batches earlier
with its park left in place. One cost real effort: `OvlFunc_919_2008200`'s park
describes a live register-role swap and the function is already done. Moved to
`toDelete/stale_parks/` rather than deleted.

And a negative on process, recorded because it cost screens: `Func_80170c4`'s
park lists a shared-exit `goto` giving correct length and 8 differing with r4/r5
exchanged. Restructuring as `if (n > 0) { ... } return d;` reproduces exactly
that. `park_retry.py` ranks on what prose does NOT mention, which is a heuristic
for generating candidates -- the park itself is still the thing to read BEFORE
screening.
