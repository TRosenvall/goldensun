# Batch 273 -- sixteen unattempted functions, eleven landed, zero new pins

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All eleven addresses checked against
the linked ELF, along with every function the seven splits left in assembly, all still
at their original addresses. A control symbol came back absent.

| | |
|---|---|
| elevated | **11** |
| parked | 4 |
| banked on a build-input decision | 1 |
| parks retired | 0 (nothing attempted here was previously parked) |
| new tools | 0 |
| `.sym` entries added | 0 |
| splits | 7 |
| Makefile rows added | 0 |
| **new fakematch debt** | **0** |
| fakematch rows added for previously UNTRACKED files | 8 |

**Four screening subagents, four never-attempted functions each, no parks.** Sixteen
targets: **eleven exact, five best-effort**, and not one of the eleven needed a register
pin. Agents screened only; every result was re-verified here with `objcmp` and the full
build before landing.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `InitActors` | `0x0800c004` | [rom_c004_a.c](src/rom_9000/rom_c004_a.c) |
| 2 | `Func_80118d8` | `0x080118d8` | [rom_11568_c_c_a_a.c](src/rom_9000/rom_11568_c_c_a_a.c) |
| 3 | `ActorCmd_FollowTargetWait` | `0x0800dcdc` | [rom_d924_c_c_b.c](src/rom_9000/rom_d924_c_c_b.c) |
| 4 | `OvlFunc_888_200b098` | `0x0200b098` | [ovl_30_c_c_a_a_a_c_c_a_c_a_c.c](src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_c.c) |
| 5 | `Func_80a2324` | `0x080a2324` | [rom_a1814_c_a_a_c_a_c_a_c_c_c.c](src/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c_c_c.c) |
| 6 | `Func_809c314` | `0x0809c314` | [rom_9bb64_c_a_b.c](src/rom_8a000/rom_9bb64_c_a_b.c) |
| 7 | `Func_80b1dec` | `0x080b1dec` | [rom_b0070_a_a_c_c_c_c_b.c](src/rom_b0000/rom_b0070_a_a_c_c_c_c_b.c) |
| 8 | `Func_80b0958` | `0x080b0958` | [rom_b0070_a_a_c_c_a_a_a_b.c](src/rom_b0000/rom_b0070_a_a_c_c_a_a_a_b.c) |
| 9 | `Func_8021620` | `0x08021620` | [rom_20198_c_c_c_a_a_c_a_a_b.c](src/rom_15000/rom_20198_c_c_c_a_a_c_a_a_b.c) |
| 10 | `Func_801ea3c` | `0x0801ea3c` | [rom_1de5c_c_c_a_c_c_c_b.c](src/rom_15000/rom_1de5c_c_c_a_c_c_c_b.c) |
| 11 | `Func_8019da8` | `0x08019da8` | [rom_19d2c_a_b.c](src/rom_15000/rom_19d2c_a_b.c) |

**Eleven functions with no pins, no flags, and no `.sym` entries.** Batch 272 managed
thirteen with zero debt; this one confirms that was not a fluke of easier targets --
these were 61--76 instructions against 272's 41--60.

## The frontier moved again

Batch 272 emptied the 41--60 available band. This round took sixteen of the eighty-three
in 61--100, leaving **67**. Every function at 60 instructions or fewer is now either
elevated or parked.

Two consequences visible in the work. **Splits are now the norm**: seven this batch,
against one in batch 271. And the hit rate fell from eleven-of-twelve to eleven-of-sixteen
-- these functions have more independent residues, and four of the five misses are
genuinely close (26, 41, 45, and two instructions short) rather than far off.

## Where a value is BORN, again and again

Five of the eleven turned on it, which makes it the round's theme rather than a
coincidence.

