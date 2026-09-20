# Batch 272 -- thirteen functions, zero fakematch debt, and a callee's return type three times

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All thirteen addresses checked
against the linked ELF, plus the three functions the splits left in assembly
(`Func_8096ddc`, `Anim_MoveIntro`, `Func_80a2324`), all still at their original
addresses. A control symbol came back absent.

| | |
|---|---|
| elevated | **13** |
| solved, verified, deliberately NOT landed | **2** (each costs something) |
| parks retired | 6 |
| parks closed with mechanism | 1 |
| new parks | 1 (a banked solution, not a blocker) |
| new tools | 0 |
| `.sym` entries added | 0 |
| splits | 7 (one file became a single TU with no split) |
| Makefile rows added | 0 |
| **fakematch debt added** | **0** |

**Four screening subagents, each given two never-attempted functions and one park.**
Twelve targets; **eleven came back exact**, one improved, one was proved unreachable.
Agents screened only -- no `make`, no git, writes confined to their own scratch
directories -- and every result was re-verified here with `objcmp` and the full build
before landing.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `OvlFunc_964_2009458` | `0x02009458` | [ovl_30_a_c_c_a_a_a.c](src/overlays/rom_7ed0a0/ovl_30_a_c_c_a_a_a.c) |
| 2 | `Func_8096d84` | `0x08096d84` | [rom_96cdc_a_a_c_b.c](src/rom_8a000/rom_96cdc_a_a_c_b.c) |
| 3 | `OvlFunc_882_200a09c` | `0x0200a09c` | [ovl_30_c_c_c_c_a_a_a_c_c_a_c_c_b.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_a_c_c_b.c) |
| 4 | `Func_80b8f58` | `0x080b8f58` | [rom_b8228_c_a_c_c_a_c_a_a.c](src/rom_b5000/rom_b8228_c_a_c_c_a_c_a_a.c) |
| 5 | `Func_801f680` | `0x0801f680` | [rom_1de5c_c_c_c_c_a_a_c_a_a_c.c](src/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_a_c.c) |
| 6 | `OvlFunc_898_2008e0c` | `0x02008e0c` | [ovl_314_c_c_c_a_a_c_c.c](src/overlays/rom_793768/ovl_314_c_c_c_a_a_c_c.c) |
| 7 | `Func_80a32b8` | `0x080a32b8` | [rom_a1814_c_a_c_c_a_c_c_c_c_b.c](src/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_b.c) |
| 8 | `Func_80a8cc0` | `0x080a8cc0` | [rom_a8604_a_a_c_c_b.c](src/rom_a1000/rom_a8604_a_a_c_c_b.c) |
| 9 | `Func_80b17e4` | `0x080b17e4` | [rom_b0070_a_a_c_c_c_a_a_b.c](src/rom_b0000/rom_b0070_a_a_c_c_c_a_a_b.c) |
| 10 | `OvlFunc_944_20090a0` | `0x020090a0` | [ovl_30_c_c_a_c_c_c_b.c](src/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_c_b.c) |
| 11 | `UploadBGPalette` | `0x080c1724` | [rom_c10e8_a_a_c_b.c](src/rom_b5000/rom_c10e8_a_a_c_b.c) |
| 12 | `Func_80218dc` | `0x080218dc` | [rom_20198_c_c_c_a_a_c_c_c.c](src/rom_15000/rom_20198_c_c_c_a_a_c_c_c.c) |
| 13 | `Func_8021950` | `0x08021950` | *(same TU)* |

Items 1--3 were solved and verified in batch 271 and banked on that round's budget;
the other ten are this round's.

**Zero fakematch rows.** Batch 270 bought its throughput entirely with pins and said
so; 271 added one; this one added none, across thirteen functions. The two results
that *would* have cost something are held below.

## The pool has moved, and that shaped the round

Picking targets turned up a structural fact worth recording: **every function at 40
instructions or fewer is now parked.** `census` puts zero available in the 1--20 and
21--40 bands. The unparked pool starts at 41, and there were exactly **eight**
functions in the 41--60 band -- which is why each agent got two, and why that band is
now empty of unassigned work.

So the never-attempted functions are now bigger than anything that had been landing,
and most share a `.s` with several neighbours. Seven splits this batch against one in
271. One file was the happy case: `rom_20198_c_c_c_a_a_c_c_c.s` held exactly two
functions, two different agents solved one each, and the whole file became a single
translation unit with **no split and no linker change at all.**

## A callee's return type, three times, through two different passes

The highest-yield finding of the round, and all three functions had been parked for
batches.

| function | pass | park's own attempt |
|---|---|---|
| `OvlFunc_898_2008e0c` | sched2 dependent counts | swept 31 spellings across batches 108, 194, 264 |
| `Func_80a8cc0` | `precompute_register_parameters` | tried "the no-prototype lever" |
| `Func_80218dc` | `precompute_register_parameters` | same |

