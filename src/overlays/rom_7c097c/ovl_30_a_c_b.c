/* Cluster OvlFunc_936_20080ec..OvlFunc_936_20080ec extracted from goldensun/asm/overlays/rom_7c097c/ovl_30_a_c_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script.
 *
 * GetEntrances, 7-way form: selects one of 7 per-area tables from
 * the gState halfword at +0x1C0, falling through to the last.
 *
 * The family sweeps in batches 08-15 capped at 4-way and reported the
 * families complete. Removing that cap found 25 more, at arities up to
 * twelve -- this one included.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_63;
extern int _AREA_66;
extern int _AREA_99;
extern int _AREA_9a;
extern int _AREA_9b;
extern int _AREA_9c;
extern unsigned char L42c8[] __asm__(".L42c8");
extern unsigned char L4448[] __asm__(".L4448");
extern unsigned char L44a8[] __asm__(".L44a8");
extern unsigned char L4520[] __asm__(".L4520");
extern unsigned char L4580[] __asm__(".L4580");
extern unsigned char gScript_943__0200c628[];
extern unsigned char L4298[] __asm__(".L4298");

unsigned char *OvlFunc_936_20080ec(void)
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
    if (v == (int)(&_AREA_63))
        return L42c8;
    if (v == (int)(&_AREA_66))
        return L4448;
    if (v == (int)(&_AREA_99))
        return L44a8;
    if (v == (int)(&_AREA_9a))
        return L4520;
    if (v == (int)(&_AREA_9b))
        return L4580;
    if (v == (int)(&_AREA_9c))
        return gScript_943__0200c628;
    return L4298;
}
