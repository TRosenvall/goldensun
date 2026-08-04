/* Cluster OvlFunc_890_2008108..OvlFunc_890_2008108 extracted from goldensun/asm/overlays/rom_78b2ac/ovl_30_c_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_a.o and asm/overlays/rom_78b2ac/ovl_30_c_c_a_a_c.o in
 * goldensun/overlays/rom_78b2ac/overlay.ld.
 *
 * A one-shot scene: if a flag is clear, run it, set that flag and clear two
 * neighbouring ones.
 *
 * Built with CSE_CFLAGS (-fno-rerun-cse-after-loop). The flag id 0x200 is built
 * TWICE in the ROM, once for the test and once for the set:
 *
 *     mov r0, #0x80 / lsl r0, #2 ... mov r0, #0x80 / lsl r0, #2
 *
 * At -O2 gcc hoists it into a callee-saved register and pays a push and a pop
 * to do it -- 25 instructions against 24, with the register assignment
 * diverging from instruction zero.
 *
 * THIS CORRECTS THE BOUND I PUT ON THAT FLAG LAST BATCH.
 * src/non_matching/overlays/constant_reuse.c said the flag fixes the POOLED
 * constant variant and does nothing for the REGISTER-BUILT one. That is wrong:
 * 0x200 here is register-built with `mov` + `lsl`, and the flag fixes it.
 *
 * The real distinction is WHERE the repetition is:
 *
 *   across separate CALLS   -- the rerun-CSE pass hoists it, and the flag stops
 *                              that. This function, and the two in batch 25.
 *   inside ONE argument     -- OvlFunc_965_2009158 builds -1 three times for a
 *   block                      single call's r0/r1/r2. The flag changes nothing
 *                              there, because that is argument setup rather
 *                              than common-subexpression elimination.
 *
 * See the CSE_CFLAGS block in the Makefile for the standing caveat on the
 * evidence, and for the global-flag experiment: applying it to every TU breaks
 * several overlays and the main ROM link, so the pass is wanted almost
 * everywhere and unwanted here.
 */
extern void __Func_8091200(int a, int b);

void OvlFunc_890_2008108(void)
{
    if (__GetFlag(0x80 << 2) == 0) {
        __CutsceneStart();
        __Func_8091200(0x80 << 9, 1);
        __Func_8091254(0x14);
        __SetFlag(0x80 << 2);
        __ClearFlag(0x201);
        __ClearFlag(0x202);
        __CutsceneEnd();
    }
}
