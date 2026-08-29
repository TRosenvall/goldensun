/* OvlFunc_930_2008870  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_c_a.s
 * Best screen: 2 instructions in disagreeing regions, of 24 (streams same length).
 *
 * BLOCKER CLASS: argument-setup scheduling.
 *
 * Three straight-line calls.  Everything matches except WHERE the first
 * argument of the last call is materialised:
 *
 *      rom   mov r1, #0xac / mov r2, #0x98 / mov r0, #0xe / lsl r1, #17 / lsl r2, #16
 *      ours  mov r1, #0xac / mov r2, #0x98 / lsl r1, #17 / lsl r2, #16 / mov r0, #0xe
 *
 * The ROM slots `mov r0, #0xe` between the two bases and the two shifts.  That
 * is the scheduler's placement, and it is not reachable from the source.
 *
 * WHAT WAS TRIED -- five spellings, ALL BYTE-IDENTICAL to each other at 2 of 24:
 *
 *  1. `__MapActor_SetPos(0xe, x << 17, y << 16)` with x, y named locals.
 *  2. A named local for the first argument too, assigned AFTER x and y and
 *     BEFORE the shifts -- i.e. source order matching the ROM exactly.
 *  3. The same with the first argument assigned BEFORE x and y.
 *  4. The declaration lever: that local declared first in the list.
 *  5. Compound shifts, `x <<= 17`.
 *
 * gcc fixes argument-setup order after these choices are made, so none of them
 * reach it.
 *
 * NOT THE SCHEDULER BEING WRONG -- it is being right.  `--no-sched2` makes this
 * WORSE, 6 of 24, so this TU wants the scheduler on and the residue is its
 * choice within a sequence it is otherwise getting right.  `--no-rerun-cse` is
 * byte-identical, so that flag is not the answer either.
 *
 * TWO SPELLINGS THAT DID FIX FIVE OF THE ORIGINAL SEVEN, kept below:
 *   - The two stack arguments as separate named locals, so both are
 *     materialised before either is stored.  Passed as literals, gcc walks one
 *     register through both slots: `mov r3, #0x15 / str r3, [sp] /
 *     mov r3, #9 / str r3, [sp, #4]` where the ROM has both movs first.  This
 *     is the stack-arg-pair lever, "two locals with distinct registers" shape.
 *   - Naming the two shift bases, which fixes the first call's r0 placement.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);

void OvlFunc_930_2008870(void)
{
    int e;
    int f;
    int x;
    int y;

    e = 0x15;
    f = 9;
    __Func_8010704(0x55, 9, 1, 1, e, f);
    __Func_808edac(0x64, 0, 0);
    x = 0xac;
    y = 0x98;
    __MapActor_SetPos(0xe, x << 17, y << 16);
}
