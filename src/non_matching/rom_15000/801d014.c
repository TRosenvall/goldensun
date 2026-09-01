/* Func_801d014 (0x0801d014) -- NON-MATCHING.
 * Blocker class: scheduling WITHIN a statement -- which of a copy's two
 * addresses is formed first.
 *
 * 74 lines against the ROM's 74, 35 differing -- and 35 is exactly five times
 * seven, because the function does the SAME seven-instruction copy five times
 * and every one of them is transposed the same way:
 *
 *     rom    ldr r1, =gState / ldr r0, =0x205 / add r3, r1, r0
 *            ldr r0, =0x594 / ldrb r2, [r3] / add r3, r4, r0 / strb r2, [r3]
 *     ours   ldr r0, =0x594 / ldr r1, =gState / add r2, r4, r0
 *            ldr r0, =0x205 / add r3, r1, r0 / ldrb r3, [r3] / strb r3, [r2]
 *
 * The ROM forms the SOURCE address first, loads, then forms the destination.
 * gcc forms the destination first. Everything else -- the pooled offsets, the
 * named `gState` base, the DMA3_CLEAR, the two calls -- is byte-exact.
 *
 * MEASURED (rom 74 lines, all at the ROM's exact length):
 *   `p[0x594] = g[0x205];` written plainly            74, 35  <- best
 *   `t = g[0x205]; p[0x594] = t;` (name the VALUE)    74, 43
 *   `q = g + 0x205; p[0x594] = *q;` (name the ADDR)   74, 40
 *   constant store before the copy in each pair       74, 56
 *   -fno-gcse                                         74, 35 (inert)
 *   -fno-strength-reduce                              74, 35 (inert)
 *   -fno-schedule-insns2                              74, 57 (worse)
 *
 * BOTH MATERIALISATION LEVERS MAKE IT WORSE, which is the informative part.
 * Batch 172 established that naming a value forces it to exist, and batches 174
 * and 176 used that to move loads and pick addressing forms. Here naming either
 * the loaded byte or the source address pins the wrong half EARLIER and costs
 * five to eight lines. The lever moves things forward, and what this function
 * needs is the destination moved LATER -- which no name can express, because a
 * name cannot delay a computation, only anchor it.
 *
 * That is worth stating as the limit of the rule: **naming is a floor, not a
 * ceiling.** It can stop gcc sinking something past a point; it cannot stop gcc
 * hoisting something above one.
 *
 * WHAT IS RIGHT: `DMA3_CLEAR(p, 0xc5 << 3)` for the `0x8500018a` count word;
 * the named `gState` base (without it each `gState + 0xNNN` folds into a single
 * pooled symbol); the pooled offsets, which gcc chooses on its own -- 0x594 and
 * 0x205 pool because they are neither imm8 nor imm8<<n, while 0x598 and 0x20c
 * come out as `mov #0xb3 / lsl #3` and `mov #0x83 / lsl #2` from writing the
 * plain values; and `StartTask(Func_801cf48, 0xc8 << 4)`.
 *
 * NEXT: nothing source-level.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *galloc_ewram(int tag, int size);
extern unsigned char gState[];
extern void Func_801cf48(void);
extern int StartTask(void *fn, int pri);

void Func_801d014(void)
{
    unsigned char *p;
    unsigned char *g;

    p = galloc_ewram(0x14, 0xc5 << 3);
    DMA3_CLEAR(p, 0xc5 << 3);
    g = gState;
    p[0x594] = g[0x205];
    p[0x599] = 0x18;
    p[0x595] = g[0x206];
    p[0x59a] = 0xf;
    p[0x596] = g[0x83 << 2];
    p[0x59b] = 3;
    p[0x597] = g[0x20a];
    p[0x59c] = 2;
    p[0xb3 << 3] = g[0x22a];
    p[0x59d] = 2;
    StartTask(Func_801cf48, 0xc8 << 4);
}
