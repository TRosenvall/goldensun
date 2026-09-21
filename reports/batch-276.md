# Batch 276 -- the size question, answered: the predictor is a neighbour, not an instruction count

Gated on a clean `make clean && make -j8 && make compare`, green at the target SHA1
`5c4695205413df7db52b9a184815a07783999971`. All nine addresses checked against the
freshly linked ELF, along with every function the four splits left in assembly. Four
control symbols came back absent.

| | |
|---|---|
| elevated | **9** |
| parked | **6 new, 2 standing parks advanced, 3 parks retired** |
| new tools | 0 |
| `.sym` entries added | **2** (`_SIZE_8015430`, `_MSG_53a`) |
| splits | 4, all two-way |
| Makefile rows added | 0 |
| **fakematch debt added** | **0** |

**Four screening subagents.** Two finished the 61--100 band, one tested the size
boundary at 101--107, one re-screened four standing parks against specifically named
levers. Nine exact of sixteen.

## The round's headline: `pickable.py`'s cut-off is wrong for the wrong reason

`tools/pickable.py` rejects anything over 120 instructions as having "too many
independent residues to converge". That was never tested, and it leaves **570 of the
605** remaining unattempted functions unpickable -- the blocker flagged as due in
batches 273 and 275. Four targets at 101--107:

| target | instructions | elevated same-stem twin? | result |
|---|---|---|---|
| `Func_8090488` | 101 | yes (`Func_80903bc`) | EXACT, **1 candidate** |
| `OvlFunc_916_2008980` | 101 | yes (this directory's lever set) | EXACT, **2 candidates** |
| `Func_801fe2c` | 107 | no | 35 of 110 after ~22 |
| `Func_808a5f8` | 103 | no | 26 of 113 after ~12 |

The two with twins converged inside the 2--8 candidate range the sub-100 rounds
averaged. **Size did not predict the outcome in any of the four cases; the neighbour
did.**

The cut-off's stated REASON is also wrong. The cost at 100+ is not additive. It is
**combinatorial** -- more levers must be right simultaneously, and they interact with
sign changes. On `Func_808a5f8`, against a 30-differing baseline:

| change | differing |
|---|---|
| first `gState` explicit build alone | 31 (worse) |
| second alone | 78 (much worse) |
| **both together** | **28 (better)** |

A one-lever-at-a-time hill climb rejects each half and never finds the pair. That is
the search-procedure counterpart of "a difference COUNT is not a difference": there an
equal count hides two changes cancelling, here two worse counts hide a better pair.

**Recommendation: raise the cut-off for any function with an elevated same-stem
neighbour, before relaxing it generally.** Both failures also stalled at 75--80%
matched with every remaining item priced and two cited to a dump line, so nothing here
suggests a wall at 120 either.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `Func_80b280c` | `0x080b280c` | [rom_b0070_c_c_a_a_a_c.c](../src/rom_b0000/rom_b0070_c_c_a_a_a_c.c) *(park closed)* |
| 2 | `Func_80c1ebc` | `0x080c1ebc` | [rom_c1a34_a_a_a_a_d.c](../src/rom_b5000/rom_c1a34_a_a_a_a_d.c) *(park closed)* |
| 3 | `Func_80c1f50` | `0x080c1f50` | [rom_c1a34_a_a_a_a_d.c](../src/rom_b5000/rom_c1a34_a_a_a_a_d.c) *(park closed)* |
| 4 | `Func_8090488` | `0x08090488` | [rom_8d9a4_c_c_c_a_a_a_a_a_c.c](../src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_a_a_c.c) |
| 5 | `OvlFunc_916_2008980` | `0x02008980` | [ovl_30_c_c_c_a_c_a_a_a_c_a.c](../src/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_c_a.c) |
| 6 | `Func_80a6a98` | `0x080a6a98` | [rom_a5534_c_c_c_a_c_b.c](../src/rom_a1000/rom_a5534_c_c_c_a_c_b.c) *(`_MSG_53a`)* |
| 7 | `DecompressString` | `0x080196c4` | [rom_1908c_c_a_a_b.c](../src/rom_15000/rom_1908c_c_a_a_b.c) *(`_SIZE_8015430`)* |
| 8 | `Func_8016f2c` | `0x08016f2c` | [rom_15e8c_c_a_a_a_c_a_b.c](../src/rom_15000/rom_15e8c_c_a_a_a_c_a_b.c) |
| 9 | `Func_80bf5a8` | `0x080bf5a8` | [rom_bbb0c_a_c_c_a_c_b.c](../src/rom_b5000/rom_bbb0c_a_c_c_a_c_b.c) |

`rom_c1a34_a_a_a_a_d.s` converts WHOLE -- both its functions matched, so no split was
needed. `Func_80b280c` and the two `rom_c1a34` functions were all standing parks.

## `unsigned char *` pins a register CLASS -- and the struct lever now has bounds

Batch 275 recorded the struct-instead-of-`unsigned char *` lever and explained it
through `strength_reduce`. `Func_80c1ebc` is a **second, different mechanism**, and it
took 63 differing to 7 while dropping the push list to the ROM's `{r5,r6,lr}` with no
induction variable involved at all.

With `unsigned char *u`, the peeled `u[0]` read compiles to a direct `ldrb [r6, #0]`.
**Thumb's `ldrb` base must be LOW**, so that one use pins `u`'s preferred class to
`LO_REGS` for the whole function. Through `u->name[k]` the peel goes via the
materialised pointer, `u` is left in `mov`/`add` only, `HI_REGS` becomes admissible,
gcc takes r12, and no low register has to be saved for it.

| symptom | pass | fix |
|---|---|---|
| a base register the ROM keeps is gone, address recomputed as one pointer | `strength_reduce` | struct typing, or a `goto` loop |
| the push list is one register WIDER than the ROM's | class preference | struct typing (moves the value to r8..r12) |

**And the lever got its first two NEGATIVES in the same round, which is what makes it
usable.** On `Func_80a6794` a full `struct St *` with named fields emits
**byte-for-byte the same output** as the char-pointer form; on `Func_8016f2c` a typed
struct with a `win[8]` member array is 159 lines against 104. Both residues are
`global_alloc` reference-count priorities. **The struct lever works on
`strength_reduce` and on register CLASS. It does not work on `global_alloc`
priority** -- do not reach for it when `.18.greg` shows the contest decided on
reference counts.

## A pooled constant's REGISTER is a symbol tell, independent of shiftability

The recorded tell is an impossibility proof: `ldr rX, =<byte << n>` is always a symbol,
because `thumb_shiftable_const` means gcc can pool only what it cannot build. It covers
only shiftable values, which is a minority of `message.sym`.

`Func_80a6a98` supplies the weaker tell for the rest. 0x53a is unshiftable, so gcc
pools it either way and **the pool load is no evidence at all**. What discriminates is
the register, across a pass boundary:

- a `CONST_INT` is materialised by **reload**, immediately before its user, taking
  whatever register is free -- here r2, just freed by `and r0, r2` -- after which
  sched2 cannot hoist the load above the `and`;
- a `SYMBOL_REF` is a real pseudo allocated at **greg**, gets r3, and the ROM's
  `ldr r0,=0x1ff / ldr r3,=0x53a / and / add r0,r3 / ...` falls out whole.

**8 differing with the literal, 1 with the symbol**, and four literal spellings tie at
8. Corroborated by `Func_80a63e4` pooling the same value into the same callee.

> **A correction I owe the log.** I withheld `_MSG_b24` in batch 275 on the ground that
> 0xb24 is unshiftable and so "fails the convention". That reasoning was wrong.
> Shiftability is one sufficient argument in `message.sym`, not its entry criterion --
> roughly half the existing entries (`_MSG_cf1`, `_MSG_c7b`, `_MSG_b33`, `_MSG_1c79`,
> `_MSG_22a3` ...) are unshiftable. `_MSG_b24` is **still not added**, but for the right
> reason now: its function is parked and nobody has run the register measurement above
> against it, which is the test it actually has to pass.

`_SIZE_8015430 = 0x140` is by contrast `size.sym`'s ordinary case in its strong form --
the value is `Func_8015570 - Func_8015430` in the same file that file's own header
already documents three routines from, and 0x140 is `0xa0 << 1` so gcc builds it and
the ROM pools it. The literal also costs more than two instructions: a compile-time
constant lets gcc fold the whole DMA control word to one pooled `0x84000050`, which
deletes the register the ROM shares with the length argument. 1 differing becomes 98.

## The `goto`-loop / LICM lever has two signs, and both were in this round

| function | ROM's invariant address | form that matches |
|---|---|---|
| `Func_8016f2c` | RECOMPUTED inside the loop | `goto` loop with explicit `w++` -- suppresses LICM |
| `Func_80ab21c` | HOISTED into the preheader | ordinary loop, pointer NAMED in source order before the offset |

In the second case LICM **appends** to the preheader, so when the ROM wants the hoisted
pool load FIRST there, naming the pointer in source order is what puts it there (2
differing against 5). On `Func_8016f2c` the `for` form cost both halves at once --
`check_dbra_loop` reversed the counter AND LICM hoisted the address -- 100 differing
against the `goto` form's 33.

**So the question to ask of a ROM invariant address is not "which loop form" but
whether it is INSIDE or OUTSIDE.** Then, if outside, check the preheader ORDER too.

## Other levers that each closed a function

- **A cross-jump residue is downstream of register ALLOCATION.** gcc-2.96 cross-jumps
  only post-reload, so two tails merge exactly when their allocation is identical.
  `Func_8016f2c` stores 1 at two sites: naming the value at both is 32 differing, at
  neither 33, and **at only one of them exact**. Do not look for a control-flow
  spelling -- make the two blocks allocate differently.
- **An argument-fill-order difference can be a CONSEQUENCE.** Half of
  `Func_80bf5a8`'s residue was a callee filling r2 before r1, the classic return-type
  signature. All four `void`/`int` combinations measured 24 differing with **identical
  pairs** -- fully inert -- and the order corrected itself when the r1/r2 allocation was
  fixed by moving one statement. Diff the pairs before spending a round on that lever.
- **A reassigned PARAMETER is a global allocno.** `fill <<= 12;` reproduces the ROM's
  `mov rHi,rArg / mov rLo,rHi / lsl / mov rHi,rLo` where a new local collapses to one
  `lsl`. A parameter's incoming value is the first assignment, so one reassignment is
  enough.
- **A HImode store target truncates a mask in the RHS.** `*p = (v & 0xffff0fff) | x;`
  through a `u16 *` emits `& 0xfff`. Look at the store's MODE, not the constant.
  Coupled: `unsigned` for `lsr` and mask preservation are two halves of one choice --
  88 differing each, 5 together.
- **`int` with a bare `return;`.** The ROM's `pop {r1} / bx r1` rather than
  `pop {r0} / bx r0` is the tell: r0 is the return register, so a declared return value
  is the only thing that makes it unavailable as the pop scratch. `return 0;` does not
  work. Worth 2 encodings on `Func_80c1ebc`, and its inverse proved
  `OvlFunc_969_200b6d0` is `void`.
- **Where a value is ASSIGNED is an axis separate from whether it is named.**
  `int id = 0x8e << 1;` before the first guard is 46; the identical assignment just
  before the call it feeds is 49.
- **A redundant pool load in the ROM means two source expressions.** `DecompressString`
  pools 0xffff twice; spelling all three sites the same way hoists it, outranks the
  reader pointer, and swaps r9/r10 across the whole function (41 differing). `- 1` at
  the one-use site closes both defects at once.
- **Do not cache a repeated read** held again, and **a park is a file-mate source**
  held again -- `Func_808b090` took its entire record layout from its twin's park,
  making six functions now landed or advanced off `src/non_matching/`.

## Three park conclusions refuted, two advanced

- `Func_80b280c` concluded it "needs a differently configured gcc rather than a
  different C" after four spellings and four flags measured identical at 53. **Its own
  closing note listed the fix as untried**, and it was exact on the first compile.
- `Func_80c1ebc`'s blocker 3 was named an allocation-order defeat. It is a register
  CLASS problem (above). All four of its blockers dissolved.
- `Func_80c1f50`'s diagnosis was right and its cure was not: struct + do/while is 30,
  identical to the raw do/while; the `goto` form is 0.
- `Func_80f6148` goes **71 differing to 20** and its "NEXT: nothing source-level" is
  refuted -- the `(u16)` cast solved residue 1 outright, and two further levers
  (masking into the destination as two statements; naming the shift, third channel
  computed between the other two) took 58 to 20. This also retires two recorded
  artefacts: `docs/elevation.md`'s "convincing false lead" and `const.sym`'s `ldrh`
  objection, which was reading a disassembly artefact -- Thumb-1 has no PC-relative
  `ldrh`.
- `OvlFunc_969_200b6d0` goes **49 to 46**, and its own prescription is now a measured
  negative. The single-exit `ret`-local restructure -- which fixed `Func_80b153c` in
  batch 274 and which that park named as next -- is 97, 97 and 59 differing in three
  forms. The reason is that the function is `void` (its `pop {r0}` proves it), so there
  is no return value for the local to carry. **The lever needs a non-`void` function.**

## Parked: six, all priced

| function | differing | class |
|---|---|---|
| `Func_80ab21c` | **2 of 101**, every register identical | sched2 PRIORITY, priced |
| `Func_80a6794` | 14 of 101, all one r8/r10 swap | `global_alloc` priority |
| `Func_808a5f8` | 26 of 113 | register allocation, structure settled |
| `Func_808b090` | 26 of 100 | `global_alloc`, via a proved dilemma |
| `Func_801fe2c` | 35 of 110 | two mutually exclusive forms, UNREACHABLE |
| `Func_80aad10` | 94 lines against 82 | **NEW CLASS** (below) |

`Func_80ab21c` is terminal and says so: at `.23.sched2` block 16, insn 291 has prio 4
and insn 167 prio 2, both ready at t=9, and **both orders cost 16 cycles**, so nothing
shortens 291's chain or lengthens 167's. Eight statement orders confirm it.

### A mutual exclusion is a result -- state it from the constraint, not the plateau

Two parks are blocked by two source forms that cannot coexist, and both are proved from
a machine constraint rather than a measurement plateau.

`Func_808b090`. The ROM's `add r3,r4,rOff / mov rZ,#0 / ldrsh rD,[r3,rZ]` needs the
address to be `(plus reg const_int)`, **invalid for Thumb HImode** -- there is no
immediate `ldrsh` -- so gcc must materialise it in one register, which is what emits
the `mov #0` and the `add`. That requires LITERAL offsets. But the base must also be in
a register or `gState + 448` folds to one pool word, and the only things that achieve
that are an explicit base variable or a REGISTER offset -- which makes the address
`(plus reg reg)`, valid, folded, and the two instructions gone.

`Func_801fe2c`. An explicit `int k` index biv gives the ROM's `ldrsb [k, p]`
addressing but makes the loop-top test and the body load syntactically identical, so
cse1 commons them. `p[0x2c + i]` kills the commoning but `.08.loop` then says
directly that loop.c folds the invariant `p` into the giv's additive term, so **`p` can
never survive as a separate base register from that spelling**. 22 candidates and seven
byte-identical expression spellings.

### New blocker class: CSE-shared expensive constants across sibling calls

`Func_80aad10` is the first specimen. Two indirect calls in one basic block pass the
same pool-sized constants (0x6004000, 0x2000, 0x5000080), and gcc shares them where the
ROM re-materialises them: +4 prologue, +4 epilogue, +4 setup `mov`s, and the function
pointer pushed out of r6 into r8 so all three sites become `bl _call_via_r8`.

`precompute_register_parameters` makes a pseudo per EXPENSIVE constant argument
(`.00.rtl`: six separate constant sets); local CSE substitutes the earlier pseudo at
the later site (`.03.cse`: `(set (reg:SI 0 r0) (reg:SI 44))` with
`REG_EQUAL (const_int 100679680)`); `.18.greg` puts them in r9/r10/r11. **A cheap 8-bit
constant is immune** because it goes straight to the hard argument register with no
pseudo, so `invalidate_for_call` kills it -- the split is on constant COST, not value.

Verified in ROM bytes rather than inferred: at `0x080aad10`--`0x080aae14` both
0x6004000 loads point at the same pool word `0x080aadd4` and 0x2000 is BUILT with
`movs #128 / lsls #6` twice.

**And isolated by a diagnostic worth copying.** With the second call site's constants
deliberately PERTURBED so nothing can be shared, the whole body lines up
instruction-for-instruction and the only residue is the perturbation itself. When one
class of defect dominates a function, break the mechanism on purpose to prove nothing
else is wrong -- that turns "93 differing" into "one known cause". Four argument
spellings are all 93, and pinning the constants is **worse** at 83, so there is no
source route and no fakematch route.

The park carries the targeting grep: before attempting a function that makes repeated
indirect calls with the same VRAM address or DMA length, look for `_call_via_rN`
alongside a repeated pool-sized literal.

## Two constraints on `tools/objcmp.py`

1. It derives cflags from the Makefile rule for the `.c` path, so it **cannot screen a
   park that needs `CSE_CFLAGS` and is not in the Makefile yet**. Worked around here
   with a seven-line wrapper monkeypatching `tryc.makefile_flags`; worth building
   properly next time.
2. It compares relocations **object-wide**, so a two-function candidate reports
   `RELOCATIONS differ` even when both functions' bytes match. The one-function-per-file
   rule is load-bearing, not advisory.

## State

**4,430 from C / 1,280 in asm** against batch 275's 4,421 / 1,289 -- exactly **+9/-9**,
the **eleventh consecutive batch** where the delta reconciles.

`census.py` TOTAL 1,278 (76 hand-asm, 14 ARM, 595 parked, 593 available). 480 park
files. `fakematch.txt` 458 rows, **unchanged** -- zero debt across nine functions, and
two of the three park closures were previously going to need scaffolding.

The 61--100 band is down to **27 available** from 35, and this was the last round that
could fill it with same-shape targets. **The 101--200 band has 232 available**, and on
this round's evidence the ones with elevated same-stem neighbours are as cheap as the
sub-100 work was. That is where the next rounds should go, selected by neighbour rather
than by size.

## Open, for the user

- **`_MSG_b24`** -- not added, and now for a better-stated reason (above). Wants the
  register measurement run against it.
- **`_TBL_7a828`** in `label.sym` and **`_SIZE_80f0024`** in `size.sym` -- both from
  batch 275, both would buy non-matches. `_SIZE_80f0024`'s arithmetic is verified and it
  is the same class as the `_SIZE_8015430` added this round, so it is the stronger of
  the two.
- **The fakematch convention question** -- 52 bare-pin files remain unregistered in
  `fakematch.txt`; 8 full-barrier files were registered in batch 273.
- **`OvlFunc_948_2009308`**'s one pin, from batch 272.
- **`OvlFunc_969_200b6d0`** wants a `CSE_CFLAGS` Makefile row to land at 46 of 47, if a
  one-instruction non-match is worth a row. It is the only thing between it and exact.
