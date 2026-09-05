// fakematch
/* OvlFunc_952_20097e8  --  0x020097e8
 * [asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c.s, third of five functions]
 *
 * 806 instructions. Byte-exact: 2092 bytes, 811 encodings and 219 relocations
 * identical.
 *
 * NO CONSTANT-CSE AT ALL, WHICH IS UNUSUAL AT THIS SIZE and is worth checking
 * before reaching for the class. Plain C is 806 lines against 806 -- the length
 * matches from the start -- and all 674 differing lines are argument-order
 * transpositions. The wide `push {r5, r6, r7, lr}` is misleading by content
 * too: only r7 is a genuine named local, while r5 is a scratch slot recycled
 * FIVE times and r6 three times.
 *
 * A PARTIAL REORDER IS A PRIORITY TIE, NOT A BARRIER PROBLEM, and that is the
 * finding here. After one halfword store the ROM emits the next `strh` BEFORE
 * its `ldr`, while still hoisting the address load above both -- a partial
 * reorder. With two dependent stores the load outranks the strh; with one they
 * tie and the lower LUID wins. So the cure is to wrap ONLY THE SECOND store in
 * `do { } while (0);`. Every whole-region spelling -- a bare barrier between
 * the stores, one wrapping the first store, one wrapping both -- costs 696
 * differing, because ending the region entirely is the wrong shape.
 *
 * THE ELEMENT-TYPE FOLD LEVER, SECOND FORM AGAIN. The ROM reads a second global
 * as `ldr r7, [r5, #0x10]` off one pooled address. `extern unsigned char
 * *iwram_3001ebc[];` indexed [0] and [4] gives exactly that; declaring the
 * neighbour as its own extern, which is how the rest of the tree spells it,
 * costs a pool word and a load. Same shape as Func_8097384.
 *
 * FLAG PLACEMENT IS SOURCE ORDER: the assignment goes at the START of each arm,
 * not the end, and LUID ties then put the two `mov` instructions exactly where
 * the ROM has them. Moving them to the end costs 2.
 *
 * Both __Func_8092c40 sites want the DESCENDING fill -- the fourth and fifth
 * functions in the corpus where that callee is the lone descending survivor.
 *
 * Thirty pins from 69 candidates at a fixpoint, re-verified in the file's final
 * shape after `volatile` was stripped. TWO PIECES OF SCAFFOLDING WERE STRIPPED
 * as byte-identical: a `volatile` on a global read, and a named pointer for two
 * repeated loads that gcc commons itself.
 *
 * A _MSG_ SYMBOL WAS CONSIDERED AND REFUTED: 0x228c is not a shifted byte, so
 * gcc pools it unaided and there is no length tell, and it has no message.sym
 * entry. The symbol spelling costs 1 differing.
 */
struct S {
    unsigned char pad00[0x52a];
    unsigned short f52a;
    unsigned char pad52c[8];
    unsigned short f534;
    unsigned short f536;
};