For `2008e0c` the fix was `extern void __CutsceneEnd(void);` on a call the park had
left **undeclared**, so C89 gave it an implicit `int`. The mechanism: the two argument
movs are equal priority and both ready at t=0, so `rank_for_schedule` falls through to
*"prefer the insn which has more later insns that depend on it"*, and the tail's
`(unspec_volatile [(return)] 1)` depends on the last **SET** of every register. An
implicitly-`int` callee emits `*call_value_insn` carrying a real
`(set (reg:SI 0 r0) (call ...))`, which displaces the r0 argument as
`reg_last_sets[0]`; the return then depends on the r1 argument and not the r0 one --
four dependents against three. A `void` callee only *clobbers* r0, clobbers go to
`reg_last_clobbers` which that handler never walks, the counts tie at four, and
`INSN_LUID` breaks the tie in source order.

For the other two it is the recorded argument-order table: for an argument needing a
precompute, a **void** callee emits `r2, r0, r1` and an **int** callee `r2, r1, r0`.

Three consequences:

- **"Prototype every callee" is not enough -- the prototype has to say `void`.** And
  "the no-prototype lever" is the *opposite* change; both `precompute` parks had tried
  it and concluded the source could not reach the ordering.
- **It is scoped to the basic block.** Dropping the `void` on the call one step
  earlier in the same block is also 2 differing; dropping it on a call in another
  block is inert.
- **A pin can never reach the `2008e0c` class.** A pin binds a register; that
  ready-list choice is made on dependent counts. That retroactively explains batch
  194's failed pin attempts.

## A difference COUNT is not a difference

`2008e0c`'s park recorded, and acted on for three batches:

> *"`-fno-schedule-insns2` IS INERT (still 2). The swap is therefore NOT sched2 ...
> That rules out the whole adjacent-pair toolkit at a stroke."*

The flag is still 2 differing -- and they are two **different instructions**. With
sched2 off the argument pair is *correct* and the diff moves to the prologue block,
where the ROM is itself scheduled. It was sched2 all along, and an elimination
argument built from a count threw away the right toolkit.

**Diff the differing PAIRS between two runs, not their totals.** Second time a count
has misled this corpus, after the label false negative where `tryc` reports 19
differing for a function whose bytes are identical.

## Pseudo numbers decide ties, and three different statements create them

Four results this round turn on *when a pseudo is created*, which is worth stating as
one idea.

**A priority tie between two locals is broken by DECLARATION ORDER.**
`OvlFunc_948_2009308`'s park concluded its residue was "not something the source
expresses". `.18.greg` gives the processing order, `.17.lreg` the identities: two
locals with **two references each** tie on priority, and `allocno_compare` falls
through to its documented last resort -- *"sort by allocno, so that the results of
qsort leave nothing to chance"*, ascending pseudo number. gcc numbers locals in
declaration order. Moving one below the other flips the tie and resolves all four
differing instructions.

The park had even measured "declaration order swapped" and still missed it, because
it moved the wrong variable. **The declaration that must move belongs to the other
member of the tie**, which you read off `.17.lreg`. It also explains that park's twin,
which matches with the *original* order: there a reference count differs, so there is
no tie and the order never mattered.

**A split declaration and initialiser are not equivalent, and not merely inert.** On
`Func_80a32b8`, `int one = 1;` at the top is exact, a bare literal is 3 differing, and
`int one;` at the top with `one = 1;` in the block is **22** -- worse than the literal,
because the extra pseudo shifts register numbering and rewrites the whole offset
chain.

**`expand_assignment` forces a store's ADDRESS into a register before the RHS.** So
`REG_BLDCNT = 0x3f42;` creates the address pseudo first and it takes the lower
register; naming the value first reverses the pair. 16 differing to 8.

**And a named mask can shorten the OTHER value's range.** On `OvlFunc_944_20090a0`,
ARM's `REG_ALLOC_ORDER` hands out r3, r2, r1, so the first-allocated of two tied
2-reference quantities takes r2; with equal reference counts the priority reduces to
*shorter live range wins*. Written naturally the mask is born after the pair it
competes with and both land in r2. Naming it as its own statement *before* them makes
the other quantity the shorter one. A pin on the same value is inert, so this cost
nothing.

## Assign a value TWICE to push it out of local-alloc

`UploadBGPalette` took 29 screens and this is the lever worth carrying, because it is
the usual reflex **backwards**.

With a separate result local, `.17.lreg` reports `set 1 time` and local-alloc gives
the value r0; `.18.greg` then has hard reg 0 in the multiplier's conflict set, so the
multiplier is pushed to r4, only r1/r2/r3 remain as loop scratch, the three products
spill to r8/r9/r10 and the prologue grows two high-register moves. Reusing the input
variable for the result gives `set 2 times; dies in 2 places`, which **local-alloc
cannot form into a single quantity** -- it falls through to global-alloc, lands in r4,
and the multiplier gets r0, the ROM's deal.

