// fakematch
/* OvlFunc_890_2009a58  --  0x02009a58
 *
 * The last of the twelve functions in the original
 * goldensun/asm/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a.s, split off after
 * OvlFunc_890_2008c00 was taken out of the same file this round.
 *
 * 152 instructions of straight-line cutscene behind two guards, EXACT ON THE
 * FIRST SCREEN. Twenty-one call sites are pinned with their assignments in ROM
 * order and `0x81 << 4` is rebuilt at both its uses with an r0 pin; there is no
 * new lever here at all.
 *
 * The one thing worth noting is the ARGUMENT ORDER VARIES BETWEEN CALLS TO THE
 * SAME FUNCTION, again. __Func_80921c4 is called five times and the ROM fills
 * its registers four different ways -- `mov r1 / mov r2 / mov r0 / lsl r1`,
 * `mov r1 / lsl r1 / mov r2 / mov r0`, `mov r1 / mov r2 / lsl r1 / mov r0`,
 * and plain ascending. __Func_80925cc is called twice and both want `mov r1`
 * before `mov r0`. Each site was transcribed from the listing rather than
 * copied from the previous one that worked, which is the only reason this
 * screened clean first time.
 *
 * __Func_80933f8 takes four arguments and the fourth is pinned along with the
 * others -- anchor every argument of a call you anchor any argument of.
 */
extern int OvlFunc_890_200a5b0(void);
extern void OvlFunc_890_200a5fc(int a, int b);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int n);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093530(void);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_890_2009a58(void)
{
    unsigned char *p;
    register int p0 __asm__("r0");

    p0 = 0x81; p0 <<= 4;
    if (__GetFlag(p0) == 0) {
        if (OvlFunc_890_200a5b0() != 0) {
            __CutsceneStart();
            { PIN3; q2 = 0x93; q0 = 0x10; q1 = 0x2410000; q2 <<= 16;
              __MapActor_SetPos(q0, q1, q2); }
            { PIN3; q1 = 0x80; q0 = 0x10; q1 <<= 7; q2 = 1;
              __Func_8092adc(q0, q1, q2); }
            { PIN4; q1 = 1; q2 = 0xb8; q3 = 1; q1 = -q1; q2 <<= 16; q0 = 0x23e0000;
              __Func_80933f8(q0, q1, q2, q3); }
            __MessageID(0x1027);
            { PIN3; q1 = 0x90; q2 = 0xe8; q0 = 0; q1 <<= 2;
              __Func_80921c4(q0, q1, q2); }
            { PIN2; q1 = 0; q0 = 0; __MapActor_SetAnim(q0, q1); }
            __Func_8093530();
            __CutsceneWait(0xa);
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x10; q1 <<= 9; q2 <<= 8;
              __MapActor_SetSpeed(q0, q1, q2); }
            { PIN3; q1 = 0x90; q1 <<= 2; q2 = 0x98; q0 = 0x10;
              __Func_80921c4(q0, q1, q2); }
            __CutsceneWait(6);
            { PIN3; q2 = 0x1e; q0 = 0x10; q1 = 6; __MapActor_Jump(q0, q1, q2); }
            OvlFunc_890_200a5fc(0x10, 6);
            { PIN2; q1 = 3; q0 = 0; __MapActor_DoAnim(q0, q1); }
            __CutsceneWait(2);
            __MapActor_DoAnim(0x10, 4);
            OvlFunc_890_200a5fc(0x10, 6);
            { PIN2; q1 = 0x81; q1 <<= 1; q0 = 0; __MapActor_Surprise(q0, q1); }
            __CutsceneWait(0x28);
            { PIN2; q1 = 2; q0 = 0x10; __Func_80925cc(q0, q1); }
            __CutsceneWait(0x1e);
            OvlFunc_890_200a5fc(0x10, 6);
            __MapActor_DoAnim(0, 3);
            { PIN3; q1 = 0x90; q2 = 0xb8; q1 <<= 2; q0 = 0x10;
              __Func_80921c4(q0, q1, q2); }
            __CutsceneWait(6);
            { PIN2; q1 = 2; q0 = 0x10; __Func_80925cc(q0, q1); }
            __CutsceneWait(0x28);
            OvlFunc_890_200a5fc(0x4010, 6);
            { PIN3; q1 = 0x90; q2 = 0xd0; q1 <<= 2; q0 = 0x10;
              __Func_80921c4(q0, q1, q2); }
            __CutsceneWait(0x28);
            { PIN2; q1 = 3; q0 = 0; __MapActor_DoAnim(q0, q1); }
            __CutsceneWait(6);
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x10; q1 <<= 8; q2 <<= 7;
              __MapActor_SetSpeed(q0, q1, q2); }
            __MapActor_SetAnim(0x10, 2);
            p = __MapActor_GetActor(0);
            if (p != 0)
                __MapActor_TravelTo(0x10, *(short *)(p + 0xa),
                                    *(short *)(p + 0x12));
            __MapActor_WaitMovement(0x10);
            __MapActor_SetPos(0x10, 0, 0);
            p0 = 0x81; p0 <<= 4;
            __SetFlag(p0);
            __CutsceneEnd();
        }
    }
}
