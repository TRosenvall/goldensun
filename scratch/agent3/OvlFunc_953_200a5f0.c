/* OvlFunc_953_200a5f0 -- NOT MATCHING. 35 differing of 43 (default flags).
 * ref: asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c.s
 *
 * BLOCKER: constant hoisted across a call, in a STRAIGHT-LINE block.
 * 0xd6 << 1 is passed to three consecutive calls; gcc builds `mov r5,#0xd6`
 * once before __MapTransitionIn, shifts it once and copies `mov r2, r5` at each
 * site, where the ROM rebuilds `mov r2,#0xd6 / lsl r2,#1` three times.
 *
 * CONTROL: with the three constants made distinct (0xd6/0xd7/0xd8) the function
 * is 10 of 43, so the shared constant accounts for 25 of the 35.  The residual
 * 10 are the arg-interleave shape (`mov r1 / mov r2 / mov r0 / lsl / lsl`) at
 * two of the three sites plus the r0/r1 rotation at __MapActor_SetSpeed --
 * i.e. the SAME lever, which also needs a boundary.
 *
 * MEASURED, all identical at 35: -fno-rerun-cse-after-loop, -fno-gcse,
 * -fno-cse-follow-jumps, -fno-cse-skip-blocks, -fno-expensive-optimizations,
 * -fno-force-mem, -fno-schedule-insns, -fno-strict-aliasing, -fno-peephole,
 * -fno-regmove, -fcall-saved-r4, and the two CSE flags combined.
 * -fno-schedule-insns2 and -O1 are 31 (better, still wrong).
 * Three int locals adjacent to the calls: 35.  Three locals at the top: 35.
 *
 * NO BOUNDARY EXISTS: the only branch is the if/else at the very end, after
 * every use, so no block dominates a use without being the use's own block.
 * Two attempts to MANUFACTURE a boundary were measured and both are no-ops:
 *   goto L; L:                       -> byte-identical (35)
 *   goto L; <dead stores>; L:        -> byte-identical (35)
 * jump.c deletes both before flow.c computes basic blocks, so REG_BASIC_BLOCK
 * never becomes GLOBAL.  This is a direct negative for the open question in
 * docs/elevation.md ("a construct that produces rematerialisation without a
 * control-flow boundary would unpark nine functions").
 */
extern void __CutsceneStart(void);
extern void __MapActor_SetSpeed(int, int, int);
extern void __MapTransitionIn(void);
extern void __MapActor_SetAnim(int, int);
extern void __Func_8092158(int, int, int);
extern void __MapActor_TravelTo(int, int, int);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern int  __GetFlag(int);
extern void __Func_8091e9c(int);

void OvlFunc_953_200a5f0(void)
{
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x19999, 0xcccc);
    __MapTransitionIn();
    __MapActor_SetAnim(0, 2);
    __Func_8092158(0, 0xc3 << 2, 0xd6 << 1);
    __Func_8092158(0, 0xdc << 2, 0xd6 << 1);
    __MapActor_TravelTo(0, 0xf5 << 2, 0xd6 << 1);
    __MapTransitionOut();
    __WaitMapTransition();
    if (__GetFlag(0x90f))
        __Func_8091e9c(0x20);
    else
        __Func_8091e9c(0xc);
}
