/* Cluster OvlFunc_942_200819c..OvlFunc_942_200819c extracted from goldensun/asm/overlays/rom_7c6bac/ovl_30_c_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted with the _b piece (which keeps
 * OvlFunc_942_2008144 as assembly) in goldensun/overlays/rom_7c6bac/overlay.ld.
 *
 * Selects a script/table pointer from the AREA ID at gState+0x1C0, with a flag
 * test inside each arm.
 *
 * The pooled area constants were already defined in area.sym. The namespace
 * comes from the CONSUMER -- a value compared against gState+0x1C0 is an area
 * id -- not from the value. See tools/sym_candidates.py.
 *
 * The shape twin of OvlFunc_942_20080a0, ported from it by substituting
 * operands. READING BOTH FIRST WAS NECESSARY: a grep for the pool operands
 * missed `GFX_Thermometer` because the pattern only matched lowercase, which
 * made the two look structurally different when they are not.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_6b;
extern int _AREA_6c;
extern int _AREA_70;
extern unsigned char L1e80[] __asm__(".L1e80");
extern unsigned char L2120[] __asm__(".L2120");
extern unsigned char L2018[] __asm__(".L2018");
extern unsigned char L2390[] __asm__(".L2390");
extern unsigned char L230c[] __asm__(".L230c");
extern unsigned char L224c[] __asm__(".L224c");
extern unsigned char L1e74[] __asm__(".L1e74");
extern unsigned char GFX_Thermometer[];
extern int __GetFlag(int id);

void *OvlFunc_942_200819c(void)
{
    unsigned char *g;
    unsigned int k;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_6b)) {
        if (__GetFlag(0x93e) != 0)
            return GFX_Thermometer;
        return L1e80;
    }
    if (v == (int)(&_AREA_70)) {
        if (__GetFlag(0x95 << 4) != 0)
            return L2120;
        return L2018;
    }
    if (v == (int)(&_AREA_6c)) {
        if (__GetFlag(0x95 << 4) != 0)
            return L2390;
        if (__GetFlag(0x93e) != 0)
            return L230c;
        return L224c;
    }
    return L1e74;
}
