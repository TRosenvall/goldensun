/* OvlFunc_964_20092e0 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * A SCRIPT SELECTOR: pick a script pointer by area id and return it. Same
 * gState+0x1c0 read as the area-dispatch family (batch 45/47) -- `off = 0` is a
 * variable because Thumb `ldrsh` has no immediate-offset form.
 *
 * The compared constant is POOLED where `cmp #imm` would do, the symbol tell,
 * so it is an area id.
 *
 * `pop {r1}` is the return-value tell: the function returns a pointer, so r0
 * is live across the epilogue.
 *
 * Unlike its sibling ovl_7c37ac/ovl_30_c_c_a_a.c, BOTH arms fall through to
 * the call: the `if` chooses the pointer and the call happens once afterwards.
 * The ROM says so by branching to a join before the `bl` rather than after it.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;

extern int _AREA_ac;
extern unsigned char L3a74[] __asm__(".L3a74");
extern unsigned char gScript_925__0200b8f4[];
extern void __Func_808b868(unsigned char *p);

unsigned char *OvlFunc_964_20092e0(void)
{
    unsigned char *p;
    unsigned int base;
    unsigned int off;
    short v;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    v = *(short *)((char *)base + off);
    if (v == (int)(&_AREA_ac))
        p = gScript_925__0200b8f4;
    else
        p = L3a74;
    __Func_808b868(p);
    return p;
}
