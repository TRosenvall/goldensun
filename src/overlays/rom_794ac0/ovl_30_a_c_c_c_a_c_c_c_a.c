/* Cluster OvlFunc_899_2009f50..OvlFunc_899_200a1c8 extracted from
 * goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_c_c.s.
 *
 * THIS FILE CONVERTS WHOLE: the `.s` held exactly these two functions and both matched, so no split
 * was needed and no linker line changed. Verified OBJECT-WIDE as one TU against the whole reference
 * -- 1556 = 1556 bytes, 609 = 609 encoding lines with ZERO differing, 156 = 156 relocations -- and
 * each function also verified separately against its own filtered reference.
 *
 * 235 and 356 instructions. Never attempted before batch 280. THREE pin sites on the first function and ELEVEN on the second, both at greedy-drop fixpoint
 * (the ladders dropped 9 of 12 and 21 of 32) -- two fakematch rows. No flags.
 *
 * ===== THE ROM'S SAME CONSTANT TWICE IN TWO COMPARISONS MEANS A `switch`, NOT AN `if` CHAIN =====
 *
 * This is the load-bearing find of the group and it is new. The ROM has `cmp #0xc9 / beq` and then
 * `cmp #0xc9 / blt` -- THE SAME CONSTANT IN BOTH -- and that cannot come from a comparison chain,
 * because gcc canonicalises `>= 0xc9` and `< 0xc9` BOTH to `#0xc8` with `bgt`/`ble`.
 *
 * `emit_case_nodes` emits the node value VERBATIM and prunes the low bound of the following range,
 * which gives `beq / blt / bgt #0xcb` exactly. Written as a `switch` with a deliberate
 * fall-through -- `case 0xca: case 0xcb: ... if (v == 0xca) break; case 0xc9: ...` -- it went 2
 * differing to 0.
 *
 * SO: AN UNCANONICALISED CONSTANT IN A COMPARISON IS A `switch` TELL. If the ROM compares against
 * N and also branches on N rather than N-1, no `if` chain will reproduce it.
 *
 * ===== AND ONE RECORDED RULE THAT DOES NOT GENERALISE =====
 *
 * Batch 212 records "shifts in the ROM's mov order". Where a PIN4 was needed here, the order that
 * works is ASCENDING q0,q1,q2,q3 -- the ROM's own mov order (q1,q0,q2,q3) measures 2 differing. So
 * that rule holds only when the ROM's mov order is itself unpermuted; when it is permuted, ascending
 * wins. Read it as "try both", which is also what batch 280's descending-fill finding says.
 *
 * Two levers cut the pin count here rather than adding to it: leaving `__Func_8092c40` UNDECLARED
 * (the tree's documented ROM-fills-r0-last idiom) removes three pins outright, and one four-register
 * crossed fill needs NO PIN4 AT ALL -- plain C gets it.
 */
struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 2,
                  b2 : 2,
                  b4 : 4;
};

struct Actor {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Sprite *f50;
    unsigned char pad54[0x64 - 0x54];
    short f64;
};

extern int iwram_3001ebc;

extern void OvlFunc_899_200aba0(void);
extern void OvlFunc_899_200c8c8(void);
extern void OvlFunc_899_200a1c8(void);
extern void OvlFunc_899_200c5f4(int a, int b);
extern void OvlFunc_899_200c60c(int a, int b, int c);
extern void OvlFunc_899_200c624(int a, int b, int c);
extern void OvlFunc_899_200c63c(int a, int b, int c);
extern void OvlFunc_899_200c658(int a, int b);
extern void OvlFunc_899_200c684(void);

