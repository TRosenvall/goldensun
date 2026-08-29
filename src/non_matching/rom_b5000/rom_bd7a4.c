/* Func_80bd7a4 -- NOT MATCHING. 17 lines against the ROM's 23.
 *
 * Source asm: goldensun/asm/rom_b5000/rom_bbb0c_a_a_c.s
 *
 * Blocker: the dma.h register-binding class again -- see
 * src/non_matching/rom_a1000/rom_a22f4.c and
 * src/non_matching/ovl_7a1ff0/2008c0c.c.
 *
 * THREE IDENTICAL TRANSFERS, and the ROM REBUILDS EVERY ARGUMENT FOR EACH:
 *
 *     mov r2,#0x84 / ldr r3,=REG_DMA3SAD / mov r0,#0 / mov r1,#0 / lsl r2,#24
 *     stmia r3!, {r0,r1,r2} / sub r3,#0xc
 *     mov r2,#0x84 / mov r0,#0 / lsl r2,#24 / stmia / sub r3,#0xc
 *     mov r2,#0x84 / mov r0,#0 / lsl r2,#24 / stmia / sub r3,#0xc
 *
 * The inline asm's operands are INPUTS, not clobbers, so gcc correctly knows
 * r0-r2 still hold their values afterwards and sets them once. Six instructions
 * fewer. The original compiler rebuilt them.
 *
 * There is no source-level handle: the values are literals and gcc is right
 * about their liveness. Listing r0-r2 as clobbers in dma.h would force the
 * rebuild, but that is a change to a shared header affecting every DMA user in
 * the tree and would have to be checked against all of them -- see the note in
 * rom_a22f4.c about doing helper work once, against the whole set.
 *
 * The tail is an indirect call through a global function pointer, written with
 * the named-local idiom from src/overlays/rom_780898/ovl_30_a_a_a_b.c. That
 * part is right.
 */
#include "dma.h"
extern void (*iwram_30000c4)(void);

void Func_80bd7a4(void)
{
    void (*fp)(void);

    DMA3_SET((void *)0, (void *)0, 0x84000000);
    DMA3_SET((void *)0, (void *)0, 0x84000000);
    DMA3_SET((void *)0, (void *)0, 0x84000000);
    fp = iwram_30000c4;
    fp();
}
