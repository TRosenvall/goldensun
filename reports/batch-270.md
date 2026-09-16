# Batch 270 -- five functions from two parks, and a "do not attempt" note overturned

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All five addresses checked
against their overlay ELFs, plus the two functions the three-way split left in
assembly, which link at their original addresses. A control symbol not in the
batch came back absent.

| | |
|---|---|
| elevated | **5** |
| parks retired | **2** (one standing for four functions) |
| new parks | 0 |
| new tools | 0 |
| `.sym` entries added | 0 |
| splits | 1 (three-way, four functions to one `.c`) |
| build-input changes | 0 |
| fakematch debt added | **5** |

Solo throughout, no agents.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `OvlFunc_881_20097fc` | `0x020097fc` | [ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c_b.c](src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c_b.c) |
| 2 | `OvlFunc_881_2009888` | `0x02009888` | *(same TU)* |
| 3 | `OvlFunc_881_2009938` | `0x02009938` | *(same TU)* |
| 4 | `OvlFunc_881_20099e8` | `0x020099e8` | *(same TU)* |
| 5 | `OvlFunc_953_200a5f0` | `0x0200a5f0` | [ovl_30_c_c_c_c_a_c_a_c_c_a.c](src/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_c_c_a.c) |

Every one of these was already parked. Batch 269 ended by noting the
single-function overlay cold pool was exhausted and that the next round would
have to re-attack parks with new levers. That is the whole of this batch.

## One lever, two parks, five functions

Batch 269 established that a bare `register int __asm__("rN")` pin defeats cse1
on a commoned small constant, with none of the `__asm__ volatile` barrier the
recorded fakematch idiom calls for. This batch pointed it at the two parks whose
recorded blocker was exactly that, and both fell.

**`src/non_matching/overlays/20097fc.c` stood for four functions.** It named its
blocker as the `-1` rematerialisation case and pointed at `OvlFunc_945_200c13c`
-- the function batch 269 had just elevated. Two pins closed all four:
40/56/56/56 differing to exact.

**`src/non_matching/overlays/200a5f0.c` is the more interesting one**, because it
is the strongest do-not-attempt note in the corpus.

## The read was right and the verdict was too strong

That park did not guess. It read `gcse.c` and established three things:

- cse1 commons a repeated constant **unconditionally**, and separate named
  locals do NOT defeat it. Demonstrated on a solved sibling, where only one of
  six `(set (reg) (const_int 588))` survives `.03.cse`; the other five become
  copies carrying `REG_EQUAL`.
- What restores such constants is gcse's constant propagation, and cprop is
  strictly **cross-block** -- `find_avail_set` only accepts a set available at
  block entry, and `cprop_insn` skips a use when the register is already set in
  that block.
- This function's only branch sits **after** every constant use, so all three
  uses live in block 0 and cprop can never reach them.

Every step holds. It then swept six local-variable spellings, a ten-way
return-type sweep, six argument spellings, fifteen flags, and scanned all 3105
generated `.s` for precedent, finding none. It closed: *"Park immediately; do not
sweep spellings."*

**It was right about all of that and wrong about what followed.** Every step of
the argument is about what happens to a PSEUDO. A `register` declaration means
no pseudo is formed for cse1 to common in the first place. 29 differing to 2 on
the first pinned candidate, and the last two were the `SetSpeed` argument order,
which the same pins fix.

So the generalisation to carry is the opposite of the one the park drew:

> **A clean cse1 argument points AT a pin, not away from one.** The better the
> argument that the value reaches cse1 as a pseudo, the better a pin will work,
> because a pin is precisely the construct that stops it being one.

"No source construct can move this" needs qualifying to "no spelling of ordinary
locals can" -- which is what was actually measured, six times.

## Three things the teardowns found

Both fakematches were torn down before landing, per the recorded discipline.

**1. Anchor the whole argument list even when only one constant is commoned.**
On `200a5f0`, r2 holds the only commoned constant (`0xd6 << 1`, three sites).
Pinning r2 alone looks like the minimal fix and is **twelve worse** than pinning
all three argument registers. The recorded *"anchor every argument of a call you
anchor any argument of"* rule governs the defeat of cse1, not just interleaves.

| removed from `200a5f0` | differing |
|---|---|
| the three `__Func_8092158` / `__MapActor_TravelTo` pin blocks | 33 |
| the `__MapActor_SetSpeed` pin block | 2 |
| pinning r2 alone at the three sites instead of all three | 12 |
| nothing (as landed) | **0** |

**2. Pin the minimum EACH SITE needs, not the maximum any site needed.** The four
`OvlFunc_881` functions call `__MapActor_SetSpeed(8, ...)` in the same position.
`20097fc` needs all three argument registers pinned; the other three need **r0
alone**. Its SetSpeed is followed by `__Func_80921c4` with two more pool loads
and the siblings' by a store, so the interleave being fixed is not the same one.
The first working form pinned all three everywhere and landed two pins heavy.
Functions that look identical at a call do not take the same scaffolding.

