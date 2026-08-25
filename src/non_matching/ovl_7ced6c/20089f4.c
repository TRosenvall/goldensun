/* OvlFunc_946_20089f4  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7ced6c/ovl_30_a_a_c_c_c.s
 * Best screen: 5 instructions in disagreeing regions, of 41 (rom 41, ours 40).
 *
 * THIS PARK COVERS FOUR FUNCTIONS -- operand-identical copies, verified
 * instruction by instruction including operands:
 *
 *      OvlFunc_927_20089f4   ovl_7b4558/ovl_30_a_a_c_c_c_c.s
 *      OvlFunc_946_20089f4   ovl_7ced6c/ovl_30_a_a_c_c_c.s
 *      OvlFunc_964_20089f4   ovl_7ed0a0/ovl_30_a_a_a_c_c_c_c.s
 *      OvlFunc_965_20089f4   ovl_7ef4f4/ovl_30_a_a_a_c_c_c_c.s
 *
 * TWO BLOCKERS, both already named elsewhere:
 *
 *  1. BASIC-BLOCK PLACEMENT. The ROM emits the null return's `mov r0, #0` at
 *     the END, after the success path, and reaches it by falling out of the
 *     guard; gcc emits it immediately at the guard. Same as
 *     src/non_matching/ovl_7cb2c0/20080fc.c, where writing the ROM's block
 *     order out with gotos was byte-identical.
 *
 *  2. A REDUNDANT ARGUMENT COPY. The ROM calls __Actor_SetSpriteFlags with r0
 *     still holding the __CreateActor result -- it sets only r1 -- because r0
 *     has not been written since the call returned. gcc copies the value back
 *     from the callee-saved register it parked it in (`mov r0, r5`). The value
 *     must live in a callee-saved register either way, since it survives three
 *     calls; the difference is that gcc does not track that r0 still holds it.
 *     There is no source-level handle: the argument has to be named, and naming
 *     it is what produces the copy.
 *
 * The argument shuffle at the top IS reproduced correctly and should not be
 * re-derived: the wrapper takes (a, b, c, d) and calls __CreateActor(d, a, b, c),
 * which is what generates the four-register rotation through r4/r5/r6.
 *
 * NOT SCREENED, and recorded as such rather than assumed: the neighbouring
 * 4-copy cluster at 46 instructions (OvlFunc_92x_2008a4c) is the same wrapper
 * with an extra `orr` and a trailing read-modify-write on byte +0x23. It has
 * the identical guard shape and the identical post-call argument pattern, so it
 * is expected to hit both blockers above, but that expectation has not been
 * tested.
 */
extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(void *a, int n);
extern void __Func_80929d8(void *a, int n);
extern void __Func_800c548(void *a, int n);

void *OvlFunc_946_20089f4(int a, int b, int c, int d)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *r;
    int n;
    int v;
    int z;
    int w;

    p = __CreateActor(d, a, b, c);
    if (p == 0)
        return 0;
    q = *(unsigned char **)(p + 0x50);
    n = 0xd;
    v = q[9];
    n = -n;
    n &= v;
    r = p;
    q[9] = n;
    r += 0x55;
    z = 0;
    *r = z;
    r += 4;
    w = 8;
    *r = w;
    __Actor_SetSpriteFlags(p, 0);
    __Func_80929d8(p, 0xe);
    __Func_800c548(p, 1);
    return p;
}
