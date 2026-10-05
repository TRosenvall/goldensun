# Batch 326 — crossing rejected rows

**4 landings, 14 parks held or improved, 0 pins, 0 devices, 0 new flag groups.**
Progress **4,867 → 4,871 of 5,710 (85.3%)**; remaining 843 → 839, parked 751 →
747. Every installed park figure independently re-measured by `parkcheck`.

| brief | bank / theme | result |
|---|---|---|
| A | copy-collapse family | **`Func_801965c` 10 → 0**; `Func_801bcd4` 48 → **16** |
| B | `rom_15000` | **`Func_8019944` 5 → 0** |
| C | `rom_a1000` | **`Func_80ad5b4` 4 → 0** |
| D | `rom_8a000` | 3 parks, every verdict refuted |
| E | `rom_77000` | `Func_807808c` 4 → 3 device-free |
| F | `rom_b5000` | 3 parks bounded from source |
| G | `rom_9000` | **`Func_801219c` 4 → 0**; `ActorCmd_Loop` 24 → 5 |
| H | `rom_c9000` | `Anim_Venus` 22 → 20 |

## The pattern of the batch, found three times independently

**Three briefs landed or advanced by crossing an edit that was already in a
park's REJECTED list with another rejected edit.**

- **Brief A, `Func_801965c`.** Closed on the byte-offset carrier crossed with
  `i < n - 1` and no `n--`. **Both were in the park's own negatives list**, and
  the park had written the answer in a parenthetical — *"loop.c's invariant hoist
  costs an insn"* — while its other body sat exactly one instruction short.
- **Brief B, `Func_8019944`.** Statement order alone is **12, worse than the
  park's 5**; declaration order alone is **exactly inert at 5**. Together, **0**.
- **Brief F, `Func_80b6d30`.** A bottom-tested `do { … } while (++j <= 1);` is
  **exactly inert at 4 with a bit-identical reload trace** — free, and invisible
  to one-at-a-time screening. With the instrument it reaches the ROM's exact
  instruction count.

And brief B explained *why* this keeps happening:

> **A TIE-BREAK LEVER IS INVISIBLE UNTIL SOMETHING ELSE MANUFACTURES THE TIE.**
> Declaration order reaches the allocator only through `allocno_compare`'s last
> rung (`global.c:617`, `return v1 - v2`), and that rung is **dead whenever the
> priority arithmetic above it separates the allocnos.** One edit made two
> priorities exactly equal (8333 against 8437, 1.2%); the other then decided them.

So an edit acting only on a tie-break rung measures inert in every body where the
rungs above are not tied. **Its "inert" row is evidence the TIE was absent, not
evidence about the lever.** That applies equally to `qty_compare_1`'s tie on
quantity number, `allocno_compare`'s tie on allocno index, and
`rank_for_schedule`'s bottom `INSN_LUID` rung.

> **When a park's inert list contains a tie-break-only edit, the question is not
> "does this lever work" but "what else would have to tie first".**

## Mechanisms read from the compiler

- **`expr.c:7340`** (*"put a constant term last and put a multiplication first"*)
  sits on a path reached **only** when the modifier is
  `EXPAND_SUM`/`EXPAND_INITIALIZER` **and** `mode == ptr_mode`
  (`:7290-7292`, else `goto binop`). An `ARRAY_REF` address **is** `EXPAND_SUM`,
  so inside a subscript the sum is reordered and `:7324` puts the MULT first
  unconditionally — while a plain `layer += E` falls to `goto binop`, where
  `expand_binop` keeps the written order and layer's own pseudo becomes the add's
  destination. **One statement, all four of `Func_801219c`'s differing
  encodings.** Its park's recorded negative `q = layer + (…)*4` at 7 is a
  *different* edit: a new pointer is a new pseudo.
- **A two-path merge variable plus an `lsl`/`asr` residue calls for ONE
  ASSIGNMENT, not a re-spelled operation.** `Func_80ad5b4` landed on
  `v = (short)(flag != 0 ? (0x8000 | b) : b);` — a ternary sets `v` once, so
  `REG_N_SETS == 1` lets `nonzero_bits` kill the narrowing pair. That park had
  measured **eleven** merge spellings, all of which kept two.
