/* OvlFunc_898_2009754 -- asm/overlays/rom_793768/ovl_314_c_c_c_c_c.s
 *
 * BLOCKER: SCHEDULING AT A JOIN + a linker-script alias
 *
 * 8 of 44 differing.
 *
 * Semantics right, 44/44 lines.  SIX positions are one scheduling permutation at
 * the join: gcc issues `ldr r5,[r6,#0x30]` ABOVE the str [#0xc]/[#0x3c] pair where
 * the ROM issues it below.
 * * The other TWO are not a C problem at all: overlays/rom_793768/overlay.ld has no
 * `__divsi3 = _divsi3_RAM;` alias, though src/overlays/rom_793768/imports.s already
 * exports divsi3_RAM.  The pattern to copy is overlays/rom_7a5214/overlay.ld:79.
 * Same is owed to overlays/rom_7bc690/overlay.ld before OvlFunc_933_2008344 is wired.
 * * MEASURED: -fno-schedule-insns, -fno-schedule-insns2, -fno-rerun-cse-after-loop,
 * -fno-gcse, -fno-cse-follow-jumps, -fno-thread-jumps, -fno-strict-aliasing,
 * -fno-force-mem, -fno-move-all-movables, -fno-strength-reduce,
 * -fno-expensive-optimizations, plus 6 source spellings of the store/add/divide
 * block.  -fno-schedule-insns2 fixes the pair but breaks the two mov r1,#0x1c /
 * mov r0,r5 orders instead -- always 8.
 *
 * UPDATE: the alias half of this is DONE.  overlays/rom_793768/overlay.ld now
 * carries `__divsi3 = _divsi3_RAM;`, as do the other 25 overlay scripts that
 * called a RAM divide helper without aliasing it -- 35 aliases in all, added
 * after checking that each overlay's imports.s actually exports the symbol.
 * The ROM is unchanged by them, which confirms the "emits no bytes" claim.
 * overlays/rom_7bc690/overlay.ld, named above as also owed one, is covered.
 *
 * So the remaining blocker here is 6 of 44, not 8, and it is entirely the
 * scheduling permutation at the join.
 *
 * NOTE FOR WHOEVER RE-ATTACKS THIS: this file is a COMMENT ONLY.  The C that
 * produced the 8-of-44 screen was never saved, so the improvement above could
 * not be verified by re-screening -- tools/tryc.py runs clean and silent on
 * this file because there is no function in it to compare.  The C has to be
 * rebuilt from the .s first.  Park files should carry their candidate; this
 * one is the reason that matters.
 */
