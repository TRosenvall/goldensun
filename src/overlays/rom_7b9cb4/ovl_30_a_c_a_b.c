/* Cluster OvlFunc_932_20080e4..OvlFunc_932_20080e4 extracted from goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_a_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly and keep their
 * slots in the overlay's linker script.
 *
 * GetEntrances, 12-way form: selects one of 12 per-area tables from
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
extern int _AREA_4e;
extern int _AREA_4f;
extern int _AREA_50;
extern int _AREA_51;
extern int _AREA_52;
extern int _AREA_53;
extern int _AREA_54;
extern int _AREA_55;
extern int _AREA_56;
extern int _AREA_57;
extern unsigned char gOvl_0200c194[];
extern unsigned char L420c[] __asm__(".L420c");
extern unsigned char L426c[] __asm__(".L426c");
extern unsigned char L4314[] __asm__(".L4314");
extern unsigned char L43ec[] __asm__(".L43ec");
extern unsigned char ActorCmd_ARRAY_943__0200c464[];
extern unsigned char L4524[] __asm__(".L4524");
extern unsigned char L459c[] __asm__(".L459c");
extern unsigned char L4644[] __asm__(".L4644");
extern unsigned char L4704[] __asm__(".L4704");
extern unsigned char L477c[] __asm__(".L477c");
extern unsigned char gScript_936__0200c164[];

unsigned char *OvlFunc_932_20080e4(void)
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
        return gOvl_0200c194;
    if (v == (int)(&_AREA_4e))
        return L420c;
    if (v == (int)(&_AREA_4f))
        return L426c;
    if (v == (int)(&_AREA_50))
        return L4314;
    if (v == (int)(&_AREA_51))
        return L43ec;
    if (v == (int)(&_AREA_52))
        return ActorCmd_ARRAY_943__0200c464;
    if (v == (int)(&_AREA_53))
        return L4524;
    if (v == (int)(&_AREA_54))
        return L459c;
    if (v == (int)(&_AREA_55))
        return L4644;
    if (v == (int)(&_AREA_56))
        return L4704;
    if (v == (int)(&_AREA_57))
        return L477c;
    return gScript_936__0200c164;
}
