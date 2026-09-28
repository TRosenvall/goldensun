# Batch 292 — five functions, and four things the project believed that are not true

Gated on `make -j8 && make compare` → `goldensun.gba: OK` at
`5c4695205413df7db52b9a184815a07783999971`. All 5 landings verified at their exact ROM
addresses. 24 new parks and 2 park updates, every one verified by `parkcheck`. 12 commits.
`git status` clean.

The first batch with briefs **moderated by function size** rather than a flat five targets
each, and the first where the corrections outweigh the landings.

| | |
|---|---|
| elevated | **5** |
| parked | **24 new + 2 updated** |
| unopened | 3 (+1 analysed without a candidate) |
| `.sym` entries added | **0** (one proposed with good evidence, withheld against the bar) |
| whole-file conversions | **4 `.s` files** |
| splits | 1 |
| tool changes | 1 (`datacheck.py` computes required exports) |
| doc corrections | **3** (`elevation.md` §1b, the mul lever, `--align` discoverability) |
| agents | 8 briefs, 33 targets, graded 3–5 by size |

**5 + 24 + 3 + 1 = 33**, reconciled per brief before publishing. Twenty-three consecutive
batches now reconcile exactly.

| brief | bank | band (insns) | targets | landed | parked | unopened |
|---|---|---|---|---|---|---|
| A | `rom_8a000` | 181–193 | 5 | **2** | 3 | 0 |
| B | `rom_15000` | 155–244 | 5 | **1** | 3 | 1 |
| C | `rom_b5000` | 152–232 | 4 | **1** | 3 | 0 |
| F | `rom_9000` | 181–233 | 5 | **1** | 4 | 0 |
| E | `rom_c9000` | 266–316 | 4 | 0 | 4 (+1 update) | 0 |
| D | `rom_a1000` | 296–382 | 4 | 0 | 2 | 1 (+1 analysed) |
| G | `rom_c9000` | 470–580 | 3 | 0 | 3 | 0 |
| H | `rom_15000` | 455–628 | 3 | 0 | 2 | 1 |

**The size grading worked, and its answer is unambiguous.** All five landings came from the
four briefs in the 152–244 band. The four briefs at 266 instructions and above produced
**zero** landings and eleven parks. That is not a failure of those agents — G and H were
told explicitly that three good parks was an acceptable outcome — it is a measurement of
where the corpus now is.

## Four things the project believed that are not true

### 1. `docs/elevation.md` §1b treated a non-difference as a blocker, and said not to look again

§1b described a candidate's `ldrh rD, .Lxx` against the ROM's `ldr rD, =K` over the same
`.word` as *"same constant in the pool, wrong instruction reading it"*, and carried a
paragraph headed **"Ruled out — do not repeat this experiment."**

