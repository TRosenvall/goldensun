/* Cluster Task_BlitPreAnim..Task_BlitPreAnim extracted from goldensun/asm/rom_b5000/rom_c10e8_a_a_a.s.
 *
 * Total .text for this TU = 56 bytes (= 0x38).
 * Preserves the original ROM layout when slotted between
 * asm/rom_b5000/rom_c10e8_a_a_a_a.o and asm/rom_b5000/rom_c10e8_a_a_a_c.o in
 * goldensun/stage1.ld.
 *
 * Two levers, both required and neither sufficient alone:
 *
 *   - `pop {r1} / bx r1` says the function RETURNS A VALUE: r0 is still live
 *     at the epilogue, so the return address cannot land there.  Declaring it
 *     `int` and tail-returning the call reproduces that.  The two early paths
 *     return no value, which is what the ROM does -- they branch straight to
 *     the epilogue without setting r0.
 *   - The third argument is written inline as `0x80 << 7` rather than through
 *     a named local.  Through a local, gcc emits the shift immediately after
 *     the `mov`; the ROM schedules the pooled second argument between them.
 *
 * Both pointer loads happen before the first test, matching the ROM, which
 * reads gPtrs+0xa0 and gPtrs+0x9c up front and only then compares.
 */
extern unsigned char gPtrs[];
extern int BlitFade_Div4(void *dst, void *src, int n);

int Task_BlitPreAnim(void)
{
    unsigned char *g;
    unsigned char *p;
    void *dst;
    int *slot;
    unsigned int off;

    g = gPtrs;
    dst = *(void **)(g + 0xa0);
    p = *(unsigned char **)(g + 0x9c);
    if (dst == 0)
        return;
    off = 0x9e << 5;
    slot = (int *)(p + off);
    if (*slot == 0)
        return;
    *slot = 0;
    return BlitFade_Div4(dst, (void *)0x6004000, 0x80 << 7);
}
