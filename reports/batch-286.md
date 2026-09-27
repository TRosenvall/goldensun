# Batch 286 — fifty-eight functions, fourteen of them out of parks

Gated on a clean `make clean && make -j8 && make compare` → `goldensun.gba: OK` at
`5c4695205413df7db52b9a184815a07783999971`, run natively in a Claude Code on the web session
(`GCC296_DIR=/opt/gcc296 AGBCC_DIR=/opt/agbcc`). Every landing was also gated on its own
incremental build + compare before its commit, and every split was gated byte-neutral before
its `.c` was written. `git status` clean.

| | |
|---|---|
| elevated | **58** (44 from scratch, 14 out of parks) |
| parks | **13 new**, **21 retired** (4 stale, 1 class file, 16 landed) |
| whole-file conversions | **23 `.s` files** / 24 functions |
| `.sym` entries added | **1** (`_FILE_1a`) |
| fakematch.txt rows added | **6** |
| `.global` exports added | **17** labels in 5 `.s` files (all beside their labels, no bytes) |
| agents | 16 briefs -- 5 on parks (2-5 targets), 11 fresh at 5 targets each |

census: **1,152 → 1,094** in asm (−58), matching the 58 rows below exactly;
**4,558 → 4,616** from C. The first-round briefs A-E ran five at a time; the user then asked
for **fifteen agents at five functions each**, so briefs F-P were added while A-E finished,
with the head session alone running make, splits, git and pushes.

## The table

