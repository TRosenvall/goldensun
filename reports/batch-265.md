# Batch 265 -- twelve functions, a hand-assembly proof, and a census discrepancy

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All thirteen symbols (twelve
functions plus one `.rodata` label) checked against the linked ELF, each landing
at exactly the address its name encodes. A deliberate control symbol not in this
batch was checked alongside them and correctly came back absent, so the check
discriminates rather than always passing.

| | |
|---|---|
| elevated | **12** |
| parks retired | **9** |
| parks closed as UNELEVATABLE | 2 (with proof) |
| whole-file conversions | 6 |
| splits | 4 |
| build-input changes | 0 |
| fakematch debt added | 2 functions |
| tool defects fixed | 2 |

One agent round (six functions), then solo.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `Func_a1c6c` | `0x080a1c6c` | agent round |
| 2 | `Func_8078480` | `0x08078480` | agent round |
| 3 | `Func_80a3480` | `0x080a3480` | agent round |
| 4 | `Func_80c23c0` | `0x080c23c0` | [rom_c1a34_a_a_c_c_a_a_c.c](src/rom_b5000/rom_c1a34_a_a_c_c_a_a_c.c) |
| 5 | `Func_80c23e8` | `0x080c23e8` | same file |
| 6 | `GetEnemyAttackAnimParam` | `0x080c2410` | same file |
| 7 | `Func_80b09fc` | `0x080b09fc` | [rom_b0070_…_b.c](src/rom_b0000/rom_b0070_a_a_c_c_a_a_b.c) |
| 8 | `Func_8020150` | `0x08020150` | [rom_1fe2c_c_c_c.c](src/rom_15000/rom_1fe2c_c_c_c.c) |
| 9 | `Func_80270d8` | `0x080270d8` | [rom_23178_…_c_b.c](src/rom_15000/rom_23178_a_a_a_a_c_a_c_b.c) |
| 10 | `Func_808c2dc` | `0x0808c2dc` | [rom_8ba38_…_a_b.c](src/rom_8a000/rom_8ba38_a_a_a_c_a_c_a_b.c) |
| 11 | `Func_80198dc` | `0x080198dc` | [rom_1908c_c_a_c_c_b.c](src/rom_15000/rom_1908c_c_a_c_c_b.c) *(fakematch)* |
| 12 | `Func_8019d0c` | `0x08019d0c` | [rom_1908c_c_c_b_b.c](src/rom_15000/rom_1908c_c_c_b_b.c) *(fakematch)* |

`.L73854` at `0x08073854` landed with #8 and is verified in the ELF.

## The biggest item: two parks close as UNELEVATABLE, with proof

`rom_c0/2dd8.c` and `2df0.c` between them carried three open residues on `gfree`
and `free` -- an extra `cmp`, a `push {lr}` on a leaf, and a register rotation
with every instruction otherwise aligned. **None were ever going to yield. The
pair was hand-assembled.** The decisive fact:

    4c08 @ 0x08002dd8  ->  (0x2dd8 + 4 & ~3) + 32  =  0x2dfc
    4c02 @ 0x08002df0  ->  (0x2df0 + 4 & ~3) +  8  =  0x2dfc

**One literal pool word serves both functions.** gcc-2.96 builds its minipool per
function in `arm_reorg` and emits it with a local label inside that function --
measured on both candidates -- and never emits the assembler's `ldr rX, =sym`, so
GAS never gets the chance to merge two literals. Two functions loading `gPtrs`
give two pool words and two relocations. The ROM has one. No source text closes
that gap.

Corroborated twice: **a pushless conditional branch does not occur in this tree**
(of 4,339 generated functions, 581 are pushless, six contain any local branch,
all six an unconditional `b` hopping a pool, zero conditional); and the
neighbourhood is plainly hand-written runtime library -- `rom_1b70.s` has
`.thumb_stub` divide routines, ARM `add pc, r12, lsl #3` jump tables, and a `cos`
that **falls through into `sin`**, a second entry point into one body.

**The detector generalises and is cheap.** Scan a thumb function's ROM bytes for
`0x4800..0x4FFF`, resolve `((a+4) & ~3) + imm8*4`, flag any target at or past the
next `.thumb_func_start`. Over all 293 hand-disassembled thumb functions with a
ROM address it returns four: `gfree` plus `MPlayJumpTableCopy`, `m4aSoundVSync`
and `MP2KPlayerMain` -- the known MP2K driver. Nothing else in the pool is
affected, so it is a targeted check before a hard park, not a cull.

**One trap, worth three false positives if skipped.** Exclude pool words before
reading halfwords as code: the low half of `0x02004c00` is `0x4c00`, a valid
`ldr r4,[pc,#0]`. Reading it as code accused `Func_80f7e34`, `GetVenusDjinni` and
`Field_Whirlwind`, all three ordinary compiler output.

## Findings

**gcc-2.96 forwards a volatile object's value in a register.** `q = chain;`
straight after `chain = _chain;` emits **no load** -- there is no read of the slot
anywhere in `Func_80270d8`, and the `mov` that takes the value is emitted *before*
the `str` that writes it. The second read costs nothing while still being a
separate use for the scheduler, which is the whole lever.

