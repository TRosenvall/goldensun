// fakematch
/* OvlFunc_882_200be48  --  0x0200be48
 *
 * Cut out of goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_c.s.
 *
 * The NPC in slot 8 reacts to the player. Flag 0x305 records whether this has
 * happened before: on a repeat it is a short greeting, and on the first visit
 * it is the full routine -- a turn, two jumps, a longer message -- ending by
 * setting the flag. Both paths finish by handing the NPC back its idle
 * behaviour script and pose 6. The facing chosen at three points is read off
 * the PLAYER's heading, not the NPC's, which is why slot 0 is fetched at all.
 *
 * Found by tools/templated.py, and it is the entry that motivated ranking on
 * SYMBOL COUNT rather than the ratio alone: 16 shared symbols at 0.88, sitting
 * below several 1.00 entries built from two symbols each. The neighbour
 * supplied nearly the whole extern block.
 *
 * MATCHED ON THE FIRST CANDIDATE BUT FOR ONE INSTRUCTION -- 2 of 140, and both
 * halves of that were the same `lsl r1, #5` landing one slot early.
 *
 * THE ROM DEFERS THE SHIFT PAST EVERY OTHER ARGUMENT:
 *
 *     mov r1, #0x80
 *     str r3, [r5, #0x18]
 *     str r3, [r5, #0x1c]
 *     mov r2, #0
 *     mov r0, #8
 *     lsl r1, #5          <- last thing before the bl
 *     bl  __Func_8092adc
 *
 * Written `0x80 << 5` the shift follows its own `mov` immediately. Neither
 * plain two-step reaches it: assigning `v = 0x80;` before the two stores and
 * `v <<= 5;` after them measures the same 2, and so does the two-step written
 * compactly. Argument evaluation order is not something the source can state.
 *
 * THE SCAFFOLDING IS ONE PIN, AND THE TEARDOWN IS WHY. The first form that
 * matched pinned all three argument registers AND used a two-step -- four
 * pieces. Removing them one at a time:
 *
 *     r0 + r1 + r2 pinned, two-step        0   (the first thing that worked)
 *     r1 + r2 pinned, two-step             2
 *     r0 + r1 pinned, two-step             0
 *     r0 + r2 pinned, no two-step          0
 *     r0 pinned alone, no two-step         0   <- landed
 *     r2 pinned alone                      2
 *
 * So r0 is the whole lever and everything else was habit. Pinning r0 forces
 * the slot argument into place early, which leaves the shift as the only work
 * left to do before the call and gcc emits it last. This is the third round
 * running where the anchor-every-argument rule has had to be bounded -- anchor
 * the argument that participates, and let the teardown find it.
 *
 * The facing test reads a SIGNED short at +6 and branches on `>= 0`, so the
 * two poses are chosen by the sign of the player's heading rather than by a
 * comparison against a constant.
 */

extern int __GetFlag(int);
extern void __SetFlag(int);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int);
extern void __ActorMessage(int, int);
extern void __MapActor_SetAnim(int slot, int anim);
extern char *__MapActor_GetActor(int slot);
extern void __MapActor_SetBehavior(int slot, void *script);
extern void __MapActor_SetIdle(int slot);
extern void __Func_8092adc(int slot, int a, int b);
extern void __Func_80925cc(int slot, int a);
extern void __Func_8093040(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void gScript_882__0200cec8;

void OvlFunc_882_200be48(void)
{
    char *a;
    char *b;

    a = __MapActor_GetActor(0);
    b = __MapActor_GetActor(8);
    __CutsceneStart();
    if (__GetFlag(0x305)) {
        __MapActor_SetIdle(8);
        __CutsceneWait(0xa);
        __Func_80925cc(8, 2);
        __CutsceneWait(0x28);
        if (*(short *)(a + 6) >= 0)
            __MapActor_SetAnim(8, 7);
        else
            __MapActor_SetAnim(8, 8);
        __Func_80925cc(8, 2);
        __CutsceneWait(0x14);
        __MessageID(0xed2);
        __ActorMessage(8, 0);
        __MapActor_SetBehavior(8, &gScript_882__0200cec8);
        __MapActor_SetAnim(8, 6);
    } else {
        __MapActor_SetIdle(8);
        *(int *)(b + 0x18) = 0x80 << 9;
        *(int *)(b + 0x1c) = 0x80 << 9;
        {
            register unsigned int q __asm__("r0") = 8;
            __Func_8092adc(q, 0x80 << 5, 0);
        }
        if (*(short *)(a + 6) >= 0)
            __MapActor_SetAnim(8, 7);
        else
            __MapActor_SetAnim(8, 8);
        __CutsceneWait(0x14);
        __MessageID(0xed1);
        __Func_8093040(8, 0, 0x14);
        __MapActor_SetAnim(8, 1);
        __MapActor_Jump(8, 4, 0);
        __CutsceneWait(0x50);
        __Func_80925cc(8, 2);
        __CutsceneWait(0x28);
        if (*(short *)(a + 6) >= 0)
            __MapActor_SetAnim(8, 7);
        else
            __MapActor_SetAnim(8, 8);
        __CutsceneWait(2);
        __MapActor_Jump(8, 2, 0);
        __CutsceneWait(0x3c);
        __Func_80925cc(8, 2);
        __CutsceneWait(0x14);
        __ActorMessage(8, 0);
        __MapActor_SetBehavior(8, &gScript_882__0200cec8);
        __MapActor_SetAnim(8, 6);
        __SetFlag(0x305);
    }
    __CutsceneEnd();
}
