/* Cluster OvlFunc_884_2008634..OvlFunc_884_2008634 extracted from goldensun/asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_c.s.
 *
 * Slotted between ovl_30_c_a_a_a_c_c_a_c_c_a.o and the rest of the overlay.
 *
 * BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile and the
 * standing item in HANDOFF.md. The flag id is read in a guard and written in
 * the body -- the recognition rule from batch 50 -- and gcc`s second CSE pass
 * hoists it into a callee-saved register across the call, where the ROM simply
 * loads it twice.
 *
 * A first-visit cutscene: one message the first time, a different one after,
 * with the flag set inside the first arm only.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);

void OvlFunc_884_2008634(void)
{
    __CutsceneStart();
    if (__GetFlag(0x302)) {
        __MessageID(0x1be4);
    } else {
        __MessageID(0x1be3);
        __SetFlag(0x302);
    }
    __ActorMessage(0xb, 0);
    __CutsceneEnd();
}
