/* OvlFunc_943_200c218  [ovl_7c7b9c]  --  0x0200c218
 *
 * Source asm: goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_c.s
 *
 * Places the player, then sets its facing. Fourteen of twenty-five differ, and
 * they are all one cause.
 *
 * Blocker: CONSTANT-CSE ACROSS A CALL, in a STRAIGHT-LINE function. `0xe8 << 16`
 * and `0xa9 << 18` are each passed to __Func_80933f8 and again to
 * __MapActor_SetPos. The ROM rebuilds both for the second call; gcc builds them
 * once into r5 and r6, pays a wider push and pop for the privilege, and copies.
 *
 *     rom    mov r0,#0xe8 / mov r1,#1 / mov r2,#0xa9 / mov r3,#0 /
 *            lsl r0,#16 / neg r1,r1 / lsl r2,#18 / bl ... / bl ... /
 *            mov r1,#0xe8 / mov r2,#0xa9 / lsl r1,#16 / lsl r2,#18
 *     ours   ... r5 and r6 held across both calls, `mov r1,r6 / mov r2,r5` ...
 *
 * A SECOND COUNTER-EXAMPLE TO THE CSE_CFLAGS RULE. This is repetition separated
 * by a call, which is exactly the shape `-fno-rerun-cse-after-loop` is supposed
 * to handle, and the flag is BYTE-IDENTICAL here -- 14 with and without. The
 * first counter-example was OvlFunc_922_2009750, and that one was solved in
 * batch 40 by the basic-block lever with separate locals.
 *
 * THAT LEVER CANNOT REACH THIS ONE, and the reason is the same as for the
 * straight-line arg-interleave cases: the lever needs the values assigned in a
 * block that DOMINATES the uses and is different from them, and this function
 * has no branch at all. Tried anyway, since it costs one screen:
 *
 *     literals at both call sites                          14 of 25
 *     four separate locals, one pair before each call      14 of 25
 *     -fno-rerun-cse-after-loop                            14 of 25
 *     -fno-schedule-insns2                                 18 (worse)
 *     -O1                                                  18 (worse)
 *
 * So the constant-CSE class now splits cleanly by control flow, and the split
 * is worth stating because it says which functions are worth attempting:
 *
 *     repetition separated by a call, WITH a branch     -> basic-block lever
 *     repetition separated by a call, straight-line     -> only CSE_CFLAGS, and
 *                                                          not always even that
 *
 * What this one needs is the same missing construct as
 * src/non_matching/ovl_77dd1c/200c5b8.c and the `-1` triple in
 * src/non_matching/ovl_787e04/20093e4.c: a way to make gcc rematerialise a
 * value inside a single basic block. Three parked shapes, one construct.
 *
 * SETTLED, batch 42, by reading the compiler rather than probing it.
 *
 * gcc-2.96 rebuilds a constant at its use instead of keeping it live in exactly
 * one place -- `update_equiv_regs` in local-alloc.c -- and only when BOTH of
 * these hold:
 *
 *     REG_N_REFS (regno) == 2        set once, used exactly once
 *     REG_BASIC_BLOCK (regno) < 0    the pseudo spans MORE THAN ONE basic block
 *
 * A straight-line function has one basic block, so the second condition can
 * never hold, whatever the C says -- it is a property of the control-flow graph
 * and not of the source. The only other pass that could do it is `combine`, and
 * combine can only fold a constant into its consumer if the target takes it as
 * an immediate, which a two-instruction constant does not.
 *
 * So this is NOT waiting on a construct that has not been found. In plain C it
 * is unreachable, and register pinning -- a fakematch -- is the only way
 * through. See docs/elevation.md and reports/fakematch-worklist.md.
 */
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void *__MapActor_GetActor(int slot);
extern void __WaitFrames(int n);

void OvlFunc_943_200c218(void)
{
    unsigned char *a;
    int v;

    __Func_80933f8(0xe8 << 16, -1, 0xa9 << 18, 0);
    __Func_800fe9c();
    __MapActor_SetPos(0, 0xe8 << 16, 0xa9 << 18);
    a = (unsigned char *)__MapActor_GetActor(0);
    v = 0x80 << 7;
    *(unsigned short *)(a + 6) = v;
    __WaitFrames(1);
}
