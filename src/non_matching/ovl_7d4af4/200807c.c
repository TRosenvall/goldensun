/*
 * OvlFunc_949_200807c -- asm/overlays/rom_7d4af4/ovl_30_a_a_c_c_a.s
 *
 * BLOCKER: allocation priority -- THIRD instance of the facing-test family.
 * 115 lines against 118.
 *
 * The ROM keeps the fourth parameter in r11 and the third in r5; we place them
 * differently, and three instructions are missing as a consequence. This is the
 * same shape as OvlFunc_883_200d64c (62 of 112, parameter one register too
 * high) and OvlFunc_901_2008350 (88 against 86, four pointers live where we
 * keep three). All three are the two-entity masked-angle facing test.
 *
 * OvlFunc_883_200d64c's park carries the compiler's own allocation dump for
 * this family, read with -dg, and global.c's allocno_compare formula explains
 * the priority order directly: a parameter with few references spread over the
 * whole function loses to pointers with more references over a short range.
 * That is why statement and declaration reordering does not reach it.
 *
 * TRIED here, all measured: declaration order of the two +8 pointers swapped
 * (107 differing, unchanged); assignment order swapped (109, worse); the result
 * variable initialised before both pointers (114 lines, 100 differing -- the
 * best, and still three short).
 *
 * SETTLED: the five masked angle candidates must each be named and masked as
 * separate statements before the first compare, in the ROM's `and` order, which
 * is the lever that closed OvlFunc_898_2009674 in the same family. That part is
 * right here; the residue is entirely allocation.
 *
 * Re-attack this one together with 883_200d64c and 901_2008350, and only when
 * there is a lever that moves allocation priority rather than statement order.
 */
extern int OvlFunc_949_2008040(int *p, int *q, int n);
extern int __atan2(int y, int x);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern unsigned char *__MapActor_GetActor(int slot);

int OvlFunc_949_200807c(unsigned char *a, unsigned char *b, int c, int d)
{
    int *pa;
    int *pb;
    int r;
    int t;
    int m;
    int h;
    int far;
    int base;
    int p2;
    int m1;
    int p1;

    r = 0;
    pb = (int *)(b + 8);
    pa = (int *)(a + 8);
    if (OvlFunc_949_2008040(pb, pa, 0) >= c && d == 0) {
        a[0x5b] = r;
        __Actor_SetAnim(a, 2);
    } else {
        t = (unsigned short)__atan2(*(int *)(b + 0x10) - *(int *)(a + 0x10), *pb - *pa);
        m = 0xf0 << 8;
        far = (t + 0xffffe000) & m;
        p2 = t + (0x80 << 6);
        m1 = t + 0xfffff000;
        p1 = t + (0x80 << 5);
        h = *(unsigned short *)(a + 6);
        base = t & m;
        h = h & m;
        p2 = p2 & m;
        m1 = m1 & m;
        p1 = p1 & m;
        if (base == h || p1 == h || m1 == h || d != 0) {
            a[0x5b] = 1;
            __Actor_SetAnim(a, 1);
            r = 1;
        }
        if (b == __MapActor_GetActor(0) && (p2 == h || far == h)) {
            a[0x5b] = 1;
            __Actor_SetAnim(a, 1);
            r = 1;
        }
    }
    return r;
}
