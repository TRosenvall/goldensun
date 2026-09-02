/* Cluster OvlFunc_941_2008210..OvlFunc_941_2008210 extracted from
 * goldensun/asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_a.s.
 *
 * A near-twin of ovl_30_c_a_c_c_a_c_a_b.c -- same callees, same six-argument
 * shape, same (0x18, 0x3e) and (0x16, 0xf) pairs -- but with a guarded prologue
 * in front, and the guard is what makes it interesting.
 *
 * A CALLEE-SAVED REGISTER RE-LOADED WITH THE SAME IMMEDIATE AFTER A JOIN IS TWO
 * VARIABLES, NOT ONE.  The obvious spelling gives the if-body and the code
 * after it one shared `n = 0x15` / `m` pair, since both need the value.  That is
 * structurally plausible and it costs 18 instructions: the pseudo's live range
 * then spans the whole function, its global-alloc priority drops below the
 * short-lived pair locals, gcc puts it in r10, and every one of the eight
 * stack-argument sites pays `mov r3, r10 / str r3, [sp]` instead of one `str`.
 *
 * The tell is in the reference listing.  `mov r5, #0x15` is materialised again
 * after the join even though r5 already holds 0x15 on the taken path.  gcc does
 * not re-materialise a value it kept live across a branch, so the second `mov`
 * is a DIFFERENT variable being born.  Splitting the if-body's uses into their
 * own `a`/`b` shortens both ranges, lifts them above the pair locals, and lands
 * the hot values in r5/r6 with a single `str` per site.  18 differing to exact.
 *
 * Reading it the other way round is the cheap version of the check: if you find
 * yourself writing one local for a value the ROM loads twice, you have merged
 * two.  Live range, not value identity, is what picks the register.
 *
 * `0x202` is genuinely pooled and is NOT the pooled-small-constant tell -- it is
 * 0x101 << 1, and 0x101 does not fit a Thumb `mov` immediate, so no symbol is
 * called for.  `pop {r0} / bx r0` gives the `void` return.
 */
extern int __GetFlag(int id);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092b08(int a, int b);
extern void __WaitFrames(int n);

void OvlFunc_941_2008210(void)
{
    int a, b;
    int p, q;
    int n, m;
    int r;
    int p2, q2;

    if (__GetFlag(0x202)) {
        a = 0x15;
        b = 0x39;
        __Func_80105d4(0x29, 0x56, 2, 6, a, b);
        __WaitFrames(4);
        __Func_80105d4(0x2b, 0x56, 2, 6, a, b);
        __WaitFrames(4);
        b = 0x3a;
        __Func_80105d4(0x29, 0x56, 2, 6, a, b);
        __WaitFrames(4);
        __Func_80105d4(0x2b, 0x56, 2, 6, a, b);
        __WaitFrames(4);
    }
    p = 0x18;
    q = 0x3e;
    __Func_80105d4(2, 0x5d, 1, 1, p, q);
    n = 0x15;
    r = 0x37;
    __Func_80105d4(2, 0x5e, 1, 1, n, r);
    m = 0x3b;
    __Func_80105d4(0x29, 0x56, 2, 6, n, m);
    __WaitFrames(4);
    __Func_80105d4(1, 0x5d, 1, 1, p, q);
    __Func_80105d4(3, 0x5e, 1, 1, n, r);
    __Func_80105d4(0x2b, 0x56, 2, 6, n, m);
    __WaitFrames(4);
    __Func_8092b08(0xa, 3);
    p2 = 0x16;
    q2 = 0xf;
    __Func_8010704(0x13, 0x11, 1, 1, p2, q2);
}
