# Batch 288 — eighteen functions from twenty-five unattempted targets

Gated on a clean `make clean && make -j8 && make compare` → `goldensun.gba: OK` at
`5c4695205413df7db52b9a184815a07783999971`. Every landing was also gated on its own build +
compare, every split byte-neutral first. `git status` clean.

Five agents at five UNATTEMPTED targets each (the user's request): rom_8a000, rom_9000 and
three overlay briefs. 17 of 25 targets landed plus one neighbour (OvlFunc_932_200ab58)
that closed with its target's file; 8 parked.

## Remaining, after this batch

| | functions |
|---|---|
| **unattempted** | **381** (was 407) |
| **parked** | **571** (was 563) |
| not C at all (hand-asm 76, ARM 14) | 90 |
| total still in assembly | 1,042 (was 1,060; −18, reconciled) |

| instructions | unattempted | parked |
|---|---|---|
| 1-100 | 1 | 361 |
| 101-200 | 105 | 140 |
| 201-400 | 121 | 48 |
| 401-800 | 85 | 20 |
| 800+ | 69 | 2 |

## The table

| function | address | new `.c` |
|---|---|---|
| `OvlFunc_common1_1814` | 0x0200b0ac | `src/overlays/common/common1_c_a_c_c_a_c_b.c` |
| `OvlFunc_879_2008054` | 0x02008054 | `src/overlays/rom_779188/ovl_30_c_c_a.c` |
| `OvlFunc_880_20091e4` | 0x020091e4 | `src/overlays/rom_7795e8/ovl_30_c_c_c_a_b.c` |
| `OvlFunc_881_2008598` | 0x02008598 | `src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_a_a.c` |
| `OvlFunc_918_200962c` | 0x0200962c | `src/overlays/rom_7a5214/ovl_314_c_c_c_c_b.c` |
| `OvlFunc_932_200aa48` | 0x0200aa48 | `src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_c.c` |
| `OvlFunc_932_200ab58` | 0x0200ab58 | `src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_c.c` |
| `OvlFunc_932_200b484` | 0x0200b484 | `src/overlays/rom_7b9cb4/ovl_30_a_c_c_c_c.c` |
| `OvlFunc_933_2009054` | 0x02009054 | `src/overlays/rom_7bc690/ovl_4e4_c_a_b.c` |
| `OvlFunc_947_2008ddc` | 0x02008ddc | `src/overlays/rom_7d0e88/ovl_314_c_a_a_a_a.c` |
| `OvlFunc_947_2009074` | 0x02009074 | `src/overlays/rom_7d0e88/ovl_314_c_a_a_c.c` |
| `OvlFunc_954_200833c` | 0x0200833c | `src/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a_c_b.c` |
| `InitPlayerPos` | 0x0808cf78 | `src/rom_8a000/rom_8ba38_a_c_a_b.c` |
| `ScreenTransitionOut` | 0x080901c0 | `src/rom_8a000/rom_8d9a4_c_c_a_c_c_b.c` |
| `Func_809641c` | 0x0809641c | `src/rom_8a000/rom_944ec_a_c_a_c_c_a_c_b.c` |
| `Field_Frost` | 0x08099160 | `src/rom_8a000/rom_97b54_a_c_c_a_c_c_c_a.c` |
| `Func_809bb64` | 0x0809bb64 | `src/rom_8a000/rom_9bb64_a.c` |
| `Func_800b074` | 0x0800b074 | `src/rom_9000/rom_b074_a_b.c` |

## Per brief

| brief | area | landed | parked |
|---|---|---|---|
| A | rom_8a000 | **5 of 5** (the largest, 149-161 insns) | -- |
| B | rom_9000 | 1 | Func_801179c (32), Func_8011bf4 (78 / 23 aligned), Func_80123f4 (59), Task_Debug_SpriteTest (exact, needs a .rodata split) |
| C | overlays | 4 | OvlFunc_common1_1354 (16) |
| D | overlays | 3 + OvlFunc_932_200ab58 | OvlFunc_884_200a440 (12), OvlFunc_887_200968c (12) |
| E | overlays | 4 | OvlFunc_959_2009528 (exact under CSE_CFLAGS) |

rom_8a000 has now landed 22 of its last 25 targets across two batches.

## Findings worth keeping

- **The overlay family's `add r3,#0x55 / strb / add r3,#0xf / strh` chain is
  reload_cse_move2add** (reload1.c:8840), needing both address pseudos in one hard register.
  A walk through deliberately BLKmode structs gets it and keeps one SImode zero for both
  stores. Applied to a scratch copy of the parked OvlFunc_883_200dd68 it goes from 109
  differing at the wrong length to 12 at the right one -- **not yet applied to that park.**
- **u16 struct-field stores feed later byte stores** through store_bit_field's (reg:HI)
  constant pseudos, which cse reuses -- the ROM's pooled 0 / 0x20 feeding `strb` after calls.
  Needs one EBB per case; `p` declared per case makes the pseudos local-alloc candidates
  (ScreenTransitionOut, 81 → exact).
- **`return 0;` in each arm stops two identical call tails merging**: jump.c's jump_chain
  only compares labels older than the pass (84 → 3).
- **A ternary can free a reload register**; REG_ALLOC_ORDER is {3,2,1,0}, so a ROM with every
  reload in r0 means r1-r3 held live values.
- **A named constant's birth moved one insn later raises its local-alloc priority**, read
  under gdb in find_free_reg (qty array at symbol `qty`, 40-byte stride) -- 18 → 1.
- A strength-reduced index (`&pos0[i * 4]`), not `pos += 4`, gives a pointer copied into its
  register just before the loop; copy a derived value into its own local rather than
  shifting in place; index an output by its running count.
- gcc-2.96 **ICEs under -fno-rerun-cse-after-loop** (decode_rtx_const, varasm.c:3421) when a
  gState offset is named; per-arm `g = gState` locals avoid it.

## Tree changes beyond the functions

- `const.sym`: `_CONST_b = 0xb` -- the ROM pools __Func_8002f3c's plain-int argument, a
  literal gives `mov`, the same callee takes a pooled 0xc at two other sites, `_CONST_2` and
  `_CONST_0` are the same shape; it completes OvlFunc_879_2008054. nm: resolves once.
- `_TBL_L16 = .L16;` in the three overlay.ld files that link common, beside `_TBL_L10..L13`
  and for the same reason (gcc emits its own `.L16:` in OvlFunc_common1_1814).

## For a decision

- **OvlFunc_959_2009528 is exact under CSE_CFLAGS** (-fno-rerun-cse-after-loop), joining
  ColorCycleVFXPalette (exact under -fno-gcse). Both need a per-file flag row; both parked.
- **Task_Debug_SpriteTest is exact as C** and waits on a hand-ordered .rodata split of
  rom_1219c_c_c.s -- mechanical, deferred for credits, not blocked.

Counts: 570 park files; census parked 571; fakematch.txt 527 rows; parkcheck **99 OK / 0
MISMATCH / 470 UNCHECKABLE**; 4,668 from C.
