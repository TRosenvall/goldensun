/* Cluster OvlFunc_968_2008558..OvlFunc_968_2008558 extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_a_c.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched -- gcc regenerates the
 * .s at the same path.
 *
 * Walks the object table at [iwram_3001ebc]+0x34 for slots 8..0x41 and pokes
 * every object whose packed field at +0x64 has 0x212 in its signed top bits.
 *
 * Two spellings carry this:
 *   - `i = 8` IS ASSIGNED BEFORE `q`.  The other way round, gcc emits
 *     `add r5, #0x34` before `mov r6, #8` and the ROM has them the other way.
 *     That was the only difference; statement order, not declaration order.
 *   - `k = 0xf; k &= h;` makes the CONSTANT the destination, matching the
 *     ROM's `mov r1, #0xf / and r1, r2`.  See docs/elevation.md.
 *
 * The loop bound is unsigned (`bls`) while the field test is signed (`asr`),
 * which is why `i` is `unsigned int` and `h` is `int`.
 */
extern unsigned char *iwram_3001ebc;
extern void __Func_80929d8(void *a, int n);

void OvlFunc_968_2008558(void)
{
    unsigned char *p;
    int **q;
    int *x;
    unsigned char *r;
    int lim;
    int h;
    int k;
    unsigned int i;

    p = iwram_3001ebc;
    lim = 0x212;
    i = 8;
    q = (int **)(p + 0x34);
    do {
        x = *q++;
        r = (unsigned char *)x + 0x64;
        h = *(unsigned short *)r;
        if (((h << 16) >> 20) == lim) {
            k = 0xf;
            k &= h;
            __Func_80929d8(x, k);
        }
        i++;
    } while (i <= 0x41);
}
