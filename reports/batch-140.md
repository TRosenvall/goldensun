# Batch 140 — a control-flow shape moved register allocation

Verified on a clean `make clean && make compare` — `goldensun.gba` SHA1
`5c4695205413df7db52b9a184815a07783999971`, all 96 overlays comparing — with
every address below read out of the linked ELFs.

**remaining 2214 · elevated 3203 · parked 324**

## Elevated (5)

| function | address | ELF | what it took |
|---|---|---|---|
| `Func_80cd4b4` | `080cd4b4` | goldensun | first screen; counter pointer named once |
| `OvlFunc_947_200a5f8` | `0200a5f8` | rom_7d0e88 | first screen; `actor.h` throughout |
| `OvlFunc_928_2008370` | `02008370` | rom_7b6668 | first screen; frame size gave the struct size |
| `OvlFunc_921_2009704` | `02009704` | rom_7a7298 | first screen; `actor.h` throughout |
| `OvlFunc_968_2008098` | `02008098` | rom_7f2f14 | moving `return 0` to the tail |

## The finding: an early guard is not the same shape as a tail return

`OvlFunc_968_2008098` screened at **26 of 36** written the obvious way, and
**matched exactly** with the same logic rearranged:

```c
/* 26 of 36 */                    /* exact */
n = CreateActor(...);             n = CreateActor(...);
if (n == 0)                       if (n != 0) {
    return 0;                         ...work...
...work...                            return n;
return n;                         }
                                  return 0;
```

Two differences collapsed together:

* gcc materialises the early return's zero **above** the comparison that
  selects it — `mov r0, #0 / cmp r5, #0 / beq` against the ROM's `cmp / beq`.
* a redundant `mov r0, r5` before the inner call also disappeared.

The second is the one that matters. That copy is textbook **"elided copy"**,
one of the three shapes HANDOFF.md's register-pressure section lists, and that
section concludes such differences follow from pressure in the original
translation unit rather than from how the C is written. Here it followed from
the control-flow shape after all.

**So a difference that resembles a known pressure-residue shape is not
automatically pressure residue.** Check the control-flow shape first — it is
cheap and it moves register allocation. The ROM's layout is the hint: a single
exit, the zero assigned in an else arm, one `b` to a shared epilogue, means the
source had one exit and not two.

## `actor.h` is now carrying whole functions

Three of the five matched on the first screen with no pointer arithmetic at
all — `interactFlag`, `interactFlags`, `goalFacing`, `flags`, `sprite`, `pos`,
`rotX`, `rotY` read straight from the header. The recurring detail worth
repeating is that a negated mask needs an `int` intermediate (`m = -0xd;`)
or gcc truncates it to a byte constant, which `docs/elevation.md` already
records and which came up in three of these functions.

## Parked (3)

`OvlFunc_957_2008f10` — register-pressure residue, 18 of 43 with the **body
exact**; every difference is prologue and epilogue, where the ROM saves one
callee-saved register more than gcc needs. Getting there from 43 differing was
the useful part: written the obvious way the stream is **nine lines short**,
because gcc materialises two constants late and never reaches for `r8`-`r11` at
all. Naming them before the call so four values are live across it is what
creates the demand. Stream length was the tell — nine missing lines meant a
missing register class, not a missing instruction.

`Func_80b0840` — a new class: **the macro's pinned register is already
correct**. Two DMA3 transfers share a source; `DMA3_SET` pins it to `r0` and
its asm clobbers `"memory"` but not `r0`, so gcc emits nothing for the second
and our stream is one instruction *short*. Not the same as the double-DMA
functions the tree already matches — `rom_7a4370/ovl_30_c_c_c_c_c_c_b.c` is
byte-exact because its two calls have **different** sources. Recorded as
deliberately not tried: adding `"r0"` to the clobber list would force the
reload but risks every other `DMA3_SET` user.

`OvlFunc_945_20082f4` — 3 of 36, address computation scheduled one slot late.
Solved on the way: the sprite must be **loaded before** `interactFlags` is
stored, which is pure source order and worth 5 differing down to 3.

## Corrections

`src/non_matching/overlays/20082f0.c` claimed the twin pair is `-O1`. That is
confirmed as far as it goes — 86 against 86, 15 differing, reproduced — but at
least one of the residue is argument precompute, which is diagnosed and not
reachable from C, and swapping the two loads makes it 18. So closing that
function **cannot settle the -O1 question**, and no O1 rule was added on the
strength of a line count that still leaves 15 differing, in a directory whose
only explicit rule is CSE.

`OvlFunc_968_200af30` was skipped without screening: its argument lists mix
cheap constants with shifted ones and a cheap one is not last, which is
HANDOFF.md's predictive rule for argument precompute. Using the rule to avoid a
screen is the point of having it.
