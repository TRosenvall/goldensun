/* Cluster OvlFunc_918_2008f58..OvlFunc_918_2008f58 extracted from
 * goldensun/asm/overlays/rom_7a5214/ovl_314_c_c_a.s.
 *
 * Six six-argument calls, and it matched on the first screen because the
 * batch-149/150 stack-argument reading answers every one of them off the `str`
 * operands alone.  No lever was needed; the assembly simply says which values
 * get names.
 *
 *   n6 = 0xa   -- the ROM holds it in r6 and stores it from there at three of
 *                 the six sites (once as the sixth argument, twice as the
 *                 fifth).  A callee-saved register spent on a constant is the
 *                 ROM asking for a shared local.
 *   n5 = 0x1e  -- same, in r5, at two sites.
 *   everything else in the taken branch -- 0x5d, 0x2b, 9, 8, 0x28 -- is
 *                 materialised fresh into r3 immediately before its store, so
 *                 those stay literals.
 *   p, q       -- the ELSE arm's pair (0xa, 8) is built into two separate
 *                 registers before either store, so it gets its own pair of
 *                 locals and is deliberately NOT shared with n6, even though
 *                 the value 0xa is the same.  Sharing across sites is the
 *                 batch-144 failure.
 *
 * The assignments sit where the ROM materialises them -- `n6` inside the first
 * call's setup, `n5` inside the second's -- which is what keeps each in the
 * register the ROM uses rather than promoting it earlier.
 *
 * The reference keeps its literal pool inside the function, so tryc.py cannot
 * see PC-relative distance; this was verified with make compare.
 */
extern int __GetFlag(int id);
extern void OvlFunc_918_2008918(void);
extern void __WaitFrames(int n);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_918_2008f58(int a)
{
    int n6;
    int n5;
    int p;
    int q;

    if (a != 0 && __GetFlag(0x109) == 0)
        OvlFunc_918_2008918();
    __WaitFrames(1);
    if (__GetFlag(0x844)) {
        n6 = 0xa;
        __Func_80105d4(0x79, 0x22, 3, 1, 0x5d, n6);
        n5 = 0x1e;
        __Func_80105d4(0x2e, 0x26, 1, 1, n5, 0x2b);
        __Func_8010704(0, 0, 1, 2, n5, 9);
        __Func_8010704(0x1a, 3, 1, 2, n6, 8);
        __Func_80105d4(0x1a, 0x23, 1, 4, n6, 0x28);
    } else {
        p = 0xa;
        q = 8;
        __Func_8010704(0xb, 8, 1, 2, p, q);
    }
}
