# Batch 283 — six functions, `rom_c9000` opens, and 485 park claims that nothing can verify

Gated on a clean `make clean && make -j8 && make compare` → `goldensun.gba: OK` at
`5c4695205413df7db52b9a184815a07783999971`. All six verified at their exact ROM addresses.
`git status` clean.

| | |
|---|---|
| elevated | **6** |
| parked | **15 new + 2 advanced** |
| retired | 1 park (became a landing) |
| `.sym` entries added | **1** (`_CONST_0`) |
| new tooling | **`tools/parkcheck.py`** |
| agents | 8, at 5 targets each = 40 |

**The landings concentrated in three of the eight agents**, and one agent produced four of
the six:

| agent | bank | landed | parked |
|---|---|---|---|
| F | overlays 899 / 970 / 947 | **4** | 0 |
| A | `rom_c9000` animations | **1** (`Anim_Break`) | 1 (`Anim_Drain`) |
| C | overlay 951 | **1** | 0 |
| D | overlays 930 / 952 / `rom_b9b30` | 0 | 3 — but one is **EXACT** |
| B | `rom_c9000` animations | 0 | 5 |
| G | `rom_c9000` (Tackle, Spore) | 0 | 1 new + 1 advanced |
| E | `rom_15000` menus | 0 | 2 |
| H | `rom_c9000` / `rom_b5000` | 0 | 2 new + 1 advanced |

I first wrote this table from my launch notes, which credited agent D with a landing; the
commit record shows all three of D's results are parks, and agent F's fourth
(`OvlFunc_947_200a74c`) landed in a separate commit I had counted against the wrong agent.
**The commits are the record, not the launch log.**

**6 landed + 15 parked + 2 advanced = 23 of 40 targets accounted for, so 17 were never
opened.** That is the highest unopened count of the session and it is a direct consequence of
5 targets per agent at this size band — four of the six landings are 173–702 instructions, and
a 700-instruction function consumes an agent's whole budget. **Four targets per agent has been
the better ratio in every comparison this session.**

The six:

| function | insns | bytes | file |
|---|---|---|---|
| `OvlFunc_899_200afd4` | 702 | 1828 | `src/overlays/rom_794ac0/ovl_30_c_a_a_c_b.c` |
| `OvlFunc_970_2008430` | 678 | 1796 | `src/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_a_b.c` |
| `Anim_Break` | 345 | 800 | `src/rom_c9000/rom_d82b0_b.c` |
| `OvlFunc_947_200a74c` | 284 | 736 | `src/overlays/rom_7d0e88/ovl_2580_c_c.c` |
| `OvlFunc_951_20081d8` | 277 | 740 | `src/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_a_a_a.c` |
| `OvlFunc_947_200975c` | 173 | 476 | `src/overlays/rom_7d0e88/ovl_1528_a_a_c_a_b.c` |

## `rom_c9000` opened, two batches after the bet was placed

Batches 281 and 282 put **nine agent-targets into this bank and landed nothing**. Batch 283
landed **`Anim_Break` — the first animation entry point in the bank** — and took
`BaseAnim_Tackle` to **12 of 402 with size, count, frame AND relocations all exact**, with
nothing structural left. What changed was not effort: it was the accumulated levers, four of
which came from the 0-for-9 rounds. **A bank that returns idioms and lands nothing is not a
failed bet**, and this is the clearest instance the project has.

And the bank immediately contradicted two rules it had itself produced:

- **`while (c)` and `if (c) { do … while (c); }` are different code.**
  `duplicate_loop_exit_test` runs *after* gcse, so a `while` guard cannot be gcse'd and an
  explicit `if` guard can — which lets gcse hoist a loop-invariant address into the preheader.
  `Anim_Break` needs the `if` form; **`Anim_Drain` needs `while`**, where the `if` form hoisted
  the address into a callee-saved register and **cost `base` its r9**. 168 → 135, and it put
  base back in r9 **without a pin**.
- **Per-target induction variables belong as strength-reduced givs**, against the "must be
  explicit locals" rule two earlier functions had claimed as a bank rule. Letting
  `strength_reduce` build them puts the initialising stores in the preheader *after* the guard
  branch, where this ROM has them. 30 → 10 in one edit.

**And I have been giving agents advice that is wrong for half its cases.** "Pin `base` to its
high register" is right for a value consumed whole and **wrong for a struct pointer**:
`register int *p __asm__("r9")` puts a hard hi-reg *inside* the MEM address and reload then
reloads the whole address, where an unpinned pseudo that `global_alloc` happens to put in r9
gets the ROM's `mov r4, r9 / ldr r3, [r4, #0x10]`. Removing one such pin went **62 windows /
249 lines → 18 / 73** with the size becoming exact. **Three functions now say the pin rule is
conditional** — a third instance is `BaseAnim_StatDown`, whose `base` must be *spilled*, via a
three-step chain (`.18.greg` read rather than guessed).

