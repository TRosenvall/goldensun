/*
 * OvlFunc_901_2008350 -- asm/overlays/rom_797990/ovl_314_a_a_c_a.s
 *
 * BLOCKER: register pressure. 88 lines against 86, and the allocation of the
 * four long-lived values differs from the first callee-saved move onward.
 *
 * The ROM keeps FOUR pointers live across the whole function: the two record
 * bases (r6, r8) and the two `+8` sub-pointers derived from them (r5, r7). We
 * keep the bases and derive one of the offsets, so `a + 8` lands in r8 where
 * the ROM has the record itself.
 *
 * The structure is believed right and is not the problem: the guard is
 * `if (OvlFunc_901_2008314(pb, pa, 0) >= c && d == 0)`, the angle is
 * `(unsigned short)__atan2(b[0x10] - a[0x10], *pb - *pa)`, and the three-way
 * comparison tests the angle, the angle + 0x1000 and the angle - 0x1000 each
 * masked with 0xf000 against the same masked field, in that order, with
 * `|| d != 0` last.
 *
 * NOT YET TRIED: naming the two `+8` pointers AND the two bases as four
 * separate locals assigned in the ROM's birth order (b, b+8, a+8, then the
 * arguments). The current spelling derives the sub-pointers from the bases in
 * one expression each, which is what leaves gcc free to drop a base.
 */
extern int OvlFunc_901_2008314(int *p, int *q, int n);
extern int __atan2(int y, int x);
extern void __Actor_SetAnim(unsigned char *a, int n);

int OvlFunc_901_2008350(unsigned char *a, unsigned char *b, int c, int d)
{
    int *pb;
    int *pa;
    int r;
    int t;
    int m;
    int h;

    pb = (int *)(b + 8);
    pa = (int *)(a + 8);
    r = 0;
    if (OvlFunc_901_2008314(pb, pa, 0) >= c && d == 0) {
        a[0x5b] = r;
        __Actor_SetAnim(a, 2);
    } else {
        t = (unsigned short)__atan2(*(int *)(b + 0x10) - *(int *)(a + 0x10), *pb - *pa);
        m = 0xf0 << 8;
        h = *(unsigned short *)(a + 6) & m;
        if ((t & m) == h || ((t + (0x80 << 5)) & m) == h
            || ((t + 0xfffff000) & m) == h || d != 0) {
            a[0x5b] = 1;
            __Actor_SetAnim(a, 1);
            r = 1;
        }
    }
    return r;
}
