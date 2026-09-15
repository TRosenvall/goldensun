# Batch 266 -- five functions, five parks, and the pool figure reconciles

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All five addresses checked
against the linked ELF, each at exactly the address its name or its `.s` comment
encodes; a control symbol not in the batch was checked alongside and correctly
came back absent, so the check discriminates rather than always passing.

| | |
|---|---|
| elevated | **5** |
| parked (all SIZE EXACT) | **5** |
| cross-reference parks | 6 |
| whole-file conversions | 3 |
| splits | 2 |
| build-input changes | 0 |
| fakematch debt added | 0 |
| tool defects fixed | 1 |

Solo throughout, no agents.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `DeleteSprite` | `0x0800bdd4` | [rom_b798_c_c_c_a_b.c](src/rom_9000/rom_b798_c_c_c_a_b.c) |
| 2 | `CloseUIBox` | `0x08016418` | [rom_15e8c_a_c_c_a_a_a_a.c](src/rom_15000/rom_15e8c_a_c_c_a_a_a_a.c) |
| 3 | `Func_8091f14` | `0x08091f14` | [rom_91584_c_c_a_c_a_a_b.c](src/rom_8a000/rom_91584_c_c_a_c_a_a_b.c) |
| 4 | `Func_809233c` | `0x0809233c` | [rom_91584_…_a_a_a.c](src/rom_8a000/rom_91584_c_c_a_c_c_c_c_c_a_a_a.c) |
| 5 | `SetTextColor` | `0x0801e71c` | [rom_1de5c_c_a_b.c](src/rom_15000/rom_1de5c_c_a_b.c) |

Three of the five were **exact on the first candidate**. That is the notebook
paying for itself rather than anything clever: `DeleteSprite` needed batch 265's
"an induction variable's final form is evidence about the PASS", `Func_809233c`
needed the base/offset split, `CloseUIBox` needed the named zero.

## The pool figure reconciles

Batch 265 published measured counts and **no delta**, flagging a ~52 gap against
the expected figure and naming stale-cache drift as an unproved hypothesis. This
batch, re-measured from scratch with the cache forced:

```
funcindex --stats   4348 from C, 1362 still in asm     (batch 265: 4343 / 1367)
tools/census.py     TOTAL 1360  (76 hand-asm, 14 ARM, 594 parked, 676 available)
matched .c files in src/ (excl. parks): 3982
park files: 473
```

**+5 from C and -5 still in asm, for exactly the five functions elevated.** The
batch-over-batch delta is now internally consistent, which is what the flag
asked for. It does not retroactively explain batch 264's number, and that entry
stays open -- but the measurement path is sound and the figures above can be
trusted going forward.

## Findings

**Argument fill order also follows the CALLEE'S OWN return type.** The recorded
rule blames the *preceding* call. `Func_8091f14` is a case it does not reach:
the mismatch is on the LAST call in the function, its result unused, with no
following call to defer. Making the three preceding callees return `int` is
INERT -- all still 2 differing -- while `int Func_808b320(...)` is exact.

**A bare pool load floats above a call, and the argument order follows the load,
not the prototype.** Two specimens (`Func_80919d8`, `Func_80a8578`). In both the
ROM finishes the preceding statement, which frees a register, then loads the
constant into the register just freed. `Func_80919d8` fills r0 last at one call
and first at the next -- normally read as evidence about declarations, and here
measurably not: three return-type combinations gave 7, 7 and 6 against a
baseline of 6. **Check the pool load's position before reaching for the
prototype lever.**

**Naming a constant is not enough if gcc can still see a constant.**
`Func_80cd52c`'s ROM spends r8 on -1. `m = -1` gets rematerialised at every use
(92 bytes against 104); `m = 0; m--;` defeats the constant propagation that
makes remat look free and the value survives into a register. Batch 264's case
worked from the name alone because its value was expensive to rebuild.

**The base/offset split is not about commoning.** `Func_809233c` reads
`gState + 0x1f4` exactly once and it still folds into the pool word. Two
statements stop it either way. The same lever also applies to a base used at two
different offsets, where the symptom is different and easier to miss: gcc pools
`gState + 0x234` and derives the second with `sub r3, #64`, so the function comes
out SHORT rather than merely differently addressed.

**Assign a base where the ROM loads it.** `Func_8091f14`'s `ldr r6, =gState`
sits at a join label, not the prologue. Written at the top of the C it is live
across a call, has the longest range in the function, and local-alloc puts it
LAST -- rotating three registers. Moving one assignment down fixed the whole
rotation, 30 -> 10. **A `ldr rX, =symbol` inside a block is a statement about
where the value is BORN.**

**A stored parameter register under a guard on that parameter is a substituted
constant.** `CloseUIBox`'s else arm does `strh r7, [r5, #0x18]` where r7 is the
second parameter -- not a field assigned from the argument. The arm is reached
only when that parameter is zero and gcc substitutes the register it has.

**A pooled small constant that meets a HALFWORD is not a symbol.**
`SetTextColor`'s ROM pools `0xf`, which is the tell that normally sends you to
`const.sym`. It is that file's own documented exception: the AND feeds a `strh`,
gcc narrows it to HImode, and a HImode constant goes to the pool. A plain
`c & 0xf` reproduces it, so **no `_CONST_f` entry is warranted** -- the check
const.sym's header asks for, run rather than assumed.

