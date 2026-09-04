// fakematch
/* OvlFunc_954_2008158  --  0x02008158
 *
 * From goldensun/asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_a_c_a.s, which held
 * this function alone, so no split was needed.
 *
 * Member of shape group 2 (flat, {bl, bx, ldr, lsl, mov, pop, push, str}), and
 * the smallest of the three at ten instructions.
 *
 * A STORE INTERLEAVED INTO ARGUMENT SETUP, which is the shape recorded in
 * src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c_a_b.c:
 *
 *     ldr r3, =.L441c / mov r2, #0x42 / mov r1, #0xc8 / str r2, [r3] / lsl r1, #4
 *
 * The stored value and the shifted argument are built BEFORE the store, and the
 * store sits between them and the shift. Pinning r1 and r2 and writing the
 * store between the assignments reproduces it. r0 takes the task's address from
 * the pool and needs no pin.
 *
 * The `.L441c` slot is reached with the tree's asm-renamed extern idiom, since
 * C cannot spell a name beginning with a dot.
 */

extern int L441c __asm__(".L441c");
extern void __StartTask(void *fn, int arg);
extern void OvlFunc_954_200804c(void);

void OvlFunc_954_2008158(void)
{
    register int q1 __asm__("r1");
    register int q2 __asm__("r2");

    q2 = 0x42;
    q1 = 0xc8;
    L441c = q2;
    q1 <<= 4;
    __StartTask(OvlFunc_954_200804c, q1);
}
