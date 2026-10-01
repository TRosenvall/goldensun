/* OvlFunc_884_20097c8 -- 0x020097c8 -- REFERENCE ANALYSIS ONLY, NOT A PARK.
 *
 * NO CANDIDATE WAS COMPILED, so there is deliberately NO `N of M` line and no
 * objcmp, aligncmp or shimcount figure in this file.  Do not read one in.  It
 * carries only measurements taken from the reference and the lever that those
 * measurements prescribe, so that whoever picks it up does not re-derive them.
 *
 * REFERENCE FACTS.  1048 instructions, THREE branches to a label, 3 labels,
 * 293 calls, `sub sp, #8` (an 8-byte outgoing-argument area, so at least one
 * callee takes five or more arguments), and THIRTY-SEVEN mentions of r8-r11
 * behind the full six-instruction Thumb high-save prologue
 * (`push {r5,r6,r7,lr}` / three `mov` / `push {r5,r6,r7}` / `mov r7,r8` /
 * `push {r7}`).  Reference file asm/overlays/rom_784360/ovl_30_c_a_c_c_c_a_c_b.s.
 *
 * CONTROL FLOW: ONE REAL TEST AND TWO POOL SKIPS.  `beq .L1b66` at ref line 357
 * is the only conditional; the `b .L1c3c` at 418 and `b .L20a4` at 848 each sit
 * immediately before a `.pool_aligned` and jump over it.  Treating a pool skip
 * as control flow was worth four wrong argument fills on this batch's 897, so
 * read those two as straight-line and let register state carry across them.
 *
 * LANDING IS A WHOLE-FILE CONVERSION.  The .s holds exactly ONE function;
 * datacheck.py is silent; split_s.py is not needed.  ONE linker row names the
 * object -- overlays/rom_784360/overlay.ld:49 -- and note that the BASENAME
 * `ovl_30_c_a_c_c_c_a_c_b.o` also appears under rom_786f0c, rom_7a8c8c and
 * rom_7c5efc, so match on the FULL PATH when editing the linker script.
 *
 * THE LEVER THIS ONE WANTS IS *NOT* PINS, AND THAT IS THE WHOLE POINT.
 * Five high-register live ranges, and every one of them holds a CONSTANT that
 * is built once and then copied down repeatedly:
 *     r8  built at ref 86   `mov r3,#0xe0 / lsl r3,#7`  (0x7000)   4 reads
 *     r11 built at ref 103  `mov r3,#0x80 / lsl r3,#5`  (0x1000)   6 reads
 *     r9  built at ref 232  `mov r3,#0xa0 / lsl r3,#7`  (0x5000)  10 reads
 *     r10 built at ref 508  `mov r3,#0xc0 / lsl r3,#8`  (0xc000)   2 reads
 *     r8  rebuilt at ref 515 `mov r3,#0xb0 / lsl r3,#8` (0xb000)   2 reads
 * Across the whole function: 90 build sequences against 28 `mov rlo, rhigh`
 * reuses -- 23.7% of wide-constant uses are REUSES, not rebuilds.  This is the
 * cse1 cross-call commoning that docs/band-800plus.md describes, present and
 * dominant, and it means a pin-per-site transcription is the WRONG starting
 * shape here: a pin's destination is a call-clobbered hard register, so it
 * rebuilds at every site and denies the allocator the five quantities the ROM
 * keeps.  (That mechanism is now measured both ways -- see this batch's
 * PARK_OvlFunc_889_2008074.c and the pin/no-pin experiment written up with it.)
 *
 * PRESCRIPTION, in the order to try it:
 *   1. FIVE NAMED LONG-LIVED CONSTANTS, one per range above, each assigned in
 *      its own statement at the reference's own build point (ref 86, 103, 232,
 *      508, 515) and read at its sites -- the band doc's "make your candidate
 *      hold the reference's NUMBER of long-lived quantities", which took its
 *      2009f3c from +20/+10 to +4/+1.  Expect `push {r5,r6,r7,lr}` plus the
 *      high-save sequence to appear only once the fifth name is in place.
 *   2. PINS ONLY AT THE SITES THAT ARE NOT COVERED BY (1).  Mixing is the
 *      point: 884 needs both, and the 70 build sequences that are NOT reuses
 *      are ordinary pin sites.
 *   3. Ascending `q0..q3` fills everywhere, then flip only
 *      `__Func_8092c40`-class sites whose call is followed by a
 *      `__Func_8091c7c` test.  Do NOT read the ROM's register-write order as
 *      the source order; on 889 that cost 52 encodings.
 *   4. Do NOT reach for a CSE_CFLAGS row, `-ffixed-r8..r11` or
 *      `-fno-rerun-cse-after-loop`.  Twelve flag settings were measured against
 *      this exact mechanism in batch 307 and none reaches it, and the `-ffixed`
 *      result is a RETRACTED theory -- it masks the commoning excess rather
 *      than preventing it.
 *
 * CALLEES, for the extern block: 39 OvlFunc_884_200a2e0, 33 __MapActor_SetAnim,
 * 30 __CutsceneWait, 22 each of __MapActor_Jump, __MapActor_Emote and
 * OvlFunc_884_200a2c8, 13 __MapActor_DoAnim, 11 each of __MapActor_GetActor and
 * __Func_8092adc, 10 __MapActor_SetSpeed, and 28 more callees in the tail.  The
 * family's prototype set is already written out in
 * src/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_c_b.c and covers most of them.
 */
