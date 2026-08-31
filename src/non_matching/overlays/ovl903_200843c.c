/* OvlFunc_903_200843c -- 0x0200843c  (asm/overlays/rom_798dc4/ovl_314_a_c_a_c.s)
 *
 * BLOCKER: duplicate-constant CSE into callee-saved registers.  SECOND instance
 * of the class first written up in src/non_matching/overlays/ovl942_2008144.c.
 * 58 of 51 differ (ours 59 lines) -- almost everything, from one cause.
 *
 * Two consecutive __MapActor_SetSpeed calls take the same pair of constants:
 *
 *     rom   ldr r1, =0x3333 / ldr r2, =0x1999      (both calls, reloaded)
 *     ours  ldr r5, =0x3333 / ldr r6, =0x1999      once, then mov r1, r5 / mov r2, r6
 *
 * gcc hoists both into callee-saved registers and holds them across the calls,
 * which forces `push {r5, r6, lr}` plus a separate save of r8 -- eight lines of
 * prologue and epilogue to save two pool loads.  The trade does not even pay:
 * `ldr r1, =0x3333` and `mov r1, r5` are both one instruction, so the hoist
 * saves nothing and costs four.  The ROM simply reloads.
 *
 * WHY THIS INSTANCE MATTERS: 942 hoisted a constant that cost TWO instructions
 * to materialise (mov + lsl), so gcc's cost model at least had a reason.  Here
 * each use costs ONE instruction and gcc hoists anyway.  So the class is not
 * "expensive constants get hoisted" -- it is "repeated constants get hoisted",
 * full stop, regardless of what recomputing them would cost.
 *
 * THE MAKEFILE'S EXISTING RULE DOES NOT APPLY.  Makefile:192 documents two
 * overlay TUs that match only with -fno-rerun-cse-after-loop, described as
 * "load a save-flag id twice around a call, and at -O2 gcc hoists it into a
 * callee-saved register -- costing a push, a pop and two moves to save one pool
 * load".  That is a word-for-word description of THIS function, so it was the
 * obvious thing to try.  It does not help:
 *
 *     default          58 differ
 *     --no-rerun-cse   58 differ
 *     --O1             57 differ
 *
 * This confirms the caveat already in that Makefile comment -- "sweeping all 85
 * parked files with this flag matched only these two, so it is NOT a general
 * lever for the constant-CSE class".  Two functions now sit on the wrong side
 * of that rule, which is worth knowing before anyone widens it: the flag is not
 * what separates hoisting from not hoisting in general.  Whatever distinguishes
 * those two TUs, it is narrower than this pattern.
 *
 * EVERYTHING ELSE IS RIGHT.  Once the hoist is discounted the body lines up:
 * the actor ids, the 0xb9 sound, the sign-extended halfword read, and the
 * (0xb - (v << 1)) << 4 position math all reproduce.  The read uses the
 * `*(short *)(g + (unsigned int)0)` idiom taken from the kin file
 * src/overlays/rom_7c6bac/ovl_30_c_c_a_a_c.c, which is what produces the ROM's
 * `mov r2, #0 / ldrsh r3, [r5, r2]` register-offset form rather than an
 * immediate-offset load.  That transfer worked exactly as intended; it is only
 * the constant hoist standing between this and a match.
 */
extern int iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetAnim(int a, int n);
extern void __MapActor_SetSpeed(int a, int x, int y);
extern void __MapActor_WaitMovement(int a);
extern void __Func_809228c(int a, int x, int y);
extern void __Func_809202c(void);
extern void OvlFunc_903_2008348(void);

void OvlFunc_903_200843c(void)
{
    unsigned char *g;
    int k;
    int v;
    int x;

    g = (unsigned char *)iwram_3001ebc;
    __CutsceneStart();
    __MapActor_SetAnim(0, 8);
    __CutsceneWait(0x14);
    __MapActor_SetSpeed(0, 0x3333, 0x1999);
    __MapActor_SetSpeed(9, 0x3333, 0x1999);
    __PlaySound(0xb9);
    k = 0xb6 << 1;
    g += k;
    v = *(short *)(g + (unsigned int)0);
    x = (0xb - (v << 1)) << 4;
    __Func_809228c(0, x, 0);
    __Func_809228c(9, x, 0);
    __MapActor_WaitMovement(0);
    __MapActor_WaitMovement(9);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0, 1);
    OvlFunc_903_2008348();
    __Func_809202c();
    __CutsceneEnd();
}
