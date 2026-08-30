/*
 * OvlFunc_946_2009494 -- asm/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_a_a_c.s
 *
 * BLOCKER: argument emission order. 35 lines against 35, FOUR differing, and
 * they are two adjacent pairs at two different call sites:
 *
 *      rom   ldr r1, =0x6666 / mov r0, #0x0 / ldr r2, =0x3333
 *      ours  ldr r1, =0x6666 / ldr r2, =0x3333 / mov r0, #0x0
 *
 *      rom   mov r2, #0x10 / mov r1, #0x3 / neg r2, r2
 *      ours  mov r2, #0x10 / neg r2, r2 / mov r1, #0x3
 *
 * In both cases the ROM INTERLEAVES a cheap argument between the two halves of
 * an expensive one, and we finish the expensive one first. Same class as
 * OvlFunc_932_20082cc.
 *
 * TRIED AND REJECTED, both byte-identical to the version below:
 *
 *   * Naming the -0x10 in a local, on the theory that a two-instruction
 *     constant wants a name (which is true for OvlFunc_927_2009078).
 *   * Naming the two pooled speed constants in locals.
 *
 * SETTLED, and it is why this is 4 rather than unmatched: the flag id is
 * `*(short *)(gp + (0xe0 << 1)) + 0x8c8 - (int)&_AREA_7e`, written INLINE.
 * The ROM computes 0x84a at runtime from two pooled values -- 0x7e fits an
 * eight-bit mov, so pooling it is the symbol tell, and gcc would fold a
 * subtraction of two literals at compile time, so it cannot be two literals.
 * Naming the difference hoists its block above the gState address block; see
 * OvlFunc_946_20092b4, where that is 14 differing against 2.
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
    __MapActor_SetSpeed(0, 0x6666, 0x3333);
    __MapTransitionIn();
    __WaitMapTransition();
    gp = gState;
    __SetFlag(*(short *)(gp + (0xe0 << 1)) + 0x8c8 - (int)&_AREA_7e);
    __CutsceneWait(0x1e);
    __Func_8010560(gOvl_0200b2bc, 0x2c, 7);
    __Func_8092208(0, 3, -0x10);
    __Func_8091e9c(3);
    __CutsceneEnd();
}
