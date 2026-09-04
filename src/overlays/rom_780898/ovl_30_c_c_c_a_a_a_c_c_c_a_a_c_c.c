// fakematch
/* OvlFunc_883_2008f8c  --  0x02008f8c
 *
 * From goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a_c.s.
 *
 * Member of the "arg_interleave_flat" class, marked resolved in
 * src/non_matching/overlays/arg_interleave_flat.c. The shape:
 *
 *     mov r1, #0xbb / mov r0, #0 / lsl r1, #1 / ldr r2, =0x4d6
 *
 * `mov r0` sits inside r1's two-instruction build. Pinning the argument
 * registers and assigning them in the ROM's order places it; matched on the
 * first screen, as every member of this class has.
 *
 * The class note's reasoning was the basic-block lever, which needs a block
 * dominating the call, and these functions have no branch at all. A pin does
 * not go through basic blocks -- it names the hard register, so the placement
 * is decided at the assignment.
 */

extern unsigned char L7586[] __asm__(".L7586");
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_883_2008f8c(void)
{
    __PlaySound(0x9e);
    __Func_8010560(L7586, 0x34, 0x4c);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xbb;
        q0 = 0;
        q1 <<= 1;
        q2 = 0x4d6;
        __Func_809218c(q0, q1, q2);
    }
    __Func_8091e9c(9);
}