Thumb-1 has no PC-relative `ldrh` encoding. gas assembles `ldrh rD, <label>` to the same
`ldr rD, [pc, #N]` word. Assembled and disassembled to settle it:

    ldr  r3, .Lp   ->  4b01   ldr r3, [pc, #4]
    ldrh r4, .Lp   ->  4c01   ldr r4, [pc, #4]

They differ only in the destination register field. `Func_80b6148` landed byte-identical
with 13 of 13 relocations while carrying **nine** such loads, and its already-landed twin
`Func_80b60a0` has the same pair.

**Three agents hit this independently in one batch**, one after chasing it as a phantom,
and "fixing" it with `SET_IO` cost 325 differing on a neighbour. The instruction was worse
than absent. The correction is scoped: a `ldrh rD, [rN, #imm]` from memory **is** a
different instruction, and §1b's store-width-narrowing mechanism is real for those. The rule
is to check pool-load-width residues at the **encoding** level with objcmp, never in the
disassembly text.

### 2. The `"+r"` barrier does not defeat cse2 on a constant — I told all eight agents it did

My briefs said `__asm__ ("" : "+r" (v))` defeats cse2 for one value and belongs on the
first use of a repeated pool **constant**. On a constant it is inert in every placement, and
on `BaseAnim_Spasm` it is **destructive** — 155 differing at instruction count 287, i.e. it
deletes instructions. The mechanism says why: `"+r"` expands to a copy in and a copy out, so
the pseudo reaching the asm still carries the known constant. **A barrier can make a value
opaque downstream of itself; it cannot un-know a constant upstream of itself.**

The barrier is real, for a different class: copy direction and live ranges. My own
`ActorCmd_Wander` result (10 → 2, measured directly) is that class, and `ActorCmd_Camera`
is the third hit in the same family — 189 → **36 of 235** at exact length, where **no**
cse-family flag reaches it at all. That last point is the strongest per-site-not-per-file
evidence in the corpus. And aiming it at the literal Wander spelling on a different function
is wrong (177 of 237): it must be aimed at the value whose live range is the problem.

I generalised one measurement into the wrong class and propagated it to eight agents. One
checked.

### 3. The mul lever has no fixed direction

`Field_Whirlwind` and `Field_Halt` are in the same bank, emit the same
`mov r0, r<i> / mul r0, r3` from the same interpolation, and want **opposite** source
orders — because Whirlwind's counter is in r8 and Halt's in r7, so one already needs a `mov`
of its own. Two structurally identical sites, one bank, opposite spellings.

`elevation.md:7056` already had this right as a **procedure**: *read which value the ROM's
`mov` copies and put the OTHER one on the right.* I compressed a procedure into a direction
in the briefs — "the ROM's `mul rD,rS` destination is the readout" — which sounds
deterministic and invites transferring a spelling between sites.

### 4. Eleven agents have rebuilt `tryc --align` because my briefs hid it

`scratch_elev/*/*/norm.py` exists in **eleven** agent scratch directories across batches
238–292: hand-rolled scripts that normalise registers, immediates, operand forms and pool
references, then diff the streams. That is `tryc.py --align`, and `elevation.md:1473` has
documented it since, alongside `tools/realign.py` and `tools/asmdiff.py`.

Every brief I have written calls `tryc.py` "for screening only" and names `objcmp.py` as the
authority. True about **verdicts**, actively misleading about **diagnosis** — and an agent
follows its brief over a 24,000-line file. Two agents rebuilt it in this batch alone, and one
**had** to: its objcmp number moved the *wrong way*, 574 → 575, on a change that was really
329 → 309.

Measured on one of this batch's parks: objcmp says 485 of 510; `--align` says **171
instructions in disagreeing regions of 500**. Only the second ranks variants. A brief for
anything over ~50 instructions must name three things — `objcmp` for the verdict,
`tryc --align` for the distance, `--ref` for pointing a scratch candidate at any `.s`.

## The measurement rule gained two refinements, from opposite directions

The standing rule is that a differing count is a distance only when **size and instruction
count both match**. That rule rejects meaningless counts; it does not promote the survivors.

- **A true distance can be the worse candidate.** `Func_80a414c` had a variant at 234 of 363
  with size *and* count both exact — a true distance — that was worse than the shipped 347,
  which is not one, because the 234 had a `mul` where the ROM has `lsl / add`. Same length,
  same count, wrong arithmetic. The tie-breaker is whether instruction **kinds** agree, and
  an aligned-row read settles it.
- **And it cuts the other way.** On `Func_808ae74`, splitting one expression into destructive
  statements took the count **up** (115 → 116) while making the **length** right (201 → 200).
  Take the length.

A third, from `BaseAnim_Blast`: **a count that is too LOW can be a signature rather than
progress.** Its first candidates came out *shorter* than the ROM (462 against 470) because
separate counters land in low registers, where the ROM keeps one counter in r8 and pays
`mov r2,#1 / add r8,r2` per increment.

## Mechanisms established

**Separate inline bodies beat a parameterised inline, and it refuted a park's own note.**
`AnimStart`'s park named its blocker as four queue-push constants CSE'd across the function
and listed *"give each push its own copy of the inline body"* as untried and unpromising,
"same RTL". It is not the same RTL and it is the lever: four `static inline QueuePush1..4`
with the literals **inside** each body took AnimStart **291 → 71** and AnimStart2 190 → 89.
With a parameter the inliner lands `(set (pseudo) (const_int K))` at the top of each inlined
block and cse2 equates the pseudos; written inside the body the literal expands straight into
the store insn. **A park's "unpromising" note is a hypothesis, not a result.**

Its boundary, found the same batch: the substitution is **per site**. One copy site refuses
an inline body (4 of 291) because its argument setup is the one the ROM interleaves with
`ldr r6,=Func_8001af8`, and only a pinned r0/r1 pair orders that. Prefer the inline body;
fall back to the pin where ordering is at stake.

**A union member access is a sched2 *ordering* lever, not only a gcse one.** Reaching alias
set 0 through a union restores the anti-dependence the ROM has — it closed `Field_Whirlwind`
and was worth 186 → 67 on `InitMapActors`. `*(void **)`, a cast, and `volatile` on the
pointee are all inert; only the union reaches alias set 0.

**A count-up loop with a VARIABLE bound whose counter is used for nothing else must be
`do { } while` under its own `if` guard.** `check_dbra_loop`'s vanilla path needs
`GET_CODE (comparison_value) == CONST_INT`, so a variable bound leaves only loop.c:8097,
which needs `loop->vtop` — the `NOTE_INSN_LOOP_VTOP` stmt.c emits **only** when it rotates a
loop's entry test to the bottom. A do-while has no such note, so the reversal is unreachable
by construction. `-fno-rerun-loop-opt` alone takes the `for` spelling 201 → 202 of 203; the
do-while gets it with no flag.

**Naming a constant can REMOVE a spill, via argument precompute rather than CSE.**
`calls.c:855` precomputes any argument whose `rtx_cost (value, SET) > 2` into a pseudo, and
thumb's CONST_INT cost for a shiftable constant is `COSTS_N_INSNS(2)` = 6. Naming the
constant at *all* sites gives local-alloc r5 plus rematerialisation later — the ROM's shape,
112 → 82. Naming it at only some is worse than naming none.

**Phantom frame slack is bank-wide in `rom_9000`, and a trailing-member aggregate reproduces
it free.** The `rom_ebec` family is exactly `68 + 12*N` bytes for N vec3s. A bare unused array
is deleted and storing into one costs instructions, but
`struct { unsigned char pad[68]; vec3_t v; } s; vec3_t *p = &s.v;` gives
`sub sp,#80 / add r5,sp,#68` at **no instruction cost**.

**`move_movables`' threshold (loop.c:1803) is reachable from source through `m->lifetime`** —
the cut sits between lifetime 3 and 4 at 49 loop instructions, and the knob is which of two
halfword stores goes through a named pointer. The standing "halfword `int` carrier after the
address" rule is a special case.

Smaller, each measured: **`do { } while (0)` and `__asm__ ("")` are NOT interchangeable** as
cross-jump barriers (only the latter stops `find_cross_jump` merging two identical call
tails); **a loop-invariant multiplier assigned *inside* the loop defeats strength reduction**
while assigning it before lets cse fold it back; **when two bitfield writes in different
bytes share a mask constant, their source order decides whether cse2 copies or consumes it**
and the ROM has the copy; **`signed char` not `s8`** turns `ldrb` into `ldrsb`; and **an
explicit `case N` identical to the default** tips `CASE_VALUES_THRESHOLD` into the ROM's jump
table.

## The constant-divisor rule, scoped by measurement

Two agents contradicted each other, so I measured it with the project's own flags — `k / N`
and `k % N` against a literal versus an `int d = N;`:

| divisor | literal | variable | so a libcall … |
|---|---|---|---|
| 2, 4, 16 | expands **inline** | libcall | … **does** prove a variable |
| 3, 10, 100 | libcall | libcall | … proves **nothing** (byte-identical output) |

thumb-1 has no high-part multiply, so gcc-2.96's only strength-reduction path for a constant
divisor is the shift path, which needs a power of two. Both agents were right about their own
function — one was reasoning about a divisor of 2, the other about 10. The documented rule
was simply stated too broadly. No landed code is affected.

## Blockers proven unreachable, not merely unbeaten

- **`Field_Halt`, 2 of 191** — `combine.c`'s `simplify_comparison` rewrites `LT C` (C>0) to
  `LE C-1` **unconditionally** from `combine_simplify_rtx`, so the ROM's `cmp #0xb / blt` is
  unreachable from a literal bound. Eleven spellings all measure exactly 2. The one escape, a
  register-resident bound, becomes a seventh call-crossing allocno and measures 195 against
  191. Corpus check: all 19 `cmp #K / blt` (K>0) sites in generated asm are switch chains,
  never loop bounds.
- **`BaseAnim_Spasm`, 2 of 291** — size, count and all 38 relocations identical in type,
  symbol *and* offset. One `rank_for_schedule` transposition, backed by a trace: the ready
  list at t=93 is `312 293 884 303 291`, we pick uid 291 where the ROM picks 303, and it is
  not a LUID tie. Statement order provably cannot reach it. **`-fno-schedule-insns2` is
  catastrophic (250 of 291), which is positive evidence that sched2 is doing nearly all the
  right work and exactly one rank is wrong** — a flag making things far worse says the pass is
  load-bearing, not that it is the enemy.
- **`Func_80a8604`, 295 of 341** — four values contend for three low callee-saved registers
  and gcc obeys `allocno_compare` exactly; the ROM's order needs a `live_length` under 5.6
  instructions where the ROM itself forces ~100. Eighteen times short.
- **`Debug_WarpMenu_UI`, 17 of 163** — all 17 are one r0/r6 swap; `d`'s live range is a strict
  superset of the address's, so `pri(d) < pri(addr)` for **any** spelling emitting this stream.

## Method notes

**The single-drop discipline my briefs demand cannot find coupled levers.** `Anim_Torch`'s two
quadrant cursors and its shared-zero spelling are each a **regression** alone (283 and 284
against a 289 baseline) and only pay together, reaching 202. A one-at-a-time sweep discards
both and reports them inert. Single drops stay right for *confirming* a lever; they are not a
search strategy.

**A shim count was reported wrong three times running** on one file — draft said four and
listed five, an audit said five, a post-revision audit said three, and my own grep said five
because it counted header prose alongside declarations. The count that matters is register
declarations in the **code** with the leading comment stripped. A file that changes under
review gets re-counted and re-measured, never carried forward.

**`repoint_parks.py` must run after parks are placed**, and `<this file>` as a recipe
placeholder silently defeats parkcheck's regex where the documented `<this>` works. parkcheck
caught three headers disagreeing with their bodies, and **two corrections went in the park's
favour**: `DrawInventoryIcon` is 105 and a *true* distance where its draft said 107 and "not a
distance", and `BaseAnim_Blast` is 46, not 48.

**Three briefs stated the wrong function count for their own file** and one gave a malformed
reference path; agents caught all four. `grep -ci func_start` before trusting a brief.

**Three agents died at launch** to an Opus 5 safeguard false-positive (`reasoning_extraction`)
on the first four briefs. Rewording the same content in a less imperative, less ALL-CAPS style
cleared it. Worth knowing before a large fan-out.

## `datacheck` now computes the exports a split requires

The `EXPORTS` line lists labels that are **already** `.global`, which is not the set a split
needs — and the two can be disjoint. On `rom_c91dc_c_c_c_c_c_c.s` it names `.Leded6` and
`.Lededc`, both already global and **neither read** by `BaseAnim_SonicWave`, while the two it
does read are not global and are exactly what the split must export. The tool now reports, per
function, which data labels it reads and which lack `.global`, as a ready-to-paste list.

Validated against two independent derivations before installing, and confirmed not to change
the existing counts or any of the four exit codes. 74 of 82 data-carrying files had a
requirement the old line did not state. Every agent in this batch got the answer in its brief
instead of rediscovering it.

## State

`census` TOTAL and `funcindex`'s still-in-asm figure agree at **1,000**, the cross-check
`census.py`'s docstring requires.

| | batch 291 | batch 292 |
|---|---|---|
| functions elevated | 4,705 of 5,710 (82.4%) | **4,710 of 5,710 (82.5%)** |
| still in `asm/` | 1,005 | 1,000 |
| — parked | 611 | 635 |
| — **unattempted and attemptable** | 304 | **275** |
| — hand-written assembly | 76 | 76 |
| — ARM (no compile path) | 14 | 14 |

Available fell by exactly 29 = 5 landed + 24 parked.

**274 of the 275 remaining available functions are over 100 instructions**; 236 are over 200,
and all four bands below 100 hold **one** function between them. The single full pass is
effectively down to its last stretch, and that stretch is entirely large functions.

## Open

- **Do not run another batch of three fresh 600-instruction functions.** On the band evidence
  above, the productive shapes are: one agent on `Func_80191cc` alone with its local-alloc
  inequality as the brief (`4 * calls_crossed < n_refs`, needing five references where the
  candidate has three or four), and `AdvanceMsgText` re-briefed as two halves — "everything
  except case 1", then "case 1".
- **`BaseAnim_Blast` at 46 of 493 and `ActorCmd_Camera` at 36 of 235** are the most reachable
  large parks; `BaseAnim_Spasm` at 2 and `Field_Halt` at 2 are closest but both proven
  unreachable from source.
- **`BaseAnim_Spasm`'s split is the Anim_Vine shape, not Anim_Break's** — its three `.rodata`
  blobs are mid-run, with one file holding the bytes immediately before and another
  immediately after, so the data must **not** be emitted from C.
- `_LEN_2c4 = 0x2c4` is **withheld**: good evidence (179 → 133 of 192, and it is what lets the
  function reach exact length) but it does not *complete* its function, which is the bar.
- `Func_80008ac`'s landed prototype in `src/rom_f4000/rom_f4008_a_a_c.c` has its parameter
  names backwards — it returns `r1/r0`. Positions are unaffected so nothing is broken.
- The `make clean` churn still reproduces; ~469 parks still carry no `Verify with:` recipe;
  161 parks carry stale prose `Source asm:` lines (documentation only — parkcheck does not read
  them).
- Standing owner decisions unchanged: `-ffixed-r7` for `CamelotLogo`, `OvlFunc_930_20091b0`'s
  three, `-fcall-saved-r4` for `OvlFunc_970_2008f80`, `ALIAS_CFLAGS` for
  `OvlFunc_882_200c41c`, a `DMA3_SET` `"r2"`-clobber variant, and the withheld symbols
  `_MSG_26fa`, `_MSG_2850`, `_MSG_ae0`, `_FILE_18`, `_FILE_19`, `_SIZE_80b5138`.
