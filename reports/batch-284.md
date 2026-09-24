# Batch 284 — twenty-one functions, and a tool that had been blaming the corpus

Gated on a clean `make clean && make -j8 && make compare` → `goldensun.gba: OK` at
`5c4695205413df7db52b9a184815a07783999971`. All 21 landings verified at their exact ROM
addresses. `git status` clean.

| | |
|---|---|
| elevated | **21** (best of the session; previous high 19) |
| parked | **5 new + 3 advanced + 1 confirmed** |
| `.sym` entries added | **0** |
| layout changes | 6 function splits, 2 data rehomes, 12 `.global` exports |
| agents | 7 briefs at 5 targets each = 35 |

| brief | bank | landed | other |
|---|---|---|---|
| A | ≤120 band (`pickable`) | **5 / 5** | — |
| F | overlays 200–220 | **5 / 5** | — |
| G | `rom_8a000` map-actor | **5 / 5** | — |
| B | `rom_b0000` / `rom_b5000` | **4 / 5** | 1 new park |
| C | `rom_15000` menus | 1 / 5 | 3 new parks, 1 confirmed |
| D | `rom_c9000` animations | 1 | Tackle 12→**8**, 1 new park, 2 unopened |
| E | the five closest parks | 0 | 2 advanced, 3 held with mechanisms |

**21 + 5 + 3 + 1 + 2 unopened + 3 held = 35.** Reconciled before publishing, which is the
check batch 281 skipped.

The three briefs that went 5-for-5 were the three aimed at **unattempted functions in banks
with a landed sibling to read**. The brief aimed at the five closest parks landed nothing —
and that is the expected shape, not a failure: a park is parked because it already resisted.

## The headline is a tool that was blaming the corpus for a missing compiler

`objcmp.py` needs `/opt/gcc296/xgcc`, which exists only inside the container. On the host it
dies with `FileNotFoundError` and writes nothing, and `parkcheck.py` read that empty output as
*"objcmp produced no encoding line"* — reporting **UNCHECKABLE for all 531 parks**.

So the tool built to stop parks making unverifiable claims was itself making an unverifiable
claim, in bulk, about every one of them. **A blanket failure is indistinguishable from a
finding** — `UNCHECKABLE: 531` reads exactly like the real result it is not, and an agent lost
time reasonably concluding the corpus was unverifiable.

Fixed two ways: `main()` now refuses to run without the compiler and prints the container
invocation; `check()` inspects `returncode` and `stderr` and files a per-park failure as
**TOOLING**, kept out of the park-defect buckets. **It earned that the same hour** — my own new
park header nested `/* … */` inside a comment block and broke the compile, and TOOLING caught
it without calling it a park defect.

**This does not retract batch 283's figure.** In-container the numbers are **46 OK, 0 MISMATCH,
485 UNCHECKABLE** — 46 parks carry a verify recipe and 485 do not, exactly as published. What
was wrong was only the host behaviour. After this batch's park work: **51 OK, 0 MISMATCH.**

## And `census.py` was counting a cited callee as parked

Found because **`pickable.py` and `census.py` disagreed about seven functions** while I was
building the target list. `survey()` tested `name in parks` against the concatenated text of
every park, so a function counted as parked whenever any *other* park merely mentioned it:

```
extern void gfree(int tag);                                     in 38 parks
extern void Func_80d6888(int id, int a, int b, int c, int d);    in 14 parks
"Anim_Haunt is function 4 of 5; Anim_PlanetDiver is 3."
".rodata belongs to Anim_Atalanta / BaseAnim_Breath, not to this function."
```

**Over 150 functions nobody had ever attempted were being reported as already tried**, and I had
quoted that figure repeatedly. Before: 677 parked / 417 available. After: **522 / 572** at the
time of the fix. The total never moved — 1,184 in asm either way — but the split moves in the
helpful direction, since an unattempted function has no known blocker.

