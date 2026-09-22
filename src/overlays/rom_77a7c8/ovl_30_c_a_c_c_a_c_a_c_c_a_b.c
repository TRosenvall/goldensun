/* OvlFunc_881_200acb4 -- 397 instructions, 1148 bytes, byte-identical (433
 * encodings, 112 relocations).  The largest function landed in batch 280 and
 * the largest in this overlay.  Split out of
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_a.s; its file-mate
 * OvlFunc_881_200a8e8 stays in asm (parked at 5 of 381, sched2 priority).
 *
 * SYMBOL TELL, ALREADY PROVISIONED.  `ldr r0, =0` feeding __SetDestMap is a
 * pooled ZERO -- the strongest form of area.sym's criterion.  _AREA_00 = 0x0
 * has been in area.sym since batch 68; nothing new was needed.
 *
 * PINNING AND UN-PINNING ARE BOTH LEVERS AND THE LADDER MUST TEST BOTH
 * DIRECTIONS.  This is the finding of the function, and it cost three of the
 * four rounds to see:
 *
 *   129 -> 18   PINNING 31 repeated-pool-constant sites in one step.
 *    18 -> 6    UN-PINNING q1 at nine of them, so the mov/lsl pair stays
 *               expanded and sched2 can slot the pinned `mov r0` BETWEEN the
 *               mov and the lsl.  Pinning the pair keeps it adjacent and the
 *               r0 fill lands after it.
 *     6 -> 0    REMOVING the last pin block entirely.  The second
 *               __Func_80933f8(0x1e580000, -1, 0xdc80000, 1) is byte-exact
 *               written PLAIN, while every pinned spelling of it (five orders
 *               tried) leaves `negs r1` and `ldr r2` transposed.
 *
 * A PIN THAT IS INERT IS NOT FREE -- ONE OF THEM WAS ACTIVELY WRONG.  Two
 * greedy rounds found 8 individually-inert pins, but dropping all 8 jointly
 * broke it (8 differing) -- the rom_7d95dc shape.  A proper greedy kept 4 of
 * the 8.
 *
 * ELEVEN-ARGUMENT CALLS: NAME THE STACK ARGUMENTS.  __Func_80931ec with
 * eleven literals emits seven mov/str pairs alternating through r3 (anti-
 * dependences pin the order).  With v1..v7 as named locals each constant is a
 * pseudo, local-alloc gives each its own register, the two 4s share one by
 * CSE, and sched2 groups all the movs ahead of all the strs -- the ROM's
 * shape, including its use of r4 and r5.  Worth 20 differing; nothing else
 * reached it.
 *
 * WHY THE PIN COUNT IS 25 AND THAT IS NOT A DEFECT IN THE READING.  This is a
 * long straight-line cutscene script, so the same emotion id / speed pair /
 * coordinate recurs 2-5 times inside a SINGLE basic block (calls do not end a
 * block).  gcc commons every one; the ROM commons none.  CSE OF REPEATED POOL
 * CONSTANTS accounted for 111 of the original 129 differences, and argument
 * pins are its only cure in this tree's idiom.  The pin count scales with the
 * script's repetition.
 */
extern unsigned char gState[];
extern unsigned char iwram_3001ebc[];
extern int _AREA_00;
extern unsigned char gScript_881__0200cf7c[];
extern unsigned char gScript_881__0200d01c[];
extern unsigned char gScript_881__0200d0a8[];

