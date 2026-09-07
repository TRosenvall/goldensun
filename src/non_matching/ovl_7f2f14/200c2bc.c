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
 * WHY STATEMENT ORDER CANNOT REACH IT. THIS FILE PREVIOUSLY GAVE A WRONG
 * MECHANISM HERE -- it claimed gcse's `insert_insn_end_bb` appends always-last
 * unless the block ends in a jump or call. THAT IS NOT THE PATH TAKEN, and the
 * claim is struck. `pre_edge_insert` (gcse.c:4440) calls insert_insn_end_bb
 * only for EDGE_ABNORMAL; everything else goes through `insert_insn_on_edge`,
 * and `commit_one_edge_insertion` (flow.c:1656) then chooses among three
 * placements. Nor is the copy gcse's: gcse only rewrites the in-loop address in
 * place (the dump logs `COPY-PROP: Replacing reg 155 in insn 366 with reg 46`),
 * leaving a copy INSIDE loop 2.
 *
 * THE REAL FLOOR IS loop.c's. `move_movables` inserts every hoisted movable
 * with `emit_insn_before (pat, loop_start)` -- every branch of it (loop.c:1825,
 * 1890, 1982, 1991, 2024, 2028, 2047, 2055). `loop_start` is the
 * NOTE_INSN_LOOP_BEG, and `expand_start_loop` emits that note and the loop's
 * top label back to back, so NO C STATEMENT CAN LAND BETWEEN THEM. A hoisted
 * invariant is therefore always after every source-level preheader statement,
 * and `acc = 0` is one of those (`Biv 36 initialized at insn 355`).
 *
 * BOTH ESCAPES WERE MEASURED AND BOTH ARE SHUT. The init would have to be
 * created by a pass running after move_movables:
 *   - strength_reduce's giv initialiser -- refused, because the giv is cheap:
 *     the loop dump prints `not worth while, 0 vs 65` for the shift, so
 *     `acc = i << 20` stays in the body;
 *   - check_dbra_loop's re-emitted biv init -- re-emits only the loop's
 *     COMPARISON biv, and the ROM compares `i`, not `acc`.
 * A source-level copy (`q = &t` / `q = tp`) in the preheader is coalesced away
 * in every placement, freeing r9 for a pool constant -- that is the recorded
 * 114 class, reproduced.
 *
 * 30 further variants measured across for/while/goto forms, `acc` in the
 * for-init, wrapper loops (`do{}while(0)`, `while(1){...break;}`, `for(;;)` --
 * all deleted before loop.c, byte-identical to base), eight `t.` vs `tp->`
 * store combinations, store reordering, operand order, and
 * register/unsigned/declaration-position on `acc`. NOT ONE put the hoisted copy
 * anywhere but last. Notables: b1/t1/t2 swap r9<->r10 globally (10 differing);
 * b2/b3/t3/t4/p1-p5 hoist 0x17ffc into r9; g1 comes out TWO INSTRUCTIONS SHORT
 * (269); g2/g3 land at 108.
 *
 * SO THIS IS A FLOOR, NOT A SPELLING. Full sweep and the RTL dumps are in
 * scratch_elev/b251/c2bc/ (final/NOTES.md, dumps_base/, o.sh, gen.sh, batch.sh).
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
