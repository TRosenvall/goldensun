/* Cluster OvlFunc_942_20080a0..OvlFunc_942_20080a0 extracted from goldensun/asm/overlays/rom_7c6bac/ovl_30_c_c_a_a.s.
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
 * One of a SHAPE PAIR with OvlFunc_942_200819c in the same .s: identical opcode
 * sequence, different constants and tables. The second was elevated by taking
 * this source and substituting operands, and matched on the first screen.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_6b;
extern int _AREA_6c;
extern int _AREA_70;
extern unsigned char L1acc[] __asm__(".L1acc");
extern unsigned char L19c4[] __asm__(".L19c4");
extern unsigned char L1dcc[] __asm__(".L1dcc");
extern unsigned char L1d24[] __asm__(".L1d24");
extern unsigned char L1c7c[] __asm__(".L1c7c");
extern unsigned char L18d4[] __asm__(".L18d4");
extern unsigned char gOvl_02009ba4[];
extern unsigned char gOvl_020098ec[];
extern int __GetFlag(int id);

void *OvlFunc_942_20080a0(void)
{
    unsigned char *g;
    unsigned int k;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_6b)) {
        if (__GetFlag(0x93e) != 0)
            return gOvl_02009ba4;
        return L1acc;
    }
    if (v == (int)(&_AREA_70)) {
        if (__GetFlag(0x95 << 4) != 0)
            return L19c4;
        return gOvl_020098ec;
    }
    if (v == (int)(&_AREA_6c)) {
        if (__GetFlag(0x95 << 4) != 0)
            return L1dcc;
        if (__GetFlag(0x93e) != 0)
            return L1d24;
        return L1c7c;
    }
    return L18d4;
}