- **`make_regs_eqv` (`cse.c:1026-1035`) compares LAST MENTIONS**, and a
  `default:` arm is laid out after every other arm — so `default: break;` instead
  of `default: return s;` took `Func_801bcd4` from 48 to **16** with relocations
  now identical, removing the cross-jump its park said had "no second door".
- **New bound, full citation chain: on Thumb no post-reload pass can replace a
  `const_int` below 256 with a register.** `reload_cse_simplify_set`
  (`reload1.c:8046-8056`) and `reload_cse_simplify_operands` (`:8219-8224`) weigh
  `arm_rtx_costs` returning **0** for such a constant in a SET
  (`arm.c:2078-2081`) against `REGISTER_MOVE_COST` **4** for `HI_REGS`
  (`arm.h:1280-1285`) and `rtx_cost(reg,SET)` **1**.
- **`spill_regs[]` is built in ASCENDING hard-register order**
  (`reload1.c:3527-3532`), **not `REG_ALLOC_ORDER`** — so do not reason about the
  reload cursor with `{3,2,1,0}`. Brief C used this to show one park's residue is
  **not** the cursor at all: its first two tail reloads agree with the ROM, so the
  cursor stands in the same place in both builds, and the variable is **r3's
  availability at one insn**.
- **`global.c:1894-1901` prints `hard_reg_conflicts` after the allocno list**, so
  a trailing `3` there is **hard register r3**, not a third allocno. That
  corrected brief F's own framing of its named test.
- **`arm_adjust_cost` (`arm.c:2416-2453`) never RAISES a cost**, and the `core`
  unit (`arm.md:253-264`) gives delay 2 only to `load`/`store1` — so no cost-2
  edge exists for a register `subs`.
- **`rank_for_schedule` has a `last_scheduled_insn` CLASS rung
  (`haifa-sched.c:4069-4095`) ABOVE the dependent count** that two parks reasoned
  on. One dump shows an insn going from second-worst at t=10 to best at t=11 with
  priority and dependent count unchanged.
- **A symbol address and a large integer constant are different operands to the
  scheduler**: the symbol becomes a real pool MEM carrying memory dependences, the
  integer stays a bare `const_int` with none.
- **`write_dependence_p` tests `MEM_VOLATILE_P` on BOTH operands first**, so a
  volatile store does not buy a memory dependence.

## Park figures that are local optima pointing away from the fix

Brief D found the sharpest instance. `InitMapActors` holds at **14**, and **two
of its matching encodings match by coincidence**: in the ROM `a` is in r2 and `b`
in r1 — the reverse of ours — so two `ldrb` tests agree while testing different
variables. The `a->flags`-first variant reads **16**, and its diff is ours *plus
exactly those two lines*: same four runs, same single cause. **Fixing the
registers is 14 → 11 on the installed body and 16 → 9 on the other.**

> **A lower figure can be a worse starting point.** Check whether a matching
> encoding matches for the right reason before treating it as ground won.

Brief C recorded the same trap from the other side: dropping a local reads **4,
the same figure as its park**, at identical encoding count while emitting three
`strh` against the ROM's two.

## Three errors of mine, and they are one error

1. **I told briefs that `objcmp`'s pad guard had caught a real misalignment on
   brief C's body. It had not — that was the RELOCATION FALSE POSITIVE.** `_insns`
   was stripping the reference's trailing *relocated* pool word (a relocation
   dumps as `00000000`), so the guard reported the pool transposition — the defect
   itself — as a length defect. **I relayed a buggy tool's output as evidence the
   tool worked.** Only brief G's mid-stream case was a real catch.
2. **I told brief H that ZERO `Anim_*`/`BaseAnim_*` sources have landed and not to
   look for one. 148 are landed** against 52 parked, and **`Anim_Hail` is landed
   in `Anim_Venus`'s own upstream module** — that park's header cited it by name,
   and `tools/upstream_module.py` prints it unprompted. The claim came from batch
   325 checking **filenames** (all split-named) rather than definitions.
3. **I told two briefs to attack `allocno_compare` on `DisplayMenuArrowCursor`.**
   The pseudo that must move is **block-local and absent from `.18.greg`'s allocno
   list**, so global-alloc never sees it. The real decider is `find_free_reg`
   (`local-alloc.c:1934`, `:2026`): r3 is first in `REG_ALLOC_ORDER` and **free**
   — an empty exclusion set, not a priority loss.

