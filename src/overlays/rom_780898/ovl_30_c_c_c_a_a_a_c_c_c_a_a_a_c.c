// fakematch
/* OvlFunc_883_2008e84  --  0x02008e84
 *
 * From goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_a_a_c.s.
 *
 * Third member of the "arg_interleave_flat" class elevated this batch, and the
 * third to match on the first screen. See
 * src/non_matching/overlays/arg_interleave_flat.c, now marked resolved.
 *
 *     rom   mov r1, #0x83 / mov r0, #0 / lsl r1, #1 / ldr r2, =0x325
 *
 * `mov r0` sits inside r1's two-instruction build. Pinning the argument
 * registers and assigning them in the ROM's order places it.
 *
 * These three members differ only in their constants and their `.L` data label;
 * the body shape is identical. That is what the class note was right about --
 * the C is trivial and the whole difficulty was one `mov`'s position.
 */

extern unsigned char L7570[] __asm__(".L7570");
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_883_2008e84(void)
{
    __PlaySound(0x9e);
    __Func_8010560(L7570, 0x2d, 0x27);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x83;
        q0 = 0;
        q1 <<= 1;
        q2 = 0x325;
        __Func_809218c(q0, q1, q2);
    }
    __Func_8091e9c(6);
}
