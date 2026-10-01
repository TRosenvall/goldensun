/* LuckyWheelsMain -- RECON ONLY, NO FIGURES INVENTED.
 *
 * NO objcmp and NO aligncmp number exists and none is claimed; nothing was
 * compiled against it. There is deliberately no `NON-MATCHING, N of M` line,
 * because inventing one would make this park re-measurable against a figure
 * that was never taken.
 *
 * Verify with, once a candidate exists at the installed path:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f6000/LuckyWheelsMain.c \
 *     asm/rom_f6000/rom_f6008_c_a_e_c.s --func LuckyWheelsMain
 *
 * SHIMS: not applicable, no candidate. The fact that replaces it: THE
 * REFERENCE CARRIES THREE VENEERS, all `bl _call_via_r3`, from indirect calls
 * through a function pointer. A candidate must produce exactly three.
 *
 * ================================================================
 * NO SPLIT IS NEEDED AT ALL -- THE ONLY TARGET IN THIS BRIEF LIKE THAT
 * ================================================================
 *
 * Say this first because it changes the cost of the whole job.
 * `python3 tools/split_s.py asm/rom_f6000/rom_f6008_c_a_e_c.s LuckyWheelsMain
 * --dry-run` reports:
 *
 *     asm/rom_f6000/rom_f6008_c_a_e_c.s holds only LuckyWheelsMain and no
 *     data; convert it directly, no split needed
 *
 * and `python3 tools/datacheck.py` on the file is SILENT. So there is NO
 * three-way split, NO linker-script rewrite, NO `.global` export and NO
 * asm-label capture hazard to screen. Write
 * src/rom_f6000/rom_f6008_c_a_e_c.c, delete
 * asm/rom_f6000/rom_f6008_c_a_e_c.s, done. Every other target in this brief
 * costs a split first, and two of them cost an export and a separate
 * `make compare` verification before the split. That makes this the cheapest
 * target to LAND even though it is the LONGEST to reconstruct.
 *
 * ================================================================
 * MEASURED STRUCTURE
 * ================================================================
 *
 *     asm/rom_f6000/rom_f6008_c_a_e_c.s, lines 17..1095
 *     948 instructions, 82 branches, 81 labels, 130 high-register mentions
 *     frame `sub sp, #0x74`  (116 bytes)
 *     3 _call_via_r3 veneers, takes NO arguments
 *
 * It is the longest of the five and the only one with no parameters, so the
 * declaration list starts clean -- no incoming stack arguments to identify,
 * and the prologue's first store (`str r0, [sp, #0x2c]`) is a RESULT
 * (`galloc_iwram(0x29, 0x60e)`), not a parameter.
 *
 * THE FRAME IS FULLY EXPLAINED AND IT CLOSES EXACTLY. I grepped BOTH stack
 * address idioms rather than leaving it as homework, AND THE SECOND IDIOM IS
 * THE ONE THAT MATTERS HERE -- this function is the live example of the
 * brief's warning. There are exactly three stack-address sites:
 *
 *     add  r7, sp, #0x54
 *     add  r5, sp, #0x38
 *     mov  r7, sp / add r7, #0x30 / str r7, [sp, #0xc]
 *
 * The third is the `mov rX, sp / add rX, #K` form, and a grep for
 * `add rX, sp, #` alone WOULD HAVE MISSED IT ENTIRELY -- the aggregate at
 * sp+0x30 would have looked like part of a hole. Note also that its address
 * is then SPILLED to sp+0x0c, so sp+0x0c is a gcc-made address pseudo and NOT
 * a source-level quantity, exactly as sp+0x08, sp+0x0c and sp+0x10 were on
 * Anim_Ramses in this same brief.
 *
 * That gives the whole 116 bytes, bottom up:
 *
 *     sp+0x00 .. sp+0x0b   outgoing argument area (3 words)
 *     sp+0x0c              gcc-made address pseudo, = &(the sp+0x30 aggregate)
 *     sp+0x10 .. sp+0x2c   EIGHT reload spill slots (the source scalars)
 *     sp+0x30 .. sp+0x37   aggregate,  8 bytes   (expand-time slot)
 *     sp+0x38 .. sp+0x53   aggregate, 28 bytes   (expand-time slot)
 *     sp+0x54 .. sp+0x73   aggregate, 32 bytes   (expand-time slot)
 *     -------------------------------------------------------------
 *     12 + 4 + 32 + 8 + 28 + 32 = 116 = 0x74, exact
 *
 * There is no hole. The two apparent gaps -- 0x34 and 0x3c..0x50, 0x58..0x73 --
 * are the interiors of the three aggregates, which are reached by address and
 * then walked by pointer, so no `sp, #imm` reference to them ever exists.
 *
 * APPLYING THE DECLARATION-ORDER RULES to that map gives the list directly:
 *   - AGGREGATE order is REVERSED, first-declared takes the HIGHEST offset.
 *     So the 32-byte aggregate at sp+0x54 is declared FIRST, the 28-byte one
 *     at sp+0x38 SECOND, and the 8-byte one at sp+0x30 THIRD. Their SIZES are
 *     inferred from the gaps between them and must be confirmed against what
 *     writes them, because an aggregate of the wrong size moves every offset
 *     below it.
 *   - SCALARS sorted DESCENDING by offset give declaration order, so the eight
 *     source scalars are declared sp+0x2c, 0x28, 0x24, 0x20, 0x1c, 0x18, 0x14,
 *     0x10 in that order. sp+0x2c is therefore the FIRST-declared scalar, and
 *     the prologue confirms it against a known quantity: the very first store
 *     in the body is `str r0, [sp, #0x2c]` holding the result of
 *     `galloc_iwram(0x29, 0x60e)`. A first-declared local receiving the first
 *     allocation is exactly what the rule predicts, which is a free check that
 *     the rule holds in this bank before anything is written.
 *   - An unused AGGREGATE still gets a slot; an unused SCALAR does not.
 *   - sp+0x0c is gcc's, not the source's. Do not try to account for it with a
 *     declaration; a declared pointer there would be an extra quantity.
 *
 * WHAT IT IS, from the reference's own header plus the call profile: the
 * setup routine for the prize minigame. It allocates under tags 0x29, 0x2C and
 * 0x2E, loads assets through GetFile and DecompressLZ into 0x6002800,
 * 0x6002D00, 0x6003000, 0x6003500, 0x600B500, 0x6010000 and 0x6016E00, writes
 * palettes at 0x5000080, 0x5000140, 0x5000200 and 0x50003E0, and registers
 * `Task_BlitLuckyWheelsAnim` as a per-frame task.
 *
 *     7x Random            7x Func_80f62b8      6x gfree
 *     6x Func_80f6038      5x GetFile           5x Func_80f61e8
 *     4x DecompressLZ      3x _Func_801e7c0     2x galloc_iwram
 *     2x galloc_ewram      2x __umodsi3         2x _call_via_r3 (3rd elsewhere)
 *
 * TWO `__umodsi3` CALLS ARE A SOURCE SIGNAL: gcc emits the unsigned-modulo
 * helper only for a modulo by a NON-CONSTANT, and `__umodsi3` rather than
 * `__modsi3` means the operands are UNSIGNED. Both of those must be written as
 * unsigned `%` with a variable divisor; a constant divisor compiles to
 * shifts-and-multiply and would show as neither.
 *
 * NINE `_FILE_` SYMBOLS IN THE POOL -- `_FILE_3f`, `_FILE_40`, `_FILE_41`,
 * `_FILE_76`, `_FILE_8f`, `_FILE_91`, `_FILE_93`, `_FILE_a0`, `_FILE_b4` --
 * every one of which is an `(int)&_FILE_xx` through include/file_table.h, so
 * all nine are RELOCATIONS. That makes the relocation SYMBOL SEQUENCE
 * unusually informative on this function and it is the cheapest confirmation
 * that a first candidate has the right program shape. The rest of the pool:
 *
 *     Func_80008d4  Func_80008d8  Func_80f6440  REG_BG1CNT  REG_BG1HOFS
 *     REG_DMA3SAD  REG_WIN0H  REG_WIN0V  REG_WIN1H
 *     Task_BlitLuckyWheelsAnim  ewram_200024c  gBuffer  gPtrs  iwram_3001ad0
 *     iwram_3001d18
 *
 * `gPtrs` is in there, which ties this function to the same allocation-table
 * idiom the battle animations use: src/rom_c9000/rom_d9ab8_c_c_c_c_c_b.c's
 * landed note says `gPtrs[0x2e]` / `gPtrs[0x2f]`, NOT
 * `*(DrawFn *)(gPtrs + 0xb8)`, and src/rom_c9000/rom_ceb30_c_c_c_b.c MEASURED
 * that `gPtrs` must be a DIRECT READ and not a named local. Both facts
 * transfer and save a probe each.
 *
 * THE RUNTIME CODE GENERATOR. The reference's header records that this module
 * calls rom_c9000's `_Func_ed408` twice, with (0x2E, 8, 7, 3) and
 * (0x2F, ?, 7, 3) -- the routine that ASSEMBLES A SPECIALISED SPRITE BLITTER
 * AT RUNTIME and back-patches its own branches -- and that outside rom_c9000
 * this is the ONLY module that uses it. That is the same routine the battle
 * animations reach as `BuildDraw2DFuncEx`, whose landed five-argument form is
 * in src/rom_c9000/rom_d82b0_b.c and rom_dd2ac_c_c_b.c
 * (`BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2)`). Reconcile the two names before
 * writing the extern; a wrong arity here is three wrong arguments and a wrong
 * `_call_via_r3` site.
 *
 * ================================================================
 * THE CENSUS NOTE, AND WHY THIS ONE IS THE LESSON
 * ================================================================
 *
 * This function was recovered by the census bug fix, and it is the function
 * the bug was NAMED for: a park filename was matching function names by BARE
 * SUFFIX, so any park written for some other `*Main` function hid
 * `LuckyWheelsMain` completely. There is NO prior work and NO inherited
 * figure. Park exclusion is by function NAME and the names come from the park
 * HEADERS, so a park whose header line is malformed -- or whose
 * `Verify with:` recipe names a placeholder instead of the real installed path
 * under src/non_matching/ -- is unparseable and therefore never re-measured.
 * That is why this file names its real path explicitly even though no
 * candidate exists yet.
 *
 * ================================================================
 * WHERE TO START
 * ================================================================
 *
 * 1. The frame map and declaration ORDER above are done. What is NOT done is
 *    the three aggregates' TYPES -- find what writes sp+0x54 (32 bytes),
 *    sp+0x38 (28 bytes) and sp+0x30 (8 bytes) and size each from its writer,
 *    because an aggregate of the wrong size moves every offset below it. The
 *    28-byte one is a strong candidate for the 0x1c-stride particle record
 *    (`int x, y, z, vx, vy, vz, t`) that the whole rom_c9000 animation family
 *    uses and that gBuffer -- in this function's own pool -- is an array of.
 * 2. Nearest landed sibling by address: src/rom_f6000/rom_f6008_c_a_e_a_b.c is
 *    landed in this very stem and declares
 *    `extern unsigned char *iwram_3001ef0;`, which settles this bank's extern
 *    style for the iwram pointer block. Read the rest of src/rom_f6000/ for
 *    the `galloc_iwram` / `galloc_ewram` / `gfree` tag conventions.
 * 3. The five register writes are all through io.h macros in this tree
 *    (REG_BG1CNT, REG_BG1HOFS, REG_WIN0H, REG_WIN0V, REG_WIN1H) and they are
 *    HALFWORD stores. Anim_Ramses measured the rule that matters:
 *    `*thumb_movhi_insn` has NO immediate form, so EVERY halfword store of a
 *    constant is POOLED, and a plain literal store is therefore correct. Do
 *    NOT apply lever 9's int-carrier here without first reading which way the
 *    reference goes at each of the five sites -- on Anim_Ramses the carrier
 *    would have broken all six register writes.
 * 4. 82 branches over 948 instructions is ~12 instructions per block, the
 *    ORDINARY density -- so docs/band-800plus.md section 2 does NOT apply and
 *    the standard lever set does. This park makes no prediction about WHICH
 *    lever, because none was measured.
 */
