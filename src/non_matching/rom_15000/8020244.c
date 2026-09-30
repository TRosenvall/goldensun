/* Func_8020244 -- RECONNAISSANCE ONLY.  NO CANDIDATE AND NO FIGURE.
 * NO `Verify with:` RECIPE BY DESIGN -- there is no candidate body in this file, so
 * there is nothing for objcmp to measure and nothing for parkcheck to re-check.
 * parkcheck reporting this file UNCHECKABLE is CORRECT, not documentation debt.
 * This file is NOT a park and must not be fed to parkcheck: it carries no
 * "NON-MATCHING, N of M" line because no candidate was written, and inventing a
 * figure for one is the one thing worse than running out of budget.
 *
 * Reference asm/rom_15000/rom_20198_a_a_a.s, 614 instructions, the largest target
 * in batch 302 brief F.  Budget went to Func_809bcf8 (which reached an exact
 * instruction count) and Func_80b0aac; this one got structure and split shape only.
 *
 * NOT blocked: `grep -c call_via` over its block is 0, unlike its brief-mate
 * Func_808bec0.  It is a legitimate decompilation target.
 *
 * ============ SPLIT SHAPE (computed, not guessed) ============
 *
 * `python3 tools/datacheck.py asm/rom_15000/rom_20198_a_a_a.s` prints NOTHING: no
 * text/data split is needed.  The .s holds TWO functions, Func_8020198 at 0x08020198
 * and Func_8020244 at 0x08020244, and the target is the LAST, so split_s.py is a
 * clean TAIL split: ..._a_a_a.s keeps Func_8020198 and the target becomes
 * src/rom_15000/rom_20198_a_a_a_b.c.
 * stage1.ld:528 names asm/rom_15000/rom_20198_a_a_a.o(.text), one line.
 * NO NEW `.global` IS REQUIRED: checked mechanically, the function's block
 * references no `.L` label it does not itself define.
 * COLLISION WARNING, same shape as the one between Func_809bcf8 and Func_809c138:
 * src/non_matching/rom_15000/8020198.c is a park for the OTHER function in this .s,
 * and Func_8020198 being FIRST means its own split would also want the _b suffix.
 * Whichever of the two lands first defines the split; the other park's recipe then
 * needs rewriting rather than following.
 *
 * ============ WHAT IT IS ============
 *
 * A save-slot select screen.  Two arguments: r0 = a starting slot index (kept in r8
 * and wrapped modulo 3), r1 = a MODE (kept in r9) that the function dispatches on
 * with plain compares against 1, 4, 5 and 0 -- not a switch, four sequential
 * `cmp r2, #k / bne` tests.  It opens with
 *     galloc_iwram(0x37, 0xa70)            @ mov r1,#0xa7 / lsl #4
 *     ctx  = *(T **)iwram_3001f1c
 *     aux  = *(T **)(iwram_3001f1c - 0x90)
 *     _Func_8077cb8()                      @ result kept at sp+4
 * and the slot records live at ctx + 0x105c, stride 0x40 (`lsl r3, #6`), three of
 * them.  Later code uses +0x1068, +0x1070 and +0x1074 within a record, reads a
 * halfword at +0x1a and a word at +4 / +0x1c, a halfword at +2 for a text id
 * (`+ 0x99b`), and a `signed char` at +0x15.  A 0x8500029c DMA3 control word
 * clears a 4-byte local at sp+0x24 (`ldr r0, =0` style zero fill via REG_DMA3SAD).
 *
 * ============ THE ONE THING THAT WILL DECIDE THIS FUNCTION ============
 *
 * ITS FIRST 110 INSTRUCTIONS ARE FOUR NEAR-IDENTICAL COPIES OF ONE SCAN LOOP, one
 * per mode, at .L2028a (mode 1), .L202d2 (mode 4), .L2031a (mode 5) and .L20362
 * (mode 0), all falling into a SHARED exit test at .L2039a (`cmp r6, #3 / bne`).
 * Each copy is the same "advance to the next non-empty slot, at most three tries"
 * walk:
 *     r6 = 0
 *     v = ctx[0x105c + slot*0x40]
 *     while (v == 0 || <per-mode byte test>) { slot = (slot+1)%3; if (++r6 > 2) break; ... }
 * and they differ ONLY in which byte of the record is tested and in the polarity:
 *   mode 1  record[0x1070+1] SIGNED, `bne` -> loop while it is ZERO
 *   mode 4  record[0x1070+2] SIGNED, `beq` -> loop while it is NON-zero
 *   mode 5  record[0x1070+1] SIGNED, `beq`
 *   mode 0  no byte test at all, and it is the ODD ONE OUT -- the only copy that
 *           STRENGTH-REDUCES the record address into a walking pointer
 *           (`add r2, #0x40`, reset to `r7 + 0x105c` on wrap) instead of
 *           recomputing `slot*0x40` each iteration.
 * The mode-0 copy being the only strength-reduced one is the lever to find first:
 * four textual copies of one loop in the source would compile the same way four
 * times, so mode 0 IS WRITTEN DIFFERENTLY IN THE SOURCE -- most likely as the plain
 * `while (v == 0)` with no second condition, which is what lets loop.c's
 * strength reduction fire where the compound condition blocks it.  Get that asymmetry
 * from the source shape and roughly a fifth of the function follows; write the four
 * loops as one helper or one loop with a mode test inside and it cannot be reached.
 *
 * SECOND THING: `cmp r6, #3` at the shared exit, against a counter the four loops
 * increment separately, is the "counters unify across disjoint loops" shape already
 * recorded in docs/elevation.md from batch 301 (five loops there).  Expect ONE
 * counter variable, not four.
 *
 * THIRD: the second half is a display loop counting DOWN (`mov r6, #2 ... sub r6, #1
 * ... cmp r6, #0 / bge`) over the same three records with a parallel pointer pair --
 * `r5` walking the record at +0x105c and `sp+0x14` walking a second array at
 * +0x1040 (`mov r1,#0x82 / lsl #5`), both `+= 0x40` -- and an `r11` y-coordinate
 * stepping by 0x10.  Three induction variables, one loop; per the recorded rule that
 * is three source variables, not one plus arithmetic.
 */
