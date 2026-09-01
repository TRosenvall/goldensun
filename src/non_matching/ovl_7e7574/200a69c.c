/* OvlFunc_959_200a69c (0x0200a69c) -- NON-MATCHING.
 * Blocker class: duplicate-constant CSE. THIRD specimen this batch.
 *
 * A pure cutscene script -- thirteen calls, no branches, no loops. 47 lines
 * against the ROM's 46, and the single extra line is `push {r5, r6, r14}`
 * where the ROM pushes only lr.
 *
 * `__MapActor_SetSpeed` is called twice with the same two speeds,
 * `0x80 << 9` and `0x80 << 8`. The ROM rebuilds both with mov/lsl at each
 * site. gcc CSEs them, and because the values must survive the intervening
 * calls it puts them in r5 and r6 -- which the function otherwise does not
 * need, hence the wider push and pop.
 *
 * MEASURED (rom 46 lines):
 *   baseline                     47, 29
 *   -fno-gcse                    47, 29
 *   -fno-rerun-cse-after-loop    47, 29
 *   -fno-strength-reduce         47, 29
 *
 * THE THREE SPECIMENS THIS BATCH AGREE, and together they say something the
 * individual entries did not:
 *
 *   OvlFunc_970_20092ac  one constant twice across three calls  -> r8, +5 lines
 *   Func_801965c         a zero twice through halfword stores   -> +1 line
 *   this                 two constants twice across one call    -> r5/r6, +1
 *
 * -fno-gcse is inert on ALL THREE. So the pass responsible is cse.c's local
 * constant CSE, not the global one, and there is no flag that reaches it. That
 * is worth knowing because -fno-gcse is the obvious thing to reach for and it
 * has now cost four runs across three functions to rule out.
 *
 * The class also has a PREDICTABLE VICTIM. A cutscene script -- straight-line
 * calls with repeated coordinate or speed constants -- will hit this every
 * time, because the same literal appears at two call sites with a call between
 * them, which is exactly the shape that forces a callee-saved register.
 * pickable.py already rejects on a repeated expensive constant; the point here
 * is that a function can look trivial (no branches, no loops, thirteen calls in
 * a row) and still be unreachable.
 *
 * WHAT IS RIGHT: every call, every argument, and every constant BUILD --
 * `0x80 << 9`, `0x80 << 8`, `0xec << 1` and the pooled `ldr r0, =0x247c` all
 * come out exactly as the ROM has them. Only the sharing is wrong.
 *
 * NEXT: nothing. The class has no known source-level lever.
 */
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __CutsceneWait(int n);
extern int __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809218c(int a, int b, int c);
extern void __SetCameraTarget(int slot, int a);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetAnim(int slot, int anim);

void OvlFunc_959_200a69c(void)
{
    __Func_8093500(0xb, 1);
    __Func_8093530();
    __CutsceneWait(0x3c);
    __MessageID(0x247c);
    __ActorMessage(0xd, 0);
    __MapActor_SetSpeed(0xb, 0x80 << 9, 0x80 << 8);
    __MapActor_SetSpeed(0xf, 0x80 << 9, 0x80 << 8);
    __Func_809218c(0xb, 0xec << 1, 0xb4);
    __Func_809218c(0xf, 0xec << 1, 0xb4);
    __SetCameraTarget(0xb, 1);
    __MapActor_WaitMovement(0xb);
    __MapActor_SetAnim(0xb, 4);
    __CutsceneWait(0x1e);
}
