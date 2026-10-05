# Batch 324 — the mid-band (21–50 encodings)

**7 landings, 13 parks improved, 0 pins, 0 devices, 0 new flag groups.**
Progress **4,853 → 4,860 of 5,710 (85.1%)**; remaining functions 857 → 850,
parked 765 → 758, park files 788 → 773.

Eight briefs, grouped by bank: six in `rom_15000` (235 landed / 89 parked at the
start), two in `rom_9000` (131 / 59). All eight reported.

## Why the mid-band, and whether it worked

Batches 317–323 worked the near-zero shelf. This batch deliberately aimed at
21–50 on the reasoning that **a large figure usually means several independent
causes, each individually small** — batch 323 had gone 3-for-3 at figures of 18,
19 and 20.

**It worked.** The six landings came from parks at 22, 27, 28, 31, 31 and 39. Not
one was a single-cause problem, and in several the decomposition was the whole
job:

| function | park | landed by |
|---|---|---|
| `Func_8021b80` | 22 | a `switch` (not if/else-if) + two literal stack args |
| `HeightTile_5` | 27 | `a += b` + three returns + comparison form + multiply order |
| `Func_80217a4` | 28 | **bitfields** + an `EXPAND_SUM` address form + an `int` temp |
| `HeightTile_B` | 31 | port two sibling levers + flip the multiply's operand order |
| `Func_801c954` | 39 | the read inside the `while` condition + a named halfword pointer |
| `Func_801edec` | 48 | the helper as a **macro**, not an inline function |

And the parks that did not land moved a long way: **41 → 2**, **37 → 3**,
**23 → 2**, **35 → 5**, **39 → 10**, **27 → 10**.

> **The band's real advantage was not that the problems were easier. It was that
> the first one or two causes in a long diff are usually mechanisms already solved
> elsewhere in the same bank.**

## The finding that generalises furthest: reload's round-robin cursor

**Two agents, two functions, two banks, no contact between them, same
conclusion.** Brief E found it at `reload1.c:5003`; brief A found it at
`reload1.c:4996-5013`.

