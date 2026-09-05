// fakematch
/* OvlFunc_938_2009494  --  0x02009494
 * [asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_c.s, the only function, with a
 *  strictly trailing .data section that split_s.py peels into its own object]
 *
 * 558 instructions. Byte-exact: 1440 bytes, 567 encodings and 136 relocations
 * identical.
 *
 * `push {r5, r6, lr}` is a WIDE push and this is still a pin function -- read
 * what it keeps. r6 is the ADDRESS OF iwram_3001ebc, and r5 does three jobs in
 * sequence: the commoned constant 0x100, then the `int v` flag, then the
 * address of a script array. No callee-saved register holds a script constant.
 * Plain C is 566 lines against 558 with an r8/r10 spill and 530 differing.
 *
 * `__Func_8092c40` IS NOW A RELIABLE TELL BY NAME. The uniform ascending fill
 * took 113 pinned sites to FOUR differing in one step, and the entire residue
 * was the two __Func_8092c40 calls, both wanting the DESCENDING fill
 * `q1 = 0; q0 = N;`. That makes three functions in a row where this callee is
 * the lone descending survivor -- OvlFunc_966_2008218 and OvlFunc_910_20085dc
 * being the others. Write it descending on sight and check the residue for the
 * rest.
 *
 * THE POLARITY REVERSES ACROSS THE JOINS. Of seven __Func_8093040(0x2009, 0,
 * 0xa) sites, FOUR survive minimisation and three drop -- and the survivors are
 * not a prefix. The two __Func_8091c7c if/else joins are what breaks the
 * first-use ordering, the same shape as OvlFunc_952_2008674. Another site is
 * pinned at BOTH its uses, the second buying argument ordering rather than
 * destroying a dead CSE.
 *
 * THREE VALUES ARE COMMONED BY THE ROM ITSELF AND MUST STAY PLAIN: 0x100 is
 * built once, stored, then reused at both Emote sites; the script array address
 * is held across the SetBehavior/SetBehavior/RunScript trio. gcc reproduces
 * both unaided, and pinning the trio costs 30 differing.
 *
 * `int v` IS STRUCTURAL, NOT SCAFFOLDING -- set before the first guard, cleared
 * in one arm, re-tested after the join so the counter is bumped once on either
 * path at different times. Binding it to r5 is byte-identical and was stripped:
 * gcc picks r5 itself.
 *
 * 113 pins minimised to 45, with all 68 candidates removable together this
 * time and no late breakage, then a reverse-order sweep from the fixpoint
 * removing nothing further.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char ActorCmd_ARRAY_938__02009b94[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_SetBehavior(int slot, void *script);
extern void __MapActor_RunScript(int slot, void *script);
extern void __MapTransitionIn(void);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_800fe9c(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern int __Func_8091c7c(int a, int b);
extern void OvlFunc_938_2009450(int n);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_938_2009494(void)
{
    unsigned char *p;
    int v;

    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
    __WaitFrames(1);
    { PIN4; q0 = 0xd8 << 18; q1 = -1; q2 = 0x86 << 18; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    { PIN3; q0 = 0; q1 = 0xd8 << 18; q2 = 0x2760000; __MapActor_SetPos(q0, q1, q2); }
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x80 << 1;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x28;
    __MapTransitionIn();
    { PIN2; q0 = 0x6666; q1 = 0xccc; __Func_80933d4(q0, q1); }
    __Func_80933f8(0xd8 << 18, -1, 0xec << 17, 1);
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0, 0xd8 << 2, 0xf9 << 1);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(1, *(int *)(p + 8), *(int *)(p + 0x10));
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(2, *(int *)(p + 8), *(int *)(p + 0x10));
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(3, *(int *)(p + 8), *(int *)(p + 0x10));
    { PIN3; q0 = 0; q1 = 0xd6 << 2; q2 = 0xf3 << 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xd4 << 2; q2 = 0xfb << 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xda << 2; q2 = 0xf3 << 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xdc << 2; q2 = 0xfb << 1; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0, 1);
    __MapActor_SetAnim(1, 1);
    __MapActor_SetAnim(2, 1);
    __CutsceneWait(0xa);
    OvlFunc_938_2009450(0xa);
    __MapActor_Emote(9, 0x80 << 1, 0x14);
    __Func_8092adc(9, 0xa0 << 7, 0x14);
    __MessageID(0x2588);
    { PIN3; q0 = 0x2009; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    __MapActor_Emote(8, 0x80 << 1, 0x14);
    { PIN3; q0 = 8; q1 = 0xc0 << 6; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(8, 0, 0x14);
    { PIN3; q0 = 8; q1 = 0x107; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(8, 0, 0xa);
    { PIN2; q0 = 0; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 1; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 2; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 3; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 9; q1 = 0x81 << 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xe0 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x2009, 0, 0xa);
    { PIN3; q0 = 8; q1 = 0x80 << 5; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x84 << 1; q2 = 0x14; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(8, 0, 0x28);
    __Func_80925cc(8, 2);
    __Func_8093040(8, 0, 0x14);
    { PIN3; q0 = 8; q1 = 0xc0 << 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 8; __Func_8092c40(q0, q1); }
    v = 1;
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0xa);
        __MapActor_SetAnim(8, 3);
    } else {
        __CutsceneWait(0xa);
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
        __MapActor_SetAnim(8, 4);
        v = 0;
    }
    __Func_8093040(8, 0, 0xa);
    if (v != 0)
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
    __Func_809259c(9, 2);
    __MapActor_Surprise(9, 0x81 << 1);
    __CutsceneWait(0x50);
    __Func_8093040(0x2009, 0, 0xa);
    { PIN3; q0 = 8; q1 = 0x80 << 5; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x107; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(2, 3);
    { PIN3; q0 = 0x2002; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x105; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xc0 << 6; q2 = 0x3c; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(8, 0, 0xa);
    __Func_809259c(1, 2);
    __Func_8093040(1, 0, 0xa);
    { PIN3; q0 = 9; q1 = 0xa0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_809259c(2, 2);
    { PIN3; q0 = 0x6002; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_809259c(2, 2);
    __Func_8093040(0x2002, 0, 0x14);
    __MapActor_DoAnim(8, 3);
    __Func_8093040(8, 0, 0x14);
    { PIN3; q0 = 2; q1 = 0x80 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(0x6002, 0, 0xa);
    { PIN3; q0 = 3; q1 = 0xa0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x105; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(3, 0, 0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x50);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(1, 4);
    { PIN2; q1 = 0; q0 = 1; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        (*(unsigned short *)(iwram_3001ebc + (0xec << 1)))++;
    } else {
        __CutsceneWait(0x14);
        __Func_8093040(1, 0, 0xa);
    }
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    OvlFunc_938_2009450(0x14);
    __Func_80925cc(9, 2);
    { PIN3; q0 = 0x2009; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    __Func_80925cc(3, 2);
    __Func_8093040(3, 0, 0xa);
    { PIN3; q0 = 9; q1 = 0xc0 << 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(9, 3);
    { PIN3; q0 = 0x2009; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x2002, 0, 0xa);
    __Func_80925cc(9, 1);
    { PIN3; q0 = 9; q1 = 0xa0 << 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x2009; q1 = 0; q2 = 0xa; __Func_8093040(q0, q1, q2); }
    __Func_80925cc(1, 2);
    __Func_8093040(1, 0, 0xa);
    __MapActor_DoAnim(9, 4);
    __Func_8093040(0x2009, 0, 0xa);
    __Func_80925cc(3, 1);
    __Func_8093040(3, 0, 0xa);
    __Func_80925cc(8, 1);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0xa);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(1, ActorCmd_ARRAY_938__02009b94);
    __MapActor_SetBehavior(2, ActorCmd_ARRAY_938__02009b94);
    __MapActor_RunScript(3, ActorCmd_ARRAY_938__02009b94);
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
    __ClearFlag(0x12f);
    __SetFlag(0x914);
    __CutsceneEnd();
}
