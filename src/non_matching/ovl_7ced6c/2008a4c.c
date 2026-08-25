/* OvlFunc_946_2008a4c  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7ced6c/ovl_30_a_a_c_c_c.s
 * Best screen: 19 instructions in disagreeing regions, of 48 (rom 48, ours 47).
 *
 * THIS PARK COVERS FOUR FUNCTIONS -- operand-identical copies:
 *
 *      OvlFunc_927_2008a4c   ovl_7b4558/ovl_30_a_a_c_c_c_c.s
 *      OvlFunc_946_2008a4c   ovl_7ced6c/ovl_30_a_a_c_c_c.s
 *      OvlFunc_964_2008a4c   ovl_7ed0a0/ovl_30_a_a_a_c_c_c_c.s
 *      OvlFunc_965_2008a4c   ovl_7ef4f4/ovl_30_a_a_a_c_c_c_c.s
 *
 * The same __CreateActor wrapper as src/non_matching/ovl_7ced6c/20089f4.c with
 * an extra `orr #4` and a trailing read-modify-write on byte +0x23. That park
 * predicted this one would hit the same two blockers; SCREENING CONFIRMS IT,
 * which is why this file says "confirmed" and not "presumably".
 *
 *  1. BASIC-BLOCK PLACEMENT -- the null return's `mov r0, #0` is emitted at the
 *     guard rather than at the end.
 *  2. REGISTER NAMING through the actor-flags block, which follows from gcc
 *     putting the loaded pointer in r2 where the ROM uses r1.
 *
 * The count is higher here than in the 39-instruction sibling (19 against 5)
 * only because the extra `orr` and the read-modify-write give the renaming more
 * instructions to propagate through; the two causes are the same.
 *
 * TAKEN TOGETHER with the 39-instruction cluster, EIGHT functions are blocked by
 * these two behaviours. That is worth recording next to the seven-function
 * scheduling park (rom_8a000/rom_9a44c.c) as the second-largest concentration
 * of blocked work in the corpus.
 *
 * The constant-as-destination spellings are right and should not be re-derived:
 * `n = 0xd; n = -n; n &= v;` gives `mov r3, #0xd / neg r3, r3 / and r3, r2`,
 * and `t = 0xfe; t &= v;` gives `mov r3, #0xfe / and r3, r2`.
 */
extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(void *a, int n);
extern void __Func_80929d8(void *a, int n);

void *OvlFunc_946_2008a4c(int a, int b, int c, int d)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *r;
    unsigned char *s;
    int n;
    int v;
    int m;
    int z;
    int w;
    int t;

    p = __CreateActor(d, a, b, c);
    if (p == 0)
        return 0;
    q = *(unsigned char **)(p + 0x50);
    n = 0xd;
    v = q[9];
    n = -n;
    n &= v;
    m = 4;
    n |= m;
    r = p;
    q[9] = n;
    r += 0x55;
    z = 0;
    *r = z;
    r += 4;
    w = 8;
    *r = w;
    __Actor_SetSpriteFlags(p, 0);
    __Func_80929d8(p, 0xf);
    s = p;
    s += 0x23;
    v = *s;
    t = 0xfe;
    t &= v;
    m = 2;
    t |= m;
    *s = t;
    return p;
}
