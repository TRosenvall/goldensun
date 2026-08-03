/* Func_80a3480 @ 0x080a3480 -- asm/rom_a1000/rom_a1814_c_a_c_c_c_a.s
 *
 * Blocker class 2, REGISTER BIRTH ORDER, in the loop SET-UP only. All 28
 * instructions are present and the loop body is identical; the five setup
 * moves come out in a different order, and the constant lands in r3 rather
 * than r2:
 *
 *     rom    mov r2, #0xd / mov r7, r3 / mov r6, #0 / mov r8, r2 / add r7, #0x48
 *     ours   mov r7, r3 / mov r3, #0xd / add r7, #0x48 / mov r6, #0 / mov r8, r3
 *
 * The ROM builds the constant BEFORE the pointer. Hoisting it into a local
 * declared first changes the order but not the outcome -- it just moves the
 * divergence from instruction 5 to instruction 3.
 *
 * Worth noting what came out right without any help: `ldmia r7!, {r5}` for the
 * pointer walk, and the modulo lowered to a __modsi3 call rather than a
 * strength-reduced sequence.
 */
#include "gba/types.h"

struct Sprite {
    u8 pad_00[5];
    u8 flags;
};

extern u8 *iwram_3001f2c;

/* Marks every fifth sprite in the 32-slot table at +0x48 -- the row headings
 * in the five-wide layouts. Null slots are skipped without consuming an index,
 * so the position follows the SLOT index, not the occupied count.
 */
void Func_80a3480(void)
{
    struct Sprite **slot = (struct Sprite **)(iwram_3001f2c + 0x48);
    s32 i;

    for (i = 0; i <= 0x1f; i++) {
        struct Sprite *sprite = *slot++;

        if (sprite != NULL && i % 5 == 0)
            sprite->flags = 0xd;
    }
}
