/* Func_80c0130 (0x080c0130) -- NON-MATCHING.
 * Blocker class: scheduling -- one `add` hoisted across a volatile store.
 *
 * 32 lines against the ROM's 32, THREE differing, same instruction multiset:
 *
 *   rom   ldr r1, =0x400000c / strh r3, [r1] / ldr r3, =0x40000b0
 *         ldr r2, =0xa2600001 / add r0, #0x22 / stmia r3!, {r0, r1, r2}
 *   ours  ldr r1, =0x400000c / add r0, #0x22 / strh r3, [r1]
 *         ldr r2, =0xa2600001 / ldr r3, =0x40000b0 / stmia r3!, {r0, r1, r2}
 *
 * gcc hoists the DMA source's `+ 0x22` above the volatile `strh` to REG_BG2CNT
 * -- legal, since a non-volatile computation may cross a volatile access -- and
 * swaps the two pool loads. The original build did neither.
 *
 * MEASURED (rom 32 lines):
 *   baseline                                              32, 3
 *   `q = p + 0x22;` as its own statement AFTER the strh    32, 3
 *   -fno-strict-aliasing / -fno-gcse / -fno-strength-reduce
 *     / -fno-rerun-cse-after-loop                          32, 3 (all inert)
 *   -fno-schedule-insns2                                   32, 6 (worse)
 *
 * -fno-schedule-insns2 doubling the count is the recorded "destroying the
 * evidence" signature, which per batch 173 rules out the scheduler pass rather
 * than merely that flag -- so this is insn placement decided earlier, and no
 * spelling reaches it. Naming the address after the store was the one probe
 * with a mechanism behind it (batch 172's materialisation-point rule) and it is
 * byte-identical.
 *
 * WHAT IS RIGHT, and worth reading before touching this again -- three separate
 * things gcc reproduced on its own that look like they would need levers:
 *
 *   1. TWO ADJACENT GLOBALS FROM ONE POOL ENTRY, at a NEGATIVE offset. The ROM
 *      reaches iwram_3001e78 as `mov r3, r2 / sub r3, #0x88` off
 *      iwram_3001f00's pool address. `extern unsigned char iwram_3001f00[];`
 *      with `*(unsigned char **)(iwram_3001f00 - 0x88)` gives exactly that.
 *      Same rule as batch 174's `ldmia` finding -- adjacent globals the ROM
 *      reaches from one pool entry are one array -- and it works backwards too.
 *
 *   2. THE DMA3 BASE DERIVED FROM THE DMA0 BASE. The ROM's second transfer uses
 *      `add r3, #0x24` off &REG_DMA0SAD rather than a fresh &REG_DMA3SAD pool
 *      load. Writing DMA0_SET followed by DMA3_SET produces it -- gcc's
 *      constant CSE finds 0x40000d4 = 0x40000b0 + 0x24 by itself.
 *
 *   3. THE SECOND DMA'S DESTINATION LIKEWISE. `add r1, #0x14` off &REG_BG2CNT
 *      comes from plainly writing `(void *)&REG_BG2PA`.
 *
 * NEXT: nothing source-level.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char iwram_3001f00[];

void Func_80c0130(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    int n;

    a = *(unsigned char **)iwram_3001f00;
    if (*(int *)(a + 8) != 2)
        return;
    b = *(unsigned char **)(iwram_3001f00 - 0x88);
    n = *(int *)b;
    p = b + ((n * 5) << 6);
    REG_BG2CNT = *(unsigned short *)(p + 0x20);
    DMA0_SET(p + 0x22, (void *)&REG_BG2CNT, 0xa2600001);
    DMA3_SET(b + 0x10, (void *)&REG_BG2PA, 0x84000004);
}
