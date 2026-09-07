/* OvlFunc_955_2009424 -- 0x02009424
 *
 * // fakematch: SIX `register ... __asm__("rN")` pins on argument registers,
 * plus one per-call-site prototype alias.
 *
 *  - The -1 pins (n1 at the first __Func_80933f8, m0/m1 at the second).  The
 *    two calls pass -1 four times between them; -1 costs `mov rN,#1 / neg`,
 *    so cse commons the four into ONE pseudo, and because that pseudo is live
 *    across __Func_80933d4 it must be callee-saved -- which is the fourth
 *    callee-saved register and the `mov r7, r8 / push {r7}` the ROM does not
 *    have.  Pinning the value to the argument register at each site builds it
 *    where the ROM builds it and leaves nothing to common.  This is what the
 *    park called the blocker, and it is what un-blocked the base naming below:
 *    `g = gState` alone was 97 differing, the pins alone 100, together 9.
 *
 *  - s0/s1/s2 at __MapActor_SetSpeed, same LUID mechanism as 2008160's first
 *    __Actor_TravelTo: the ROM's `mov r0, #0` sits between the two `mov #imm`s
 *    and the two `lsl`s.  All three needed -- s0 alone is worse than none.
 *
 *  - __ActorMessage_1 is __ActorMessage under a different RETURN TYPE, used at
 *    the ONE of its four call sites where the ROM emits `mov r1` before
 *    `mov r0`.  Per-call-site declaration lever; the other three sites keep
 *    the void prototype.
 *
 *  - OvlFunc_common1_1078, _15b8 and _5e4 are deliberately left UNDECLARED
 *    (implicit int).  That is what puts `mov r0` at the END of their argument
 *    setup, as the ROM has it.  Do not add prototypes for them.
 */
extern unsigned char gState[];

extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern int __ActorMessage_1(int slot, int n) __asm__("__ActorMessage");
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __SetCameraTarget(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8093c00(void);
extern int OvlFunc_common1_4cc(int a, int b);
extern void OvlFunc_common1_2c4(void);
extern void OvlFunc_common1_1254(int a);
extern void OvlFunc_common1_588(int a, int b);
extern void OvlFunc_955_20088ec(void);
extern void OvlFunc_955_2008950(void);
extern void OvlFunc_955_2008970(void);

void OvlFunc_955_2009424(int a)
{
    int r;
    int k;

    unsigned char *g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
        return;
    }
    __CutsceneStart();
    r = OvlFunc_common1_4cc(a, 2);
    if (r == 0) {
        __MessageID(0x20a2);
        OvlFunc_955_20088ec();
        __Func_80933d4(0xc0 << 10, 0xc0 << 7);
        {
            int q0;
            register int n1 __asm__("r1");
            q0 = 0xf6 << 18;
            n1 = -1;
            __Func_80933f8(q0, n1, 0xe8 << 16, 1);
        }
        __Func_8093530();
        __ActorMessage_1(a, 0);
        k = 0x87;
        OvlFunc_955_2008950();
        __ActorMessage(a, 0);
        k = k << 3;
        OvlFunc_common1_1078(0, k, 0x84 << 1);
        __CutsceneWait(0xf);
        {
            register int s0 __asm__("r0");
            register int s1 __asm__("r1");
            register int s2 __asm__("r2");
            s0 = 0;
            s1 = 0xc0 << 9;
            s2 = 0xc0 << 8;
            __MapActor_SetSpeed(s0, s1, s2);
        }
        OvlFunc_common1_15b8(0, k, 0xd8);
        OvlFunc_common1_15b8(0, 0x85 << 3, 0xd8);
        OvlFunc_955_2008970();
        __Func_8093c00();
        {
            register int m0 __asm__("r0");
            register int m1 __asm__("r1");
            int m2;
            m0 = -1;
            m1 = -1;
            m2 = -1;
            __Func_80933f8(m0, m1, m2, 0);
        }
        __ActorMessage(a, 0);
        OvlFunc_common1_1254(0);
        __SetCameraTarget(0, 0);
        OvlFunc_common1_588(a, 2);
    } else if (r == 1) {
        __MessageID(0x20a1);
        __ActorMessage(a, 0);
    }
    OvlFunc_common1_5e4(r, a, 2);
    __CutsceneEnd();
}
