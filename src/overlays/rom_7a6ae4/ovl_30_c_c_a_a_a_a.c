/* OvlFunc_920_200846c extracted from goldensun/asm/overlays/rom_7a6ae4/ovl_30_c_c_a_a_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in the overlay's linker script is unchanged.
 *
 * Map-load entry for slot 0: dispatches to one of three per-area setup
 * functions and does nothing for any other area. Same address arithmetic as
 * the 24-member GetEntrances family -- see
 * src/overlays/rom_7b4558/ovl_30_a_c_c_c.c for why it is spelled out and why
 * `off = 0` is a variable rather than a literal (Thumb `ldrsh` has no
 * immediate-offset form, so the zero has to live in a register).
 *
 * The three compared constants are POOLED (`ldr r3, =0x31` where `cmp r2, #0x31`
 * would do), which is the symbol tell, so they are area ids rather than
 * literals. area.sym already defines all three.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_2f;
extern int _AREA_30;
extern int _AREA_31;
extern void OvlFunc_920_20084b4(void);
extern void OvlFunc_920_20084e8(void);
extern void OvlFunc_920_2008538(void);

int OvlFunc_920_200846c(void)
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
    if (v == (int)(&_AREA_31))
        OvlFunc_920_20084b4();
    else if (v == (int)(&_AREA_30))
        OvlFunc_920_20084e8();
    else if (v == (int)(&_AREA_2f))
        OvlFunc_920_2008538();
    return 0;
}
