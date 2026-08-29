/* OvlFunc_923_20091b4  [ovl_7aa430]  --  0x020091b4
 *
 * Source asm: goldensun/asm/overlays/rom_7aa430/ovl_1150_c_c.s
 *
 * Two of twenty-eight, brought down from nine by two findings this round.
 *
 * SOLVED ON THE WAY, and both are reusable:
 *
 *   * `__Func_8091f90` TAKES AN AREA ID. The ROM pools 0x35, which fits in a
 *     `mov`, and that is the pool tell -- the operand was a symbol. Two already
 *     elevated files pass `_AREA_51` and `_AREA_4d` to the same callee, which
 *     identifies the parameter. `_AREA_35` added to area.sym.
 *   * THE IWRAM ADDRESS IS ONE EXPRESSION, NOT A WALK. `base = iwram + off` in
 *     a single statement gives the ROM's register roles; `base = iwram;
 *     base += off;` swaps which register holds the base and which the offset,
 *     and costs six positions. The gState pointer three lines later wants the
 *     OPPOSITE -- written as one expression gcc folds it into a single pool
 *     constant `gState+555` and it is worse. Same function, two pointer
 *     computations, two different right answers, and only the ROM says which.
 *
 * Blocker: TWO POOL LOADS IN THE WRONG ORDER.
 *
 *     rom    ldr r3, =gState / ldr r2, =0x22b / add r3, r2
 *     ours   ldr r2, =0x22b  / ldr r3, =gState / add r3, r2
 *
 * This is NOT the pool-loads-first class -- both operands are pool loads, so
 * there is no mov for one to jump ahead of. It is the order of two loads.
 *
 * Tried: the offset as a named local, the offset named and assigned before the
 * gState pointer, and the whole thing as one expression. 2, 2 and 5.
 *
 * The function is STRAIGHT-LINE, so the basic-block lever has nowhere to put
 * anything -- see docs/elevation.md.
 *
 * REVISITED, and reclassified.  This is NOT a register-role swap: the
 * `add r3, r2` is identical on both sides and both operands land in the same
 * registers.  Only the EMISSION ORDER of two independent pool loads differs.
 *
 * The useful measurement is corpus-wide rather than local.  The shape
 *
 *      ldr rA, =<symbol> / ldr rB, =<constant> / add rA, rB
 *
 * occurs at 25 sites in the ROM's assembly, and in 3,156 elevated translation
 * units it has been reproduced ZERO times.  So this is not a spelling somebody
 * else already found and I am missing; no source form in this tree has ever
 * produced it.  That makes 20091b4 a test case for a corpus-wide unsolved shape
 * rather than a one-off.
 *
 * ALSO TRIED this round, all 2: the offset named and assigned AFTER the pointer
 * (the park previously recorded only the "before" direction); a non-compound
 * `g = g + 0x22b`; the array-index form `g[0x22b] = 3`; gState declared as a
 * plain array instead of a struct; and six flag groups -- CSE, GCSE, ALIAS,
 * -fno-schedule-insns, SCHED2 and O1.  SCHED2 and O1 are worse (6 differing),
 * which says the ROM was built with both, and -fno-schedule-insns changes
 * nothing, which says the pass responsible is not sched1.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int iwram_3001ebc;
extern int _AREA_35;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80925cc(int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_8091f90(int id, int b);
extern void __Func_8091eb0(int a, int b);

void OvlFunc_923_20091b4(void)
{
    unsigned char *base;
    unsigned int off;
    unsigned char *g;

    __CutsceneStart();
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    off = 0xe0;
    off <<= 1;
    base = (unsigned char *)iwram_3001ebc + off;
    off += 0x40;
    *(unsigned int *)base = off;
    __Func_8091f90((int)(&_AREA_35), 0x1f);
    g = (unsigned char *)&gState;
    g += 0x22b;
    *g = 3;
    __Func_8091eb0(0x24, 1);
    __CutsceneEnd();
}
