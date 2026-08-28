# Batch 135 — one lever carried the batch, and two documented rules were wrong

Verified on a clean `make clean && make compare` — `goldensun.gba` SHA1
`5c4695205413df7db52b9a184815a07783999971` — with every address read out of the
linked overlay ELFs. The clean rebuild regenerated every committed `.s`
intermediate byte-identically (`git status` empty afterwards), which checks the
whole elevated corpus rather than just this batch.

Counts are now MEASURED by `tools/remaining.py` rather than carried forward; see
"the count was never a measurement" below.

**remaining 2243 · elevated 3176 · parked 309**

## Elevated (5)

| function | address | ELF | what it took |
|---|---|---|---|
| `OvlFunc_932_200a9dc` | 0x0200a9dc | rom_7b9cb4 | recovered from a park declared unreachable |
| `OvlFunc_959_200cbfc` | 0x0200cbfc | rom_7e7574 | CSE_CFLAGS + message symbols + no-prototype |
| `OvlFunc_948_200a188` | 0x0200a188 | rom_7d30e0 | four guarded interleave sites at once |
| `OvlFunc_954_20095e0` | 0x020095e0 | rom_7db0c8 | both argument-order levers, opposite directions |
| `OvlFunc_883_20090d8` | 0x020090d8 | rom_780898 | both directions again, five lines on first screen |

`_MSG_242e` and `_MSG_2430` were added to `message.sym` for the second of those
and resolve as absolute symbols in the overlay ELF.

## The interleave lever moves the argument you do NOT name

`OvlFunc_932_200a9dc` had been parked with "NEXT: nothing. This is the documented
limit of the lever rather than a new shape." It was two lines out and the lever
did reach it.

The park had tried naming the interleaved argument itself — the `mov r0, #9` that
needs to move — and correctly found the slot is used on both sides of the `if`,
so naming it breaks the third clause. That reasoning is sound and irrelevant.
**Name the other arguments.** Naming the two split builds and leaving the slot a
bare literal is exact.

A park that concludes "the interleaved argument cannot be named" has checked the
wrong half of the call.

## Which interleave parks are worth re-attacking

Of 48 interleave-class parks with a live `.s`, only **9** have a guarded site —
a branch BEFORE the interleave, which is what gives the lever a block to name in.
The filter is `pool.py`'s `site > 0 AND unguarded == 0`, **not** "the function
contains a branch": `OvlFunc_967_2008308` has one and is still unreachable
because its site precedes it.

That filter has now picked four functions correctly. Working the guarded queue to
exhaustion found that outside the one recovery, every member had its interleave
already solved and was blocked by something else — so the class is close to
worked out, and what those parks are really waiting on is the register-role swap
(21 parks, no lever).

## "r0 in the middle" and "r0 at the end" are different problems

`OvlFunc_954_20095e0` had eight argument-order differences across five sites
needing OPPOSITE fixes in one function:

    ... / mov r0 / lsl r1 / ...     r0 INSIDE a split build -> name the others
    ... / mov r1 / mov r2 / mov r0  r0 LAST -> drop the prototype

Reaching for the wrong one looks exactly like the right one failing. Read where
r0 sits at each site. `OvlFunc_883_20090d8` then wanted both directions again and
went from 5 differing to exact once each site got its own treatment.

## The division helper names the signedness

gcc emits `__divsi3`/`__modsi3` for signed and `__udivsi3`/`__umodsi3` for
unsigned, so which one the ROM calls says directly what the operand types were.
On `OvlFunc_943_2009684` a `__Random()` declared `int` produced `__modsi3` where
the ROM calls the unsigned helper; changing one declaration took the screen from
**55 differing to 17**.

Corollary that prevents miscounting: in overlays `bl __umodsi3` and the ROM's
`bl _umodsi3_RAM` are the same symbol, via the alias in `overlay.ld`. A screen
showing only that pair is showing no difference at all.

## Two documented rules turned out to be wrong

**The commoned-constant tell does not promise a fix.** `docs/elevation.md` said
it yields either to `CSE_CFLAGS` or to separate named locals, and to try both.
`OvlFunc_881_2009c08` and `OvlFunc_939_2008eb0` are two independent
counterexamples where both fail, along with every other flag group. Reworded to
"try both, expect neither". The guess about branch-vs-block that the docs offered
alongside it is contradicted by the first of those and is now marked unknown.

**The no-prototype lever does not always trade one site for another.** Batch 133
said it fails when a callee's sites disagree about argument order.
`OvlFunc_959_200cbfc` is exactly that shape — three calls, inconsistent ordering —
and dropping the prototype fixed the wrong site while leaving the other two
correct. I expected it to trade and screened it anyway, which is the only reason
it was found.

## The count was never a measurement

Batches 130-133 printed 2224 → 2219 → 2214 → 2208 → 2202, and every step equals
that batch's elevation count exactly: a hand-decremented counter, not a
measurement, so its baseline error could never correct itself. It had drifted 46
low. Counting four ways — raw and distinct, with and without excluding elevated
TUs — all give the same number, so there was no definitional ambiguity either.

`tools/remaining.py` measures it. A figure only ever derived from the previous
figure is not evidence.

## Tooling: the park exclusion was leaking two thirds of the corpus

`pool.py` excluded parked functions by matching their header, and park headers use
FIVE formats accumulated over the project. It matched one. **169 of 253 parked
functions were being offered back as fresh candidates** — `OvlFunc_881_2009c08`
was parked on the 27th and investigated from scratch on the 28th, five flag
groups and both remedies, because the pool served it up again.

Fixed with two passes (strict scan plus header scan); leak is zero with an
over-match control. The same root cause had produced five duplicate parks, now
merged into their canonical files.

## A lead killed cheaply

Batch 134 flagged the cluster experiment for mid-function literal pools as the
most promising open question. It is refuted. Compiling a function alone and again
with a second function appended puts the pool in exactly the same place, after
the epilogue, with no branch — so early pool dumping is old_agbcc's own emission
and not a translation-unit property. That cost one compile instead of three
function transcriptions.

The method generalises: when the hypothesis is about compiler behaviour rather
than a particular function, build the smallest input that would show it.

## Parked this batch (12 functions across 9 entries)

`OvlFunc_881_2009c08`, `Func_8094428`, `OvlFunc_932_200a5c0`, the
`Func_80f2ebc`/`Func_809088c` twins, the `OvlFunc_949_20082f0`/`20083d0` twins,
`OvlFunc_959_200c794`, `OvlFunc_939_2008eb0`, the `OvlFunc_943_20090a0`/`20091c8`
twins, and `OvlFunc_943_2009684`. Several are two lines out; the recurring wall
is the register-role swap, which 21 parks now describe and nothing reaches.
