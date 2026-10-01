/* Func_80ad6d4 -- RECON, NO CANDIDATE BODY, THEREFORE NO FIGURE.
 *
 * NO `NON-MATCHING, N of M` LINE AND NO `Verify with:` RECIPE: there is no .c
 * body to measure.  1,274 instructions, UNATTEMPTED, and this round did not
 * write it.  parkcheck will report UNCHECKABLE and that is correct.  This is a
 * state report, NOT a blocker claim -- the function is actionable.
 *
 * Reference: asm/rom_a1000/rom_ad274_c_c_a.s, lines 42..1437, 1,274
 * instructions (1,713 lines counting labels and the literal pool).
 *
 * ============ THE VENEER: ONE INLINE SITE, AND IT IS NOT THE PROBLEM ============
 * Screened on the anchored pattern `^[[:space:]]*\.call_via` over the function's
 * own block: EXACTLY ONE inline site --
 *
 *     add r0, r3 / lsl r0, #2 / bl sin / ldr r3, =Func_8000888 /
 *     mov r1, r0 / mov r0, #0x10 / .call_via r3
 *
 * -- which is the `r = call_via_r3(__sin(*p << 3), K);` shape recorded for
 * OvlFunc_881_20081c4 in docs/elevation.md, with the arguments the other way
 * round (the sine result is argument 1, the literal 0x10 is argument 0).
 * Single site, so BIND THE SYMBOL INSIDE THE HELPER, register r3.
 *
 * AND THE FUNCTION ALREADY PROVES THE TWO FORMS COEXIST.  Beside that one
 * inline site it carries FOURTEEN ordinary compiler veneers -- `bl _call_via_r3`
 * x10, `bl _call_via_r5` x3, `bl _call_via_r4` x1 -- i.e. ordinary C function
 * pointers that gcc-2.96 emits for free.  docs/elevation.md's "mixing the inline
 * veneer with ordinary bl _call_via_rN in one function blocks nothing" has a
 * concrete instance here, and it is also the trap the document warns about: DO
 * NOT GREP FOR `call_via`.  An unanchored grep over this function answers 15 and
 * 14 of those are the OTHER form.  Anchor on `^[[:space:]]*\.call_via`; the
 * relocations are the other discriminator (the ordinary form carries an
 * R_ARM_THM_CALL to `_call_via_rN`, the inline macro carries none).
 *
 * The veneer register number in each `bl _call_via_rN` is a FREE DIAGNOSTIC --
 * it is a readout of where the allocator put the pointer.  With fourteen of them
 * this function will tell you more about its own allocation than most.
 *
 * ============ SPLIT SHAPE, DRY-RUN VERIFIED THIS BATCH ============
 *   python3 tools/split_s.py --dry-run asm/rom_a1000/rom_ad274_c_c_a.s Func_80ad6d4
 * The target is the MIDDLE of three functions, so this is a TWO-CUT split:
 *     _a.s   1 function,   31 lines   (Func_80ad69c, 0x080ad69c)
 *     _b.s   1 function, 1399 lines   (Func_80ad6d4)            <- the target
 *     _c.s   1 function,  472 lines   (Func_80ae2f4, 0x080ae2f4)
 * and stage1.ld is rewritten.  tools/datacheck.py is silent on the file: no
 * code/data cut.  The target becomes src/rom_a1000/rom_ad274_c_c_a_b.c.
 * NOTE: Func_80ae2f4 ALREADY HAS A PARK at src/non_matching/rom_a1000/80ae2f4.c,
 * so the cut order matters -- batch 307 recorded that a shared-file split
 * constraint is NOT symmetric.  DRY-RUN BOTH ORDERS before committing to one,
 * and gate `make compare` on the split BEFORE writing any .c.
 * ALWAYS --dry-run: split_s.py deletes a tracked .s.
 *
 * ============ THE SPILL MAP *IS* THE DECLARATION LIST ============
 * Frame `sub sp, #0x88` = 136 bytes.  Every sp reference in the block, sorted
 * DESCENDING:
 *     sp+0x80, sp+0x78                      two word scalars at the top
 *     sp+0x64, sp+0x60, sp+0x5c, sp+0x58    four word scalars
 *     sp+0x54 down to sp+0x10 in 4-byte steps, UNBROKEN (19 words)
 *                                           -- that run is an AGGREGATE, not
 *                                              nineteen locals.  An int[19] or a
 *                                              76-byte struct; do not read it as
 *                                              a declaration list.
 *     sp+0xc, sp+8, sp+4, sp+0              four more words
 * Reading the rule: the SCALARS sorted descending give declaration order; the
 * AGGREGATE order is reversed; and the 0x10..0x54 run says a quantity of 76
 * bytes existed, not that the source held it in one place.  Against the ROM's
 * ACCESS COUNT, access count has won by 12 encodings before -- count the
 * distinct offsets actually touched in the run before committing to a type.
 * sp+0 and sp+4 are the outgoing-argument words for any call taking five or more
 * arguments AND are where `-fcall-used-r4` spills r4; check before claiming
 * either is a local.
 * Prologue: r0 is copied to r11 immediately (`mov r11, r0`) and
 * `iwram_3001f2c`'s contents go to r9, so argument 0 is long-lived and the
 * state block is named.
 *
 * ============ SHAPE METRICS, so the first draft is not a surprise ============
 *   1,274 instructions, 76 conditional branches, ZERO jump tables
 *   (`grep -c '\.word .L'` = 0) -- so every multi-way decision in this function
 *   is a COMPARE CHAIN, not a switch past CASE_VALUES_THRESHOLD.  That matters:
 *   the switch lever (a `case 0` sharing the default arm, a `bcc` entry test
 *   meaning a fourth lowest case) does not apply here.
 *   66 pooled constants (`ldr rN, =0x...`).  Pool words are a size-and-count
 *   defect, so expect to spend real effort on which constants pool and which
 *   gcc synthesises -- and note the honest position on carriers: an `int` temp
 *   can push a constant EITHER way and the lever has no reliable precondition.
 *   Callees, heaviest first: _Func_801e7c0 x13, WaitFrames x12, _call_via_r3 x10,
 *   _Func_8019000 x10, _Func_8016498 x9, free x7, _PlaySound x7, Func_80a1114 x7,
 *   Func_80a10d0 x7, Func_8004970 x7, _SetTextColor x6, Func_80acab8 x6,
 *   Func_80aa538 x6, __modsi3 x5, __divsi3 x4, _GetUnit x4, _Func_8016478 x4,
 *   Func_80ad5f4 x4, Func_80ad5b4 x4, Func_80aae14 x4.
 *   WaitFrames x12 plus _PlaySound x7 plus _SetTextColor x6 reads as a scripted
 *   cutscene/menu routine, which is the population where branch density, not
 *   instruction count, is the axis (docs/band-800plus.md).
 *   __divsi3 x4 and __modsi3 x5: those are REAL `/` and `%` on signed ints, and
 *   a `/` reached through `bl __divsi3` side-steps the `__divsi3` alias blocker.
 *
 * NEXT: this is a large but ordinary reconstruction.  Do the split first and
 * gate it, then build the function in regions against aligncmp (objcmp's count
 * saturates while the length is still wrong).  The one inline veneer is four
 * bytes of solved shape and should be written in on the first draft, not
 * deferred -- it is not a blocker and never was.
 */