| function | address | new `.c` |
|---|---|---|
| `OvlFunc_common1_21c8` | 0x0200ba60 | `src/overlays/common/common1_c_c_b_b.c` |
| `OvlFunc_879_2008238` | 0x02008238 | `src/overlays/rom_779188/ovl_30_c_c_c_b.c` |
| `OvlFunc_879_2008454` | 0x02008454 | `src/overlays/rom_779188/ovl_30_c_c_c_c_b.c` |
| `OvlFunc_880_2008054` | 0x02008054 | `src/overlays/rom_7795e8/ovl_30_c_c_a_a_a_a.c` |
| `OvlFunc_883_20091d8` | 0x020091d8 | `src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a_c.c` |
| `OvlFunc_924_2009164` | 0x02009164 | `src/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a_c_a.c` |
| `OvlFunc_924_20098f8` | 0x020098f8 | `src/overlays/rom_7ac2d8/ovl_f84_c_a_b.c` |
| `OvlFunc_927_2008d90` | 0x02008d90 | `src/overlays/rom_7b4558/ovl_30_a_c_c_a_a.c` |
| `OvlFunc_935_2008704` | 0x02008704 | `src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_a.c` |
| `OvlFunc_946_2009774` | 0x02009774 | `src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_a_a_c_a.c` |
| `OvlFunc_947_2009aa8` | 0x02009aa8 | `src/overlays/rom_7d0e88/ovl_1528_a_a_c_a_c_b.c` |
| `OvlFunc_950_2008898` | 0x02008898 | `src/overlays/rom_7d5838/ovl_30_c_c_c_a.c` |
| `OvlFunc_956_20081c8` | 0x020081c8 | `src/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_a_a.c` |
| `OvlFunc_957_2008a54` | 0x02008a54 | `src/overlays/rom_7e3e08/ovl_30_c_c_a_a_a_c.c` |
| `OvlFunc_965_2009158` | 0x02009158 | `src/overlays/rom_7ef4f4/ovl_30_a_c_c_a_c.c` |
| `Func_8016018` | 0x08016018 | `src/rom_15000/rom_15e8c_a_c_a_a_b.c` |
| `ClearUIRegion` | 0x08016178 | `src/rom_15000/rom_15e8c_a_c_a_a_c_b.c` |
| `Func_801776c` | 0x0801776c | `src/rom_15000/rom_15e8c_c_c_a_b.c` |
| `Func_8017c1c` | 0x08017c1c | `src/rom_15000/rom_178b0_c_b.c` |
| `Func_8017c8c` | 0x08017c8c | `src/rom_15000/rom_178b0_c_c_b.c` |
| `PrintNum` | 0x08017dd4 | `src/rom_15000/rom_178b0_c_c_c_b.c` |
| `Func_801c0dc` | 0x0801c0dc | `src/rom_15000/rom_1aeec_c_a_a_a_a_a_a_b.c` |
| `Func_801cf48` | 0x0801cf48 | `src/rom_15000/rom_1ca1c_c_a_b.c` |
| `Func_801eadc` | 0x0801eadc | `src/rom_15000/rom_1de5c_c_c_a_c_c_c_c.c` |
| `Func_802281c` | 0x0802281c | `src/rom_15000/rom_21dfc_a_c_c_c_b.c` |
| `EquipItem` | 0x08078708 | `src/rom_77000/rom_78414_c_c_a_c_a_a_c.c` |
| `GiveInnateMove` | 0x08078e28 | `src/rom_77000/rom_78b9c_a_c_c_b.c` |
| `Func_8079b24` | 0x08079b24 | `src/rom_77000/rom_79460_c_c_c_c_a_a_c.c` |
| `Func_807a1f8` | 0x0807a1f8 | `src/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_a_c.c` |
| `StartSnow` | 0x08094da0 | `src/rom_8a000/rom_944ec_a_a_a_a_c_c_a_b.c` |
| `Func_80955b0` | 0x080955b0 | `src/rom_8a000/rom_944ec_a_a_c_c_c_a_b.c` |
| `Func_80981b0` | 0x080981b0 | `src/rom_8a000/rom_97b54_a_c_a_a_a_c_c_a.c` |
| `Func_809b11c` | 0x0809b11c | `src/rom_8a000/rom_9ad70_c_a_c_a_b.c` |
| `Func_809ba90` | 0x0809ba90 | `src/rom_8a000/rom_9b698_c_c_a.c` |
| `Func_8010d48` | 0x08010d48 | `src/rom_9000/rom_108e4_b.c` |
| `Func_8012350` | 0x08012350 | `src/rom_9000/rom_1219c_a_c_c_a_b.c` |
| `DecompressSpriteLZ` | 0x0800a97c | `src/rom_9000/rom_a97c_b.c` |
| `Func_80a76d0` | 0x080a76d0 | `src/rom_a1000/rom_a7380_a_c_a_b.c` |
| `Func_80a9c18` | 0x080a9c18 | `src/rom_a1000/rom_a8604_c_c_a_c_a_a.c` |
| `Func_80ad35c` | 0x080ad35c | `src/rom_a1000/rom_ad274_c_a_a_b.c` |
| `Func_80ad508` | 0x080ad508 | `src/rom_a1000/rom_ad274_c_a_a_c_b.c` |
| `Func_80ae778` | 0x080ae778 | `src/rom_a1000/rom_ad274_c_c_c_b.c` |
| `Func_80ae7fc` | 0x080ae7fc | `src/rom_a1000/rom_ad274_c_c_c_c_b.c` |
| `Func_80b08b8` | 0x080b08b8 | `src/rom_b0000/rom_b0070_a_a_c_c_a_a_a_a.c` |
| `Func_80b6b40` | 0x080b6b40 | `src/rom_b5000/rom_b5a0c_c_c_a_a_a_c_c.c` |
| `Func_80b6c08` | 0x080b6c08 | `src/rom_b5000/rom_b5a0c_c_c_a_a_a_c_c.c` |
| `Func_80b78e4` | 0x080b78e4 | `src/rom_b5000/rom_b7410_a_a_c_c_a_a_b.c` |
| `Func_80ba978` | 0x080ba978 | `src/rom_b5000/rom_b9b30_c_a_c_a.c` |
| `Func_80bad7c` | 0x080bad7c | `src/rom_b5000/rom_b9b30_c_c_b.c` |
| `Func_8003d28` | 0x08003d28 | `src/rom_c0/rom_3d04_a.c` |
| `Func_80064b8` | 0x080064b8 | `src/rom_c0/rom_5cf8_a_a_c_c.c` |
| `Func_80cdb24` | 0x080cdb24 | `src/rom_c9000/rom_cd508_a_c_b.c` |
| `Func_80d6888` | 0x080d6888 | `src/rom_c9000/rom_d6504_a_c_c_c_c_b.c` |
| `CreateSummonSprite` | 0x080dbb24 | `src/rom_c9000/rom_dbb24_a.c` |
| `UpdateScreenShake` | 0x080e155c | `src/rom_c9000/rom_e0564_b.c` |
| `Func_80e46f0` | 0x080e46f0 | `src/rom_c9000/rom_e3958_c_c_c_c_b.c` |
| `Func_80f0538` | 0x080f0538 | `src/rom_f0000/rom_f0254_c_b.c` |
| `Func_80f7e60` | 0x080f7e60 | `src/rom_f6000/rom_f6008_c_c_b.c` |

Sibling `.s` pieces produced by each split are in the commits, and `stage1.ld` / the overlay
`.ld` files are rewritten to match. `Func_8015fb8` is not in the table: it was already C; it
is now a gcc **nested function** inside `Func_8016018` in the same file (below), so its
symbol is the local `Func_8015fb8.0`.

## Per brief

