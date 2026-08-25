/* Cluster Func_80f037c..Func_80f037c extracted from goldensun/asm/rom_f0000/rom_f0254_a.s.
 *
 * Total .text for this TU = 74 bytes (= 0x4a).
 * Placed in the run in goldensun/stage1.ld.
 *
 * Fills a 512-word buffer in four runs: 32 words of 0x01ff01ff, 240 words of a
 * value stepping by 0x00020002 from 0x00010000, 48 more of 0x01ff01ff, and 192
 * zeroes. A packed pair of 16-bit ramps.
 *
 * BUILT WITH -fno-gcse (see GCSE_CFLAGS in the Makefile). At -O2 gcc SINKS the
 * `ldr r4, =0x20002` past the loop label, so the step constant is re-loaded on
 * every one of the 240 iterations:
 *
 *      rom    ldr r4, =0x20002 / mov r3, #0xef / L1: sub r3, #1 / ...
 *      ours   mov r3, #0xef / L1: ldr r4, =0x20002 / sub r3, #1 / ...
 *
 * That is global CSE doing partial-redundancy motion in the wrong direction for
 * our purposes, and it is not reachable from C: assigning the constant at the
 * top of the function with the others, or immediately before the counter, or
 * immediately after it, all produce byte-identical output. Three placements
 * tried.
 *
 * FIVE FLAGS WERE PROBED and only this one moves it: -fno-strict-aliasing,
 * -fno-strength-reduce and -fno-rerun-cse-after-loop are byte-identical, and
 * --no-sched2 is worse (7 differing lines rather than 3).
 *
 * The loops are `do { i--; store; } while (i >= 0)` -- the decrement BEFORE the
 * store, which is what the ROM's `sub r3, #1 / stmia r0!, {r1}` order says. A
 * post-decrement in the condition puts the two the other way round.
 */

void Func_80f037c(int *p)
{
    int w;
    int v;
    int d;
    int i;
    int z;

    w = 0x80 << 9;
    v = 0x1ff01ff;
    i = 0x1f;
    do {
        i--;
        *p = v;
        p++;
    } while (i >= 0);
    d = 0x20002;
    i = 0xef;
    do {
        i--;
        *p = w;
        p++;
        w += d;
    } while (i >= 0);
    i = 0x2f;
    do {
        i--;
        *p = v;
        p++;
    } while (i >= 0);
    z = 0;
    i = 0xbf;
    do {
        i--;
        *p = z;
        p++;
    } while (i >= 0);
}