So: a value assigned **once** is a local-alloc quantity; a value assigned **twice** is
a global-alloc allocno. When the ROM's register choice looks like a global one, try
reusing an existing variable rather than naming a new one.

Its other lever: **Thumb's `mulsi3` is destructive**, so `r = (c & 0x1f) * amount`
cannot tie the product to the dying mask and reload pays a copy at every site;
mask-then-`*=` as two statements makes the mask pseudo the product pseudo.

## One park closed with arithmetic

`OvlFunc_951_2008880` stayed at 4 of 47, but from a much better-conditioned shape --
prologue, both pool-load orders, the whole first loop and the epilogue byte-exact,
with two adjacent transpositions left -- and it is now **closed**, not open.

`rank_for_schedule` compares `INSN_PRIORITY` first and returns before any tie-break.
Priority is the longest path to the block end through **true** dependences, so an insn
whose only successor is reached by an anti-dependence inherits nothing. `add r5, #2`
is ready at the contested slot with priority **1** against three competitors at **2**;
its only successor is the call, via a cost-0 anti-dependence. For the ROM's order it
would need priority >= 3, which requires a true dependence onto the following `strh`
-- i.e. that store would have to read r5, and in the ROM it reads only r3 and r7.

**No source spelling can create the edge.** The alias reading was checked rather than
assumed and does not apply: the missing edge is a *register* dependence, not a memory
one, so a union, a typed field and `-fno-strict-aliasing` are all irrelevant. And
`-fno-schedule-insns2` is actively wrong here, because the ROM's order is itself a
*scheduled* order rather than the unscheduled one. Ten further flags inert.

## Held, because each costs something

Both are verified exact. Neither is blocked on evidence.

**`Func_80a9a5c`** -- all 58 instruction encodings and all 12 relocations match at
identical offsets; the one differing word is the pool entry, where ours holds an
`ABS32` against `_MSG_b24` and the ROM holds the literal. It needs `_MSG_b24 = 0xb24;`
in `message.sym`, which is a build input.

The measured argument for the symbol is good: with the plain literal the function is
**nine** differing, and the sole fault is sched2 hoisting `ldr r5, =0xb24` into the
load-latency slot ahead of three calls, at priority 11 against the calls' 9 --
structurally pinned, so no spelling reaches it. That is the recorded *"a pool load of
a SYMBOL is not hoisted, where a pool load of an int constant is"* tell firing **on
its own**; the more familiar "gcc never pools a constant it can build with a `mov`"
tell says nothing, because 0xb24 is not a shiftable byte. Worth stating before anyone
adds the entry on the wrong argument.

Also recorded and **rejected**: `msg = (int)&_MSG_b20 + 4;` reaches the same result
with no `.sym` edit, emitting pool word 4 plus `ABS32 _MSG_b20`. It works and it is a
lie about the source.

**`OvlFunc_948_2009308`** -- exact, but needs one register pin, so one `fakematch.txt`
row. The pin minimum was brought down from the park's two sites to one, and both
single-site forms are byte-identical.

## State

```
funcindex --stats   4386 from C, 1324 still in asm     (batch 271: 4373 / 1337)
tools/census.py     TOTAL 1322  (76 hand-asm, 14 ARM, 572 parked, 660 available)
matched .c files in src/ (excl. parks): 4016
park files: 464   (batch 271: 469 -- six retired, one banked solution added)
fakematch.txt rows: 448  (batch 271: 448 -- UNCHANGED)
```

**+13 from C and -13 still in asm, for exactly the thirteen elevated** -- the SEVENTH
consecutive batch where the delta reconciles. Batch 264's figure remains unexplained
and that entry stays open.

Thirteen functions is the largest batch in the recent log by a factor of two and a
half, and it added no fakematch debt at all. The reason is not a better lever -- it is
that twelve parks and new functions each got a **gcc dump read against them** in one
round. Nine of the thirteen turned on something visible only in `.17.lreg`,
`.18.greg`, `flow2` or `-fsched-verbose=5`.

## Not done

* The two held results above -- one `message.sym` line and one pin, both decisions
  rather than measurements.
* `OvlFunc_951_2008880` at 4, closed; needs a compiler-side change.
* `HeightTile_A` at 2 from batch 271, blocked on `set_preference`'s operand-0
  preference.
* The commoned-constant parks (`overlays/2008e34.c`, `ovl903_200843c.c`,
  `ovl_7d30e0/200938c.c`, `ovl_7e7574/200a69c.c`) are still untouched and will still
  fall to the batch-269 pin, at four fakematch rows.
* The 41--60 available band is now empty. The next never-attempted targets are the
  85 functions in 61--100, all of which will need splits.
* The three global_alloc parks (`Func_80a8578`, `Func_80cd52c`, `Func_80919d8`) still
  want `global.c`, though `allocno_compare`'s tie-break and `set_preference` have both
  now been read.
* `8021390`'s `_MSG_1b` question and `OvlFunc_968_200c048`/`200c520` behind
  `200c2bc`'s floor, both still held.