| brief | targets | landed | parked / no change |
|---|---|---|---|
| A | 2 parks | **2** (Func_80ba978 from 1 of 273, OvlFunc_947_2009aa8 from 2 of 152) | 0 |
| B | 2 parks | **2** (OvlFunc_924_2009164 from 12, Func_801776c from 6) | 0 |
| C | 3 parks | **2** (both pinned or barriered: fakematch) | common1_1078, header only |
| D | 2 fresh | **2** | 0 |
| E | 5 parks | **5 + 2 bonus** (the whole pre-header-load-merge class) | 0 |
| F | 5 fresh | **4 + 1 bonus** (OvlFunc_879_2008454 out of its park) | common1_1ecc at 3 of 100 |
| G | 5 fresh | **3** (one is the nested-function merge) | 20099f0 (37), 200c520 (87) |
| H | 5 fresh | **5** | 0 |
| I | 5 fresh | **2** | 8021cb8 (14), 8022768 (77), 8022a7c (nested; needs its parent) |
| J | 5 fresh | **4** | 807a3a8 (68) |
| K | 5 fresh | **3** | Field_Cloak (3), 80113e4 |
| L | 5 fresh | **3** | 80a77a4 (8), 80114a0 (55) |
| M | 5 fresh | **5** | 0 |
| N | 5 fresh | **5** | 0 |
| O | 5 fresh | **3** | ColorCycleVFXPalette (EXACT at -fno-gcse), 8005ee0 (19) |
| P | 5 fresh | **5** | 0 |

**69 targets + 3 bonus = 72 attempted, 58 landed, 13 parked, 1 unchanged.** The fresh
61-100-instruction band is now **empty** (51 → 0) and `pickable.py` has no candidates.

## Findings worth keeping

**The pre-header load merge is scheduling, and `do { } while (0)` fixes it.** Its class file
said "no known fix". In `.19.flow2` the pre-header order is already the ROM's; sched2 moves
the counter's `mov #0` into the load-use stall, leaving the load as the last insn before the
loop's unconditional `b`, and jump2's `find_cross_jump` (jump.c:1428) matches it against the
load before the loop label and sinks it into the loop. The empty do-while is a total
scheduling barrier (batch 282) that emits nothing, so the tails stop matching. All three
members landed and the class file is retired. **When cross-jumping merges a pair the ROM
keeps separate, check whether sched2 created the matching tail.**

**"Three local-alloc quantities give dst = r3" is a gcc-2.96 bug.** Traced under gdb in
`find_free_reg`: for exactly three quantities `block_alloc` hand-sorts instead of calling
qsort, and passes QUANTITY NUMBERS rather than `qty_order[]` slots to `qty_compare`, so the
0/1 swap runs twice and cancels. Two, or four or more, sort correctly. The lever is the
count: take the block off three (OvlFunc_924_2009164 moved its flag constant out).

**gcc nested functions are in this ROM.** A function that saves r9, reads it without writing
it, and whose every caller does `add rN, sp, #K / mov r9, rN` before the `bl`, is a nested
function reading its static chain. Written nested in its caller, in one TU, gcc-2.96 emits it
first (so the ROM order falls out) and the chain save comes out with no binding and no
volatile slot. Func_8016018 + Func_8015fb8 landed this way, replacing a transcription;
Func_8022a7c is parked waiting on its ~715-line parent; `rom_23178_a_a_a_a_c_a_c_b.c` and
the Func_80270d8 / 80e73a0 / 80be18c parks likely share the shape. objcmp cannot score these;
`make compare` is the check.

**jump.c's `duplicate_loop_exit_test` copies an exit block of <= 20 insns, and WHEN it runs
decides the loop.** Short at jump1: copied before loop.c, loop "ignored due to multiple entry
points", never strength-reduced. Over 20 at jump1, under 20 after loop.c: copied by the later
jump pass, giving a pre-check plus duplicated found-block on a REDUCED loop. GiveInnateMove's
switch was the element type (an HImode struct-field store is 27 insns, a plain u16 view 17).

**A callee's return type orders argument moves and the caller's own return.** A value-
returning call sets r0 in RTL; a `void` one does not, so a later write to r0 depends on the
pre-call argument copy instead, which changes that copy's dependent count and so
rank_for_schedule's tie. Seen from both sides (Func_8017c8c's argument moves; Func_801eadc's
own `mov r0,rN`). Permuted moves in the same registers: try `int` on the callee.

**Alias sets decide sched2 ties, in three spellings this batch.** (1) A union cast on the
BASE pointer of a bitfield store (`store_bit_field` takes the base object's alias set,
expr.c:5008) added one anti-dep and won OvlFunc_947_2009aa8's LUID tie. (2) A char-typed
store, alias set 0, placed where the ROM keeps it, is an ordering pin (Func_809ba90, 76 of
76 against 70). (3) A union on ONE load gives it alias set 0 alone, when
-fno-strict-aliasing fixes all but one pair (Func_80ad35c).

**Which load GCSE copies depends on its RTL shape**: a byte member assigned to a promoted
local is `(zero_extend:SI (mem:QI))`, hashed apart from the `(mem:QI)` reads in compares;
`x | x` is not folded by the front end and becomes a QI load GCSE copies (Func_80ba978, 1 of
273 → exact).

