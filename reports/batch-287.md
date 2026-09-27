# Batch 287 — thirty-four functions from fifty unattempted targets

Gated on a clean `make clean && make -j8 && make compare` → `goldensun.gba: OK` at
`5c4695205413df7db52b9a184815a07783999971`, in a Claude Code on the web session. Every landing
was also gated on its own build + compare, and every split byte-neutral before its `.c`.
`git status` clean.

**At the user's request this batch took only UNATTEMPTED functions**: ten agents at five
targets each, the smallest fifty left (104-149 instructions, plus one of 39), each agent's
five from one bank. The head session alone ran make, splits, git and pushes.

| | |
|---|---|
| elevated | **34** -- 32 of the 50 targets, plus 2 old parks closed as bonuses |
| parked | **18 new** (the 18 targets that did not close), 2 retired |
| whole-file conversions | **18 `.s` files** / 19 functions |
| `.sym` entries added | **1** (`_MSG_af7`) |
| fakematch rows | **0** |

## Remaining, after this batch

| | functions |
|---|---|
| **unattempted** | **407** (was 459) |
| **parked** | **563** (was 545) |
| not C at all (hand-asm 76, ARM 14) | 90 |
| total still in assembly | 1,060 (was 1,094; −34, reconciled) |

Unattempted fell by 52 on 50 targets because census counts a function NAMED in a park as
parked: Func_80b9724 (the parent in the Func_80b9604 nested-function park) and DrawText (the
other function in Func_8018efc's park) moved without being attempted. Parked: +18 new, −2
retired, +2 of those cited = +18.

| instructions | unattempted | parked |
|---|---|---|
| 1-100 | 1 | 362 |
| 101-200 | 130 | 132 |
| 201-400 | 121 | 48 |
| 401-800 | 86 | 19 |
| 800+ | 69 | 2 |

## The table

| function | address | new `.c` |
|---|---|---|
| `CreateUIBox` | 0x080162d4 | `src/rom_15000/rom_15e8c_a_c_a_c.c` |
| `Func_8017aa4` | 0x08017aa4 | `src/rom_15000/rom_178b0_c_a.c` |
| `Func_801868c` | 0x0801868c | `src/rom_15000/rom_17e88_a_b.c` |
| `SetUIColor` | 0x0801ccc0 | `src/rom_15000/rom_1ca1c_a_a_c_c_a_b.c` |
| `InitMapFlags` | 0x0808ab74 | `src/rom_8a000/rom_8a5f8_a_c_c.c` |
| `InitEncounters` | 0x0808ace0 | `src/rom_8a000/rom_8ace0_a_a_a_a.c` |
| `FindMapActorEvent` | 0x0808d48c | `src/rom_8a000/rom_8ba38_c_a.c` |
| `Func_808ddec` | 0x0808ddec | `src/rom_8a000/rom_8d9a4_a_a_c_b.c` |
| `Func_808e4b4` | 0x0808e4b4 | `src/rom_8a000/rom_8d9a4_a_c_a_a_a_c_a_c_b.c` |
| `Func_8091890` | 0x08091890 | `src/rom_8a000/rom_91584_c_a_c_c_c_a_c_b.c` |
| `Func_80933f8` | 0x080933f8 | `src/rom_8a000/rom_93304_a_a_a_a_c.c` |
| `BattleIntro` | 0x080941e0 | `src/rom_8a000/rom_93304_c_a_b.c` |
| `Func_8095778` | 0x08095778 | `src/rom_8a000/rom_944ec_a_a_c_c_c_c.c` |
| `Func_8096048` | 0x08096048 | `src/rom_8a000/rom_944ec_a_c_a_c_c_a_b.c` |
| `Func_8098070` | 0x08098070 | `src/rom_8a000/rom_97b54_a_c_a_a_a_c_a_c_b.c` |
| `Field_Reveal` | 0x080983a0 | `src/rom_8a000/rom_97b54_a_c_a_a_a_c_c_c_c.c` |
| `Field_Cloak` | 0x08099838 | `src/rom_8a000/rom_97b54_c_a_a_a.c` |
| `Func_809aa98` | 0x0809aa98 | `src/rom_8a000/rom_9a44c_c_c_a_b.c` |
| `Field_Retreat` | 0x0809b208 | `src/rom_8a000/rom_9ad70_c_a_c_a_c.c` |
| `Field_Avoid` | 0x0809b698 | `src/rom_8a000/rom_9b698_a_a_a_a.c` |
| `Func_809b8f4` | 0x0809b8f4 | `src/rom_8a000/rom_9b698_a_a_c.c` |
| `Func_80a1870` | 0x080a1870 | `src/rom_a1000/rom_a1814_a_a_a_c.c` |
| `Func_a1f74` | 0x080a1f74 | `src/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c_c_c_a.c` |
| `Func_80a448c` | 0x080a448c | `src/rom_a1000/rom_a1814_c_c_c_a_a_b.c` |
| `Func_80a4800` | 0x080a4800 | `src/rom_a1000/rom_a47b4_a_c_a_b.c` |
| `Func_80a9598` | 0x080a9598 | `src/rom_a1000/rom_a8604_c_a_a_b.c` |
| `Func_80ac8fc` | 0x080ac8fc | `src/rom_a1000/rom_aa538_c_c_c_a_b.c` |
| `Func_80b7424` | 0x080b7424 | `src/rom_b5000/rom_b7410_a_a_a_c.c` |
| `Func_80b7548` | 0x080b7548 | `src/rom_b5000/rom_b7410_a_a_c_a.c` |
| `Func_80b75dc` | 0x080b75dc | `src/rom_b5000/rom_b7410_a_a_c_a.c` |
| `Func_80b7994` | 0x080b7994 | `src/rom_b5000/rom_b7410_a_a_c_c_a_a_c.c` |
| `Func_80be0b4` | 0x080be0b4 | `src/rom_b5000/rom_bbb0c_a_c_a_c_c_b.c` |
| `Func_80c0774` | 0x080c0774 | `src/rom_b5000/rom_bffb8_a_c_a_a_c.c` |
| `Func_80c10e8` | 0x080c10e8 | `src/rom_b5000/rom_c10e8_a_a_a_a_b.c` |

## Per brief

| brief | bank | landed | parked |
|---|---|---|---|
| Q | rom_8a000 | 4 | Func_808ce74 (2 of 121) |
| R | rom_8a000 | **5** | -- |
| S | rom_8a000 | 3 + **Field_Cloak** (park) | Func_8096ddc (11), Func_8096fb0 (79) |
| T | rom_8a000 | 4 | Func_809b450 (2 of 145) |
| U | rom_a1000 | 4 | Func_80a1ac0 (67) |
| V | rom_a1000 | 2 | Func_80a524c (7), Func_80a68ec (47), Func_80ad40c (73) |
| W | rom_15000 | 3 | Func_8018efc (86, 4 insns long), Func_8019bfc (105) |
| X | rom_15000 | 1 | Func_801a7f4 (17), DisplayMenuArrowCursor (16), DisplayMenuArrowCursor2 (34), Func_8021488 (124 / 4 aligned) |
| Y | rom_b5000 | 3 + **Func_80b7548** (park) | Func_80b6d30 (4), Func_80b9604 (nested job) |
| Z | rom_b5000 | 3 | Func_80b9dc4 (1 of 108), WaitTextPrompt (33) |

64% of targets landed at 104-149 instructions, against batch 286's ~80% at 51-111 -- the
band is harder, as expected, and still well worth it. rom_8a000 was the richest bank (17 of
20). rom_15000's menu/UI functions were the hardest (4 of 10).

## Findings worth keeping

**Both bonus parks closed on batch 286's own levers, applied to parks written before them.**
Field_Cloak (parked last batch at 3 of 104 on a StartTask argument tie) closed by declaring
`StartTask` `int` -- the callee-return-type lever. Func_80b7548 (about 30 spellings tried)
closed on the `p->a[i + 0x32]` queue spelling batch 286's report named for it. **After a batch
finds a lever, re-run the parks it names.**

**loop.c hoists a constant store address only if its load lives long enough.** move_movables
needs `threshold*savings*lifetime >= insn_count` (loop.c:1803): `*(u16 *)K = expr` loads K
first (lifetime 5, hoisted into a callee-saved register), `w = expr; *(u16 *)K = w;` puts it
beside the store (lifetime 1, the ROM's per-iteration `ldr =K`). BattleIntro, ~48 → ~17.

**One loop reversed and its twin not is check_dbra_loop**: `no_use_except_counting` needs
`giv_count == 0` (loop.c:7896). And a count-down loop whose counter the body never reads is
written counting UP, so loop.c reverses it (Field_Avoid, 128 → 47).

**combine_movables lets one constant stand in for another** (loop.c:1448): a single-set
`r = c & 0x1f` pseudo is a movable, merged with two later HImode 0x1f ANDs into one hoisted
`mov`. `r = 0x1f; r &= c;` sets r twice and stays out. TELL: a pooled small constant hoisted
before a loop beside an in-loop `mov #same` (Func_80c0774).

**Reload registers rotate** (`allocate_reload_reg`, reload1.c:5003, `last_spill_reg`),
skipping registers held by live pseudos, and do not follow find_reg's choice. An r2/r3
alternation where we get r3 every time means the ROM had one more spill register somewhere
else. Reorder neighbouring statements so a live pseudo holds the unwanted register.

**Global-alloc priority ties break on pseudo number, and declaration order sets it** -- two
allocnos at exactly 0.54545 in `.17.lreg`, and declaring the pointer local first won (20 →
7). Declaration order is inert until `.17.lreg` shows an EXACT tie; then it is the lever.

**A dead QImode zero can steal an address register**: `s->f1d |= 1` expands to an ior with a
QI zero that combine leaves as a REG_UNUSED set, and local-alloc still gives it r3. Look for
REG_UNUSED QI sets in `.17.lreg` when an address register is off by one (Func_809b450,
now parked at 2; 30 → 2).

**gcc rewrites a bound that is not an ARM immediate** (`x > 0x2fff` → `x >= 0x3000`) and
folds `||` pairs into one unsigned range test; a local per limit, assigned just before its
test, keeps the ROM's `ldr =0x2fff / bgt`.

Smaller: an `int` carrier keeps a zero-extension combine would drop (46 → exact); cse_insn's
copy swap (cse.c:5972) ignores notes, so a do-while does not stop it; an exact division by an
odd number is pointer subtraction on a 7-byte ARRAY type; a libcall with a constant divisor
means the divisor was a variable at expand; a ROM loop that hoists nothing was a goto loop;
`sel = 0;` written early can rotate a whole allocation; BLKmode struct members share one
SImode zero across byte and halfword stores.

## Tree changes beyond the functions

- `message.sym`: `_MSG_af7 = 0x0af7`, on batch 285's `_MSG_333` evidence -- `m` then `m + 1`
  beside the already-admitted `_MSG_182` in the same function; as a literal sched2 hoists it
  above the preceding call; four literal spellings tie at 8-9; it completes the function. nm:
  resolves once. **Withheld:** `_MSG_ad4`, `_MSG_b2c` (Func_80a524c) and `_MSG_1d`
  (Func_8021488), because none completes its function.
- `tools/parkcheck.py`: a recipe written `... REF.s \` / `--func NAME` lost its `--func`; the
  regex now allows the continuation (76 → 84 OK when fixed, 92 OK at batch end).
- GetVenusDjinni's park: a note that its written split plan is stale.

## Open

- Near parks: Func_80b9dc4 at 1, Func_808ce74 and Func_809b450 at 2, Func_80b6d30 at 4,
  Func_8021488 at 4 aligned (3 of them the CreateUIBox zero-argument wall), Func_80a524c at 7.
- Two nested-function jobs: Func_80b9604 (inside Func_80b9724, also superseding 80b9554.c's
  r9 binding) and batch 286's Func_8022a7c.
- ColorCycleVFXPalette's `-fno-gcse` decision (batch 286) still open.

Counts: 562 park files; census parked 563; fakematch.txt 527 rows; parkcheck **92 OK / 0
MISMATCH / 469 UNCHECKABLE**; 4,650 from C.
