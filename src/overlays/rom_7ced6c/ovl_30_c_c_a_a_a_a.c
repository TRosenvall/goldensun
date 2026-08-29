/* Cluster OvlFunc_946_2008d48..OvlFunc_946_2008d48 extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_a_a_a_a.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched.
 *
 * Two equality tests on the AREA ID at gState+0x1C0, then a RANGE test over the
 * area space: 0x7e <= area <= 0x86 selects one table, everything else the
 * default.
 *
 * A SYMBOL-BOUNDED RANGE SIDESTEPS THE SIGNED LOWER-BOUND FLOOR, and that is
 * the point of this file. That floor -- documented since batch 55 -- is that
 * gcc rewrites every lower bound against an IMMEDIATE to `cmp #(K-1) / ble`,
 * where the ROM has `cmp #K / blt`, giving a two-line minimum. It does not
 * apply here: `_AREA_7e` is a symbol, so the comparison is register-to-register
 * (`cmp r2, r3`) and there is no immediate to decrement. gcc emits `blt`
 * exactly as the ROM does.
 *
 * The candidate filter had been excluding every function containing a signed
 * range branch, which was too coarse -- it should exclude only ranges against
 * IMMEDIATES. Corrected in tools/sym_candidates.py.
 *
 * The two out-of-range paths share ONE exit via `goto out;` rather than two
 * `return` statements. Written as two returns gcc merges them into a block it
 * places differently, costing six instructions; the shared label puts it where
 * the ROM has it.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_71;
extern int _AREA_7b;
extern int _AREA_7e;
extern int _AREA_86;
extern unsigned char gScript_911__0200b610[];
extern unsigned char gOvl_0200b5f8[];
extern unsigned char L3718[] __asm__(".L3718");
extern unsigned char L3850[] __asm__(".L3850");

void *OvlFunc_946_2008d48(void)
{
    unsigned char *g;
    unsigned int k;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_71))
        return gScript_911__0200b610;
    if (v == (int)(&_AREA_7b))
        return L3718;
    if (v > (int)(&_AREA_86))
        goto out;
    if (v < (int)(&_AREA_7e))
        goto out;
    return L3850;
out:
    return gOvl_0200b5f8;
}
