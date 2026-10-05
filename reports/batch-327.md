# Batch 327 — four landings, and two frames that were wrong

**4 landings, 29 parks held or improved, 0 pins added, 0 devices shipped, 0 new
flag groups.** Progress **4,871 → 4,875 of 5,710 (85.4%)**; remaining 839 → 835,
parked 747 → 743. Every installed park figure independently re-measured.

Ten briefs at four targets each.

| brief | theme | result |
|---|---|---|
| A | `Anim_*` low band | `Anim_CriticalHit` 15 → 13 |
| B | `Anim_*` mid band | **`Task_SpinCamera` 18 → 2** |
| C | copy-collapse + two at 2 | **`Func_8020b64` 50 → 0**, **`SystemMsgBox` 2 → 0**, `Func_801bcd4` 16 → 13 |
| D | `rom_15000` | 4 held; `Func_801f730` is **one instruction out** |
| E | `rom_a1000` | **`Func_80a9a5c` 9 → 0** |
| F | `rom_8a000` | 4 held, `Field_Halt`'s lattice closed |
| G | `rom_77000` | 4 held; three closed together as unreachable |
| H | `rom_b5000` | 4 held |
| I | `rom_9000` | **`ActorCmd_Loop` 24 → 0** |
| J | `ovl_7ac2d8` | two advanced to true distances |

## The lesson of the batch: a FRAME is more expensive than a wrong diagnosis

`Func_8020b64` sat at 50 for three batches as the flagship of the
"copy-collapse family" — a mechanism this project read correctly out of
`cse.c:5979-6005`, documented at length, and used to organise four legs with a
named open leg 3. **It lands at 0, and the frame was wrong.**

> **The ROM's three insns are not three C names. They are three separate C
> EXPRESSIONS each reading `*src`** — entry test, body store, bottom test. A
> `while` loop has only **two**; `expand_end_loop` manufactures the third **by
> rotating the loop**, and the rotation's duplicate pseudos (`REG_LOOP_TEST_P`,
> printed **`/s`**) live and die inside the loop body's own cse block, so ordinary
> cse propagation folds the chain.

Writing the rotation by hand closes it. The naive three-statement `while` body
**with no named char at all** already read 46 and was exact in 55 of 56
instructions — while **more than twenty park bodies had all varied what was
*inside* the `while`.** The loop's own shape was never a variable.

> **A frame tells everyone where to look, which is exactly its danger.** "Leg 3"
> was a well-evidenced description of a mechanism that was not the blocker. The
> family's other listed members must now be re-examined on their own evidence
> rather than inheriting it.

The second wrong frame was mine, and it is in the next section.

## Bounds that were really levers

Three of this batch's results came from reading a documented bound **in the other
direction**.

**`lang_get_alias_set` returns 0 for a *reference*, not a type** — cited here many
times as a bound (two `char` accesses cannot be given distinct sets). It reads
forward too, and that landed `SystemMsgBox`. `rank_for_schedule`'s
**dependent-count rung (`haifa-sched.c:4096-4107`) is an alias-set fact, not a
structural tie**: on the same byte of the same object, `gState.f22a` gives
`(mem/s:QI … 7)` with **2** in-block dependents while
`((unsigned char *)&gState)[0x22a]` gives `(mem:QI … 0)` with **4**, because
alias set 0 cannot suppress the anti-dependences to two later `strh`. The park
had proved `INSN_LUID` was the only free variable and spent **21 bodies** moving
statements; **the free variable was the type of the reference.** Confirmed from
the other side: `-fno-strict-aliasing` on the unmodified body is byte-identical,
so the figure was never about aliasing *strength*.

**A one-member union reaches an `int`-width field** — refuting a bound I had
relayed to brief B as fact (*"no source spelling reaches alias set 0 here"*).
`Task_SpinCamera` went **18 → 2** on two multiplicative levers: a temp consuming
its operand before the mode store (18→15) crossed with the union (18→12). The
union route was **already documented completely** in `elevation.md`, including
that it beats the flag; the new part is only that it works on an `int`-width
field. That also makes its `ALIAS_CFLAGS` request moot — one fewer owner decision.

**`Func_801f730` is one instruction from matching**, and its bound assumed the
constant must come from a reload of a strength-reduced giv init. With a
walking-pointer **biv** it is an ordinary pseudo that global-alloc puts in r1.
Also corrected: `extendqisi2` (`arm.md:3449`) is a `define_expand` whose Thumb arm
is unconditional, so **every** Thumb QImode sign-extending load expands to
`movqi` + `lsl` + `asr`, and `ldrsb` only ever comes from combine **re-forming**
it — blocked by `can_combine_p` when an insn between load and shift writes a
register used in the MEM's address. Found by crossing the park's rejected
walking-pointer rows with an axis it never varied: **the bump's position.**

## Compiler mechanisms read this batch

- **`arm_adjust_cost` returns 1 for any true dependence whose CONSUMER is a
  `CALL_INSN`** (`arm.c:2430-2432`; haifa calls `ADJUST_COST (used, link, insn,
  cost)`, so the first parameter is the consumer). So **every insn feeding only
  the call sits at exactly `prio(call)+1`** and a pre-call store is capped, while
  a three-hop sign-extend chain is forced higher — and `rank_for_schedule` returns
  on the priority rung. That makes a 3/4 trade **a property of the dependence
  graph** and closed **three** `rom_77000` functions together, as unreachable.
