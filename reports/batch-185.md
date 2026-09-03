# Batch 185

Five elevated, three parked, and the first park in blocker class 5 backed by a
**proof** rather than an exhausted search.

Two themes ran through the round. The `[cse]` marker failed twice more, and the
second failure was systematic enough to be worth stating as a rule about the
marker rather than another anecdote. And two agents independently found that a
correct lever applied at the wrong *scope* is worse than not applying it at all.

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `OvlFunc_899_200a564` | `0x0200a564` | [ovl_30_…_c_c_b.c](src/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_c_c_b.c) | `ldrsh` wants a REG+REG address |
| 2 | `OvlFunc_932_200a804` | `0x0200a804` | [ovl_30_…_a_c_a_b.c](src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_c_a_c_a_b.c) | `CSE_CFLAGS`; a pooled zero at a `strb` is an **HImode temporary** |
| 3 | `Func_80b9934` | `0x080b9934` | [rom_b8228_c_a_c_c_a_c_c.c](src/rom_b5000/rom_b8228_c_a_c_c_a_c_c.c) | splitting a two-role local; naming **both** halves of a store through a call result |
| 4 | `OvlFunc_924_200a1cc` | `0x0200a1cc` | [ovl_1db4_b.c](src/overlays/rom_7ac2d8/ovl_1db4_b.c) | **write the redundant compare** — the `else if` chain is the spelling |
| 5 | `OvlFunc_881_200b1fc` | `0x0200b1fc` | [ovl_30_…_c_c_b.c](src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_c_b.c) | the **multiply/shift split point** places a second call |

Parked: `OvlFunc_960_2008b24` (21 of 88), `Func_801b424` (12 of 91),
`Func_8077f70` (4 of 123, only 2 byte-affecting).

## A CLASS-5 PARK WITH A PROOF

Blocker class 5 has always been recorded as "nothing I tried moved it". It does
not have to be. `-fsched-verbose=6` prints the ready list with each insn's
priority, and `rank_for_schedule` returns on priority first — so the table says
whether a scheduling gap is one tie-break away or structurally impossible.

On `Func_8077f70` the store that must move has priority 34; the shift that takes
its slot has 36. The only route to 36 is an **anti-dependence** on that shift —
reading the register it writes. The shift writes exactly one register, and the
store's source is a different one, so **no C spelling that keeps this
instruction set can create the dependence**. `.20.ce2` already holds the ROM's
order; sched2 sinks the store afterwards.

That park sits at 4 of 123 with only 2 byte-affecting, and it is now the model
for the class: run the probe before writing "nothing moves it", because a proof
and an exhausted search read identically in a park and are worth very different
things to the next reader.

## THE `[cse]` MARKER IS A SYSTEMATIC FALSE POSITIVE, NOT AN UNLUCKY ONE

Two more failures this batch make it **six wrong in seven**. The second is the
one that matters:

> The marker fires on **textual constant repetition** and cannot see **arm
> exclusivity**. A chain of `else if` guards testing overlapping flag ids is
> therefore a structural false positive, not bad luck.

`OvlFunc_924_200a1cc` matched on its *first* spelling at default flags, with
four flag ids repeated across mutually exclusive arms. `OvlFunc_881_200b1fc`
likewise — its repeats sit in different `switch` arms, so cse never relates
them.

The bucket still has value as a *selection* heuristic — it picks functions of the
right shape and size, and five of this batch's candidates came from it. It has
no value as a *diagnosis*. Worth keeping for the first and ignoring for the
second.

The same round also fixed a real bug behind the neighbouring bucket:
`_site_kind` treated any label as a basic-block boundary, but gcc dumps a
literal pool mid-function and branches over it whenever a load's `pool_range` is
short, and that pool carries a `.L` label. It landed inside the lookahead
window, so **every branch-over-pool function read as `[offset]` when it was
`[cse]`**. `OvlFunc_932_200a804` is the specimen — classified `[offset]`,
actually a guard/set pair that `CSE_CFLAGS` closed.

## A CORRECT LEVER AT THE WRONG SCOPE IS WORSE THAN NO LEVER

Two independent instances, and together they change how the levers should be
written down.

**The narrow-mask lever.** The recorded fix is to name a mask so it is not
narrowed. On `OvlFunc_881_200b1fc` that is worth 14 → 12 *when the assignment
sits immediately before the use*. Declared with an initialiser at the top of the
enclosing block — so it lives across an intervening call — the pseudo takes a
third callee-saved register and rotates every register after it: **14 → 36,
strictly worse than not naming the mask at all.**

**The multiply/shift split.** Splitting a computed argument out of a call is
right, but only at the ROM's *own* boundary. Putting exactly the multiply in the
statement and leaving exactly the shift-and-add in the argument expression is
clean; naming only the raw call results is 12, the same as doing nothing; and
leaving one shift behind in the statement misplaces a single `lsl` by one slot,
3 differing. **Screen fully-in against partially-in before concluding the lever
does not apply.** Function scope against block scope was byte-identical here, so
scope was not the lever — the split point was.

