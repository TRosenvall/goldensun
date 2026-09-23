/* Func_801c49c -- NON-MATCHING, 92 encodings of 374 against the tree reference.
 * SIZE EXACT (864 bytes both), INSTRUCTION COUNT EXACT (374 = 374), AND THE 11 POOL
 * WORDS ARE THE ROM'S IN THE ROM'S ORDER.  362 instructions.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/801c49c.c \
 *     asm/rom_15000/rom_1aeec_c_a_c_a_b_a.s
 * ONE function, no data sections -- CONVERTS WHOLE when it lands, no split.
 *
 * A CAUTION ABOUT THE SYMBOLISED-REFERENCE TECHNIQUE, FOUND HERE.  Measured against a
 * symbolised copy of the reference this reads 98; against the TREE reference it reads
 * **92**.  The symbolised copy is WORSE, which is the opposite of that technique's
 * usual direction and means it was OVER-APPLIED -- more constants were rewritten to
 * symbols than the candidate actually uses.  So: symbolise ONLY the constants your
 * candidate spells as symbols, and if the symbolised number is worse than the tree
 * number, the copy is wrong rather than the candidate.  The tree number is primary.
 *
 * All 48 `bl` relocation offsets are identical except a single 2-byte window between
 * 0xba and 0x126 -- TWO ERRORS THAT CANCEL: -1 instruction at the Func_801eadc call
 * (the cse-zero class below) and +1 in the sprite-position block.
 *
 * ================================================================
 * NEW LEVER: A LONG-LIVED LOCAL HOLDING A *SYMBOL ADDRESS* REACHES
 * `ldr rX,=sym / sub rX,#K` WITHOUT PAYING A REGISTER
 * ================================================================
 *
 * This resolves, for the SYMBOL case, the shape docs/elevation.md records as "a
 * `sub rN,#K` applied to a pooled constant ... is not currently reachable" (the
 * Func_80160fc "INVERSE constant problem" entry).
 *
 * This function needs `ldr r0,=0xb1e / sub r0,#2` and `... sub r0,#1` THREE INDEPENDENT
 * TIMES OFF ONE POOL WORD.  Measured:
 *
 *   plain literals 0xb1e / 0xb1c / 0xb1d        3 pool words, pool ORDER wrong, 209
 *   (int)&_MSG_b1e and ... - 2 / - 1 INLINE     base wins r6, `dirty` spills,
 *                                               frame 0x14 -> 0x18, 149
 *   `int mb = (int)&_MSG_b1e;` at the TOP of    the ROM's exact shape: ONE pool word,
 *   the function, used at the three sites       `ldr r0,.L+8` x3, frame 0x14, 149, and
 *                                               the 11-word pool matches word-for-word
 *
 * MECHANISM: gcc-2.96 splits SYMBOL_REF + offset into `base_pseudo +/- K` (verified on a
 * probe -- LEGITIMATE_CONSTANT rejects symbol+offset for Thumb), and cse commons the
 * three base loads into one pseudo.  global.c prices an allocno at about
 * n_refs / live_length, so three refs over a SHORT range beats `dirty`'s seven refs over
 * the whole loop, the base wins a callee-saved register, and a real variable is pushed to
 * the stack.  STRETCHING THE SYMBOL LOCAL'S LIVE RANGE TO THE WHOLE FUNCTION INVERTS
 * THAT PRIORITY; the allocno is denied a hard register, and because it carries a
 * `reg_equiv_constant` RELOAD REMATERIALISES `ldr rX,=sym` AT EVERY USE instead of
 * spilling to the stack.
 *
 * POSITION OF THE ASSIGNMENT IS THE WHOLE LEVER.  Assigned immediately before the loop
 * measured identically to assigned at the top here; assigned INLINE AT EACH USE is 60
 * lines worse.
 *
 * ================================================================
 * BLOCKER CLASS: cse COMMONS `const_int 0` WHERE THE ROM'S COMPILER DID NOT -- AND THE
 * REASON THE ASM BARRIER CANNOT REACH IT IS IN cse.c
 * ================================================================
 *
 * The ROM materialises `mov r3,#0` FOUR separate times (ia/ib zeroing, and twice at one
 * call site); gcc-2.96 substitutes the live pseudo holding `sel = 0`, giving
 * `mov r3, r8`.  Traced to cse1 (.03.cse): insns setting fresh pseudos to 0 are deleted
 * and `canon_reg` replaces their uses with the class's `qty_first_reg`, which is `sel`.
 * COST(pseudo) = 1 against notreg_cost(const_int 0), so THE REGISTER ALWAYS WINS once it
 * is in the table.
 *
 * **WHY THE ASM BARRIERS CANNOT WORK, AND THIS BOUNDS A TOOL THIS PROJECT LEANS ON:**
 * cse.c:5745 flushes the table only when `GET_CODE (PATTERN (insn)) == ASM_OPERANDS`.  A
 * no-operand `asm volatile("")` emits `(asm_input "")`, and ADDING A CLOBBER WRAPS IT IN
 * A PARALLEL -- neither is ASM_OPERANDS, so neither flushes.  The `"i"(0)` form DOES
 * produce ASM_OPERANDS and DOES flush -- and cost more than it saved (362/120 ->
 * 363/199).  So the empty-asm barrier is a SCHEDULING tool only; it has no effect on cse
 * at all, which is worth knowing before spending a budget on it.
 *
 * Exhausted and measured, none worked: three prologue statement orders, passing the
 * argument as `sel` or `(int)spr`, PIN4 on the call, `__asm__ volatile("")`,
 * `... ::: "memory"`, `... :: "i"(0)`, `... "i"(0) : "memory"`.
 *
 * IT COSTS EXACTLY ONE INSTRUCTION: the ROM spends `mov r0,#0 / str r0,[sp]` plus
 * `mov r3,#0` where gcc spends `mov r3,r8 / str r3,[sp]`.  Most of the other ~90
 * differences are register-name shifts downstream of that one extra equivalence class.
 *
 * Ladder: 323 -> 209 (symbols + literal b1c/b1d) -> 189 (descending PIN2 x7) -> 149
 * (`mb` remat) -> 120 (`v` local for _MSG_b19 + asm barrier) -> 108 (named `p = spr` at
 * the store site) -> 92.
 *
 * ARGUMENT-ORDER PINS HERE ARE *DESCENDING*, WITH A CORPUS CONTROL.
 * `_GiveInnateMove(who, move)` x7: the ROM emits `mov r1,#0x8c` BEFORE `mov r0,#N` at
 * every site.  rank_for_schedule's last tie-break is INSN_LUID and
 * `load_register_parameters` walks args[] FORWARD (LOAD_ARGS_REVERSED undefined for
 * ARM), so plain C always emits r0 first.  `{ PIN2; q1 = move; q0 = who; f(q0,q1); }`
 * matched all seven (209 -> 189); THE ASCENDING FILL WAS INERT.  Corpus control: 118
 * generated .s files show r1-then-r0 against 191 r0-then-r1, so both orders are
 * reachable and this is a source signal rather than a toolchain artefact.
 *
 * ================================================================
 * SIX SYMBOL TELLS, ALL REPORTED AND ALL WITHHELD
 * ================================================================
 *
 * All six are the __MessageID space, verified only via `__asm__(".equ ...")` in the
 * scratch candidate.  Withheld under batch 282's precedent -- add on evidence AND
 * completion -- because the file is not complete.
 *
 *   _MSG_66   0x66   STRONGEST.  Pooled although it fits `mov rN,#imm8`.  In-function
 *                    controls that DO reproduce as plain literals: 0x8c/0x8d/0x4e/0x5d
 *                    (move ids), 0x6f/0x70/0x71 (sound ids), and eleven coordinates.
 *                    Namespace fixed by src/rom_a1000/rom_a1050_c_c_c_c_a.c naming
 *                    _MSG_b20 on the same callee and parameter position.
 *   _MSG_b1e  0xb1e  The derive shape above, plus pool order.
 *   _MSG_333  0x333  Held in r5 across two uses.  CROSS-FUNCTION CONTROL: Func_8022b44
 *                    has the identical `and r0,#0x3fff / ldr r3,=0x333 / add r0,r3`
 *                    shape on the same callee.
 *   _MSG_b19  0xb19  The ROM's `ldr r5,=X / mov r0,r5` only reproduces from a named
 *                    local holding the symbol.
 *   _MSG_aec  0xaec  WEAK individually -- not shiftable, so they pool either way.
 *   _MSG_53a  0x53a  Carried only by the fact that all six together make the pool word
 *                    order match the ROM exactly.  Do not add these two alone.
 *
 * No per-file Makefile flag override applies to this stem.
 */
