/* Cluster OvlFunc_959_2008a34..OvlFunc_959_2008a34 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_a_a_a.s.
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
 * 0xa2 and 0xa3 share one arm, which is why the second test falls INTO the
 * third rather than past it.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_a1;
extern int _AREA_a2;
extern int _AREA_a3;
extern unsigned char L6910[] __asm__(".L6910");
extern unsigned char L697c[] __asm__(".L697c");
extern unsigned char L68a4[] __asm__(".L68a4");

void *OvlFunc_959_2008a34(void)
{
    unsigned char *g;
    unsigned int k;
    unsigned int o;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    o = 0;
    v = *(short *)(g + o);
    if (v != (int)(&_AREA_a1))
        goto next;
    return L6910;
next:
    if (v == (int)(&_AREA_a2))
        goto both;
    if (v != (int)(&_AREA_a3))
        goto other;
both:
    return L697c;
other:
    return L68a4;
}
