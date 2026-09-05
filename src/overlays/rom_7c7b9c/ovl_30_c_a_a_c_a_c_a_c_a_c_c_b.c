// fakematch
/* OvlFunc_943_200a9d4  --  0x0200a9d4
 *
 * 142 instructions of straight-line cutscene: place three actors, hand two of
 * them behaviour scripts, transition in, then run the beat and set the flags.
 *
 * TWO CONSTANTS ARE USED TWICE EACH, AND THE ROM MATERIALISES BOTH TWICE.
 * That is the whole shape of this function's residue. `0xcccc` is argument 2
 * of the first __MapActor_SetSpeed and argument 3 of the second; `0x96 << 2`
 * is argument 3 of two __Func_80921c4 calls. Written plainly, cse_main commons
 * each of them into a pseudo, and because every use is separated by calls the
 * allocator has to give that pseudo a CALLEE-SAVED register. Two extra
 * long-lived values is one more than r5 and r6 can hold, so gcc reaches for r8
 * and the function grows a `mov r6, r8 / push {r6}` prologue and a matching
 * two-instruction epilogue -- eight lines the ROM does not have.
 *
 * NO FLAG REACHES IT. -fno-gcse, -fno-cse-follow-jumps,
 * -fno-expensive-optimizations, -fno-force-mem and --no-rerun-cse were all
 * measured: 150 lines every time, identical output. This is cse_main itself,
 * which is not separable at -O2, so the fix has to be in the source.
 *
 * THE FIX IS THE PIN, and this is the mechanism it exists for: a value assigned
 * to a hard CALL-CLOBBERED argument register cannot be commoned across a call,
 * because the call invalidates the register. Pinning the two SetSpeed fills
 * dropped one long-lived value (150 -> 149 lines) and pinning the four
 * __Func_80921c4 fills dropped the other, taking the r8 traffic with it
 * (149 -> 143). The remaining pins are ordinary fill-order anchoring.
 *
 * ALSO LOAD-BEARING: THE ACTOR POINTERS ARE NOT NAMED. `a->f59 |= 0x80` is a
 * read-modify-write whose address form `add r0, #0x59` DESTROYS the pointer --
 * 0x59 is past the 5-bit byte-offset range, so there is no `[r0, #0x59]` to
 * use instead. With the result of __MapActor_GetActor held in a named
 * variable, gcc copies it first (`mov r1, r0 / add r1, #0x59`) to keep the
 * variable intact; calling through the return value directly lets it clobber
 * r0 the way the ROM does.
 *
 * Fill orders below are transcribed from the ROM site by site; four of them
 * are crossed (the shift or the pool load lands between the two movs), which
 * is why each is written out rather than left to gcc.
 */
struct Actor {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x59 - 8];
    unsigned char f59;
};

extern char *iwram_3001ebc;
extern int gScript_943__0200c980[];
extern int gScript_943__0200c628[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __LoadFieldActors(void *p);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, void *p);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void OvlFunc_943_200ba00(int a, int b);
extern void OvlFunc_943_200b9ec(int a);
extern void OvlFunc_943_2008bb8(void);

extern const int _TBL_5160[];   /* = .L5160, aliased in overlays/rom_7c7b9c/overlay.ld */

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_943_200a9d4(void)
{
    __CutsceneStart();
    __LoadFieldActors((void *)_TBL_5160);
    __WaitFrames(1);
    { PIN3; q1 = 0xb6; q0 = 0x14; q1 <<= 16; q2 = 0x26a0000; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xee; q0 = 0x17; q1 <<= 16; q2 = 0x2720000; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x86; q2 = 0x2a60000; q1 <<= 17; q0 = 0x16; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_GetActor(0x16)->f6 = 0;
    __MapActor_SetBehavior(0x16, gScript_943__0200c980);
    __MapActor_GetActor(0x15)->f59 |= 0x80;
    { PIN3; q2 = 0x6666; q0 = 0x15; q1 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(0x15, gScript_943__0200c628);
    *(int *)(iwram_3001ebc + 0x1c0) = 0x100;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x14; q1 = 0x19999; q2 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x89; q2 <<= 2; q0 = 0x14; q1 = 0xb6; __Func_80921c4(q0, q1, q2); }
    OvlFunc_943_200ba00(0x14, 0);
    OvlFunc_943_200ba00(0, 0x80 << 8);
    __Func_80925cc(0x14, 1);
    __MessageID(0x1ee1);
    OvlFunc_943_200b9ec(0x14);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x28);
    { PIN3; q1 = 0xa0; q0 = 0x14; q1 <<= 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x14; q1 = 0x105; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(0x14, 0, 0x28);
    OvlFunc_943_200ba00(0x14, 0);
    OvlFunc_943_200b9ec(0x14);
    __MapActor_DoAnim(0, 3);
    __MapActor_DoAnim(0x14, 3);
    { PIN3; q2 = 0x96; q0 = 0x14; q1 = 0xb6; q2 <<= 2; __Func_80921c4(q0, q1, q2); }
    { PIN3; q2 = 0x96; q2 <<= 2; q0 = 0x14; q1 = 0xd8; __Func_80921c4(q0, q1, q2); }
    OvlFunc_943_200ba00(0x14, 0xc0 << 8);
    OvlFunc_943_2008bb8();
    __CutsceneWait(0xa);
    { PIN3; q2 = 0x91; q0 = 0x14; q1 = 0xd8; q2 <<= 2; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0; q0 = 0x14; q2 = 0; __MapActor_SetPos(q0, q1, q2); }
    *(int *)(iwram_3001ebc + 0x1c0) = 0x209;
    __SetFlag(0x92b);
    __ClearFlag(0x302);
    __CutsceneEnd();
}