extern void *Func_8004938(unsigned int size);
extern void _GiveInnateMove(int who, int move);
extern unsigned int Func_801c7fc(void *buf);
extern void Func_801c8a0(unsigned int *a, unsigned int *b, void *buf);
extern unsigned char *CreateUIBox(int a, int b, int c, int d, int e);
extern int AllocSpriteSlot(void);
extern void UploadSpriteGFX(int slot, int n, void *gfx);
extern unsigned char *Func_801eadc(int slot, unsigned int m, void *box, int d, int e);
extern void Func_801e7c0(int id, void *box, int x, int y);
extern void Func_8016498(void *box);
extern void Func_801e41c(void *box, int b, int c, int d, int e);
extern void Func_801e9d4(int a, int b, void *box, int x, int y);
extern unsigned char *_GetMoveInfo(int id);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void CloseUIBox(void *box, int mode);
extern void free(void *p);
extern volatile int gKeyRepeat;
extern volatile int gKeyPress;
extern unsigned char gState[];
extern int _MSG_66;
extern int _MSG_333;
extern int _MSG_53a;
extern int _MSG_aec;
extern int _MSG_b19;
extern int _MSG_b1e;
__asm__(".equ _MSG_66, 0x66");
__asm__(".equ _MSG_333, 0x333");
__asm__(".equ _MSG_53a, 0x53a");
__asm__(".equ _MSG_aec, 0xaec");
__asm__(".equ _MSG_b19, 0xb19");
__asm__(".equ _MSG_b1e, 0xb1e");
extern unsigned char Data_310a4[];

