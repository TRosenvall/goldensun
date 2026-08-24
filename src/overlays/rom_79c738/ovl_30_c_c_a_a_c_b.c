/* Cluster OvlFunc_909_20081b4..OvlFunc_909_20081b4 extracted from goldensun/asm/overlays/rom_79c738/ovl_30_c_c_a_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_79c738/ovl_30_c_c_a_a_c_a.o and the rest of the overlay in
 * goldensun/overlays/rom_79c738/overlay.ld.
 *
 * A one-shot line of dialogue: say the standard message, add a second one the
 * first time through, then speak and set the flag. The near-twin of
 * src/overlays/rom_79c738/ovl_30_c_c_a_c_a_a_c.c in the same overlay -- same
 * structure, flag 0x302 instead of 0x303 -- found by tools/match_shapes.py.
 *
 * BUILT WITH -fno-rerun-cse-after-loop, and it is the SEVENTH TU that needs it.
 * The flag id is used twice around a call and at -O2 gcc hoists it into a
 * callee-saved register, spending a push, a pop and two moves to save one pool
 * load; the ROM loads it twice. Nineteen instructions against twenty-one
 * without the flag, and byte-exact with it. See CSE_CFLAGS in the Makefile for
 * which flags do NOT work and for the standing caveat: this may mean gcc-2.96
 * runs a pass the original compiler did not, in which case the right fix is a
 * compiler difference and all seven rules should go.
 *
 * WHAT IS LOAD-BEARING BESIDES THE FLAG: __ActorMessage and __SetFlag are
 * OUTSIDE the `if`. The ROM's join label sits before the __ActorMessage, not
 * after the __SetFlag. The exemplar was parked for several rounds with both
 * inside the guard -- code that would have skipped a line of speech and never
 * set the flag -- because the constant-CSE diagnosis was also true and hid it.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern int __GetFlag(int id);

void OvlFunc_909_20081b4(void)
{
    __CutsceneStart();
    __MessageID(0x1750);
    if (__GetFlag(0x302))
        __MessageID(0x1768);
    __ActorMessage(0xf, 0);
    __SetFlag(0x302);
    __CutsceneEnd();
}