- **A bare `__asm__ volatile ("")` supplies a missing predecessor** via
  `reg_pending_sets_all`, and this tree's documented convention classes it as
  **not a shim** (`elevation.md:15115`, `:24868` — once it names an operand it
  manufactures a reference and becomes a fakematch). That landed `Func_80a9a5c`
  **without** the declined `_MSG_b24`, so owner-decisions entry 1 is **moot
  rather than overturned**.
- **`expand_preferences` (`global.c:828-869`), not `set_preference`**, carries a
  preference along `REG_DEAD` notes cumulatively in note order.
- **`cse_end_of_basic_block` ends a block at a `NOTE_INSN_LOOP_END` only when
  `! after_loop`** (`cse.c:6588-6591`), and **cse2 runs with `after_loop == 1`**.
  A `LOOP_END` costs zero instructions, so a free cse1 boundary exists where a
  plain `CODE_LABEL` does not — the block is extended across a forward
  conditional jump by `follow_jumps` and `skip_blocks`. That explains **every**
  inert label row in one park's history.
- **`fold-const.c:6269-6291`** does `LT 11 → LE 10` as a **front-end tree fold**,
  which is why a 12-toggle RTL flag sweep found nothing — and it makes the
  operator space exhaustible from the fold table.
- **`stor-layout.c:1432-1449`** against `BIGGEST_ALIGNMENT 32`: an unreferenced
  local costs frame space only if its type stays BLKmode.

## Two errors of mine, both the same error

**1. The false "zero landed `Anim_*` sources" claim was written into PARK
HEADERS**, not just briefs — `Anim_CriticalHit.c` and `Anim_Djinni.c` both
asserted it. **148 are landed** against 52 parked. Both headers now carry a
retraction naming the real count, the cause, and the tool that answers it.

**2. The same defect was in my own tool.** `upstream_module.py` printed
`our parks: 0` for a module with nine, because landings are **split-named** under
`src/overlays/` while parks are **address-named** under `src/non_matching/` — a
prefix match finds every landing and no park, and **the landed side being right is
what made the zero look plausible.** Parks are now resolved by their **recipe
symbol**.

The unlock is large: **640 parks resolve to a module across 141 modules, and 130
modules have BOTH landed siblings and parks** — `overlays/ovl_30.s` alone has
**2,261 landed against 124 parked**. Reading landed siblings is this project's
most reliable source of landings, and the tool that surfaces them was blind on
the park side.

Then **I broke that tool while ten agents were running**: the fix opens every
`.s` from `git ls-tree HEAD`, and a file deleted by a split is still in HEAD but
gone from disk, so `index()` died for **every** function. Brief F caught it and
noted that any "no landed sibling" conclusion from this batch was reached without
it. Both call sites are guarded.

> **That is my third mid-batch change to something agents depend on.** So I
> **deferred** the `crossfire` fix below until the last brief reported.

## Tooling

- **`crossfire.py` carried a private copy of the authority.** Its docstring
  promises *"IT IMPORTS tools/objcmp.py, THE AUTHORITY … it cannot drift from
  it"*, and it then had its own `_insn_count()` counting objdump-rendered lines
  out of a `.s` — which received **none** of the three corrections `objcmp`'s
  counter received. After `objcmp` was fixed, `crossfire` still false-positived
  its `INSNS` flag on a **base row**. The duplicate is deleted: counting is now
  `objcmp.insn_pool_counts()`, and `crossfire` reads `objcmp`'s own verdicts from
  the run it already performs.
  > **A tool that states it cannot drift from the authority is exactly the tool to
  > check for a private copy.** The docstring is not the mechanism; the import is.
- **`objcmp` separates POOL WORDS from instructions** — a fifth variant of the
  padding trap, found by brief J: two bodies emitted 22 real instructions, the
  same as the reference, while the line read `ref 26 / ours 25`. They were **one
  pool word short, never one instruction short**, and two earlier batches had
  attacked a pool that was only short because of an unrelated trade.
- **A fifth sighting of the padding trap inside a standing header**:
  `HeightTile_A`'s recorded "32" for the flipped multiply was **misalignment** —
  `objcmp` now prints `ref 35 / ours 36` on every flipped body, so the flip is
  excluded on **length**.

## Rule Zero worked

Last batch, three agents stopped early and the one that had not checkpointed lost
its reasoning permanently. This batch required `NOTES.md` **before the first
measurement** rather than as a habit. **No agent was lost, and every brief
reported having written it first.**

It also paid retrospectively: **`ActorCmd_Loop` landed on a body batch 326 had
already written and never measured** — that brief produced ten crossed variants
of its duplicated-call shape and timed out before scoring any. Three were
byte-identical.

## Carried forward

- **`Func_801f730`, one instruction out** — strike its old reload/`loop.c` bound
  wherever it is propagated.
- **130 modules with both landed siblings and parks**, now visible for the first
  time. `overlays/ovl_30.s` (2,261 / 124) is the largest single opportunity in the
  tree.
- **`ovl_7ac2d8` scoped BY PIECE**: both `ovl_35b8` targets are `dupfuncs`
  representatives and the duplication is whole-piece — solving both functions of
  `ovl_35b8_a_a_c_c_a` in one `.c` is a whole-piece match covering **four**
  functions. Do not give its twins another round until someone finds a C spelling
  for a bare HImode pseudo.
- **The copy-collapse family's remaining members** must be re-examined without the
  frame.
- `Func_801bcd4` at 13; `Anim_CriticalHit` at 13 with `prio(1771)` the target;
  `Field_Halt`'s revisit condition answered **no**; `FieldMove_NoTarget` needs a
  zero-instruction fourth reference to one allocno.
