/* Cluster OvlFunc_888_20084e8..OvlFunc_888_20084e8 extracted from
 * goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_a_c.s.
 *
 * AN ELEVEN-ARGUMENT CALL, and it is the clearest demonstration yet of the
 * batch-149 stack-argument rule -- generalised from "each site needs its own
 * PAIR" to "each stack SLOT needs its own local".
 *
 * __Func_80931ec takes four register arguments and SEVEN stack words.  The ROM
 * materialises every stack value into its own register first and only then
 * issues the stores:
 *
 *      mov r0,#1 / mov r3,#3 / mov r2,#7 / mov r1,#0x10 / mov r4,#0xe
 *      str r0,[sp] / str r3,[sp,#4] / str r2,[sp,#8] / str r0,[sp,#0x10]
 *      mov r5,#0 / ... / str r1,[sp,#0xc] / str r4,[sp,#0x14] / str r5,[sp,#0x18]
 *
 * Written as eleven literals gcc reuses ONE register for all seven --
 * `mov r3,#3 / str r3 / mov r3,#7 / str r3 / ...` -- which is 55 lines against
 * 56 and 22 differing, and it never spends the callee-saved r5 the ROM pushes.
 * Naming three of the seven is not enough and is WORSE (54 lines, 28); the rule
 * is all-or-nothing because the register the extra locals compete for is the
 * one that decides the prologue.  All seven named: 22 differing -> 2.
 *
 * TWO OF THE SEVEN ARE SHARED, read off the `str` operands as usual: `a1 = 1`
 * is stored to [sp] and [sp,#0x10] from the same register, and `a4 = 0x10` is
 * BOTH the second register argument and the [sp,#0xc] store.  The assignment
 * order follows the ROM's materialisation order -- 1, 3, 7, 0x10, 0xe, then the
 * zero last, which is the one that lands in r5.
 *
 * The last two lines were __Func_8092c40 wanting `mov r0` at the END of its
 * setup, so it is DELIBERATELY UNDECLARED -- the batch-147 lever, and the third
 * function to need it on this same callee (see OvlFunc_941_20091b8 and
 * OvlFunc_951_20089f8).  Do not add a prototype for it.
 */
extern unsigned char L3c9c[] __asm__(".L3c9c");
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __LoadFieldActors(void *p);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int a, int b);
extern void __Func_80931ec(int a, int b, int c, int d, int e, int f, int g,
                           int h, int i, int j, int k);

void OvlFunc_888_20084e8(void)
{
    unsigned char *p;
    int a1, a2, a3, a4, a5, a6;

    __CutsceneStart();
    __LoadFieldActors(L3c9c);
    __WaitFrames(1);
    __MessageID(0x1bfd);
    __Func_8092c40(9, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __ActorMessage(9, 0);
    } else {
        p = iwram_3001ebc;
        *(unsigned short *)(p + (0xec << 1)) += 1;
        a1 = 1;
        a2 = 3;
        a3 = 7;
        a4 = 0x10;
        a5 = 0xe;
        a6 = 0;
        __Func_80931ec(2, a4, 1, 0x18, a1, a2, a3, a4, a1, a5, a6);
        __ActorMessage(9, 0);
    }
    __CutsceneEnd();
}
