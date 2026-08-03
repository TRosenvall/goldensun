/* Cluster OvlFunc_957_200b598..OvlFunc_957_200b598 extracted from goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_c_b.s.
 *
 * Split out of that .s. tools/split_s.py refused the cut until the seven
 * tables this function returns were declared .global -- they are data the .c
 * cannot carry, so they stay in assembly and are referenced across the object
 * boundary. A .global emits no bytes; make compare was verified green after
 * the export and before the split, so the two changes stay separable.
 *
 * GetEntrances, 6-way form: selects one of 6 per-area tables from
 * the gState halfword at +0x1C0, falling through to the last.
 *
 * See src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_93;
extern int _AREA_94;
extern int _AREA_95;
extern int _AREA_96;
extern int _AREA_97;
extern unsigned char L4688[] __asm__(".L4688");
extern unsigned char L4724[] __asm__(".L4724");
extern unsigned char L476c[] __asm__(".L476c");
extern unsigned char L4808[] __asm__(".L4808");
extern unsigned char L4850[] __asm__(".L4850");
extern unsigned char L45e0[] __asm__(".L45e0");

unsigned char *OvlFunc_957_200b598(void)
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
    if (v == (int)(&_AREA_93))
        return L4688;
    if (v == (int)(&_AREA_94))
        return L4724;
    if (v == (int)(&_AREA_95))
        return L476c;
    if (v == (int)(&_AREA_96))
        return L4808;
    if (v == (int)(&_AREA_97))
        return L4850;
    return L45e0;
}
