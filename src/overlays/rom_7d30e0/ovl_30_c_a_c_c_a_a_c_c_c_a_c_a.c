// fakematch
/* OvlFunc_948_20091d8  --  0x020091d8
 *
 * From goldensun/asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_c_a.s, which
 * held this function alone, so no split was needed.
 *
 * PARKED AT 8 OF 19 AGAINST THE ROM'S 20 -- one line SHORT, which is the tell.
 *
 *     rom   mov r1, #0x80 / mov r2, #0x80 / mov r0, #0xc / lsl r1, #12 / lsl r2, #12
 *     ours  mov r2, #0x80 / lsl r2, #12   / mov r0, #0xc / mov r1, r2
 *
 * __MapActor_SetPos is called with THE SAME VALUE for both x and y. The ROM
 * builds it twice; gcc builds it once and copies the register, which is one
 * instruction cheaper -- so the function comes out shorter than the ROM rather
 * than longer, and the count of differing lines understates how little is
 * wrong.
 *
 * Pinning r1 and r2 forces both builds: they are call-clobbered, so neither
 * can be served by a copy of the other across the call boundary, and the ROM's
 * own order places `mov r0` between the two movs and the two shifts.
 *
 * This is the rematerialisation lever meeting the interleave in one call. The
 * park named the cause exactly -- "CONSTANT CSE, NO BOUNDARY" -- and reasoned
 * from the basic-block lever, which needs a boundary this straight-line
 * function does not have. A pin needs no boundary.
 */

extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_948_20091d8(void)
{
    int s1, s2;

    s1 = 0x19;
    s2 = 0x30;
    __Func_80105d4(0x18, 0x30, 1, 2, s1, s2);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q2 = 0x80;
        q0 = 0xc;
        q1 <<= 12;
        q2 <<= 12;
        __MapActor_SetPos(q0, q1, q2);
    }
}
