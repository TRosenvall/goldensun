/* Cluster OvlFunc_967_20084b0..OvlFunc_967_20084b0 extracted from goldensun/asm/overlays/rom_7f21b8/ovl_30_c_c_c_c.s.
 *
 * Slotted between ovl_30_c_c_c_c_a.o and the rest of the overlay.
 *
 * A script selector on the area AND a flag: four scripts from a two-level
 * decision. `pop {r1}` is the return-value tell.
 *
 * THIS FUNCTION LOOKS LIKE constant-CSE AND IS NOT, which is worth recording
 * because the mechanical search for that class flagged it. The flag id 0x9a7 is
 * loaded for two __GetFlag calls, which is the shape -- but the two calls are on
 * MUTUALLY EXCLUSIVE arms of the area test, so gcc never has both live and
 * never hoists. It matches at plain -O2 and no Makefile rule was added.
 *
 * That is exactly the caveat tools/pick_candidates.py records for its
 * repeated-pooled-constant filter, now confirmed on a live function rather than
 * argued from the mechanism.
 *
 * THREE `.global` LINES WERE ADDED to the .s -- .L2010, .L1eb4 and .L1a94, the
 * three script tables this returns, all defined below the cut. Eleventh through
 * thirteenth in this tree. A `.global` emits no bytes and `make compare` was
 * verified green after the export and BEFORE the split.
 *
 * _AREA_b4 was added to area.sym by value, as that file's comment prescribes.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_b4;
extern int __GetFlag(int id);
extern unsigned char L2010[] __asm__(".L2010");
extern unsigned char L1eb4[] __asm__(".L1eb4");
extern unsigned char L1a94[] __asm__(".L1a94");
extern unsigned char gScript_887__02009ca4[];

unsigned char *OvlFunc_967_20084b0(void)
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
    if (v == (int)(&_AREA_b4)) {
        if (__GetFlag(0x9a7))
            return L2010;
        return L1eb4;
    }
    if (__GetFlag(0x9a7))
        return gScript_887__02009ca4;
    return L1a94;
}
