// fakematch
/* OvlFunc_944_2008468  --  0x02008468
 *
 * From goldensun/asm/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_a_b.s, which held this function alone.
 *
 * Twelfth member of the "arg_interleave_flat" class, and with it that class is
 * CLOSED -- all fourteen elevated, every one on its first screen. See
 * src/non_matching/overlays/arg_interleave_flat.c.
 *
 * The blocker throughout: the ROM writes one argument INSIDE another register's
 * two-instruction build, and gcc emits it before or after the whole block.
 * Pinning the argument registers and assigning them in the ROM's order places
 * it, because a pin names the hard register and so decides placement at the
 * assignment rather than through a dominating basic block.
 *
 * The interleaved argument here is a POOL LOAD, `ldr r2, =0x1410000`, sitting
 * between `mov r1, #0xa4` and its shift. Same fix; the class's shape does not
 * care whether the neighbouring argument is a mov or a load.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_8092950(int a, int b);
extern void __Func_800fe9c(void);
extern void OvlFunc_944_20084b0(void);

void OvlFunc_944_2008468(void)
{
    __CutsceneStart();
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xa4;
        q2 = 0x1410000;
        q0 = 0;
        q1 <<= 16;
        __MapActor_SetPos(q0, q1, q2);
    }
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __WaitFrames(1);
    __Func_800fe9c();
    __WaitFrames(1);
    OvlFunc_944_20084b0();
    __CutsceneEnd();
}
