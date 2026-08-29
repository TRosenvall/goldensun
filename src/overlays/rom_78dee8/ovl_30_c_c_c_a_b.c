/* Cluster OvlFunc_895_20088f4..OvlFunc_895_20088f4 extracted from goldensun/asm/overlays/rom_78dee8/ovl_30_c_c_c_a.s.
 *
 * Slotted between ovl_30_c_c_c_a_a.o and the rest of the overlay.
 *
 * Area dispatch, two-arm form: read the area halfword at gState+0x1c0 and call
 * one of two per-area setup functions, doing nothing for any other area. One
 * of the family whose three-arm member OvlFunc_920_200846c is in batch 45; the
 * address arithmetic is the GetEntrances spelling, `off = 0` included, because
 * Thumb `ldrsh` has no immediate-offset form and the zero needs a register.
 *
 * The compared constants are POOLED where a `cmp #imm` would do, which is the
 * symbol tell, so they are area ids. area.sym already defined both.
 *
 * `pop {r1}` rather than `pop {r0}` is the return-value tell: the function
 * returns 0, so r0 is live across the epilogue.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_13;
extern int _AREA_10;
extern void OvlFunc_895_200892c(void);
extern void OvlFunc_895_2008a24(void);

int OvlFunc_895_20088f4(void)
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
    if (v == (int)(&_AREA_13))
        OvlFunc_895_200892c();
    else if (v == (int)(&_AREA_10))
        OvlFunc_895_2008a24();
    return 0;
}
