/* Cluster OvlFunc_882_2008198..OvlFunc_882_2008198 extracted from goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_a_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_77dd1c/overlay.ld is
 * unchanged.
 *
 * Eighteen instructions: a sound, a table-driven call, a movement, a wait and a
 * hand-off. Matched on the first screen with no lever at all, which is worth
 * one line of note by itself -- every call here wants r0 filled last, so the
 * house style of leaving callees implicit is simply correct and nothing needed
 * declaring.
 *
 * The one thing to get right is `.L5774`, an overlay-local data label that C
 * cannot spell. It is bound with a gcc asm-label:
 *
 *     extern unsigned char L5774[] __asm__(".L5774");
 *
 * which emits an R_ARM_ABS32 against `.L5774`, identical to the ROM's
 * `ldr r0, =.L5774`.
 *
 * 0x1a4 is built at runtime (`mov r2, #0xd2 / lsl r2, #1`) and written as
 * `0xd2 << 1` rather than folded; 0x101 is pooled because it does not fit an
 * eight-bit mov, and that is gcc's own choice rather than the pool tell -- the
 * tell only applies to a constant that WOULD fit.
 */
extern unsigned char L5774[] __asm__(".L5774");
extern void __Func_8010560(void *p, int a, int b);

void OvlFunc_882_2008198(void)
{
    __PlaySound(0x9e);
    __Func_8010560(L5774, 0x2d, 0xb);
    __Func_809218c(0, 0x101, 0xd2 << 1);
    __CutsceneWait(3);
    OvlFunc_882_200815c(0xb);
}
