/* OvlFunc_968_200c2bc -- 0x0200c2bc,
 * asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_c.s
 *
 * TWO differing encodings of 271, at the ROM's EXACT encoding count, with
 * EVERY REGISTER ROLE ALREADY THE ROM'S.
 *      first at index 163: ref 46ca  ours 4693
 * Best candidate: scratch_elev/b250/trio/final/e2bc_BEST.c.
 *
 * FLOOR HISTORY: 205 -> 110 -> 2. Start from e2bc_BEST.c. Starting over has
 * now cost two rounds and is the single most expensive mistake available here.
 *
 * ITS TWO FILE-MATES ARE ALREADY MATCHED and are waiting on this one:
 *      OvlFunc_968_200c048  628 bytes, 290 encodings, 20 relocations
 *      OvlFunc_968_200c520  208 bytes,  93 encodings,  9 relocations
 * They sit in scratch_elev/b250/trio/final/ as e048_MATCH.c and e520_MATCH.c,
 * and final/merged.c holds all three in ROM order with both still
 * instruction-identical inside the merged TU. c2bc is in the MIDDLE of the .s,
 * so landing the other two alone would need an awkward three-way split; the
 * file lands WHOLE the moment these two encodings close.
 *
 * THE RESIDUE, in loop 2's preheader:
 *      ROM    mov r2,#0 / mov r8,r2 / mov r10,r9 / mov r11,r2
 *      ours   mov r2,#0 / mov r8,r2 / mov r11,r2 / mov r10,r9
 * One adjacent pair, transposed.
 *
 * BLOCKER CLASS: GCSE INSERTION POINT, not allocation. The count is exact and
 * the register assignment is the ROM's throughout.
 *
 * THE PREVIOUSLY RECORDED HYPOTHESIS IS REFUTED. `base`/`z` are NOT a GIV and a
 * hoisted invariant. What actually held the function was `q = tp`: gcc
 * COALESCES two names for one address -- the recorded "if the two pointers
 * genuinely hold the same address, this lever has nothing to work with" -- which
 * freed r9 and let gcse hoist 0x17ffc there. Writing plain `t.f8 = ...` lets
 * GCSE MANUFACTURE THE SECOND REGISTER ITSELF, and that alone was 114 -> 12.
 *
 * ALSO LOAD-BEARING in the current candidate: `tp = &t` assigned THIRD (birth
 * order); `m = i + 0x1a` named before `n`; and a FRESH n2/s4/s5 for the second
 * __CopyMapTiles site.
 *
 * WHY STATEMENT ORDER CANNOT REACH IT -- read out of
 * /opt/camelot-gcc/gcc-2.96/gcc/gcse.c in the build image. `insert_insn_end_bb`
 * appends with `emit_insn_after (pat, BLOCK_END)` -- ALWAYS LAST -- unless the
 * block ends in a jump or a call, which is the other arm of that `if`. A
 * gcse-hoisted `&t` copy therefore cannot be moved before a preceding source
 * assignment by any spelling. 14 spellings measured, all 2 or worse; earlier, 48
 * permutations of two blocks all landed 205-206. ORDERING IS EXHAUSTED. Do not
 * run another sweep.
 *
 * THE REMAINING FREEDOM, as hypotheses to TEST rather than conclusions:
 *   1. have the `mov r11, r2` assignment created by a pass LATER than gcse, so
 *      it lands after the hoisted copy;
 *   2. stop the r10 copy being gcse-hoisted at all -- an ordinary source
 *      assignment created before gcse runs is never touched by
 *      insert_insn_end_bb, and normal ordering applies again. This fights the
 *      114 -> 12 lever above, so it needs the second register manufactured some
 *      other way;
 *   3. change how the preheader block ENDS. Always-last is conditional on the
 *      block not ending in a jump or call, so a source shape that puts a call
 *      at the end of that block moves the insertion point.
 *
 * MEASURED WORSE: the split-condition `do` + `break` form is 270 lines, 224
 * differing, frame grown to 0x3c by a spill.
 *
 * LANDING SHAPE WHEN CLOSED: src/overlays/rom_7f2f14/ovl_30_c_c_a_c_c.c holding
 * all three in ROM order. No split, no linker edit -- overlay.ld:91 already
 * names asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_c.o(.text) and MUST KEEP its
 * asm/ path. The .s carries no data, and the only pooled symbols are gState,
 * iwram_3001ebc and iwram_3001e40, all already extern in the tree.
 * makefile_flags() is empty, so plain -O2.
 */