`Func_80a2324` needed four such levers in one function: the loop bound must be inlined
rather than a variable (a variable puts `mov r10, r0` before the entry guard); the `0x48`
must sit in the INDEX, held there by a named `off = i*4 + 0x48`, because every
parenthesisation of `state + 0x48 + i*4` reassociates; and the y cursor must be a
strength-reduced giv (`y + (i - first) * 0x10`) so its stack-argument load lands in the
preheader. 44 differing to 3.

Its fifth lever is the same alias story as batch 272: the last three instructions were a
spurious store-to-spill-slot dependence, and **a typed struct killed it where source
reordering did nothing.**

`Func_809c314`'s preheader was 18 differing because the ROM keeps the player pointer in r0
so r2 stays free as reload's scratch. **Reusing the loop's own variable** -- which already
owned r0 -- fixed all eighteen. Naming the id, declaring the getter `void`, a separate
local, and moving its declaration all measured exactly 18. The variable IDENTITY is the
lever, which is "two results of one call need two variables" run in reverse.

`Func_8019da8` was two instructions off, and the `-4` sixth argument had to be a NAMED
LOCAL: as a literal gcc rematerialises it at the call site rather than paying a
callee-saved register, and the function then spends only r8--r10 where the ROM also pushes
r11. **The push list is the cheap signal** -- read the prologue before hunting the body.

## Naming a load, and ordering declarations, are different levers

Batch 272 found a priority tie broken by declaration order. `Func_80b0958` is the exact
complement: with the competing load left **anonymous**, no declaration order reached the
ROM -- five differing at best. Once it was **named**, all six permutations matched.

**The discriminator is whether the competing value is a named pseudo at all.** An
anonymous load has no allocno to tie with, so there is no tie for an order to break. Name
it first; order only if a contest remains.

Its structs came from `src/non_matching/rom_b0000/80b0a20.c` -- a **park**. Worth saying
plainly: a park is a file-mate source. It carries a candidate and its measurements even
when its own function is unsolved.

## Two control-flow mechanisms, and an escape hatch

`Func_80b110c` produced the most reusable finding of the round even though it did not
land.

**`if (c) goto L;` can never leave `L`'s code where you wrote it** when `L` has one
predecessor. `cleanup_cfg` / `try_merge_blocks` runs at the **sibling** pass -- dump
`.01.sibling`, before `.02.jump` -- and merges the intermediate `[b L]` block with `L`,
physically relocating `L`'s code to the jump site.

The cure is to put the `goto` in an **`else` arm**: `if (!c) { ... } else goto L;`.
`jump.c`'s `follow_jumps` runs unconditionally at the top of its loop, before every other
rule, and tensions the conditional straight onto `L`, so no single-successor block ever
exists. That one change laid out the entire CFG as the ROM has it, 56 differing to 53.

The recorded *"a two-instruction block reached by `goto` is DUPLICATED INLINE"* is the
same phenomenon observed from outside; this is the mechanism and the way round it. Related:
`bne FAR / b EXIT` needs `if (!c) return; else goto FAR;`, because `follow_jumps`
otherwise retargets the drop-through conditional to the exit.

And a third, from `Func_801ea3c`: **where an if/else's STORE lives decides whether `.14.ce`
fires.** `ce` if-converts arms that are single register sets and declines arms containing a
memory store. A merged variable stored after the join let `ce` hoist the else-arm load
above the branch and drop the ROM's `b`; putting the store inside each arm made `ce`
decline, and `jump2` then cross-jumped the identical `strh` tails into the join. Same
machinery as batch 271's "duplicate a shared store into both arms", reached from the other
direction, and free for the same reason.

## Loop spelling is a register-allocation lever

`Func_801ea3c`'s digit copy had to be **index-based** (`out[i + 2] = p[i]`), not a running
pointer. An index lets strength reduction build the destination pointer and keep its base
in a register, giving the ROM's `add r2, r4, #4`; a running pointer lets cse fold the
frame-address pseudo and reload emits `add r2, sp, #4`. **`add rX, r4, #imm` against
`add rX, sp, #imm` is the readable tell**, and seventeen pointer spellings plus five
`-fno-*` flags all failed on that one instruction.

