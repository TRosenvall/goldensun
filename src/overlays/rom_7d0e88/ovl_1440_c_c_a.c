/* Cluster OvlFunc_947_20094c4..OvlFunc_947_20094c4 extracted from goldensun/asm/overlays/rom_7d0e88/ovl_1440_c_c.s.
 *
 * GetEntrances, 5-way form: selects one of five per-area tables from the gState
 * halfword at +0x1c0, falling through to the last. Same shape as
 * src/overlays/rom_7ac2d8/ovl_e20_c_c_a.c; see
 * src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is spelled
 * out and why the compared constants have to be symbols.
 *
 * SPLIT BY HAND, because tools/split_s.py refuses this one: the .s holds this
 * function AND twelve .incbin blobs, and a whole-file conversion would delete
 * them. The function moved here as ovl_1440_c_c_a.o and the data stayed in
 * asm/overlays/rom_7d0e88/ovl_1440_c_c_b.s as ovl_1440_c_c_b.o. The two go in
 * different output sections -- .text and .data are listed separately in
 * goldensun/overlays/rom_7d0e88/overlay.ld -- so the layout is unchanged.
 *
 * THREE `.global` LINES WERE ADDED to the data half. `.L3174`, `.L3264` and
 * `.L32dc` are returned by this function but were not exported, while the six
 * labels above them already were. A `.global` emits no bytes; this is the same
 * change batch 09 made to two other files, and it reverts cleanly.
 *
 * Two of the five tables are ordinary globals (`gOvl_0200b06c`,
 * `gOvl_0200b0e4`) and three are `.L` labels. Nothing distinguishes them in the
 * ROM -- the naming is an artefact of which addresses the disassembler happened
 * to have symbols for.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_73;
extern int _AREA_74;
extern int _AREA_77;
extern int _AREA_7a;
extern unsigned char gOvl_0200b06c[];
extern unsigned char gOvl_0200b0e4[];
extern unsigned char L3174[] __asm__(".L3174");
extern unsigned char L32dc[] __asm__(".L32dc");
extern unsigned char L3264[] __asm__(".L3264");

unsigned char *OvlFunc_947_20094c4(void)
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
    if (v == (int)(&_AREA_73))
        return gOvl_0200b06c;
    if (v == (int)(&_AREA_74))
        return gOvl_0200b0e4;
    if (v == (int)(&_AREA_77))
        return L3174;
    if (v == (int)(&_AREA_7a))
        return L32dc;
    return L3264;
}
