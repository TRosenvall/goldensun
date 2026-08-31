/* OvlFunc_926_2008e94 -- 0x02008e94  (asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_a.s)
 *
 * BLOCKER: register roles and argument scheduling across four eight-argument
 * calls, plus an UNRESOLVED aliasing question. 65 of 101 with
 * -fno-strict-aliasing, 82 without.
 *
 * THE LENGTH IS WRONG IN BOTH DIRECTIONS, which is the useful observation:
 *
 *     without -fno-strict-aliasing   99 lines against 101   (two reloads short)
 *     with it                       102 lines against 101   (one reload too many)
 *
 * So this function is not a clean member of the aliasing class. Some of its
 * re-reads survive in the ROM and some do not, which means the original source
 * distinguishes them -- presumably by accessing some fields through a type that
 * can alias the halfword store at [r3, #0x1e] and others through one that
 * cannot. A whole-TU flag cannot express that, and neither can the single
 * `int *` view used below. Worth returning to only with a struct definition
 * that gives the fields real types; docs/structs.md is the place to start.
 *
 * WHAT IS ALREADY RIGHT: with the flag on, the four-iteration animation loop is
 * nearly instruction-exact -- the countdown `fr` from 8 by 2, the unsigned
 * `bls` guard, the in-place `a[4] +=`, and the halfword clear after it. The
 * differences there are register names only (the ROM uses r4 for the loop's
 * temporary constant where we use r2) and one transposition of the `mov r2, r8`
 * against the store it precedes.
 *
 * MEASURED: assigning the carried 0x80 << 24 LAST among the entry
 * assignments -- matching the ROM's `mov r7,r0 / mov r6,#0 / mov r5,#8 /
 * mov r8,r2` order -- is worth 2 differing in each configuration (84 to 82,
 * 68 to 65) and moves the first divergence from 7 to 17. Small, but it is the
 * assignment-position lever behaving as documented.
 *
 * NOT TRIED, and the obvious next step: typed struct access. Everything here
 * goes through `int *a` with hand-computed indices, which is what makes the
 * aliasing behaviour all-or-nothing.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void OvlFunc_common0_10c(int a, int b, int c, int d,
                                int e, int f, int g, int h);

void OvlFunc_926_2008e94(void)
{
    int *a;
    unsigned int i;
    int fr;
    int z;
    int big;
    int d1;
    int d2;
    int s;
    int q;

    a = (int *)__MapActor_GetActor(0x13);
    i = 0;
    fr = 8;
    big = 0x80 << 24;
    do {
        __WaitFrames(fr);
        a[4] += 0x80 << 9;
        a[0x10] = big;
        i++;
        fr -= 2;
    } while (i <= 3);
    z = 0;
    *(unsigned short *)(*(char **)(a + 0x14) + 0x1e) = z;
    a[4] += 0xc0 << 13;
    a[0x10] = 0x80 << 24;
    __PlaySound(0xe3);
    d1 = 0xc0 << 12;
    s = 0x3333;
    OvlFunc_common0_10c(a[2], a[3], a[4] + d1, 0xffff3334, z, s, z, z);
    OvlFunc_common0_10c(a[2], a[3], a[4] + d1, 0xcccc, z, s, z, z);
    d2 = 0xfff80000;
    q = 0x80 << 9;
    OvlFunc_common0_10c(a[2] + 0xfffa0000, a[3], a[4] + d2, s, z, q, z, z);
    OvlFunc_common0_10c(a[2] + (0xc0 << 11), a[3], a[4] + d2, s, z, q, z, z);
}
