/* Cluster OvlFunc_959_20092e0..OvlFunc_959_20092e0 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_a_a_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7e7574/ovl_9dc_c_a_a_a_a_c_a.o and asm/overlays/rom_7e7574/ovl_9dc_c_a_a_a_a_c_c.o in
 * goldensun/overlays/rom_7e7574/overlay.ld.
 *
 * A short beat: park an actor, face it, animate it, emote, hand off.
 * Straight-line, twenty-five instructions.
 *
 * THIS CORRECTS A CLAIM MADE IN src/non_matching/ovl_7ac2d8/2008ffc.c. That park
 * observed r0 being filled in the MIDDLE of an argument block --
 *
 *     rom  ldr r2, =0x3333 / mov r0, #0 / ldr r1, =0x6666
 *
 * -- and concluded that the class is "r0's position in the argument block" and
 * that neither declaration lever reaches it. The first half is a fair
 * description; the second half is too strong.
 *
 * __Func_809228c here has exactly that shape:
 *
 *     rom    mov r2, #0x0 / mov r0, #0x9 / mov r1, #0x0
 *     ours   mov r2, #0x0 / mov r1, #0x0 / mov r0, #0x9
 *
 * and DECLARING THE CALLEE FIXES IT. So a middle-position r0 is sometimes
 * reachable and sometimes not, and "r0 is not at either end" is not by itself a
 * blocker. What distinguishes the two cases is still open -- the one that
 * resists has its other two arguments coming from POOL LOADS while this one has
 * three plain `mov`s, which is a difference but not yet an explanation.
 *
 * Three of the five multi-argument calls needed declaring and two did not,
 * which is the usual per-call-site split.
 */
extern void __Func_809228c(int a, int b, int c);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_809280c(int a, int b, int c);

void OvlFunc_959_20092e0(void)
{
    __CutsceneStart();
    __MapActor_SetIdle(9);
    __Func_809228c(9, 0, 0);
    __MapActor_SetAnim(9, 0);
    __Func_809280c(9, 0, 0);
    __MapActor_Emote(9, 0x80 << 1, 0);
    OvlFunc_959_2009b24(0xa);
    __CutsceneEnd();
}
