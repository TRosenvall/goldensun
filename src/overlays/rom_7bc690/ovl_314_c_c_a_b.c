/* Cluster OvlFunc_933_2008498..OvlFunc_933_2008498 extracted from goldensun/asm/overlays/rom_7bc690/ovl_314_c_c_a.s.
 *
 * Slotted between ovl_314_c_c_a_a.o and the rest of the overlay.
 *
 * BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile and the
 * standing item in HANDOFF.md. The flag id is read in a guard and written in
 * the body on the SAME path, so gcc`s second CSE pass hoists it into a
 * callee-saved register across the call; the ROM loads it twice.
 *
 * TWO ids repeated, not one: 0x8b2 and 0x8b3 are each read in the `&&` guard
 * and written in the body. 25 instructions against 22 without the flag.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __PlaySound(int id);
extern void __Func_8091e9c(int a);

void OvlFunc_933_2008498(void)
{
    __CutsceneStart();
    if (!__GetFlag(0x8b2) && !__GetFlag(0x8b3)) {
        __SetFlag(0x8b3);
        __SetFlag(0x8b2);
    }
    __PlaySound(0x7b);
    __Func_8091e9c(3);
    __CutsceneEnd();
}
