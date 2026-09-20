# Batch 275 -- nine of sixteen, two corrections to published findings, and the zero-debt streak ends

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All nine addresses checked against the
linked ELF, along with every function the four splits left in assembly. A control symbol came
back absent.

| | |
|---|---|
| elevated | **9** |
| parked | **7** |
| new tools | 0 |
| `.sym` entries added | 0 |
| splits | 4 (two four-way, one five-way, one two-way) |
| Makefile rows added | **0** -- and that was measured, not assumed |
| **fakematch debt added** | **2** |

**Four screening subagents, four never-attempted functions each.** Nine exact, seven
best-effort. The hit rate fell from fifteen-of-sixteen to nine-of-sixteen, and the reason is
visible in the sizes: this set was 61--98 instructions with most above 84, against batch 274's
69--91. **Five of the seven misses are at 1--5 differing with size exact**, and four of those
are priced out with scheduler arithmetic rather than unswept.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `Func_80b2da8` | `0x080b2da8` | [rom_b0070_c_c_a_c_c_a_b.c](src/rom_b0000/rom_b0070_c_c_a_c_c_a_b.c) *(fakematch)* |
| 2 | `Func_80b2e30` | `0x080b2e30` | [rom_b0070_c_c_a_c_c_a_c.c](src/rom_b0000/rom_b0070_c_c_a_c_c_a_c.c) |
| 3 | `Func_80c1a34` | `0x080c1a34` | [rom_c1a34_a_a_a_a_a.c](src/rom_b5000/rom_c1a34_a_a_a_a_a.c) |
| 4 | `Func_80c1df4` | `0x080c1df4` | [rom_c1a34_a_a_a_a_c.c](src/rom_b5000/rom_c1a34_a_a_a_a_c.c) |
| 5 | `Task_BlitLuckyWheelsAnim` | `0x080f60a0` | [rom_f6008_c_a_b.c](src/rom_f6000/rom_f6008_c_a_b.c) *(fakematch)* |
| 6 | `Func_80f61e8` | `0x080f61e8` | [rom_f6008_c_a_d.c](src/rom_f6000/rom_f6008_c_a_d.c) |
| 7 | `Func_808ee0c` | `0x0808ee0c` | [rom_8d9a4_c_a_c_c_c_c_a_a_c.c](src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_a_a_c.c) |
| 8 | `Func_8099738` | `0x08099738` | [rom_97b54_a_c_c_c_c.c](src/rom_8a000/rom_97b54_a_c_c_c_c.c) |
| 9 | `OvlFunc_917_20095a0` | `0x020095a0` | [ovl_30_c_c_c_c_a_a_c_c.c](src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_c_c.c) |

`OvlFunc_917_20095a0` at **396 bytes** is the largest function landed in this run of batches,
and it needed exactly one lever.

## I published a wrong rule last batch

Batch 274's `docs/elevation.md` entry reads **"`synth_mult` CAPS AT THREE OPERATIONS"**. An
agent contradicted it with a direct measurement; I reproduced the measurement myself:

| multiplier | result |
|---|---|
| `x * 400` | **synthesised in FIVE ops** -- `lsl #1 / add / lsl #3 / add / lsl #4` |
| `x * 504` | synthesised in three |
| `x * 100` | `mov r3, #100 / mul` |
| `x * 13` | `mov r3, #13 / mul` |
| `x * 6553` | pool load + `mul` |

`400` synthesises in five while `100` -- which decomposes **the same way**, `25 << 2` against
`25 << 4` -- uses `mul`. So the decision is a cost comparison inside `synth_mult`, not an
operation budget, and it is not predictable from the operand's shape.

**Probe the specific multiplier.** One `xgcc -S` on a two-line file answers it. The section is
retitled and the correction recorded *in place* rather than edited away, because the wrong
version was published and may have been relied on. What survives is the consequence, not the
cap: `Func_8092624`'s nine-operation chain still needed its composite spelling.

## A recorded "false lead" was rejected on a disassembly artefact

`Func_80f61e8` settles the `_CONST_1f` class. A `(u16)` cast before the mask --
`(u16)((c << 16) >> 21) & 0x1f` -- makes it a **HImode** pool entry (`pool_range` 64), which is
what forces the ROM's mid-function pool and its `b`-over-pool, at **zero extra instructions**.