The fix is a **union of four signals**, because both earlier versions were wrong in opposite
directions: filename matching alone found 464 (undercount, which is why substring matching was
adopted), substring alone gives 677. Now: functions *defined* in park code with comments
stripped, plus the subject named in the header's first two lines, plus every name in a class
park, plus the park filename. **The filename signal is load-bearing** — it is the only one that
sees a park whose header opens with a batch note instead of a subject line.

Verified two ways, both now standing checks in the file: ten disputed names sampled at random
(all citations, none a park), and an **orphan-park invariant** — every park file should claim at
least one function, and only 11 of 531 do not. `TOTAL` still agrees with `funcindex`.

That invariant found a separate defect: **four stale parks whose subject has already been
elevated** — `Func_80f6038`, `Func_80f4100`, `Func_80a22f4`, `OvlFunc_881_2009888` — which
should have been retired at landing. Not yet fixed; retiring a park is a content change.

**Two tools disagreeing is the cheapest bug detector this project has.** It cost nothing to
notice and it is the reason both of the above were found in one batch.

## Levers

- **A DECLARED LOCAL CAN COST THE REGISTER THE ROM GIVES IT.** `OvlFunc_928_2008408` sat at 11
  of 117 — a pure r6/r7 exchange — through six spellings including pins. It went exact when
  **both pointer locals were deleted** and the address written inline, letting gcc's own CSE
  make the pseudos: a CSE pseudo gets a higher pseudo number than any source local and
  `global_alloc` ties on allocno number, so the declared local was allocated ahead of the
  parameter and took its register. **The inverse of "name the pointer": when the residue is a
  callee-saved exchange and spellings are inert, try REMOVING a local.**
- **WRITE A SHARED EXIT TAIL TWICE SO gcc CROSS-JUMPS IT** — that is how you control block order
  when `goto` cannot. Written once behind a shared label, gcc makes the after-loop block the
  goto's fall-through and shoves the tail past the epilogue; **179 differing, and five source
  shapes plus five flags were all inert.** Writing it in both paths gives the ROM's layout.
  **A ROM label with two predecessors just above a shared tail is the tell the original had the
  tail twice.**
- **A JUMP-TABLE SWITCH THAT IS NEARLY RIGHT MAY ONLY BE MIS-ORDERED.** gcc emits case bodies in
  *source* order, so the ROM's block addresses plus its jump table recover the order the
  original file was written in. `Func_80b9b30`'s cases run 1, 2, 5, 9, 3, 6, 8, 4, 7 — written
  1..9 it is **115 of 215 differing from the permutation alone**, and the ROM's cross-jump then
  falls out free. Cheap to test before any lever.
- **A CALLEE-SAVED REGISTER FOR A VALUE THAT CROSSES NO CALL MEANS ONE VARIABLE WITH TWO
  DISJOINT LIVE RANGES.** On `MapActor_Surprise` the entry `mov r6,#0`, a store after
  `_DeleteActor`, and a sprite-pointer load in the tail are all the same source variable. That
  explains both puzzles at once — why a zero survives to the top instead of being
  const-propagated, and why the pointer is in r6 rather than r0. **85 → 0 in one edit.**
  Before blaming allocation, check whether the ROM's callee-saved choice is explained by a call
  on a path you have not modelled.
- **`sym[i]` AND A WALKING POINTER ARE DIFFERENT CODE FOR A SIGNED-CHAR LOAD**, worth 161 → 4.
  gcc-2.96's thumb `extendqisi2` chooses at *expand* time off the MEM shape: a walking pointer
  makes reload manufacture a zero register and emit `ldrsb rd,[rn,rm]`, while
  `(plus symbol_ref reg)` is not `ldrsb`-able so `ldrb / lsl / asr` is emitted **and survives**
  strength reduction. Every arithmetic dodge fails — combine folds it back to a `sign_extend`.
  **And the index must be used twice or `loop.c` deletes the counter** (158 → 4).
