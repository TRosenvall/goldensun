/* Func_80b9dc4 (RunRetreatSequence) -- 0x080b9dc4.  BYTE-EXACT, ONE SHIM.
 *
 *   OK Func_80b9dc4 -- 252 bytes, 108 encodings and 16 relocations identical
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this file> \
 *     asm/rom_b5000/rom_b9b30_a_c.s --func Func_80b9dc4
 *
 * SIZE 252 == 252, ENCODINGS 108 == 108, RELOCATIONS 16 == 16.
 *
 * SHIMS: python3 tools/shimcount.py reports
 *     "+r" barriers : 1  (classed as a fakematch)
 * i.e. ONE `__asm__ ("" : "+r" (flag));` and nothing else -- no register pin, no
 * .equ, no volatile, no flag override.  IT NEEDS A fakematch.txt ROW:
 *     Func_80b9dc4       <landed path>
 *
 * SPLIT SHAPE.  asm/rom_b5000/rom_b9b30_a_c.s holds TWO functions
 * (anchored `grep -cE '^[[:space:]]*\.?thumb_func_start'` = 2):
 *     Func_80b9dc4   @ 0x080b9dc4   <- this one
 *     Func_80b9ec0   @ 0x080b9ec0   <- still parked, src/non_matching/rom_b5000/80b9ec0.c
 * tools/datacheck.py prints NOTHING for this file: no data section, so there is
 * NO data label to export and NO linker alias.  The landing is a TEXT-ONLY split:
 * cut Func_80b9dc4 out, leave Func_80b9ec0 in the residual `.s`.
 *
 * ============================== WHAT CLOSED IT ==============================
 *
 * The park was at 1 of 108: index 25, ROM `mov r2, r7` (1c3a = ADD r2,r7,#0)
 * against our `mov r2, #0` (2200).  r7 is `flag`, already 0; r2 is `t`.
 *
 * The park had the PASS right and the mechanism right -- cse1 (.03.cse) folds
 * `(set (reg t) (reg flag))` to `(set (reg t) (const_int 0))` -- and said the
 * remaining question was HOW the ROM's copy survived.  Confirmed from the dumps
 * of this very file: .00.rtl has
 *     (insn 36 ...)  (set (reg/v:SI 36) (const_int 0))        ; flag = 0
 *     (jump_insn 44 ...) if_then_else (gtu (reg 44) (const_int 7))
 *     (note 309 ... [bb 1] NOTE_INSN_BASIC_BLOCK)
 *     (insn 49 ...)  (set (reg/v:SI 37) (reg/v:SI 36))        ; t = flag
 * and .03.cse already shows insn 49 as `(set (reg 37) (const_int 0))` with a
 * REG_EQUAL note.  .07.gcse and .09.cse2 do not change it back.
 *
 * ANSWER TO THE PARK'S OPEN QUESTION -- cse1's extended basic block SPANS A
 * CONDITIONAL JUMP'S FALL-THROUGH.  insn 49 is in a fresh NOTE_INSN_BASIC_BLOCK
 * (bb 1) and cse still folds, and `-fno-cse-follow-jumps` / `-fno-cse-skip-blocks`
 * change nothing.  So the escape is a read in a block reached BY A JUMP, not a
 * read after a branch.  THE CORPUS PROVES THE POSITIVE FORM: in the generated
 * asm/src pair src/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_b.c, `for (i = ret; ...)`
 * with `ret` already zero KEEPS the copy -- and that loop sits in the ELSE arm,
 * at label .L3, i.e. a jumped-to block.  Its own header records the rule from the
 * other side: "a `mov rX,#0` there is NOT what gcc emits when the zero is already
 * named and live".  Here the ROM's read is in the FALL-THROUGH (`cmp r3,#7 /
 * bhi .Lb9e64` then straight into the t block), so no rewrite of the control flow
 * reaches it while keeping the ROM's branch direction -- inverting the `if` gives
 * 87 of 104 at 240 bytes, because the layout inverts with it.
 *
 * WHY A BARRIER IS THE RIGHT SHIM HERE AND COSTS NOTHING.  `flag` lives in r7, a
 * LOW register, so `__asm__ ("" : "+r" (flag))` (thumb "r" = LO_REGS) needs no
 * copy in or out: 108 encodings, 252 bytes, unchanged.  Contrast
 * src/non_matching/rom_b5000/80b6d30.c, the same cse1 residue, where the zero
 * lives in `sl`: every barrier constraint class costs the same two instructions
 * ("+r" 96 at 121 enc, "+h" 96 at 121, "+l" 100 at 123, "+g" 98 at 121 -- all
 * measured in batch 297a).  SO THE RULE IS: a "+r" barrier is free when the
 * value is already in a low register and costs two instructions when it is in a
 * hi register.  That is the discriminator to check before reaching for it.
 *
 * ============ WHAT WAS RULED OUT FOR A PIN-FREE LANDING (all measured) ========
 *
 * FLAG SWEEP, 15 toggles, none touches index 25 (all still 1 of 108 unless noted):
 *   -fno-cse-follow-jumps, -fno-cse-skip-blocks, -fno-rerun-cse-after-loop,
 *   -fno-expensive-optimizations, -fno-schedule-insns, -fno-thread-jumps,
 *   -fno-function-cse, -fno-peephole, -fno-caller-saves, -fno-delayed-branch,
 *   -fno-defer-pop.  WORSE: -fno-gcse (91, 114 enc), -fno-schedule-insns2 (33),
 *   -fno-strength-reduce (106, 112 enc), -fno-force-mem (108, 112 enc).
 *   So it is cse1, which has no switch.
 *
 * EXPRESSION REWRITES OF THE COPY, all exactly 1 (the fold is on the VALUE, so
 * every algebraic identity folds with it):  `t = flag | 0`, `flag & ~0`,
 * `flag ^ 0`, `flag * 1`, `0 + flag`, `flag + 0`, `-(-flag)`, `(int)flag`,
 * `(int)(unsigned)flag`, `flag >> 0`.
 *
 * THE ADDRESSOF ROUTE IS CLOSED AT THE FRONT END.  `.04.addressof` runs AFTER
 * cse1, so a value that is a MEM at cse time and a register afterwards would
 * hide the constant -- but the C front end folds `*&x` to `x` before RTL, so
 * `t = *&flag`, `t = *(int *)&flag`, `t = ((int *)&flag)[0]`, a `fp = &flag`
 * pointer local, `*fp = 0` with a plain read, and a one-member union all measure
 * exactly 1.  Only `*(volatile int *)&flag` changes anything and it is worse
 * (89 at 110 enc).
 *
 * OTHER SHIMS, for the record: `register int flag __asm__("r7")` is 11 (a pin is
 * WORSE than the barrier here); `register int t __asm__("r2")` is 1 (inert);
 * `volatile int t` 108 at 116 enc; `volatile int flag` 113 at 116 enc; an
 * input-only barrier `__asm__ ("" : : "r" (t))` after the copy is 4.
 *
 * Everything the park already listed as inert was re-confirmed.  The rest of the
 * function is unchanged from the park and was exact before this edit: the frame
 * (buf[14] declared BEFORE pair[2] puts pair at sp, buf at sp+4),
 * `(Random() * 10) >> 16 <= 6` with an unsigned Random, the ldrsh loop over buf
 * from n-1 down to -1, and the dead `pair` stores.
 */
