# Batch 39 — the lever generalises, and finds its own edges

*Status: ready to port.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–38 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare` — 96 overlays compared
byte-for-byte and `goldensun.gba: OK`. Every address read back from the linked
overlay ELF with `nm`, and the one new symbol from `stage1.o`.

## The basic-block lever is not about shifts

Batch 37 described it as fixing a **shifted** constant whose `mov`/`lsl` pair the
ROM splits around another argument. That was too narrow.

`OvlFunc_943_2008c28` passes `-0xa`, which gcc builds as `mov r2,#0xa /
neg r2,r2` — and the ROM splits *that* pair the same way:

    rom    mov r2,#0xa / mov r0,#0 / mov r1,#1 / neg r2,r2
    ours   mov r2,#0xa / neg r2,r2 / mov r0,#0 / mov r1,#1

Assigning `n = -0xa;` in a dominating block fixes it exactly as it fixes a
shift. **Read the rule as "a constant that takes two instructions to build",
not "a shift".**

That correction matters for scale, not just tidiness: `tools/find_bb_lever.py`
had been looking only for `mov`/`lsl` and reporting 73 reachable functions. With
`mov`/`neg` included it reports **391, across 1,268 sites.**

## …and it does not reach inside a loop body

`OvlFunc_935_2008b8c` is parked at **2 of 51**, and it is worth having as a park
rather than as assembly because it is the lever's other edge.

The assignment has to be in a block that dominates the call — and in a loop,
every such block is also reachable across the **back edge**, so the value is live
around the loop and gcc keeps it in a callee-saved register instead of
rematerialising at the call. Both placements measured:

| | |
|---|---|
| literal at the call site | **2 of 51** (kept) |
| assigned before the loop | 7 of 51 |
| assigned in the block that jumps *into* the loop | 9 of 51 |

So the rule needs a clause: **a dominating block that is not part of the loop the
call sits in.** For a call in the only block of a self-contained loop there is no
such block — the same position the straight-line functions are in, for a
different reason.

## A `switch` is not an `if`/`else` chain

`OvlFunc_943_2008c28` came out **16 of 50** written as `if (v == 1) … else if
(v == 3) …`, and 2 of 50 written as a `switch`. The ROM tests both cases up front
and branches away from each:

    cmp r3, #1 / beq L0 / cmp r3, #3 / beq L1 / b L2

An `if`/`else` chain gives `cmp` / `bne` instead. Same class of discriminator as
the unsigned selector in `OvlFunc_881_200b448` — **the shape of the chain reports
what the source was**, and it is readable off the ROM before writing a line.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `OvlFunc_921_2008a3c` | `0x02008a3c` | rom_7a7298 | basic-block lever |
| `OvlFunc_939_20088ec` | `0x020088ec` | rom_7c460c | three levers stacked |
| `OvlFunc_945_2008670` | `0x02008670` | rom_7cb2c0 | lever + undeclared callee |
| `OvlFunc_943_2008c28` | `0x02008c28` | rom_7c7b9c | `switch`, then `mov`/`neg` |
| `OvlFunc_948_2009a9c` | `0x02009a9c` | rom_7d30e0 | shape-queue twin |
| `OvlFunc_948_2009ca0` | `0x02009ca0` | rom_7d30e0 | twin |
| `OvlFunc_948_2009ccc` | `0x02009ccc` | rom_7d30e0 | twin |

One symbol added to `message.sym`: `_MSG_24db`.

`OvlFunc_939_20088ec` is worth reading as a worked example — three established
levers stack in fifty instructions: the basic-block lever, the message base as a
symbol (the ROM reaches the second line with `add r5, #1`, which gcc emits only
for a symbol address), and a destructive add on a walked gState pointer.

## One contrast now visible in the tree

`OvlFunc_943_2008c28` computes its gState-style offset with the **non-destructive**
`add r3, r6, r2` and is written as a single expression. `OvlFunc_939_20088ec`,
elevated in the same batch, **walks** the pointer with `+=`. Both forms are now
adjacent in the corpus, which is the point: the ROM picks, and the C follows. A
reader who learns one form as "the rule" will lose a round on the other.

## Parked

`OvlFunc_935_2008b8c` (rom_7bf5a8), 2 of 51 — the loop-body edge above. Its note
records the three levers that *did* work on it (un-rotated `goto` loop,
fall-through exit rather than an early `return`, and a fourth sighting of
`narrow_constant` inverted) so nothing has to be re-derived.

## Counts

343 functions elevated in total, of which 7 are fakematches. 2,952 hand-written
functions remain in `asm/` of 5,714. 96 parked functions and the two
large-function experiments.
