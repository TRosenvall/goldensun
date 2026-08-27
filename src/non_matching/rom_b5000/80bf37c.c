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
 * A SECOND CLUSTER HAS THE SAME BLOCKER. Func_80bf250, Func_80bf2b4 and
 * Func_80bf318 are their own three-member shape group -- the same counter tick
 * with an extra signed companion byte at +0x133 -- and screening the first in
 * batch 91 gives 50 lines against 49 with 31 differing, the same `mov r0, #0`
 * hoisted above the first test. So this park stands for SIX functions.
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
 * BATCH 97 -- THE POSITIVE-TEST LEVER DOES NOT APPLY HERE, and that is worth
 * recording because batch 96 found it defeating what looked like this same
 * blocker in src/non_matching/ovl_common/common0_18.c. There, turning
 * `if (n == 0) return 0;` into `if (n != 0) { ... return n; } return 0;` moved
 * the hoisted constant back into its block, 30 differing to 8.
 *
 * Three restructurings were measured against this function and all are WORSE
 * than the 7 the form below gives:
 *
 *   return 1 as the fall-through, with the call test inside a nested if   18
 *   the same with the loaded byte in its own extra local                 18
 *   the loaded byte as `unsigned char` and the working copy `int`        20
 *   return 0 as the fall-through with two locals                    31/26
 *
 * So the return-constant hoist has at least TWO distinct causes. The one
 * common0_18 has is reachable by restructuring the exits; the one here is not.
 * The distinguishing feature is probably that common0_18 has a single early
 * return against a long body, while this function has FOUR exits, two returning
 * each value, and gcc has a real choice about which constant to preload. Do not
 * spend another round on exit shapes here.
 *
 * The unexplained instruction remains `ldrb r2, [r5] / mov r3, r2` -- a
 * redundant register copy in the ROM, with r2 dead immediately after. Two-local
 * spellings do not produce it.
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
