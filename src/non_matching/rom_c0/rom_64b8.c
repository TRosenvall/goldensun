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