## `tools/parkcheck.py`, and what its first sweep found

**Two parks claimed numbers their own C did not produce** — `Func_80a96d8` (header 4, body 10)
and `BaseAnim_Tackle` (header 12, body 47). Both mine, both the same mechanism: I updated the
header with improved figures and never swapped in the improved body. **Both were caught by
agents re-measuring a park they had been told to start from** — the next reader paying for my
mistake, twice.

Batch 281 had **already recorded the lesson** ("read a park's header against its body before
committing"). Recording it did not prevent the recurrence, **which is the argument for a tool
instead of a habit.** `parkcheck.py` reads each park's `Verify with: objcmp.py …` recipe, runs
it, and compares the measured count to the header's first claim.

**The sweep found something bigger than the two mismatches: 33 OK, 0 MISMATCH, and 485
UNCHECKABLE** — only recently-written parks carry a recipe at all, so **the overwhelming
majority of the park corpus states a number nothing can verify.** That is not a claim they are
wrong; it is the reason the two that *were* wrong survived. The recipe line is mechanically
derivable, so **backfilling it is available work that would make the whole corpus checkable.**

It then earned its place three more times in the same batch: two park files written with
**unterminated comment blocks** (broken C in the tree), and two headers whose numbers I had
invalidated by **stripping their verification shims** while landing their exact siblings.
**Parks are self-verifying and legitimately keep their shims; only landed files must not.**

## The measurement rule takes a fourth and fifth angle

Batch 280: a normalised count understates when the pool moves. Batch 281: it overstates to
calling a byte-identical object 230 differences, and once instruction counts disagree its
*ordering* is wrong. This batch:

- **`objcmp`'s SIZE line is a false positive, by exactly the blob's length, when the candidate
  *defines* a `.rodata` array from C** — it sums the object's sections, so `Anim_Break` screened
  as "ref 800, ours 816" while encodings and relocations stayed silent. **Screen with `extern`,
  restore the definition for the landing.**
- **Even with size AND count agreeing and no jump table, the positional count mis-ranks.**
  `Func_80bfba4`: **232 positional, 124 shift-tolerant, 35 register-blind.** Size and count
  already agree, so the positional number carries *no ordering information at all*, and the
  register-blind number is what correctly ranked all 25 candidates.
- **A third `tryc` blind spot, a false positive this time:** `ldrh rX, label` and `ldr rX, label`
  assemble identically because GAS rewrites the halfword form. A candidate was burned "fixing"
  a difference that did not exist in the bytes.
- **And a caution on the symbolised-reference technique**, found because two numbers disagreed:
  `Func_801c49c` reads 98 against a symbolised copy and 92 against the tree reference.
  **A symbolised copy that is WORSE has been over-applied** — symbolise only what your candidate
  spells as a symbol, and if its number is worse, the copy is wrong rather than the candidate.

## Levers, each from a compiler line rather than a pattern

- **`use_related_value` only fires for `CONST`, never a bare `CONST_INT`** (`cse.c:1637`). This
  is the line behind **every `_MSG_*` base-symbol entry in `message.sym`**, and it converts that
  tell from a pattern that works into a mechanism that *must* work: when a ROM reaches
  neighbouring ids by register arithmetic off a base, **no plain-literal spelling can produce
  it.** It also bounds the claim — the neighbours stay plain literals.
- **The empty-asm barrier is a scheduling tool only and cannot touch cse.** `cse.c:5745` flushes
  the table only when `GET_CODE (PATTERN (insn)) == ASM_OPERANDS`; a no-operand
  `asm volatile("")` emits `(asm_input "")`, and adding a clobber wraps it in a `PARALLEL` —
  neither is `ASM_OPERANDS`. The `"i"(0)` form does flush and cost more than it saved. **What
  does reach cse is a pinned CALL-CLOBBERED register** (`invalidate_for_call`), confirmed
  separately as the cure for `BaseAnim_Tackle`'s two-pool-loads problem — which **strikes that
  park's "not reachable from C with any lever on file" claim.**
- **A bare `__asm__ volatile ("")` placed BEFORE the statement is a cheap sched2 lever, and it is
  not symmetric.** From `rank_for_schedule`: with priorities equal, **the class relative to
  `last_scheduled_insn` decides before `depend_count` and long before `INSN_LUID`**, so the
  barrier works by changing which insn was last-scheduled at the sort. Placed *after* the same
  statement it fixes the register order but kills the interleave. 8 → 6 → 4.
- **`loop.c` hoists invariants with `emit_insn_before (…, loop_start)`, so a hoisted invariant
  can never precede preheader code** — which is why 112 barrier, pin and carrier spellings in
  one park could not reorder two instructions. **The cure is to change WHICH PASS emits your
  code**: making the pointer a strength-reduced giv puts its init after `move_movables`, the
  ROM's order. 2 → 0. Same family as batch 282's `check_dbra_loop` finding, and it generalises
  it — the escape is not always loop reversal.
- **Read `.18.greg`'s two lists** rather than guessing: `;; N regs to allocate:` is
  `global_alloc`'s priority order, `;; Register dispositions:` is the outcome. With the corollary
  that settles spill-versus-address-taken: **expand-time slots are always allocated above
  reload's**, so a slot below the parameter spill is a reload spill.
- **A block-scoped declaration gets a LATER pseudo number than a compiler temp**, hence a lower
  spill slot — `expand_decl` runs in code order. 37 → 32; refines the declaration-order rule,
  which only covered the outer decl list.
- **"Assign the `base + K` pointer last" is the highest-yield lever in `rom_c9000`**: 26 → 12
  came entirely from statement ordering inside regions, four separate hits.
- **A long-lived local holding a SYMBOL ADDRESS reaches `ldr rX,=sym / sub rX,#K`** — stretching
  its live range denies the allocno a hard register, at which point `reg_equiv_constant` makes
  reload **rematerialise** rather than spill. The *position* of the assignment is the whole
  lever; inline at each use is 60 lines worse. This resolves, for the symbol case, a shape the
  docs called unreachable.
- **A block between a loop's `b test` and its body means the loop lives AFTER the enclosing
  loop**, entered by a goto — gcc-2.96 has no block-reordering pass, so physical order *is*
  source order. 407 → 363 and eight relocations byte-exact. And **loops in one function can be
  a mix**: three of five in `StartTitleScreen` are goto loops with no LICM.
- **The shape of an induction expression decides strength reduction, and it is the shift not the
  algebra**: `frame * (i * 8 + 0x100)` is not reduced, `frame * ((i + 0x20) << 3)` is, and
  `((i + 0x20) * 8)` measures identical to the unreduced form.
- **A pinned fill's op order INVERTS through sched2, and the source is ascending either way.**
  When the ROM shifts descending, **write ascending anyway** — writing descending transposes the
  movs. That was `200afd4`'s entire residue, 4 → 0. For 4-argument `neg` fills a 30-permutation
  sweep found the form is **pair each register's define with its own op, ascending** — not all
  defines then all ops. 10 → 3.
- **A dead store the ROM keeps is reproduced only by an ARRAY.** A slot written once and never
  read gets its store deleted by flow analysis when written as two scalars, taking the frame
  0x44 → 0x40. An array element cannot be deleted. **A frame exactly four bytes short with
  everything above one slot displaced is now a recognisable family** — four instances in one bank
  this batch, in three shapes — and the array is the first thing to try.
- **Drop the sibling's `off = 0;` scaffold — it is not free, and copying it blindly cost 15 of
  294.** Thumb `ldrsh` is register-offset-only so gcc generates that zero itself; supplying it as
  a named local makes it a CSE candidate PRE hoists into the dominator block. **That idiom is a
  lever for the function it was found on, not a transcription convention** — the **third time
  this session** a recorded lever proved per-site rather than general, which is now the default
  expectation rather than a surprise.
- **One counter per loop, one offset variable per region** — two loops in the same arm live in
  different register *classes* (caller-saved for the call-free loop, callee-saved for the one
  that calls), so sharing a counter cost 10 of 294. **And that corrected a misattribution:** the
  gState pool-load placement had been blamed on aliasing with `-fno-strict-aliasing` thought
  necessary. It was not. **Check live ranges before reaching for an aliasing flag.**
- **Count the ROM's hard registers to decide whether a reused counter is one variable.** Two
  functions this batch reached *opposite* conclusions — one counter in r8 across three loops
  (splitting cost a spill slot each time) against three distinct registers for three counters
  (splitting worth 160 → 150). One pseudo gets one hard register, so **the ROM's register count
  is the answer.** This also resolves the apparent tension with "split reused locals per region":
  that rule is about **values**, this one about **counters**.
- **An alias-set change cannot reach a combine merge** — a distinct struct overlay giving a
  distinct alias set had no effect on a sign/zero-extend load merge, because it is combine's
  value substitution rather than `exp_equiv_p`.
- **gcc-2.96's code hoisting only runs under `optimize_size`** (`gcse.c:755`), so a ROM's
  dominator copy cannot be hoisting at -O2.
- **A member read used directly as a compare operand is turned into a copy of the first load's
  register**, while the same read assigned to a named local in a join block stays an `ldrb`.

## `_CONST_0 = 0x0` — `const.sym`'s first zero entry

It **completes its function**; the halfword exception was checked and does not apply (an SImode
call argument, not a byte or halfword store); and the in-function control is strong — the same
function passes a literal 0 in four other argument slots, all reproducing as `mov rN, #0`.
`docs/elevation.md`'s generalised tell already says it holds all the way down to zero. Unlike
its `_CONST_2` precedent's recorded `.word` mismatch, the zero case comes out byte-identical.

Also, an **anti-tell in the other direction**: `Anim_EPowerUp`'s single relocation difference is
**the reference being under-annotated** — `ldr r6, =0x57` can only be a relocation for a value
that fits `mov r6, #0x57`, so `_FILE_57` is correct and the C should not be "fixed".

## Two process errors of mine, both caught by the gate rather than by me

**I landed three functions in one build**, which `docs/elevation.md` explicitly warns against.
`compare` went red and I had to bisect; it was quick only because exactly one overlay failed.

**And the cause was trusting a report over the file.** The agent said its candidate verified with
a `__asm__(".equ _CONST_0, 0")` shim; **the file on disk contained neither the shim nor any
reference to the symbol**, so I landed the four-bytes-short version — the missing pool word, 3,307
differing bytes from the layout shift — and my shim-stripping regex had nothing to strip.
**Check that a candidate contains what its report claims before landing it**: "verified with a
shim" means the shim must be in the file you measured.

**And a counting error in this batch's own bookkeeping.** My per-batch landing counter reported
**5**, missing `OvlFunc_951_20081d8` because it came out of a **park**, so git recorded a rename
rather than an addition and `--diff-filter=A` skipped it. The count is **6**; `--diff-filter=AR`
(or `-M`) is what the counter needs. This is the first landing this session to come out of a
park file, which is why the bug had not surfaced — and the arithmetic reconciliation is what
caught it, as in batch 281.

## State

**4,526 from C / 1,184 in asm** against batch 282's 4,520 / 1,190 — exactly **+6/−6**, the
**eighteenth consecutive reconciling batch**. `census.py` TOTAL 1184 agrees with `funcindex`
(76 hand-written asm, 14 ARM, 677 parked). **531 park files**, `fakematch.txt` **509 rows**.

**417 available** — 165 in 101–200, **106 in 201–400, 65 in 401–800 and 59 above 800**, with 21
left in 61–100 and effectively nothing under 60. The bands the batches are now working in are
where the remaining work is: **230 of the 417 exceed 200 instructions.**

## Parks worth the next reader's attention

**`OvlFunc_930_20091b0` is EXACT** — 744 bytes / 302 encodings / 59 relocations under
`-fno-strict-aliasing`, stable over three repeats — and parked only because landing it needs
**three decisions that are not mine**: an `ALIAS_CFLAGS` row (a claim about how the original was
compiled), `_AREA_58 = 0x58` (a genuine hole between `_AREA_57` and `_AREA_59`, namespace
checked), and a 1,984-byte text/data rehome.

`Func_80ba978` is at **1 of 273**, size and relocations identical, and converts whole outright.
`BaseAnim_Tackle` at **12 of 402** has size, count, frame *and* relocations exact — the residue
is post-reload scheduling alone, in four windows. Four of the five `rom_c9000` animation parks
have count and/or size already exact.

## Open

- **`ALIAS_CFLAGS` for `asm/overlays/rom_7b7f1c/ovl_30_c_c_c_c_c.o`** plus `_AREA_58` and a data
  rehome — all three needed together to land an already-EXACT function.
- `-fcall-saved-r4` for `OvlFunc_970_2008f80`; `ALIAS_CFLAGS` for `OvlFunc_882_200c41c`.
- **Withheld `.sym` entries**, none completing its function: six `_MSG_*` for `Func_801c49c`
  (two of the six explicitly weak on their own), `_MSG_cc3`, `_MSG_828`, `_FILE_e4`, `_FILE_e5`,
  plus batch 280's `_MSG_2644` / `_MSG_920` / `_MSG_970`.
- Three long-standing entries: `_MSG_2080`, `_SIZE_80f0024`, `_TBL_7a828`.
- The fakematch convention question, ~20 of 181 PIN-using files unbooked.
- `OvlFunc_948_2009308`'s one pin, from batch 272.
- **Backfill `Verify with:` recipes into the 485 UNCHECKABLE parks.**