Other levers, one line each: `asm volatile("")` cuts a two-sided priority wall in one sched2
region (fakematch); the inline-parameter lever stops `&buf` living across calls; derive a
running value from the counter when setup order is wrong and permutation is inert; a
`volatile short *` store keeps base + offset; a member array keeps base and offset registers
apart; `(u8)(x & m)` picks the AND's destination; post-increment position flips a source-order
tie; shift arguments in place (107 → 7); a pooled byte-store zero comes from a halfword-zero
pseudo in the same EBB; `(unsigned short)v == move` keeps a later store on `v`; a .greg
"Register N now in 12" is a reload eviction; `s8` in `gba/types.h` is UNSIGNED `char`.

**Declaration order was NOT inert once** (Func_8079b24, 17 → 11 swapping two
register-resident locals), a counterexample to batch 285's 180-compile negative. It stays a
lever to try last.

## Tree changes beyond the functions

- `tools/objcmp.py`: a private temp directory per run (the fixed `/tmp/objcmp` let parallel
  agents read each other's objects), and **`--whole`**, the whole-TU harness batch 285 asked
  for: reference assembled as built, candidate with the Makefile's trailing align, a
  per-function breakdown, and the section tail.
- `file_table.sym`: `_FILE_1a = 0x1a`. It is the argument of `__GetFile`, so the file-id
  namespace is the identified one; the ROM pools it; it completes OvlFunc_880_2008054 and
  (with the same edits) its sibling OvlFunc_879_2008454. nm: resolves once.
- `.global` exports: 9 in `rom_e0564.s`, 4 in `rom_e3958_c_c_c_c.s`, 1 in `rom_f0254_c.s`,
  `.L9` in `common1_c_c_b.s` (linked into three overlays; no clash), and `.L68c` / `.L6a0`
  on `.lcomm` lines in `rom_779188/ovl_30_c_c_c.s`. None emits bytes.
- `rom_f6008_c.s` renamed `rom_f6008_c_c.s` before its split, because its `_b` suffix was
  already an elevated neighbour's generated build input.
- fakematch.txt: OvlFunc_965_2009158, OvlFunc_950_2008898, OvlFunc_957_2008a54,
  OvlFunc_927_2008d90, Func_80e46f0, StartSnow.

## For a decision

- **ColorCycleVFXPalette is EXACT with `-fno-gcse`** and 86 of 86 at -O2 (four instructions
  long). Not landed: a per-file GCSE_CFLAGS row needs the "spellings provably cannot differ"
  bar and three failed spellings are not that. Its `.s` also needs a split (six functions and
  .rodata). Park: `src/non_matching/rom_c9000/ColorCycleVFXPalette.c`.

## Corrections I owe the log

1. **Two commits went out wrong and were fixed forward, never rewritten.** `Elevate five in
   rom_b0000/rom_b5000` left two pre-split `.s` files tracked (a shell glob only matched files
   still on disk); `Elevate two in rom_779188` contained ONLY its park deletion, because
   `git add` failed on an emptied directory and the commit ran anyway. Each has a follow-up
   commit. From then on every `git add` was chained `&&` into the commit.
2. **My running total to the user was high by two** (I said 60; it is 58). Func_8016018's
   merge was double-counted, and my own table script missed Func_80ba978 because git records
   its `.c` as a RENAME from the park. The census and a `thumb_func_start` diff between the
   two commits both give 58.
3. Two agent park headers inverted the count ("101 of 104", "19 of 87" -- the MATCHING
   counts); parkcheck caught one, I caught the other, both corrected before commit.

## Open

- Parks now within 20: Field_Cloak 3 of 104, common1_1ecc 3 of 100 (a LUID tie; also needs
  the `_TBL_L5 = .L5` alias), common1_1078 7 of 217, Func_80a77a4 8 of 76, Func_8021cb8 14 of
  92, Func_8005ee0 19 of 108.
- Func_8022a7c: land it nested inside Func_8022b44 (RunItemScreen).
- StartRain and StartEarthquake: reopen with StartSnow's r1-zero DMA3_CLEAR pin and BLD
  do-while (the pin closes residue 3 of the StartEarthquake park).
- Func_80b5a0c and Func_80b7548 (parked): try the `p->a[i + 0x32]` queue spelling.
- Carried from batch 285: OvlFunc_930_20091b0's three decisions, the rom_9000 text/data splits,
  the withheld `.sym` candidates, Verify-recipe backfill (now 469 uncheckable).

Counts: 546 park files (554 − 21 + 13); census parked 545; fakematch.txt 527 rows; parkcheck
**76 OK / 0 MISMATCH / 469 UNCHECKABLE**; 459 available (0 in 61-100, 180 in 101-200, 122 in
201-400, 86 in 401-800, 69 above 800).