**A `register` binding puts its register in the save mask, and reload then spends
it.** An uninitialised `register x __asm__("r9")` makes r9 cheap -- already in
`live_regs_mask` -- so reload hands it to an unrelated long-lived value *ahead of
r6*, out of allocation order. Worth 17 of 27 encodings, and it reads like "a
register rotation" if you only diff the numbers. Deriving the competing value
from the **volatile object** keeps r9 reserved: 17 → 4.

**An induction variable's final form is evidence about the PASS, not the
statement.** `Func_808c2dc`'s loop reads `ldrb r0,[r6] / add r6,#1` against a
descending counter -- a textbook walking pointer. Transcribing it as one is
*worse* (18 differing of 21, four bytes short). The ascending index the file-mate
uses is **exact on the first candidate**. The pointer is `strength_reduce`'s.
This qualifies the recorded walking-index lever without overturning it: that
lever says which form gcc *emits* for a given source, and **it does not invert**.

**Pool order and allocno priority can demand opposite modes for one operand.**
`local-alloc.c:1496` puts `size` in the NUMERATOR of the priority formula, so
HImode is penalised 2× against SImode. `Func_8019d0c`'s pool forces `0x3e7` to be
`*thumb_movhi_insn` (range 64) because an SImode word could never sort ahead of a
symbol loaded at address 0 -- and HImode then loses the allocation tie that the
pin has to pay for. Nineteen spellings plateau at an identical residue.

Two corrections: **a `short` local does not give you HImode** (`PROMOTE_MODE`
widens it; six spellings measured unchanged), and **`ldrh rX, .Label` assembles to
a plain `ldr`** -- GAS folds it, so a disassembly showing `ldr` from a pool label
is *not* evidence against HImode. That one cost a detour.

**sched2 owns adjacent pre-call setup.** Six spellings all measured 2 on
`Func_80270d8`. What fixed it was moving **the arithmetic**, not the pointer:
read into a plain local before the call, subtract after. `-fno-schedule-insns2`
moves it; `-fno-schedule-insns` and `-fno-peephole2` do not.

**A `.rodata` blob can be emitted from C with no split and no linker edit** --
`const int L73854[4] __asm__(".L73854") = {...}` reproduces an `.incrom` byte for
byte with both linker lines verbatim. Rehome when the blob is large or opaque;
emit from C when it is a handful of words you can read. `tryc` prints a FALSE
pool warning on such a candidate because it does not expand `.incrom`.

## Tool defects fixed

**`park_for(name)` -- the inverse of `park_subject`, and the one callers want.**
`park_subject` takes a park *path*; two rounds running a sweep called it with a
*name*, got `None` for everything, and **reported live parks as cold targets**.
Globbing the symbol does not work either -- park filenames use at least four
conventions, and a function can be parked under two at once (`Func_80198dc` had
both `80198dc.c` and `rom_198dc.c`). Run over the candidate list, the new lookup
says **8 of the top 10 are already parked**. The "blocked by" column was never a
park-existence proxy.

I hit this myself this round: I took `gfree`/`free` as cold targets, reproduced
both parks' analysis from scratch, and only found the existing files when I went
to write new ones. The cross-pool proof came out of it, so the round was not
wasted -- but the lookup is why it happened.

Second: the `force` path of that new lookup re-walked `asm/` and `src/` once per
park file, 470 full directory walks. Fixed to refresh the index once.

## OPEN: the pool figure does not reconcile, and I am not publishing a delta

Batch 264 published 1,323 remaining, corrected within the same batch to **1,325**
once the two case-spelled `.s` files became visible. Twelve functions landed since,
so the expected figure is **1,313**. Re-measured from scratch this batch, with the
cache forced:

```
tools/census.py   TOTAL 1365   (76 hand-asm, 14 ARM, 592 parked, 683 available)
funcindex --stats 5710 indexed, 4343 from C, 1367 still in asm
matched .c files in src/ (excl. parks): 3977
```

The two tools agree with each other to within 2, and disagree with the expected
figure by **about +52**. The `.thumb_func_Start` fix accounts for 2 of that, not
52. The `parked` column moved 438 → 592 in the same interval, which is the
opposite direction from the park-file count (501 → 470 → 466), so that column is
measuring something other than park files.

**Do not trust batch-over-batch pool deltas until this is audited.** My working
hypothesis is stale-cache drift: `tools/.funcindex.json` is keyed on `_newest()`,
and the documented sweep optimisation *pins* `_newest()` for the length of a run,
so a long session's figures can be computed against an index that stopped
invalidating. That is a hypothesis, not a finding -- I have not proved it, and the
numbers above are the measured ones, not a reconciliation. Two figures have
already been retired this way once; I would rather flag a third than quietly
publish a delta that looks tidy.

## Not done

* `cos` and `sin` (`rom_1b70.s`) are offered as the #1 and #8 ranked candidates
  and are **hand-written** -- `cos` falls through into `sin`, which no compiler
  emits. They are unparked, so the ranker will keep offering them. Worth a park.
* `8021390`'s `_MSG_1b` build-input question, and
  `OvlFunc_968_200c048`/`200c520` behind `200c2bc`'s floor, both still held.
* Both fakematches this batch are provably pin-dependent: for `Func_80198dc` the
  requirement is "`off` must be a global allocno", and every no-code-emitting C
  route to that is closed (four `goto`/label placements are merged by cfg
  cleanup). That is the single question to attack if anyone wants them clean.
