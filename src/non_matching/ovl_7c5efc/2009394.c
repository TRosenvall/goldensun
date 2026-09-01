/* OvlFunc_941_2009394 (0x02009394) -- NON-MATCHING.
 * Blocker class: duplicate-constant CSE. THE CANONICAL SMALL SPECIMEN.
 *
 * 53 lines against 53. `0x81 << 1` is passed to two `__MapActor_Emote` calls
 * eleven calls apart. The ROM rebuilds it both times with `mov r1, #0x81 /
 * lsl r1, #1`; gcc hoists it into r6 and pays a wider push and pop. Every one
 * of the 39 differing lines follows from that one decision.
 *
 * THIRTEEN FLAGS HAVE NOW BEEN TESTED AGAINST THIS CLASS AND NONE REACHES IT.
 * Batch 175 ruled out five on three specimens; this function, being small and
 * having exactly one residue, was used to rule out eight more:
 *
 *   -fno-gcse                     inert (batch 175, three specimens)
 *   -fno-rerun-cse-after-loop     inert
 *   -fno-strength-reduce          inert
 *   -fno-strict-aliasing          inert
 *   -fno-schedule-insns2          inert / worse
 *   -fno-cse-follow-jumps         inert
 *   -fno-cse-skip-blocks          inert
 *   -fno-force-mem                inert
 *   -fno-caller-saves             inert
 *   -fno-function-cse             inert
 *   -fno-inline                   inert
 *   -fno-expensive-optimizations  54 lines, 47 differing (WORSE)
 *   -fno-omit-frame-pointer       55 lines, 54 differing (WORSE)
 *
 * That is the search closed. The sharing happens in cse.c's local constant
 * propagation, which gcc-2.96 exposes no switch for, and no spelling separates
 * two uses of one literal (batch 175 also ruled out giving the two uses
 * different parameter modes -- gcc folds the narrowing before CSE runs).
 *
 * WHY THIS MATTERS FOR SELECTION. A scan for call-dominated functions with no
 * stack arguments and no high registers returns 158 candidates, and EVERY ONE
 * of them repeats an expensive constant. The pure-cutscene-script class, which
 * produced four elevations across batches 176 and 177, is exhausted: what is
 * left of it is all this wall. Do not re-derive that -- the DUP-CONST column in
 * pickable.py, filtered.py, family_siblings.py and lowpressure.py is correct
 * and is a hard skip.
 *
 * WHAT IS RIGHT: every call and argument, the message id carried in a
 * callee-saved register and stepped (`m`, `m + 1`, `m += 2`), and the
 * `neg / orr / lsr #31 / mov #1 / sub` tail from `return f(0, 0) == 0;`.
 *
 * NEXT: nothing. This is the class boundary, not a function-specific problem.
 */
extern void __Func_809280c(int a, int b, int c);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __Func_8092adc(int slot, int a, int b);
extern void __CutsceneWait(int n);
extern void __MapActor_DoAnim(int slot, int a);
extern void __Func_8092c40(int slot, int a);
extern int __Func_8091c7c(int a, int b);

int OvlFunc_941_2009394(void)
{
    int m;

    __Func_809280c(2, 0, 0);
    __MapActor_Emote(2, 0x81 << 1, 0x3c);
    m = 0x255e;
    __MessageID(m);
    __ActorMessage(2, 0);
    __Func_8092adc(0xc, 0xc0 << 6, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xc, 4);
    __MessageID(m + 1);
    __ActorMessage(0xc, 0);
    __MapActor_Emote(3, 0x81 << 1, 0x3c);
    m += 2;
    __MessageID(m);
    __Func_8092c40(3, 0);
    return __Func_8091c7c(0, 0) == 0;
}
