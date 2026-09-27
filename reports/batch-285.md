# Batch 285 — eleven functions, four whole files, and three loop-shape rules

Gated on a clean `make clean && make -j8 && make compare` → `goldensun.gba: OK` at
`5c4695205413df7db52b9a184815a07783999971`. All 11 landings verified at their exact ROM
addresses. `git status` clean.

| | |
|---|---|
| elevated | **11** |
| parked | **19 new** (one park retired) |
| `.sym` entries added | **1** (`_MSG_333`) |
| whole-file conversions | **4 `.s` files / 8 functions** |
| agents | 7 briefs at 5 targets each = 35 |

| brief | bank | landed | parked | unopened |
|---|---|---|---|---|
| E | `rom_b5000` | **3 + 1 bonus** | 2 | 0 |
| B | overlays 100–190 | **3** | 1 | 1 |
| C | overlays 200–270 | **2** | 2 | 1 |
| A | `rom_8a000` | **1** | 4 | 0 |
| D | `rom_a1000` | **1** | 3 | 1 |
| F | `rom_9000` | 0 | 4 | 1 |
| G | `rom_15000` | 0 | 3 | 2 |

**10 landed from targets + 1 bonus + 19 parked + 6 unopened = 35 targets accounted for**,
reconciled before publishing.

Four `.s` files converted **whole** — no split, no linker change, no data rehome:
`ovl_30_c_c_a_a.s` (2), `ovl_30_c_c_c_c_a_c.s` (1), `ovl_30_c_a_c_c_c_a_a_c.s` (2), and
`rom_b5a0c_c_a.s` (3). The other three landed by split.

**The bonus is the result worth noting.** Agent E finished its brief and then went back to
an untouched park, `Func_80bb588`, booked at 56 of 98 with "nothing recorded" reproducing
the ROM's alternating offset registers. Two edits took it to exact. **A park written and
never revisited is not evidence of difficulty** — third instance of that now, after batches
282 and 283 each landed twice-carried targets cheaply on the round they were opened.

## Three loop-shape rules, and the newest is grounded in the compiler's source

The most reusable finding of the batch produced **no landing at all** — it came from agent F,
which went 0 for 5.

> **`break` and `goto` out of a loop compile to different loop shapes.** `expand_end_loop`
> (`stmt.c:2340–2551`) scans from the loop top for a conditional jump whose target is the
> loop's `end_label` — a `break`, or a `while` condition — and **rolls** everything above the
> last such jump to the bottom. `jump.c:325` then fires `duplicate_loop_exit_test` on the
> resulting `NOTE_INSN_LOOP_BEG` plus unconditional jump and **peels** a copy of the test
> above the loop. **A `goto` to a label outside the loop is not `end_label`, so the scan
> finds nothing and no roll happens.**
>
> Therefore: **a ROM loop with its exit tests at the TOP and an unconditional `b` back-edge
> at the BOTTOM was written with `goto`, not `break`.**

**And the converse holds**, which is what makes it a rule rather than a preference: a ROM
loop that *is* rolled gets its roll **suppressed** by writing the counter exit as `break`,
because the break's jump becomes `last_test_insn` and equals `get_last_insn()`, failing the
`last_test_insn != get_last_insn()` guard. Both directions measured on the two loops of one
function.

That joins batch 284's **"do not hand-rotate a `while` loop"** and **"`while` vs an
`if`-guard is a question about the ROM — read it off whether the ROM shares or re-computes
the guard's address"**. Three rules now, all saying the same thing from different angles:
**the ROM's loop layout is a statement about which source construct was used, and it is
readable rather than guessable.** A fourth instance landed in the same batch —
`Func_80c0a24`'s fill loops must be guard + `do/while`, which is what puts the invariant
between the guard and the body.

## Alias set 0 is a scheduling barrier, and that is now the unifying statement

> **A `char *` deref has alias set 0, so it depends on every other memory access. Routing
> the same byte stores through a struct member frees the scheduler.**

22 stores through `unsigned char *u` measure 109 of 115 and come out **six instructions
short**; through `struct U *u` with `u->f132[k]` it is exact. With set 0 the stores conflict
with the `ldrh` that reloads the buffer, so sched2 gives the add the longer path. The struct
also supplies the ROM's second const-0 pseudo. Seven flags and all six declaration orders
are inert on it.

That is the **third form** of this lever in three batches — batch 284 had *promote the
pointer, not the one reference* and *an alias-set change cannot reach a combine merge*, and
this batch's agent B found the union resolution below. They are one fact:

> **The `~0xc` constant distinguishes a BITFIELD store from a HAND-MASKED BYTE store, and
> the two pull in opposite directions while the ROM has both.** The bitfield form gets the
> ROM's SImode `mov #0xd / neg` but lets CSE keep the pointer; the byte form gets the reload
> but narrows the constant to a one-instruction `mov #0xf3`.
>
> **The resolution is to put the union on the POINTER FIELD, not on the bitfield:**
> `union { struct Sub *p; unsigned char raw[4]; } f50;`. The load then has alias set 0 so
> the byte store invalidates it and the reload appears, while the store stays a bitfield
> insert. Wrapping the bitfields themselves does **not** work — gcc-2.96's union
> type-punning escape does not reach a bitfield store, and one shape silently changed the
> union's alignment so offset 9 became 0xc.

**That makes a sibling park's closing note half-wrong.** `ovl_7d0e88/2009938.c` says "the
aliasing tell does not apply here … a byte store is not it". A byte store **is** the tell;
the union has to go on the pointer being reloaded. Flagged in the new park for amendment.

## Two functions in one TU are not two independent verifications

**This is a real gap in how the tree has been checking whole-file work.** `objcmp` compiles
the whole candidate but filters **one** reference function, so two green `--func` runs on two
separate candidate files say nothing about the TU you actually ship.

`OvlFunc_932_200b738` matched **alone** and went to **43 differing** when pasted into the
combined file, because one statement had moved — which swaps two values between r8 and r10
and renames everything downstream. **Re-verify each function from the combined file before
declaring a whole-file conversion**, and keep the single-function candidates as the ladder's
record. Two agents independently wrote whole-file harnesses this batch; that capability
belongs in `tools/`.

Relatedly, a fifth measurement-tool artefact: **`objcmp`'s size+encodings check is blind to
section-tail alignment fill.** It reported "376 bytes … identical" on an object whose last
two bytes were `46c0` against the reference's `0000`. Not a candidate defect —
`Makefile:137` appends `printf '\n\t.text\n\t.align\t2, 0\n'` to gcc's `.s` because gcc
zero-fills *between* functions but not after the last one. **Any ad-hoc harness must
replicate that append**, or it reports a phantom 2-byte difference on every TU whose final
function is 2 mod 4 in length.

## `_MSG_333 = 0x0333`, on base-arithmetic evidence

```c
__Func_801e7c0(id + (int)&_MSG_333, box, 0x78, 0);
id += (int)&_MSG_53a;
```

`cse.c:1637`'s `use_related_value` fires only for `CONST`, never a bare `CONST_INT`, so when
a ROM reaches ids by register arithmetic off a base, **no plain-literal spelling can produce
it.** In-function control is decisive: `_MSG_53a` was already admitted and is used three
lines away in the same arithmetic at the **adjacent pool word**, while `0x500`, `0x3ff`,
`0x800`, `0x200`, `0x10f`, `0x10d` and `0x100` in the same function all reproduce as plain
literals. Declining `0x333` while keeping `0x53a` would be incoherent — the `_AREA_32`
argument from batch 281. **And it completes the whole `.s`.**

**The pair is decisive rather than either half**: literals for both give 4 differing,
`_MSG_53a` alone 26, `_MSG_333` alone 24, both together 0 — the combination-sweep lesson
from batch 284's `Func_80bfba4`. Verified with `nm` that it resolves **exactly once**, so
the line is genuinely exercised.

**One symbol reported and withheld**: `_SIZE_80b5138 = 0x230`. Strong control — the ROM has
`ldr r5,=0x230` and builds the DMA control word at runtime, where the literal lets gcc build
`0x8c << 2` and fold the whole word to a pooled `=0x8400008c`, structurally wrong; the
arithmetic checks out (`Func_80b5138` to `Func_80b5368` is exactly 0x230, the ARM routine the
function DMA-copies). **Withheld because it does not complete its function**, which is
`size.sym`'s own rule.

## Levers

- **A `bl` THAT APPEARS TWICE IN THE ROM'S RELOCATION LIST AT ADJACENT ADDRESSES MEANS ONE
  SHARED `goto` TAIL, NOT TWO INLINE COPIES.** 170 lines / 121 differing → 169 / 48 **with
  size and relocations exact**. **The relocation list is a cheap structural oracle for tail
  sharing**, readable before you look at a single encoding.
- **A SWAP SPELLED TWICE IS CROSS-JUMPED, AND THAT IS THE ROM'S SHAPE** — indices computed in
  the arms with one swap after the join is 43 differing; duplicating the swap is 5. Same
  family as batch 284's "write a shared exit tail twice", from the opposite direction.
- **A `case` ARM THAT DOES NOT FALL THROUGH IS EMITTED WHERE SOURCE ORDER PUTS IT; TO GET IT
  AFTER THE SHARED TAIL, MAKE ITS BODY A BARE `goto`.** `expand_end_case` `reorder_insns` the
  dispatch to the front then lays bodies out in source order, so no case permutation fixes
  it. **A switch arm appearing after the after-switch code in the ROM is a `goto`, not a
  `return`.**