The general form: **every "name the value" lever has a placement, and the
placement is part of the lever.** A park that says "I tried naming it and it got
worse" has not tried the lever.

## Two tells narrowed

**A pooled small constant whose consumer is a HALFWORD STORE is blocker 1b, not
a symbol.** The recorded tell says a pooled value that `mov` could build means
the source named a linker symbol. `Func_8077f70` pools a 16 — and so does our
own compiler, for the same store, with no symbol involved. Check the consumer
before touching a `.sym` file. `OvlFunc_932_200a804` is the constructive half of
the same point: its pooled zero reaching a `strb` wanted an **unpromoted HImode
lvalue** (a one-field struct or one-element array of `unsigned short`), because
`PROMOTE_MODE` forces plain `short` locals to SImode. Spelling it as a symbol was
*worse* — an SImode symbol's pool range is 1020, which lets the pool sink to the
end barrier and the ROM's mid-function pool disappears.

**Pool distance reads out the operand's mode.** Halfword constant alternative
`pool_range` 64, byte move has no constant alternative at all, word move 1020. A
mid-function pool at a short distance means HImode — identifiable before any
spelling is tried.

**The `[offset]` question is inert at `ldrsh` sites.** Thumb `ldrsh` has no
immediate-offset form, so the offset must reach a register whichever way it is
written; bare literals and per-block offset locals give byte-identical output,
measured A/B across six sites.

## Loop-invariant motion is a printed cost model, and `goto` is a blunt tool

`loop.c` moves a movable when `threshold * savings * lifetime >= insn_count`, and
subtracts 3 from the threshold **after each move** — so hoisting one invariant
makes the next cheaper. gcc runs the loop optimiser twice, and `Func_8077f70`'s
`.08.loop` dumps show a pooled constant refused on pass 1 in both loops and
moved on pass 2, because pass 1 had hoisted a mask and shrunk the loop by two
instructions. Those two verdicts bracket the threshold at 15..17, so defeating
the hoist by growing the loop would need 18 instructions — unreachable for a
13-instruction loop.

**Amendment to the `goto` note: a backward `goto` denies ALL invariant motion,
not only the motion you wanted stopped.** Where the ROM keeps something outside
the loop, the `goto` must be paired with hoisting that thing by hand — `goto`
alone 15, `goto` plus the hand-hoisted mask 7. `Func_801b424`'s park is the
clean case of the same tool used correctly: its loop head is branched to from
three places, and no single `for` or `while` produces that shape.

## Other mechanisms worth keeping

**Write the redundant compare.** When the ROM re-tests a scalar it has already
tested in an earlier arm, that is not redundant codegen — it is the literal
`else if` chain. Nesting the second arm under one shared test, the version a
reviewer would prefer, *loses the compare*.

**Two shift statements beat a `(short)` cast** for an in-place sign extension:
the cast builds a sign-extend pattern with a clobber and reload hands it a
scratch, giving a three-register `lsl / asr`.

**Do not disable sched2 while testing the declaration lever.** Implicit against
prototyped is 9 against 18 on `Func_8077f70`, but under `--no-sched2` both are
wrong and equal — the ROM's argument order comes from sched2 *fed* that operand
order.

**Check whether two shifted immediates share a byte** before naming them. The
interleaved `mov / mov / lsl / lsl` came free from bare literals on
`OvlFunc_924_200a1cc` because both constants share the same `mov` byte and gcc
batches identical values; a sibling needed locals precisely because its
immediates differ.

**Grep the corpus for a solved neighbour first.** `Func_8077f70`'s first screen
was 26 of 123 because a solved function in the same bank is verbatim its middle
two-thirds. Largest first-screen improvement recorded, for one grep.

## State

- **1,867 functions remain in assembly** — 641 unparked and 293 parked in the
  main ROM, 610 unparked and 323 parked across the overlays. 3,489 elevated
  `.c` files.
- `make clean && make -j8 && make compare` green; SHA1
  `5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
  linked ELF, `.gcc2_compiled.` present in each object.
- Four splits, each verified byte-neutral before any `.c` landed. **Two were
  hand-split**: both `ovl_1db4.s` and the batch-184 case carry their overlay's
  `.data` *after* the target function, and `split_s.py` keeps trailing data with
  the function it follows — which would have carried it into the `.c` and
  dropped it.
- Two functions were attempted and set aside rather than parked
  (`OvlFunc_899_20085bc`, `OvlFunc_891_20094b8`): two and three spellings
  respectively is not enough measurement to justify a park, and a thin park
  misrepresents the evidence to whoever picks it up.
