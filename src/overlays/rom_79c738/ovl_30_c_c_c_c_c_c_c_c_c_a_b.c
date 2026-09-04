// fakematch
/* OvlFunc_909_200979c  --  0x0200979c
 *
 * Cut out of goldensun/asm/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_a.s.
 *
 * 159 instructions of straight-line cutscene, exact on the first screen, with
 * twenty-four pinned call sites and two crossed fills.
 *
 * ONLY ONE OF THE TWO WAS FLAGGED. `tools/crossed.py` reported
 * `crossed-sites=1`, and the site it found is the `mov r1 / mov r2 / lsl r2 /
 * lsl r1` __Func_80921c4. The other is __Func_80933f8, whose movs run r0, r1,
 * r2 while its NEGATIONS run r1, r2, r0 -- the same crossing with `neg` in
 * place of `lsl`, which the tool's scan does not look for. Both were barriered
 * from the listing and the function matched first time, so the gap cost nothing
 * here; it is recorded in the tool.
 *
 * The three pooled-constant pairs -- `0x9999`/`0x4ccc` twice and
 * `0xcccc`/`0x6666` once -- are all anchored, because a call whose arguments
 * include a pool load issues those first unless the fill is pinned, whatever
 * the register order looks like.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_80118a8(int a);
extern void __Func_80118c0(int a);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093054(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_909_200979c(void)
{
    __CutsceneStart();
    __MapTransitionIn();
    __WaitMapTransition();
    { PIN3; q1 = 0xc0; q0 = 0x13; q1 <<= 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0xa5; q1 <<= 1; q2 <<= 2; q0 = 0;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x14);
    {
        PIN4;
        q0 = 1; __asm__ volatile ("" : : "r" (q0));
        q1 = 1; q2 = 1; q3 = 0; q1 = -q1; q2 = -q2; q0 = -q0;
        __Func_80933f8(q0, q1, q2, q3);
    }
    __SetFlag(0x80 << 2);
    __PlaySound(0xbc);
    __Func_80118a8(1);
    __Func_80118a8(2);
    { PIN3; q1 = 0x80; q2 = 0x9e; q1 <<= 17; q2 <<= 18; q0 = 0x13;
      __MapActor_SetPos(q0, q1, q2); }
    __WaitFrames(1);
    { PIN3; q0 = 0x13; q1 = 0x9999; q2 = 0x4ccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    {
        PIN3;
        q1 = 0x80; __asm__ volatile ("" : : "r" (q1));
        q2 = 0xa1; q2 <<= 2; q1 <<= 1; q0 = 0x13;
        __Func_80921c4(q0, q1, q2);
    }
    __Func_80118c0(1);
    __Func_80118c0(2);
    __CutsceneWait(0x14);
    { PIN2; q1 = 2; q0 = 0x13; __Func_80925cc(q0, q1); }
    __MessageID(0x145e);
    __Func_8093040(0x13, 0, 0xa);
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x84; q2 = 0xa5; q0 = 0; q1 <<= 1; q2 <<= 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0xa5; q0 = 0x13; q1 = 0xf8; q2 <<= 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x28; q0 = 0x13; q1 <<= 5;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0x13, 4);
    __ActorMessage(0x13, 0);
    __MapActor_DoAnim(0x13, 3);
    { PIN2; q1 = 0; q0 = 0x13; __Func_8093054(q0, q1); }
    __Func_80925cc(0x13, 2);
    __Func_8093040(0x13, 0, 0xa);
    { PIN3; q2 = 0x3c; q0 = 0; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0x81; q1 <<= 1; q0 = 0x13; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    __Func_80925cc(0x13, 1);
    { PIN3; q2 = 0xa; q0 = 0x13; q1 = 0; __Func_8093040(q0, q1, q2); }
    __MapActor_DoAnim(0x13, 3);
    __ActorMessage(0x13, 0);
    { PIN3; q0 = 0x13; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xc1; q0 = 0x13; q1 = 0xf8; q2 <<= 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0; q2 = 0; q0 = 0x13; __MapActor_SetPos(q0, q1, q2); }
    __ClearFlag(0x12f);
    __SetFlag(0x84f);
    __CutsceneEnd();
}
