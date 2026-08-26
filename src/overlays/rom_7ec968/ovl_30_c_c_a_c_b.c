/* OvlFunc_963_2008730  --  0x02008730, cut from the tail of
 * goldensun/asm/overlays/rom_7ec968/ovl_30_c_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted after
 * asm/overlays/rom_7ec968/ovl_30_c_c_a_c.o in
 * goldensun/overlays/rom_7ec968/overlay.ld.
 *
 * The follow-up scene: a different line depending on a save bit, and on one
 * path a check that either plays an animation or bumps a counter by two.
 *
 * `__Func_8092c40` IS DELIBERATELY UNDECLARED. Do not add a prototype for it.
 * With one, gcc fills its argument registers r0 then r1; the ROM fills r1 then
 * r0, and that is the only difference in the function -- 2 of 47. This is the
 * SECOND declaration lever in docs/elevation.md: leaving the MISMATCHING call
 * implicit puts r0 last in that call's own argument block. Two other
 * combinations were measured and neither moves it: giving `__MessageID` an
 * `int` return type (the first lever, about the preceding call) and leaving
 * `__MessageID` implicit both leave the 2 in place.
 *
 * The three `__ActorMessage(9, 0)` calls really are three calls in the source.
 * The ROM has two, because gcc cross-jumps the flag arm and the inner
 * then-branch into a shared tail at .L772 -- writing two calls and a `goto`
 * would be transcribing the optimiser rather than the source.
 *
 * Both `if`s are written in the ROM's FALLTHROUGH order: `beq .L748` puts the
 * else-arm at the branch target, so the `if` body is the 0x2668 line, and
 * `bne .L77c` likewise puts the counter bump in the else.
 */
extern unsigned char iwram_3001ebc[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_DoAnim(int slot, int a);
extern int __Func_8091c7c(int a, int b);
/* __Func_8092c40 is intentionally implicit -- see the note above. */

void OvlFunc_963_2008730(void)
{
    char *base;

    __CutsceneStart();
    if (__GetFlag(0x89f)) {
        __MessageID(0x2668);
        __ActorMessage(9, 0);
    } else {
        __MessageID(0x264e);
        __Func_8092c40(9, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __ActorMessage(9, 0);
            __MapActor_DoAnim(9, 4);
            __ActorMessage(9, 0);
        } else {
            base = *(char **)iwram_3001ebc;
            (*(unsigned short *)(base + (0xec << 1))) += 2;
            __ActorMessage(9, 0);
        }
    }
    __CutsceneEnd();
}
