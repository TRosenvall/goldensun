// fakematch
/* OvlFunc_939_2008b0c  --  0x02008b0c
 *
 * From goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a_b_a.s, which held
 * this function alone, so no split was needed.
 *
 * Member of shape group 0 (flat, {bl, bx, ldr, lsl, mov, pop, push}) at the
 * widened 45-instruction cut.
 *
 * THREE __MapActor_SetAnim CALLS, TWO ARGUMENT ORDERS. The ROM fills them
 * inconsistently within this one function:
 *
 *     mov r1, #1 / mov r0, #0      <- reversed, pinned
 *     mov r0, #0 / mov r1, #2      <- natural, written plainly
 *     mov r1, #1 / mov r0, #0      <- reversed, pinned
 *
 * The first and third are the same call with the same arguments and both need
 * the pin; the middle one does not and would be broken by it. This is the
 * clearest instance yet of why each call is read off the listing rather than
 * made consistent with its neighbours -- the inconsistency is in the ROM.
 *
 * __Func_809228c is filled r2, r1, r0 -- fully reversed -- and __MapActor_Emote
 * puts `mov r0` inside r1's shift build. __ActorMessage takes gcc's natural
 * order and is written plainly.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int actor, int b);
extern void __ClearFlag(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809228c(int a, int b, int c);

void OvlFunc_939_2008b0c(void)
{
    __CutsceneStart();
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 1;
        q0 = 0;
        __MapActor_SetAnim(q0, q1);
    }
    __MessageID(0x24cf);
    __ActorMessage(1, 0);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x81;
        q2 = 0x64;
        q0 = 0;
        q1 <<= 1;
        __MapActor_Emote(q0, q1, q2);
    }
    __MapActor_SetAnim(0, 2);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0xc;
        q1 = 0;
        q0 = 0;
        __Func_809228c(q0, q1, q2);
    }
    __MapActor_WaitMovement(0);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 1;
        q0 = 0;
        __MapActor_SetAnim(q0, q1);
    }
    __ClearFlag(0x243);
    __CutsceneEnd();
}
