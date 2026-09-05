// fakematch
/* OvlFunc_942_2008ba0  --  0x02008ba0
 *
 * 255 instructions of straight-line cutscene, no guard: the whole body is one
 * basic block apart from the null test on actor 0 near the end.
 *
 * THE PROLOGUE IS THE WHOLE PROBLEM, AND IT TELLS YOU THE CURE. The ROM opens
 * with `push {lr}` ALONE. It spends no callee-saved register at all, so every
 * repeated constant is rebuilt at every use. Plain C is 271 lines against 258
 * with `push {r5, r6, lr}` and r8/r9/r10 saved on top: cse1 hoists 0x80 << 8,
 * 0xa0 << 8, 0xb2 << 3, 0x84 << 1 and the 0x13333/0x9999 pair into pseudos
 * whose ranges straddle calls, and the allocator must give those pseudos
 * callee-saved registers. 265 of 258 differing.
 *
 * The three cures in docs/elevation.md ("A CONSTANT USED TWICE ACROSS CALLS")
 * are ranked by use count, and this function is the far end of that scale --
 * 0x80 << 8 alone has six sites. NAMED LOCALS ARE THE WRONG CURE HERE and
 * measure WORSE than plain literals: one named local per duplicated value is
 * 275 lines / 265 differing, four lines LONGER than doing nothing, because a
 * named pseudo with six references is exactly what global-alloc wants to keep
 * in a callee-saved register. Only pins reach it. A value assigned to a hard
 * call-clobbered register is dead across the next `bl`, so gcc has nothing to
 * carry it in and must rematerialise.
 *
 * FOURTEEN PINS, MINIMAL BY MEASUREMENT. Seventeen sites were pinned to get
 * the first exact screen; each was then stripped one at a time and a greedy
 * pass repeated to fixpoint from BOTH ends of the list. Both directions
 * converge on FOURTEEN, and the two orders keep DIFFERENT sets: the pins that
 * share a value are mutually substitutable, and exactly one of each
 * same-value pair must survive. Forward order drops the __Func_809233c fill,
 * the first 0xa0 << 8 fill and the first __Func_80933f8 fill; reverse order
 * drops the first 0xa0 << 8 fill, the second __Func_80933f8 fill and the
 * 0x80 << 8 fill before __CutsceneWait(0x32). "N pins" is therefore a size,
 * not a set, and stripping the surviving fourteen one at a time on the
 * shipped file leaves every one load-bearing (2 to 170 differing).
 *
 * WRITE THE SHIFTS IN THE MOVS' ORDER, not the ROM's shift order. Both
 * __Func_80933f8 sites want `q0 <<= 17; q1 = -q1; q2 <<= 19` in source, which
 * lands the ROM's `mov r0 / mov r1 / mov r2` fill order; sched2 then re-lands
 * the shifts as `neg r1 / lsl r2 / lsl r0` by itself. Transcribing the ROM's
 * shift order instead reverses the movs -- 3 differing per site.
 *
 * NEW, AND THE POINT OF THIS FILE: `p->goalFacing` AND `*(short *)(p + 0x64)`
 * ARE NOT THE SAME STORE when the offset needs an explicit `add`.
 *
 *     unsigned char *p;                              -- 20 differing
 *     *(short *)(p + 0x64) = *(int *)(p + 8) / 0x10000;
 *
 *     Actor *p;                                      -- EXACT
 *     p->goalFacing = p->pos.x / 0x10000;
 *
 * strh's immediate offset is 5 bits scaled by 2, so 0x64 is out of range and
 * the address must be built with `mov r3, r0 / add r3, #0x64`. The ROM emits
 * the VALUE first (`ldr r2, [r0, #8]`, the sign fixup, then the address). The
 * cast form emits the ADDRESS first, because `p + 0x64` is a C expression that
 * expand_expr forces into a pseudo before the RHS is touched; a COMPONENT_REF
 * is a MEM whose `plus (reg, 100)` address is only legitimized at emit time,
 * after the RHS. This is the same family as the call-in-the-RHS rule already
 * on file, reached without any call: what decides it is whether the address is
 * an expression or a member offset. Naming the RHS into a temp is the cure for
 * the cast form and is INERT once the member form is used -- both temps strip
 * clean.
 *
 * THE 16.16 FIELD IS READ TWO DIFFERENT WAYS AND THE ROM SHOWS BOTH.
 * `pos.x >> 16` at the __MapActor_TravelTo site becomes a sign-extending
 * halfword load, `mov r3, #0xa / ldrsh r1, [r0, r3]` -- combine folds the
 * shift into the load, and ldrsh has no immediate form in Thumb, hence the
 * register offset. `pos.x / 0x10000` at the goalFacing store keeps the
 * `cmp / bge / add 0xffff / asr` round-toward-zero fixup. Swapping them is
 * measurable in both directions: `/ 0x10000` at TravelTo is 268 lines and 52
 * differing, `>> 16` in the tail is 248 lines and 22 differing. The 0xffff
 * fixup is the tell for which one the source used.
 *
 * THE TWO `stop` STORES MUST BE DIRECT CALL EXPRESSIONS, not a named local.
 * `__MapActor_GetActor(8)->stop = 1;` reuses the returned r0 in place, as the
 * ROM does (`add r0, #0x5b / strb r3, [r0]`); binding it to `p` first costs a
 * `mov r2, r0` copy and a line. `p` is genuinely live across two uses only in
 * the tail, which is where the ROM's own `mov r3, r0` copy appears.
 *
 * BUILT AT -O2 -- the tree default, via `asm/%.o: src/%.c`. There is no flag
 * wildcard under rom_7c6bac whose prefix reaches ovl_30_c_c_c%; the three
 * explicit rules there name other objects. Checked because a mis-scoped
 * wildcard has now bitten four times (Makefile:289): at -O1 this file is 83
 * differing. objcmp against the ORIGINAL .s path and against a scratch path
 * agree, which is what confirms the flag is not path-dependent here.
 *
 * All 69 relocations in the reference are R_ARM_THM_CALL. There is no data
 * relocation anywhere in the function, so every pool word -- 0x1f89, 0x19999,
 * 0xcccc, 0x13333, 0x9999, 0xffff -- is a literal and not a symbol. Checked
 * rather than inferred from line counts.
 *
 * The assumption behind all of the above, stated so it can be re-derived: the
 * bare `push {lr}` means NO value survives a call, so every difference is a
 * scheduling or rematerialisation question and never an allocation one. A
 * sibling that pushes even one register breaks the reading and wants the
 * named-local cure instead.
 */
