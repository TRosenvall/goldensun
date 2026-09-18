/* Cluster Func_80a22f4..Func_80a22f4 extracted from goldensun/asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c_c.s.
 *
 * Total .text for this TU = 48 bytes (= 0x30).
 * Preserves the original ROM layout when slotted between
 * asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c_b.o and asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c_c_c.o in
 * goldensun/stage1.ld.
 *
 * Two DMA copies into palette RAM. No pins, no flags.
 *
 * THE BLOCKER WAS NEVER THE CONSTANTS -- it was that the shared DMA3_COPY16
 * helper tells gcc the `stmia` leaves r0 and r2 intact. CSE then knows r0 still
 * holds 0x5000200 and r2 still holds 0x80000010 at the second call, and derives
 * the second pair with `sub r0, #0x18` / `sub r2, #0xf`. The ROM reloads both
 * from the pool:
 *
 *     rom    add r1, #0x1c / ldr r0, =0x50001e8 / ldr r2, =0x80000001
 *     ours   sub r0, #0x18 / add r1, #0x1c      / sub r2, #0xf
 *
 * Note the DESTINATION is derived in BOTH (`add r1, #0x1c`), which is the tell
 * that this is about those two operands specifically and not about constants in
 * general.
 *
 * Declaring the source and the count as READ-WRITE operands (`"+l"`) makes gcc
 * treat them as modified, CSE loses both values, and the second call reloads them
 * -- while the destination, still a plain input, keeps being derived exactly as
 * the ROM has it. Both destinations stay bare literals. That is the whole fix.
 *
 * HONESTY ABOUT THE CONSTRAINT: the asm does NOT write r0 or r2. It writes r3
 * (and memory). `"+l"` therefore overstates what the instruction does. It is
 * still the right constraint here for two reasons. It errs in the SAFE direction
 * -- it only makes gcc more conservative about reuse, so no wrong code can follow
 * from it -- and it is legal, where the alternative that also matches (adding
 * "r0","r2" to the clobber list) names INPUT registers as clobbered, which the
 * gcc manual makes undefined. Batch 268 declined exactly that clobber widening on
 * DMA3_SET, and this is the same judgement reached the same way.
 *
 * AND THE ROM IS EVIDENCE FOR IT. The original build did not derive these
 * constants, so whatever helper the original source used did not promise gcc that
 * r0 and r2 survive the transfer. Reconstructing that promise-free helper is what
 * this is.
 *
 * DELIBERATELY STATIC AND LOCAL rather than added to include/dma.h. One function's
 * evidence does not justify a shared-header change, and 29 files use the existing
 * helpers. Promote it if a second function needs it; DO NOT retrofit DMA3_COPY16,
 * which the rest of the tree depends on keeping its current promise.
 *
 * MEASURED, at object level: "+l" on the count alone is 44 bytes and 5 places;
 * on the source alone, 44 bytes and 2 places -- each leaves the other constant
 * strength-reduced, so both operands are load-bearing. The existing DMA3_SET
 * (which clobbers r0 alone) is 44 bytes and 2 places.
 */
#include "dma.h"

/* DMA3_COPY16 without the promise that the transfer preserves src and count. */
static inline void DMA3_COPY16_RW(void *src, void *dst, u32 size)
{
    register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
    register void *_src __asm__("r0") = src;
    register void *_dst __asm__("r1") = dst;
    register u32 _cnt __asm__("r2") = 0x80000000 | (size / 4);
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        : "+l" (_src), "+l" (_cnt)
        : "l" (_base), "l" (_dst)
        : "memory"
    );
}

void Func_80a22f4(void)
{
    DMA3_COPY16_RW((void *)0x5000200, (void *)0x50001c0, 0x40);
    DMA3_COPY16_RW((void *)0x50001e8, (void *)0x50001dc, 4);
}
