/* Cluster OvlFunc_960_200834c..OvlFunc_960_200834c extracted from goldensun/asm/overlays/rom_7eaf28/ovl_314_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * GetEntrances, 4-way form, returning a named global from one arm. That
 * is why the family sweeps in batches 08-13 missed it -- they matched only on
 * `.L` returns, so fourteen members of the three families were invisible.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for the shape.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _ID_a4;
extern int _ID_a5;
extern int _ID_a6;
extern unsigned char gOvl_02009488[];
extern unsigned char L14d0[] __asm__(".L14d0");
extern unsigned char L1548[] __asm__(".L1548");
extern unsigned char L1458[] __asm__(".L1458");

unsigned char *OvlFunc_960_200834c(void)
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
    if (v == (int)(&_ID_a4))
        return gOvl_02009488;
    if (v == (int)(&_ID_a5))
        return L14d0;
    if (v == (int)(&_ID_a6))
        return L1548;
    return L1458;
}
