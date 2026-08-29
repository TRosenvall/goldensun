/* Cluster OvlFunc_934_2008d80..OvlFunc_934_2008d80 extracted from goldensun/asm/overlays/rom_7bdeb0/ovl_d20_c_c_a_a.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched.
 *
 * Selects a script/table pointer from the AREA ID at gState+0x1C0.
 *
 * The compared constants are pooled by the ROM where `cmp r2, #imm8` would do
 * -- the pool tell -- so they are symbols, and these were already defined in
 * area.sym. `extern int _AREA_xx;` compared against `(int)(&_AREA_xx)`
 * reproduces the pool load exactly.
 *
 * The namespace is not guessed from the value. 95 small values are defined in
 * more than one .sym file, so a value-only lookup would emit a byte-correct ROM
 * asserting a false thing. The CONSUMER decides, and a value compared against
 * the halfword at gState+0x1C0 is an area id. See tools/sym_candidates.py.
 *
 * Three-way with a shared default: 0x5d jumps straight to the default, and
 * only 0x5e and 0x5f select tables. The gotos spell the ROM's block order out
 * rather than letting gcc choose it.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_5d;
extern int _AREA_5e;
extern int _AREA_5f;
extern unsigned char L22c4[] __asm__(".L22c4");
extern unsigned char L239c[] __asm__(".L239c");
extern unsigned char L2234[] __asm__(".L2234");

void *OvlFunc_934_2008d80(void)
{
    unsigned char *g;
    unsigned int k;
    unsigned int o;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    o = 0;
    v = *(short *)(g + o);
    if (v == (int)(&_AREA_5d))
        goto last;
    if (v != (int)(&_AREA_5e))
        goto try5f;
    return L22c4;
try5f:
    if (v != (int)(&_AREA_5f))
        goto last;
    return L239c;
last:
    return L2234;
}
