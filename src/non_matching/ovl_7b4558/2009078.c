/*
 * OvlFunc_927_2009078 -- asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a_a.s
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
