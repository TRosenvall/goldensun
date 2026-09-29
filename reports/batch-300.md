# Batch 300 — the 201–400 band is closed, and three of our own claims were wrong

Ten agents, 29 targets — every remaining unattempted function in the 201–400 instruction band.
Gate green at `5c4695205413df7db52b9a184815a07783999971` throughout.

**8 landed, 22 newly parked, 3 existing parks improved.**
State: **4,758 of 5,710 elevated (83.3%)** — 135 available, 725 parked, 76 hand-written asm,
14 ARM, 1 unmatchable.

Reconciliation, exact from git: 8 landed + 22 newly parked = **30 functions left the available
pool** (165 → 135), and parked rose by exactly 22 (703 → 725). **`census --list 201 400` now
reports 0 available.**

## Landed

| function | encodings | shims |
|---|---|---|
| `Anim_Flare` | 290 | 0 |
| `Anim_Confuse` | 323 | 0 |
| `Anim_Hail` | 422 | 0 |
| `Func_80a35f8` | 308 | 0 |
| `Func_80a8114` | 385 | 0 |
| `OvlFunc_968_200c048` | 290 | 0 |
| `OvlFunc_968_200c520` | 93 | 0 |
| `OvlFunc_896_200bf24` | 353 | 7 pins |

Seven of eight pin-free. Two of them — `200c048` and `200c520` — were **byte-exact since batch
250** and had never been landed; see below.

## The bookkeeping failure worth more than any single landing

`OvlFunc_968_200c048` was handed to a brief **as an unattempted target**. It had been byte-exact
for fifty batches, sitting in a gitignored scratch directory, named in a sibling park's header,
alongside a second matched function in the same file.

That park was wrong in **three independent ways at once**:

- `census.py` counted both matched functions as available, because neither had a `.c`;
- the park had **no `Verify with:` recipe**, so `parkcheck` never examined it;
- its **body was not the candidate its header described** — the header claimed 2 of 271 while the
  body measured **271**, with the real 2-of-271 candidate unused beside it.

Adding a recipe made `parkcheck` catch the mismatch immediately. All three functions are now
resolved: the file is split three ways, two are landed, the park carries its real body.

**Rule: never leave a matched candidate in scratch.** It is one `rm -rf` from gone and counted as
unattempted by every tool meanwhile.

## Three of our own claims corrected

- **The `REG_EQUIV` dominating-block lever is retired for single-block constants.** It is a
  *global*-alloc lever; `local-alloc.c`'s comment says outright *"does not affect the priority in
  local-alloc!"*. Four placements inert; the tie breaks on qty **number**, so RTL creation order
  decides and no local reaches it.
- **The named-constant-base lever is bounded from both sides.** Same basic block with ≥2 uses
  needs **no flag** — `cprop_insn` gates on `oprs_not_set_p` before availability and a call does
  not end a block (294 → 52). Straddling a join, cprop folds it and **`-fno-gcse` is not the
  remedy** (354 of 385, first difference at index 7).
- **The shiftability tell needs a MODE.** gcc-2.96 Thumb always splits a shiftable **SImode**
  constant into `mov`+`lsl` and always **pools** an HImode one — and `ldrh <pcrel>` assembles to
  the same encoding as `ldr [pc,#x]`, so the disassembly cannot tell you which. **Pooled +
  shiftable + SImode** implies a symbol; **pooled + shiftable + HImode** implies nothing.

## A defect in the authority tool

objcmp's `--func` filters only the **reference** and compares the whole candidate object — which
is why the rule is one function per candidate file. A **GNU C nested function** breaks that
invariant silently, because gcc emits the inner function as `name.0`, so the object holds two
symbols. On `Func_80bd424` that read **431 of 417** where the per-symbol truth was **402 against
417**: a plausible, authoritative-looking, wrong number.

`objcmp` now prints a loud warning naming the extra symbols. Verified to fire on the nested case
and stay silent on ordinary candidates.

## GNU C nested functions are in this ROM

`Func_80bd3e4` is nested inside `Func_80bd424`, and written nested it is **byte-identical, 32 of
32** — where its park had concluded the residue was *"not reachable, nothing in the source to
recover it from"*. Retracted.

**The tell is r9.** Thumb's `STATIC_CHAIN_REGNUM` is r9, so the caller does
`add r3, sp, #0x1c / mov r9, r3 / bl`, and the "dead" `str r3,[sp]` the park could not explain
**is the chain slot** — the frame exists to hold it. Landing constraint new to the tree: the pair
must share one object, so a text cut may not be placed between them.

A distinct case to avoid confusing with it: `Func_80f07f0` also reserves a frame it barely
addresses, but with **no** r9 read and no nested callee. An unexplained frame alone does not imply
the nested explanation.

