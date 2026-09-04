# Batch 189

Seven elevated, four parked. Two of the seven are fakematches and one park is a
near-miss at the byte level.

The functions are not the main result. **This round found that three separate
things this project had been reporting about itself were wrong**, including a
verification claim in the previous batch report, and it discovered a whole class
of functions — 14 of them — that had been counted as available work for months
and cannot be attempted at all.

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `Func_80c0700` | `0x080c0700` | [rom_bffb8_a_c_a_a_b.c](src/rom_b5000/rom_bffb8_a_c_a_a_b.c) | **`do{}while(0)` is a scheduling barrier** |
| 2 | `OvlFunc_884_2008780` | `0x02008780` | [ovl_30_…_c_c_a.c](src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_c_c_a.c) | **fakematch** — try the bare pin *before* the barrier |
| 3 | `Func_80ba918` | `0x080ba918` | [rom_b9b30_c_a_b.c](src/rom_b5000/rom_b9b30_c_a_b.c) | an all-ones mask is a **naming tell** |
| 4 | `BuildDraw2DFuncs` | `0x080cef64` | [rom_ceb30_c_c_c_b.c](src/rom_c9000/rom_ceb30_c_c_c_b.c) | the return-type lever is about **whether r0 is written** |
| 5 | `Func_80da24c` | `0x080da24c` | [rom_d9ab8_c_c_c_c_b.c](src/rom_c9000/rom_d9ab8_c_c_c_c_b.c) | the loop guard's condition code reads the bound |
| 6 | `Func_808b320` | `0x0808b320` | [rom_8ace0_a_a_c_c_c.c](src/rom_8a000/rom_8ace0_a_a_c_c_c.c) | the `gState` base must be a named local |
| 7 | `OvlFunc_943_200b558` | `0x0200b558` | [ovl_30_…_a_a_b.c](src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c_a_a_b.c) | a range test is an ordinary comparison |

Parked: [`Func_80e7338`](src/non_matching/rom_c9000/80e7338.c) and its twin
`Func_80e73a0` (43 of 54, gated on their parent),
[`Func_80a1a40`](src/non_matching/rom_a1000/80a1a40.c) (exact instruction
stream, pool word ORDER only), and
[`Func_80b280c`](src/non_matching/rom_b0000/80b280c.c) (53 of 55,
allocation-order class).

Gated on a clean `make clean && make compare`, and every address above verified
with `tools/checkaddr.py` against `goldensun.elf` and the per-overlay
`overlay.elf`.

## THREE THINGS THIS PROJECT WAS REPORTING WRONG

### 1. Batch 188 claimed a verification it had not done

`reports/batch-188.md` stated that every address in its table had been checked
against the linked ELF. **Four of its fourteen were wrong**, and the pattern is
the whole lesson:

| | |
|---|---|
| `Func_<addr>` / `OvlFunc_<n>_<addr>` entries | **10 of 10 correct** |
| entries with real names | **0 of 4 correct** |

An address-named symbol carries its own answer, so "checking" it is a tautology
that cannot fail. A named symbol is the only case where the check *can* fail —
and it is exactly the case where the address gets guessed from the `.s` file's
stem or from a neighbouring function. **The verification record was therefore
strongest where it was useless and absent where it was load-bearing.**

`0x08091f14` was published for `MapActor_SetPos` while still belonging to
`Func_8091f14`, which is unelevated to this day. Nothing caught it; it surfaced
only because the candidate filter later offered `Func_8091f14` and the collision
became visible.

`tools/checkaddr.py` now parses a report's table, resolves every symbol against
the ELFs, and exits non-zero on a mismatch. **A claim of verification is worth
nothing unless the check can fail.**

### 2. The remaining-function census was wrong three times in one day

Three throwaway scripts, three different bugs, each producing a plausible number:

