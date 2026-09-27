/* Func_80064b8  --  0x080064b8, was asm/rom_c0/rom_5cf8_a_a_c_c.s (this function
 * alone), so it converts whole.
 *
 * The PRE-HEADER LOAD MERGE, retired (was src/non_matching/preheader_load_merge.c,
 * "no known fix"). In .19.flow2 the pre-header order is already the ROM's; sched2
 * then moves the counter's `mov #0` into the load-use stall, which leaves the
 * pre-header load as the last insn before the unconditional `b`, and jump2's
 * find_cross_jump (jump.c:1428) matches it against the load before the loop's
 * label and SINKS it into the loop. `do { } while (0);` between the load and the
 * counter's initialisation is a scheduling barrier that emits nothing, so the two
 * tails no longer match and nothing is merged.
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
    do { } while (0);
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
