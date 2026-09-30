# Batch 301 — five landings, and a census bug that had been hiding six functions

Ten agents, 30 targets, all unattempted. Gate green at
`5c4695205413df7db52b9a184815a07783999971` at every commit.

**5 landed, 24 newly parked, 1 existing park corrected, 1 target left at recon only.**
State: **4,764 of 5,710 elevated (83.4%)** — 111 available, 744 parked, 76 hand-written
asm, 14 ARM, 1 unmatchable.

Reconciliation, exact from `census`: remaining 951 → 946 (**5 landed**), parked 720 → 744
(**+24**), available 140 → 111 (**29 left the pool**) — and 5 + 24 = 29. Those before-figures
are *re-measured under the census fix below*; the figures batch 300 published for the same
tree were 135 available / 725 parked, and the difference is the bug, not new work.

## Landed

| function | encodings | shims |
|---|---|---|
| `Anim_Fireball` | 536 | 0 |
| `OvlFunc_916_20083f0` | 422 | 0 |
| `OvlFunc_933_20084e4` | — | 1 pin (booked) |
| `OvlFunc_933_200888c` | — | shares the file above |
| `OvlFunc_891_200905c` | 472 | 19 pins + a `CSE_CFLAGS` rule |

Three of the five are pin-free. `Anim_Fireball` needed six `.global` exports added and
verified **before** the split — `split_s.py` refused the split until they existed and named
that order, which is the tool enforcing the right discipline. `OvlFunc_891_200905c` lands
byte-exact only under `-fno-rerun-cse-after-loop`, so it carries a new `CSE_CFLAGS` Makefile
row with its figures and reasoning; its 19 pins were minimised to a fixpoint (every block
dropped and remeasured, none inert, cheapest drop costing 3 encodings).

## The census bug, found by a reconciliation that would not balance

The batch reconciled to **30** functions leaving the available pool against **29** accounted
for. One function had been marked parked that nobody had parked: `match_stem` splits a park
filename on `_` and accepted any part of three or more characters as a **bare suffix** of a
function name, so installing `Anim_Ray.c` made `BaseAnim_ParticleSpray` — which merely *ends
with* those three letters — count as parked.

Fixing the anchor showed the false attribution was **eight functions wide**, not one:

    BaseAnim_ParticleSpray  BattleMain  FieldMain  LuckyDiceMain
    LuckyWheelsMain         MP2KPlayerMain  SoundMain  UpdateSpriteAnim

`UpdateSpriteAnim` ends with "anim", so every `Anim_*.c` park in the tree hid it; the five
`*Main` functions were hidden the same way. Six of the eight are available work (the other two
are hand-written), and they are **not small**: `LuckyDiceMain` is 2,046 instructions,
`BaseAnim_ParticleSpray` 1,605, `FieldMain` 965, `LuckyWheelsMain` 948, `BattleMain` 645,
`UpdateSpriteAnim` 544. The 800+ band gains three targets and 401–800 gains two.

The fix keeps the two kinds of stem part apart, because they sit differently in a name: an
**address** part (all hex digits) still matches as a bare suffix, since `Func_80cd52c` embeds
`d52c` after `80` and not after `_`; a **name** part must land on a word boundary. Verified
apples-to-apples on one tree: the new rule un-parks exactly those 8, parks nothing new, and
removes nothing from available.

Two lessons, both about method rather than the compiler:

