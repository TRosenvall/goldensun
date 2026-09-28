/* Func_80105d4 (CopyMapRectFull) @ 0x080105d4 -- NON-MATCHING.
 *
 * NON-MATCHING: 122 encodings of 145 differ (objcmp).
 * objcmp verbatim:
 *   XX SIZE  ref 304 bytes, ours 312
 *   XX ENCODINGS differ in 122 place(s) (ref 145, ours 149)
 * Reference 145 encodings / 304 bytes; ours 149 / 312 -- FOUR OVER, so the 122
 * measures the shift too.  Relocations: same four symbols, different offsets.
 * First divergence at index 7: `sub sp, #0x24` (ROM) vs `sub sp, #0x30` (ours).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_9000/rom_10424_a.s --func Func_80105d4
 *
 * asm/rom_9000/rom_10424_a.s holds THREE functions and NO data section
 * (datacheck.py); landing needs a split.  See the CopyMapTiles park beside this.
 *
 * SAME FUNCTION AS Func_8010788 except that the destination record is copied
 * WHOLE (`*dst = t`, a bare `stmia r0!, {r3}`) instead of merged, and the tile
 * index for the refresh is masked off separately (`and r7, r3`).  Signed
 * extents, argument order r0=sx r1=sy r2=w r3=h, dx/dy on the stack.
 *
 * BLOCKER: THE SAME TWO-STAGE LOOP-INVARIANT HOIST as Func_8010788 -- read that
 * park's header for the mechanism and the measurement table.  This member is
 * ONE SLOT WORSE than its two siblings (frame 0x30, not 0x2c) because the
 * separately masked tile index is a third value the x loop must carry.
 *
 * MEASURED HERE:
 *   `t &= 0xfff` reusing the stored value (this file)   122 [149/145] frame 0x30
 *   a separate `u = t & 0xfff` for the refresh          122 [149/145] frame 0x30
 * The two are byte-identical, so the extra slot is not the spelling of the mask.
 */
extern unsigned int gBuffer[];
extern unsigned char *iwram_3001e70;
extern unsigned char ewram_2020000[];
extern unsigned char ewram_2020004[];

void Func_80105d4(int sx, int sy, int w, int h, int dx, int dy)
{
    unsigned int *src;
    unsigned int *dst;
    unsigned char *p;
    int *q;
    int win[6];
    int i, x, y, k;
    unsigned int t;

    src = gBuffer + ((sy << 7) + sx);
    dst = gBuffer + ((dy << 7) + dx);
    p = iwram_3001e70 + (0x82 << 1);
    q = win;
    for (i = 0; i < 3; i++) {
        q[0] = *(int *)p >> 20;
        q[1] = *(int *)(p + 4) >> 20;
        p += 0x30;
        q += 2;
    }
    for (y = dy; y < dy + h; y++) {
        for (x = dx; x < dx + w; x++) {
            t = *src++;
            *dst = t;
            t &= 0xfff;
            dst++;
            q = win;
            for (k = 0; k < 3; k++) {
                i = ((((y & 0xf) + (k << 4)) << 5) + (x & 0xf)) << 2;
                if (q[0] <= x && q[0] + 0x10 > x && q[1] <= y && q[1] + 0xc > y) {
                    *(int *)(0x6002800 + i) = *(int *)(ewram_2020000 + (t << 3));
                    *(int *)(0x6002840 + i) = *(int *)(ewram_2020004 + (t << 3));
                    break;
                }
                q += 2;
            }
        }
        src += 0x80 - w;
        dst += 0x80 - w;
    }
}
