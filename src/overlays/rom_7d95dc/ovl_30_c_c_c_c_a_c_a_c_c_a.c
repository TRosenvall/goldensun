// fakematch
/* Cluster OvlFunc_953_200a5f0..OvlFunc_953_200a5f0 extracted from goldensun/asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_c_c.s.
 *
 * Total .text for this TU = 120 bytes (= 0x78).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_c_b.o and asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_c_c_b.o in
 * goldensun/overlays/rom_7d95dc/overlay.ld.
 *
 * This was parked at 29 of 43 and declared UNREACHABLE on a read of gcse.c.
 * The read is correct and the verdict was too strong -- see the commit message.
 * A register pin closes it; the park never tried one.
 *
 * TEARDOWN, by REMOVING each piece from the finished file:
 *
 *   the three __Func_8092158 / __MapActor_TravelTo pin blocks   33 differing
 *   the __MapActor_SetSpeed pin block                            2
 *   pinning r2 alone at the three sites instead of all three    12
 *   nothing (as landed)                                          0
 *
 * The third row is the recorded "anchor every argument of a call you anchor any
 * argument of" rule: r2 is the only COMMONED constant, but pinning it alone is
 * worse than the full block.
 */
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __Func_8092158(int slot, int x, int z);
extern void __Func_8091e9c(int n);

void OvlFunc_953_200a5f0(void)
{
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");

    __CutsceneStart();
    p2 = 0xcccc;
    p0 = 0;
    p1 = 0x19999;
    __MapActor_SetSpeed(p0, p1, p2);
    __MapTransitionIn();
    __MapActor_SetAnim(0, 2);
    p1 = 0xc3;
    p2 = 0xd6;
    p0 = 0;
    p1 <<= 2;
    p2 <<= 1;
    __Func_8092158(p0, p1, p2);
    p1 = 0xdc;
    p2 = 0xd6;
    p0 = 0;
    p1 <<= 2;
    p2 <<= 1;
    __Func_8092158(p0, p1, p2);
    p1 = 0xf5;
    p2 = 0xd6;
    p0 = 0;
    p1 <<= 2;
    p2 <<= 1;
    __MapActor_TravelTo(p0, p1, p2);
    __MapTransitionOut();
    __WaitMapTransition();
    if (__GetFlag(0x90f) != 0)
        __Func_8091e9c(0x20);
    else
        __Func_8091e9c(0xc);
}
