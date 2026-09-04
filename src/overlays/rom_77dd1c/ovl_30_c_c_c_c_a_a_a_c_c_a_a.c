// fakematch
/* OvlFunc_882_2009828  --  0x02009828
 *
 * Was the whole of goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_a_a.s,
 * so no split was needed.
 *
 * 132 instructions behind one save flag. Twenty-two pinned call sites, one
 * crossed __MapActor_SetSpeed taking a barrier, and two named callee-saved
 * locals -- the message base in r5 and a shifted constant in r6 that is stored
 * to an actor field and then passed as an argument nine calls later.
 *
 * THE MESSAGE BASE NEEDS THE PIN HERE, and the contrast with
 * src/overlays/rom_7c6bac/ovl_30_c_c_a_c_a_c.c from the previous batch is the
 * point. There the base was a LINKER SYMBOL and `m + 1` derived unaided,
 * because a symbol address is not something constant propagation can fold into
 * separate pool entries. Here it is the plain integer 0xe74 with `m += 5`, so
 * without `register int m __asm__("r5")` cprop folds both uses into their own
 * pool words and the register is never taken. The spelling of the id decides
 * whether the lever is needed; read what the ROM pools.
 *
 * ONE WALL, NOT TWO. With m pinned, `ldr r5, =0xe74` was scheduled one
 * statement early into __MapActor_Surprise's argument group. A single
 * `do { } while (0)` behind it is exact; bracketing the call with two also
 * matches, so the minimal form is kept. That is the batch-207 rule holding: the
 * load crossed ONE statement, so one wall is enough, and the second is only
 * needed when it crossed two.
 */
extern unsigned char gScript_882__0200c934[];
extern unsigned char gScript_882__0200c984[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int n);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_RunScript(int slot, int s);
extern void __MapActor_SetBehavior(int slot, int s);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_80917d0(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093054(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_882_2009828(void)
{
    unsigned char *p;
    register int p0 __asm__("r0");
    register int m __asm__("r5");
    register int w __asm__("r6");

    p0 = 0x837;
    if (__GetFlag(p0) == 0) {
        __CutsceneStart();
        { PIN2; q1 = 0x80; q1 <<= 1; q0 = 0x16; __MapActor_Surprise(q0, q1); }
        do { } while (0);
        m = 0xe74;
        __MessageID(m);
        __ActorMessage(0x16, 0);
        { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0x14;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q1 = 0x80; q2 = 0; q0 = 0; q1 <<= 7;
          __Func_8092adc(q0, q1, q2); }
        __Func_80933d4(0x6666, 0xccc);
        { PIN4; q0 = 0x80; q1 = 1; q2 = 0x93; q3 = 1; q0 <<= 17; q1 = -q1; q2 <<= 18;
          __Func_80933f8(q0, q1, q2, q3); }
        {
            PIN3;
            q1 = 0x80; __asm__ volatile ("" : : "r" (q1));
            q2 = 0x80; q2 <<= 9; q0 = 0x16; q1 <<= 10;
            __MapActor_SetSpeed(q0, q1, q2);
        }
        { PIN2; q1 = (int)gScript_882__0200c934; q0 = 0x16;
          __MapActor_RunScript(q0, q1); }
        { PIN3; q2 = 0; q1 = 0x16; q0 = 0; __Func_8092848(q0, q1, q2); }
        __CutsceneWait(0x1e);
        { PIN2; q1 = (int)gScript_882__0200c984; q0 = 0x16;
          __MapActor_SetBehavior(q0, q1); }
        { PIN2; q1 = 0; q0 = 0x16; __ActorMessage(q0, q1); }
        w = 0x80;
        w <<= 9;
        *(int *)(__MapActor_GetActor(0x16) + 0x1c) = w;
        { PIN2; q1 = 1; q0 = 0x16; __Func_80925cc(q0, q1); }
        __CutsceneWait(0x14);
        { PIN2; q1 = 0; q0 = 0x16; __Func_8093054(q0, q1); }
        __CutsceneWait(0x28);
        m += 5;
        { PIN2; q1 = 1; q0 = 0x16; __Func_80925cc(q0, q1); }
        __MessageID(m);
        { PIN3; q2 = 0x14; q0 = 0x16; q1 = 0; __Func_8093040(q0, q1, q2); }
        __MapActor_DoAnim(0, 3);
        __MapActor_DoAnim(0x16, 3);
        __ActorMessage(0x16, 0);
        { PIN3; q2 = 0x80; q0 = 0x16; q1 = w; q2 <<= 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetAnim(0x16, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(0x16, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(0x16);
        { PIN3; q2 = 0; q0 = 0x16; q1 = 0; __MapActor_SetPos(q0, q1, q2); }
        __Func_80917d0(1, 1);
        __MapActor_SetAnim(0x15, 3);
        p0 = 0x837;
        __SetFlag(p0);
        __CutsceneEnd();
    }
}