- **Counting by `.func_end` instead of `.thumb_func_start`.** Hand-written `.s`
  files do not reliably close their functions — `rom_92b8.s` has 9 starts and 6
  ends — so a parser that emits only on an end silently drops the rest. This
  undercounted hand-written assembly as 59 when it is 76, and it is what
  produced the published "28 across 5 files".
- **Assuming one park file per function.** Class parks like
  `arg_interleave_flat.c` cover many functions at once; some parks are named for
  a different address than their subject; some for a source-file stem. Matching
  filenames to addresses found 464 parked when the figure is ~659.
- **Word-boundary matching on park mentions.** An overlay park cites callees as
  `__Func_808b868`, and `\bFunc_` cannot match that because `_` is a word
  character. It also misses every park whose subject has a real name.

`tools/census.py` now holds the classification with all three failure modes
recorded, and `--list <lo> <hi>` prints the available functions in a band so any
number can be spot-checked by hand before it is quoted. That check is what
caught the third bug: it predicted 1 available function in the 1–20 band, and a
hand check of the ten the stricter matcher offered found nine were parked.

### 3. A whole class of functions was never attemptable

**This build compiles no ARM code at all.** Every one of the 935 gcc-2.96 rules
passes `-mthumb`, and none of the 3,521 solved files is ARM. So a
`.arm_func_start` function cannot be elevated without a build path that does not
exist — yet **14 of them across 4 files were being counted as available**, and
they were the entire reason the 1–20 and 21–40 bands looked like they still had
work in them. Both bands are now **0 available**.

`HANDASM` structurally cannot see them: it looks for `mov r12, lr` and
`bx r12`, which are Thumb-era idioms. The ARM ones announce themselves
differently — three-way transfer-width selection by predication
(`ldrccb`/`ldreqh`/`ldrgt`), `rrx`, constant tables read via `adr`+`ldm` rather
than a literal pool, and code that rewrites Thumb BL pairs to relocate itself.
Rejecting on the directive is exact, so none of those tells are needed.

**Why the ROM has ARM at all**, which is the part worth keeping: these routines
are staged in ROM and DMA-copied into IWRAM to run there. `LoadMapCode` in
`src/rom_c0/rom_2e00_c_b.c` copies `FixupRamCode_ROM` using the size the `.s`
exports and then calls it. IWRAM is a 32-bit bus with no waitstates, where ARM
beats Thumb; ROM is 16-bit, where ARM needs two fetches per instruction and
loses. Everything that actually executes from ROM is Thumb. **ARM and
hand-written both fall out of that one decision.**

`Func_80f0008` is the sharp case and nearly got attempted. Seven instructions —
`smull`, two `smlal`, then `(hi << 16) | (lo >> 16)` — read exactly as a
three-term fixed-point dot product returning `acc >> 16`, and gcc-2.96 *does*
compile that to `smull`/`smlal` under `-marm`. It still cannot match: the ROM
holds the accumulator in `(r12, r0)`, which is call-clobbered and so needs no
prologue, where gcc picks `(r4, r5)` and must save r5. `HARD_REGNO_MODE_OK`
(`arm.h:965`) puts no alignment constraint on ARM DImode, so that is not the
obstacle — but DImode still occupies **consecutive** registers, and r12's
successor is r13/sp. No allocation can express it. **`-marm` compiling cleanly
is not evidence the original was C; check whether the register assignment is one
the machine description can even represent.**

## New levers, all measured

- **`do { } while (0)` IS A SCHEDULING BARRIER.** Read from `haifa-sched.c`: a
  loop note makes `sched_analyze_insn` fire `schedule_barrier_found`, which hangs
  a `REG_DEP_ANTI` on every prior use and set. So `SET_IO` and `SET_PALETTE`
  split one basic block into two scheduling regions **without emitting an
  instruction**. It reaches a residue source order cannot: on `Func_80c0700`,
  `add r5` beat `sub sp` by a *fixed* +2 priority gap via an unavoidable WAR, and
  `rank_for_schedule` compares priority first. **A fixed priority gap means a
  missing barrier, not a missing swap.**
