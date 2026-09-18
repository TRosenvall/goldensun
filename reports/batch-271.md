# Batch 271 -- an agent round: five landed, three banked, three dead parks buried

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All five addresses checked
against the linked ELF, plus the three `_UIBANNER_*` aliases (all resolving to
`0x08031864` alongside `Data_31864`) and `Func_80a2324`, which the split left in
assembly and which still links at its original address. A control symbol came back
absent.

| | |
|---|---|
| elevated | **5** |
| solved, verified, NOT landed | **3** (round budget) |
| parks retired by elevation | 5 |
| dead parks deleted | **3** |
| parks closed with mechanism | 2 |
| new tools | 0 |
| `.sym` entries added | 3 (`label.sym`, aliases of an existing symbol) |
| splits | 1 (two-way, text only) |
| Makefile rows added | 1 (`SCHED2_CFLAGS`) |
| fakematch debt added | **1** |

**Four screening subagents, one per blocker class.** They screened only -- no
`make`, no git, writes confined to their own scratch directories. Every result was
re-verified here with `objcmp` and the full build before anything landed.

## What landed

| | function | address | source | cost |
|---|---|---|---|---|
| 1 | `LoadUIBanner` | `0x0801a32c` | [rom_19ebc_a_c_c_c_a_c_c_a.c](src/rom_15000/rom_19ebc_a_c_c_c_a_c_c_a.c) | 3 linker aliases |
| 2 | `Func_80a22f4` | `0x080a22f4` | [rom_a1814_c_a_a_c_a_c_a_c_c_b.c](src/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c_c_b.c) | none |
| 3 | `NewActor` | `0x0800c0cc` | [rom_c004_c_a_a_a_a_a_a_a.c](src/rom_9000/rom_c004_c_a_a_a_a_a_a_a.c) | 1 pin + 1 Makefile row |
| 4 | `OvlFunc_945_20082f4` | `0x020082f4` | [ovl_30_a_c_c_a_c.c](src/overlays/rom_7cb2c0/ovl_30_a_c_c_a_c.c) | none |
| 5 | `OvlFunc_951_2008dd0` | `0x02008dd0` | [ovl_30_c_c_c_a_c_c.c](src/overlays/rom_7d6418/ovl_30_c_c_c_a_c_c.c) | none |

Batch 270 closed by flagging that it had bought its throughput entirely with
fakematch debt, and that the obvious next queue would double it. **Four of these
five cost none**, and the one that does is the smallest kind.

## The agent round, and what it is actually good for

The four agents were pointed at four different blocker classes, deliberately
avoiding the commoned-constant queue that would have produced five more pins. Of
twelve parks screened: **five solved exactly**, one improved 4 -> 2, two closed
with mechanism, three found to be **already elevated**, one left unmoved.

The useful pattern is not "agents find matches". It is that **twelve parks each
got a gcc dump read against them in one round**, which is the thing a serial round
never has time for. Six of the twelve results are corrections to a park's own
stated diagnosis -- the parks were not short of effort, they were short of someone
reading `.17.lreg`, `.18.greg`, `flow2` and `-fsched-verbose=5` instead of sweeping
spellings.

Screening is also the right division of labour: agents must not touch the build,
because the landing gate is a single serial `make compare`.

## Three dead parks, and why that is a measurement bug

`Func_80a3ce4` had **two** park files describing it as blocked. It was elevated in
`a7075825` via the switch range test, after the split its park was waiting on.
`GetFlag`/`SetFlag`/`ClearFlag` had one, elevated in `532cfd03`.

All three describe code that is already in `src/`, and `docs/elevation.md` still
carried a sentence saying `Func_80a3ce4` "is not elevated only because its `.s`
holds four functions". Verified before deleting anything: each function is in the
linked ELF, each has a gcc-generated `.s`, and no hand-written `.s` defines any of
them.

**`funcindex` and `census` count park FILES**, so three dead parks have been
inflating the parked figure in every batch report that quoted it. Park files fell
469 from 477 this batch: five retired by elevation and three that should never have
been counted.

## Four identical pool words mean four distinct source symbols

`LoadUIBanner`'s four-way switch has a pool holding four entries that are all the
address of `Data_31864`. Measured both ways:

| source | result |
|---|---|
| three distinct externs plus the default | 29 instructions against 29, 3 differing -- the NAMES only |
| one symbol in all four arms | **9 instructions** |

gcc-2.96 tail-merges identical basic blocks, so one name in all four arms folds the
switch away entirely. The ROM keeps all four arms. **Repeated identical pool words
are a positive statement about the source**, not redundancy in the ROM.

