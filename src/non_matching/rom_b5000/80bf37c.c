/* Func_80bf37c  --  0x080bf37c, asm/rom_b5000/rom_bbb0c_a_c_c_a.s
 * Func_80bf400  --  0x080bf400   (offset 0x9d << 1, argument 0x46)
 * Func_80bf484  --  0x080bf484   (offset 0x9e << 1, argument 0x32)
 *
 * Source asm: goldensun/asm/rom_b5000/rom_bbb0c_a_c_c_a.s
 *
 * BLOCKER CLASS: a return constant hoisted above the test that needs it.
 * Status: 32 lines against 32 for each, and the seven differing lines come from
 * ONE decision -- five of them are label renumbering behind it.
 *
 *     rom    ldrb r2, [r5] / mov r3, r2   / cmp r3, #0 / beq <shared return 0>
 *     ours   ldrb r3, [r5] / mov r0, #0   / cmp r3, #0 / beq <inline return 0>
 *
 * Both `return 0` paths share a tail in the ROM and gcc hoists the `mov r0, #0`
 * that feeds them above the first test instead, which frees r3 for the loaded
 * byte and leaves the ROM's `mov r3, r2` copy with nothing to correspond to.
 * The two exits then come out in the opposite order and every label swaps.
 *
 * A THREE-MEMBER SHAPE CLUSTER (tools/find_shape.py), differing only in the
 * record offset they tick and the third argument to Func_80bf208, so one fix
 * takes all three. The annotation calls it one of ten near-identical routines;
 * the other seven differ enough in shape not to cluster with these.
 *
 * TWO THINGS WERE SOLVED GETTING HERE and both are worth keeping:
 *
 *   THE DECREMENT IS `v = v + 0xff`, NOT `v--`. The value is a byte and the
 *   result is stored back into one, and gcc emits `sub r3, #1` for the
 *   decrement but `add r3, #0xff` for the addition. The ROM has the add. That
 *   took 12 differing lines to 11.
 *
 *   THE LAST TEST IS WRITTEN POSITIVE. `if (Func_80bf208(...) != 0) { *p = 0;
 *   return 1; } return 0;` gives the ROM's `beq` to the shared tail; the
 *   negative form inverts the branch and costs four more lines. 11 to 7.
 *
 * ALSO MEASURED, all worse: `unsigned char v` (33 lines, 24 differ); the whole
 * body wrapped in `if (v != 0)` with one trailing `return 0` (31/26); a `goto`
 * to a `zero:` label (31/26); a second variable for the decremented value
 * (32/7, no change); re-reading `*p` for the first test (33/25); `!` instead of
 * `== 0` throughout (32/11).
 *
 * Flags: -fno-gcse, -fno-rerun-cse-after-loop, -fno-cse-follow-jumps,
 * -fno-cse-skip-blocks and -fno-thread-jumps all leave it at 7; -O1 is 27.
 */
extern void *_GetUnit(int id);
extern int Func_80bf208(int id, int n, int k);

int Func_80bf37c(int id)
{
    unsigned char *p;
    int v;

    p = (unsigned char *)_GetUnit(id) + (0x9c << 1);
    v = *p;
    if (v == 0)
        return 0;
    v = v + 0xff;
    *p = v;
    if ((unsigned char)v == 0)
        return 1;
    if (Func_80bf208(id, *p, 0x1e) != 0) {
        *p = 0;
        return 1;
    }
    return 0;
}
