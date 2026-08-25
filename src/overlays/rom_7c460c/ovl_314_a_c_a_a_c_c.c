/* OvlFunc_939_20086e4 extracted from goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_a_a_c_c.s.
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
 * TWO AREAS, TREATED DIFFERENTLY, and the asymmetry is what the ROM says:
 * area 0x9f calls __GetFlag AND DISCARDS THE RESULT before returning its
 * script unconditionally, while area 0x68 branches on the same flag and falls
 * through to the shared fallback when it is clear. Writing the first arm as a
 * conditional too would be the tidier reading and is not what is there.
 *
 * _AREA_68 and _AREA_9f were added to area.sym by value, as that file`s own
 * comment prescribes for ids without a semantic name. Absolute symbol
 * definitions emit no bytes.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;

extern int _AREA_9f;
extern int _AREA_68;
extern unsigned char L23b4[] __asm__(".L23b4");
extern unsigned char L21bc[] __asm__(".L21bc");
extern unsigned char L1fc4[] __asm__(".L1fc4");
extern int __GetFlag(int id);

unsigned char *OvlFunc_939_20086e4(void)
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
    if (v == (int)(&_AREA_9f)) {
        __GetFlag(0x941);
        return L23b4;
    }
    if (v == (int)(&_AREA_68)) {
        if (__GetFlag(0x941))
            return L21bc;
    }
    return L1fc4;
}