## Measurement findings

- **An inert spelling is untested, not disproved.** An operand flip measured completely inert
  three rounds earlier and **closed the function** on the corrected base. A park's inert list is
  evidence about that park's base — so an inert entry must record the figure it was measured at.
- **A closer SIZE can be a wrong program.** `s8` is `char`, and `char` is unsigned on ARM; the
  buggy unsigned spelling hit the reference's size *exactly* while the correct program did not.
  Third instance of this shape, after an odd-offset union and a count-matching pin.
- **A whole-function low-register rotation is ONE missing short-lived quantity** — 15 hunks
  collapsed together when the first was fixed. The discriminator against a genuine allocation tie:
  a missing quantity changes the instruction COUNT, a tie does not.
- **Three park drafts stated their `aligncmp` figure in the claim line** parkcheck re-measures with
  objcmp, tripping MISMATCH on install each time (63/199, 29/301, 2/271). The claim line is
  objcmp's by definition.
- **A 400-file sample of a 4,339-file corpus is not a corpus check** — an agent's own mid-batch
  claim, disproved by its full sweep.
- **A source spelling can flip with a flag**: production wanted add-first, `-fno-gcse` wanted
  subtract-first, and the latter measures worse under production. A flag figure measured on the
  production-best spelling understates the flag.

## Levers

- **Pointers split, counters unify — and they are converse.** Two functions in the same bank and
  family wanted opposite treatment (42 → 23 by splitting pointers; 56 → 22 by unifying counters).
  Neither is predictable from the family.
- **An embedded assignment inside an argument is a pool-ORDER lever.** Argument 2 expands first and
  takes the first `ldr`; hoisting the assignment reverses the pool, worth 16 of 290 — a relocation
  difference `tryc` cannot see.
- **A preheader transposition between a pool load and a hoisted constant** means a source pointer
  init where the ROM has a giv; index rather than walk.
- **When the ROM re-derives an offset, the second site must differ in its RHS**, not its
  destination.
- **A dead mask interleaved between two inserts** reaches what `volatile` reaches, pin-free — a
  candidate answer to another park's open question.
- **Jump-table arm BODIES are emitted in source order** (this ROM's is 0, 1, 3, 2, 4).
- **Two stack-slot values in two live registers means two named locals; one register reused means
  literals** — read the register count at the `str [sp]` pair, not the values.
- **Inner-scope declaration orders spill slots**, because gcc assigns them in pseudo-number order
  and pseudo numbers follow per-block declaration expansion.
- **A twin can be found by CALLEE FINGERPRINT** where `dupfuncs.py` cannot: grepping for a
  distinctive *set* of callees paired two functions differing by one line in 378, and the fix found
  on one took the other from 16 to 14.
- **Pointer bumps in a `for`-increment versus the body is a correctness question** — as body
  statements they are a bug when a `continue` target sits above them, and the instruction count is
  what caught it.

## Decisions

- **`-ffixed-r7` on `rom_f2028_c_a.o`: NO.** A park requests the test; the ROM's prologue saves r7
  *to reach* r8, and under the flag the source saves three callee-saved values where the ROM saves
  four (180 instructions against 202). Both functions build from that object.
- **`DMA3_COPY16_RW` promoted to `include/dma.h`**, on the standing note left with it — a second
  function needed it. Gate proves the move byte-neutral. `DMA3_COPY16` itself untouched.
- **`_SIZE_80155d0 = 0x318` admitted** (criterion 1 in its strong form plus the DMA-word
  criterion); **`_FILE_18` and `_SIZE_8009bb8` withheld** — the latter now has two independent
  users, which is new evidence, but it completes neither function.

## Open

- **`REG_ALLOC_ORDER` is the most-cited open question in the tree: 78 park files name it**, and
  three briefs this batch independently concluded their residue was that class. Two facts are
  measured: a **pin is the wrong instrument** (hard register, bigger frame, worse score), and
  **hand-writing the ROM's coalescings is worse still** (it deletes the competing allocnos instead
  of raising pressure). The decisive experiment is a gcc-2.96 rebuild with the order starting at 4;
  `Func_80f62b8` is the cheapest instrument and carries a falsifiable prediction.
- **The 201–400 band is exhausted.** What remains available is 135 functions, **~74 at 401–800 and
  ~62 at 800+**.
- Against that, the park queue is deep and well characterised: **~70 parks at ≤10 differing**,
  several at 1 or 2 with size and count exact.
- ~30 frontier parks still carry stale or missing `Verify with:` recipes; note `parkcheck` reads
  only the **first** comment block.
- Seven reference `@` prose comments were wrong about their own function across three batches.
  "Named means understood" does not hold.
