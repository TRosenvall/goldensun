// fakematch
/* OvlFunc_881_200a81c  --  0x0200a81c
 *
 * From goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_c.s.
 *
 * Fourteenth and last member of the "arg_interleave_flat" class, and with it that class is
 * CLOSED -- all fourteen elevated, every one on its first screen. See
 * src/non_matching/overlays/arg_interleave_flat.c.
 *
 * The blocker throughout: the ROM writes one argument INSIDE another register's
 * two-instruction build, and gcc emits it before or after the whole block.
 * Pinning the argument registers and assigning them in the ROM's order places
 * it, because a pin names the hard register and so decides placement at the
 * assignment rather than through a dominating basic block.
 *
 * Three pin sites, one of them a single pin on r1 to place `mov r1, #0` between
 * the `.L679c` address load and the dereference that follows it. The actor id
 * is read THROUGH that label -- `ldr r3, =.L679c / ldr r0, [r3]` -- so the C
 * takes the address of the asm-renamed extern and dereferences it.
 */

extern int L679c __asm__(".L679c");
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int actor, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_881_200a81c(void)
{
    __CutsceneStart();
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0;
        q1 = 0;
        q0 = 0x37;
        __Func_809280c(q0, q1, q2);
    }
    __MessageID(0x2642);
    {
        register int q1 __asm__("r1");
        q1 = 0;
        __ActorMessage(*(int *)&L679c, q1);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xc0;
        q0 = 0x37;
        q1 <<= 6;
        q2 = 0;
        __Func_8092adc(q0, q1, q2);
    }
    __CutsceneEnd();
}
