/* Task_ScreenWindowTransition -- RECON ONLY, NO FIGURES INVENTED.
 *
 * NO objcmp and NO aligncmp number exists and none is claimed; nothing was
 * compiled against it. There is deliberately no `NON-MATCHING, N of M` line,
 * because inventing one would make this park re-measurable against a figure
 * that was never taken.
 *
 * Verify with, once a candidate exists at the installed path:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/Task_ScreenWindowTransition.c \
 *     asm/rom_8a000/rom_8d9a4_c_c_a_a_c.s --func Task_ScreenWindowTransition
 *
 * SHIMS: not applicable, no candidate. The fact that replaces it, and it is
 * the DOMINANT fact about this function: THE REFERENCE CARRIES SIX
 * _call_via_rX VENEERS AND THREE DIFFERENT ONES -- 3x _call_via_r9,
 * 2x _call_via_r3, 1x _call_via_r7. A candidate must produce exactly those
 * six, through those three registers. Count the veneers on BOTH sides before
 * reading a single hunk; a veneer-count or veneer-REGISTER mismatch is a
 * different program, not a residue.
 *
 * ================================================================
 * MEASURED STRUCTURE -- AND WHY THE TINY FRAME IS A WARNING, NOT A GIFT
 * ================================================================
 *
 *     asm/rom_8a000/rom_8d9a4_c_c_a_a_c.s, lines 74..1159
 *     885 instructions, 83 branches, 87 labels, 129 high-register mentions
 *     frame `sub sp, #0x18`  (24 bytes)
 *     6 veneers through r9, r3 and r7
 *
 * The frame is the SMALLEST in the batch by a wide margin -- 24 bytes against
 * 0x2c, 0x74 and 0xac for the others -- and only four slot addresses appear in
 * the whole body: sp+0x0c, sp+0x10, sp+0x14 and the `add sp, #0x18` epilogue.
 * So AT MOST THREE 4-BYTE SLOTS plus the outgoing-argument area, across 885
 * instructions.
 *
 * DO NOT READ THAT AS "a short declaration list, therefore easy". It means the
 * OPPOSITE: 885 instructions living in 24 bytes of frame means essentially
 * every quantity is in a register, 129 high-register mentions deep, and the
 * function is therefore decided ENTIRELY BY ALLOCATION. The free-first-move's
 * spill-slot map -- the strongest tool in this brief, and what carried
 * Anim_Ramses to -4/-1 on a first compile -- has almost nothing to work with
 * here: three slots cannot pin a declaration list. Expect the batch-304-style
 * register-partition questions (lever 3: a register repeating across disjoint
 * loops suggests a partition but is NOT proof, the direction is NOT fixed, and
 * splits are NOT additive) to be the whole job, and budget for trying BOTH
 * directions on each, since one batch paid +6.3 for SPLITTING a counter and
 * +8.6 for UNIFYING nine from identical evidence.
 *
 * 87 LABELS IS THE HIGHEST LABEL COUNT IN THE BATCH, against 83 branches --
 * more labels than branches, which means fallthrough joins, which means
 * cross-jumping has merged tails. Lever 8 is therefore live: a literal equal
 * to a variable's known value is NOT interchangeable with it, because
 * cross-jumping compares EMITTED TAILS.
 *
 * WHAT IT IS. The reference's own header names it OverlayUpdateTask: a
 * per-frame task that advances the overlay animation and recomputes its window
 * geometry. The call profile is small and all of it is geometry or dispatch:
 *
 *     3x _call_via_r9      3x GetFieldActor     2x _call_via_r3
 *     2x __divsi3          2x StopTask          2x Random
 *     1x _call_via_r7      1x _Func_801edec
 *
 * TWO `__divsi3` CALLS ARE A SOURCE SIGNAL, not an artifact: gcc only emits
 * the division helper for a division by a NON-CONSTANT, so two of the
 * divisions in this function have a variable divisor and must be written as
 * such. Division by a constant compiles to shifts and would show as none.
 *
 * `divsi3_RAM` ALSO APPEARS AS A POOL SYMBOL alongside `__divsi3`, which is
 * this ROM's IWRAM-resident copy of the helper. Check how the landed files in
 * this bank reach it before assuming a plain `/`.
 *
 * The pool symbol set is the whole external surface and is short enough to
 * list, which makes the band doc's section-5 CONSTANT-SET CHECK the cheapest
 * first move after any compile:
 *
 *     Data_9f840  Func_8000948  Func_808f498  REG_DMA0SAD  divsi3_RAM
 *     gState  iwram_3001e40  iwram_3001e70  Task_ScreenWindowTransition
 *
 * Note `Task_ScreenWindowTransition` is in its OWN pool -- the function takes
 * its own address, which is how `StopTask(Task_ScreenWindowTransition)` is
 * reached, and a candidate must do the same rather than passing a parameter.
 * `Data_9f840` lives in another object and `REG_DMA0SAD` means this function
 * programs a DMA channel directly, so the register writes must go through the
 * io.h macros to get the volatile accesses right.
 *
 * The prologue is explicit and gives the first two quantities for free:
 *
 *     ldr r3, =iwram_3001e70
 *     ldr r6, [r3]          -> the state pointer, kept in r6, never spilled
 *     ldr r3, [r3, #0x5c]   -> a second pointer 0x5c further on
 *     sub sp, #0x18
 *     ldr r0, =0x53c
 *     str r3, [sp, #0x14]   -> that second pointer is the TOP spilled slot
 *     add r4, r3, r0        -> and it is immediately used with +0x53c
 *
 * So sp+0x14 is the first-declared of the at most three spilled scalars, and
 * the two globals are read off ONE pool word at `iwram_3001e70` with offsets 0
 * and +0x5c. Anim_Ramses measured the matching lesson for exactly this shape
 * and it transfers: reading the second slot DIRECTLY off the symbol
 * (`iwram_3001e70[23]`) is fine for a POSITIVE offset, but if any sibling read
 * in this function turns out to be NEGATIVE it must go through a declared
 * pointer local or gcc builds the offset in a register and costs an
 * instruction. There is no negative offset here, so a direct read is right.
 *
 * ================================================================
 * SPLIT SHAPE -- CLEAN THREE-WAY, NO EXPORTS, NO DATA
 * ================================================================
 *
 * `python3 tools/datacheck.py asm/rom_8a000/rom_8d9a4_c_c_a_a_c.s` is SILENT:
 * the file carries no data section of its own and no function in it reads a
 * data label, so nothing is lost by the split and NO `.global` is needed.
 * The asm-label capture hazard does not arise, there being no labels to
 * export.
 *
 * `python3 tools/split_s.py asm/rom_8a000/rom_8d9a4_c_c_a_a_c.s
 * Task_ScreenWindowTransition --dry-run` does NOT refuse:
 *
 *     would write asm/rom_8a000/rom_8d9a4_c_c_a_a_c_a.s  (1 fn,   66 lines)
 *     would write asm/rom_8a000/rom_8d9a4_c_c_a_a_c_b.s  (1 fn, 1091 lines)
 *     would write asm/rom_8a000/rom_8d9a4_c_c_a_a_c_c.s  (1 fn,   49 lines)
 *     would REMOVE asm/rom_8a000/rom_8d9a4_c_c_a_a_c.s
 *     would rewrite stage1.ld
 *
 * The target lands as src/rom_8a000/rom_8d9a4_c_c_a_a_c_b.c, and the _a piece
 * holds `Func_808f498`, whose tail is visible just above the target and is
 * worth noting because it is a THREE-WORD `stmia r3!, {r0, r1, r2}` store of
 * a `0xa6600001` constant -- if that function is ever converted it will want
 * its own look. ALWAYS run split_s.py with --dry-run first: it DELETES a
 * tracked .s and REWRITES a linker script. Verify `make compare` is GREEN
 * after the split and BEFORE writing any .c.
 *
 * ================================================================
 * THE CENSUS NOTE, SO IT IS NOT LOST
 * ================================================================
 *
 * This function was recovered by the census bug fix: a park filename was
 * matching function names by BARE SUFFIX, so any park written for a `*Main`
 * function hid it. There is therefore NO prior work and NO inherited figure,
 * and the absence of either is not evidence that the function is hard. Park
 * exclusion is by function NAME, and this is the second class of collision
 * found in that mechanism after the overlay low-address collision
 * (`OvlFunc_971_200808c` hidden by a park for `OvlFunc_881_200808c`).
 *
 * ================================================================
 * WHERE TO START
 * ================================================================
 *
 * 1. Count the veneers and settle the three indirect-call sites FIRST. Six
 *    veneers through three different registers in 885 instructions is the
 *    shape of a table of function pointers being dispatched, and getting the
 *    dispatch wrong makes every later measurement meaningless.
 * 2. Nearest landed siblings by address are the other pieces of this stem in
 *    src/rom_8a000/; read them for the `iwram_3001e70` struct and the
 *    `GetFieldActor` return type before writing a line. A landed file in the
 *    same bank proves what gcc-2.96 does with that bank's idioms and is
 *    stronger evidence than any park.
 * 3. 83 branches over 885 instructions is ~11 instructions per block, the
 *    ORDINARY density -- so docs/band-800plus.md section 2 does NOT apply and
 *    the standard lever set does. This park makes no prediction about WHICH
 *    lever, because none was measured.
 */