Its park had this reading and declined to act, on the grounds that `.sym` files
"hold ID VALUES, not addresses". That is false -- `wram.sym` is 275 lines of
addresses and `label.sym` exists to name data-label addresses. The aliases are
assigned **from the symbol** (`_UIBANNER_1 = Data_31864;`), matching `label.sym`'s
own style, so they cannot drift; and what is proven (four distinct symbols) is kept
separate in the comment from what is not (what they were called).

One tooling consequence: **`objcmp` cannot return OK here and that is not a
defect.** It compares relocation NAMES, and ours say `_UIBANNER_1` where the
reference says `Data_31864` -- different names, identical resolved addresses.
`make compare` is the only authority available for that shape.

## A missing anti-dependency looks exactly like a scheduling problem

Two functions this batch, and it is the most reusable finding here.

`Func_8096d84`'s park read its residue as gcc sinking a load toward its use and
swept statement orders. The sched2 dump says the load is **ready from t=1** with
priority 0 -- it loses every slot until nothing else is left. The ROM's order is not
a priority difference at all: its load must precede the `strh` because of an
anti-dependency **gcc never built**, since the load is `char *` and the store is
`unsigned short` and `-fstrict-aliasing` disambiguates them.

Routing either access through a union restores it (`c_get_alias_set` returns 0 for
a direct union access). **`volatile` does not** -- the obvious first try, measured,
unchanged at 4.

`OvlFunc_945_20082f4` is the same thing through a struct: reading the object with
named byte fields instead of `s[9]`/`s[0x15]` changes the store's alias set and the
scheduler lands the ROM's order. Its residue was a genuine sched2 tie whose
tie-break falls through to source order, with the address insn created by reload
glued to its use -- so no statement order could ever have reached it.

**The tell that separates the two cases: an insn READY EARLY with low priority is
an alias problem; one that becomes ready LATE is a real dependence chain.** Only the
second is a scheduling question. That is twice in three batches that a supposed
sched2 wall was a type problem.

## An HImode pool load is what puts a pool in the middle of a function

`OvlFunc_951_2008dd0` was parked at 20 of 57, exactly **one instruction short**,
resting on an `_AREA_00` entry its own park called "almost certainly the WRONG
NAME".

The missing instruction was the `b` over a mid-function literal pool. An HImode
pool fixup has the narrow **32..60 byte** range, so `arm_reorg` cannot carry it to
the end-of-function barrier and `dump_table` manufactures a pool mid-function with
a branch over it; an SImode fixup reaches the end and the pool goes after the
epilogue with no branch. The `_AREA_00` spelling was SImode, which is exactly why
the function came out a line short.

The fix is one plain-C line -- the pooled zero is an `unsigned short` local -- and
the generated `.s` carries `ldrh r2, .L10 / b .L11 / .word 0` at the ROM's
position. **The symbol dependency is retired**, no `area.sym` entry needed.

**When a function is exactly one instruction short and the ROM has a mid-function
pool, the missing instruction is probably that `b`, and the question is a MODE, not
a symbol.**

## `-fno-schedule-insns2` is right sometimes, and the discriminator is size

`docs/elevation.md` calls the flag "an actively misleading probe", and batch 266
concluded no function warranted a `SCHED2_CFLAGS` rule. `NewActor` is the
counterexample: 2 differing without it, **exact** with it, and the third file in the
tree to carry it.

Both statements stand once the discriminator is named. The flag destroys the
evidence when reached for against a *large* scheduling-shaped residue -- that is
what the warning measured. Against a residue of exactly two adjacent instructions,
where the ROM's order is the *unscheduled* order, it can be the honest answer. Try
it last, and only after a source sweep has established nothing else moves.

It is the batch's only fakematch row, and the arithmetic is explicit: 40 bytes for
one pin plus one build recipe. Both are load-bearing (unpinned with the flag is
still 6; pinned without it is 2), and the pin is interchangeable between two
operands, so it is one pin on either.

## Two allocator levers worth more than the functions they came from

**Duplicate a shared store into both arms.** A store written once after an if/else
join makes the stored value a GLOBAL allocno, so local-alloc gives the arm's local
the good register and the shared value takes what is left. Writing the store inside
each arm makes it block-local -- and **jump2 cross-jumping merges the two identical
tails back into one store**, so the duplicate never reaches the output. It costs
nothing and only changes where the value is born. `OvlFunc_964_2009458`: three
instructions to exact. Its solved twin already used the shape and the finding never
got back to the park.