extern struct Actor *__MapActor_GetActor(int slot);
extern void __PlayMapMusic(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __StopTask(void (*fn)(void));
extern int __StartTask(void (*fn)(void), int n);
extern void __ClearFlag(int f);
extern void __SetFlag(int f);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __SetCameraTarget(int a, int b);
extern void __Func_8093530(void);
extern void __Func_800fe9c(void);
extern void __Func_8092b08(int a, int b);
extern void __Func_80935b0(int a, int b, int c, int d);
extern void __MapActor_SetSpeed(int a, int b, int c);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __MapActor_SetAnim(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);
extern void __MapActor_Surprise(int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_8097adc(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_899_2009f50(void)
{
    char *p;
    char *q;
    int who;
    int s1;
    int s2;

    p = (char *)iwram_3001ebc;
    __CutsceneStart();
    __StopTask(OvlFunc_899_200c8c8);
    __ClearFlag(0x107);
    __ClearFlag(0x94 << 2);
    __MapActor_SetAnim(0x18, 1);
    __MapActor_SetAnim(0x19, 1);
    __MapActor_SetAnim(2, 1);
    __MapActor_SetAnim(0, 1);
    __MapActor_SetAnim(1, 1);
    __Func_809280c(1, 2, 0);
    __Func_8092848(0, 2, 0);
    __Func_809280c(0x18, 2, 0);
    __Func_809280c(0x19, 2, 0);
    __CutsceneWait(0xa);
    p += 0xc1 << 1;
    who = 0x18;
    switch (*(short *)p) {
    case 0xca:
    case 0xcb:
        __MessageID(0x12a4);
        { PIN2; q1 = 0x81; q0 = 0x19; q1 <<= 1; __MapActor_Surprise(q0, q1); }
        __Func_80925cc(0x19, 2);
        OvlFunc_899_200c5f4(0x19, 0x14);
        who = 0x19;
        if (*(short *)p == 0xca)
            break;
    case 0xc9:
        __MessageID(0x12a3);
        { PIN2; q1 = 0x81; q0 = 0x18; q1 <<= 1; __MapActor_Surprise(q0, q1); }
        __Func_80925cc(0x18, 2);
        who = 0x18;
        OvlFunc_899_200c5f4(0x18, 0x14);
        break;
    }
    __MapActor_SetAnim(2, 1);
    __Func_809280c(2, who, 0);
    __Func_809280c(1, 2, 0);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __MessageID(0x12a5);
    OvlFunc_899_200c5f4(1, 0x14);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(2, 0x14);
    OvlFunc_899_200c658(2, who);
    __Func_8097adc();
    __CutsceneWait(0x3c);
    __Func_80925cc(0x18, 2);
    OvlFunc_899_200c5f4(0x18, 0x14);
    __Func_80925cc(0x19, 2);
    OvlFunc_899_200c5f4(0x19, 0x14);
    { PIN3; q1 = 0x80; q2 = 0x3c; q0 = 2; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    OvlFunc_899_200c684();
    __Func_809259c(1, 2);
    __Func_80925cc(0, 2);
    __CutsceneWait(0x14);
    OvlFunc_899_200c5f4(1, 0x14);
    __Func_809280c(2, 1, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x28);
    __MapActor_GetActor(0)->f50->b2 = 1;
    __MapActor_GetActor(1)->f50->b2 = 1;
    __MapActor_GetActor(2)->f50->b2 = 1;
    q = (char *)iwram_3001ebc;
    *(int *)(q + (0xe4 << 1)) = 0x18;
    *(int *)(q + (0xe0 << 1)) = 0x201;
    __MapTransitionOut();
    __WaitMapTransition();
    OvlFunc_899_200a1c8();
    s1 = 0xe;
    s2 = 0x2c;
    __Func_8010704(0xe, 0x2d, 3, 1, s1, s2);
    __SetFlag(0x853);
    __MapActor_GetActor(0x18)->f64 = 5;
    __MapActor_GetActor(0x19)->f64 = 4;
    __StartTask(OvlFunc_899_200aba0, 0xc8 << 4);
    *(int *)((char *)iwram_3001ebc + (0xe0 << 1)) = 0x209;
    __CutsceneEnd();
}

void OvlFunc_899_200a1c8(void)
{
    char *p;
    int done;

    __PlayMapMusic();
    __Func_8092b08(0, 1);
    __MapActor_GetActor(0)->f23 |= 1;
    __Func_80935b0(0x80 << 14, 0x90 << 18, 0xc8 << 17, 0xea << 18);
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xf8; q2 = 0xb6; q0 = 0; q1 <<= 16; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x84; q2 = 0xba; q0 = 2; q1 <<= 17; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetPos(1, 0xe8 << 16, 0xba << 18);
    __MapActor_GetActor(0)->f50->b2 = 1;
    __MapActor_GetActor(1)->f50->b2 = 1;
    __MapActor_GetActor(2)->f50->b2 = 1;
    __Func_8092848(0, 2, 0);
    OvlFunc_899_200c60c(1, 2, 0x1e);
    { PIN3; q1 = 0xd0; q2 = 0xae; q0 = 0x18; q1 <<= 15; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xf0; q2 = 0xae; q0 = 0x19; q1 <<= 15; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    __Func_8092848(0x18, 0x19, 0);
    __SetCameraTarget(0, 0);
    __Func_8093530();
    __Func_800fe9c();
    __CutsceneWait(0x1e);
    p = (char *)iwram_3001ebc;
    *(int *)(p + (0xe4 << 1)) = 0x18;
    *(int *)(p + (0xe0 << 1)) = 0x201;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __Func_80925cc(1, 1);
    __CutsceneWait(0xa);
    __MessageID(0x12ae);
    OvlFunc_899_200c5f4(1, 0x14);
    OvlFunc_899_200c63c(2, 3, 0x14);
    OvlFunc_899_200c5f4(2, 0x14);
    OvlFunc_899_200c624(0, 1, 0x32);
    __Func_809280c(0, 2, 0);
    OvlFunc_899_200c60c(1, 2, 0x14);
    OvlFunc_899_200c63c(2, 4, 0x14);
    __ActorMessage(2, 0);
    { PIN3; q1 = 0x81; q0 = 0; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(1, 0x81 << 1, 0);
    __CutsceneWait(0x3c);
    OvlFunc_899_200c63c(2, 3, 0x14);
    OvlFunc_899_200c5f4(2, 0x1e);
    __Func_80925cc(1, 1);
    __CutsceneWait(0x14);
    OvlFunc_899_200c624(0, 1, 0x14);
    __Func_8092c40(1, 0);
    done = 0;
    if (__Func_8091c7c(0, 0) != 0) {
        __CutsceneWait(0x14);
        __Func_809259c(2, 2);
        OvlFunc_899_200c63c(1, 4, 0x1e);
        __Func_8092c40(1, 0);
        if (__Func_8091c7c(0, 0) != 0) {
            __CutsceneWait(0x14);
            { PIN3; q1 = 0x81; q2 = 0; q1 <<= 1; q0 = 2; __MapActor_Emote(q0, q1, q2); }
            __CutsceneWait(0x3c);
            __Func_80925cc(2, 2);
            OvlFunc_899_200c624(0, 2, 0x14);
            __Func_8092c40(2, 0);
            if (__Func_8091c7c(2, 0) != 0) {
                __CutsceneWait(0x14);
                __MapActor_Emote(2, 0x105, 0);
                __CutsceneWait(0x3c);
                OvlFunc_899_200c5f4(2, 0x14);
                OvlFunc_899_200c60c(1, 2, 0xa);
                __Func_80925cc(1, 1);
                __CutsceneWait(0xa);
                OvlFunc_899_200c5f4(1, 0xa);
                OvlFunc_899_200c60c(2, 1, 0x14);
                __MapActor_Emote(2, 0x101, 0);
                __CutsceneWait(0x3c);
                OvlFunc_899_200c63c(2, 4, 0x14);
                OvlFunc_899_200c5f4(1, 0xa);
                OvlFunc_899_200c624(1, 0, 0x14);
                OvlFunc_899_200c63c(1, 3, 0x14);
                done = 1;
                OvlFunc_899_200c5f4(1, 0x14);
            }
        }
    }
    if (done == 0) {
        __MessageID(0x12bc);
        OvlFunc_899_200c5f4(1, 0x14);
        OvlFunc_899_200c624(1, 0, 0x14);
        OvlFunc_899_200c5f4(1, 0x14);
    }
    __MapActor_Emote(0, 0x105, 0);
    __CutsceneWait(0x3c);
    __Func_80925cc(1, 1);
    OvlFunc_899_200c5f4(1, 0xa);
    OvlFunc_899_200c63c(1, 3, 0xa);
    __Func_8092848(1, 2, 0);
    OvlFunc_899_200c624(0, 2, 0xa);
    __ActorMessage(1, 0);
    OvlFunc_899_200c63c(2, 3, 0xa);
    { PIN3; q2 = 0xb6; q0 = 2; q1 = 0xf8; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    { PIN3; q2 = 0xb6; q0 = 1; q1 = 0xf8; q2 <<= 2; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetPos(2, 0, 0);
}
