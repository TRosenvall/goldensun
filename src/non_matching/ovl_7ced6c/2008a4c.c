/* OvlFunc_946_2008a4c  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7ced6c/ovl_30_a_a_c_c_c.s
 * Best screen: 14 instructions in disagreeing regions, of 48 (streams same length).
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
 *  1. BASIC-BLOCK PLACEMENT -- SOLVED. Putting the body inside
 *     `if (p != 0) { ... return p; }` with `return 0;` after moves the null
 *     return to the end, where the ROM has it. That took this from 19 of 48 to
 *     14 and made the lengths match. It also elevated the whole 39-instruction
 *     sibling cluster outright -- see src/overlays/rom_7ced6c/ovl_30_a_a_c_c_c_b.c.
 *  2. REGISTER NAMING through the actor-flags block -- STILL OPEN. gcc puts the
 *     loaded pointer in r2 where the ROM uses r1, and the extra `orr` and the
 *     trailing read-modify-write give that renaming fourteen instructions to
 *     propagate through. This is what remains.
 *
 * The count is higher here than in the 39-instruction sibling (19 against 5)
 * only because the extra `orr` and the read-modify-write give the renaming more
 * instructions to propagate through; the two causes are the same.
 *
 * The 39-instruction cluster is now ELEVATED, so what was an eight-function
 * concentration is down to these four.
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
