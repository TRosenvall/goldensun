/* ColorCycleVFXPalette -- NON-MATCHING at -O2: 86 encodings of 86 differ, FOUR
 * INSTRUCTIONS LONG (ours 90, 192 bytes against 184).  EXACT WITH -fno-gcse.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/ColorCycleVFXPalette.c \
 *     asm/rom_c9000/rom_d2d98.s --func ColorCycleVFXPalette
 * and with -fno-gcse added to objcmp's flags it reports
 *   OK ColorCycleVFXPalette -- 184 bytes, 86 encodings and 5 relocations identical
 *
 * The body below is byte-exact under GCSE_CFLAGS. At -O2, gcse merges the three
 * sp frame addresses (the buf[0] store, the loop start, the call argument) into
 * one register, which costs an extra callee-saved register and shifts
 * everything. Tried without the flag, all failing: a volatile buf[0] store, a
 * pointer-local argument, moving the buf[0] store.
 *
 * NOT LANDED because it would need a per-file GCSE_CFLAGS rule, and three
 * failed spellings do not meet docs/elevation.md's bar ("What justifies a
 * per-file FLAG RULE": the spellings must PROVABLY not differ). Also
 * asm/rom_c9000/rom_d2d98.s holds six functions plus .rodata, so landing it
 * needs a split as well. Decision for the owner: flag row, or keep hunting.
 */
extern int sin(int a);
extern int Func_8001af8();

void ColorCycleVFXPalette(int t, int a, int b, int c)
{
    unsigned short buf[64];
    int r, g, bl;
    int i;
    int (*fp)();

    t <<= 10;
    r = (sin(t + a) << 4) >> 15;
    g = (sin(t + b) << 4) >> 15;
    bl = (sin(t + c) << 4) >> 15;
    buf[0] = 0;
    for (i = 1; i != 64; i++) {
        int x = (i + r) / 2;
        int y = (i + g) / 2;
        int z = (i + bl) / 2;
        if (x < 0) x = 0;
        if (x > 31) x = 31;
        if (y < 0) y = 0;
        if (y > 31) y = 31;
        if (z < 0) z = 0;
        if (z > 31) z = 31;
        buf[i] = (z << 10) | (y << 5) | x;
    }
    fp = Func_8001af8;
    fp(0x5000002, buf, 0x80);
}
