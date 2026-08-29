/* OvlFunc_881_2009c08 -- NOT MATCHING. 34 differing of 49; ours 52 lines.
 * ref: asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a.s
 *
 * BLOCKER: constant hoisted across a call, and this is the CLEANEST instance
 * found -- it is the only defect in the function.  0x16f and 0x171 are each
 * passed to two calls in one straight-line block; gcc pools each once into
 * r5/r6 well before its first use and copies `mov r0, r6` at each site, adding
 * a two-register push the ROM does not have.  The ROM re-issues
 * `ldr r0, =0x16f` at every site.
 *
 * CONTROL: change the second pair to 0x16d/0x173 and the function is 49 lines
 * with exactly 2 differing positions -- both of them the constants I changed.
 * So with the real constants everything else is instruction-for-instruction
 * exact and the CSE accounts for all 34.
 *
 * MEASURED, all identical at 34: -fno-rerun-cse-after-loop (the flag the tree
 * uses for the read-then-write form of this class), -fno-gcse,
 * -fno-cse-follow-jumps, -fno-cse-skip-blocks, -fno-expensive-optimizations,
 * -fno-caller-saves, -fno-function-cse, and the CSE flags combined.
 * -O1 and -fno-schedule-insns2 are 35.
 *
 * This belongs with src/non_matching/rom_7d30e0/2009838.c -- the straight-line
 * members of the class the basic-block lever cannot reach.
 */
extern void __Func_808c4c0(void);
extern void __Func_80936a0(int, int);
extern void __Func_8093710(void);
extern void __Func_808c44c(void);
extern void __Func_80925cc(int, int);
extern void __MessageID(int);
extern void __ActorMessage(int, int);
extern void __CutsceneWait(int);
extern void __PlaySound(int);
extern void __Func_802899c(int, int);
extern void __ClearFlag(int);
extern void __SetFlag(int);
extern void __Func_80aa56c(void);
extern void __MapActor_Jump(int, int, int);
extern void __Func_8091eb0(int, int);

void OvlFunc_881_2009c08(void)
{
    __Func_808c4c0();
    __Func_80936a0(0x80 << 9, 6);
    __Func_8093710();
    __Func_808c44c();
    __Func_80925cc(8, 2);
    __MessageID(0xc66);
    __ActorMessage(8, 0);
    __CutsceneWait(0x1e);
    __PlaySound(0x6f);
    __Func_802899c(0, 2);
    __ClearFlag(0x16f);
    __ClearFlag(0x171);
    __Func_80aa56c();
    __MapActor_Jump(8, 4, 0x1e);
    __MessageID(0xc67);
    __ActorMessage(8, 0);
    __ClearFlag(0x16f);
    __SetFlag(0x171);
    __Func_80aa56c();
    __CutsceneWait(0x1e);
    __Func_8091eb0(0xc, 6);
}
