/* Func_801f730  @  0x0801f730  [rom_15000]   *** PARK -- 3 of 33 ***
 * NON-MATCHING, 3 of 33 encodings (measured batch 322).
 *
 * NOT A LANDING. Parked at 3, PIN-FREE, improved from the installed park's 7.
 *
 * FIGURE (measured, batch 322 brief B, on this body):
 *   objcmp --func : 3 of 33 encodings differ, COUNT MATCHED (ref 33, ours 33),
 *                   SIZE MATCHED (76 v 76), relocations identical.
 *   objcmp --whole: same 3 of 33, first at index 12.
 *   per-opcode MEM screen: clean (ref ldr x3 + ldrb x1; ours identical).
 *   PIN COUNT: 0.  No inline asm, no device, no fictitious symbol.
 *
 * THE INSTALLED PARK MEASURES 7 AND IS A WRONG PROGRAM BY THE MEM SCREEN.
 *   src/non_matching/rom_15000/801f730.c reads 7 of 33 with
 *   MEM {ldr:3, ldrsb:1} against the ROM's {ldr:3, ldrb:1} -- it reads the byte
 *   SIGN-EXTENDING where the ROM reads it unsigned. A figure of 7 on the wrong
 *   load opcode is not "four away" from 3.
 *   The installed park's own header already says "Residue is now 3, not 8" and
 *   points at scratch_elev/b235/f801f77c/f801f730_sibling.c -- WHICH WAS NEVER
 *   INSTALLED. This body is that one, reproduced so the figure above is
 *   reproducible from this file alone. Batch 235 found the fix; batch 322 is
 *   installing it.
 *
 * Verify with (INSTALLED PATH):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801f730.c \
 *     asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_a.s --func Func_801f730
 *
 * SPLIT SHAPE: NONE NEEDED.
 *   grep -c thumb_func_start asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_a.s -> 1
 *   python3 tools/datacheck.py <that .s>            -> clean (no output)
 *   python3 tools/split_s.py   <that .s> --dry-run  -> nothing to split
 *   INSTALL PATH if it ever lands: src/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_a.c
 *
 * ---------------------------------------------------------------------------
 * WHAT THE RESIDUE IS: THREE REAL INSTRUCTIONS. NO POOL WORDS.
 *
 * Per-index, indices 30/31/32 are the alignment halfword and the two pool
 * words and they are IDENTICAL, in the ROM's order. So all three differing
 * encodings are real instructions, and the usual "an objcmp encoding can be a
 * pool word" trap does not apply here:
 *
 *      11  4b0a  ldr  r3,[pc,#40]   | 4b0a  ldr  r3,[pc,#40]
 *   XX 12  490a  ldr  r1,[pc,#40]   | 480a  ldr  r0,[pc,#40]
 *      13  681b  ldr  r3,[r3,#0]    | 681b  ldr  r3,[r3,#0]
 *   XX 14  185a  adds r2,r3,r1      | 2102  movs r1,#2
 *   XX 15  2102  movs r1,#2         | 181a  adds r2,r3,r0
 *
 * 30 of 33 exact. One register, and the two insns around it.
 *
 * THE DECIDING RUNG IS RELOAD'S SPILL CHOICE -- not sched2, not the allocator.
 *
 *   .18.greg says:   ;; 4 regs to allocate: 45 36 33 32
 *                    ;; Register dispositions: 32 in 6  33 in 5  34 in 0
 *                                              36 in 1  ...  45 in 2
 *                    Spilling for insn 134.
 *                    Using reg 0 for reload 0
 *
 *   pseudo 36 = the counter `i` -> r1;  45 = the giv cursor -> r2;
 *   33 = `r` -> r5;  32 = `a` -> r6.
 *
 *   0x1071 IS NOT A PSEUDO AT ALL. .18.greg's BB2 has the loop-created insn
 *   deleted and a RELOAD insn in its place:
 *       (note  132 ... NOTE_INSN_DELETED 0)
 *       (insn  143 (set (reg:SI 0 r0) (const_int 4209)))
 *       (insn  134 (set (reg:SI 2 r2) (plus (reg 3) (reg 0))))
 *   so index 12 is RELOAD picking a spill register, and
 *   REG_ALLOC_ORDER (config/arm/arm.h:989) is  3, 2, 1, 0, 12, 14, 4, 5, ...
 *   -- r3 (base), r2 (cursor) and r1 (counter) are all live at insn 134, so r0
 *   is simply the first free one.
 *
 *   Indices 14/15 are sched2 choosing the counter init over the add. EVEN IF A
 *   SCHEDULING LEVER FLIPPED THEM, index 14 would still differ
 *   (`adds r2,r3,r0` v `adds r2,r3,r1`), so the best a sched2 lever alone can
 *   reach here is 2, not 0. The register is the irreducible part.
 *
 *   NOTE FOR THE PRIORITY-ARITHMETIC LEVER: `;; 4 regs to allocate` is NONZERO,
 *   so global-alloc is live on this function -- but the decision is not global
 *   alloc's, so deriving allocno_compare here is wasted work. Reported because
 *   the brief asked for the rung with numbers.
 *
 * ---------------------------------------------------------------------------
 * THE BOUND, WITH ITS MECHANISM -- READ FROM THE COMPILER, NOT FROM DUMPS
 *   (~/gs_project/camelot-gcc/gcc-2.96/gcc/)
 *
 * For reload to pick r1 the counter must not be live at the giv init. It always
 * is, and the reason is two file:line facts plus the call order between them:
 *
 *  1. THE GIV INIT IS EMITTED AT THE END OF THE PREHEADER.  loop.c:4778, in
 *     strength_reduce, under the comment "Add code at loop start to initialize
 *     giv's reduced reg":
 *         emit_iv_add_mult (bl->initial_value, v->mult_val, v->add_val,
 *                           v->new_reg, loop_start);
 *     and emit_iv_add_mult (loop.c:7610) ends in
 *         emit_insn_before (seq, insert_before);
 *     with insert_before == loop_start, i.e. immediately before
 *     NOTE_INSN_LOOP_BEG. Our dump shows exactly that: insn 134 sits between
 *     insn 39 (`i = 2`) and (note 40 ... NOTE_INSN_LOOP_BEG).
 *
 *  2. THERE ARE ONLY TWO PLACES THE COUNTER INIT CAN COME FROM, AND BOTH LAND
 *     BEFORE IT.
 *       (a) ordinary preheader code -- `i = 2` written in the source. Trivially
 *           earlier, and no declaration or statement order can move it past a
 *           pass-generated insn.
 *       (b) check_dbra_loop's reversed-biv init,
 *               emit_insn_before (gen_move_insn (reg, start_value), loop_start)
 *           at loop.c:~8154.  check_dbra_loop is CALLED FROM strength_reduce at
 *           loop.c:4408 -- 370 lines BEFORE the giv-init emission at 4778.
 *           Both insert immediately before loop_start, so the LATER insertion
 *           ends up CLOSER to loop_start: the giv init always lands AFTER the
 *           dbra counter init.
 *
 * So no source shape can order them the ROM's way, the counter's hard register
 * is always live at the giv init, and reload can never choose it.
 *
 * STATED WITH EVIDENCE ATTACHED SO IT CAN BE REFUTED, not as a closed class.
 * WHAT WOULD RETIRE IT: any gcc-2.96 Thumb function in this tree where a
 * reload-materialised giv addend takes the same hard register a loop counter
 * later uses. I did not find one; I also did not scan for one.
 *
 * ---------------------------------------------------------------------------
 * A NEW POSITIVE RESULT, WORTH MORE THAN THE REGISTER
 *
 * *** THE ROM'S COUNTDOWN LOOP IS REACHABLE FROM AN UP-COUNTING SOURCE. ***
 * `for (i = 0; i < 3; i++)` compiles to the ROM's
 *     movs r1,#2 / ... / subs r1,#1 / cmp r1,#0 / bge
 * BIT-IDENTICALLY to the explicit `for (i = 2; i >= 0; i--)` -- 3 of 33 at the
 * same three indices, same 76 bytes. That is check_dbra_loop reversing the
 * loop (loop.c:4408). Two spellings that look different in C are the same
 * program here.
 *
 * CONSEQUENCE FOR EVERY FUTURE BRIEF: do not sweep LOOP FORM on a function
 * whose counter biv is used only in the exit test. for/while/do-while and
 * up/down are one equivalence class under check_dbra_loop, and sweeping them
 * measures nothing. Fifteen of my rows below are that mistake, made cheaply.
 *
 * ---------------------------------------------------------------------------
 * CROSSED AND SWEPT -- 15 ROWS EXACTLY INERT, BIT-IDENTICAL OUTPUT
 *
 * All at 3 of 33, nenc 33, size 76, reloc ok, MEM clean, idx=[12,14,15]:
 *   base (this body)                      h1 h3 h4  declaration order x3
 *   h2  statement order k= before p=      h5  while instead of for
 *   h6  i=2 hoisted above p=/k=           h7  do { } while (i >= 0)
 *   i1  q = p + 0x1071, index from 0      i2  p = iwram_3001f1c + 0x1071
 *   i3  j = 0 before q =                  i4  k = k + 0x40 not k += 0x40
 *   a1  UP-COUNTING for (i=0;i<3;i++)     a2  up-counting while        [new]
 *   a4  k += 0x40 in the for-increment    a5  *(p+k) not p[k]          [new]
 *   a6  unsigned index                                                 [new]
 *
 * MEASURED WORSE:
 *   a3  `i != 3` exit test      6   (+3 at idx 22/23/24, the loop-end compare)
 *   installed park              7   walking pointer -> ldrsb, MEM FLAG
 *
 * Flag groups, from the batch-235 header, not re-run here:
 *   -fno-schedule-insns2 6; -fno-strength-reduce 20/13; -fno-gcse,
 *   -fno-rerun-cse-after-loop, -fno-rerun-loop-opt, -fno-strict-aliasing all 3.
 *
 * WHY THE FLAT SWEEP IS THE FINDING, AND HOW IT WAS SCREENED.
 * Per the batch-321 rule that allocation edits are screened by .17.lreg and not
 * by the figure, I checked the one variant that looks like it should move the
 * add out of the preheader. `i2`'s .18.greg BB2:
 *       (note 35 ... NOTE_INSN_DELETED 0)   <- `p = iwram_3001f1c + 0x1071` GONE
 *       (note 37 ... NOTE_INSN_DELETED 0)   <- `j = 0`                      GONE
 *       (insn 43  (set (reg/v:SI 1 r1) (const_int 2)))
 *       (insn 146 (set (reg:SI 0 r0) (const_int 4209)))     <- reload, r0 again
 *       (insn 137 (set (reg:SI 2 r2) (plus (reg 3) (reg 0))))
 *       Spilling for insn 137. / Using reg 0 for reload 0
 * strength_reduce FOLDS the source's explicit +0x1071 away and RE-CREATES it at
 * the preheader end. `a1` is the same: Spilling for insn 124 / Using reg 0.
 * So the fifteen flat rows are fifteen ERASED EDITS, not fifteen inert levers.
 *
 * ---------------------------------------------------------------------------
 * WHAT IS RIGHT AND MUST BE KEPT (30 of 33, both pool words, pool ORDER)
 *   - the WALKING INT INDEX `p[k]` with `k += 0x40`, which is what produces the
 *     ROM's `ldrb` + `lsl #24` + interleaved `add r2,#0x40` + `cmp #0`. A
 *     walking POINTER gives `mov #0` + `ldrsb` and is the installed park's bug.
 *     This refutes docs/elevation.md's two by-name negatives on this function
 *     ("`ldrb` + `lsl #24` before a `cmp #0` is not reachable" and its
 *     generalisation); batch 235 found that and it reproduces here.
 *   - `signed char *` for iwram_3001f1c -- unsigned gives `ldrb`+`cmp` with no
 *     `lsl`, one instruction short.
 *   - `r = -9` assigned AFTER the Func_80056cc call, which gives
 *     `bl / mov r5,#9 / neg r5,r5 / cmp r0,#0` in that order.
 *   - the pooled 0x1071 added to the DEREFERENCED global.
 */
extern int Func_80056cc(void);
extern int Func_8005c68(void);
extern void Func_8005cf8(void);
extern signed char *iwram_3001f1c;

int Func_801f730(int a)
{
    int r;
    int t;
    int k;
    int i;
    signed char *p;

    t = Func_80056cc();
    r = -9;
    if (t == 0) {
        r = Func_8005c68();
        if (a != 0) {
            p = iwram_3001f1c;
            k = 0x1071;
            for (i = 2; i >= 0; i--) {
                if (p[k] != 0) {
                    r--;
                }
                k += 0x40;
            }
        }
    }
    Func_8005cf8();
    return r;
}
