// fakematch
/* OvlFunc_939_2008ff0  --  0x02008ff0
 *
 * WAS PARKED at 2 of 157 as src/non_matching/ovl_7c460c/2008ff0.c, on the rule
 * that a site whose mov order runs against its shift order cannot be reached.
 * IT IS REACHABLE. The park closed by asking for exactly one thing:
 *
 *     "whether anything makes gcc pick the other mov first when both shifts
 *      are pending -- a construct that reverses the scheduler's preference
 *      without introducing a dependence, since a dependence emits a register
 *      copy instead of the immediate"
 *
 * That construct is a volatile asm with the register as an input and no output:
 *
 *     q1 = 0x80; __asm__ volatile ("" : : "r" (q1)); q2 = 0x80; q2 <<= 9; ...
 *
 * It consumes q1, so the mov must be materialised before it, and it produces
 * nothing, so there is no value for gcc to copy. Two of 157 to exact, one line.
 * The park's own analysis of WHY the site was stuck was right in every detail --
 * gcc orders the pair by which shift consumes first, and the two other
 * __MapActor_SetSpeed calls in this function whose shifts run the other way
 * matched unaided. What was wrong was the conclusion that the ordering could
 * not be overridden. It cannot be overridden by choosing operands, which is
 * what the seven forms in the park all did; it is a SCHEDULING fact and it
 * yields to a scheduling barrier.
 *
 * The other five levers are the park's and are unchanged: the flag id and the
 * three speed pairs are rematerialised by pinning their call-clobbered
 * registers (which also removes an eight-instruction r8/r10 spill), a
 * `do { } while (0)` puts a hoisted pool load back below the call it belongs
 * after, and the pins are block-scoped per call site rather than shared, since
 * a pin reused across a dozen calls is weaker than one declared beside the call
 * it serves.
 *
 * See also src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_a_c_a.c, the other
 * function parked on the same rule, closed in the same round by the same lever.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809259c(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __StartTask(void *fn, int arg);
extern void OvlFunc_939_2009240(void);

void OvlFunc_939_2008ff0(void)
{
    unsigned char *a;
    int msg;
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");

    p0 = 0x91;
    p0 <<= 2;
    if (__GetFlag(p0) != 0)
        return;
    p0 = 0x91;
    p0 <<= 2;
    __SetFlag(p0);
    __CutsceneStart();
    a = __MapActor_GetActor(0);
    __Func_809280c(8, 0, 0);
    __Func_809280c(9, 0, 0);
    __Func_809259c(8, 1);
    __Func_809259c(9, 1);
    __CutsceneWait(0x14);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x81; q2 = 0x3c; q1 <<= 1; q0 = 8;
        __MapActor_Emote(q0, q1, q2);
    }
    do { } while (0);
    msg = 0x2409;
    __MessageID(msg);
    __ActorMessage(8, 0);
    p1 = 0x80; p2 = 0x80; p0 = 0; p1 <<= 10; p2 <<= 9;
    __MapActor_SetSpeed(p0, p1, p2);
    p1 = 0x80; p2 = 0x80; p0 = 8; p1 <<= 10; p2 <<= 9;
    __MapActor_SetSpeed(p0, p1, p2);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80; __asm__ volatile ("" : : "r" (q1)); q2 = 0x80; q2 <<= 9; q0 = 9; q1 <<= 10;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    __MapActor_SetAnim(9, 4);
    __CutsceneWait(0x23);
    __MessageID(msg + 1);
    __ActorMessage(9, 0);
    __MapActor_Emote(8, 0x103, 0x1e);
    __MessageID(msg + 2);
    __ActorMessage(8, 0);
    __MapActor_SetAnim(9, 3);
    msg += 3;
    __CutsceneWait(0x19);
    __MessageID(msg);
    __ActorMessage(9, 0);
    __Func_809218c(8, *(short *)(a + 0xa) - 1, *(short *)(a + 0x12));
    __MapActor_WaitMovement(8);
    __Func_809218c(0, 0xa0, 0xd8);
    __Func_809218c(8, 0x98, 0xc8);
    __Func_809218c(9, 0xa8, 0xc8);
    __MapActor_WaitMovement(8);
    __MapActor_WaitMovement(9);
    __MapActor_WaitMovement(0);
    __Func_809280c(8, 0, 0);
    __Func_809280c(9, 0, 0);
    __CutsceneWait(0xc);
    p2 = 0x88; p0 = 0; p1 = 0xa0; p2 <<= 1;
    __Func_809218c(p0, p1, p2);
    p2 = 0x80; p0 = 8; p1 = 0x98; p2 <<= 1;
    __Func_809218c(p0, p1, p2);
    p2 = 0x80; p1 = 0xa8; p2 <<= 1; p0 = 9;
    __Func_809218c(p0, p1, p2);
    __MapActor_WaitMovement(8);
    __MapActor_WaitMovement(9);
    __MapActor_WaitMovement(0);
    __CutsceneEnd();
    __StartTask(OvlFunc_939_2009240, 0xc8 << 4);
}