This document calls the `(int)&_CONST_1f` reading a convincing false lead ("right text, wrong
bytes" -- an SImode symbol has range 1020 and the pool moves to the end). And `const.sym`'s own
`_CONST_1f` entry lists `unsigned short t; (t >> 5) & 0x1f` as a near miss **"not close enough
to count", because it prints `ldrh`.**

**Batch 79's own finding settles that: Thumb-1 has no PC-relative `ldrh`, so gas assembles
`ldrh rN, .L` to the identical halfword.** The near miss was a match all along, rejected on an
artefact of how the disassembly prints. No `_CONST_1f` symbol is needed for the class, and
`src/non_matching/rom_f6000/80f6148.c` -- whose *entire* residue is this pool, and which now
sits in the `_c` piece of this batch's own five-way split -- is the obvious re-screen.

## A struct instead of a `char *` refutes a standing park

**Declaring the state block as a `struct` rather than `unsigned char *` is a
register-allocation lever.** With a char pointer plus hand-written offsets, `strength_reduce`
folds the whole address into ONE pointer giv; a typed struct member array keeps the ROM's base
register plus stepping integer offset, which costs one more callee-saved register and
reproduces the push list.

On `Func_80b2e30`, all four `unsigned char *` address spellings measured **identically at 76
differing**, as did four flags. The struct was exact.

`src/non_matching/rom_b0000/80b280c.c` concludes the class "needs a differently configured gcc
rather than a different C" -- and the fix is **that park's own closing note**, a struct
declaration listed there as "NOT tried, and worth one screen". The park is annotated in this
batch and names two further candidates (`Func_80b2b10`, and `80c1f50`'s park).

So the earlier reading holds **for the `unsigned char *` typing only**. When every expression
spelling measures identical, the variable not yet varied may be the TYPE.

## The zero-debt streak ends, at two rows, both minimised first

Batches 272--274 landed 39 functions with no scaffolding. This batch adds two, and in both
cases I reduced before accepting.

**`Func_80b2da8` -- one `volatile` read, and I overruled the agent to book it.** It reported
zero rows. Its ROM loads `*p` three times; the third is legitimate because an intervening call
clobbers memory, but the first two sit *before* the call with no write between them, so they
are provably the same value and **CSE merging them is correct**. That is exactly the blocked
sub-class this document records: *"Only `volatile` keeps both loads, and that is a fakematch."*
The recorded positive escape does not apply -- both are plain 16-bit reads feeding a mask and
an argument, so there is no use the merged value cannot satisfy.

I reduced it to **one volatile read** rather than a volatile pointer declaration, leaving the
other two loads ordinary. Measured: minimal form exact, whole-pointer exact, volatile on the
call read instead 2 differing, none 8, a union 26.

**`Task_BlitLuckyWheelsAnim` -- one pin, and the minimum AMENDS a recorded rule.**
`OvlFunc_942_20087dc`'s entry records pin-r0-alone as inert and r0+r1 as the minimum, and
generalises it to "pin the pair". Here **r0 alone is exact and r1 alone is inert** -- the
asymmetry runs the other way, so it is not a property of the class. Screen r0 alone first.

## No Makefile row, and that was measured

`src/rom_f0000/rom_f0254_a_b.c` carries `GCSE_CFLAGS`, and it is the sibling inside
`LoadGS1CreditsBG`'s own parent file. That is exactly the kind of neighbourhood inference that
gets acted on, so I asked for the measurement rather than the assumption.

**`-fno-gcse` is inert, twice.** On `LoadGS1CreditsBG` the full instruction streams were diffed
rather than the counts, across three candidates, and are byte-for-byte **identical**. On
`rom_f6008` it is 2 differing either way, and both functions there landed exact under default
flags. No row is warranted anywhere in this batch.

## Levers that each closed a function

* **A four-term `&&` range chain must be parenthesised into pairs.** Flat, `fold_truthop` folds
  only the FIRST pair; the second is left-associated against the accumulated test and
  `fold_range_test` never fires on it. `(A && B) && (C && D)` took `Func_808ee0c` from 95
  differing to 26. The failure is silent and asymmetric -- the first half of the test is right.
* **Invariant loads belong inside the loop**, so LICM drops them into the preheader, which is
  *after* the guard, where the ROM has them.
* **A byte field past Thumb's 5-bit `strb` offset wants a typed struct field**, not a computed
  pointer -- and the reason is new: the field form leaves the address computation to the
  preheader, which frees the low callee-saved registers the loop's constants need. 23
  consecutive instructions became exact on `Func_8099738`.
* **Two sequential loops share one counter.** Separate counters let cse fold the second `= 0`
  into a stored constant, costing an instruction and rotating the low registers.
* **`(x & (1 << n)) == 0` is folded** to `((x >> n) & 1) == 0`; a named `int bit` blocks it.
* **Put the early-exit constant in the textually last block** so it falls through into the
  epilogue rather than being emitted inline and branched over.
* **`DMA3_SET` and `DMA3_COPY` are not synonyms** -- the extra `"r0"` clobber produces a second
  `mov r0, rN` before a following call. 46 differing to 11.
* **`i = 0` as a statement rather than a `for`-init** was the *only* lever
  `OvlFunc_917_20095a0` needed, and it is the fourth function in two batches to turn on it.
  Worth promoting: when a loop counter and another preheader value hold each other's registers,
  try this before anything else.

## Parked

Seven, all carrying their candidate C and measurements. Five are at 1--5 differing with size
exact.

**Two name a build-input entry and deliberately do not add it**, on the batch-272 rule that an
entry is worth adding when it *completes* a function rather than improves one:

* `Func_8078144` wants `_TBL_7a828 = .L7a828;` in `label.sym`. Verified that `.L7a828` is
  already `.global`, which is that file's stated bar, so the entry would link. It also records
  a trap: `aliases.txt:537` already has `BYTE_ARRAY_0807a828`, but **`aliases.txt` is not
  `INCLUDE`d by `stage1.ld`**, so it is not a substitute. Four encodings still differ with the
  symbol.
* `LoadGS1CreditsBG` wants `_SIZE_80f0024 = 0x230` in `size.sym`. The arithmetic was verified
  here rather than inferred -- `Func_80f0024` at `0x080f0024`, `Func_80f0254` at `0x080f0254`,
  gap exactly `0x230`, with four existing `_SIZE_` entries on the same footing. Thirteen
  encodings still differ with it.

That second park **retires an open question elsewhere**:
`src/non_matching/rom_9000/8012388.c` closes with "NEXT: a way to keep a local constant
unfolded, which nothing in the notebook currently offers". Its `0x27c` is this same `size.sym`
class, not a fold problem.

**Four are priced out with the scheduler arithmetic in the park**, not unswept: `StartRain`
(priority 67 against 66, so `rank_for_schedule` never reaches the LUID tie-break),
`Func_807808c` (store priority 34 against shift 36 -- the same blocker `8077f70.c` already
proved), `Func_80286a0` (an initialiser with no in-block successor to inherit priority from),
and `Func_8078144`, whose **two residues trade against each other** at 4 and 6 with no source
order satisfying both.

`OvlFunc_969_200b6d0` is one instruction short and needs a `CSE_CFLAGS` row -- the established
flag-id constant-CSE class, confirmed pass-level with five structural spellings identical. Its
missing instruction is the `find_equiv_reg` call-result-copy class, and the single-exit
restructure that fixed `Func_80b153c`'s version was **not** among the nine spellings tried.

Two levers banked from the failures: **two pointer roles can be one source variable** (37
differing in one edit -- the other side of batch 274's "distinct call results want distinct
variables"), and **a `sub sp, #N` larger than the address-taken locals is a caller-save slot**
under `-fcall-used-r4`, so read the push list and the frame size together.

## State

```
funcindex --stats   4421 from C, 1289 still in asm     (batch 274: 4412 / 1298)
tools/census.py     TOTAL 1287  (76 hand-asm, 14 ARM, 592 parked, 605 available)
available 61-100: 35  (was 51)
matched .c files in src/ (excl. parks): 4049
park files: 477   (batch 274: 470 -- seven added)
fakematch.txt rows: 458  (batch 274: 456)
```

**+9 from C and -9 still in asm, for exactly the nine elevated** -- the TENTH consecutive batch
where the delta reconciles. Batch 264's figure remains unexplained and that entry stays open.

## Not done

* **The 61--100 band is down to 35**, and only about 20 of those are data-free. That is one
  more round of this shape, maybe two.
* Seven new parks, five of them at 1--5 differing. `OvlFunc_969_200b6d0` has a named untried
  next step; the two build-input parks need a decision, not a measurement.
* Re-screen with this batch's levers: `80b280c.c` and `Func_80b2b10` with a struct;
  `80f6148.c` with the `(u16)` cast; `8012388.c` re-read as a `size.sym` case;
  `c1ebc.c` with the `while (i < n && arr[i] != key)` search form.
* `_MSG_b24` (batch 272) still fails the shiftable convention.
* The fakematch convention question from batch 273: 52 bare-pin files unregistered.
* `InitSprites` and `rom_56cc_c_c_a.c` for the `volatile` substitution -- still the only
  debt-REMOVING lead in the log.
* **The real question is now the one flagged in batch 273 and it is due.** 570 of the 605
  unattempted are over 100 instructions, which `pickable.py` rejects as having too many
  independent residues. After the 61--100 band empties, either the method extends past 120
  instructions or large functions need piecewise verification. That is next round's decision,
  not a later one.
