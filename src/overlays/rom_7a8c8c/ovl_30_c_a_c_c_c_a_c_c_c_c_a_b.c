/* Cluster OvlFunc_922_2008f30..OvlFunc_922_2008f30 extracted from goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_a_c_c_c_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_a_c_c_c_c_a_a.o and the rest of the
 * overlay in goldensun/overlays/rom_7a8c8c/overlay.ld.
 *
 * GetEntrances, 8-way form -- the widest yet elevated. Selects one of eight
 * per-area tables from the gState halfword at +0x1c0, falling through to the
 * last. Same shape as src/overlays/rom_7ac2d8/ovl_e20_c_c_a.c; see
 * src/overlays/rom_79aad8/ovl_314_a.c for why the address arithmetic is
 * spelled out and why the compared constants have to be symbols.
 *
 * All seven table labels were ALREADY exported by the sibling .s that defines
 * them, so unlike the last two members of this family no `.global` had to be
 * added. One of the eight is an ordinary global (`gScript_911__0200ac08`) and
 * the rest are `.L` labels; nothing distinguishes them in the ROM.
 *
 * THIS FUNCTION FOUND A BUG IN THE SCREEN. It reported one differing position
 * in the middle of an otherwise clean 53-instruction diff:
 *
 *     rom   ldr r0,=.L3058
 *     ours  ldr r0, =.L3058
 *
 * The inherited disassembly has no space after the comma on that one line, and
 * tools/tryc.py collapsed runs of whitespace but could not insert one that was
 * not there. Three lines in the whole tree have this, in three different files,
 * and each one would have cost a round -- a single differing operand in a long
 * clean diff reads exactly like a wrong symbol. The screen now normalises
 * comma spacing on both sides.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_34;
extern int _AREA_3e;
extern int _AREA_3f;
extern int _AREA_40;
extern int _AREA_41;
extern int _AREA_42;
extern int _AREA_43;
extern unsigned char gScript_911__0200ac08[];
extern unsigned char L2bd8[] __asm__(".L2bd8");
extern unsigned char L2d1c[] __asm__(".L2d1c");
extern unsigned char L2e24[] __asm__(".L2e24");
extern unsigned char L3058[] __asm__(".L3058");
extern unsigned char L3130[] __asm__(".L3130");
extern unsigned char L3184[] __asm__(".L3184");
extern unsigned char L2bcc[] __asm__(".L2bcc");

unsigned char *OvlFunc_922_2008f30(void)
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
    if (v == (int)(&_AREA_34))
        return L2bd8;
    if (v == (int)(&_AREA_3e))
        return gScript_911__0200ac08;
    if (v == (int)(&_AREA_3f))
        return L2d1c;
    if (v == (int)(&_AREA_40))
        return L2e24;
    if (v == (int)(&_AREA_41))
        return L3058;
    if (v == (int)(&_AREA_42))
        return L3130;
    if (v == (int)(&_AREA_43))
        return L3184;
    return L2bcc;
}
