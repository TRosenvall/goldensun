/* Cluster OvlFunc_932_20081c8..OvlFunc_932_20081c8 extracted from goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_a_c.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched.
 *
 * Selects a script/table pointer from the AREA ID at gState+0x1C0.
 *
 * The compared constants are pooled by the ROM where `cmp r2, #imm8` would do
 * -- the pool tell -- so they are symbols, and these were already defined in
 * area.sym. `extern int _AREA_xx;` compared against `(int)(&_AREA_xx)`
 * reproduces the pool load exactly.
 *
 * The namespace is not guessed from the value. 95 small values are defined in
 * more than one .sym file, so a value-only lookup would emit a byte-correct ROM
 * asserting a false thing. The CONSUMER decides, and a value compared against
 * the halfword at gState+0x1C0 is an area id. See tools/sym_candidates.py.
 *
 * The `return 0;` arm is real: the ROM sets `mov r0, #0` before the second
 * compare so the not-matched path falls through with a null pointer.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_55;
extern int _AREA_56;
extern unsigned char gScript_943__0200c80c[];
extern unsigned char gOvl_0200c83c[];

void *OvlFunc_932_20081c8(void)
{
    unsigned char *g;
    unsigned int k;
    unsigned int o;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    o = 0;
    v = *(short *)(g + o);
    if (v == (int)(&_AREA_55))
        return gScript_943__0200c80c;
    if (v != (int)(&_AREA_56))
        return 0;
    return gOvl_0200c83c;
}