**One load, two variables.** `OvlFunc_882_200a09c`'s ROM needs `ldrb r3, [r3]` AND
a surviving `mov r1, r3`. Its park's double-read lever produces the copy but gives
the two loads one shared address pseudo spanning two blocks, which becomes a global
allocno and is pushed to r2. `c = o->f27; if (c != 0) { i = c; ... }` keeps the
address block-local while the copy survives, because the second variable is
multi-block and `combine_regs` refuses to tie it.

## When the priority formula decides, statement order is the wrong knob

`HeightTile_A` went 4 differing to 2. Its park had two successive diagnoses -- a
born-order theory (already dead) and a qty-number tie -- and the second is wrong
too. local-alloc sorts by `floor_log2(n_refs) * n_refs * size / (death - birth)`,
and the contest was never between two 2-reference quantities: `combine_regs` ties
the shift result into the dying `ldrsb` temp and the `minus` into that, making one
**six-reference** quantity no 2-reference quantity in a five-insn block can beat.

Five statement-order spellings all measured 4 because they moved INSNS and never
touched a REFERENCE COUNT. **Read the reference counts before reordering
statements.**

The last two instructions are a hard stop, which is worth recording so nobody
sweeps it: `global.c`'s `set_preference` keys on `XEXP (src, 0)`, and the only
multiply operand order that gives the ROM's `mul` also costs a `mov r4, r1`. Pins
measured strictly worse (28 against 2), so no fakematch row should be spent.

## Solved, verified, deliberately not landed

The per-round budget is 2--5 functions and five landed, so three verified solutions
are banked in their parks with candidates, ready to land first next round:

| function | result | landing cost |
|---|---|---|
| `OvlFunc_964_2009458` | 84 bytes, 35 encodings -- exact, no pins | none; `.s` holds it alone |
| `Func_8096d84` | 88 bytes, 41 encodings -- exact, no pins | two-way text split |
| `OvlFunc_882_200a09c` | 96 bytes, 46 encodings -- exact, no pins | two-way text split |

Two parks were closed rather than left open, both with dumps:

* **`Field_Move`** -- `sub sp` loses its scheduling slot by exactly ONE priority
  unit, structurally: on arm7tdmi a load's result costs 2 while a true dependence
  into a `CALL_INSN` costs 1, so the competing load always outranks the frame
  adjust for any source that loads from memory. Forcing a tie breaks toward the
  larger LUID, still not `sub sp`. Six further spellings identical, a VLA much
  worse. Needs a compiler-side change; **do not sweep it again.**
* **`OvlFunc_969_200db90`** -- the `-512` is created by RELOAD immediately before
  the add, so it can never precede the `ldrh`. Source position is irrelevant
  because cse and reload discard any earlier materialisation, confirmed in `flow2`
  for both a plain `int` and a split register pin. **A pin binds a pseudo; there is
  no pseudo here, so do not spend a fakematch row.** Its park's existing spelling
  is also load-bearing rather than merely equivalent -- three obvious alternatives
  are 42 lines and 11 differing where it is 41 and 2.

## State

```
funcindex --stats   4373 from C, 1337 still in asm     (batch 270: 4368 / 1342)
tools/census.py     TOTAL 1335  (76 hand-asm, 14 ARM, 577 parked, 668 available)
matched .c files in src/ (excl. parks): 4004
park files: 469   (batch 270: 477 -- five retired by elevation, three deleted as dead)
fakematch.txt rows: 448  (batch 270: 447)
```

**+5 from C and -5 still in asm, for exactly the five elevated** -- the SIXTH
consecutive batch where the delta reconciles. Batch 264's figure remains
unexplained and that entry stays open.

Fakematch debt grew by **one** against batch 270's five, which was the correction
this batch set out to make.

## Not done

* The three banked solutions. Two need a two-way text split each; none needs a
  pin.
* `HeightTile_A` at 2, blocked on `set_preference`'s operand-0 preference.
* The commoned-constant parks (`overlays/2008e34.c`, `ovl903_200843c.c`,
  `ovl_7d30e0/200938c.c`, `ovl_7e7574/200a69c.c`) are still untouched and will
  still fall to the batch-269 pin. That is four more fakematch rows whenever it is
  judged worth it.
* The three global_alloc parks (`Func_80a8578`, `Func_80cd52c`, `Func_80919d8`)
  still want `global.c` -- though `set_preference` was read this batch, so that is
  now partly opened.
* `8021390`'s `_MSG_1b` build-input question and `OvlFunc_968_200c048`/`200c520`
  behind `200c2bc`'s floor, both still held.
* The `DMA3_SET` clobber widening from batch 268 is still deliberately unshipped --
  and `Func_80a22f4` this batch is the argument for why: the legal `"+l"` form does
  the same job without naming an input register as clobbered.
