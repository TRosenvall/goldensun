/* OvlFunc_927_2009078, the whole of
 * goldensun/asm/overlays/rom_7b4558/ovl_30_c_c_a_c_a_a.s.
 *
 * A cutscene nudge: the actor at slot 0 is walked two steps in -y and two
 * steps back in +y, its animation flag byte at +0x55 saved and restored around
 * the move, with the whole thing gated on OvlFunc_927_2008cd0 accepting a
 * three-word probe built from the actor's own coordinates.
 *
 * TWO DELTA LOCALS, NOT ONE, AND THAT IS THE WHOLE MATCH.  The ROM holds the
 * actor in r6 and the step in r5; a single `int d` assigned twice gives the
 * mirror image, r5 for the actor and r6 for the step, and 32 of the 80
 * encodings differ by nothing but that swap.
 *
 * The step is only a local-allocator quantity while it dies in ONE place.  As a
 * single variable set twice it "dies in 2 places" and local-alloc declines it,
 * so it falls through to global-alloc and is ranked against the actor by
 * floor_log2(n_refs) * n_refs / live_length: the actor scores 4*21/57 and the
 * step 3*10/56, the actor is allocated first and takes r5.  Split into two
 * names with disjoint live ranges, each is a clean single-def block-local,
 * local-alloc runs FIRST and hands both the same r5, and global-alloc is left
 * to give the actor r6, the flag pointer r7 and the saved byte r8 -- the ROM's
 * assignment, with no register pin and no inline asm.
 *
 * Naming the step at all is still required: writing the two constants at all
 * eight use sites is two instructions SHORT, because gcc will not otherwise
 * keep 0xffff0000 or 0x10000 in a register across the calls.
 *
 * The split-derived name falls inside the ovl_30_c_c_a_c_a% O1 wildcard, which
 * belongs to an unrelated stem.  This TU is EXACT at the default -O2 and 67 of
 * 80 differing at -O1, so it needs the explicit Makefile rule to escape it.
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
    int up;
    int down;

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
        up = 0xffff0000;
        *(int *)(e + 0xc) += up;
        *(int *)(e + 0x14) += up;
        __WaitFrames(2);
        *(int *)(e + 0xc) += up;
        *(int *)(e + 0x14) += up;
        __WaitFrames(0xa);
        down = 0x80 << 9;
        *(int *)(e + 0xc) += down;
        *(int *)(e + 0x14) += down;
        __WaitFrames(4);
        *(int *)(e + 0xc) += down;
        *(int *)(e + 0x14) += down;
        *f = saved;
        __CutsceneEnd();
    }
}