The reverse appears in the `Func_80b7548` park: `.09.cse2` folds an address giv's initial
value `p + 0x64` into `base + 0x66` where the ROM keeps `base + offset-iv`, and forcing an
`ldrsh` at loop-pass time (copy through an `s32`) makes `(mem (reg))` an illegal address so
`loop.c` cannot build an address giv at all.

Also: `Func_80a2324`'s loop 1 is an **ascending** `for` despite the ROM's
`sub r6,#1 / cmp r6,#0 / bge`, which is `check_dbra_loop` reversing it -- and the control
is in the same file stem, where a sibling with `i % 5` in its body is NOT reversed.

## Pool loads: one new tell, one bound, one counter-case

**`ldr rX, =<byte << n>` IS ALWAYS A SYMBOL.** `thumb_shiftable_const` plus the `K`-constraint
split in `arm.md` mean gcc always builds a `0xff << n` value as `mov` + `lsl`; it cannot pool
a `const_int` of that form at all. `Func_80b110c` rests on this -- 0x182 is `0xc1 << 1`. That
is a structural impossibility rather than *"the ROM pooled something small"*, which is the
weaker argument every other `.sym` request in this session rests on. **Say which tell you are
using when proposing an entry.**

**A QImode literal store also goes to the pool**, because Thumb's `movqi` has no immediate
alternative and `force_const_mem` fires. That is the companion to the HImode rule, and it is
how `Func_80a602c`'s preheader came from 73 to 46 -- it makes the byte store's int VARIABLE
and the byte store's LITERAL readable apart.

**But the HImode/SImode distinction in a pool LOAD is invisible in bytes.** Verified with the
assembler: `ldrh r1, .L1` assembles to `4900`, exactly `ldr r1, [pc, #0]`. So "make the stored
constant an int variable" is not a byte-level lever on the load. What is real is the pool
POSITION, which is what batch 271's mid-function-pool finding rests on. Both stand; keep them
apart.

**And the `int`-local form has a counter-case.** On `Func_80118d8` the store is in a loop, the
`int` local becomes a fifth loop-invariant global allocno and drags r9 into the prologue -- 81
differing against the bare literal's 37 -- while a typed `short` struct field is exact. The
recorded rule needs its condition: the int local works where the store is the only consumer,
not where the constant becomes a hoistable invariant.

## `volatile` replaces a two-pin idiom, and can remove debt

`InitActors`' own callee, the already-elevated `InitSprites`, records `DMA3_SET` plus a
caller-owned zero plus **two register pins** as what produces `mov r4, sp / str r5, [r4]` --
the pins existing only to stop cse folding `*v = 0` back into `str rX, [sp]`.

Declaring the fill word `volatile u32` blocks that fold on its own and is byte-identical to
the pinned form. Ladder: two `DMA3_CLEAR` 71 of 72; `DMA3_SET` with a caller-owned zero 11;
plus two pins 5; one pin 0; **volatile and no pins 0**.

`InitSprites` itself and `src/rom_c0/rom_56cc_c_c_a.c` are the same class. Re-testing them is
a chance to take rows OUT of the scaffolding count, which is rare enough to be worth naming.

## A bookkeeping error in the fakematch figures, including my own

Chasing that `volatile` lead turned up something worse than the lead. `InitSprites` carries
three register pins and is **not in `fakematch.txt` at all.** Measured across the tree:

```
fakematch.txt names 404 files -- 371 use a pin, 116 use the empty-asm barrier,
                                 312 carry the `// fakematch` first-line marker
NOT listed but scaffolded:  64 files -- 63 use a pin, 8 use a barrier,
                                         4 carry the marker
