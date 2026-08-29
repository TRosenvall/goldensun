/* Cluster Func_80251d4..Func_80251d4 extracted from goldensun/asm/rom_15000/rom_23178_a_a_a_a.s.
 *
 * Slotted between rom_23178_a_a_a_a_a.o and the rest of stage1.ld.
 *
 * Copies 0x20 bytes between two VRAM tile slots. A leaf -- no prologue at all.
 *
 * THE MASK IS A NAMED LOCAL AND THE TWO MASKINGS ARE IN THE ORDER a THEN b,
 * which is the reverse of what the ROM's `and` sequence looks like. The ROM
 * does `and r1, r0` (the SECOND argument) before `and r0, r3`, so the obvious
 * reading is `b &= mask; a &= mask;` -- and that is 4 of 15, with the mask
 * landing in the wrong register. Writing them a-then-b matches.
 *
 * Without the named mask at all it is one instruction SHORTER than the ROM
 * (14 against 15): gcc keeps the argument in r0 and loads the mask into r3,
 * where the ROM spends a `mov r3, r0` to free r0 for the mask. Naming it is
 * what buys the extra instruction back.
 */
#include "dma.h"

void Func_80251d4(int a, int b)
{
    int mask;

    mask = 0x3ff;
    a &= mask;
    b &= mask;
    DMA3_COPY((void *)((a << 5) + (0xc0 << 19)),
              (void *)((b << 5) + (0xc0 << 19)), 0x20);
}
