/* OvlFunc_926_2009334  --  0x02009334, cut from the head of
 * goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_c.s; the remaining
 * functions follow as ovl_314_c_c_a_c_c_c_a_a_c_c.o.
 *
 * Dispatch on the player's facing quadrant: four 0x4000-wide arcs, each with
 * its own handler, bracketed by a cutscene and an animation change.
 *
 * THE RANGE TEST IS ONE UNSIGNED COMPARE, not two signed ones. The ROM writes
 *
 *     add r3, r2, r0        @ r0 = -0x2000
 *     lsl r3, #16
 *     cmp r3, r0            @ r0 = 0x3fff0000
 *     bhi ...
 *
 * for the first arc and `lsl #16 / lsr #16 / cmp r3, r1` with r1 = 0x3fff for
 * the other three -- the same test, once with the value left in the high half
 * against a pre-shifted bound and three times shifted back down against the
 * plain one. Both fall out of `(unsigned short)(a - 0x2000) <= 0x3fff`; gcc
 * picks which form per arc and neither has to be written differently.
 *
 * The two bounds are hoisted into r0 and r1 before the first test and reused
 * across all four, which is what writing the same `<= 0x3fff` four times gives.
 * It matched on the first screen.
 */
struct E { unsigned char pad00[6]; unsigned short f6; };

extern struct E *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int slot, int a);
extern void OvlFunc_926_2008e94(void);
extern void OvlFunc_926_2008bf4(void);
extern void OvlFunc_926_2008db4(void);
extern void OvlFunc_926_2008cd4(void);
extern void OvlFunc_926_200902c(int n);

void OvlFunc_926_2009334(void)
{
    struct E *e;
    unsigned short a;

    e = __MapActor_GetActor(0);
    __CutsceneStart();
    __MapActor_SetAnim(0, 8);
    __CutsceneWait(0x14);
    a = e->f6;
    if ((unsigned short)(a - 0x2000) <= 0x3fff)
        OvlFunc_926_2008e94();
    else if ((unsigned short)(a - 0x6000) <= 0x3fff)
        OvlFunc_926_2008bf4();
    else if ((unsigned short)(a + 0x6000) <= 0x3fff)
        OvlFunc_926_2008db4();
    else
        OvlFunc_926_2008cd4();
    __MapActor_SetAnim(0, 1);
    OvlFunc_926_200902c(1);
    __CutsceneEnd();
}
