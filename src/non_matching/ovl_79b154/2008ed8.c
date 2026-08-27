/* OvlFunc_907_2008ed8 -- asm/overlays/rom_79b154/ovl_30_c_c.s
 *
 * BLOCKER: HImode / POOL-LOADED ZERO IN ONE ARM ONLY
 *
 * 1 of 44 differing.
 *
 * ROM `ldr r3, =0x0 / strh r3, [r5]` in the INNER branch; we get `mov r3, #0x0`.
 * The identical `*L1d88 = 0;` in the sibling else arm DOES emit the pool load, so
 * gcc is finding an int-typed 0 available only in that one block.  Everything else
 * -- the gState build, both guards, the ldrsh/ldrh pair, the block layout -- is
 * exact.
 * * MEASURED (all 1 of 44 unless noted):
 *   L1d88[0] = 0 / *L1d88 = 0 / *(short*)((char*)L1d88) = 0    1
 *   index-and-deref mixed                                      1
 *   unsigned short array                                      32
 *   early-return restructure                                   4
 *   separate short* local; short z1 = 0; *L1d88 = z1           4
 *   int v = L1d88[0] + 1 split                                20
 *   ++L1d88[0] fused                                           1
 *   literal 0x1e instead of wrap >> 16                         1
 *   -fno-rerun-cse-after-loop                                 10
 *   -fno-gcse / -fno-cse-skip-blocks                          32
 *   -fno-cse-follow-jumps / -fno-expensive-optimizations       1
 */
