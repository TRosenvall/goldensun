// fakematch
/* OvlFunc_882_200bfb0  --  0x0200bfb0
 *
 * Cut out of goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_c_c.s.
 *
 * THE REPEATED ID IS THE WHOLE FUNCTION. `0x1016` is passed to __ActorMessage
 * once and __Func_8093040 twice. Left as three plain literals, gcc loads it once
 * into r5 and feeds all three from there, which widens the prologue from
 * `push {lr}` to `push {r5, lr}` and puts 77 of 118 instructions out of step --
 * the entire body shifts. Pinning r0 at each of the three sites forces the
 * reload, because r0 is call-clobbered and the value cannot survive a `bl`.
 * 77 of 118 to 3 of 116 in one change, with the length becoming exact.
 *
 * THE PROLOGUE WIDTH SAID THIS BEFORE ANY INSTRUCTION WAS COMPARED, and it is
 * the second batch running where that tell paid: one pushed register against
 * the ROM's zero means a value is being kept that the ROM rebuilds, exactly as
 * two against our one meant a value needed naming in
 * src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_c_b.c. Read the push list
 * first; it is one line and it is decisive in both directions.
 *
 * The opening __Func_80933f8(-1, -1, -1, 0) is a crossed site -- movs r0, r1,
 * r2 against negs r1, r2, r0 -- and takes the volatile-asm barrier after the
 * first mov. The byte flag at +0x23 is set with the four-statement accumulate
 * form from src/overlays/rom_799abc/ovl_30_c_c_c_c_b.c, with the pointer named
 * inside a tight block so its range dies at the store; the ROM interleaves the
 * following __MapActor_SetPos fill around the `strb`, and sched2 does that
 * unaided once the statements are right.
 *
 * The last call was left plain on the assumption that an ascending r0/r1/r2 ROM
 * order needs no help. It does here: the pooled third argument is issued first
 * unless the call is anchored. ANCHOR ANY CALL WITH A POOL LOAD IN IT, whatever
 * the register order looks like.
 */
extern void OvlFunc_882_200c5b8(void);

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __StopTask(void (*f)(void));
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_882_200bfb0(void)
{
    __CutsceneStart();
    {
        PIN4;
        q0 = 1; __asm__ volatile ("" : : "r" (q0));
        q1 = 1; q2 = 1; q3 = 0; q1 = -q1; q2 = -q2; q0 = -q0;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __MapActor_SetIdle(0x16);
    __StopTask(OvlFunc_882_200c5b8);
    { PIN3; q1 = 0xf0; q2 = 0xae; q0 = 0; q1 <<= 1; q2 <<= 3; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(0, 0, 0);
    { PIN3; q1 = 0xc0; q1 <<= 6; q2 = 0x14; q0 = 0x16; __Func_8092adc(q0, q1, q2); }
    {
        unsigned char *q = __MapActor_GetActor(0x16);
        int t, k;
        t = q[0x23]; k = 1; k |= t;
        q[0x23] = k;
    }
    { PIN3; q1 = 0xf9; q2 = 0x9b; q2 <<= 19; q1 <<= 16; q0 = 0x16; __MapActor_SetPos(q0, q1, q2); }
    __WaitFrames(1);
    __MessageID(0xed3);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q0 = 0x1016; q1 = 0;
        __ActorMessage(q0, q1);
    }
    { PIN3; q1 = 0xac; q2 = 0x4fe0000; q1 <<= 16; q0 = 0x16; __MapActor_SetPos(q0, q1, q2); }
    __WaitFrames(1);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q0 = 0x80; q1 = 0x80; q0 <<= 11; q1 <<= 8;
        __Func_80933d4(q0, q1);
    }
    { PIN4; q0 = 0xa2; q3 = 1; q2 = 0x5050000; q1 = 0; q0 <<= 16; __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0x16, 4);
    { PIN3; q0 = 0x1016; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0x14; q0 = 0x16; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x16, 2);
    { PIN3; q0 = 0x1016; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x14; q0 = 0x16; q1 <<= 5; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x16, 3);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x16; q1 <<= 10; q2 <<= 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0xa5; q2 = 0x514; __Func_80921c4(q0, q1, q2); }
    { PIN3; q2 = 0xb3; q0 = 0x16; q1 = 0xc3; q2 <<= 3; __Func_80921c4(q0, q1, q2); }
    __SetFlag(0x842);
}
