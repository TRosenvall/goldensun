# Batch 247 — eviction pins must be added as a set; the `goto`-loop rule runs backwards

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_945_200e110` | `0x0200e110` | [ovl_30_…_a_c_c_b.c](src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_a_c_c_b.c) |
| 2 | `OvlFunc_928_20089dc` | `0x020089dc` | [ovl_314_c_c_a_c_c_a_c_c.c](src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_c_c.c) |
| 3 | `OvlFunc_968_200c610` | `0x0200c610` | [ovl_30_c_c_c_c_a.c](src/overlays/rom_7f2f14/ovl_30_c_c_c_c_a.c) |
| 4 | `OvlFunc_968_200c7c0` | `0x0200c7c0` | [ovl_30_c_c_c_c_a.c](src/overlays/rom_7f2f14/ovl_30_c_c_c_c_a.c) |
| 5 | `OvlFunc_968_200c968` | `0x0200c968` | [ovl_30_c_c_c_c_a.c](src/overlays/rom_7f2f14/ovl_30_c_c_c_c_a.c) |

## Retired parks

| function | was parked on | broken by |
|---|---|---|
| `OvlFunc_968_200c968` | SCRATCH-REGISTER SELECTION, 22 of 90 | an early-return rewrite **and** `volatile`, neither sufficient alone |

Entries 3–5 are the whole of `asm/overlays/rom_7f2f14/ovl_30_c_c_c_c_a.s`, so
that file lands **whole**: one `.c`, no split, no linker edit, no flag rule.
`overlay.ld:94` and `:102` already name the `.o`'s `.text` and `.data`.
Entry 1 is second-of-two and needed a split with **both** linker groups
expanded; entry 2 is one-function-per-`.s`.

Functions 1 and 2 are the fifth and sixth consecutive hiv=4 matches, closing the
experiment opened in batch 246 at **six for six**.

## EVICTION PINS COMPETE FOR THE SAME REGISTER

This is the finding that would have cost a function, and it is the reason the
batch is worth reading.

On `20089dc` the plain first transcription was **68 of 320 at exactly the ROM's
size**, with all 23 non-boilerplate high-register references already correct.
Two rebuilt constants needed evicting. Measured:

| pins | differing |
|---|---|
| none | 68, size and count already right |
| one rebuilt constant pinned | 171, **four bytes short** |
| the other pair pinned | 184, **four bytes short** |
| **both** | **4** |

The mechanism is visible in both failure diffs: with only one pinned, **the other
constant moves into the register the first pin just freed**, and the value the
ROM holds is pooled again.

> **EVICTION PINS MUST BE ADDED AS A SET.** Each alone is strictly worse than
> none. A greedy one-at-a-time *add* pass rejects both and reports the function
> blocked at 68 — a false park on a function four encodings from exact.

The docs carry the removal-side rule (*"re-verify survivors AS A SET"*) and
nothing on the addition side. **The asymmetry matters more than it looks: a
wrong removal costs a worse candidate, a wrong addition costs the whole
function.**

## THE HOLE TEST IS ABOUT REMATERIALISABILITY, NOT ABOUT THE COPY

The recorded rule says a `mov rLOW, rHIGH` argument is a hole in the pin set by
construction. On `200e110`, twenty-two sites take their actor argument that way
and **ten of them are pinned in the final set** — because a `bl` result cannot be
rematerialised, so the copy is forced and the pin costs nothing.

Applied only to **rebuildable** values the test is exact: 21 nominated, one hole,
20 pins, four differing at exact size with identical relocations **on the first
screen**. Pinning that one hole is the confirmation — its tenant vanishes and the
**push mask itself** becomes the first differing encoding.

## A THIRD ARM FOR THE `-ffixed-r7` DISCRIMINATOR

The entry records *"same count, rotated → the flag; one more → a live range"*.
`200e110` is same count, rotated, **and the flag is wrong**: r7 is one of the
seven registers the ROM spends, so reserving it leaves six (230 differing).

> Same count plus rotated implies only that **membership** is wrong. Check for a
> CSE-class difference first — it is cheap, and it was the answer here.

Second false-positive strike on that entry, failing for a different reason than
the first.

## THE `goto`-LOOP RULE IS NOT SOUND IN THE REVERSE DIRECTION

The recorded rule — *"if the ROM rebuilds a loop-invariant constant inside the
loop body, the source's loop was not a `while`/`for`"* — **cost this round its
first three candidates.**

In `200c610` the ROM reloads `0x4ccc` and `0x17ffc` from the pool at both sites
inside the outer loop, and the source's loops are `do { } while`. The `goto`
spelling **created** the hoist rather than removing it. With no loop notes,
`gcse` hoists `&t` early enough that `cse2` rewrites every field store to
`mov r2, r8 / str r3, [r2, #8]` where the ROM has `str r3, [sp, #0x18]` — which
also makes `&t` the hottest call-crossing value and hands it r8 instead of the
ROM's r11. With real loops, `loop.c` hoists into the pre-header **after** `cse1`
has already emitted the sp-relative stores, and both defects vanish together.

Worth **38 differing → 17 in one step**, the single biggest lever in the file.

> **Rebuilt loop-invariant constants NOMINATE the `goto` rewrite; they do not
> decide it. Screen both spellings — one screen each.**

The existing counter-example table ranks by *what gcc transformed*. This is a
third case, failing for a different reason: not that the rewrite's overhead
exceeds what it recovers, but that **`goto` mode introduces hoists real-loop mode
never had**.

## `200c968`: A PARK BROKEN BY TWO LEVERS, NEITHER SUFFICIENT ALONE

1. Early `return 0` guard rewritten as `if (v == 0) { ... } return 0;` — 22 → 18.
2. **`volatile` on the re-read global.** The ROM holds `&iwram_3001e40` in r2 and
   loads through it *twice*. The park's own recorded lever — naming the address —
   **cannot** reach that: a symbol address is a link-time constant, so the
   pointer folds and gcc still CSEs the load. Re-measured, byte-identical to the
   park, confirming the park's note. `volatile` emits both loads, and the second
   load's live range is what hands r7 to the flag and r10 to the struct pointer.

Both `extern volatile unsigned int` and a use-site cast match byte-for-byte; the
shipped file uses the declaration.

**And the park read as pure allocation because two defects cancelled.** Its
"90 lines against the ROM's 90" was a spurious early `mov r0, #0` paying for the
missing second `ldr` — a second instance of the recorded cancelling-length rule.

> A candidate matching the ROM's instruction count is not evidence on its own.
> Trust the differing-encoding count and the first-diff position, not the length.

## WHERE THE EVICTION RECIPE DID *NOT* APPLY

Worth recording, because a recipe with no stated boundary is a superstition.
In the `200c610` family the **push mask was never wrong** — from the first
candidate we spent exactly the ROM's set — so the you-have-more-registers
commoning tell never fired. The r0–r2 pin *was* still needed, but at the `-1`
triple and for the recorded `cse1` reason (three identical two-instruction
constants in one basic block), worth 6 → 0. It moved no high-register tenant.

The boilerplate caveat is confirmed exactly: 4 push + 4 pop = **8** of each
function's high-register references are prologue/epilogue noise.

## OTHER BOUNDED ENTRIES

- **The recorded HImode cure inverts when the halfword constant is shared with an
  SImode use.** In `200e110` the ROM *wants* the pooled word — it is also a call
  argument living in a high register — so the signed spelling is exact while the
  `unsigned short` cure splits one value into two: 284 differing, twelve bytes
  long.
- **Two ordering pins in one function with different widths *and* different fill
  directions**, so "fill direction does not transfer" now holds *within* a single
  function, not just between twins.
- `200e110`'s width minimum is a **non-prefix register subset**, which the
  prefix-width search a sibling used would have missed.

## `200c7c0` came nearly free

It is `200c610` with six constants changed and the `StartTask` moved out of the
`j == 3` arm. It matched on the **first** screen from the template — the
solved-sibling heuristic at its cheapest, and the reason taking whole `.s` files
is worth preferring.

## Lead, not solved

`OvlFunc_968_200c2bc` (`ovl_30_c_c_a_c_c.s`, a different file, so no
file-completion payoff) was time-boxed at **265 lines against 275, 205
differing, first diff at 34**. Two things separate it from its siblings, each
needing a round of its own: a rotated `while (i <= 7 && k <= 3)` whose
initialisers gcc must sink below the guard, and a second outer loop that copies
the struct pointer into a *second* high register (`mov r10, r9`) and stores
fields through it while the first loop stores sp-relative — **the same address in
two roles in one function**. The split-condition `do`+`break` variant is worse
(270 lines, 224 differing, frame grown to `0x3c` by a spill).

## Gate

`make clean && make -j8 && make compare` green. Every address above checked
against the linked ELF with `tools/checkaddr.py`.