- **ARGUMENT PRECOMPUTE, NOT POOL-CONSTANT CSE, WAS THE DOMINANT BLOCKER AT 200 INSTRUCTIONS** —
  the brief said otherwise and was wrong. `OvlFunc_916_2008194` went **39 → EXACT** on writing
  each 6-argument call's two stack values as block-scoped locals. Left as literals gcc computes
  one, stores it, and **re-uses the same register** for the second; the ROM holds both.
  **The tell is a stack slot filled from the register just stored to the slot before it** — and
  "name the stack arguments" is not about eleven-argument calls, which is how batch 280 recorded
  it. Six is enough.
- **WHICH STATEMENT IS WRITTEN FIRST INSIDE EACH ARM SETS THE `local-alloc` QUANTITY ORDER, AND
  THAT IS WHAT DEFEATS CROSS-JUMPING.** Tail-identical arms get merged unless the accumulator
  lands in a different hard register per arm; writing the byte load before the mask in one arm
  and the reverse in the other reproduces the ROM's role swap. 10 → 0, transferring to a second
  function unchanged. **Distinct variables per arm are required.**
- **THE DECLARATION-ORDER RULE EXTENDS TO STATEMENT ORDER FOR CONSTANTS AND COUNTERS.** On
  `ActorMessage` all 24 declaration permutations did nothing and moving one assignment above a
  call did everything (5 → 0). On `Func_8095680` hoisting `i = 8;` out of the `for` header put
  two constants in one hard register, letting **`reload_cse_move2add`** rewrite
  `mov #0xfa / lsl #1` as `sub r2, #0x42` — one instruction shorter, 64 → 0. **When the ROM
  derives one constant from a nearby one, get both into the same hard register.**
- **LOCAL DECLARATION ORDER MAPS TO SLOTS IN REVERSE**, and backwards is easy to misread as
  noise: a 28-byte buffer at sp+0 and a 4-byte pair at sp+0x1c means the first-declared object
  takes the *higher* address. Wrong way round is 2 of 215, a pure addressing-mode difference.
- **`volatile` CHANGES *WHEN* A SCALAR'S SLOT IS ASSIGNED** — at `expand_decl` like an array,
  rather than at `put_var_into_stack` when its address is taken. A real refinement: the
  declaration-order rule has been stated as if slot-assignment time were fixed.
- **A HALF-APPLIED ALIAS-SET CHANGE OVERSHOOTS — PROMOTE THE POINTER, NOT THE ONE REFERENCE.**
  Casting just the store freed a blocked load but let sched2 hoist it two slots too far;
  routing all three accesses through struct members landed it exactly.
- **AN INLINE-ASM DMA MACRO'S CLOBBER LIST IS A PER-REGISTER cse SWITCH.** `dma.h`'s `DMA3_SET`
  already clobbers `"r0"`, which is why the ROM's second source is a fresh `ldr r0`; adding
  `"r2"` — and deliberately not `"r1"` — reproduces two adjacent transfers the ROM pools.
- **`+=` VERSUS `= … +` DECIDED WHICH VALUE GOT SPILLED**, 178 encodings away, after permuting
  declarations, loads and temps were all inert. **When a register-allocation residue looks
  structural, check whether a LATER use of the value is deciding it.**
- **`unsigned` VERSUS `int` ON A SWITCH SELECTOR IS VISIBLE IN THE COMPARE TREE**: unsigned folds
  `< 1` into `== 0` and gives the ROM's `cmp #1/bcc`. **A `bcc`/`bcs` against a case value that
  is not a range bound is the tell.**
- **SAME-BLOCK CONSTANT CSE: ONE PIN ON THE *EARLIER* USE IS ENOUGH**, and the flag route does
  not exist — `-fno-rerun-cse-after-loop` is byte-identical to the default, so this is the
  *first* cse pass, which scopes `docs/elevation.md`'s "prefer the flag" rule.
