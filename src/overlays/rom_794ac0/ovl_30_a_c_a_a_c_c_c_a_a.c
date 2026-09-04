// fakematch
/* OvlFunc_899_2008428  --  0x02008428
 *
 * From goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_a_a_c_c_c_a_a.s, which held this function alone.
 *
 * Tenth member of the "arg_interleave_flat" class, and with it that class is
 * CLOSED -- all fourteen elevated, every one on its first screen. See
 * src/non_matching/overlays/arg_interleave_flat.c.
 *
 * The blocker throughout: the ROM writes one argument INSIDE another register's
 * two-instruction build, and gcc emits it before or after the whole block.
 * Pinning the argument registers and assigning them in the ROM's order places
 * it, because a pin names the hard register and so decides placement at the
 * assignment rather than through a dominating basic block.
 *
 * One pin site: `mov r1, #0x80 / mov r0, #0xf / lsl r1, #8 / mov r2, #0`.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void OvlFunc_899_20083bc(int n);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_899_2008428(void)
{
    __CutsceneStart();
    __MessageID(0x1253);
    OvlFunc_899_20083bc(0xf);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q0 = 0xf;
        q1 <<= 8;
        q2 = 0;
        __Func_8092adc(q0, q1, q2);
    }
    __CutsceneEnd();
}
