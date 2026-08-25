/* Cluster OvlFunc_939_2008314..OvlFunc_939_2008314 extracted from goldensun/asm/overlays/rom_7c460c/ovl_314_a_a.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched.
 *
 * Selects a script/table pointer from the AREA ID at gState+0x1C0.
 *
 * ELEVATED BY USING SYMBOLS THAT ALREADY EXISTED. The ROM pools 0x68 and 0x9f
 * where `cmp r2, #0x68` would do -- the pool tell -- so the operands are
 * symbols. `_AREA_68` and `_AREA_9f` were already defined in area.sym.
 * `extern int _AREA_68;` compared against `(int)(&_AREA_68)` reproduces the
 * pool load; gcc always pools a symbol's address and never pools a constant it
 * can build with an eight-bit `mov`.
 *
 * WHICH NAMESPACE, and why it is not guesswork: 0x68 is defined in BOTH
 * area.sym and file_table.sym, and 95 small values collide that way. The bytes
 * are identical either way, so a value-only lookup would emit a correct ROM and
 * assert a false thing. The consumer decides -- `_FILE_` goes to GetFile,
 * `_MSG_` to the message calls, and a value COMPARED AGAINST gState+0x1C0 is an
 * area id. This function compares, so it is `_AREA_`.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_68;
extern int _AREA_9f;
extern unsigned char gOvl_02009d3c[];
extern unsigned char L1bec[] __asm__(".L1bec");

void *OvlFunc_939_2008314(void)
{
    unsigned char *g;
    unsigned int k;
    unsigned int o;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    o = 0;
    v = *(short *)(g + o);
    if (v == (int)(&_AREA_68))
        goto other;
    if (v != (int)(&_AREA_9f))
        goto other;
    return gOvl_02009d3c;
other:
    return L1bec;
}