- **GIVE THE SECOND CONSUMER THE SHARED NAME, NOT THE FIRST** — the missing half of "separate
  variables do not defeat a COPY". 5 of 110 → 0.
- **WHERE A sched2 WINDOW IS A PERMUTATION AGAINST A COMPILER-GENERATED OPERAND, PIN THAT
  OPERAND** so it gets its own statement and a lower insn UID. Four hits in three functions.
  **Its limit, sharply: it does NOT work when the competing insn is reload-generated** — a
  spill store or reload. Tackle's surviving 8 are both of that kind.
- **`while` VS AN `if`-GUARD IS NOW A QUESTION ABOUT THE ROM**, not a coin flip: read it off
  whether the ROM **shares or re-computes the guard's address**. Third instance, and the first
  with a test that predicts which.

## Measured negatives that close open questions

- **The "written-never-read slot" lever does not work for a SCALAR.** flow1 deletes the store
  *and* the slot. Confirms the array-element half of the rule and rules the scalar route out.
- **`-fcall-saved-r4` does not emulate the ROM's r4 avoidance** — gcc still picks r4 and merely
  adds it to the push mask. Strong new evidence for the `REG_ALLOC_ORDER` hypothesis, and it
  says the workaround is not a flag.
- **ARM's `PROMOTE_MODE` forbids spelling a QImode carrier from C** — `unsigned char zi`
  measures identically to `unsigned int zi`, because a sub-word local gets an SImode pseudo.
  Closes a whole line of attack on `Func_80ba978`, which holds at **1 of 273**.
- **Code hoisting refuted by measurement, not just by `gcse.c:755`** — `-Os` gives 124 and 220
  differing.
- **Pinning a table pointer instead of `base` MISCOMPILES while screening well**: gcc coalesced
  `base` into the pinned register and read the wrong element. It scored size exact, count exact,
  180 differ — **a good score on a program that does the wrong thing.**
- **A pooled zero reaching a `strb` does NOT always need the `struct HalfWord` carrier**, against
  what a park records: here the bare literal already emits `ldrh`. Try the bare literal first.
- **"It went right when I added X" is not evidence** — two constructs in one winning candidate
  were both inert under a proper single drop.

## A park body can retire a lever it never tested

`OvlFunc_952_200c0b4`'s shipped body declared `unsigned short *hp;`, **never assigned it**, and
read `h = *hp;` — a load through an uninitialised pointer. Its header recorded that an `hp`
alias pointer "was tried and refuted". **That refutation is void: the test never aliased
anything.** Fixing it is 37 → 33 on its own.

`parkcheck` compares the header's *number* to the body's number, and both were self-consistent
at 37 — so the tool was always going to pass this. **What was wrong was what the body meant.**
A number-checker cannot replace reading the C, and this is the first case where the two come
apart.

Relatedly, **two synergistic changes on `Func_80bfba4`, neither of which works alone**: a
reversed zero loop is worse by itself (372 positional), and hoisting an offset add is the park's
own earlier candidate (422). Together, **174**. The park had both halves on file and never
crossed them — the clearest argument yet for sweeping *combinations* of recorded levers.

**And a caution against one of my own published numbers.** Batch 283 cited this park's
"35 register-blind of 454" as evidence that a positional count carries no ordering information.
An independent register-blind measure built this batch ranks the same candidates the **opposite**
way round (80 against 116). The *direction* of the batch-283 claim survives — size and count do
agree, so positional distance is not a distance — but **the specific 35 and 29 are not
reproducible** from anything recorded, and are now marked unverifiable in the park. A
measurement quoted without its recipe is not evidence, which is the same lesson as the 485.

## Corrections I owe the log

**I claimed "no pins" in a commit while the pin grep was in that same command's output.**
`Anim_Vine` carries `register void *tf __asm__("r0")` and needed a `fakematch.txt` row. **A
check whose result you read after the commit is not a check.**

