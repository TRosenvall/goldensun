/* OvlFunc_911_20081dc extracted from goldensun/asm/overlays/rom_79e5c0/ovl_30_c_a_a_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * A script selector with a SIDE EFFECT in one arm: area 0x24 conditionally runs
 * OvlFunc_911_20080a0 on the script it is about to return, then returns it. The
 * script symbol is loaded TWICE in that arm -- once for the call, once for the
 * return -- and the ROM does not share the register, so the source names it
 * twice too.
 *
 * _AREA_24 was added to area.sym by value. It was the LAST missing id needed by
 * any pure dispatcher: enumerating the family found exactly one member left and
 * this is it, so that vein is now closed.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_24;
extern int _AREA_27;
extern int __GetFlag(int id);
extern void OvlFunc_911_20080a0(unsigned char *p);
extern unsigned char L3098[] __asm__(".L3098");
extern unsigned char L3368[] __asm__(".L3368");
extern unsigned char L3080[] __asm__(".L3080");

unsigned char *OvlFunc_911_20081dc(void)
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
    if (v == (int)(&_AREA_24)) {
        if (!__GetFlag(0x845))
            OvlFunc_911_20080a0(L3098);
        return L3098;
    }
    if (v == (int)(&_AREA_27))
        return L3368;
    return L3080;
}
