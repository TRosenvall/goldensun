# Batch 125 — searching against what is already solved

Verified on a clean `make clean && make compare` (with the host-side agbcc
recovery) — `goldensun.gba: OK` — and every address below read out of the linked
ELF.

## Elevated (2258 → 2246)

| function | address | found by |
|---|---|---|
| `OvlFunc_924_200d900` | 0x0200d900 | twin of `OvlFunc_923_200a370` |
| `OvlFunc_956_2008658` | 0x02008658 | integer-local address class |
| `OvlFunc_968_2009048` | 0x02009048 | twin of `OvlFunc_968_20090cc` |
| `OvlFunc_959_200a38c` | 0x0200a38c | twin of `OvlFunc_959_200a308` |
| `Func_8020a60` | 0x08020a60 | twin of `Func_80a2268` — **closes a park** |
| `OvlFunc_common0_18` | 0x02008048 | fifth copy of the `__CreateActor` wrapper |
| `OvlFunc_968_2008b08` | 0x02008b08 | twin of `OvlFunc_964_20091e0` |
| `OvlFunc_913_200aad8` | 0x0200aad8 | twin of `OvlFunc_911_2008800` |

Seven of the eight are twins of functions already in the tree. Six of those were
found by a tool that did not exist at the start of the batch.

## `tools/solved_twins.py`

`OvlFunc_924_200d900` turned out to be `OvlFunc_923_200a370` — elevated the
round before — with one call target changed. One `sed`, one screen. I found it
by accident, and that was the problem: `twin_families.py` groups the REMAINING
functions against each other, so the first member of every family still has to be
solved the hard way. Nothing was searching the remaining functions against the
ones already matched.

That search reports **11 hits**, and independently rediscovered the accident.
Six were elevated here; five remain queued.

Three things it established:

- **A twin inherits its template's flag group, not just its shape.**
  `OvlFunc_959_200a38c` is its template with four immediates changed: 41
  differing at `-O2`, exact with `-fno-rerun-cse-after-loop`. Two explicit
  Makefile rules were added — that one, and `OvlFunc_968_2009048`, which falls
  in the same mis-scoped `ovl_30_c_a_c_a_c_a%` `-O1` wildcard already documented
  for its own template. That clears part of the standing wildcard debt.
- **A park is only blocked until something that looks like it gets solved, and
  nothing was re-checking.** `src/non_matching/rom_15000/8020a60.c` recorded
  `Func_8020a60` together with its byte-identical twin `Func_80a2268`. The twin
  was elevated later; the park was never revisited. It is the twin's source with
  the name changed, and the park is now deleted.
- The corpus is a **build artefact** — `asm/<path>/X.s` is gcc's output wherever
  `src/<path>/X.c` exists — so it is only as current as the last build, and it
  grows every round. This search gets cheaper and more productive as the tree
  fills in.

### The zero-result guard, third instance of the same failure

The first run reported **0 solved out of 3,095 files**, which without the guard
reads exactly like "there are no twins". gcc emits `.thumb_func`, then
`.type NAME,function`, then `NAME:` — and the parser wanted the directive on the
immediately preceding line.

That is the same class of failure as the two-operand `add` in batch 123 and the
`.thumb_func_start` mismatch recorded in `twin_families.py`. The tool now refuses
to report when either corpus is empty. On this codebase an empty corpus has never
once been the real answer.

## A rule of mine, corrected

Batch 124 recorded that the named-pointer lever "needs the offset to be mutated
after the pointer is taken". `OvlFunc_881_200808c` (parked, 28 lines against 29)
mutates the offset afterwards and gcc folds the address into a reg+reg access
anyway — while the **identical construct** in `OvlFunc_881_20084a0`, elevated one
round earlier, produced the ROM's separate `add`.

So the precondition is necessary, not sufficient: whether gcc materialises an
address or uses the addressing mode is decided by register pressure, and the
source can only ask indirectly. `OvlFunc_899_20099a4` (also parked) shows the
mechanism from the other side — its store materialises the address exactly as the
ROM does, and the asm shows why, since the offset register is then reused for the
stored value.

## Parked

- `OvlFunc_881_200808c` + `20080d4` — a twin pair, one solution covers both.
  28 of 29 lines, the address-fold above.
- `OvlFunc_899_20099a4` — 26 of 26 lines, 5 differing, all the position of one
  `mov r0, #0` in argument setup. The basic-block lever needs a dominating block
  and this is straight-line.

## Still owed

- 12 not-yet-elevated `.s` TUs sit inside Makefile wildcards and will silently
  inherit a flag when elevated.
- ~3,300 lines of exact-duplicate Makefile rule blocks; the build prints
  "overriding recipe" warnings for them on every run. Behaviour is unchanged, but
  the noise would hide a real warning.
- 264 parks. Several were written under reasoning that batches 123–125 have since
  changed; a re-screen sweep is owed. `Func_8020a60` fell to a one-line `sed`.
