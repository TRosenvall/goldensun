/*
 * OvlFunc_883_200d64c -- asm/overlays/rom_780898/ovl_30_c_c_c_a_c_c_c_c_c_c_c_c.s
 *
 * BLOCKER: allocation priority -- the parameter one register too high.
 * 113 lines against 112, and the whole 62 differing is ONE ROTATION: the ROM
 * puts the fourth parameter in r6 and we put it in r8, which costs three extra
 * `mov rN, r8` copies and lets gcc cross-jump the final store.
 *
 * READ OUT OF THE COMPILER, which is the useful part. `-dg` gives
 *   12 regs to allocate: 39 41 42 32 56 57 35 86 44 33 34 36
 *   dispositions: 32 in 5, 33 in 11, 35 in 8, 44 in 9, 56 in 6, 57 in 7, 86 in 10
 * with 34 and 36 spilled. Cross-referencing the .lreg RTL, our priority order is
 * a > b+8 > a+8 > d > f62 > f5b > b; the ROM's is a > d > b+8 > a+8 > f5b > f62
 * > b. global.c's allocno_compare is
 * floor_log2(n_refs) * n_refs / live_length * 10000 * size, and by that formula
 * the parameter -- five references spread over the whole function -- genuinely
 * loses to two pointers with three references over about forty instructions.
 * Raising it needs three more references or a much shorter live range, and
 * neither is available without changing the semantics.
 *
 * TRIED AND REJECTED, all measured against the 112-line reference: named pointer
 * locals at the call site, either order (62, identical to baseline); the same at
 * the top of the function (110 lines/82 and 111/92 -- this DOES put the
 * parameter in r6, but drops both pointers below three other values); only one
 * of them named (91, 92); an `int` copy of the parameter at the top (112/85 --
 * lands in r9, still high); named field pointers (109/92); the failure block
 * duplicated as if/else (112/96) and with a goto join (105/105); a nested first
 * guard (62); `!d` for `d == 0` (62); reordering the compound condition
 * (105/105); the zero initialisation moved after the guard (84); five orderings
 * of the three masked values (62-88); a full struct typing of both actors -- the
 * lever that closed OvlFunc_964_2008cd0 -- (62, unchanged); and eight scheduler
 * and optimisation flags (82-104).
 *
 * SETTLED: naming all three masked angle values before the first compare is a
 * real lever here, 82 differing to 62, because the ROM evaluates all three
 * before it branches.
 */
extern int OvlFunc_883_200d610(int *p, int *q);
extern int __atan2(int y, int x);
extern void __Actor_SetAnim(unsigned char *a, int n);

int OvlFunc_883_200d64c(unsigned char *a, unsigned char *b, int c, int d)
{
    int r;
    int t;
    int m;
    int h;
    int v0;
    int v1;
    int v2;

    r = 0;
    if (a[0x5b] == 1 && a[0x62] == 0) {
        __Actor_SetAnim(a, 1);
        return 1;
    }
    if (OvlFunc_883_200d610((int *)(b + 8), (int *)(a + 8)) >= c && d == 0)
        goto lose;
    t = (unsigned short)__atan2(*(int *)(b + 0x10) - *(int *)(a + 0x10),
                                *(int *)(b + 8) - *(int *)(a + 8));
    m = 0xf0 << 8;
    v2 = (t + 0xfffff000) & m;
    v1 = (t + (0x80 << 5)) & m;
    v0 = t & m;
    h = m & *(unsigned short *)(a + 6);
    if (v0 != h && v1 != h && v2 != h && d == 0)
        goto lose;
    a[0x5b] = 1;
    __Actor_SetAnim(a, 1);
    r = 1;
    a[0x62] = 1;
    goto done;
lose:
    a[0x5b] = d;
    __Actor_SetAnim(a, 2);
    a[0x62] = d;
done:
    return r;
}