- **A park filename must name a function, not rhyme with it.** This is the third route to the
  same over-attribution `census.py` already documents twice (lesson 6: a park *citing* a
  function; lesson 7: a park's opening sentence naming a neighbour). Each time the effect is
  the same — work that has never been attempted disappears from the list work is handed out
  from.
- **A reconciliation that will not balance is a finding, not an annoyance.** 30 against 29 was
  the entire signal; chasing one function's worth of arithmetic surfaced six available
  functions and corrected two published figures.

## A correction to our own documentation, read from the gcc source

Entries here said or implied `duplicate_loop_exit_test` lives in `loop.c` and runs after gcse.
It is **`jump.c:1137`, called from `jump_optimize`**, and its gate requires an unconditional
jump after the loop note — the RTL signature of a `while`/`for`. So **a `do`-`while` can never
reach it**, which is the mechanism behind an empirical result that was right for the wrong
reason.

It also resolves a loose end recorded as unexplained: because the copy happens *before*
`loop.c`, the duplicated block is already outside the body when `strength_reduce` runs, so a
candidate that misses the transform strength-reduces addresses into givs and loses two high
registers as a **consequence**, not as an independent defect. Its bail conditions give a
checkable tell — the duplicated region is call-free, label-free and at most 20 insns.

`allocno_compare` is now recorded verbatim from `global.c`. The consequence is the usable part:
**`n_refs`, `live_length` and declaration order are the only three inputs a spelling can
move**, and ties break on allocno number. That is why a declaration sweep comes back inert on
a function whose residue is not a tie.

## Register residues are about allocation inputs, not the allocation order

Two parks corroborated the `REG_ALLOC_ORDER` experiment from a second direction, and the rule
is now two-sided:

- a whole-function **low**-register rotation is one **missing** short-lived quantity (batch 300)
- a whole-function **high**-register rotation is one **extra allocno** — on `Anim_Ray`, an `if`
  guard before a `do`/`while` let gcse hoist an address into a fourth high-register allocno,
  rotating three registers across the entire function; writing it as a `while` removed the
  allocno and the rotation with it.

Neither side is an order problem. A pin measured worse **twice more** this batch (`r11` on
`OvlFunc_880_2008de4`: aligned-equal *falls* 39.7% → 35.5% because something else spills;
and on `Anim_Annihilation`, where the value is dereferenced at offsets) — the fourth and fifth
independent measurements that a pin is the wrong instrument for this class. The lever is
reducing competing pressure.

## Levers that transfer

- **`fold` canonicalises a pointer `PLUS`**, so two address reloads swap registers while
  printing identically; integer arithmetic escapes it. The same lever *costs* on `Anim_Froth`,
  in the same bank.
- **Two shiftable constants in one call need dominating-block locals**, or
  `precompute_register_parameters` hoists them ahead of the cheap argument. 52 `.s` files carry
  that ROM shape and none had a `.c` sibling, so it had never been reached; try it early
  wherever a call site shows two shiftable constants.
- **Counters unify across disjoint loops** — four on `Anim_Mars`, five on `Anim_Mercury`, and on
  `Anim_Whirlwind` two shared names across five loops were worth 442 → 464 encodings. gcc-2.96
  gives one hard register per variable, so N loops in one register is one variable, not N. This
  is the converse of the recorded one-variable-per-region rule.
- **One local per stacked argument**, one set per call site, assigned in **ascending** argument
  order. Sharing one pair across six sites gets the registers backwards; reversing the order
  gets the registers right and the order backwards; and naming *only* the fifth argument is
  worse than naming neither — so an "inert or worse" result on one site is not evidence against
  the full rule.
- **A loop bound must sometimes be re-derived, not read through a cached pointer** —
  `Anim_Whirlwind` 466 → **472** and 408 → 46 differing.
- **`while (A && B)` is not `while (1) { if (A && B) … else break; }`** — worth 201 → 453 of 476
  on `Func_80a5788`, and readable off objcmp's **relocation sequence alone** before any
  instruction is compared: the ROM emits the outer test and the setup block last, entering with
  a `b` to it (stmt.c's `while` rotation). Its near-sibling `Func_80a96d8` needs the `break` form.

## Traps recorded

- **An int carrier for a pooled register constant can be wrong and invisible.** Thumb-1 has no
  PC-relative `LDRH`, so gas assembles `ldrh rX,.LC` and `ldr rX,.LC` to the same encoding;
  "fixing" `REG_BLDALPHA = 0x1010` to match the ROM's printed `ldr` cost 8 encodings.
- **`ldmia rX!, {rY}` does not prove a walking pointer.** `Anim_PsyphonSeal`'s prologue is array
  subscripts; writing it that way was worth 437 → 351. Auto-increment is what gcc emits for a
  subscripted access it has strength-reduced — read the preheader, not the load.
- **A u16 constant pools sign-extended** (`.word 0xfffff080` against the ROM's `0x0000f080`) —
  identical mnemonics, different pool words.
- **`mov #3 / neg` is `& ~2`, not `& ~3`** — an off-by-one invisible in the mnemonics.
- **A value stored on three paths is gcse PRE's pseudo, not a source variable.** PRE inserts on
  the edges where the expression is unavailable, so N paths give N stores of one value, which no
  source variable does. Declaring a local for it cost 58 encodings.
- **A dead value the ROM computes is a signal to find its live use elsewhere**, not a spelling
  problem to solve in place.
- **The asm-label capture hazard is a measured wrong-program risk**, not a theoretical one:
  `OvlFunc_common1_1b08` needs `.L4`/`.L5` as externs and its own generated assembly *defines*
  both, so the extern binds to gcc's branch target with no build or link error. Screen by
  generating the candidate's `.s` and grepping for the label before writing any one- or
  two-digit asm-label extern.

## Measurement

- **`objcmp`'s count inflates even when size and count both match.** `Anim_Spire` reads 249 of
  432 where the real figure is 64: one inserted instruction shifts everything after it while a
  matching delete keeps the totals equal. So size-and-count agreement qualifies a figure as a
  distance but does not mean it is undistorted — whenever the hunks show an insert/delete pair,
  `aligncmp` is the ranking view **even when the totals agree**. This narrows the rule twice
  recorded as "saturates when the count differs".
- **The lower number can be the worse candidate**, most cleanly on `Anim_PsyphonSeal`: one lever
  reached 351, a second moved it *up* to 383, and 383 is the one with size and count both exact.
  Fourth instance this week. Rank by size-and-count, then the aligned figure, never by raw count.
- **A relocation FORM is not a residue** — confirmed a third time, by `OvlFunc_933_20084e4`'s
  four pool words carrying `R_ARM_ABS32` against `_AREA_59..5c` where the ROM has bare literals.
  The green compare is the proof.
- **The declaration-order frame rule needs its `expand_decl` corollary**: a word-sized scalar is
  not an object `expand_decl` allocates, so `u32 value;` and `u32 value[1];` both sit at sp+4 at
  any declaration position, while `unsigned char value[4]` lands at 0x90 and makes the whole
  frame agree. A scalar that refuses to move is not a failure of the rule.

## Parked, with the closest first

| function | figure | note |
|---|---|---|
| `Anim_Froth` | **10 of 529** | size, count **and** relocations all exact, pin-free |
| `Anim_Whirlwind` | **26 of 472** | size and count exact; residue is 12 giv slots + 14 on one guard |
| `Anim_Mars` | 37 of 426 | size and count exact, no `RELOCATIONS` line at all |
| `Func_80a9f10` | **48 of 569** | size and count exact; relocation symbol sequence identical |
| `Func_80a4924` | 132 of 505 | size and count exact; one reload `find_equiv_reg` inheritance |
| `Anim_Unsummon` | 415 of 432 | 4 short, relocation sequence exact; residue is one decision |
| `Func_801f200` | 404 of 447 | size, count and relocation multiset exact; a two-cycle swap |
| `Anim_PsyphonSeal` | 383 of 459 | size and count exact; 2 pins would need a fakematch row |
| `Menu_Settings` | 444 of 500 | count exact; the 4-byte deficit is **one pool word** |
| `Func_80f2028` | 463 of 532 | blocker bounded at 7 by 26 laundering barriers (not shipped) |

Fourteen more parked at lower confidence, all `parkcheck`-verified: `Anim_Mercury` (295 of 429),
`Anim_Spire` (249 of 432), `Anim_Ray` (294 of 495), `Anim_Plasma` (378 of 493), `Anim_Prism`
(475 of 514), `Anim_Annihilation` (450 of 459), `Anim_AstralBlast` (475), `Anim_Thorn` (348 of
476, saturated — 296 aligned), `Anim_Quake` (448 of 484, saturated — 295 aligned),
`Func_80a5788` (172 of 476, saturated — **454 aligned, 95.4%**), `Func_80ae2f4` (395 of 461),
`Func_80c02a4` (250 of 468), `OvlFunc_880_2008de4`, `OvlFunc_common1_1b08`.

`Func_809bcf8` is **recon only** — the agent ran out of budget and correctly declined to invent
a figure for it.

## Decisions waiting on the owner

- The `.L4`/`.L5` rename in the shared common1 data file three overlays link — the only remedy
  for the label-capture hazard above.
- `Func_80c02a4` was correctly **not** landed: its 8 pins come from two local copies of
  dma.h-shaped helpers. The right order is the one `DMA3_COPY16_RW` followed — promote on the
  evidence of a second user, then land.
- `_CONST_1f` and `_CONST_200` remain park-only.

## Open

- **Host disk.** The Data volume hit 100% with 1.5 GB free and starved Docker's VM disk, killing
  the daemon for four of this batch's agents. Each recovered by restarting, but there is no
  native fallback — `objcmp` needs `$GCC296_DIR/xgcc` and this host has no `gcc296`. Roughly
  5–10 GB needs freeing, and the candidates identified are all outside this project.
- **`Anim_Whirlwind` at 26 of 472 with size and count exact** is the best next target in the
  tree after `Anim_Froth`; both residues are named and neither is a register-order problem.
- ~30 frontier parks still carry stale or missing `Verify with:` recipes (`parkcheck` reads only
  the first comment block).
- 250 parks carry 445 wrong return-type declarations — documentation debt, not a build risk.
