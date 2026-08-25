/* OvlFunc_914_2008c0c -- NOT MATCHING. 21 lines against the ROM's 22.
 *
 * Source asm: goldensun/asm/overlays/rom_7a1ff0/ovl_30_c_c_c_c_c_c_a_c.s
 *
 * FOUR INSTRUCTION-IDENTICAL COPIES, one per overlay, so solving this elevates
 * four functions:
 *
 *   OvlFunc_914_2008c0c   asm/overlays/rom_7a1ff0/ovl_30_c_c_c_c_c_c_a_c.s
 *   OvlFunc_915_2008ddc   asm/overlays/rom_7a2bf0/ovl_30_c_c_c_c_c_c.s
 *   OvlFunc_916_2008fb4   asm/overlays/rom_7a37f0/ovl_30_c_c_c_c_c_c.s
 *   OvlFunc_917_20098b8   asm/overlays/rom_7a4370/ovl_30_c_c_c_c_c_c_c.s
 *
 * Blocker: THE ROM LOADS &REG_DMA3SAD TWICE, ONCE IN EACH ARM, and performs the
 * stmia ONCE after the join:
 *
 *     beq  .Lc1c
 *     ldr r3, =REG_DMA3SAD / ldr r0, =.L17b0 / b .Lc20
 *   .Lc1c:
 *     ldr r3, =REG_DMA3SAD / ldr r0, =.L10b0
 *   .Lc20:
 *     ldr r2, =0x840000e0 / stmia r3!, {r0, r1, r2} / sub r3, #0xc
 *
 * That is a tail merge that kept the pool load duplicated. DMA3_COPY sets its
 * base register inside the inline function, so the number of `ldr r3, =` is the
 * number of times DMA3_COPY appears:
 *
 *   one call after the `if`, selecting only the source   21 lines (this body),
 *                                                        one `ldr r3, =`
 *   a call in EACH arm                                   24 lines, the whole
 *                                                        stmia block duplicated
 *
 * gcc-2.96 does not perform the partial tail merge that would give 22. Nothing
 * in the C chooses how many times a `register ... __asm__("r3")` binding is
 * materialised -- that is inside include/dma.h.
 *
 * NEXT: this is a property of the dma.h helper, not of this function. A variant
 * of DMA3_COPY that takes the base as a parameter, or a formulation where the
 * source selection happens after the base is bound, might reach it. Worth doing
 * once for four functions, and worth checking against the OTHER stmia users
 * first -- 264 unelevated functions contain one.
 */
#include "dma.h"
extern unsigned int iwram_3001ed0;
extern unsigned char L17b0[] __asm__(".L17b0");
extern unsigned char L10b0[] __asm__(".L10b0");
extern void __Func_8091200(int a, int b);
extern void OvlFunc_914_2008bcc(void);

void OvlFunc_914_2008c0c(int which)
{
    unsigned char *src;

    if (which)
        src = L17b0;
    else
        src = L10b0;
    DMA3_COPY(src, (void *)iwram_3001ed0, 0x380);
    __Func_8091200(0x80 << 9, 0);
    OvlFunc_914_2008bcc();
}