#include "actor.h"

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_808e118(void);
extern void __Func_809233c(int a, int b, int c, int d);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_942_2008ba0(void)
{
    Actor *p;

    __CutsceneStart();
    __Func_808e118();
    __MapActor_SetPos(8, 0xa4 << 17, 0xb2 << 19);
    __MapActor_GetActor(8)->stop = 1;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __Func_809233c(1, -0x10, 0, 0x80 << 8);
    __MapActor_WaitMovement(1);
    __Func_8092adc(0, 0xa0 << 8, 0);
    __CutsceneWait(0x14);
    __MessageID(0x1f89);
    { PIN3; q1 = 0xa0; q0 = 0; q1 <<= 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x19999; q2 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xb2; q0 = 1; q1 = 0xe8; q2 <<= 3;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 1; q1 <<= 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_80933f8(0xb8 << 16, -1, 0xb4 << 19, 1);
    __Func_8093530();
    __CutsceneWait(0xa);
    __MapActor_Jump(1, 6, 0xf);
    __MapActor_Jump(1, 6, 0x28);
    __ActorMessage(1, 0);
    __CutsceneWait(0x14);
    { PIN4; q0 = 0x84; q1 = 1; q2 = 0xb5; q3 = 1; q0 <<= 17; q1 = -q1; q2 <<= 19;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x80; q0 = 8; q1 <<= 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x84; q2 = 0xb2; q0 = 8; q1 <<= 1; q2 <<= 3;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q1 <<= 8; q2 = 0; q0 = 8;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_8092adc(1, 0, 0);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(8, 4);
    __CutsceneWait(0xa);
    __ActorMessage(8, 0);
    __CutsceneWait(0x14);
    __MapActor_Emote(1, 0x81 << 1, 0x28);
    __CutsceneWait(0x1e);
    { PIN3; q1 = 0x80; q2 = 0; q1 <<= 8; q0 = 1;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x32);
    __ActorMessage(1, 0);
    __CutsceneWait(0x14);
    __Func_8092adc(1, 0x80 << 6, 0);
    __CutsceneWait(0x1e);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 1; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x84; q2 = 0xb7; q0 = 1; q1 <<= 1; q2 <<= 3;
      __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(1, 0, 0);
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(8, 0x80 << 7, 0);
    __CutsceneWait(0x1e);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, p->pos.x >> 16, p->pos.z >> 16);
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __CutsceneWait(0x14);
    __MapActor_GetActor(8)->stop = 0;
    __MapActor_SetBehavior(8, 2);
    p = __MapActor_GetActor(8);
    p->goalFacing = p->pos.x / 0x10000;
    p->tickSlow = p->pos.z / 0x10000;
    __CutsceneEnd();
}
