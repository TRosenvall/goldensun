/* OvlFunc_969_20088b4 -- 0x020088b4 -- REFERENCE ANALYSIS ONLY, NOT A PARK.
 *
 * NO CANDIDATE WAS COMPILED.  No `N of M` line, no objcmp/aligncmp/shimcount
 * figure, deliberately.  Reference facts and prescription only.
 *
 * REFERENCE FACTS.  923 instructions, THREE branches to a label, 4 labels, 255
 * calls, NO `sub sp` frame, THIRTY-NINE mentions of r8-r11 behind the full
 * six-instruction high-save prologue.  Reference file
 * asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_a_c_a.s.  It is an `ovl_314` stem,
 * not `ovl_30`; check whether any Makefile row or `%` pattern reaches that stem
 * before assuming the tree default -O2.
 *
 * CONTROL FLOW: ONE REAL TEST, TWO POOL SKIPS.  `bne .L11d6` at ref line 883 is
 * the only conditional and it is a FORWARD branch over 8 instructions -- a
 * short `if`, not an if/else.  `b .La88` (179) and `b .Ledc` (594) each precede
 * a pool directive (`.pool` and `.pool_aligned` respectively) and are skips.
 * Note `.La6c` at 182 is defined but never branched to from inside the
 * function: it is the pool's own label, not a target.
 *
 * LANDING IS A WHOLE-FILE CONVERSION.  One function in the .s, datacheck.py
 * silent, no split, ONE linker row: overlays/rom_7f6e64/overlay.ld:38.
 *
 * SIX HIGH-REGISTER RANGES, AND THEY ARE MIXED -- WHICH MAKES THIS THE BEST OF
 * THE THREE TO TRY SECOND, NOT FIRST.
 *     r8  at ref 33   from `mov r2,#0` -- a ZERO, 2 reads
 *     r8  at ref 161  from `mov r2,...` in a store burst, 1 read
 *     r9  at ref 196  `mov r3,#0x80 / lsl r3,#7`  (0x4000)   6 reads
 *     r11 at ref 340  `mov r2,#0xd0 / lsl r2,#8`  (0xd000)   2 reads
 *     r10 at ref 359  `mov r3,#0xb0 / lsl r3,#8`  (0xb000)   3 reads
 *     r8  at ref 464  `ldr r2,=0x2013`            (0x2013)  11 reads
 * Function-wide: 109 build sequences against 29 reuses -- 21.0% reuse.  So the
 * commoning mechanism is present and the five-constant prescription written up
 * for 884 applies, with one addition: THE r8 RANGE AT ref 33 IS A COMMONED
 * ZERO, and the 0x2013 range at 464 is a POOLED constant read ELEVEN times.
 * That 0x2013 range is the single highest-value name in any of this brief's
 * five functions -- one declared `int` assigned once at ref 464 should buy the
 * whole r8 range.
 *
 * ALSO CHECK THE POOLED-ZERO DEFECT FIRST, BEFORE ANY OF THAT.  969 has 3
 * `strh` and 1 `ldrh`, which is exactly the shape where
 * `*(short *)p = 0` pools the zero (`ldrh r3, .LN`) because
 * `*thumb_movhi_insn` has no immediate form, while the ROM has `mov r3, #0`.
 * An int carrier -- `z = 0; *p = z;` -- was worth 16 bytes and 5 encodings on
 * one line in batch 307.  The reference's one `.word 0` pool entry is the thing
 * to account for.
 *
 * PRESCRIPTION: as for 884 -- named long-lived constants FIRST to create the
 * reference's number of quantities, pins only at the remaining sites, ascending
 * fills, and no flag row.  Order the names by read count (0x2013 first, then
 * 0x4000, then 0xb000, 0xd000, the zero).
 *
 * CALLEES: 43 OvlFunc_969_2008894, 29 __MapActor_Emote, 28 __Func_8092adc, 13
 * each __MapActor_DoAnim and __CutsceneWait, 11 __MapActor_SetAnim, 10
 * __Func_8093040, and 33 more.  Three are overlay-local
 * (OvlFunc_969_2008894, _20088a8, _2009280) and must be declared extern, not
 * defined -- this file converts only 20088b4.
 */
