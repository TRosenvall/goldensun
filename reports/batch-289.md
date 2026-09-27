# Batch 289 — twelve functions from twelve unattempted targets

Gated on a clean `make clean && make -j8 && make compare` → `goldensun.gba: OK` at
`5c4695205413df7db52b9a184815a07783999971`. Every landing gated on its own build + compare,
every split byte-neutral first. `git status` clean.

Three agents at four UNATTEMPTED targets each (the user's request): rom_8a000 and two overlay
briefs. **10 of 12 targets landed**, plus two neighbours that closed with their targets' files
(OvlFunc_898_2009754, and OvlFunc_960_2008f50 out of a park); 2 parked.

## Remaining, after this batch

| | functions |
|---|---|
| **unattempted** | **369** (was 381) |
| **parked** | **571** (was 571: +2 new, −1 retired, −1 cited neighbour landed) |
| not C at all (hand-asm 76, ARM 14) | 90 |
| total still in assembly | 1,030 (was 1,042; −12, reconciled) |

| instructions | unattempted | parked |
|---|---|---|
| 1-100 | 1 | 360 |
| 101-200 | 93 | 141 |
| 201-400 | 121 | 48 |
| 401-800 | 85 | 20 |
| 800+ | 69 | 2 |

## The table

| function | address | new `.c` |
|---|---|---|
| `OvlFunc_898_2009754` | 0x02009754 | `src/overlays/rom_793768/ovl_314_c_c_c_c_c_a.c` |
| `OvlFunc_898_20097ac` | 0x020097ac | `src/overlays/rom_793768/ovl_314_c_c_c_c_c_a.c` |
| `OvlFunc_923_2008fd8` | 0x02008fd8 | `src/overlays/rom_7aa430/ovl_e90_c_c_c_a.c` |
| `OvlFunc_935_2008754` | 0x02008754 | `src/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_c_a_a.c` |
| `OvlFunc_939_200849c` | 0x0200849c | `src/overlays/rom_7c460c/ovl_314_a_c_a_a_c_a_c_a.c` |
| `OvlFunc_945_200837c` | 0x0200837c | `src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_a_a_a_a.c` |
| `OvlFunc_960_2008f50` | 0x02008f50 | `src/overlays/rom_7eaf28/ovl_314_c_c_c_c_a.c` |
| `OvlFunc_960_2009094` | 0x02009094 | `src/overlays/rom_7eaf28/ovl_314_c_c_c_c_a.c` |
| `Func_808df1c` | 0x0808df1c | `src/rom_8a000/rom_8d9a4_a_a_c_c.c` |
| `Func_808f32c` | 0x0808f32c | `src/rom_8a000/rom_8d9a4_c_c_a_a_b.c` |
| `Task_Rain` | 0x08094820 | `src/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_b.c` |
| `Field_Ply` | 0x080994d0 | `src/rom_8a000/rom_97b54_a_c_c_c_a.c` |

Two files hold two functions each (the 898 and 960 pairs): each `.s` had a `.data` tail, so
split_s cut code from data, and the two code pieces it left were merged back into one object
by hand (one linker line removed per section; gated byte-exact).

## Per brief

| brief | area | landed | parked |
|---|---|---|---|
| A | rom_8a000 | **4 of 4** (163-177 insns), Field_Ply with one pin | -- |
| B | overlays | 2 + OvlFunc_898_2009754 | OvlFunc_897_200aeb0 (12), OvlFunc_924_200d244 (40) |
| C | overlays | **4 of 4** + OvlFunc_960_2008f50 (park) | -- |

rom_8a000 has now landed 26 of its last 29 targets across three batches.

## Findings worth keeping

- **A dead store inside a skipped block kills cse's constant reuse** -- a SOURCE route where a
  per-file CSE flag looked necessary. OvlFunc_945_200837c was exact under
  -fno-rerun-cse-after-loop or -fno-cse-skip-blocks; naming the id `f = 0x8a0` and assigning
  `f = 1` inside the conditionally skipped block makes invalidate_skipped_block drop the
  equivalence; flow deletes the store. **Worth trying on OvlFunc_959_2009528 (exact under
  CSE_CFLAGS) before any flag row is added.**
- **`__attribute__((const))` on explicit soft-float calls** reproduces gcc's libcall
  allocation (each call wrapped in a libcall block, named long long constants survive cprop):
  22 → 0 on OvlFunc_935_2008754.
- **A loop-entry `b` to the bottom test can be two test copies merged back** once their reload
  registers agree (Func_808f32c, 146 → 1).
- **Loop-invariant hoisting can hinge on one insn of lifetime** (Task_Rain: lifetime 8 fails,
  9 hoists; `(unsigned char)p[1]` adds exactly that insn).
- **Check the epilogue before blaming allocation**: a park declared `void` whose ROM ends
  `pop {r1}` carries avoidable residue -- the parked OvlFunc_923_2009cb4 went 43 → 40 on
  declaring it `int`, applied this batch.
- Fold puts constants outermost (`r + K + x` → `(r + x) + K`); two statements keep the ROM's
  order.

## Tree changes beyond the functions

- fakematch.txt: Field_Ply (one r5 pin -- the same source is exact under -fno-gcse without
  it), OvlFunc_923_2008fd8 (its booked sibling's r3/r2 stack-argument pins),
  OvlFunc_935_2008754 (r0-r2 pin).
- OvlFunc_923_2009cb4's park: declared `int`, 43 → 40.

## For a decision

- Three functions are now exact under a per-file flag: ColorCycleVFXPalette (-fno-gcse,
  parked), OvlFunc_959_2009528 (CSE_CFLAGS, parked) and Field_Ply (-fno-gcse, landed with a
  pin instead). This batch's dead-store finding may give OvlFunc_959 a source route.

Counts: 571 park files; census parked 571; fakematch.txt 530 rows; parkcheck **101 OK / 0
MISMATCH / 469 UNCHECKABLE**; 4,698 from C.
