/* OvlFunc_965_2008fac extracted from goldensun/asm/overlays/rom_7ef4f4/ovl_30_a_a_c_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * UNPARKED. The park held `if (area == X) return script; return 0;`, which is
 * 14 lines against the ROM`s 15 -- gcc computing the zero only on the path that
 * needs it. The ROM sets the default FIRST, before the compare, and overwrites
 * it in the matching arm:
 *
 *     ldr r3, =<area id> / mov r0, #0 / cmp r2, r3 / bne <out> / ldr r0, =<script>
 *
 * so the source assigns `p = 0;` unconditionally and then conditionally
 * replaces it. Found by sweeping the parked set for cases where OUR stream is
 * SHORTER than the ROM`s -- that difference is the signature of gcc having
 * found something cheaper than the original compiler did.
 *
 * Twin of src/overlays/rom_7ed0a0/ovl_30_a_a_c_c_c.c in another overlay,
 * differing in the area id and the script.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_b0;
extern unsigned char L35b8[] __asm__(".L35b8");

unsigned char *OvlFunc_965_2008fac(void)
{
    unsigned int base;
    unsigned int off;
    short v;
    unsigned char *p;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    v = *(short *)((char *)base + off);
    p = 0;
    if (v == (int)(&_AREA_b0))
        p = L35b8;
    return p;
}
