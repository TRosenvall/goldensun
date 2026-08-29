# Batch 134 — levers compose, and a rule that turned out not to be one

Verified on a clean `make clean && make compare` — `goldensun.gba` SHA1
`5c4695205413df7db52b9a184815a07783999971` — with every address read out of the
linked ELF. The clean rebuild regenerated all 3,172 committed `.s` intermediates
byte-identically (`git status` empty afterwards), which is an independent check
on the whole elevated corpus, not just this batch.

## Elevated (6)

| function | address | ELF | notes |
|---|---|---|---|
| `OvlFunc_974_2008bb8` | 0x02008bb8 | rom_7fcd20 | 35 sites, one prototype removed |
| `OvlFunc_common1_ea0` | 0x0200a738 / 0x0200a9d0 / 0x0200b468 | rom_7db0c8, rom_7ddb88, rom_7e0928 | shared file, three linker scripts |
| `OvlFunc_967_2008eec` | 0x02008eec | rom_7f21b8 | no-prototype at a single call site |
| `OvlFunc_974_2008f14` | 0x02008f14 | rom_7fcd20 | sibling of 2008bb8, first screen |
| `OvlFunc_938_2008264` | 0x02008264 | rom_7c37ac | four levers stacked, 70 → 0 |
| `OvlFunc_895_200892c` | 0x0200892c | rom_78dee8 | source order + dominating-block naming |

`OvlFunc_common1_ea0` has three addresses because its file is loaded by three
overlays at different bases. That it resolves correctly in all three is the
confirmation that splitting a shared common file updated every script.

## Levers compose — read WHERE the diff is, not how big it is

`OvlFunc_938_2008264` needed four separate documented spellings stacked:

    plain transcription                             70 differing
    + int intermediate for the halfword stores       9
    + name the shifted argument                      8
    + name the pooled argument as well               6
    + name both spilled stack arguments              0

The 9 → 8 step is the one worth keeping. By count it looks like a dead end; but
the first differing line jumped from 49 to 65, meaning the early blocker was
solved and a later one had surfaced. **Position is the signal, not the total.**

The same function shows that a lever can be *insufficient* rather than
inapplicable: naming the shifted argument alone left a register pair transposed,
naming the pooled argument alone did nothing, and together they were exact.
Retry a failed lever in combination before recording it as a dead end.

## Two spilled stack arguments want two named locals

A call with more than four arguments spills the rest to `[sp]`. As bare literals
gcc reuses one register — load, store, load, store. The ROM materialises both
into two registers and then stores both. Naming them as two locals reproduces it;
one fix covered both six-argument calls in the function.

## The dominance precondition is real

`OvlFunc_895_200892c` came with its own control. Naming the two split-build
arguments in the block **dominating** the guarded call was exact; naming them
*inside* the guarded arm left the count untouched at 3. The docs stated this
precondition for the interleave lever; this is the first time it has been
screened both ways in one function.

## A documented rule that is not a rule

The commoned-constant tell — an added push of a callee-saved register holding a
constant used more than once — is reliable as a *diagnosis*. The docs also said
it is fixed either by `CSE_CFLAGS` or by separate named locals, and to try both.

`OvlFunc_881_2009c08` shows the tell in textbook form and **both remedies fail**,
as does every other flag group in the tree. The docs additionally offered a guess
that named locals apply when the constant is reused inside one block; this
function is exactly that shape (`br == 0`) and they do not. Both claims are now
corrected in `docs/elevation.md` rather than left to mislead the next round: the
tell identifies the cause but does not promise a fix.

## The symbol-base lever is bounded by the displacement range

Batch 133's fix — declare the symbol with the access's type and index it — keeps
an offset as an addressing-mode *displacement*. That only works while the offset
fits one. `Func_8094428` needs `gState + 0x1f4`, and thumb word loads cap at 124,
so the offset must live in a register regardless; at that point gcc folds it into
the pooled address and no spelling reaches back. Check the offset against the
mode first: byte/halfword/word displacements are 31/62/124.

## Do not carry a reuse verdict between functions

`Func_80933f8(-1, -1, -1, 0)` appears in `Func_8094428` with the `-1` commoned
and in `OvlFunc_965_2008eac` with it rebuilt three times. Same callee, same
arguments, opposite codegen. The choice belongs to the translation unit, not the
call — so a second function cannot be parked by analogy with the first.

## Tooling: `pool.py` gained a `reuse` column

It counts constants that cost more than one instruction to build and are needed
more than once — the shape gcc hoists into a callee-saved register while the ROM
rebuilds in place. Validated against six recorded outcomes (two known-reuse
parks, one exact match, three parks blocked on other things) and checked to
discriminate rather than always fire.

It paid immediately: of six dense candidates it flagged four as traps and two as
clean, both clean ones became elevations, and nothing was spent on the other
four. Two functions had been transcribed correctly and lost to that blocker in
the round before the column existed.

## Parked (3)

| function | at | blocker |
|---|---|---|
| `OvlFunc_881_2009c08` | 34 of 49 | commoned constant; both documented remedies fail |
| `Func_8094428` | 3 lines of 83 | symbol base past the displacement range |
| `OvlFunc_965_2008eac`, `OvlFunc_966_2009090` | — | constant reuse (previous round, listed for context) |

## A counting discrepancy, left open

Batch 133 recorded 2208 → 2202 remaining functions. Counting now — walking
`asm/**/*.s` for `.thumb_func_start`/`.arm_func_start` and skipping any TU with a
`.c` at the mirrored `src/` path — gives **2248**, which cannot be reconciled
with 2202 minus this batch's 6. The two figures come from different methods and
I could not find a tool in `tools/` that reproduces the earlier one.

Reporting the measured number with its method rather than a number that
continues the previous series. The delta across these two batches should not be
read as a rate until the methods are reconciled. Elevated `.c` files (excluding
parks) stand at 3,171 and parks at 305, both directly counted.

### Resolved, immediately after publication

The discrepancy was not a difference of method. Batches 130-133 ran
2224 -> 2219 -> 2214 -> 2208 -> 2202, and every step equals that batch's
elevation count exactly: the figure was a hand-maintained counter being
decremented rather than a measurement, so a baseline error could never correct
itself. It had drifted 46 low.

Counting four ways -- raw occurrences and distinct names, with and without
excluding TUs that already have a `.c` -- all give **2248**, so the definition is
not ambiguous either. gcc-generated `.s` intermediates carry `.thumb_func`, not
`.thumb_func_start`, so they never contaminate the count.

`tools/remaining.py` now measures it. Future batches should print its output
rather than subtracting from the previous batch.
