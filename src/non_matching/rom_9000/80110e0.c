/* Func_80110e0 -- asm/rom_9000/rom_108e4.s
 *
 * BLOCKER: LICM + one spare register. 49 of 61, and OURS IS SHORTER (57 vs 61).
 *
 * Two identical tile-copy loops, 32 iterations each, reading an index from
 * ewram_2020000 and using it to fetch a pair of halfwords out of gBuffer.
 * The address setup before the first loop is EXACT -- the signed halving of
 * the argument, both masks, both shifts, both bases.
 *
 * THE WHOLE RESIDUE IS ONE DECISION, taken inside the loop body:
 *
 *     rom   ldr r5, =gBuffer     (RELOADED every iteration)
 *           add r3, r2, r5  / ldrh r3, [r3]
 *           add r5, #2                       <- bumps the BASE, not the offset
 *           add r3, r2, r5  / ldrh r3, [r3]
 *
 *     ours  ldr r6, =gBuffer     (hoisted out of the loop, held in r6)
 *           ldrh r3, [r6, r2]                <- register-offset, one insn
 *
 * gcc hoists the loop-invariant base and then addresses both halfwords with
 * register-offset loads. That is strictly better code -- which is why ours is
 * FOUR LINES SHORTER than the ROM -- and it costs one extra callee-saved
 * register, so we `push {r5, r6, r14}` where the ROM pushes `{r5, r14}`.
 *
 * THREE FORMULATIONS, ALL FOLDED TO THE SAME OUTPUT:
 *   gBuffer[v * 2] and gBuffer[v * 2 + 1]                  57 lines, 49 differ
 *   explicit byte arithmetic, (char *)gBuffer + v and + 2 + v
 *                                                          57 lines, 49 differ
 *   named entry pointer, e = gBuffer + *src * 2; e[0], e[1]
 *                                                          57 lines, 50 differ
 *
 * The middle one is the informative negative. docs/elevation.md records an
 * "explicit pointer" lever that fixed exactly this addressing choice in
 * SetTextColor; here gcc canonicalises the byte arithmetic straight back into
 * register-offset addressing and the lever does not survive the fold.
 *
 * WHY IT RESISTS. Both differences follow from the hoist, and the hoist is
 * profitable because a register is free. To match, gcc must be denied that
 * register -- the same shape as Func_80f4100 in src/non_matching/rom_f4000/,
 * where the ROM also spends a register gcc correctly declines to spend. No
 * spelling of a loop body creates register pressure that is not already there.
 *
 * NOT TRIED: `volatile` on the base, which would defeat the hoist but changes
 * the semantics rather than the spelling, and would be inventing a qualifier
 * the ROM gives no evidence for.
 */
extern unsigned short ewram_2020000[];
extern unsigned short gBuffer[];

void Func_80110e0(int x)
{
    unsigned short *src;
    unsigned short *dst;
    int i;
    int v;

    src = ewram_2020000 + ((x / 2) & 0x1f) * 64;
    dst = (unsigned short *)(0x6004000 + (x & 0x3e) * 64);
    i = 0;
    do {
        v = *src;
        dst[0] = gBuffer[v * 2];
        dst[0x20] = gBuffer[v * 2 + 1];
        i++;
        dst++;
        src += 2;
    } while (i <= 0x1f);
    dst += 0x7e0;
    src += 0x7c0;
    i = 0;
    do {
        v = *src;
        dst[0] = gBuffer[v * 2];
        dst[0x20] = gBuffer[v * 2 + 1];
        i++;
        dst++;
        src += 2;
    } while (i <= 0x1f);
}
