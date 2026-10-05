/* Func_801c244 -- 0x0801c244  (asm/rom_15000/rom_1aeec_c_a_a_a_a_a_c_a_a_c.s)
 *
 * MATCHING.  objcmp --func and --whole both green:
 *   OK whole file -- 140 bytes, 55 encodings and 15 relocations identical
 * The park read 31 of 55, ref 140 bytes against ours 132, with relocations
 * differing at a shifted offset.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1aeec_c_a_a_a_a_a_c_a_a_c.c \
 *     asm/rom_15000/rom_1aeec_c_a_a_a_a_a_c_a_a_c.s --whole
 *
 * SPLIT SHAPE: none.  The .s holds this function alone.  PINS: 0.  No flag
 * group.
 *
 * THE PARK DIAGNOSIS IS REFUTED.  It called this the third member of the "-1
 * rematerialisation" family in src/non_matching/overlays/constant_reuse.c,
 * claiming gcc shares a -1 the ROM builds fresh in each arm.  It does not:
 * even the park body rematerialised -1 in cases 3 and 4 exactly as the ROM
 * does.  Case 1 was missing its copy because jump2 had CROSS-JUMPED the whole
 * arm into case 4 -- and the cross-jump was itself a consequence of the branch
 * polarity, so runs A, B and C were ONE cause, not three.
 *
 * Two edits.
 *
 * 1. THE FUNCTION IS NOT void.  thumb_exit, arm.c:8306-8320:
 *        if (size == 0)
 *          {
 *            if (mode == VOIDmode)
 *              regs_available_for_popping = r0|r1|r2;
 *            else
 *              regs_available_for_popping = r1|r2;
 *          }
 *        else if (size <= 4)
 *          regs_available_for_popping = r1|r2;
 *    where size is GET_MODE_SIZE of current_function_return_rtx, or of
 *    DECL_MODE (DECL_RESULT) when that is null.  The matching loop below then
 *    clears all but the lowest available bit.  So a void function pops its
 *    return address into r0 and anything with a 1-4 byte return pops it into
 *    r1.  The ROM ends "pop {r1} / bx r1", so this function returns a value.
 *
 * 2. THE ARMS EXIT THROUGH A goto LABEL, AND THE TESTS ARE INVERTED.  The
 *    polarity is settled in jump.c, and it is a RACE between two
 *    transformations that between them collapse every obvious spelling onto
 *    the wrong answer:
 *
 *      - jump.c:348 does
 *            nlabel = follow_jumps (JUMP_LABEL (insn));
 *            if (nlabel != JUMP_LABEL (insn)) redirect_jump (insn, nlabel, 1);
 *        and it runs BEFORE the whole else-if chain.  With
 *        "if (c) return; break;" the false-label of the conditional holds a
 *        bare jump to the end of the switch, which follow_jumps (jump.c:2350 --
 *        it needs a JUMP_INSN with a trailing BARRIER) chains all the way to
 *        the loop top.  Result: bne LOOPTOP / b EXIT.
 *      - jump.c:434, "conditional jump jumping over an unconditional jump",
 *        inverts when prev_active_insn (reallabelprev) == insn.  With
 *        "if (c) break; return;" follow_jumps is blocked but THIS fires and
 *        inverts back to bne LOOPTOP / b EXIT.
 *
 *    What breaks the race is that a bare "return;" in a NON-void function
 *    expands to (clobber (reg/i:SI 0 r0)) PLUS the jump.  That clobber is an
 *    active insn, so it blocks follow_jumps (it is not a JUMP_INSN) and it
 *    also blocks the invert (prev_active_insn of the jump is the clobber, not
 *    the conditional) -- which is why an int return type with in-arm returns
 *    stays flat at 31 in all four spellings of the arms.  Routing every arm
 *    through "goto out;" puts the single clobber at the one trailing
 *    "return;", so each arm exit label holds a bare jump that DOES chain: the
 *    conditional is redirected straight to the return label and the shape in
 *    the ROM, "cond -> EXIT ; b LOOPTOP", falls out.
 *
 *    Once cases 1-3 carry that polarity, case 1 ends "bne EXIT / b LOOPTOP"
 *    while case 4 still ends "beq LOOPTOP" with EXIT as its fallthrough.  The
 *    tails stop being identical, jump2 tail cross-jumping has nothing to
 *    merge, and the missing four instructions come back.
 *
 * MEASURED, as a control that separates the two causes: void + inverted arms +
 * in-arm "return;" reads 2 of 55 with counts equal at 55/55 -- the polarity
 * alone, leaving only pop {r0} against pop {r1}.  And "break" versus
 * "continue" as the loop-continuing statement is byte-identical here (both
 * land); "break" is kept because the arms are inside a switch.
 *
 * Measured inert, all at 31 with identical relocations: int return with
 * in-arm returns, in all four combinations of arm polarity and break/continue.
 */
extern char *iwram_3001ebc;
extern void Func_801c2d0(void);
extern void Func_801c2e4(void);
extern int Func_8028920(int n);
extern int _Func_808ce74(void);
extern int _Func_80a5b94(void);
extern int _Func_80aa56c(void);
extern int _Func_80a24d0(void);
extern int _Func_80a7478(void);

int Func_801c244(void)
{
    char *p;
    int r;
    int v;

    p = iwram_3001ebc;
    r = 0;
    for (;;) {
        Func_801c2d0();
        r = Func_8028920(r);
        Func_801c2e4();
        switch (r) {
        case 0:
            v = _Func_808ce74();
            if (v == 0)
                v = 0xff;
            *(unsigned short *)(p + (0xbd << 1)) = v;
            goto out;
        case 1:
            if (_Func_80a5b94() == -1)
                break;
            goto out;
        case 2:
            if (_Func_80aa56c() != 0)
                break;
            goto out;
        case 3:
            if (_Func_80a24d0() == -1)
                break;
            goto out;
        case 4:
            if (_Func_80a7478() == -1)
                break;
            goto out;
        default:
            goto out;
        }
    }
out:
    return;
}