extern unsigned char *iwram_3001ebc[];
extern int gKeyHeld;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_Jump(int a, int b, int c);
extern void __ActorMessage(int a, int b);
extern void __Func_808fe38(int n);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_952_20097e8(void)
{
    struct S *b;
    unsigned char *a;
    int i;
    int f;

    __CutsceneStart();
    __MessageID(0x228c);
    __MapActor_SetAnim(0, 0x1f);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    { PIN3; q0 = 1; q1 = 0xd0 << 15; q2 = 0xd0 << 15;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xb0 << 15; q2 = 0xf0 << 15;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xf0 << 15; q2 = 0xf0 << 15;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(3, 0, 0);
    { PIN3; q0 = 2; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    *(int *)(iwram_3001ebc[0] + 0x1c0) = 0x100;
    *(int *)(iwram_3001ebc[0] + 0x1c8) = 0xc;
    __WaitFrames(1);
    __Func_808fe38(9);
    b = (struct S *)iwram_3001ebc[4];
    b->f52a = 0;
    b->f534 = 0x1f1f;
    b->f536 = 1;
    __MapTransitionIn();
    __WaitMapTransition();
    for (i = 1; i <= 5; i++) {
        __WaitFrames(3);
        b->f52a = i;
    }
    __CutsceneWait(0x28);
    __Func_80925cc(0, 2);
    __CutsceneWait(0x1e);
    for (i = 5; i <= 0x1f; i++) {
        __WaitFrames(3);
        b->f52a = i;
    }
    b->f536 = 0x1f;
    *(int *)(iwram_3001ebc[0] + 0x1c0) = 0x209;
    do { *(int *)(iwram_3001ebc[0] + 0x1c8) = 0x18; } while (0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __Func_809280c(1, 0, 0x28);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x1e);
    __CutsceneWait(0xa);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    __ActorMessage(3, 0);
    __Func_8092adc(3, 0xe0 << 8, 0);
    __CutsceneWait(0x1e);
    __Func_80925cc(3, 2);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 3; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0; q2 = -0x28;
      __Func_8092304(q0, q1, q2); }
    __Func_8092304(3, 0x20, 0);
    __Func_8092adc(3, 0x80 << 6, 0);
    __CutsceneWait(0xa);
    __ActorMessage(3, 0);
    *(int *)(__MapActor_GetActor(0) + 0x10) += 0xfffd0000;
    *(int *)(__MapActor_GetActor(0) + 0x40) += 0xfffd0000;
    __MapActor_SetAnim(0, 0x20);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0, 0x22);
    __CutsceneWait(0x1e);
    __MapActor_SetAnim(0, 0x21);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(1, 4);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0x50;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x80 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 2;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(-1, 0) == 0) {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 0x21);
        __CutsceneWait(0x14);
        { PIN3; q0 = 1; q1 = 0x103; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __MapActor_Jump(1, 4, 0xd);
        __MapActor_Jump(1, 4, 0x1e);
        __ActorMessage(1, 0);
        *(unsigned short *)(iwram_3001ebc[0] + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 0x22);
        __CutsceneWait(0x14);
        { PIN3; q0 = 1; q1 = 0x103; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __MapActor_Jump(1, 4, 0xd);
        __MapActor_Jump(1, 4, 0x1e);
        *(unsigned short *)(iwram_3001ebc[0] + (0xec << 1)) += 1;
        __ActorMessage(1, 0);
    }
    __CutsceneWait(0xa);
    __Func_8092adc(2, 0xa0 << 8, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(2, 4);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0x80 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x28);
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(2, 0xe0 << 8, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(3, 0xc0 << 7, 0);
    __CutsceneWait(0x32);
    __Func_8092adc(3, 0x80 << 6, 0);
    __CutsceneWait(0x23);
    __MapActor_Emote(3, 0x84 << 1, 0x32);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    { PIN2; q1 = 0; q0 = 2;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(-1, 0) == 0) {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 0x21);
        __CutsceneWait(0x14);
        { PIN3; q0 = 1; q1 = 0x107; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(1, 0);
        f = 0;
        *(unsigned short *)(iwram_3001ebc[0] + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 0x22);
        __CutsceneWait(0x14);
        { PIN3; q0 = 1; q1 = 0x107; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        *(unsigned short *)(iwram_3001ebc[0] + (0xec << 1)) += 1;
        f = 1;
        __ActorMessage(1, 0);
    }
    __CutsceneWait(0xa);
    __Func_8092adc(2, 0xa0 << 8, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(2, 4);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0x80 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x28);
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(2, 0xe0 << 8, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(3, 0xc0 << 7, 0);
    __CutsceneWait(0x32);
    __Func_8092adc(3, 0x80 << 6, 0);
    __CutsceneWait(0x23);
    __MapActor_Emote(3, 0x84 << 1, 0x32);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(3, 0);
    if (f == 0) {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(2, 3);
        __CutsceneWait(0x1e);
        __ActorMessage(2, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(1, 2);
        __CutsceneWait(0x1e);
        __ActorMessage(1, 0);
        *(unsigned short *)(iwram_3001ebc[0] + (0xec << 1)) += 2;
    } else {
        *(unsigned short *)(iwram_3001ebc[0] + (0xec << 1)) += 2;
        __CutsceneWait(0xa);
        __MapActor_DoAnim(2, 4);
        __CutsceneWait(0x14);
        __ActorMessage(2, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(1, 2);
        __CutsceneWait(0x14);
        __ActorMessage(1, 0);
    }
    __CutsceneWait(0xa);
    __MapActor_Emote(0, 0x81 << 1, 0x3c);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(1, 0);
    __CutsceneWait(0x14);
    __Func_8092848(2, 3, 0x28);
    { PIN3; q0 = 3; q1 = 0x80 << 6; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(2, 0xe0 << 8, 0);
    __CutsceneWait(0x1e);
    while ((gKeyHeld & 0xf0) == 0)
        __WaitFrames(1);
    __MapActor_Jump(0, 6, 0);
    { PIN3; q0 = 0; q1 = 0x1e666; q2 = 0xf333;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(0, -0x20, -8);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(2, 0xc0 << 8, 0);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    __Func_80925cc(3, 2);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(1, 0x80 << 6, 0);
    __CutsceneWait(0x1e);
    __ActorMessage(2, 0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(1, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetAnim(3, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(3, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __MapActor_SetAnim(2, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(2, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(2, 0, 0);
    __CutsceneWait(0xa);
    __CutsceneEnd();
}
