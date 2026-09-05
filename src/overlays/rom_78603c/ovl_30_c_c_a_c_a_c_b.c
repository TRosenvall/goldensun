// fakematch
/* OvlFunc_885_20092a0  --  0x020092a0
 *
 * 238 instructions of cutscene, the whole body behind one guard.
 *
 * BUILT AT -O2, AGAINST A WILDCARD THAT SAYS -O1. Makefile's
 * rom_78603c/ovl_30_c_c_a_c_a% pattern captures this split product on prefix
 * alone; the function is EXACT at -O2 and 32 encodings wrong at -O1. An
 * explicit non-pattern rule overrides it -- the fourth instance of the trap
 * already written up at Makefile:289, and the reason to distrust an inherited
 * flag when screening: objcmp run against the ORIGINAL .s path reports
 * `(built with: O1)`, while the same candidate on a scratch path gets the tree
 * default. Two different answers for one file.
 *
 * WHY THIRTEEN PINS. The guard dominates everything, so every constant in the
 * body sits on one straight-line path and cse1 hoists each repeated one into a
 * callee-saved pseudo: 0x800 to r7, 0x8000 to r8, 0xcccc/0x6666 to r5/r6,
 * 0xa4<<1 to r5, 0xb000 to r5. Plain C is 236 of 238 differing at 243 lines
 * with a `push {r5, r6, r7, lr}` and an r8 spill. The ROM pushes only
 * {r5, r6, lr} -- those two hold the actor coordinates and NOTHING else -- and
 * rebuilds every constant at every use. A pin on a call-clobbered argument
 * register makes the value dead across the next bl, so gcc has no register it
 * can carry it in and must rematerialise. No pinned range crosses a call, so
 * the dropped-store hazard does not apply.
 *
 * THE SCAFFOLDING IS MINIMAL BY MEASUREMENT, not by eye. Each of 23 pinned
 * sites was stripped one at a time; 11 were inert and are gone, and all 11
 * removed together still screens exact. A second round over the surviving 12
 * found every one load-bearing. The two dearest: unpinning
 * __MapActor_SetSpeed(5, 0xcccc, 0x6666) costs 209 differing and a longer
 * push, and unpinning the first __Func_8092adc(0xd, 0x8000, 0x14) costs 229,
 * because 0x8000 is shared with the later site.
 *
 * WRITE THE SHIFTS IN THE MOVS' ORDER, not the ROM's shift order -- the movs
 * are slaved to source shift order and sched2 re-lands the shifts itself.
 *
 * NEW, AND THE POINT OF THIS FILE: SPLITTING THE SHIFT OFF THE LOAD SWAPS
 * WHICH CALLEE-SAVED REGISTER EACH OF A COORDINATE PAIR GETS.
 *
 *     x = *(short *)(p + 0xa) << 16;    -- r6 = [+0xa], r5 = [+0x12]. 8 differ.
 *     y = *(short *)(p + 0x12) << 16;
 *
 *     x = *(short *)(p + 0xa);          -- r5 = [+0xa], r6 = [+0x12]. EXACT.
 *     y = *(short *)(p + 0x12);
 *     x <<= 16;
 *     y <<= 16;
 *
 * Both forms emit the SAME EIGHT INSTRUCTIONS IN THE SAME ORDER; only the
 * register numbers differ. Birth order is identical, and declaration order is
 * inert -- `int x, y`, `int y, x` and declaring inside the block are all 8
 * differing in the fused form and all exact in the split form. So this is
 * neither the pointer-birth-order rule nor the statement-order rule; the
 * statement order does not change. What changes is HOW MANY RTL INSNS EACH
 * PSEUDO'S DEFINITION IS SPLIT INTO AT EXPAND, which reorders the allocator's
 * work list. "Everything right but two callee-saved registers swapped" now has
 * a third thing to try, and it costs nothing.
 *
 * The assumption behind all of the above, stated so it can be re-derived: r5
 * and r6 belong to the actor-coordinate pair and to nothing else. The narrow
 * `push {r5, r6, lr}` is what proves it, and it is what makes "every constant
 * must be rebuilt" the whole problem. A sibling that also pushes r7 breaks the
 * reading.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __SetCameraTarget(int a, int b);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_885_20092a0(void)
{
    unsigned char *p;
    int x, y;
    register int p0 __asm__("r0");

    p0 = 0x80; p0 <<= 4;
    if (__GetFlag(p0) == 0) {
        __CutsceneStart();
        __MapActor_SetSpeed(0, 0x80 << 9, 0x80 << 8);
        __MessageID(0xf9f);
        __ActorMessage(0xd, 0);
        p = __MapActor_GetActor(0);
        x = *(short *)(p + 0xa);
        y = *(short *)(p + 0x12);
        x <<= 16;
        y <<= 16;
        __MapActor_SetPos(5, x, y);
        __MapActor_SetPos(1, x, y);
        { PIN3; q0 = 5; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q1 = 0x94; q2 = 0xa4; q0 = 5; q1 <<= 1; q2 <<= 1;
          __Func_809218c(q0, q1, q2); }
        { PIN3; q1 = 0x8c; q2 = 0xa4; q0 = 1; q1 <<= 1; q2 <<= 1;
          __Func_80921c4(q0, q1, q2); }
        __MapActor_SetAnim(0, 0);
        __MapActor_SetAnim(5, 0);
        __MapActor_SetAnim(1, 0);
        { PIN3; q1 = 0xb0; q0 = 5; q1 <<= 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xb0; q0 = 1; q1 <<= 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xb0; q0 = 0; q1 <<= 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __Func_80933f8(0xe8 << 16, -1, 0xf0 << 16, 1);
        __Func_8093530();
        { PIN3; q1 = 0x80; q2 = 0x14; q0 = 0xd; q1 <<= 8;
          __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(0xd, 2);
        __ActorMessage(0xd, 0);
        { PIN3; q0 = 0xd; q1 = 0x3333; q2 = 0x1999;
          __MapActor_SetSpeed(q0, q1, q2); }
        __Func_80921c4(0xd, 0xd8, 0xe8);
        __CutsceneWait(0x14);
        __Func_80925cc(0xd, 1);
        __CutsceneWait(0xa);
        __Func_8093040(0xd, 0, 0x28);
        __MapActor_Jump(0xd, 2, 0xa);
        __Func_8092adc(0xd, 0, 0xa);
        __Func_8093040(0xd, 0, 0xa);
        __Func_80921c4(0xd, 0xf8, 0xe8);
        __CutsceneWait(0x28);
        __Func_80925cc(0xd, 2);
        __CutsceneWait(0x14);
        __ActorMessage(0xd, 0);
        { PIN3; q2 = 0x3c; q0 = 0xd; q1 = 0x101;
          __MapActor_Emote(q0, q1, q2); }
        __Func_80925cc(0xd, 1);
        { PIN3; q1 = 0x80; q0 = 0xd; q1 <<= 8; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        __Func_8093040(0xd, 0, 2);
        __Func_80921c4(0xd, 0xe8, 0xe8);
        __CutsceneWait(2);
        { PIN3; q1 = 0x80; q2 = 4; q0 = 0xd; q1 <<= 7;
          __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(0xd, 2);
        __CutsceneWait(4);
        __Func_8093040(0xd, 0, 2);
        __MapActor_DoAnim(0xd, 4);
        __CutsceneWait(0x14);
        __SetCameraTarget(0, 1);
        __Func_8093530();
        __MapActor_SetAnim(1, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(1);
        __MapActor_SetPos(1, 0, 0);
        __MapActor_SetAnim(5, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(5, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(5);
        __MapActor_SetPos(5, 0, 0);
        p0 = 0x80; p0 <<= 4;
        __SetFlag(p0);
        __CutsceneEnd();
    }
}
