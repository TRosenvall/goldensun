/* OvlFunc_947_2008cc0 -- NON-MATCHING: 70 encodings of 86 differ (objcmp).
 * 0x02008cc0, asm/overlays/rom_7d0e88/ovl_314_a_c_c_c_c.s (1 of 2 functions).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/2008cc0.c \
 *     asm/overlays/rom_7d0e88/ovl_314_a_c_c_c_c.s --func OvlFunc_947_2008cc0
 *
 * THIS FILE EXISTS ONLY SO THE FUNCTION IS COUNTED AND CHECKABLE. The analysis lives in
 * src/non_matching/overlays/2008098.c, which has always named this address and .s path on
 * its second line: OvlFunc_916_2008098 and this function are BYTE-IDENTICAL TWINS -- both
 * exactly 80 instructions, and a line-by-line diff of the two ROM bodies differs only in
 * four branch label names. READ 2008098.c, not this file, for the blocker and the ladder.
 *
 * Why the separate file: tools/census.py attributes a park to the FIRST identifier in its
 * header, so a twin named on line 2 of another park was being counted UNATTEMPTED. It is
 * not -- 2008098.c carries a batch-278 re-screen, and re-screening this reference in batch
 * 290 reproduced its numbers exactly (its landed C 78 of 86, its recorded best spelling
 * 70 of 86, which is what this file carries).
 *
 * The C below is that recorded best spelling, retargeted to this function's own reference.
 * Its blocker is 2008098.c's: LICM hoists five pool constants where the ROM hoists three.
 */
extern unsigned char gBuffer[];
extern unsigned char ewram_2020000[];
extern unsigned char ewram_2020004[];



void OvlFunc_947_2008cc0(int col, int row, int w, int h, int page, int x0, int y0)
{
    unsigned int *src;
    int x, y, i;
    unsigned int t;

    src = (unsigned int *)gBuffer + (col + (row << 7));
    for (y = y0; y < y0 + h; y++) {
        for (x = x0; x < x0 + w; x++) {
            t = *src++;
            i = ((((y & 0xf) + (page << 4)) << 5) + (x & 0xf)) << 2;
            i += 0x6002800;
            *(int *)i = *(int *)(ewram_2020000 + ((t & 0xfff) << 3));
            *(int *)(i + 0x40) = *(int *)(ewram_2020004 + ((t & 0xfff) << 3));
        }
        src += 0x80 - w;
    }
}
