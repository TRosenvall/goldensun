/* Cluster OvlFunc_933_200841c..OvlFunc_933_200841c extracted from goldensun/asm/overlays/rom_7bc690/ovl_314_c_c_a_a.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched.
 *
 * Selects a script/table pointer from the AREA ID at gState+0x1C0.
 *
 * The compared constants are pooled by the ROM where `cmp r2, #imm8` would do
 * -- the pool tell -- so they are symbols, and they were already defined in
 * area.sym. The namespace comes from the CONSUMER, not the value: 95 small
 * values are defined in more than one .sym file, and a value compared against
 * gState+0x1C0 is an area id. See tools/sym_candidates.py.
 *
 * THE AREA FIELD IS READ TWICE, and that is deliberate. The ROM re-loads it
 * after the __SetFlag block rather than reusing the value it already had.
 * Writing it once and reusing the local drops the second `ldrsh` and the whole
 * tail shifts.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_59;
extern int _AREA_5a;
extern int _AREA_5b;
extern unsigned char L23c8[] __asm__(".L23c8");
extern unsigned char L2410[] __asm__(".L2410");
extern unsigned char L24b8[] __asm__(".L24b8");
extern unsigned char L23b0[] __asm__(".L23b0");
extern void __SetFlag(int id);

void *OvlFunc_933_200841c(void)
{
    unsigned char *b;
    unsigned char *g;
    unsigned int k;
    unsigned int o;
    int v;
    int w;

    b = (unsigned char *)&gState;
    k = 0xe0 << 1;
    g = b + k;
    o = 0;
    v = *(short *)(g + o);
    if (v == (int)(&_AREA_5b)) {
        k = 0xe1 << 1;
        g = b + k;
        o = 0;
        w = *(short *)(g + o);
        if (w == 5)
            __SetFlag(0x90a);
    }
    k = 0xe0 << 1;
    g = b + k;
    o = 0;
    v = *(short *)(g + o);
    if (v == (int)(&_AREA_59))
        return L23c8;
    if (v == (int)(&_AREA_5a))
        return L2410;
    if (v == (int)(&_AREA_5b))
        return L24b8;
    return L23b0;
}
