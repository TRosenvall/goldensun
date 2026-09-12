/*
 * OvlFunc_927_2009078 -- asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a_a.s
 *
 * ### CORRECTION, batch 258 -- EVERY NUMBER BELOW THIS LINE WAS MEASURED AT THE
 * ### WRONG OPTIMISATION LEVEL AND IS UNRELIABLE.
 *
 * This TU is caught by the mis-scoped rom_7b4558/ovl_30_c_c_a_c_a% wildcard,
 * which applies O1_CFLAGS. Re-measured on the source below:
 *
 *     -O1   72 differing of 80, ours 81 instructions  <- what the notes below saw
 *     -O2   32 differing of 80, ours 80 instructions, LENGTH EXACT
 *
 * So "80 lines against 79 -- ONE OVER" is an -O1 artifact; at -O2 the length is
 * already right and the extra instruction the note builds its whole argument on
 * does not exist. The "TRIED AND REJECTED" table below was measured against the
 * -O1 output and must be re-run before any of it is trusted.
 *
 * The residue that IS real, at -O2, is a systematic register-role swap -- the
 * ROM reads through r6 where we read through r5, e.g. ROM `ldr r3, [r6, #8]`
 * (68b3) against our `ldr r3, [r5, #8]` (68ab), repeated across most of the 32.
 * That is the recorded QTY_CMP_PRI / ref-count class, so the note's headline
 * ("callee-saved register roles") was pointing the right way even though its
 * evidence was not.
 *
 * This was found by sweeping all parks for TUs whose flags come from a wildcard
 * rather than an explicit rule (scratch_elev/b260/sweep.py). See
 * src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_c_b.c, which was parked the same
 * way and turned out to be two instructions from exact.
 *
 * --- original note follows, at -O1 ---
 *
 * BLOCKER: callee-saved register roles. 80 lines against 79 -- ONE OVER. The
 * ROM puts the actor in r6 and the position delta in r5; we do the reverse,
 * and the extra instruction follows from that.
 *
 * The prologue is otherwise instruction-for-instruction identical, including
 * the three stack-buffer stores and the two `and` operations against the
 * pooled 0xfff00000.
 *
 * TRIED AND REJECTED, all measured:
 *
 *   * Declaring the delta before the actor (declaration order). NO CHANGE.
 *   * Assigning the delta at the top of the function, before the actor, to
 *     make it the first-born value. WORSE -- 77 differing.
 *   * `saved = e[0x55];` before `f = e + 0x55;` rather than after. NO CHANGE.
 *   * Initialising the delta to 0 early to lengthen its live range. WORSE.
 *   * Removing the delta local entirely and writing both literals at all eight
 *     use sites. WORSE -- 77 lines, TWO SHORT. So the local IS required; gcc
 *     will not otherwise keep either constant in a register across the calls.
 *
 * The last of those is the useful one: this is a case where naming a constant
 * is necessary, in contrast to OvlFunc_926_200a5b8 where naming a zero cost six
 * instructions. The difference is that 0xffff0000 and 0x10000 are pooled or
 * two-instruction values, so keeping them beats rebuilding them; a zero is one
 * instruction and gcc always rebuilds it.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern int OvlFunc_927_2008cd0(int *p);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);

void OvlFunc_927_2009078(void)
{
    int buf[3];
    unsigned char *e;
    unsigned char *f;
    int saved;
    int d;

    e = __MapActor_GetActor(0);
    f = e + 0x55;
    saved = *f;
    buf[0] = (*(int *)(e + 8) & 0xfff00000) + (0x80 << 12);
    buf[1] = *(int *)(e + 0xc);
    buf[2] = (*(int *)(e + 0x10) & 0xfff00000) + (0xa0 << 14);
    if (OvlFunc_927_2008cd0(buf) != 0) {
        __CutsceneStart();
        *f = 0;
        __MapActor_SetAnim(9, 7);
        d = 0xffff0000;
        *(int *)(e + 0xc) += d;
        *(int *)(e + 0x14) += d;
        __WaitFrames(2);
        *(int *)(e + 0xc) += d;
        *(int *)(e + 0x14) += d;
        __WaitFrames(0xa);
        d = 0x80 << 9;
        *(int *)(e + 0xc) += d;
        *(int *)(e + 0x14) += d;
        __WaitFrames(4);
        *(int *)(e + 0xc) += d;
        *(int *)(e + 0x14) += d;
        *f = saved;
        __CutsceneEnd();
    }
}