Park after park has described a coherent r0/r2/r3 "rotation", blamed
`REG_ALLOC_ORDER`, and declared it immovable. It is not `REG_ALLOC_ORDER`, and
the instructions involved are not allocator output at all — **they are reloads.**
`allocate_reload_reg` (`reload1.c:4962`) picks round-robin from `last_spill_reg`
(`:5003`, comment: *"We advance it round-robin between insns to use all spill
regs equally"*), which is set on every successful allocation (`:4937`), reset
**exactly once per function** (`:823`), over a `spill_regs` built in ascending
hard-register order by `finish_spills` (`:3531`).

> **A reload register is a CURSOR keyed on how many reloads precede it in the
> function, modulo `n_spills` — not a preference order.** Add or remove one
> earlier reload and every later reload register shifts by one. That is reachable
> from ordinary C.

**Any park blaming `REG_ALLOC_ORDER` for a coherent rotation should be reopened.**

## Mechanisms read from the compiler this batch

- **`expand_assignment` is LHS-first except for one gated path.** `expr.c:3402`,
  with the single RHS-first branch at `expr.c:3604` gated on
  `TREE_CODE (from) == CALL_EXPR`; everything else reaches `expr.c:3643`. This is
  why `s->obj = f(...)` never has its address pseudo cross the call — and why
  `Func_801d014`'s 37 was one edit away.
- **And the temporary's TYPE is the lever, not its existence.** `int t` = 49,
  `unsigned char t` = **3**, on the same statement pair the park had already
  tried and measured at 43 before concluding "nothing source-level".
- **`*thumb_mulsi3` (`arm.md:1116`) ties the dest to md operand 1, and C `X*Y`
  expands to `(mult Y X)` — so md operand 1 is the SECOND C operand.** With
  `set_preference` stripping `XEXP(src,0)` for a format-`"ee"` mult
  (`global.c:1585`), the spelling of a multiply chooses which variable is dragged
  toward r0.
- **`thumb_exit` (`arm.c:8306-8320`): `size == 0 && mode == VOIDmode` gives
  `r0|r1|r2`, anything else `r1|r2`.** So **`pop {r1}` proves a non-void return.**
- **A bare `return;` in a non-void function expands to
  `(clobber (reg/i:SI 0 r0))` plus the jump**, and that clobber blocks both
  `jump.c:348`'s `follow_jumps` redirect and `jump.c:434`'s invert — which is why
  four obvious spellings of `Func_801c244` stayed flat at 31.
- **`find_and_verify_loops` (`loop.c:2751-2944`) relocates an early-exit block out
  of the loop and inverts the branch, and its first guard is `this_loop` from
  front-end loop notes — so a label plus a backward `goto` emits none and the pass
  cannot fire.** Worth 27 → 13.
- **`nonzero_bits`' REG case (`combine.c:7987`) returns tracked bits only for
  `REG_N_SETS == 1`.** So **reusing an existing local** gives a masked value a
  multi-set pseudo and lets a wide constant reach the pool. With
  `LOAD_EXTEND_OP` unconditionally `ZERO_EXTEND` on Thumb (`arm.h:2330`),
  `simplify_and_const_int` had been narrowing `0xfffffc00` to `0xfc00 = 0xfc << 8`,
  which can never be pooled. Predicted from source, then measured.
- **`expr.c:7340` — "put a constant term last and put a multiplication first" —
  swaps when `GET_CODE (op1) == MULT`**, and an `ARRAY_REF` hands back a MULT
  under `EXPAND_SUM` where a shift hands back a REG. Hence
  `*(int *)((char *)t + (idx << 2))` differs from `t[idx]`.
- **`combine.c:3492`'s third clause** swaps a commutative op when operand 0 is a
  SUBREG of a class-`o` object and operand 1 is not class `o`; a `const_int`
  operand is class `o` and stops the cascade.
- **`allocno_compare` (`global.c:597-620`) is
  `(floor_log2(n_refs) * n_refs / live_length) * 10000 * size` — with NO frequency
  and NO loop-depth term**, refuting one park's stated mechanism. All 13
  priorities were reproduced in sequence against `.17.lreg`.
- **New bound:** `local-alloc.c:886`'s `reg_equiv_replace` can never fire for a
  thumb symbol constant — `SET_SRC` is a pool-load MEM and the note is a bare
  `symbol_ref`, so its `rtx_equal_p` is always false.
- **Qualifies a standing note:** `rtlanal.c:234 get_related_value` **does** return
  the base for `CONST (PLUS sym int)`, so cse *can* relate those constants
  (`cse.c:2074 use_related_value`). The standing "cse cannot relate constants" note
  holds only for the non-`CONST` case.
- **New compiler fact:** a non-constant pointer local as `DMA3_SET`'s `dst` **ICEs
  this gcc** (`expand_inline_function`, `integrate.c:678`).

## The crossing law, in its purest recorded form

`Func_8019944`: `unsigned int i` alone is **exactly inert at 35**. The
single-exit `for(;;)`/`break` restructure alone is **exactly inert at 35**.
**Together: 17**, size- and relocation-exact. Neither survives one-at-a-time
screening.

Two more instances, both about *negatives*:

- `Func_801d9d4`: the park's `rows = 1` before `sel = 2` reads **+2 alone, −2 on
  top of the first edit, −6 crossed.** A measured negative that was a **missing
  prerequisite.**
- `Func_8011fd8`: the park's rejected "offset as a named local" (35) is **exactly
  inert at 30** once the register map is fixed. Same shape.
- And a **half-fix**: `Func_801d9d4`'s `h before y` was load-bearing while
  `bx->x * 8` was inline and a **+4 regression** once `tx` is named — the pair
  worth −26. Shape 6 of the cross-the-lists law, with the two halves in
  **different lists**.

> **A park's "MEASURED: worse" is as much a hypothesis as its diagnosis.** It is a
> measurement of that edit *against that body*, and the body is what changed.

## Park verdicts refuted this batch

Of eleven parks examined, **ten had their diagnosis corrected** and one survived:

- "every one of the 41 differing encodings is the same instruction with a
  different register number … nothing tried here moves it" — **two runs totalling
  25 of 41 were a pure permutation and a different sequence.**
- "the boundary is a CALL, which is as strong a boundary as exists" — **a call is
  not a basic-block boundary.** Verified independently in `flow.c:493-519`:
  a block starts only at a `CODE_LABEL`, or after a `JUMP_INSN`, `BARRIER`, or a
  call with `call_had_abnormal_edge`, and that flag needs EH or a nonlocal goto.
  What a call *does* is `cse.c:5710-5714`: `invalidate_memory()` for a
  non-constant call plus `invalidate_for_call()`, which drops only call-clobbered
  **hard** registers. This reconciles two findings that looked contradictory.
- "no honest source form produces that" — refuted; it was invisible only because
  a length defect made the positional figure measure **misalignment**.
- "nothing source-level" — refuted by a one-word **type** change.
- "every difference is which scratch register" — two of the five runs are
  instruction **order**.
- "the two halves can't be got at the same time" — two edits do both.
- Plus a park whose single "SOLVED" claim was wrong about **which veneer** it
  emits (`_call_via_fp`, not `_call_via_r3` — and the veneer name *is* the
  register).

**The one that survived** is `StartMenu_Main`: both named causes reproduced
bit-for-bit, and its inert list reproduced independently before being read.

> **A park's "what is right and should be kept" list is as suspect as its
> diagnosis.** `SystemMsgBox` went 23 → 5 by **deleting the one local its park
> said to keep.**

## Housekeeping, and four errors of mine

**Retired 8 stale parks** (10 functions, every one already byte-exact and absent
from `asm/`). The class park among them, `constant_reuse.c`, grouped three
overlay functions under one hypothesis — *"finding it would move all three at
once"*. All three landed, **no two by the same route**, and one was not a codegen
problem at all (its C was semantically wrong). Its eleven-flag sweep was not
wrong, it was **unanswerable**: a sweep over a symptom-grouped class cannot fail
informatively.

**Backfilled 8 figureless parks.** Best find: `StartRain` at **4 of 104,
relocations identical** — invisible because its header prose contained
`int/void*/undeclared`, whose embedded comment terminator closed the header four
lines early and left the body uncompilable. No tool could rank it, check it, or
contradict it. `parkcheck.py` gained a `HEADERCUT` verdict (zero false positives
across 780 parks). Also `Debug_WarpMenu_UI` at **17 of 163**, relocations
identical, that no ranking tool could see.

**Retired an `unmatchable.txt` enrollment.** `Func_80b09fc` was listed as
permanently unmatchable because the ROM dumps its pool mid-function with a
skip-branch — *"an original-TU pool-pressure artifact a standalone TU cannot
reproduce"*. **It is matched**, from thirty lines of plain struct assignment. The
tail is gcc's ordinary output for `a->fc = 0;`: `*thumb_movhi_insn` alt 1 takes
`mn` (`arm.md:4318`) so a HImode `const_int` can never use an 8-bit `mov`, and
`pool_range` 64 against SImode's 1020 forces the dump before the epilogue. An
enrollment now needs the same standard as a `*.sym` entry.

My errors, all four caught by agents or by measurement:

1. **My duplicate-park detector counted a top-level `extern` declaration as a
   definition**, so briefs A, C, D and G were each told to cross a phantom. Now
   `tools/park_bodies.py`, which balances parens, requires `{`, and requires
   column 0 — and **writing it reproduced a sibling of the bug**, mistaking an
   indented call feeding an expression (`BufferString.c:435`) for a K&R
   definition, contradicting brief C's correct reading of that file.
2. **`install_batch.py` crashed with `SameFileError`** on a park a brief
   deliberately left unchanged — and crashed *after* applying three entries,
   leaving the batch half-installed with no summary. An unchanged park is a
   result, not an error.
3. **I filtered `objcmp`'s output when deriving a figure**, which hid a `SIZE`
   line and made `Task_Debug_SpriteTest` briefly look like a near-match when it is
   eight bytes long. The "equal counts hide length differences" trap in a new
   costume.
4. **My backfill's first draft wrote figures as `N of M differing encodings`**,
   which `parkcheck`'s `CLAIM` regex does not parse — six parks came back
   `NO CLAIM` with correct figures sitting in their headers. Canonical order is
   `N differing encodings of M`, and the verify recipe must be on **one line**
   (`VERIFY`'s separator class excludes the `*` of a comment prefix).

## Brief F — the largest figures in the batch, and one mechanism behind two of them

`Func_801edec` **48 → 0**. The park's body did not contain its own helper
(`DMA3_FILL16` was not in `dma.h`, so it emitted a call relocation); pasting the
park's commented-out inline function back in gave 48 → 40, and **the whole
remaining 40 was pool layout** — both streams held the same 43 instructions.

The fix was writing the helper as a **MACRO** rather than an inline function:

> **A `u16` PARAMETER IS PROMOTED.** With the inline function, `.02.jump` already
> holds `(set (reg/v:SI 37) (const_int 57568))` — the fill value is an SImode
> pseudo and **the constant never enters `movhi` at all.** A macro pastes the
> literal into the HImode store, `force_reg (HImode, …)` fires, the HImode fix
> appears, both pools land in the ROM's order, and the `mov r0,sp` ordering
> closes as a side effect. 40 → 1 → 0.

Promoted to `include/dma.h`, the fourth of that shape after `DMA3_FILL_OFS`,
`DMA3_SET_RW` and `DMA3_CLEAR_OFS`.

**It also overturned a screening decision from batch 205.** That park had
recorded `_FUNC_80158E8_SIZE` and its literal as "NOT interchangeable in the
built ROM". The symbol is **absolute** (`nm`: `00000214 A`), so they are
interchangeable in the link — the rejection had been screened on `tryc.py`
reaching one differing line, and **`tryc.py` is blind to pool order**, which is
where the other 40 encodings were. The symbol is now independently *vindicated*:
the literal `0x214` lets the control word fold and reads **50 of 52**.

> **A screening tool's null result is scoped to what that tool can see.** We had
> already written down that `tryc.py` cannot see pool order. The cost of not
> joining those two facts was one symbol-table entry wrongly doubted and a park
> left parked for 119 batches.

**`Func_8020b64` stays at 50 of 57, and `Func_801bcd4` at 48 of 81** — but both
are now one named mechanism, found in two different passes, and the figures are
reframed. `Func_8020b64`'s reference is 56 instructions plus a 2-byte pad against
our 55, so **the entire 50 is ONE missing instruction** and its misalignment.
`Func_801bcd4`'s two streams hold **70 instructions each**.

### The copy-collapse family

A two-instruction copy chain — load into a temp, then copy to the variable — is
collapsed inside the cse pass block, by `delete_trivially_dead_insns` at
`toplev.c:2908-2933`, not by `cse_insn`'s canonicalisation (`cse.c` never reads
`REG_DEAD`; grep gives no hits).

> **The surviving insn keeps its own LUID, and sched2's last rung is `INSN_LUID`.
> So which of the two survives decides BOTH which register holds the value AND
> where the instruction sits** — one collapse producing what looks like two
> unrelated defects.

`Func_8020b64` is the sharper diagnostic: through `.03.cse`, `.07.gcse` and
`.08.loop` the chain runs in the **ROM's** direction, and **`.09.cse2` reverses
it** — which is why that park's `-fno-rerun-cse-after-loop` row measured *worse*
rather than inert. **Fourteen whole bodies across the two functions, all exactly
flat**, every one matching the reference's per-opcode memory profile.

### The cross-jump threshold is TWO, and argument order decides it

`jump.c:660` calls `find_cross_jump` with minimum 1 for a simplejump; for every
**other** jump to the same label on `jump_chain`, `jump.c:675` passes
**minimum 2**. Each matching insn decrements it (`jump.c:1602`) and the merge
fires at `minimum <= 0` (`:1607`).

	ROM   adds r2,r4,#0 / adds r0,r5,#0 / movs r1,#0x3a / bl / b   -> suffix 1, NO MERGE
	ours  adds r0,r6,#0 / movs r1,#0x3a / adds r2,r4,#0 / bl / b   -> suffix 2, MERGE

**The ROM sets the COMMON argument first and the DIFFERING one last**, leaving a
one-insn suffix, exactly one short of the threshold. Cross-jumping is
unconditional at `-O1`+ (`toplev.c:3515` is the only `JUMP_CROSS_JUMP` site), so
**no flag reaches it.** The ROM can set `r2` first because `s` is
**address-taken**, so `store_one_arg` emits that read at *evaluation* time, ahead
of `load_register_parameters`' moves — independently confirmed by the same
function's 5-argument arms, which already match for the same reason.

Three instruments on `Func_801bcd4` read **42** and were correctly **rejected as
figures that lie**: 83 instructions against the reference's 81, with `ldr=7`
against the ROM's 3.

## Carried forward
- **`HeightTile_*` is 13 of 16 landed**, and the family rule is now stated and
  measured twice: the ROM's opening `mov` tells you which way to spell the
  multiply. `_4` (which has two parks), `_6` and `_A` remain.
- **Reopen parks blaming `REG_ALLOC_ORDER`** for a register rotation, against the
  reload-cursor reading.
- **Re-measure `ovl_7b2078/2008388.c`**, whose park claims to be "the first
  counterexample" to the pool-constant CSE rule on the grounds that its boundary
  is a `beq`. A `beq` *is* a real boundary, so its premise is sound where
  `Func_801c954`'s was not — but it was written by the same author in the same
  round.
- **`Func_80b0a20`** at 26 of 34. Its park is a genuine proof citing `arm.c:4820`,
  a measured `.26.mach` bound and `arm.h:1807`, and it is **not** refuted by its
  sibling's landing — but its step-3 premise ("nothing can supply a narrow
  `0xffff`") now has evidence against it.
- **`Func_801965c`'s remaining 10** and **`Func_8011164`'s 27** both reduce to
  "greg has a free register and the original build did not"; `-ffixed-r7` closes
  the latter's length gap exactly, as an instrument.
- **The copy-collapse family** (`Func_8020b64`, `Func_801bcd4`) is the batch's one
  genuinely unreached mechanism — fourteen bodies, all flat. Worth a brief of its
  own rather than another spelling sweep.

## A process error of mine, the third of its kind this session

I ran `git add -A` for the publish commit **while brief F was still writing to
the tree**, so its landing was swept into "Publish batch 324" and its `dma.h`
change into the briefs-A-and-C commit. Both commits are green — I re-gated HEAD
to confirm — but the publish commit went in **ungated**, and that was luck rather
than care.

> **Before `git add -A`, check that no agent is still running, or stage explicit
> paths.** I had already raced my own background jobs twice this session; this is
> the first time it reached a commit.

Brief F's manifest also carried **only its landing**, so its two parks' findings
existed nowhere but scratch. They are now written into the park headers and
verified by `parkcheck` at 50 and 48.
