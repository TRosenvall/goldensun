/* OvlFunc_953_200a5f0 -- 0x0200a5f0,
 * asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_a_c.s
 *
 * A cutscene script: start, set the leader's speed, fade in, set the walk
 * anim, warp twice, walk to the last mark, fade out, wait, then branch on
 * flag 0x90f to one of two waits.
 *
 * 29 of 43 differing.  Candidate below.
 *
 * BLOCKER: CONSTANT REMATERIALISATION INSIDE ONE BASIC BLOCK.  The ROM
 * rebuilds `mov r2, #0xd6 / lsl r2, #1` at each of three call sites; gcc
 * builds 428 once, parks it in r5, and copies `mov r2, r5` three times --
 * which also grows the prologue to `push {r5, lr}`.  That accounts for the
 * whole residue: the r5 pair, the three copies, and two argument interleaves
 * that follow from r5 being live.
 *
 * WHY IT IS UNREACHABLE HERE, read from gcse.c, not guessed.  Two passes are
 * involved and only the second can undo the commoning:
 *
 *   - cse1 commons the repeat unconditionally.  MEASURED: in the SOLVED
 *     sibling OvlFunc_953_200a3e0, whose source names y1..y6 all `= 0x93 << 2`,
 *     only ONE `(set (reg) (const_int 588))` survives .03.cse; the other five
 *     become copies carrying REG_EQUAL.  Separate named locals do NOT defeat
 *     cse -- the folklore that they do is false, and here the named-local
 *     spellings are strictly worse (40 and 41 versus 29 for plain literals).
 *
 *   - What restores the constants is gcse's CONSTANT PROPAGATION, and cprop is
 *     strictly CROSS-BLOCK.  `cprop_insn` skips a use when
 *     `! oprs_not_set_p (...)`, with gcc's own comment "If the register has
 *     already been set in this block, there's nothing we can do."  And
 *     `find_avail_set` only accepts a set available at the START of the block
 *     (`TEST_BIT (cprop_avin[BLOCK_NUM (insn)], ...)`).
 *
 * So the sibling wins because its `y` assignments sit in block 0, above a
 * leading `if (__GetFlag(5))`, and its uses sit in later blocks: cprop
 * restores six separate constant sets, each pseudo then satisfies
 * `REG_N_REFS == 2 && REG_BASIC_BLOCK < 0`, update_equiv_regs marks it
 * replaceable, and all twelve coordinate pseudos vanish -- that file gets
 * `push {lr}` alone.  This function's only branch is AFTER every constant use,
 * so all three uses live in block 0.  MEASURED: .17.lreg reads "Register 35
 * used 4 times across 28 insns in block 0", and .18.greg says ";; 0 regs to
 * allocate" with "35 in 5".
 *
 * THE GENERAL RULE, worth more than this function: if the ROM rebuilds the
 * same two-instruction constant at two or more call sites, look for a branch
 * that DOMINATES those sites.  If the only branch is after them, or there is
 * none, the shape is unreachable -- cse1 commons it and cprop cannot undo an
 * in-block commoning.  Park immediately; do not sweep spellings.
 *
 * This is the second consequence of the batch-152 straight-line boundary.
 * That entry noted only that `REG_BASIC_BLOCK < 0` never holds in a branchless
 * function; the constant-remat consequence is the larger half.
 *
 * TRIED, none of it moving the r5 residue: six local-variable spellings
 * (literals at the call sites, y-locals immediately before each call, a
 * `k = 0xd6` factor, x/y locals at the top, and both sibling spellings);
 * a ten-way return-type sweep on __Func_8092158; six SetSpeed argument
 * spellings; and fifteen flags -- -fno-gcse, -fno-rerun-cse-after-loop,
 * -fno-expensive-optimizations, -fno-cse-follow-jumps, -fno-cse-skip-blocks,
 * -fno-strength-reduce, -fno-schedule-insns, -fno-force-mem, -fno-peephole,
 * -fcaller-saves, -fno-defer-pop, -fno-omit-frame-pointer.
 * -fno-schedule-insns2 and -O1 both print 15, but that is fewer difflib
 * alignment artefacts over the SAME defects, not an improvement in kind; the
 * r5 pair and the three copies are untouched.  Neither group is justified.
 *
 * Also checked so nobody repeats it: a scan of all 3105 generated asm/**\/*.s
 * for a function that rebuilds the same `mov #C / lsl #n` pair twice inside
 * one basic block finds exactly three, and all three are high-pressure spill
 * cases, not constant remat.  There is no solved precedent for this shape.
 */

extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __Func_8092158(int slot, int x, int z);
extern void __Func_8091e9c(int n);

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
    if (__GetFlag(0x90f) != 0)
        __Func_8091e9c(0x20);
    else
        __Func_8091e9c(0xc);
}
