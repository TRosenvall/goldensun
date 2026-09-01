/* OvlFunc_953_200a904 (0x0200a904) -- NON-MATCHING.
 * Blocker class: duplicate-constant CSE.
 *
 * At the ROM's exact 34 lines with 25 differing, and every one of the 25 is a
 * consequence of one decision. `0xd6 << 1` is passed to three consecutive
 * calls. The ROM rebuilds it each time with `mov r2, #0xd6 / lsl r2, #0x1`.
 * gcc hoists it into r5 -- AND PUSHES r5 TO DO SO:
 *
 *     rom    push {r14}        ... mov r2, #0xd6 / lsl r2, #0x1   (x3)
 *     ours   push {r5, r14}    mov r5, #0xd6 / lsl r5, #0x1 ... mov r2, r5 (x3)
 *
 * THIS FUNCTION WAS SCREENED TO TEST A HYPOTHESIS, AND THE HYPOTHESIS WAS
 * WRONG. Batch 175 parked three functions on this class, and all three had
 * callee-saved registers already in use -- so the reading was that gcc shares a
 * repeated constant only when it has a register spare, and that a function
 * whose ROM pushes nothing but `lr` would be safe. It is not.
 *
 *   **gcc will ADD a callee-saved register, and its push and its pop, to share
 *   a constant it could rebuild in two instructions.**
 *
 * That is why the length still comes out identical: the two hoist instructions
 * plus the wider push/pop exactly pay for the three rebuilds it saves. A
 * zero-pressure ROM push list is no protection, and the duplicate-constant
 * reject in pickable.py / filtered.py / family_siblings.py is right to be a
 * hard skip regardless of pressure.
 *
 * MEASURED (rom 34 lines, all at exact length):
 *   baseline                      34, 25
 *   -fno-gcse                     34, 25
 *   -fno-rerun-cse-after-loop     34, 25
 *   -fno-strength-reduce          34, 25
 *   -fno-strict-aliasing          34, 25
 *   -fno-schedule-insns2          34, 22 (fewer, but it is the recorded
 *                                 "destroying the evidence" pattern -- the
 *                                 count drops because unrelated lines shuffle
 *                                 back into place, not because the hoist moved)
 *
 * WHAT IS RIGHT: every call, every argument, both pooled constants
 * (`=0xcccc`, `=0x19999`), and all three `imm8 << n` builds.
 *
 * NEXT: nothing. Same wall as ovl_7fa4ec/20092ac.c and ovl_7e7574/200a69c.c.
 */
extern void __CutsceneStart(void);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapTransitionIn(void);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092158(int slot, int x, int y);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8091e9c(int n);

void OvlFunc_953_200a904(void)
{
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x19999, 0xcccc);
    __MapTransitionIn();
    __MapActor_SetAnim(0, 2);
    __Func_8092158(0, 0xc8 << 2, 0xd6 << 1);
    __Func_8092158(0, 0xaf << 2, 0xd6 << 1);
    __MapActor_TravelTo(0, 0x96 << 2, 0xd6 << 1);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x16);
}