- **A HImode CONSTANT STORE CANNOT BE CHAINED BY `reload_cse_move2add`** — reload rewrites it
  as `(set (subreg:SI (reg:HI)) (const_int))` and move2add skips a SUBREG destination, so gcc
  falls back to the pool and odd values are pool-only. **Routing the value through an `int`**
  makes it a plain SImode set and the ROM's whole `mov / lsl / sub / add / sub` chain falls
  out. 191 → 185 instructions, 149 → 139 differing.
- **AND move2add SOMETIMES MUST BE DEFEATED**: one entry derived `-0x1f` as
  `mov r3,#0 / sub r3,#0x1f`, **one instruction shorter** than the ROM's
  `mov r2,#0x1f / neg r2,r2`, because a zero had landed in the same register. Batch 284 used
  move2add forward; this is the first measured case of needing it lengthened. Three
  appearances in two batches.
- **THE `extendqisi2`-AT-EXPAND LEVER HOLDS FOR `extendhisi2`** — a separate walking pointer
  for an offset-2 field makes reload manufacture a zero register and emit `ldrsh [r1,r6]`
  with the add in the preheader, where `e->id` gives base+const. Batch 284 established this
  for signed char; it is now general to sign-extending loads.
- **A SHARED INDEX ACROSS TWO UNRELATED LOOPS IS EVIDENCE OF ONE VARIABLE, AND THE ROM'S
  WORSE CODE IS THE EVIDENCE.** Two separate locals came out **eight instructions short**;
  the ROM spends `mov r2,r8` twice per iteration, and that is what says the original had one
  variable. **Its counterpart is in the same file**, where separate locals are *required* and
  sharing costs 151 against 145 — the greg dump shows one pseudo live in two disjoint block
  ranges, which spills a parameter. Both directions, one file, one commit.
- **A LOOP-INVARIANT CONSTANT IS HOISTED ONLY ONCE cse HAS MERGED ITS TWO USES**, and
  `loop.c:1803` gives the threshold. The lever is `m->lifetime`: **write the two uses adjacent
  in source order and the hoist stops.** 110 insns / 107 differing → 102 / 20.
- **`int mask = -13;` AS A SHARED NAMED LOCAL, NOT THE LITERAL** — 135 differing. **A
  `mov`+`neg` pair where a byte mask would fit in one `mov` is the tell**, and without it the
  function comes out two instructions short, so the count disagreeing is itself the signal.
- **A BLOCK WITH EXACTLY THREE local-alloc QUANTITIES GIVES dst = r3; FOUR OR MORE GIVES
  dst = r2**, read out of `.17.lreg` and **verified by construction** — adding one unrelated
  call to one arm flips that site and leaves its sibling alone. It predicts all six divergent
  sites on one function.
- **`int g;` ASSIGNED IN BOTH ARMS stops being a local-alloc quantity at all.** Two separate
  locals, or naming only one site, are both inert — **the sharing is the lever, not the
  naming.**
- **FOUR LEAF ARMS ALL ASSIGNING THE SAME VARIABLE** let cross-jumping put one store at the
  join; a shared temp with one assignment after does not. 110 → 13.
- **DECLARATION-INITIALISER FORM IS NOT THE SAME AS AN ASSIGNMENT**: `int x = 0, y = 0;` read
  17 where `x = 0; y = 0;` read 6.
- **A `do/while` WITH AN `(int)`-CAST POINTER COMPARE** reproduces a clear loop with no entry
  guard and a *signed* back edge. The counter form does not merely miss the loop — **it moves
  the whole function's allocation.**
- **THE PRIORITY FORMULA IS A READOUT, NOT A GUESS**: the `-da` `.17.lreg` dump prints both
  terms of `floor_log2(n_refs) * n_refs / live_length`, and on three functions it reproduced
  the printed `;; N regs to allocate:` order **exactly**, ties included. Check the arithmetic
  before writing a spelling.
- **A non-void return type is what puts `pop {r1}` in the epilogue** — three functions return
  `int` with no `return` statement, and `void` costs exactly 2 encodings each.

## The measured negative that should change how I write briefs

**Declaration-order permutation was completely inert across 180 compiles** — 60 permutations
on each of three functions, every one byte-identical. I have been leading every brief with
that lever.

**Declaration order is a SLOT lever and does nothing when the locals are all
register-resident**, because there is no stack slot to reorder. The residues it was swept
against were callee-saved **role rotations at equal register count**, which live in
`global_alloc`'s `allocno_compare` — a pass that only tiebreaks on allocno number *after* the
priority compare, so names and order cannot reach it. A next attempt must change **reference
counts or live lengths**.