**Then I did the same thing again, one commit later**, staging with a path-scoped `git add` that
missed **four regenerated `.s` files** — precisely the defect that left 17 elevated files bare
in batches 257–259, reintroduced by narrowing the add path. Caught by reading `git status`
*after* committing. **Twice in one batch I read a check's output after acting on it**; the fix is
ordering, not diligence.

**I nearly propagated an agent's generalisation without checking it.** Agent D reported
`REG_BLDALPHA` as a bank-wide bug "worth re-screening", naming eight parks. The header is
correct (`BG2PA` 0x20, `BLDCNT` 0x50, `BLDALPHA` 0x52), and D's finding is real **but local to
`Anim_Curse`**, whose reference genuinely does `BG2PA + 0x30`. The bank's assembly already names
both symbols correctly in 129 places, no other `+0x30` pattern exists there, and the five parks
that write `REG_BLDALPHA` write plausible BLDALPHA values, not `0`. **The local fact is
valuable; the generalisation was not checkable and did not hold.**

**And the brief I wrote for the 200-instruction band named the wrong dominant blocker** — I told
the agent it was pool-constant CSE, citing batch 280, and it was argument precompute.

**`split_s.py` refused a split I had cleared**, naming 11 further `.L` labels that a *neighbour*
references and that would have landed in a different object — the link would have failed. The
agent reported "exactly one asm edit" and that was true of its own function's needs, not of the
split. **The tool caught what both of us missed**, which is the case for it refusing rather than
warning.

## State

**4,547 from C / 1,163 in asm** against batch 283's 4,526 / 1,184 — exactly **+21/−21**, the
**nineteenth consecutive reconciling batch**. `census.py` TOTAL 1163 agrees (76 hand-asm, 14
ARM, 527 parked). **536 park files**, `fakematch.txt` **517 rows**.

**546 available** — 53 in 61–100, 203 in 101–200, 128 in 201–400, 87 in 401–800, **69 above
800**. Note these are on the *corrected* basis; the comparable batch-283 figure is 572, not the
417 published then.

## Open

- **Four stale parks to retire**, subjects already elevated: `Func_80f6038`, `Func_80f4100`,
  `Func_80a22f4`, `OvlFunc_881_2009888`.
- **`OvlFunc_930_20091b0` is still EXACT and still blocked on three decisions that are not
  mine**: an `ALIAS_CFLAGS` row, `_AREA_58 = 0x58`, and a 1,984-byte rehome.
- **A `DMA3_SET` variant with `"r2"` in its clobber list**: one landed file carries a local copy
  rather than changing `include/dma.h`, to avoid putting every other user back in scope for
  re-verification. A `dma.h` variant is the alternative.
- `-fcall-saved-r4` for `OvlFunc_970_2008f80` — now argued **against** for the `REG_ALLOC_ORDER`
  class by direct measurement; `ALIAS_CFLAGS` for `OvlFunc_882_200c41c`.
- **Withheld `.sym` entries**, none completing its function: six `_MSG_*` for `Func_801c49c`,
  `_MSG_cc3`, `_MSG_828`, `_FILE_e4`, `_FILE_e5`, plus batch 280's three. Long-standing:
  `_MSG_2080`, `_SIZE_80f0024`, `_TBL_7a828`.
- **Backfill `Verify with:` recipes into the 485 UNCHECKABLE parks.** 51 are now checkable.
- Two `rom_c9000` targets traced but unopened, with notes: `BaseAnim_Tentacle` (five `.rodata`
  blobs needing the `.global` treatment) and `Anim_Unused_Haunt` (frame 0x58, eleven spill
  slots).
- The fakematch convention question, ~20 of 181 PIN-using files unbooked;
  `OvlFunc_948_2009308`'s one pin, from batch 272.
