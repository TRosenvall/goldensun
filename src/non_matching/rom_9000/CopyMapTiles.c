/* CopyMapTiles (CopyMapRectIndicesU) @ 0x08010424 -- NON-MATCHING.
 *
 * NON-MATCHING: 135 encodings of 150 differ (objcmp).
 * objcmp verbatim:  XX ENCODINGS differ in 135 place(s) (ref 150, ours 150)
 * Reference 150 encodings, ours 150 -- the ROM's length.  Relocations differ in
 * pool ORDER only (same four symbols).  First divergence at index 7:
 * `sub sp, #0x24` (ROM) vs `sub sp, #0x2c` (ours).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_9000/rom_10424_a.s --func CopyMapTiles
 *
 * asm/rom_9000/rom_10424_a.s holds THREE functions -- CopyMapTiles,
 * Func_8010560 and Func_80105d4 -- and NO data section (datacheck.py), so
 * landing needs a split, not a whole-file conversion.  Func_80105d4 is parked
 * beside this one; Func_8010560 (PlayMapRectAnimation, ~46 instructions) is the
 * third and was not attempted, so converting the file whole would take all three.
 *
 * SAME FUNCTION AS Func_8010788 with the argument order of CopyMapTiles
 * (r0=sx, r1=sy, r2=dx, r3=dy, w and h on the stack) and UNSIGNED extents --
 * which is why `w`/`h` are `unsigned int` here and `x`/`y` stay `int`: the loop
 * comparisons promote to unsigned (`bcs`/`bcc`) while the window tests against
 * the `int` array stay signed (`bgt`/`ble`), exactly as the ROM has them.
 *
 * BLOCKER: THE SAME TWO-STAGE LOOP-INVARIANT HOIST as Func_8010788 -- read that
 * park's header for the mechanism, the compiler-source citation and the full
 * 17-spelling measurement table.  Frame 0x2c against the ROM's 0x24; r9 and r11
 * hold 0x6002800 and ewram_2020004 where the ROM holds &win and dx.
 */
extern unsigned int gBuffer[];
extern unsigned char *iwram_3001e70;
extern unsigned char ewram_2020000[];
extern unsigned char ewram_2020004[];

void CopyMapTiles(int sx, int sy, int dx, int dy, unsigned int w, unsigned int h)
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
            t = *src++ & 0xfff;
            *dst = (*dst & 0xfffff000) | t;
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
