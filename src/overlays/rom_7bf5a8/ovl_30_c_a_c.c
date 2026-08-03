/* Cluster OvlFunc_935_20080e0..OvlFunc_935_20080e0 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_30_c_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * GetEntrances, 4-way form. Returns a named global from at least one arm,
 * which is why the family sweeps in batches 08-13 missed it -- they matched
 * only on `.L` returns.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _ID_60;
extern int _ID_61;
extern int _ID_62;
extern unsigned char L1d34[] __asm__(".L1d34");
extern unsigned char L1d4c[] __asm__(".L1d4c");
extern unsigned char gScript_887__02009ecc[];
extern unsigned char L1d1c[] __asm__(".L1d1c");

unsigned char *OvlFunc_935_20080e0(void)
{
    unsigned int base;
    unsigned int off;
    short v;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    v = *(short *)((char *)base + off);
    if (v == (int)(&_ID_60))
        return L1d34;
    if (v == (int)(&_ID_61))
        return L1d4c;
    if (v == (int)(&_ID_62))
        return gScript_887__02009ecc;
    return L1d1c;
}
