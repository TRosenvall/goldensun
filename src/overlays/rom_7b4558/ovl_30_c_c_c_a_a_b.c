/* OvlFunc_927_2009b84 -- MATCHES on the default flags (and unchanged under
 * -fno-rerun-cse-after-loop).  ref: asm/overlays/rom_7b4558/ovl_30_c_c_c_a_a.s
 * tryc.py: OK (76 lines).
 *
 * One lever: OvlFunc_927_2008244 declared to return `int`, which emits r0 LAST
 * in the argument block at four of the five call sites.  Declared `void` the
 * function is 8 of 76 and every difference is the position of `mov r0,#0x2`.
 * `one` and `zero` are named locals because both survive five calls in
 * callee-saved registers (r6 and r8) and are re-stored to the stack slot at
 * each one.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern int OvlFunc_927_2008244(int a, int b, int c, int d, int e, int f);

void OvlFunc_927_2009b84(void)
{
    unsigned char *a;
    unsigned char *p;
    int x;
    int z;
    int t;
    int one;
    int zero;

    __CutsceneStart();
    x = *(int *)(__MapActor_GetActor(0xd) + 8);
    t = *(int *)(__MapActor_GetActor(0xd) + 0x10);
    x >>= 20;
    z = t >> 20;
    one = 1;
    OvlFunc_927_2008244(2, x, z, 1, one, 0xff);
    zero = 0;
    OvlFunc_927_2008244(2, x + 1, z, 1, one, zero);
    OvlFunc_927_2008244(2, x - 1, z, 1, one, zero);
    OvlFunc_927_2008244(2, x, z + 1, 1, one, zero);
    OvlFunc_927_2008244(2, x, z - 1, 1, one, zero);
    if (x == 0x2d && z == 6) {
        a = __MapActor_GetActor(0xd);
        p = a + 0x55;
        *p = zero;
        *(int *)(a + 0x14) = 0xfffe0000;
        *(int *)(a + 0xc) = 0xfffe0000;
    }
    __CutsceneEnd();
}