```

Seven tools and the Makefile read one signal or the other, and batch 267 records that they
filter on **both**. So:

* the **4** marker-carrying unlisted files are still filtered by the marker -- registry drift,
  not a functional bug;
* **8 files use the full barrier idiom and carry NEITHER signal**, so they are invisible to
  every filter. Those are genuine bugs and are **registered in this batch** -- which is why
  `fakematch.txt` grew by 8 while this batch added no new scaffolding of its own;
* the remaining **52** use a bare pin with no barrier. Whether those count is a CONVENTION
  question and is left open deliberately: `docs/elevation.md` defines the fakematch idiom as
  pin PLUS barrier, but batches 269--272 registered bare pins as debt.

**The consequence for the published figures: batches 269--272 quoted `fakematch.txt` row
counts as "fakematch debt", and the real scaffolding count is higher by up to 64 files.** The
direction of each batch's delta is unaffected -- 272 added none and 273 adds none -- but the
absolute number was never the whole picture. Flagged rather than silently redefined, because
mass-registering 52 files is a decision about what the word means.

## Parked

Four best-effort results, each carrying its candidate C and every measurement behind it.
(The candidate-C check is now the last step of banking, after batch 272 landed one
comment-only.)

**`Func_80a355c` (45 of 69) and `Func_80a602c` (41 of 76, length exact) are TWINS** and each
park says so in its opening lines -- same selection byte, same `-1` reset, same glide call,
same tail. They share one unreached instruction: gcc must compute `sel * 2` into the
callee-saved join register AND use `sel` itself for a later product. `idx = sel` gets the join
right and costs a `mov` on the multiply; `idx = sel * 2` the reverse. Eleven spellings measured
identically, so it is which pseudo `regmove` coalesces with `sel`, not a spelling. From
`.18.greg` the ROM's allocation needs a reference count this source does not have, so batch
272's declaration-order lever does not apply -- confirmed, all order swaps exactly 45.
**Work them as a pair.**

**`Func_80164d4`** is two instructions short and floors at 65 of 68 across roughly 5,500
candidates, including all 1,750 topological statement orders. The park records WHY a further
order sweep cannot help: ordering `w, h` before `x, y` removes an r10 use but also lets combine
fold `(r0+ox)-(l0+ox)` and drop an instruction, so the two effects are in direct tension. The
next attempt has to change the EXPRESSIONS to make the fold unavailable.

**`Func_80b7548`** (26 of 71, length exact) is the `.09.cse2` address-giv fold above, with the
near-miss spelling named.

## State

```
funcindex --stats   4397 from C, 1313 still in asm     (batch 272: 4386 / 1324)
tools/census.py     TOTAL 1311  (76 hand-asm, 14 ARM, 582 parked, 639 available)
available 41-60: 0        available 61-100: 67  (was 83)
matched .c files in src/ (excl. parks): 4027
park files: 469   (batch 272: 464 -- five added, none retired)
fakematch.txt rows: 456  (batch 272: 448 -- all 8 from registering previously
                          untracked files, NONE from this batch's work)
```

**+11 from C and -11 still in asm, for exactly the eleven elevated** -- the EIGHTH
consecutive batch where the delta reconciles. Batch 264's figure remains unexplained and
that entry stays open.

## Not done

* Two `message.sym` requests now waiting on a decision: `_MSG_b24` (batch 272,
  `Func_80a9a5c`) and `_MSG_182` (this batch, `Func_80b110c`). The second has the much
  stronger tell -- a shiftable constant CANNOT be a pooled literal -- and would also be the
  first message-id BASE rather than an id.
* `OvlFunc_948_2009308` (batch 272) still waiting on one pin.
* The fakematch convention question above: 52 bare-pin files unregistered.
* Re-test `InitSprites` and `rom_56cc_c_c_a.c` for the `volatile` substitution -- the only
  debt-REMOVING lead in the log.
* The twins `Func_80a355c` / `Func_80a602c`, to be worked together.
* `HeightTile_A` at 2 (batch 271), blocked on `set_preference`'s operand-0 preference.
* The commoned-constant parks (four files) still fall to the batch-269 pin at four rows.
* The three global_alloc parks still want `global.c`.
