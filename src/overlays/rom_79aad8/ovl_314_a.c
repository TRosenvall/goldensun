/* Cluster OvlFunc_906_2008314..OvlFunc_906_2008314 extracted from goldensun/asm/overlays/rom_79aad8/ovl_314_a.s.
 *
 * The .s held ONLY this function, so no split was needed -- the .o keeps its
 * name and its slot in goldensun/overlays/rom_79aad8/overlay.ld is unchanged.
 *
 * GetEntrances. Picks one of two edge-transition tables from a gState
 * halfword. Head of a 22-member family, the largest in the overlays.
 *
 * This was PARKED, on two residual diffs. Both are now solved, and each needed
 * a different technique already present in the tree:
 *
 *  1. The ROM loads &gState with a zero addend and builds 0x1c0 AT RUNTIME
 *     (`mov #0xe0 / lsl #1 / add`), where every plain char-pointer or integer
 *     spelling folds 0x1c0 into the pool addend. Writing the arithmetic out
 *     as separate statements over a typed GlobalState reproduces it -- the
 *     same shape that already matched in
 *     src/overlays/rom_7b7790/ovl_314_c_c_a_b.c.
 *
 *  2. The compare pools 0x1d (`ldr r3, =0x1d / cmp r2, r3`) where `cmp #0x1d`
 *     would fit, which is the pool-tell: that operand was a symbol. `_AREA_1d`
 *     in area.sym. See docs/elevation.md.
 *
 * The two return values are `.global` data labels defined in ovl_314_c_c.s.
 * C cannot spell ".L8d8", so a legal name is bound with a gcc asm() label; the
 * emitted reloc is R_ARM_ABS32 against .L8d8, identical to the ROM. That
 * technique was worked out while this function was parked and is what makes
 * the whole `ldr =.LXXXX`-to-overlay-data class reachable.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_1d;
extern unsigned char L818[] __asm__(".L818");
extern unsigned char L8d8[] __asm__(".L8d8");

unsigned char *OvlFunc_906_2008314(void)
{
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    if (*(short *)((char *)base + off) == (int)(&_AREA_1d))
        return L8d8;
    return L818;
}