extern void __SetUIColor(int a, int b);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __MapTransitionIn(void);
extern void __SetDestMap(int map, int entrance);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_Surprise(int slot, int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int n);
extern void __Func_808c44c(void);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_801173c(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80931ec(int a, int b, int c, int d, int e, int f, int g,
                           int h, int i, int j, int k);
extern void __Func_8093710(void);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_80936a0(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_881_200acb4(void)
{
    unsigned char *a;
    int v1, v2, v3, v4, v5, v6, v7;

    __SetUIColor(gState[0x205], gState[0x206]);
    __CutsceneStart();
    __Func_80936a0(0x80 << 9, 0x96 << 1);
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
    __MapActor_SetAnim(5, 0x13);
    __MapActor_SetAnim(8, 5);
    __MapActor_SetPos(0, 0, 0);
    __WaitFrames(1);
    __Func_80936a0(0xc0 << 9, 0x10);
    *(int *)(*(unsigned char **)iwram_3001ebc + (0xe0 << 1)) = 0x80 << 1;
    __Func_8091200(0x10003, 1);
    *(int *)(*(unsigned char **)iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __MapTransitionIn();
    __Func_8093710();
    __Func_808c44c();
    __CutsceneWait(0x28);
    __Func_80925cc(5, 1);
    __CutsceneWait(0x14);
    __MessageID(0x2913);
    __Func_8093040(5, 0, 0x14);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    { PIN3; q0 = 5; q1 = 0x107; q2 = 0x14; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(5, 0, 0x14);
    { PIN3; q0 = 8; q1 = 0x105; q2 = 0x50; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(8, 0, 0xa);
    __Func_80925cc(5, 2);
    __Func_8093040(5, 0, 0x14);
    { PIN3; q0 = 8; q1 = 0x105; q2 = 0x64; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x105; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(5, 0, 0x14);
    __Func_8093040(8, 0, 0xa);
    { PIN3; q0 = 5; q1 = 0x81 << 1; q2 = 0x14; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(5, 0, 0xa);
    __MapActor_Surprise(8, 0x81 << 1);
    __CutsceneWait(0x50);
    __Func_8093040(8, 0, 0x14);
    { PIN3; q0 = 5; q1 = 0x105; q2 = 0x50; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(8, 0, 0x78);
    __Func_80925cc(5, 1);
    __Func_8093040(5, 0, 0x28);
    __Func_8093040(8, 0, 0x14);
    { PIN3; q0 = 5; q1 = 0x105; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(5, 0, 0x78);
    { PIN3; q0 = 9; q1 = 0x6666; q2 = 0x3333; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x1ddc0000; q2 = 0xd840000; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x1d94; q2 = 0xd8c; __Func_80921c4(q0, q1, q2); }
    __Func_80921c4(9, 0x1d88, 0xda << 4);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x6009; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x101; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0x3c; q0 = 5; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    __MapActor_DoAnim(9, 3);
    __ActorMessage(0x6009, 0);
    __Func_801173c();
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(9, gScript_881__0200cf7c);
    __CutsceneWait(0x50);
    __MapActor_SetAnim(8, 1);
    __MapActor_Jump(8, 4, 0x28);
    __MapActor_SetAnim(5, 1);
    __MapActor_Jump(5, 4, 0x3c);
    { PIN1; q0 = 8; __Func_8092adc(q0, 0xc0 << 6, 0); }
    { PIN1; q0 = 5; __Func_8092adc(q0, 0xb0 << 8, 0x28); }
    { PIN3; q0 = 8; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x4ccc; q0 = 5; q1 = 0x9999; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(8, gScript_881__0200d01c);
    __CutsceneWait(0x14);
    __Func_80933d4(0xb333, 0x1666);
    __Func_80933f8(0x1e380000, -1, 0xdc80000, 1);
    __MapActor_SetBehavior(5, gScript_881__0200d0a8);
    do {
        __MapActor_SetAnim(0xa, 6);
        __MapActor_SetAnim(6, 8);
        __WaitFrames(1);
        a = __MapActor_GetActor(5);
    } while (*(short *)(a + 0x64) == 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 8; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise(5, 0x81 << 1);
    __CutsceneWait(0x28);
    __Func_808c44c();
    v1 = 0xc; v2 = 8; v3 = 9; v4 = 4; v5 = 4; v6 = 3; v7 = 0;
    __Func_80931ec(5, 7, 0xd, 2, v1, v2, v3, v4, v5, v6, v7);
    __CutsceneWait(0x14);
    __Func_801173c();
    __Func_80933d4(0x80 << 9, 0x80 << 6);
    __Func_80933f8(0x1e580000, -1, 0xdc80000, 1);
    { PIN1; q0 = 9; __Func_8092adc(q0, 0xc0 << 6, 0); }
    { PIN3; q0 = 8; q1 = 0x19999; q2 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x19999; q2 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN2; q0 = 8; q1 = 0x1e7c; __Func_809218c(q0, q1, 0xdb8); }
    { PIN1; q0 = 5; __Func_80921c4(q0, 0x1e6c, 0xdd8); }
    __MapActor_SetAnim(8, 1);
    __Func_808c44c();
    __CutsceneWait(0x50);
    __Func_80925cc(8, 1);
    __Func_8093040(8, 0, 0x14);
    __Func_80925cc(5, 2);
    __Func_8093040(0x1005, 0, 0x28);
    { PIN3; q0 = 8; q1 = 0x80 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_809259c(8, 2);
    __Func_8093040(8, 0, 0x3c);
    __Func_801173c();
    __PlaySound(0x11);
    __Func_8091200(0, 0);
    __Func_8091254(0x78);
    __WaitFrames(0x78);
    __SetDestMap((int)&_AREA_00, 0xa);
}
