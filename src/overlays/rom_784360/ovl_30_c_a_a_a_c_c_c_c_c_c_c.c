// fakematch
/* OvlFunc_884_20088ac  --  0x020088ac
 *
 * From goldensun/asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_c_c_c_c_c.s, which held this function alone.
 *
 * Member of the "arg_interleave_flat" class, marked resolved in
 * src/non_matching/overlays/arg_interleave_flat.c. The ROM writes `mov r0`
 * INSIDE r1's two-instruction build:
 *
 *     mov r1, #0xb3 / mov r0, #0 / lsl r1, #1 / ldr r2, =0x29e
 *
 * Pinning the argument registers and assigning them in the ROM's order places
 * it. Matched on the first screen, as every member of this class has.
 *
 * Its twin OvlFunc_884_200881c in the neighbouring file shares the same body
 * and the same `.L3eb4` table, differing only in three constants.
 */

extern unsigned char L3eb4[] __asm__(".L3eb4");
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void OvlFunc_884_2008714(int n);

void OvlFunc_884_20088ac(void)
{
    __PlaySound(0x9e);
    __Func_8010560(L3eb4, 0x33, 0x27);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xb3;
        q0 = 0;
        q1 <<= 1;
        q2 = 0x29e;
        __Func_809218c(q0, q1, q2);
    }
    OvlFunc_884_2008714(7);
}
