/* Cluster OvlFunc_common1_ea0..OvlFunc_common1_ea0 extracted from goldensun/asm/overlays/common/common1_a_a_a_a_c_c_a.s.
 *
 * Total .text for this TU = 268 bytes (= 0x10c).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/common/common1_a_a_a_a_c_c_a_a.o and asm/overlays/common/common1_a_a_a_a_c_c_a_c.o in
 * goldensun/overlays/rom_7db0c8/overlay.ld, goldensun/overlays/rom_7ddb88/overlay.ld and
 * goldensun/overlays/rom_7e0928/overlay.ld -- this file is shared by three overlays,
 * so all three linker scripts list the three parts.
 *
 * A cutscene with a short first arm for argument 0 and a long one otherwise.
 *
 * L11 IS DECLARED short[] AND INDEXED, NOT cast-and-offset.  Writing the store
 * as *(short *)(L11 + 0x1e) folds the +30 into the pooled address and stores at
 * [r3, #0]; the ROM loads the symbol base and keeps the displacement in the
 * store, which is what indexing a short[] produces.  See the symbol-base lever
 * in docs/elevation.md.
 */
extern short L11[] __asm__(".L11");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __PlayMapMusic(void);
extern int __Func_80f954c(void);
extern void OvlFunc_common1_88c(int a);
extern void OvlFunc_common1_e10(int a, int b);

void OvlFunc_common1_ea0(int a)
{
    if (a == 0) {
        __CutsceneStart();
        __MapTransitionIn();
        __WaitMapTransition();
        __CutsceneWait(0x1e);
        __PlaySound(0x59);
        OvlFunc_common1_88c(0);
        OvlFunc_common1_e10(1, 0);
        __CutsceneWait(0x78);
        __CutsceneEnd();
        return;
    }
    __PlaySound(0xf7);
    __CutsceneStart();
    __MapTransitionIn();
    __WaitMapTransition();
    L11[0xf] = a * 60;
    __CutsceneWait(0x1e);
    __PlaySound(a + 0x5a);
    OvlFunc_common1_88c(a);
    OvlFunc_common1_e10(1, 0);
    __CutsceneWait(0x78);
    while (__Func_80f954c())
        __WaitFrames(1);
    __PlaySound(0x121);
    OvlFunc_common1_88c(5);
    OvlFunc_common1_e10(2, 0);
    __PlaySound(0xec);
    __CutsceneWait(0x3c);
    OvlFunc_common1_e10(2, 1);
    __PlaySound(0xec);
    __CutsceneWait(0x3c);
    OvlFunc_common1_88c(6);
    OvlFunc_common1_e10(2, 0);
    __PlaySound(0xec);
    __CutsceneWait(0x3c);
    OvlFunc_common1_88c(7);
    OvlFunc_common1_e10(4, 0);
    __PlaySound(0xed);
    __PlayMapMusic();
    __CutsceneEnd();
    __SetFlag(0x123);
}
