/* Cluster OvlFunc_899_2009ba0..OvlFunc_899_2009ba0 extracted from
 * goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_a_c.s -- ONE function in its .s, no split.
 *
 * Requires _MSG_1299; see message.sym, which also records that objcmp cannot see a linker-script
 * definition (it assembles the candidate directly), so verifying this class needs a top-level
 * `.equ` shim rather than the bind-mount trick that works for tryc.
 *
 * 271 instructions. Never attempted before batch 280. FIFTEEN pin sites at greedy-drop fixpoint, down from 26 -- one fakematch row. Leaving
 * `__Func_8092c40` undeclared removed one more. No flags.
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
struct Actor { unsigned char pad00[0x64]; short f64; };
#include "message.h"
extern int iwram_3001ebc;
extern int _MSG_1299;

extern unsigned char gScript_899__0200d650[];
extern unsigned char gScript_899__0200d678[];
extern unsigned char gScript_899__0200d768[];

extern void OvlFunc_899_200aba0(void);
extern void OvlFunc_899_200c5f4(int a, int b);
extern void OvlFunc_899_200c60c(int a, int b, int c);
extern void OvlFunc_899_200c624(int a, int b, int c);
extern void OvlFunc_899_200c63c(int a, int b, int c);

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __StopTask(void (*fn)(void));
extern void __ClearFlag(int f);
extern void __MapActor_SetBehavior(int a, unsigned char *s);
extern void __MapActor_SetSpeed(int a, int b, int c);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __MapActor_SetAnim(int a, int b);
extern void __MapActor_WaitMovement(int a);
extern void __MapActor_Emote(int a, int b, int c);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_899_2009ba0(void)
{
    struct Actor *a;
    struct Actor *b;
    char *p;
    int msg;
    int s1;
    int s2;

    a = __MapActor_GetActor(0x18);
    b = __MapActor_GetActor(0x19);
    __CutsceneStart();
    __StopTask(OvlFunc_899_200aba0);
    __ClearFlag(0xc0 << 2);
    if (a->f64 <= 3)
        __MapActor_SetBehavior(0x18, gScript_899__0200d678);
    else
        __MapActor_SetBehavior(0x18, gScript_899__0200d650);
    if (b->f64 <= 2)
        __MapActor_SetBehavior(0x19, gScript_899__0200d768);
    else
        __MapActor_SetBehavior(0x19, gScript_899__0200d650);
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xb6; q0 = 0; q1 = 0xf8; q2 <<= 2; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xf8; q2 = 0xb6; q0 = 2; q1 <<= 16; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xf8; q2 = 0xb6; q0 = 1; q1 <<= 16; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x84; q2 = 0xba; q0 = 2; q1 <<= 1; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    { PIN3; q2 = 0xba; q1 = 0xe8; q2 <<= 2; q0 = 1; __Func_80921c4(q0, q1, q2); }
    __MapActor_WaitMovement(2);
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    __Func_809280c(1, 0, 0);
    OvlFunc_899_200c60c(2, 0, 0x1e);
    __Func_80925cc(2, 1);
    msg = (int)(&_MSG_1299);
    __MessageID(msg);
    __ActorMessage(2, 0);
    __Func_809280c(0, 2, 0);
    OvlFunc_899_200c60c(1, 2, 0x14);
    __MapActor_SetAnim(0, 3);
    OvlFunc_899_200c63c(1, 3, 0x14);
    OvlFunc_899_200c624(0, 1, 0xa);
    __Func_8092c40(1, 0);
    if (__Func_8091c7c(0, 0) != 0) {
        p = (char *)iwram_3001ebc;
        p += 0xec << 1;
        ++*(unsigned short *)p;
    }
    OvlFunc_899_200c5f4(1, 0x1e);
    __MessageID(msg + 4);
    __Func_809280c(0, 2, 0);
    OvlFunc_899_200c60c(1, 2, 0x32);
    __MapActor_Emote(2, 0x80 << 1, 0);
    __CutsceneWait(0x3c);
    OvlFunc_899_200c624(0, 1, 0x32);
    __Func_809280c(0, 2, 0);
    OvlFunc_899_200c60c(1, 2, 0x1e);
    OvlFunc_899_200c63c(2, 3, 0xa);
    OvlFunc_899_200c5f4(2, 0x14);
    { PIN3; q1 = 0x81; q0 = 0; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(1, 0x81 << 1, 0);
    __CutsceneWait(0x3c);
    OvlFunc_899_200c63c(2, 3, 0x14);
    OvlFunc_899_200c5f4(2, 0x1e);
    __MessageID(0x129f);
    __ActorMessage(1, 0);
    __Func_8092adc(0, 0x80 << 7, 0);
    __CutsceneWait(0x28);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    OvlFunc_899_200c63c(2, 3, 0x32);
    { PIN3; q2 = 0xb6; q0 = 2; q1 = 0xf8; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    { PIN3; q2 = 0xb6; q0 = 1; q1 = 0xf8; q2 <<= 2; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetPos(2, 0, 0);
    { PIN3; q1 = 0xd0; q2 = 0xae; q0 = 0x18; q1 <<= 15; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xf0; q2 = 0xae; q0 = 0x19; q1 <<= 15; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    __Func_8092adc(0x18, 0, 0);
    { PIN3; q1 = 0x80; q0 = 0x19; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    s1 = 0xe;
    s2 = 0x2c;
    __Func_8010704(0xe, 0x32, 3, 1, s1, s2);
    __CutsceneEnd();
}