| removed from the `OvlFunc_881` cluster | differing |
|---|---|
| the p0/p1 pins on `__Func_80933f8(-1, -1, -1, 0)` | 40 / 56 / 56 / 56 |
| the pins on `__MapActor_SetSpeed` | 3 / 2 / 2 / 2 |
| the named zero for the `goalFacing` store | pool 23 against 20, then 7 / 7 / 7 |
| nothing (as landed) | **0** |

**3. A pool-size mismatch can be pointing at another function's statement.** The
literal pool is per TRANSLATION UNIT. Dropping the named zero made `tryc` report
*"our pool has 23 entries and the reference needs 20"* against `20097fc` -- a
function that does not contain the store that caused it. The extra words came
from the three siblings. When a pool count is wrong and the instructions match,
look at the whole TU, not the named function.

## A halfword store of a literal goes to the pool

`*g = 0` through a `short *` narrows the constant to HImode, and Thumb HImode
constants go to the literal pool, so gcc emits `ldr r3, =0x0` where the ROM has
`mov r3, #0`. An `int` local assigned 0 and then stored keeps SImode and the
`mov` comes back.

This is the same narrowing the recorded `SetTextColor` note describes from the
other direction -- there the ROM pools a small constant and gcc must be made to
pool it; here the ROM builds it inline and gcc must be stopped. One mechanism,
both directions. **Read which side the ROM is on before deciding a pooled small
constant is a symbol.**

Related, and a trap worth naming: Thumb-1 `ldrsh` has **no immediate form**, so a
signed halfword read always appears as a register-offset load
(`mov r2, #0 / ldrsh r3, [r5, r2]`). That pair is not evidence of an index
variable -- it is what `*(short *)p` compiles to.

## The split, and a header left alone

`ovl_30_c_a_c_c_a_c_a_a_a_a_a_c_c.s` held six functions with the four targets
contiguous in the middle, so this was a three-way split: `_a.s` keeps
`OvlFunc_881_20097a4` (parked on a different blocker), `_c.s` keeps
`OvlFunc_881_2009a98`, and `_b` became the `.c`. `overlay.ld:47` became three
lines in the original order, and the layout-only build was gated green before the
`.c` landed. Both retained functions link at their original addresses.

The cluster reads `include/actor.h` fields -- 0x18 `rotX`, 0x1c `rotY`, 0x64
`goalFacing` -- following batch 269's correction about inventing structs. One
mismatch was left standing deliberately: 0x64 is read SIGNED here (`ldrsh`) while
`actor.h` declares it `u16`. The store and the test go through a local `short *`
rather than changing a shared header on one cluster's evidence; `actor.h` already
documents the identical situation at 0x04 `scriptPos`.

## State

```
funcindex --stats   4368 from C, 1342 still in asm     (batch 269: 4363 / 1347)
tools/census.py     TOTAL 1340  (76 hand-asm, 14 ARM, 580 parked, 670 available)
matched .c files in src/ (excl. parks): 3999
park files: 477   (batch 269: 479 -- two retired, none added)
fakematch.txt rows: 447  (batch 269: 442)
```

**+5 from C and -5 still in asm, for exactly the five elevated** -- the FIFTH
consecutive batch where the delta reconciles. Batch 264's figure remains
unexplained and that entry stays open.

Note the shape of this batch in the numbers: park files went DOWN by two while
five functions landed, because one park stood for four. `fakematch.txt` grew by
five, which is the largest single-batch fakematch addition in the log and is
worth watching -- every row is teardown-verified, but five in one batch against
440 accumulated over 269 is a rate change, not a continuation.

## Not done

* **The fakematch rate.** Five rows in one batch. The lever is real and each
  file is minimal, but a round that lands only fakematches is buying throughput
  with debt. Worth a batch of ordinary elevations next.
* The remaining commoned-constant parks (`overlays/2008e34.c`,
  `ovl903_200843c.c`, `ovl_7d30e0/200938c.c`, `ovl_7e7574/200a69c.c`) are the
  same class and should fall to the same lever -- which is exactly why the
  previous point matters.
* The three global_alloc parks (`Func_80a8578`, `Func_80cd52c`, `Func_80919d8`)
  still want `global.c`.
* `Func_8028ef0`'s residue is reload scratch plus argument-setup order.
* `8021390`'s `_MSG_1b` build-input question and
  `OvlFunc_968_200c048`/`200c520` behind `200c2bc`'s floor, both still held.
* The `DMA3_SET` clobber widening measured green tree-wide in batch 268 is still
  deliberately unshipped.
