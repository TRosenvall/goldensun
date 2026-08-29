# Batch 111 — four parallel agents, and the blocker measured to the end

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of its overlay's linked ELF.

| Function | Address | Overlay | Source |
|---|---|---|---|
| `OvlFunc_936_20098a4` | `020098a4` | ovl_7c097c | coordinator |
| `OvlFunc_896_200c328` | `0200c328` | ovl_78ef88 | coordinator |
| `OvlFunc_911_200a910` | `0200a910` | ovl_79e5c0 | coordinator |
| `OvlFunc_945_2009804` | `02009804` | ovl_7cb2c0 | coordinator |
| `OvlFunc_943_20093d4` | `020093d4` | ovl_7c7b9c | agent 1 |
| `OvlFunc_958_2009158` | `02009158` | ovl_7e636c | agent 1 |
| `OvlFunc_882_2009600` | `02009600` | ovl_77dd1c | agent 1 |
| `OvlFunc_879_20081c0` | `020081c0` | ovl_779188 | agent 1 |
| `OvlFunc_926_200a6d8` | `0200a6d8` | ovl_7b2078 | agent 1 |
| `OvlFunc_926_200c140` | `0200c140` | ovl_7b2078 | agent 1 |
| `OvlFunc_958_2008fd0` | `02008fd0` | ovl_7e636c | agent 1, **unparks** |
| `OvlFunc_971_200906c` | `0200906c` | ovl_7fb4a8 | agent 1, **unparks** |

**12 elevated, 2463 remaining.** Two existing parks retired.

## The parallel experiment

Four agents ran concurrently on disjoint 14-function worklists drawn from
separate overlays. The division of labour was set by where the time goes: a
`tryc.py` screen costs 2–4 seconds and a `make compare` costs 28, but a batch
takes ~23 minutes — so **the bottleneck is reading assembly, and that is what
parallelises.**

Agents were given a written brief (`scratch/AGENT_BRIEF.md`) and were forbidden
to run `make`, `split_s.py`, or any writing `git` command, or to touch anything
under `asm/`, `src/`, `overlays/`, the `Makefile`, or any `.ld`/`.sym` file.
They write only inside their own `scratch/agentN/`. **Every mutation to the tree
goes through the coordinator, and every agent result is re-screened here before
wiring** — `tryc.py` printing OK in an agent's transcript is not evidence until
it prints OK in mine.

That held. All eight of agent 1's OK results reproduced independently.

Three of the four have reported: **agent 1 gave 7 OK + 2 blocked only on a
`.sym` line + 5 measured parks; agent 2 gave 8 OK + 2 `.sym` + 4 parks; agent 4
gave 10 OK + 4 parks.** Agent 1's are wired here; agents 2 and 4 are the next
round's work rather than being swallowed in one turn.

## The symbol-address technique generalises, and it retired two parks

Both unparked functions had the right observation written down and stopped one
step short.

`src/non_matching/ovl_7e636c/2008fd0.c` recorded: *"a named `int base` does not
change it — gcc constant-folds `base + 8`."* That is the clue. **An int constant
folds; a symbol address does not.** `base = (int)&_MSG_23cc;` cannot be folded,
so gcc holds the symbol in a callee-saved register and reaches the second id
with `mov r0, r5 / add r0, #8` — the ROM's exact sequence, push list included.

`src/non_matching/overlays/200906c.c` recorded that three nearby ids get derived
from one pool load where the ROM pools each separately. Symbol addresses cannot
be reached from one another by an immediate, so each switch arm gets its own
load.

The doc had this technique for the *subtraction* case only. It generalises:
**wherever the ROM shows gcc failing to relate two constants it obviously
could, try making them symbols.** Four ids were added to `message.sym`, named by
value like the 68 already in that block, in their own commit.

## The constant-CSE blocker, measured to the end

Last round I described the basic-block lever as the fix for pool-constant CSE.
That was wrong and it is now corrected with probes:

| between the two uses | flags | result |
|---|---|---|
| nothing — straight line | anything tried | hoisted |
| a branch | default | hoisted |
| a branch | `-fno-rerun-cse-after-loop` | **reloaded** |
| mutually exclusive arms | any | reloaded |
| two DISTINCT symbols of equal value | any | reloaded |

**Neither half alone works**, which is why batch 106's "try the flag first"
succeeded on one function and did nothing on another. Confirmed against a
matched file that does it: `src/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_b.c`
reloads `0x201` across two early returns under `CSE_CFLAGS`.

I first concluded from 34 matched files that "it's solved", then checked them
and found inline-asm fakematches, `dma.h` includes, and non-dominating cases.
The correction is in the doc.

`tools/blocked_cse.py` now counts the shape, including two-instruction builds
(`mov`+`lsl`, `mov`+`neg`) — my first pass counted only pool loads and missed
`OvlFunc_881_2009888`, which is blocked on a `-1` the ROM builds three times.

**Upper bound 630 functions, 49% of the remaining instruction mass. Certain core
(no labels at all, so no boundary obtainable) 98 functions, 2%.** In the 40–140
band it is 17% — which is why recent batches have been productive: they have
been working the part that is not blocked.

## Four levers from the round

**Each stack-argument site needs its own pair of locals.** Naming both values
adjacent to the call is the documented lever and is not sufficient when several
calls do it: one pair reused across three sites lands in r2 where the ROM has
r3, every time. `OvlFunc_936_20098a4` and `OvlFunc_911_200a910` both need this.
In the same function, a value the ROM *shares* (`mov r5, #2` across two calls)
still wants one local — carried and rebuilt on adjacent arguments of one call.

**Un-rotated loops need `goto`.** `for(;;){A; if(c) break; B;}` gets rotated by
gcc and comes out 25 differing of 58 on `OvlFunc_896_200c328`; two labels and
three gotos is exact.

**Both shifted arguments need levering, not just the first.** On
`OvlFunc_958_2009158`, levering only the first coordinate gets its split pair
right and then transposes `mov r1` against `mov r2` on the second.

**`dma[2]` through a `vu32 *` local** is what produces the ROM's
`ldr r1, =REG_DMA3SAD / ldr r3, [r1, #8]` busy-wait; both `REG_DMA3CNT` and
`(&REG_DMA3SAD)[2]` fold to a single pool address. `OvlFunc_879_20081c0` is a
clean confirmation — the existing example of the idiom is a `// fakematch` and
so did not read as evidence.
