/* OvlFunc_882_20090a4 (0x020090a4) -- NON-MATCHING.
 * Blocker class: SCRATCH-REGISTER choice (r2 against r3), 8 lines of 80.
 *
 * Exact length, and every remaining difference is the same one instruction
 * repeated: the ROM builds the value destined for [sp] in r2, we build it in
 * r3.
 *
 *     rom    mov r2, #0xf / str r2, [sp] / mov r10, r2
 *     ours   mov r3, #0xf / str r3, [sp] / mov r10, r3
 *
 * TWO LEVERS GOT IT HERE and both are worth keeping.
 *
 * 1. -ffixed-r7. Eight six-argument calls share four stack-argument constants,
 *    and the ROM holds them in r5, r6, r8 and r10 -- skipping r7 entirely.
 *    gcc's REG_ALLOC_ORDER reaches r7 before r8, so it spends r5/r6/r7/r8 and
 *    the prologue is wrong from instruction 0 (76 lines against 80, 80
 *    differing). Reserving r7 fixes the prologue outright: 78 lines, and the
 *    first difference moves to instruction 1. FIXEDR7_CFLAGS exists for this.
 *
 * 2. THE REASSIGNED LOCAL HAD TO BE SPLIT. The ROM runs r5 through 0x35 and
 *    then 0x36, which reads as one variable reassigned -- the merge lever's
 *    usual shape. It is the opposite here: one variable is 78 lines and 64
 *    differing, and two separate locals is 80 lines and 8. gcc coalesces the
 *    single variable's two live ranges into one register and then needs one
 *    FEWER callee-saved register than the ROM, which is where the two missing
 *    lines were.
 *
 *    So "one register for two values means one variable" has a counter-case:
 *    when the ROM spends a register the source must FORCE, two variables can
 *    be what creates the demand. Read the push list, not just the register.
 *
 * WHAT IS RIGHT: the four constants the ROM holds in callee-saved registers get
 * names (0xf, 0xe, 0xd, and the 0x35/0x36 pair); the two it materialises fresh
 * immediately before their stores -- 0x34 and 0x37 -- stay literals. That is
 * the recorded stack-argument reading applied off the `str` operands, and it is
 * right at all eight sites.
 *
 * NEXT: the r2/r3 choice. Nothing in the recorded levers addresses which
 * scratch register gcc picks for a value that is stored and then copied.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_882_20090a4(void)
{
    int a;
    int b;
    int c;
    int t;
    int u;

    a = 0xf;
    t = 0x35;
    __Func_8010704(0x1d, 0x17, 1, 1, a, t);
    b = 0xe;
    __Func_8010704(0x1d, 0x17, 1, 1, b, t);
    c = 0xd;
    __Func_8010704(0x1d, 0x17, 1, 1, c, t);
    __Func_8010704(0x1a, 0x14, 2, 1, b, 0x34);
    u = 0x36;
    __Func_8010704(0x19, 0x15, 1, 1, c, u);
    __Func_8010704(0x19, 0x15, 1, 1, a, u);
    __Func_8010704(0xe, 0x35, 1, 1, b, u);
    __Func_8010704(0xd, 0x37, 1, 1, a, 0x37);
}
