/* Cluster OvlFunc_951_20081a8..OvlFunc_951_20081a8 extracted from goldensun/asm/overlays/rom_7d6418/ovl_30_c_c_c_a_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly.
 *
 * GetEntrances, 2-way form: selects one of 2 per-area tables from
 * the gState halfword at +0x1C0, falling through to the last.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_bd;
extern unsigned char Events_TolbiSpring[];
extern unsigned char Events_GameBuildings[];

unsigned char *OvlFunc_951_20081a8(void)
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
    if (v == (int)(&_AREA_bd))
        return Events_TolbiSpring;
    return Events_GameBuildings;
}