It belongs *after* the residue has been shown to involve a stack slot, not before. Two
further scoping facts from the same work: **frame-slot order for address-taken locals is the
order the addresses are TAKEN** (earlier means a higher offset), and **`volatile` changes
*when* a scalar's slot is assigned** — at `expand_decl` like an array, rather than at
`put_var_into_stack`.

## Corrections I owe the log

**I made the same transcription error three times in one batch.** I kept writing park headers
as *"N encodings of N by count but K instructions short"*, which `parkcheck` parses as a claim
of N — and the bodies measured 63, 167, 119 and 47, not 76, 160, 125 and 209. **`parkcheck`
caught all of them before commit**, which is the tool doing exactly its job one batch after
being built, and the first time this defect has been caught by the tool rather than by the
next reader paying for it (batches 282 and 283 were both found by agents re-measuring).

The **47** matters: `Func_80c0a24` is one of the closer parks in its bank and my phrasing
would have buried that. **The fix is to stop using that phrasing** — state the measured
difference count first, then the length disagreement separately.

**The brief I wrote for the 200-instruction band named the wrong dominant blocker for the
second batch running.** Batch 284 corrected batch 280's pool-constant-CSE claim to argument
precompute; this batch's agent reported that **neither** was the blocker on any of its five,
and that reading the landed neighbour was worth more than any lever. **These cutscene and
menu functions come in families across overlays — grep `src/` for the callee set before
writing.**

**A landed file's recorded lever was refuted.** The twin of one target records `goto top` as
load-bearing against `for (;;)`; on both functions this batch it is **exactly inert**.

**An agent deleted two untracked gcc dump files from the repo root that were not its own**
(`t1g.c.17.lreg`, `t1g.c.18.greg`), self-reporting it. They are regenerable and the tree is
unaffected, but **the brief should say to leave unknown files alone.**

## State

**4,558 from C / 1,152 in asm** against batch 284's 4,547 / 1,163 — exactly **+11/−11**, the
**twentieth consecutive reconciling batch**. `census.py` TOTAL 1152 agrees (76 hand-asm, 14
ARM, 545 parked). **554 park files**, `fakematch.txt` **521 rows**.

**517 available** — 52 in 61–100, 181 in 101–200, 122 in 201–400, 87 in 401–800, **69 above
800**.

**`pickable.py` is down to 3 candidates from 18.** The ≤120-instruction high-probability band
is effectively exhausted; from here the work is genuinely in 101–400 and above.

## Open

- **Four parks are 12 encodings or fewer**, three of them with size, count *and* relocations
  exact: `OvlFunc_947_2009aa8` at **2 of 152**, `Func_801776c` at **6 of 137**,
  `OvlFunc_common1_1078` at **7 of 217**, `OvlFunc_924_2009164` at **12 of 205** — that last
  one **alone in its `.s`**, so 12 encodings stand between it and another whole-file
  conversion. `Func_80ba978` still holds at **1 of 273** and converts whole outright.
- **`OvlFunc_930_20091b0` is still EXACT and still blocked on three decisions that are not
  mine**: an `ALIAS_CFLAGS` row, `_AREA_58 = 0x58`, and a 1,984-byte rehome.
- **A whole-file comparison harness belongs in `tools/`** — two agents wrote one
  independently this batch, and `objcmp` structurally cannot do it.
- **Four stale parks to retire**, subjects already elevated: `Func_80f6038`, `Func_80f4100`,
  `Func_80a22f4`, `OvlFunc_881_2009888`.
- **Two `rom_9000` text/data splits are fully specified and unblocked** — one `.rodata` each,
  one exported label each, data after every function, and the pattern already exists in that
  bank as four zero-function rodata objects with `stage1.ld` lines in place.
- `_SIZE_80b5138` withheld; `_FILE_e4`, `_FILE_e5`, `_MSG_cc3`, `_MSG_828`, six `_MSG_*` for
  `Func_801c49c`, plus batch 280's three. Long-standing: `_MSG_2080`, `_SIZE_80f0024`,
  `_TBL_7a828`.
- **Backfill `Verify with:` recipes into the UNCHECKABLE parks.** All 19 new parks carry one;
  `parkcheck` is at **69 OK / 0 MISMATCH / 485 UNCHECKABLE** (was 51).
- `-fcall-saved-r4` for `OvlFunc_970_2008f80`, argued **against** by measurement;
  `ALIAS_CFLAGS` for `OvlFunc_882_200c41c`; a `DMA3_SET` `"r2"`-clobber variant for `dma.h`.
- The fakematch convention question, ~20 of 181; `OvlFunc_948_2009308`'s one pin, batch 272.
