/* Cluster OvlFunc_932_200820c..OvlFunc_932_200820c extracted from goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_a_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script.
 *
 * GetEntrances, 10-way form: selects one of 10 per-area tables from
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
extern int _AREA_4d;
extern int _AREA_4f;
extern int _AREA_51;
extern int _AREA_52;
extern int _AREA_53;
extern int _AREA_54;
extern int _AREA_55;
extern int _AREA_56;
extern int _AREA_57;
extern unsigned char L4940[] __asm__(".L4940");
extern unsigned char L49a0[] __asm__(".L49a0");
extern unsigned char gScript_882__0200ca00[];
extern unsigned char L4a60[] __asm__(".L4a60");
extern unsigned char L4aa8[] __asm__(".L4aa8");
extern unsigned char L4b68[] __asm__(".L4b68");
extern unsigned char L4b98[] __asm__(".L4b98");
extern unsigned char L4c40[] __asm__(".L4c40");
extern unsigned char L4cd0[] __asm__(".L4cd0");
extern unsigned char L4928[] __asm__(".L4928");

unsigned char *OvlFunc_932_200820c(void)
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
    if (v == (int)(&_AREA_4d))
        return L4940;
    if (v == (int)(&_AREA_4f))
        return L49a0;
    if (v == (int)(&_AREA_51))
        return gScript_882__0200ca00;
    if (v == (int)(&_AREA_52))
        return L4a60;
    if (v == (int)(&_AREA_53))
        return L4aa8;
    if (v == (int)(&_AREA_54))
        return L4b68;
    if (v == (int)(&_AREA_55))
        return L4b98;
    if (v == (int)(&_AREA_56))
        return L4c40;
    if (v == (int)(&_AREA_57))
        return L4cd0;
    return L4928;
}
