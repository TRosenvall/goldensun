/* Func_80a3d6c @ 0x080a3d6c -- asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a.s
 *
 * Source asm: goldensun/asm/rom_a1000/rom_a1814_c_a_c_c_c_c_c_a.s
 *
 * Blocker class 2, REGISTER BIRTH ORDER. Twenty instructions, all twenty
 * right, r4 and r5 swapped throughout:
 *
 *     rom    ldr r4, =0x1ff / mov r5, #0    (mask in r4, count in r5)
 *     ours   ldr r5, =0x1ff / mov r4, #0
 *
 * The ROM builds the mask first so it takes r4. Hoisting it into a named
 * local declared before the counter does not do it -- that variant loses an
 * instruction somewhere else instead (19 vs 20).
 *
 * RECHECKED from the other angle.  The source-order lever recorded in batch 134
 * is about the order of ASSIGNMENT STATEMENTS, not declarations, so it was worth
 * screening separately.  Both orders -- mask assigned before count, and count
 * before mask -- give 20 lines against the ROM's 22 and 19 differing, i.e. the
 * same two-instruction loss the earlier declaration test found.
 *
 * So naming the mask at all is what costs the instructions, independently of
 * where it is named.  The lever reaches two independent values that are ALREADY
 * both in registers; it does not reach a constant that gcc would otherwise fold
 * into the loop, because naming it changes how many instructions exist rather
 * than which register each gets.  That boundary is worth carrying to the other
 * register-birth-order parks.
 */
#include "gba/types.h"

extern u8 *_GetUnit(s32 unitId);

/* Counts the non-empty slots among the fifteen inventory halfwords at +0xD8.
 *
 * The slot format, worth recording: bits 0..8 the item id, bit 9 locked
 * (equipped or a key item), bits 11..15 the quantity less one. That is why
 * consuming one unit subtracts 0x800.
 */
s32 Func_80a3d6c(s32 unitId)
{
    u16 *slot = (u16 *)(_GetUnit(unitId) + 0xd8);
    s32 count = 0;
    s32 i;

    for (i = 14; i >= 0; i--) {
        if ((*slot++ & 0x1ff) != 0)
            count++;
    }
    return count;
}