- **The return-type lever is about WHETHER the call writes r0**, not what width.
  On `BuildDraw2DFuncs`, `int`, `unsigned int`, `char`, `short`, `void *`, `long`
  and leaving the callee undeclared **all match**; only `void` fails. Pick the
  type the callee actually has and read nothing into the choice.
- **An all-ones mask outside a loop is a naming tell.** `q->f16 |= 0xff` compiles
  to `mov`+`strb` outside a loop, because combine folds `x | 0xff` to `0xff` and
  drops the read-modify-write; the identical text *survives* inside a loop,
  because `loop.c` hoisted the literal into a pseudo before combine ran. So a ROM
  read-modify-write with an all-ones mask outside a loop proves the mask was a
  named variable; inside a loop it proves nothing.
- **A reload scratch register is a statement-order tell.** `.18.greg` shows
  reload takes the *lowest* free register, so the ROM choosing r4 over r1 means
  r1 was already occupied at allocation time. Hoisting the statement that
  occupies it took 6 → 0. Do not reach for allocation-order arithmetic when the
  only residue is a scratch rotation.
- **Try the BARE register pin before the barrier.** The fakematch idiom has two
  strengths and they pull in opposite directions: on `OvlFunc_938_2009450` the
  pin alone is inert (14) and the barrier is exact; on `OvlFunc_884_2008780` the
  pin alone is exact and the barrier is a **regression** that perturbs the
  scheduler three instructions upstream. The pin removes the pseudo entirely, so
  where the constant still flows through one, only the barrier works.
- **Duplicate the whole tail and let cross-jumping take it back.** On
  `BuildDraw2DFuncs`, sharing the trailing store between the `if`/`else` arms
  leaves 28 of 52; writing the second call *and* the store into both arms gives
  8. Same shape again on `OvlFunc_943_200b558`.
- **A register live across a call is not always a named local.** `BuildDraw2DFuncs`
  keeps the `gPtrs` address in a callee-saved register across a call — the exact
  named-local silhouette — and naming it is *worse*: hoisted local 33, per-arm
  local 28, no local at all **8**. The liveness was forced by the control flow.
- **A range test is an ordinary comparison.** `add rN, v, #-lo / lsl #16 / cmp`
  against a pre-shifted bound is gcc's own lowering of `v >= lo && v <= hi` on an
  unsigned short. It does not need hand-writing.
- **The loop guard's condition code reads the bound.** `beq`/`bne` on the bound
  means the source wrote `!=`; a `<` bound compiles the guard as `ble`. Cheap
  enough to be a first-pass read of any counted loop, and it decided two
  functions this round.

## A park worth reading: `Func_80a1a40`

Its **instruction stream is exact** — 49 of 49, 0x80 bytes, the ROM's size, with
the mid-function pool and the `b` over it reproduced. The residue is **pool word
order alone**: `0xffff` sorts first in the ROM and fifth in ours.

Minipool entries sort by the maximum address at which they remain reachable, so
`0xffff` sorting first means its reference has a narrow `pool_range`. Only
`*thumb_movhi_insn` (64) and `*thumb_zero_extendhisi2` (60) qualify, and 64 is
unreachable here, so the ROM's `0xffff` must be a zero-extending **halfword**
load of a pool constant. And no C spelling reaches it: `simplify_binary_operation`
returns `op0` when the mask equals `GET_MODE_MASK(mode)`, and tree-level `fold`
kills the cast form before RTL exists. Eleven spellings all folded the `AND` away
or left it SImode. **A HImode `& 0xffff` does not exist.**

## Discipline note

`split_s.py` asks that a `.global` export be verified green *before* the split
that needs it, so a failure can be attributed to one change or the other. On
`rom_d9ab8` the export loop was automated and ran straight through to the split
without the intervening build. The combination verified green, so nothing is
wrong — but the separation was given up rather than earned, and it is recorded
in that commit rather than glossed.
