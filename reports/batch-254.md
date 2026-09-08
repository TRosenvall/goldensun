# Batch 254 — five agents, ten functions, and three recorded rules bounded

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_943_2008598` | `0x02008598` | [ovl_30_a_a_c.c](src/overlays/rom_7c7b9c/ovl_30_a_a_c.c) |
| 2 | `OvlFunc_943_2008724` | `0x02008724` | [ovl_30_a_a_c.c](src/overlays/rom_7c7b9c/ovl_30_a_a_c.c) |
| 3 | `OvlFunc_956_20082f8` | `0x020082f8` | [ovl_30_a_c_c_a_c_c_c_c_a.c](src/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c_c_a.c) |
| 4 | `OvlFunc_907_2008ae0` | `0x02008ae0` | [ovl_30_c_a_c_c_a_a_c_c.c](src/overlays/rom_79b154/ovl_30_c_a_c_c_a_a_c_c.c) |
| 5 | `OvlFunc_924_200cc68` | `0x0200cc68` | [ovl_35b8_a_a_c_a_c_c_c.c](src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_c_c.c) |
| 6 | `OvlFunc_924_200cf44` | `0x0200cf44` | [ovl_35b8_a_a_c_a_c_c_c.c](src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_a_c_c_c.c) |
| 7 | `OvlFunc_882_2008a10` | `0x02008a10` | [ovl_30_…_c_c_c_b.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_c_c_c_b.c) |
| 8 | `OvlFunc_898_200936c` | `0x0200936c` | [ovl_314_c_c_c_a_c_c_a_c_c_c.c](src/overlays/rom_793768/ovl_314_c_c_c_a_c_c_a_c_c_c.c) |
| 9 | `OvlFunc_936_200b1b8` | `0x0200b1b8` | [ovl_30_c_c_c_c_a_a_a.c](src/overlays/rom_7c097c/ovl_30_c_c_c_c_a_a_a.c) |
| 10 | `OvlFunc_923_2009730` | `0x02009730` | [ovl_1150_c_c_c_a_b.c](src/overlays/rom_7aa430/ovl_1150_c_c_c_a_b.c) |

Seven landed whole; three needed splits, each with the layout-only build gated
green **before** any `.c` existed.

## Parked, both with floors and eliminated axes

| function | address | floor | eliminated |
|---|---|---|---|
| `OvlFunc_882_2008d5c` | `0x02008d5c` | **4 of 143**, exact size, relocations silent | alias — `-fno-schedule-insns2` regresses 4 → 32 |
| `OvlFunc_935_2008170` | `0x02008170` | **26 of 150**, exact size and count, relocations silent | alias — flag regresses 77 → 86, **and all 64 union subsets measure exactly 26** |

## THE DEAD CALLEE-SAVED REGISTER CLASS IS REFUTED

`2009730`'s ROM sets `sl` to zero and never reads it. The class has been parked
**twice** on the reasoning that *"a local assigned zero and never read is removed
before allocation."*

That is true for a dead **local** and false for a dead **expression**. Writing the
loop's byte-clears as `*p = (*p & mask) | zero;` makes `loop.c` hoist `zero` with
the other invariants; the fold happens afterwards, and the register reaches the
allocator with no consumer. Four instructions short became exact — residue
207 → 15.

> **Discriminator:** the dead register is set in a loop's **invariant block**,
> beside registers that *are* used.

`OvlFunc_945_200b7b4` and `OvlFunc_935_2008704` are parked on this class and
should be re-screened.

## THREE RECORDED RULES GAINED BOUNDS

**The cprop exemption has a DISTANCE limit.** The rule says `m += k` survives
`-O2` where `m + k` needs a pin. On `2008a10` the redefinition sits 55
instructions and ~30 calls from the definition, and a plain local still loses it.
The discriminator is distance, not shape.

**The rebuild lever and the pin are ALTERNATIVES, not a ladder.** Once a value's
register is call-clobbered by a pin, the `x = base; x <<= n;` build stops buying
anything and only costs schedule freedom — dropping to a plain literal then lets
gcc's own synthesis produce the ROM's **crossed** fill that no statement order
reaches (6 → 4). Applying it at three other sites is 11.

**The width-zero rule and the descending-fill rule are one observation.** A site
with the "ROM seeds r0 last" shape — which nominates leaving it bare — must be
pinned here: bare is 204 differing and eight bytes short, and the pin then wants
a descending fill. The shape nominates; it does not decide.

## A COMMONING TELL CAN BE DOWNSTREAM OF A CONTROL-FLOW DEFECT

On `200cc68` an r3 pin worth 223 → 144 went **completely inert** once the first
loop was spelled correctly. That is the mechanism-size search order paying
directly: the pin looked load-bearing only because the control flow above it was
wrong, and minimising the pin set first would have shipped it.

Related, from the same file: **for a mid-loop exit, write the arm the ROM falls
through to.** Two *structurally identical* scroll loops needed **opposite** C
spellings — if/else against break — because their exit tests have opposite
senses. Written the wrong way round: 96 and 38 differing.

## THE `-fno-schedule-insns2` SIGN RULE HELD FIVE TIMES

Every function in this batch that ran it got a **regression**, correctly meaning
sched2 already produces the ROM's order and alias is the wrong axis. On
`2008ae0` that prediction was then confirmed exhaustively — all twelve
source-order permutations measured inert byte-for-byte. On `2008170`, all
**sixty-four** union subsets measured *exactly* 26.

So the diagnostic is now as useful for what it **rules out** as for what it
finds, and one park attempt records the cost of skipping it: a 96-way naming
sweep run before the diagnostic cost a round and returned two encodings.

## THE RECORDED CROSS-JUMP ENTRY IS TOO PESSIMISTIC

`docs:12717` closes with *"here the fix is not available."* It is. That entry
tried only the callee return type, which moves r0 relative to r1–r3 and nothing
else; an ordering pin fixes the whole order and reaches the suffix length
directly.

And on one twin pair the same lever points **both ways from one source**: the
pooled fourth argument makes the pin *suppress* the merge, while a `mov`+`lsl`
fourth argument with coinciding shifts makes it *create* one. The lever decides
argument order; the merge follows. **The relocation count is the cheapest
cross-jump tell** — it named a first draft's defect before an encoding was read.

## SMALLER RESULTS

- **A pointer local is the store-free form of the live-range split.** Where the
  value needing a longer range is `&v` for a stack array there is no two-variable
  spelling; `q = v;` births the pseudo where you want it. Hoisting a real store
  to the same point is strictly worse (25 v 18).
- **A constant-valued pin is folded by cprop and is truly inert** — byte-identical
  to no pin, not merely the same count. The third pin device works on a value
  copied from another pseudo, never one propagated from a literal.
- **`do { } while (0)` is a full ordering barrier**, measured inert against
  `__asm__ volatile("")`. Prefer it: ordinary C, no fakematch row.
- **Pin declaration order is a lever independent of assignment order** — and it is
  per-site, not global (the same order at all five sites is 6).
- **A named pointer that should not be named** shows as N `mov rX, r0` before N
  address builds; the ROM lets the call's return register die into the address.
- **`mov r7, r0 / cmp r7, #0 / bne` then `strb r7` is not the source storing a
  flag** — it is cse's jump equivalence substituting a known-zero register.
- **A park is a template.** One park shares another function's whole opening;
  three of its spellings transferred unchanged and two of its four blockers do
  not exist there. Worth feeding the park corpus into `templated.py`.

## METHOD NOTE ON MY OWN BOOKKEEPING

The grep used to decide `fakematch.txt` rows counts `register … __asm__`
occurrences **inside header comments**, and these files document their lever
tables there. One file this batch showed three apparent pins that were all
documentation. All 80 rows added this session were re-audited against a
comment-stripped body; every one points at a file with genuine inline asm, so no
spurious row was ever written. The grep was unreliable and happened not to cost
anything.

## Gate

`make clean && make -j8 && make compare` green, `goldensun.gba: OK`. Every
address checked against the linked ELF. Pairing 4,656 sources; `--orphans` 0;
`--unlinked` unchanged at 10.