#define PIN2 register int q0 __asm__("r0"); register int q1 __asm__("r1")

void Func_801c49c(void)
{
    unsigned char *buf;
    unsigned char *boxA;
    unsigned char *boxB;
    unsigned char *boxC;
    unsigned char *spr;
    unsigned char *info;
    unsigned char *p;
    unsigned int n;
    unsigned int ia;
    unsigned int ib;
    unsigned int idx;
    int sel;
    int dirty;
    int slot;
    int v;
    unsigned int g;
    unsigned int o;
    unsigned int q;
    int mb;

    spr = 0;
    mb = (int)&_MSG_b1e;
    sel = 0;
    dirty = 1;
    __asm__ volatile ("");
    buf = Func_8004938(0x700);
    { PIN2; q1 = 0x8c; q0 = 0; _GiveInnateMove(q0, q1); }
    { PIN2; q1 = 0x8c; q0 = 1; _GiveInnateMove(q0, q1); }
    { PIN2; q1 = 0x8c; q0 = 2; _GiveInnateMove(q0, q1); }
    { PIN2; q1 = 0x8d; q0 = 2; _GiveInnateMove(q0, q1); }
    { PIN2; q1 = 0x4e; q0 = 2; _GiveInnateMove(q0, q1); }
    { PIN2; q1 = 0x5d; q0 = 3; _GiveInnateMove(q0, q1); }
    { PIN2; q1 = 0x8c; q0 = 5; _GiveInnateMove(q0, q1); }
    ib = 0;
    ia = 0;
    n = Func_801c7fc(buf);
    if (n != 0) {
        Func_801c8a0(&ia, &ib, buf);
        boxA = CreateUIBox(4, 6, 0x14, 7, 2);
        boxB = CreateUIBox(4, 3, 0x14, 3, 2);
        boxC = CreateUIBox(4, 0xe, 0x14, 5, 2);
        slot = AllocSpriteSlot();
        if (slot != 0) {
            UploadSpriteGFX(slot, 0x80, Data_310a4);
            spr = Func_801eadc(slot, 0x80 << 23, boxA, 0, 0);
        }
        v = (int)&_MSG_b19;
        Func_801e7c0(v, boxB, 0x10, 0);
    L1:
        if (dirty != 0) {
            dirty = 0;
            ia = (ia + n) % n;
            ib = (ib + n) % n;
            sel = (sel + 2) % 2;
            v = (sel << 4) + (*(unsigned short *)(boxA + 0xe) << 3) + 0x1c;
            p = spr;
            *(unsigned short *)(p + 8) = v;
            p[0x14] = v;
            Func_8016498(boxA);
            Func_801e41c(boxA, 1, 2, 0x11, 2);
            Func_801e7c0((int)&_MSG_b1e, boxA, 0x30, 0);
            Func_801e7c0(*(unsigned short *)(buf + ia * 4 + 2) + (int)&_MSG_333, boxA, 0x38, 0x10);
            Func_801e7c0(*(unsigned short *)(buf + ib * 4 + 2) + (int)&_MSG_333, boxA, 0x38, 0x20);
            Func_801e7c0(mb - 2, boxA, 0x10, 0x10);
            Func_801e7c0(mb - 1, boxA, 0x10, 0x20);
            Func_801e7c0(*(unsigned short *)(buf + ia * 4) + (int)&_MSG_66, boxA, 0x68, 0x10);
            Func_801e7c0(*(unsigned short *)(buf + ib * 4) + (int)&_MSG_66, boxA, 0x68, 0x20);
            Func_8016498(boxC);
            Func_801e7c0((int)&_MSG_aec, boxC, 0, 0x10);
            if (sel != 0) {
                info = _GetMoveInfo(*(unsigned short *)(buf + ib * 4 + 2));
                idx = ib;
            } else {
                info = _GetMoveInfo(*(unsigned short *)(buf + ia * 4 + 2));
                idx = ia;
            }
            v = *(unsigned short *)(buf + idx * 4 + 2);
            Func_801e9d4(info[9], 2, boxC, 0x40, 0x10);
            Func_801e7c0(v + (int)&_MSG_53a, boxC, 0, 0);
        }
        WaitFrames(1);
        if (gKeyRepeat & 0x20) {
            _PlaySound(0x6f);
            if (sel != 0)
                ib--;
            else
                ia--;
            dirty = 1;
        }
        if (gKeyRepeat & 0x10) {
            _PlaySound(0x6f);
            if (sel != 0)
                ib++;
            else
                ia++;
            dirty = 1;
        }
        if (gKeyRepeat & 0x40) {
            _PlaySound(0x6f);
            sel--;
            dirty = 1;
        }
        if (gKeyRepeat & 0x80) {
            _PlaySound(0x6f);
            sel++;
            dirty = 1;
        }
        if (gKeyPress & 1) {
            _PlaySound(0x70);
        } else if (gKeyPress & 2) {
            _PlaySound(0x71);
        } else if (!(gKeyPress & 8)) {
            goto L1;
        } else {
            _PlaySound(0x71);
        }
        p = buf + ia * 4;
        g = (unsigned int)&gState;
        o = 0x88;
        o <<= 2;
        q = g + o;
        *(unsigned short *)q =
            (*(unsigned short *)p << 10) | *(unsigned short *)(p + 2);
        p = buf + ib * 4;
        g += 0x222;
        *(unsigned short *)g =
            (*(unsigned short *)p << 10) | *(unsigned short *)(p + 2);
        CloseUIBox(boxA, 1);
        CloseUIBox(boxB, 1);
        CloseUIBox(boxC, 1);
        WaitFrames(1);
    }
    free(buf);
}
