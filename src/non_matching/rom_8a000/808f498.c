/* Func_808f498 -- 0x0808f498 -- asm/rom_8a000/rom_8d9a4_c_c_a_a.s
 *
 * BLOCKER: gcc CSEs the IO register addresses; the ROM loads each fresh.
 * 12 of 54, LENGTH EXACT.
 *
 * Selects a 644-byte record by an index byte, runs the DMA0 prefix, ORs 0x6000
 * into REG_DISPCNT, writes four window registers from the record, sets both
 * vertical windows to 0xa0, and kicks a DMA0 transfer of the rest of the
 * record into REG_WIN0H.
 *
 * The first 26 instructions are exact -- the record arithmetic
 * (idx*5, <<5, +idx, <<2 = idx*644), UnknownDMAPrefix(), and the DISPCNT
 * read-modify-write all reproduce unaided.
 *
 * ONE LEVER LANDED, worth three lines and half the count: WALK THE POINTER,
 * do not index it. Written as `*(u16 *)(e + 2)`, `(e + 4)`, `(e + 6)`, gcc
 * emits immediate-offset loads and the function is THREE LINES SHORT. The ROM
 * advances the record pointer between reads (`add r0, #2`), which is what
 * `e += 2;` between the stores produces.              26 differ -> 12
 *
 * WHAT REMAINS is how the destination addresses are formed:
 *
 *     rom    ldr r2, =0x4000048 / ... / ldr r1, =0x4000040 / ldr r2, =0x4000042
 *     ours   sub r1, #0xa  /  add r4, #0x42
 *
 * The four window registers live within 0x4000040..0x400004a, so gcc CSEs one
 * address and derives the others by adding or subtracting small displacements.
 * The ROM pool-loads each one separately. That is the repeated-expensive-
 * constant class docs/elevation.md already records as not yielding to spelling.
 *
 * MEASURED: routing the stored 0xa0 through an `int` local -- the operand-mode
 * fix -- is BYTE-IDENTICAL at 12. That matches the precondition added this
 * batch from OvlFunc_common1_148: the fix costs a live value, and this function
 * has no spare register at that store. Two functions now confirm the
 * precondition, one where the fix backfired and one where it simply does
 * nothing.
 */
#include "dma.h"

extern int iwram_3001ecc;

void Func_808f498(void)
{
    char *e;
    int i;

    e = (char *)iwram_3001ecc;
    i = *(unsigned char *)(e + 0x539);
    e = e + i * 644;
    UnknownDMAPrefix();
    REG_DISPCNT |= 0x6000;
    REG_WININ = *(unsigned short *)e;
    e += 2;
    REG_WINOUT = *(unsigned short *)e;
    e += 2;
    REG_WIN0H = *(unsigned short *)e;
    e += 2;
    REG_WIN1H = *(unsigned short *)e;
    REG_WIN0V = 0xa0;
    REG_WIN1V = 0xa0;
    e += 2;
    DMA0_SET(e, (void *)&REG_WIN0H, 0xa6600001);
}
