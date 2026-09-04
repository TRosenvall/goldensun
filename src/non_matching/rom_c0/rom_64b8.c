/* Func_80064b8 @ 0x080064b8  (WaitForTrackEnd, per the annotation on the .s)
 *
 * Source asm: goldensun/asm/rom_c0/rom_5cf8_a_a_c_c.s
 *
 * NOT SPLIT, and it would not need one -- the .s holds only this function and
 * no data. It is left in place because it does not match.
 *
 * Blocks on WaitFrames(1) until both of two globals read zero, giving up after
 * 0x927c0 frames. Twenty-four instructions against twenty-five.
 *
 * The goto-loop lever from docs/elevation.md carries this most of the way: the
 * ROM tests at the top and jumps into the test, and no structured spelling
 * reproduces that. `while (g1 || g2) { ... }` gives 22 and the rotated shape;
 * the goto form gives 24 with the loop skeleton exactly right.
 *
 * Blocker: THE PRE-HEADER LOAD MERGE -- see
 * src/non_matching/preheader_load_merge.c for the class, its three members
 * and everything tried across them, including the compiler-flag attack, which
 * is now closed rather than open.
 *
 * gcc FACTORS A LOAD that the ROM performs on both paths. The value is
 * read before the loop and again at the bottom of the body, and both reads
 * reach the same test:
 *
 *     rom    ldr r3, =0x2002080 / ldr r3, [r3] / mov r5, #0 / b .L0
 *            .L1: ... ldr r3, =0x2002080 / ldr r3, [r3] / .L0: cmp r3, #0
 *
 *     ours   ldr r3, =0x2002080 /              mov r5, #0 / b .L0
 *            .L1: ... ldr r3, =0x2002080 /     .L0: ldr r3, [r3] / cmp r3, #0
 *
 * Both emit the address load twice; gcc then tail-merges the DEREFERENCE into
 * the shared test block and saves an instruction. Same family as constant-CSE
 * -- gcc doing more than the original compiler did -- but the mechanism is
 * cross-jumping rather than CSE.
 *
 * TRIED:
 *   1. the goto form with the value in a local assigned in both places (below)
 *      -- 24, the closest
 *   2. `while (ewram_2002080 != 0 || ewram_20023ac != 0)` -- 22, rotated loop
 *   3. duplicating the first test inside the loop body so the two reads reach
 *      different blocks -- 29, gcc keeps both copies and adds a branch
 *
 * (3) is the direct attack on the merge and it overshoots badly, which says the
 * merge is not something an extra copy in the source can steer.
 *
 * The 0x927bf limit IS correctly re-materialised inside the loop by naming it
 * in a local. Left as a literal it gets hoisted into the pre-header, which is
 * the loop-invariant behaviour recorded in src/rom_b5000/rom_bffb8_a_a_a_b.c --
 * that lever runs the other way here and the local is what the ROM wants.
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
  *
 * BATCH 198 -- THE PIN DOES NOT REACH THIS CLASS, and the reason is worth
 * having beside the three members in preheader_load_merge.c. Batches 193-197
 * closed a long run of parks by pinning an argument register, on the principle
 * that a pin AVOIDS a pass rather than arguing with it. That principle has a
 * boundary and this is on the far side of it:
 *
 *     the loaded value pinned to r3 (call-clobbered)   byte-identical to base
 *     the loaded value pinned to r0                    one WORSE, 23 of 25
 *
 * A pin decides WHICH register holds a value and where its own write sits
 * relative to other pinned writes. It says nothing about WHERE IN THE CONTROL
 * FLOW a load is performed, and this residue is entirely that -- gcc factors
 * two loads that reach the same test into one at the merge point, where the ROM
 * performs the load on both incoming paths. The register is not the variable.
 *
 * The r3 result is the informative one: r3 is call-clobbered and the loop body
 * contains a WaitFrames call, so if pinning could force a reload anywhere it
 * would force one here. It does not, because the merged load already sits after
 * the call in the merged block.
 *
*/
#include "gba/types.h"

extern u32 ewram_2002080;
extern u32 ewram_20023ac;
extern void WaitFrames(s32 n);

void Func_80064b8(void)
{
    u32 i;
    u32 v;
    u32 lim;

    v = ewram_2002080;
    i = 0;
    goto check;
loop:
    WaitFrames(1);
    lim = 0x927bf;
    i++;
    if (i > lim)
        return;
    v = ewram_2002080;
check:
    if (v != 0)
        goto loop;
    if (ewram_20023ac != 0)
        goto loop;
}
