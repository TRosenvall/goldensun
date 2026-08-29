/* Func_8012350 @ 0x08012350
 *
 * Source asm: goldensun/asm/rom_9000/rom_1219c_a_c_c_a.s
 *
 * NOT SPLIT. The .s still holds all four of its functions.
 *
 * Spins until two fields of the block at iwram_3001e70 both drop to 0xff or
 * below, giving up after 0x12c frames, then clears a third field.
 *
 * Blocker: THE PRE-HEADER LOAD MERGE. 26 of 27, short by exactly one
 * instruction. See src/non_matching/preheader_load_merge.c for the class, its
 * three members, everything tried across them, and why the compiler-flag
 * attack is closed rather than open.
 *
 * Two things specific to this member:
 *
 *  * the pointer lands in r5 in BOTH the ROM and ours, because it is live
 *    across the WaitFrames call and has to be callee-saved. The
 *    address-register-reuse residue that shows up elsewhere in this family
 *    (src/non_matching/rom_9000/rom_c0cc.c) does not appear here.
 *  * the 0x12c limit is `mov r3, #0x96 / lsl r3, #1` INSIDE the loop, so it is
 *    written as a named local. Left as a literal it hoists into the pre-header.
 *    That is the same direction as Func_80064b8 and the OPPOSITE of
 *    src/rom_b5000/rom_bffb8_a_a_a_b.c -- the loop-invariant lever runs both
 *    ways and has to be read off the ROM each time.
 *
 * PROGRESS AND A DIAGNOSIS, batch 43. Found by tools/rank_parks.py --flags.
 *
 * AT -O1 THIS GOES TO 2, and all three members of this family land on the SAME
 * two instructions -- Func_80064b8 (25), Func_8012350 (27) and
 * OvlFunc_956_20081c8 (26). At -O2 the diff is the pre-header load merge this
 * file was named for; -O1 removes that and leaves one thing:
 *
 *     rom    mov r3, #0x96 / add r6, #0x1 / lsl r3, #0x1 / cmp r6, r3
 *     ours   add r6, #0x1  / mov r3, #0x96 / lsl r3, #0x1 / cmp r6, r3
 *
 * The loop limit is a TWO-INSTRUCTION CONSTANT and the ROM splits its mov/lsl
 * pair around the loop counter's increment. That is the arg-interleave shape,
 * on a comparison operand rather than a call argument.
 *
 * IT IS UNREACHABLE, AND THE MECHANISM SAYS SO IN ADVANCE. The basic-block
 * lever retires that shape, and it requires REG_N_REFS == 2 -- and REG_N_REFS
 * is incremented by `bb->loop_depth + 1`, so a set-once-used-once pseudo INSIDE
 * A LOOP counts 4, not 2. See docs/elevation.md.
 *
 * Confirmed rather than assumed, all at -O1:
 *
 *     the limit as a named local assigned in the loop body   2 of 27  (this file)
 *     the limit written inline in the comparison             3 of 27
 *     the limit assigned after the increment                 2 of 27
 *     the limit assigned BEFORE the loop, i.e. the lever     7 of 27  (worse)
 *
 * So the three are one blocker, not three, and the blocker is a known-closed
 * one rather than an open question. A per-file -O1 rule would take them from
 * three-or-more out to two, which is not worth a build-system change on its
 * own; it is recorded here so that whoever finds a way through the loop case
 * knows these three come with it.
 */
#include "gba/types.h"

extern u32 iwram_3001e70;
extern void WaitFrames(s32 n);

struct T { s32 pad_00; s32 f4; s32 f8; s32 fc; };

void Func_8012350(void)
{
    struct T *p;
    s32 i;
    s32 v;
    s32 lim;

    p = (struct T *)iwram_3001e70;
    v = p->f4;
    i = 0;
    goto check;
loop:
    WaitFrames(1);
    lim = 0x96 << 1;
    i++;
    if (i >= lim)
        goto out;
    v = p->f4;
check:
    if (v > 0xff)
        goto loop;
    if (p->f8 > 0xff)
        goto loop;
out:
    p->fc = 0;
}