extern unsigned char iwram_3001f00[];

extern unsigned char *_GetUnit(int id);
extern void Func_80c10e8(unsigned short *p, int n);
extern void _Func_80175a0(int id);
extern void WaitTextPrompt(void);
extern int Func_80b6b40(int kind, unsigned short *buf);
extern void Func_80b8064(int id);
extern void WaitFrames(int n);
extern unsigned int Random(void);
extern void Func_80bac6c(int id);
extern void Func_80b7e60(int id);

int Func_80b9dc4(unsigned char *p)
{
    short buf[14];
    unsigned short pair[2];
    int *g;
    unsigned char *b;
    unsigned char *u;
    int flag;
    int t;
    int i;

    g = *(int **)iwram_3001f00;
    b = *(unsigned char **)((char *)iwram_3001f00 - 0x8c);
    g[0] = 0x80 << 6;
    g[4] = 1;
    Func_80c10e8(0, 0);
    flag = 0;
    if (*p <= 7) {
        __asm__ ("" : "+r" (flag));
        t = flag;
        if (b[0x45] != 2)
            t = 1;
        if (t == 0) {
            _Func_80175a0(0x847);
            WaitTextPrompt();
        } else {
            for (i = Func_80b6b40(1, (unsigned short *)buf) - 1; i != -1; i--) {
                u = _GetUnit(buf[i]);
                if (u[0x13b] == 0 && u[0x13c] == 0) {
                    Func_80b8064(buf[i]);
                    WaitFrames(8);
                }
            }
            WaitFrames(0x16);
            flag = 1;
        }
    } else if ((Random() * 10) >> 16 <= 6) {
        pair[0] = *p;
        pair[1] = 0xff;
        Func_80b8064(*p);
        WaitFrames(8);
        Func_80bac6c(*p);
        Func_80b7e60(*p);
    } else {
        _Func_80175a0(0x847);
        WaitTextPrompt();
    }
    g[4] = 0;
    return flag;
}
