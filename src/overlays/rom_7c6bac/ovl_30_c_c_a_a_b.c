// fakematch
/* OvlFunc_942_2008144  --  0x02008144
 *
 * From goldensun/asm/overlays/rom_7c6bac/ovl_30_c_c_a_a_b.s, which held this function alone.
 *
 * Eleventh member of the "arg_interleave_flat" class, and with it that class is
 * CLOSED -- all fourteen elevated, every one on its first screen. See
 * src/non_matching/overlays/arg_interleave_flat.c.
 *
 * The blocker throughout: the ROM writes one argument INSIDE another register's
 * two-instruction build, and gcc emits it before or after the whole block.
 * Pinning the argument registers and assigning them in the ROM's order places
 * it, because a pin names the hard register and so decides placement at the
 * assignment rather than through a dominating basic block.
 *
 * THE ONLY MEMBER THAT NEEDED MORE THAN ONE KIND OF FIX. Two sites are the
 * ordinary interleave. The third is a `precompute_register_parameters` bind of
 * the sort closed in src/overlays/rom_7aa430/ovl_e90_c_c_a_a_c_c.c: both
 * arguments of __MapActor_SetSpeed are pool loads, so both are precomputed and
 * the cheap `mov r0, #8` lands last where the ROM puts it first. Pinning r0
 * alone fixed that and left the two pool loads themselves transposed, 3 of 30
 * to 2; pinning all three in the ROM's order closed it. Two independent pool
 * LOADS order the same way two independent movs do -- see
 * src/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_a_c.c, where the same
 * thing was measured on a three-register fill.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __SetFlag(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_942_2008144(void)
{
    __CutsceneStart();
    __SetFlag(0x8aa);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xc4;
        q2 = 0x94;
        q0 = 0;
        q1 <<= 1;
        q2 <<= 1;
        __Func_80921c4(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q0 = 8;
        q1 = 0x13333;
        q2 = 0x9999;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xcc;
        q2 = 0x94;
        q0 = 8;
        q1 <<= 1;
        q2 <<= 1;
        __Func_80921c4(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q1 <<= 8;
        q2 = 0;
        q0 = 8;
        __Func_8092adc(q0, q1, q2);
    }
    __CutsceneWait(0x14);
    __CutsceneEnd();
}
