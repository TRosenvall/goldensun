/* Cluster OvlFunc_941_20080d4..OvlFunc_941_20080d4 extracted from
 * goldensun/asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_a_c_a.s.
 *
 * Eight six-argument calls, so sixteen stack arguments, and the whole function
 * is a clean demonstration of WHICH of them get names.
 *
 * TWO SHARED VALUES ARE NAMED because the ROM spends a callee-saved register on
 * each: `n = 0x15` lives in r6 and is the fifth argument at six of the eight
 * calls, and `m = 0x3a` lives in r5 and is the sixth at two of them.  The ROM
 * stores them with `str r6, [sp]` / `str r5, [sp, #4]` rather than
 * materialising the constant again, which is what a named local produces.
 *
 * TWO SITES NEED THEIR OWN PAIR.  At the __Func_80105d4 call with (0x18, 0x3e)
 * and the __Func_8010704 call with (0x16, 0xf), neither stack argument is one
 * of the shared values, and the ROM builds BOTH into separate registers before
 * storing either:
 *
 *      rom   mov r3, #0x18 / mov r2, #0x3e / str r3, [sp] / str r2, [sp, #4]
 *      ours  mov r3, #0x18 / str r3, [sp]  / mov r3, #0x3e / str r3, [sp, #4]
 *
 * Written as literals gcc reuses one register for both and interleaves the
 * stores; a named pair per site gives each its own register.  6 differing to
 * exact.  This is the batch-144 rule with a discriminator attached: name the
 * pair when the ROM materialises BOTH fresh, and reference the shared local
 * when the ROM stores from a register it is already holding.  Reading which is
 * which off the `str` operands takes a second and decides it.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int a, int b);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __WaitFrames(int n);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_941_20080d4(void)
{
    unsigned char *e;
    int n;
    int m;
    int p1, q1, p2, q2;

    e = __MapActor_GetActor(0xa);
    __MapActor_SetAnim(0xa, 5);
    if (e != 0) {
        __Actor_SetSpriteFlags(e, 0);
        e[0x23] = 1;
    }
    n = 0x15;
    __Func_80105d4(0x29, 0x57, 2, 5, n, 0x3b);
    __WaitFrames(4);
    p1 = 0x18;
    q1 = 0x3e;
    __Func_80105d4(3, 0x5d, 1, 1, p1, q1);
    __Func_80105d4(1, 0x5e, 1, 1, n, 0x37);
    m = 0x3a;
    __Func_80105d4(0x2b, 0x57, 2, 5, n, m);
    __WaitFrames(4);
    __Func_80105d4(0x29, 0x57, 2, 5, n, m);
    __WaitFrames(4);
    __WaitFrames(4);
    __Func_8010704(0x15, 0xb, 2, 2, n, 0xd);
    p2 = 0x16;
    q2 = 0xf;
    __Func_8010704(0x15, 0xb, 1, 1, p2, q2);
    __Func_8010704(0x13, 0x11, 1, 1, n, 0xe);
}
