# Batch 160

Five elevated. Verified after a clean `make clean && make compare`; SHA1
`5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked images.

| function | address | image |
| --- | --- | --- |
| OvlFunc_916_2008b3c | 0x02008b3c | overlays/rom_7a37f0/overlay.elf |
| OvlFunc_916_2008be4 | 0x02008be4 | overlays/rom_7a37f0/overlay.elf |
| Func_80b86ec | 0x080b86ec | goldensun.elf |
| OvlFunc_881_200808c | 0x0200808c | overlays/rom_77a7c8/overlay.elf |
| UpdatePoison | 0x0808c3a4 | goldensun.elf |

## The headline is a tooling fault, not a lever

`pickable.parked()` was blind to **210 of 467 park files**. Four header
conventions had been bolted onto its regex one at a time, each after a parked
function was re-offered as a candidate, and batch 159 recorded the fourth with
the problem described as closed. Auditing every park file at once -- rather than
reacting to one miss -- showed the regex recognising 257 of 467. The tree also
uses `NAME [path]`, `NAME -- NON-MATCHING.`, `NAME -- NOT MATCHING`,
`NAME [path] -- 0xaddr`, `NAME and NAME2 [path]`, files whose first line is a
bare `/*`, and class write-ups that name no function at all.

The cost is not hypothetical. The selection filter returned 21 candidates before
the fix and 10 after, so **roughly half of every candidate list, for an unknown
number of rounds, was work already done**. This never showed up because a
re-offered function looks exactly like a fresh one -- it was caught only because
a candidate's residue turned out to be quoted verbatim in docs/elevation.md as a
previously measured attempt.

The fix is to stop keying on the header format. `parked()` now builds the
universe of real function names from the .s corpus and looks for any of them in
each park file's leading comment, plus any C function the file defines. A sixth
convention cannot break it. 687 entries to 1143; the audit now reports zero
unrecognised files.

**Rule: when a filter is patched repeatedly for the same class of miss, measure
its coverage instead of adding another case.** One audit answered what four
patches did not.

## solved_twins.py is the cheapest route and had not been run for rounds

Two of the five came from `tools/solved_twins.py`, which reported exactly two
remaining functions with a solved twin. Both matched.

`Func_80b86ec` is byte-for-byte `Func_80b9acc` -- same bases, same 0x36
halfword, same key masks. The template C compiled against it with nothing
changed but the name: OK on the first screen, stable across three.
`OvlFunc_881_200808c` is `OvlFunc_881_20080d4` with two immediates changed. Two
seds and one screen.

Its single differing instruction was `bl __divsi3` against the ROM's
`bl _divsi3_RAM`, which is the documented overlay divide-alias false negative --
`overlays/rom_77a7c8/overlay.ld` already carries `__divsi3 = _divsi3_RAM`. The
screen cannot see a linker alias. Confirming a recorded false negative is worth
as much as finding a new one; the alternative was writing a park for a function
that already matched.

## UpdatePoison: two levers that look like alternatives are not

79 instructions, matched after a `switch` (a three-way dispatch compares both
cases before either body -- the decision-tree form, not `if / else if`) and the
gState offset being built rather than folded. That took it from 78 differing to
17.

The remaining 17 read as two separate problems: a register-role swap, `worst`
and the array pointer exchanged, and the base materialising ABOVE the loop guard
where the ROM has it below. They were one cause. A local assigned before the
loop is born before the guard, which lengthens its live range, and by
`global.c`'s priority formula a longer range loses the earlier register.

Indexing the array directly and deleting the local -- `gState[(0xfc << 1) + i]`
-- matched outright. The index expression carries `i`, so there is nothing for
gcc to fold into `ldr =gState+504`, and the base is now born in the preheader
below the guard.

So the local-pointer lever and plain indexing are **not alternatives to choose
between**. Where the offset varies with an induction variable, indexing already
prevents the fold and the local is pure cost. Reach for the local only when the
offset is constant. Two negatives from the same function: assigning the base
inside the loop body is much worse (79 lines to 87, gcc reloads it every
iteration), and declaration order is inert, three permutations byte-identical.

## Correction carried from the preceding rounds

The "duplicate-constant CSE into a callee-saved register" class written up two
commits earlier is **not new**. It is the dominance rule already in
docs/elevation.md: uses in mutually exclusive branches rebuild, a use that
dominates another hoists. What the two new parks add is that expense does not
enter the decision -- `OvlFunc_903_200843c` hoists a constant whose every use is
a single pool load, where the hoist saves nothing and costs four instructions.
Repetition plus dominance is the whole trigger.

## Also this cycle

`tools/filtered.py` now exists: the selection filter that had been described in
the docs, used once, and never committed, so every round re-derived it by hand.

Six functions were parked across the preceding rounds -- `Func_800bfa4` at 12 of
41 and `Func_80175c0` at 18 of 43, both at exact length with only register
assignment left, plus the eight-function `gFlags` accessor family, where the
whole family turns on one scheduling decision: the base address is materialised
before the index shifts, taking the register the ROM uses as the shift temp,
which is what degrades the shift to its destructive two-operand form. `GetFlag`
alone has 180 call sites.

Those parks also pinned the precondition on the naming lever, which had been
stated without one: **the named value must not be foldable.** `Func_80175c0`
named a runtime expression, it survived, 26 to 18. `Func_801d94c` named a
constant and the gFlags family named a symbol address; both folded away to no
effect. A link-time address counts as foldable.
