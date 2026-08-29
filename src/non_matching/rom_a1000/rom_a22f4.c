/* Func_80a22f4 -- NOT MATCHING. 3 of 12 lines, same length.
 *
 * Source asm: goldensun/asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a.s
 *
 * Blocker: gcc STRENGTH-REDUCES THE SECOND TRANSFER'S CONSTANTS off the first
 * transfer's, and the ROM only does it for one of the three.
 *
 *     rom    ... stmia / sub r3,#0xc / add r1,#0x1c / ldr r0,=0x50001e8
 *                                                  / ldr r2,=0x80000001 / stmia
 *     ours   ... stmia / sub r3,#0xc / sub r0,#0x18 / add r1,#0x1c
 *                                                  / sub r2,#0xf / stmia
 *
 * 0x5000200 - 0x18 is 0x50001e8 and 0x80000010 - 0xf is 0x80000001, so gcc is
 * right and cheaper. The ROM reloads both from the pool and adjusts only the
 * destination.
 *
 * This is the batch-48 phenomenon -- an add/sub chain on a constant is often
 * gcc's own arithmetic -- running the OTHER WAY. There the ROM adjusted and the
 * source needed literals; here the source HAS literals and gcc adjusts anyway.
 * So the batch-48 rule ("try the literal form first") is necessary but not
 * sufficient: it stops you writing the chain, it cannot stop gcc finding one.
 *
 * TRIED:
 *   both destinations as literals                        3 of 12 (this body)
 *   the destination as a named local advanced by 0x1c    13 lines: the local is
 *                                                        live across the asm
 *                                                        block, which binds
 *                                                        r0-r3, so gcc parks it
 *                                                        in r4 and pays a
 *                                                        `mov r1, r4`
 *   DMA3_SET with raw count words instead of DMA3_COPY16 3 of 12, identical
 *
 * THE HELPER FIX WAS TRIED AND HALF WORKS -- do not spend the round rediscovering
 * it. A DMA3_COPY16 variant declaring the destination as a READ-WRITE operand,
 *
 *     : "+l" (_dst)
 *     : "l" (_base), "l" (_src), "l" (_cnt)
 *     : "memory"
 *
 * and returning _dst, tells gcc that r1 both supplies and carries the value.
 * That REMOVES THE SPILL: the destination stays in r1, the `mov r1, r4`
 * disappears, and the length drops from 13 to 12 with the ROM's `add r1, #0x1c`
 * in place.
 *
 * It does NOT fix the remaining three. Those are the strength reduction above,
 * which is about two CONSTANTS and has nothing to do with register binding. The
 * variant was reverted rather than left unused in a shared header, but it is
 * written out here because it is a one-step change if anyone wants it.
 *
 * So the two parks in this family split apart under the experiment:
 *   this one          the binding was ONE of two defects; fixing it is not enough
 *   ovl_7a1ff0/2008c0c.c  needs gcc's partial tail merge, which no helper shape
 *                         reaches -- the count of base loads is the count of calls
 *
 * NEXT: nothing at the helper level for this function. The remaining defect is
 * gcc choosing to derive 0x50001e8 and 0x80000001 from constants it already
 * holds, and no spelling of a literal prevents that (see batch 48, which
 * establishes the rule running the other way).
 */
#include "dma.h"

void Func_80a22f4(void)
{
    DMA3_COPY16((void *)0x5000200, (void *)0x50001c0, 0x40);
    DMA3_COPY16((void *)0x50001e8, (void *)0x50001dc, 4);
}
