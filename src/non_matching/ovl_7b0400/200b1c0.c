/* OvlFunc_925_200b1c0 (0x0200b1c0) -- NON-MATCHING.
 * Blocker class: SCRATCH-REGISTER SELECTION (see docs/elevation.md).
 *
 * 36 lines against the ROM's 36, 13 differing. The instruction sequence agrees
 * throughout; the differences are a three-way register rotation of the
 * loop counter, the walking pointer and the shifted parameter.
 *
 *     rom    mov r2, r1 / asr r2, #0x14 ... r4 = counter, r1 = pointer
 *     ours   asr r1, #0x14              ... r1 = counter, r4 = pointer
 *
 * The one thing that looked orderable is not. The ROM COPIES the parameter
 * before shifting it (`mov r2, r1 / asr r2, #0x14`) where we shift in place;
 * writing the shift into its own named local, which is the copy-then-modify
 * spelling that reads as, is byte-identical -- 36 lines, 13 differing either
 * way.
 *
 * WHAT IS RIGHT: the whole body on the first screen. The `0x40 - (y >> 20)`
 * base with its two derived bounds, the `(unsigned)(ex - 4) <= 4` range test
 * written as a subtract-and-compare (which is what produces the ROM's
 * `sub r2, #4 / cmp r2, #4 / bhi`), the two-sided bound on the second
 * coordinate, the `ldmia`/`stmia` walk over the slot array and the output
 * cursor, and the unsigned `i <= 0x41` loop.
 *
 * NEXT: nothing source-level. Per the recognition rule recorded in batch 171,
 * this got one probe past the point where the diff became a register rotation
 * at exact length, and then stopped.
 */
extern char *iwram_3001ebc;

void OvlFunc_925_200b1c0(int *out, int y)
{
    char *blk;
    int **p;
    int *e;
    unsigned int i;
    int a;
    int lo;
    int hi;
    int ex;
    int ez;

    blk = iwram_3001ebc;
    a = 0x40 - (y >> 20);
    i = 0;
    lo = a + 8;
    hi = a + 0xb;
    p = (int **)(blk + 0x14);
    do {
        e = *p++;
        if (e != 0) {
            ex = e[2] >> 20;
            ez = e[4] >> 20;
            if ((unsigned int)(ex - 4) <= 4 && lo <= ez && ez < hi)
                *out++ = i;
        }
        i++;
    } while (i <= 0x41);
}
