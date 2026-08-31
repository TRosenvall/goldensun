/* OvlFunc_924_20096c4 -- 0x020096c4  (asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c.s)
 *
 * BLOCKER: stack-argument register allocation INSIDE A LOOP. 13 of 92, exact
 * length. The interleave this function was selected for is NOT the problem --
 * it reproduced.
 *
 * That is the useful part. tools/guarded_interleave.py picked this up for its
 * three guarded interleave sites, and naming the constants in the entry block
 * reproduced all three: both __Func_8012330 calls -- the `0xa0 << 11` triple and
 * the `mov`/`neg` pair with the pooled 0xe666 -- are absent from the diff. The
 * lever did its job; what stopped the function is a different class that happens
 * to live in the same body.
 *
 * WHAT REMAINS: three call sites where the ROM materialises BOTH stack arguments
 * into two registers before storing either, and we reuse one register for both.
 *
 *     rom   mov r3, #0x1 / mov r1, r10 / str r3, [sp] / str r1, [sp,#4]
 *     ours  mov r3, #0x1 / str r3, [sp] / mov r3, r10 / str r3, [sp,#4]
 *
 * The documented remedy is a named local per stack argument per call site, and
 * it works OUTSIDE the loop and backfires INSIDE it:
 *
 *     16  naive
 *     13  name both stack arguments of the post-loop __Func_8010704   <- best
 *     13  the above plus a local for the in-loop `1`   (inert)
 *     94  the above plus a local for the in-loop `4`   (97 lines, +5)
 *     94  all four sites named at once                 (97 lines, +5)
 *
 * THE BOUNDARY IS REGISTER PRESSURE, not the lever. This loop already keeps six
 * values live across its body -- the parameter, a counter, two induction
 * variables and two loop-invariant constants -- and the ROM spends r8, r9 and
 * r10 to hold them. Adding one more named local is free; adding a second forces
 * a spill and costs five lines. So the per-call-site rule needs a proviso: it
 * buys two registers at the call and it must be paid for out of what the
 * surrounding block has spare. In a straight-line run that is nearly always
 * affordable, and inside a loop that is already spending high registers it
 * usually is not.
 *
 * The remaining six lines beyond those two sites are the loop-setup order --
 * gcc materialises the four loop constants through one scratch register where
 * the ROM alternates two. Three assignment orders were screened (six/two/x/y,
 * six/x/y/two, x/six/two/y) at 13, 14 and 14, so source order does not reach it
 * either; that is the register-role swap in its usual dressing.
 *
 * Everything else is right: the unsigned `bls` loop over 0..2, the three
 * __CopyMapTiles calls with their computed arguments (3 - i, i + 0x6a, and the
 * two induction variables stepping by 2), the guard on the parameter, and the
 * post-loop 0x90 << 1 sound.
 */
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);

void OvlFunc_924_20096c4(int arg)
{
    int s1;
    int s2;
    int s3;
    int m1;
    int m2;
    int e;
    int six;
    int two;
    unsigned int i;
    int x;
    int y;
    int n3;
    int n4;

    s1 = 0xa0 << 11;
    s2 = 0xa0 << 11;
    s3 = 0x80 << 9;
    m1 = -1;
    m2 = -1;
    e = 0xe666;
    __PlaySound(0xdb);
    six = 6;
    x = 0x29;
    two = 2;
    y = 0x28;
    for (i = 0; i <= 2; i++) {
        __CopyMapTiles(y, 0x20, x, 0x20, 3 - i, six);
        __CopyMapTiles(0x27, 0x33, y, 0x20, 1, six);
        __CopyMapTiles(0x69, 0x33, i + 0x6a, 0x20, two, 4);
        if (arg != 0) {
            __Func_8012330(s1, s2, s3);
            __Func_8012330(m1, m2, e);
            __CutsceneWait(arg);
        }
        x += 2;
        y += 2;
    }
    __PlaySound(0x90 << 1);
    n3 = 0x2a;
    n4 = 0x21;
    __Func_8010704(0x6a, 0x21, 4, 5, n3, n4);
    __Func_8012350();
}
