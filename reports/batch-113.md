# Batch 113 — agent 4 wired, and a Makefile target that silently did nothing

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of its overlay's linked ELF.

| Function | Address | Overlay | Flags |
|---|---|---|---|
| `OvlFunc_939_2008764` | `02008764` | ovl_7c460c | — |
| `OvlFunc_952_200849c` | `0200849c` | ovl_7d768c | CSE |
| `OvlFunc_932_20089ec` | `020089ec` | ovl_7b9cb4 | CSE |
| `OvlFunc_932_2008a94` | `02008a94` | ovl_7b9cb4 | CSE |
| `OvlFunc_968_2008f38` | `02008f38` | ovl_7f2f14 | — |
| `OvlFunc_968_20098f8` | `020098f8` | ovl_7f2f14 | explicit `-O2` |
| `OvlFunc_891_200995c` | `0200995c` | ovl_78c76c | — |
| `OvlFunc_965_200a4d0` | `0200a4d0` | ovl_7ef4f4 | CSE |
| `OvlFunc_965_200a548` | `0200a548` | ovl_7ef4f4 | CSE |
| `OvlFunc_965_200a738` | `0200a738` | ovl_7ef4f4 | — |

**10 elevated, 2440 remaining.** All from parallel agent 4, all re-screened here
before wiring.

## A Makefile rule that silently did nothing

Six of these needed per-file flag rules. I generated the targets from paths
produced by `grep -rln '...' asm/` — which, because the search path ends in a
slash, prints `asm//overlays/...` with a **double slash**.

POSIX collapses `//` when opening a file, so the `.c` files were written to the
right place and every screen passed. **make does not collapse it.** It treated
`asm//overlays/X.o` as a different target from `asm/overlays/X.o`, so all six
explicit rules were dead and two files silently kept building at `-O1` from a
wildcard rule.

The symptom was the confusing part: **every overlay compared clean and the ROM
SHA1 failed.** That sends you looking at the main ROM, where nothing had
changed. It took a `git stash` of the working tree to prove HEAD was green and
narrow it back to the six new lines.

**Normalise paths before putting them in a makefile**, and when a target's rule
seems not to apply, check the target string character by character before
suspecting the recipe.

## Two mis-scoped `-O1` wildcards, and how to tell

`tools/tryc.py` reports the flag group it inherited (`built with: O1`). Agent 4
hit five such warnings and **two of the five wildcards were wrong for the file
in question** — at `-O1` those functions are 20 and 43 differing; at `-O2` they
are exact.

The cost of checking is one screen, and the fix does not touch the wildcard: an
explicit rule beats a pattern rule in make, so the wildcard is left alone for
the files it is right about.

## Three levers from the agent's work

**The initialiser-order rule is not universal.** `docs/elevation.md` says two
initialisers come out in the opposite order to their assignments. On
`OvlFunc_968_2008f38` the order is **preserved** — `n = 0xa; d = 8;` emits
`mov r5, #8 / mov r6, #0xa`. So the rule to apply is *swap and re-screen*, not
*assume inversion*.

**Two textually identical statements can need two different spellings.** In
`OvlFunc_939_2008764` the same "read the iwram pointer, add `0xec << 1`,
increment the halfword" appears twice, and the ROM emits the offset constant
first in one copy and the pointer first in the other — a clean r2/r3 exchange.
Writing both as the walk form gives the second copy's allocation for both;
writing the first as **offset-first inside one expression** and the second as
the walk is exact. That is operand order *within* one expression deciding
register assignment.

**One levered local per site, and the flag does not always reach it.**
`OvlFunc_891_200995c` passes two constants to two call sites and needs four
locals. `-fno-rerun-cse-after-loop` leaves it at 44 differing with literals —
the expand-time hoist row of the batch-111 table rather than the CSE row.

## The label sweep: a clean negative

Batch 112 found a park whose 25-of-50 screen was entirely a redundant-label
cascade. `tools/label_false_negatives.py` now screens every park and reports
those whose first differing line is a label definition.

**All 228 parks screened; none open on a label.** `OvlFunc_898_2008a4c` was the
only one. That closes the question rather than leaving it as a suspicion, and
the tool stays for future parks.
