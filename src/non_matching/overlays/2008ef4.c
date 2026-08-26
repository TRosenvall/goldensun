/* OvlFunc_898_2008ef4  --  0x02008ef4, asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_a_c.s
 * and its byte-identical twin OvlFunc_901_2008a80  --  0x02008a80,
 * asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a.s
 *
 * Source asm: goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_a_c.s
 *
 * BLOCKER CLASS: argument precompute. Twelfth member, and the diagnosis is the
 * one already in HANDOFF.md -- this is a compiler difference, not a spelling.
 *
 * Status: 30 lines against 30, TWENTY-EIGHT IDENTICAL. The two that differ are
 * a transposition:
 *
 *     rom    lsl r1, #8 / mov r0, #0  / lsl r2, #7
 *     ours   lsl r1, #8 / lsl r2, #7  / mov r0, #0
 *
 * WHAT THE ROM'S ORDER MEANS, read off calls.c rather than guessed:
 * `precompute_register_parameters` copies every argument whose `rtx_cost > 2`
 * into a pseudo BEFORE any hard register is loaded, and `load_register_
 * parameters` then fills r0..r3 forward. So an argument's shift landing AFTER
 * `mov r0, #0` says that argument was NOT precomputed.
 *
 * The ROM has `lsl r1, #8` before `mov r0, #0` and `lsl r2, #7` after it --
 * the second argument was precomputed and the third was not. Both are
 * `0x80 << N`, identical in shape and in `arm_rtx_costs`, so no C expression
 * separates them: gcc-2.96 here precomputes both or neither. Ours precomputes
 * both, which is the whole difference.
 *
 * TRIED AND MEASURED, all 2-of-30:
 *   0x8000, 0x4000 as plain literals               2
 *   0x80 << 8, 0x8000 >> 1                         2
 *   no prototype on __MapActor_SetSpeed            2
 *   `int` return type on it (the r0-liveness lever) 2
 *   both speeds through named `int` locals         2
 *
 * FLAGS, also measured:
 *   -O2                        2   (this file)
 *   -O2 -fno-rerun-cse-after-loop  2
 *   -O2 -fno-gcse                  2
 *   -O2 -fno-strict-aliasing       2
 *   -O2 -fno-schedule-insns        2
 *   -O2 -fno-schedule-insns2      13   much worse
 *   -O1                           13   much worse
 *
 * The last two are worth keeping: post-reload scheduling is what produces the
 * near-match, so the remaining transposition is a tie broken by the order the
 * insns were GENERATED in -- which is the precompute decision above. Turning
 * the scheduler off does not expose the ROM's order, it destroys the match.
 *
 * The C below is believed correct and is what should be used if the precompute
 * class is ever cracked at the compiler level; two functions come with it.
 */

extern unsigned char iwram_3001ebc[];
extern void __MapActor_SetSpeed(unsigned int, int, int);
extern void __Func_809218c(int, int, int);
extern void __Func_8091e9c(int);

void OvlFunc_898_2008ef4(int a, int b, int c)
{
    char *base;

    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    __Func_809218c(0, a, b);
    base = *(char **)iwram_3001ebc;
    *(int *)(base + (0xe4 << 1)) = 0x10;
    __Func_8091e9c(c);
}
