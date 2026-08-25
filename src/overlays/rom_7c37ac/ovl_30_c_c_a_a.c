/* OvlFunc_938_200806c extracted from goldensun/asm/overlays/rom_7c37ac/ovl_30_c_c_a_a.s.
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
 * ONLY THE MATCHING ARM CALLS __Func_808b868; the fallback returns its script
 * untouched. The ROM keeps the selected pointer in r5 across the call and
 * returns it, which is what naming `p` reproduces.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;

extern int _AREA_67;
extern unsigned char L1df4[] __asm__(".L1df4");
extern unsigned char gScript_918__02009ddc[];
extern void __Func_808b868(unsigned char *p);

unsigned char *OvlFunc_938_200806c(void)
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
    if (v == (int)(&_AREA_67)) {
        p = L1df4;
        __Func_808b868(p);
        return p;
    }
    return gScript_918__02009ddc;
}
