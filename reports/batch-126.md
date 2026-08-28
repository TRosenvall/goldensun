# Batch 126 — the twin queue emptied, and two levers sharpened against each other

Verified on a clean `make clean && make compare` — `goldensun.gba: OK` — with
every address read out of the linked ELF.

## Elevated (2246 → 2241)

| function | address | found by |
|---|---|---|
| `Func_8094730` | 0x08094730 | twin of `OvlFunc_970_20090d4` |
| `OvlFunc_924_200b788` | 0x0200b788 | twin of `OvlFunc_923_2009208` |
| `OvlFunc_969_20085ec` | 0x020085ec | twin of `OvlFunc_925_20089fc` |
| `OvlFunc_934_20097d8` | 0x020097d8 | twin of `OvlFunc_922_2008180` |
| `Func_809ad90` | 0x0809ad90 | integer-local address class |

`solved_twins.py` now reports **0 hits** at both its default threshold and at
length 8 — the queue batch 125 opened is empty. It is worth re-running each
round: the solved corpus is a build artefact and grows every time something
lands, so new templates keep appearing.

Three of the four twins were a rename and nothing else. `Func_8094730` also
needed the `__` import prefix dropped from two callees, since it is main-ROM
code and its template was an overlay.

## Trust the template's header over your reading of the asm

`OvlFunc_924_200b788` took three screens instead of one, and the reason is worth
recording. Its template's header documents two levers as measured: three `-1`
locals assigned **before** an early return, and `p = a + 0x55;` written **after**
a call. Reading the target's asm, both looked wrong — the ROM builds the `-1`s
and the pointer inside the guarded block, after the call.

I "corrected" both. Moving the `-1` triple inside the guard let gcc CSE it into
one build (67 of 80 differing); moving the pointer gave 4 of 80. **The
unmodified template with only the rename was exact.**

Where the ROM's instruction order appears to disagree with a template's notes,
the ordering is the optimiser's, not the source's — the dominating-block lever
puts the assignment where gcc decides, which is not where the C statement sits.
Working order for a twin: rename and screen; if that fails, diff the two ROM
listings for missed immediates; only then move statements, and re-read the
header first.

## The named-pointer lever, finally pinned

Batch 125 recorded that this lever is pressure-dependent and that its stated
precondition — the offset must be mutated after the pointer is taken — is
necessary but not sufficient. `Func_809ad90` gives the other half.

Its only defect was one store folding into a reg+reg access where the ROM
materialises the address. Naming the destination pointer was exact next screen,
**even though the offset is dead afterwards**.

What separates it from the case where the lever failed is the *other operand*:

- `Func_809ad90` stores a **loaded value**, which needs a register of its own.
  The offset register cannot double as it, so the address must be materialised.
- `OvlFunc_881_200808c` stores a **bare constant**, which gcc can put in the
  dead offset's register — leaving the addressing mode available and the fold
  unavoidable.

`OvlFunc_899_20099a4` confirms it from a third angle: the ROM there materialises
the address and then reuses the offset register for the stored constant, which
is only possible once the address is already in a register — and ours reproduces
that with no coaxing at all.

**Usable form:** when the ROM materialises an address, look at what is stored or
loaded alongside. If that needs a register the offset cannot supply, name the
destination pointer. If it is a bare constant, expect the fold and do not spend
screens on it.

## A lever that undoes a match

`Func_80a5fe0` is parked at 7 of 34. The useful half is reusable: the ROM ends
with gcc's branchless `neg / orr / lsr #31` idiom, and `return rec[0] == 2;`
gives a branch (31 lines) while writing the comparison as a **value** —
`v = rec[0] ^ 2; return 1 - (v != 0);` — produces the whole sequence and reaches
34 of 34.

What remains is register roles in that tail, and the prescribed lever for
exactly that — naming the constant `1` as a local — does not merely fail, it
**returns the branch**, in all three placements tried. The constant-as-
destination lever and the branchless-idiom shape are in direct conflict: the
first wants the constant in a named local, the second needs the return to stay
one arithmetic expression.

## Parked

- `Func_80a5fe0` — 7 of 34, above.
- `OvlFunc_901_2008a80` — 2 of 30, argument-setup order. Second instance of the
  class. The two instances place the `mov r0, #0` in **different** slots, so it
  is not a convention being missed; and `--no-sched2` makes both worse, so the
  ROM was built with that pass running and the difference is in what it was
  handed.
- `OvlFunc_881_200808c` + `20080d4` (from batch 125) — now explained by the
  named-pointer rule above rather than left open.

## Still owed

- 12 not-yet-elevated `.s` TUs sit inside Makefile wildcards.
- ~3,300 lines of duplicate Makefile rule blocks producing "overriding recipe"
  warnings on every build.
- 266 parks. Batch 125 closed one outright when its twin was solved; nothing
  systematically re-checks parks against new results, and several were written
  under reasoning that batches 123–126 have changed.
