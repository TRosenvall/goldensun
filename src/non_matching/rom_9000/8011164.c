/* Func_8011164 (0x08011164) -- NON-MATCHING.
 * Blocker class: loop-invariant motion -- gcc HOISTS a pool load the original
 * build reloaded every iteration.
 *
 * 33 lines against the ROM's 35. The ROM re-materialises `gBuffer` inside the
 * loop and bumps it by two between the two reads:
 *
 *     rom    L0: ldrh r2, [r4] / ldr r5, =gBuffer / lsl r2, #2
 *                add r3, r2, r5 / ldrh r3, [r3] / add r5, #0x2
 *                ... add r3, r2, r5 / ldrh r3, [r3]
 *     ours   ldr r6, =gBuffer (hoisted above the loop) / L0: ...
 *                ldrh r3, [r6, r2] ... add r3, r6, #0x2
 *
 * Writing the reload explicitly -- `g = gBuffer;` as the first statement of the
 * loop body, then `g += 2;` between the reads -- is the right SHAPE and does
 * help: it takes the push list from {r5, r6, r7, lr} to {r5, r6, lr} and 26
 * differing lines to 25. But gcc still recognises `g = gBuffer` as
 * loop-invariant and hoists the initialisation out.
 *
 * MEASURED (rom 35 lines):
 *   `gBuffer + t` and `gBuffer + 2 + t` inline          33, 26, push {r5,r6,r7}
 *   `g = gBuffer;` inside the loop, `g += 2;` between   33, 25, push {r5,r6}
 *   -fno-gcse                                           33, 26 (inert)
 *   -fno-rerun-cse-after-loop                           33, 26 (inert)
 *   -fno-strength-reduce                                33, 26 (inert)
 *   -fno-schedule-insns2                                33, 31 (worse)
 *
 * -fno-gcse BEING INERT IS THE DIAGNOSTIC, and it separates this from the
 * recorded GCSE case. src/rom_f0000/rom_f0254_a_b.c is the mirror image: there
 * gcc SINKS a pool load INTO a loop that the ROM keeps outside, and -fno-gcse
 * fixes it because that motion is global CSE's. Here the motion is the opposite
 * direction and belongs to loop.c's invariant hoisting, which gcc-2.96 has no
 * flag for.
 *
 * So the two directions are DIFFERENT PASSES, and the flag only reaches one:
 *
 *     pool load sunk INTO a loop the ROM keeps it out of  -> -fno-gcse works
 *     pool load hoisted OUT of a loop the ROM reloads in  -> no flag exists
 *
 * WHAT IS RIGHT: the signed `n / 2` expansion (`lsr #31 / add / asr #1`), the
 * `& 0x1f` and `& 0x3e` masks, the unsigned `i <= 0x3f` bound, the 0x80 strides
 * on both pointers, and the `dst + 0x40` second store.
 *
 * NEXT: nothing. Reconsider if a way to suppress loop.c's invariant motion is
 * ever found.
 */
extern unsigned char ewram_2020000[];
extern unsigned char gBuffer[];

void Func_8011164(int n)
{
    unsigned char *src;
    unsigned char *dst;
    unsigned int i;
    int t;
    unsigned char *g;

    src = ewram_2020000 + ((n / 2) & 0x1f) * 4;
    dst = (unsigned char *)0x6004000 + (n & 0x3e);
    for (i = 0; i <= 0x3f; i++) {
        t = *(unsigned short *)src * 4;
        g = gBuffer;
        *(unsigned short *)dst = *(unsigned short *)(t + g);
        g += 2;
        *(unsigned short *)(dst + 0x40) = *(unsigned short *)(t + g);
        dst += 0x80;
        src += 0x80;
    }
}
