/* Cluster OvlFunc_949_20085dc..OvlFunc_949_20085dc extracted from goldensun/asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c.s.
 *
 * Slotted between ovl_30_c_c_a_c_c_c_c_c_c_a.o and the rest of the overlay.
 *
 * BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile and the
 * standing item in HANDOFF.md. The flag id is read in a guard and written in
 * the body on the SAME path, so gcc`s second CSE pass hoists it into a
 * callee-saved register across the call; the ROM loads it twice.
 *
 * A first-visit cutscene with a long first arm and a short second. 38
 * instructions against 36 without the flag.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);
extern void __Func_808f1c0(int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8091a58(int a, int b);

void OvlFunc_949_20085dc(void)
{
    __CutsceneStart();
    if (!__GetFlag(0x8bf)) {
        __SetFlag(0x8bf);
        __MessageID(0x2368);
        __ActorMessage(0x13, 0);
        __Func_808f1c0(0xe9, 3);
        __ActorMessage(0x13, 0);
        __MapActor_SetAnim(0, 1);
        __Func_8091a58(0xe9, 0);
    } else {
        __MessageID(0x236a);
        __ActorMessage(0x13, 0);
    }
    __CutsceneEnd();
}