## Five parks, all size exact

| function | residue | class |
|---|---|---|
| `Func_942e0` | 2 of 52 | sched2 places a bare `mov #15` early |
| `Func_80cd52c` | 36 of 48 | `check_dbra_loop`; registers are global_alloc |
| `Func_80a8578` | 11 of 60 | global_alloc (corrected, batch 267) |
| `Func_80919d8` | 6 of 56 | pool load hoisted above a call |
| `Func_8028ef0` | 20 of 73 | reload scratch choice (corrected, batch 267) |

Every one reaches the ROM's exact instruction count. `Func_8028ef0` has no
branches at all, which makes it the cleanest specimen of the tie and the worst
one to attack by spelling.

**`-fno-schedule-insns2` was measured on three of them and is WORSE on all three**
(2 -> 15, 6 -> 13, 20 -> 30). That is a result, not a dead end: it says the ROM
was built with sched2 ON and its order IS the scheduled one, so none of these is
a case for a `SCHED2_CFLAGS` rule.

`Func_80e73a0` is parked separately as a **third static-chain function**. The
transcription reproduces its prologue exactly, so the chain half is closed; what
is open is that its loop is entirely unoptimised, and every flag tried makes the
function SHORTER rather than longer.

## CORRECTED IN BATCH 267: they are NOT all one local-alloc decision

**What this section originally claimed is wrong and is left here corrected
rather than deleted.** It said `Func_942e0`, `Func_80cd52c`, `Func_80a8578` and
`Func_8028ef0` all end on a local-alloc decision and "all four report
`;; 0 regs to allocate` in `.18.greg`". That line was measured on `Func_942e0`
and assumed for the rest. Measured on all of them:

```
Func_942e0    ;; 0 regs to allocate                      <- local-alloc
Func_8028ef0  ;; 0 regs to allocate                      <- local-alloc
Func_80a8578  ;; 5 regs to allocate: 37 33 36 35 32      <- global_alloc
Func_80cd52c  ;; 7 regs to allocate: 37 41 33 32 34 35 36  <- global_alloc
Func_80919d8  ;; 5 regs to allocate: 35 34 50 32 33      <- global_alloc
```

Three of the five are **global_alloc**, so the recommendation to read
`local-alloc.c` was pointed at the wrong file for those three; global.c's
`allocno_compare` and `find_reg` are the place.

And for `Func_8028ef0` -- the one offered as the cleanest specimen -- the
differing registers **are not allocator quantities at all**. `.17.lreg` assigns
its two named values to hard regs 10 and 8, *already the ROM's registers*. The
r2/r3 exchange is argument setup plus the scratch reload picks for a pool load:
`.15.regmove` carries `(set (reg:SI 3 r3) (const_int 14))`, a hard register
chosen when the argument is materialised, and the 0x99b never becomes a pseudo
at all.

**The rule this produces is the useful part:** read `.18.greg`'s "regs to
allocate" line before calling anything a local-alloc tie, and read `.17.lreg`'s
`;; Register N in M.` lines to check whether the registers under argument are
quantities at all. Four functions were grouped on one diagnosis from one
measurement, and that is how a wrong lead gets published.

The local-alloc read was still worth doing and produced a real correction to the
notebook -- **local-alloc runs two passes and the published priority formula is
only the second**, never consulted for any quantity that has a hard-register
copy suggestion (parameters, arguments, return values). See docs/elevation.md.

## Tooling: multi-function parks hide cold targets

`Func_801b9ec` was offered as a cold candidate and re-derived from scratch before
the existing park was found: `rom_15000/801b9a8.c` says "COVERS TWO FUNCTIONS" in
its second line and names it, with the blocker already identified. Second time
this class of miss has cost a round, after `cos`/`sin`.

`park_subject()` resolves exactly ONE subject per park file. A precise search --
parks carrying an explicit "COVERS N FUNCTIONS" marker, **8 of 465** -- finds four
such functions, now each given a cross-reference park:

    Func_801b9ec          behind rom_15000/801b9a8.c
    OvlFunc_890_200901c   behind ovl_78b2ac/2008ef8.c
    OvlFunc_890_2009140   behind ovl_78b2ac/2008ef8.c
    OvlFunc_948_200949c   behind rom_7d30e0/2009838.c

**The broad version was rejected, and the numbers are why.** Matching every park
that merely NAMES a second unelevated function returns **151 of 465** -- but
almost all those names are CALLEES in prose. Acting on it would have hidden ~150
elevatable functions. Same conclusion the reverted `park_for` widening reached
from the other side: for park lookup a false positive is far more expensive than
a false negative, because it removes a function from every future candidate list
silently. `cos`/`sin` was split into one park file per function for the same
reason.

## Not done

* ~~The four-function local-alloc read~~ -- DONE in batch 267, and it corrected
  this report; see the corrected section above.
* `Func_80e73a0`'s parent is unidentified; the grep is
  `add rN, sp, #K / mov r9, rN / bl Func_80e73a0`.
* `8021390`'s `_MSG_1b` build-input question and
  `OvlFunc_968_200c048`/`200c520` behind `200c2bc`'s floor, both still held.
* Batch 264's pool figure is still unexplained; this batch's delta is consistent
  but does not settle that entry.
