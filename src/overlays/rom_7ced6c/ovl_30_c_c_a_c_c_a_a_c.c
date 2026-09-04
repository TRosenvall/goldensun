// fakematch
/* OvlFunc_946_2009494  --  0x02009494
 *
 * From goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_a_a_c.s, which held
 * this function alone and no data, so no split was needed.
 *
 * PARKED AT 4 OF 35, and both halves of the residue were shapes closed earlier
 * in this same batch. It needed no new lever at all -- only recognising which
 * two it already had.
 *
 *     rom   ldr r1, =0x6666 / mov r0, #0x0 / ldr r2, =0x3333
 *     ours  ldr r1, =0x6666 / ldr r2, =0x3333 / mov r0, #0x0
 *
 * That is the `precompute_register_parameters` bind: calls.c:805 copies every
 * argument whose rtx_cost exceeds 2 into a pseudo before any hard register is
 * loaded, both pool loads qualify, and the cheap `mov r0, #0` lands last. ONE
 * PIN ON r0 takes that argument out of the precompute path -- the same fix as
 * src/overlays/rom_7aa430/ovl_e90_c_c_a_a_c_c.c.
 *
 *     rom   mov r2, #0x10 / mov r1, #0x3 / neg r2, r2
 *     ours  mov r2, #0x10 / neg r2, r2   / mov r1, #0x3
 *
 * That is the mov/neg split with a DISTINCT interleaved value, which pinning
 * the three argument registers in the ROM's order places -- the same fix as
 * src/overlays/rom_7ec968/ovl_30_c_c_a_a_c_b.c, and the sub-case the
 * discriminator in src/non_matching/ovl_7ebdfc/2008120.c predicts will yield.
 *
 * TORN DOWN: pinning all three registers at the __MapActor_SetSpeed site as
 * well is byte-identical to the one-pin form, so only the r0 pin is kept there.
 *
 * The park's own rejected spellings stand and none is needed: naming the -0x10
 * in a local, and the other declaration levers it lists.
 */

extern unsigned char gState[];
extern int _AREA_7e;
extern unsigned char gOvl_0200b2bc[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_946_2009494(void)
{
    unsigned char *gp;

    __CutsceneStart();
    {
        register int q0 __asm__("r0");
        q0 = 0;
        __MapActor_SetSpeed(q0, 0x6666, 0x3333);
    }
    __MapTransitionIn();
    __WaitMapTransition();
    gp = gState;
    __SetFlag(*(short *)(gp + (0xe0 << 1)) + 0x8c8 - (int)&_AREA_7e);
    __CutsceneWait(0x1e);
    __Func_8010560(gOvl_0200b2bc, 0x2c, 7);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0x10;
        q1 = 3;
        q2 = -q2;
        q0 = 0;
        __Func_8092208(q0, q1, q2);
    }
    __Func_8091e9c(3);
    __CutsceneEnd();
}