All three are the same failure: **a check on names or on a tool's summary
standing in for a check on definitions or on the thing itself.** It is now the
third, fourth and fifth instance this session, after the `extern`-as-definition
detector and its K&R sibling.

> **A false NEGATIVE is worse than a false positive here, because it tells an
> agent not to look.** Error 2 suppressed the best available evidence for two
> briefs across two batches.

## Tooling

- **`objcmp`'s pad guard, twice fixed.** It must not strip a trailing zero whose
  offset **carries a relocation** (that was error 1), and it must also catch a
  **mid-stream** pad — `.align 2` before a pool inserts a 2-byte zero that is not
  trailing, which is a **fourth** variant of the padding trap and the reason
  `ActorCmd_Loop`'s park had recorded its 24 as a distance. The mid-stream test is
  deliberately **offset-independent**, because in `--func` mode the encoding list
  starts at 0 rather than at the function's real object offset, so a `% 4`
  alignment test is unreliable. Verified on five cases including both false
  positives already found.
- **`parkcheck` gains `CLAIM5`** for the mirror word order (`4 of 103 encodings`
  as well as `4 encodings of 103`), because three of brief E's headers read
  `NO CLAIM` with correct measurements in them — the same defect as the batch-324
  backfill. Corpus-checked: **3 parks newly parse a claim, 0 existing claims
  change value.**
- **`install_batch`'s retire was silently failing.** It announced "retire park …"
  and `git rm` refused the file because the repoint guard added the previous day
  modifies a sibling park during the *splits* phase, leaving it
  modified-but-unstaged by the *install* phase — and the return code was
  discarded. Every batch-325 landing involving a split hit it, so three parks
  survived their own functions landing, one still claiming "11 differing
  encodings" for byte-identical code. Now uses `-f`, checks the return code,
  confirms the file is gone, and exits loudly otherwise.

**And a discipline breach**, caught by brief E: `objcmp` — the measurement
authority — was modified **twice while seven agents measured against it**.
Verified blast radius is nil for figures (both edits touch only `_insns` and its
print, which feed exclusively the `INSTRUCTION COUNT` warning; encoding lists,
the differ count, SIZE, RELOCATIONS and the OK verdict are untouched). It should
not have happened mid-batch.

## Agent losses, and what checkpointing bought

Three agents stopped early: two API timeouts and one accidental kill.

- **Brief G timed out** before writing a manifest. Because it had checkpointed, I
  reconstructed `Func_801219c`'s landing from its `NOTES.md`, verified it
  independently with `--func` *and* `--whole`, and installed it.
- **Brief F was killed by accident** ~45 minutes in. Its `NOTES.md` and every
  variant tree survived, so the replacement agent was handed the full state
  instead of starting over.
- **Brief C timed out and had written no `NOTES.md` at all.** Its *figures* were
  recoverable from the variant files; **its reasoning was not.** Why `ad_v8` works
  where v2–v7 do not had to be re-derived by the resumed agent.

> **Checkpointing is now load-bearing twice over and has failed once.** The next
> brief should require `NOTES.md` to exist before the first measurement rather
> than asking for it as a habit.

## Carried forward

- **`Func_8020b64`** stays at 50 but its blocker is down from four sites to one:
  the `cse_insn` swap carries **two structural gates besides the liveness test**
  (`cse.c:5980`, `:5992-5994`), and **one ordinary statement between two chain
  members defeats it outright, in cse and cse2, whatever the liveness.** Four
  bodies now carry the chain past cse2 and two past combine; before this batch
  nothing got past cse2 at all. Leg 3 is open.
- **148 landed `Anim_*` relatives** for 52 parked animations — use
  `tools/upstream_module.py`, which names the landed module-mate directly.
- **`Func_801bcd4` at 16**, blocked by cse2's extended basic block (no
  `CODE_LABEL` between entry and the early return).
- **`ActorCmd_Loop` at 5**, residue one run, with the register-sharing cause named.
- **`Anim_Attack`'s closed-form pin ladder** (none 5, one pin 3, two pins 0) is
  deferred to pass 3 per the pin-free policy.
- `Func_80b6d30`'s device-free boundary; `Func_8078144`'s complementary 4-and-6
  corners; `Func_80a8f40`'s r3 availability at one insn.
