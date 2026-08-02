/* Func_8091254 @ 0x08091254 -- asm/rom_8a000/rom_8d9a4_c_c_c_a_c_a_a.s
 *
 * CONSTANT CSE. Same instruction count, and the whole second half -- the four
 * shifted constants and the call -- is identical. The two byte offsets differ:
 *
 *     rom    ldr r1, =0x2a01 / add r3, r4, r1 / add r1, #1 / ... / add r3, r4, r1
 *     ours   ldr r2, =0x2a01 / add r3, r4, r2 / ... / ldr r3, =0x2a02 / add r2, r4, r3
 *
 * The ROM derives the second offset from the first with an add; gcc pools it
 * separately. Expressing the pair as adjacent struct members does not help --
 * that makes gcc use an immediate displacement (strb r2, [r3, #1]) instead,
 * which is a third shape and matches neither.
 *
 * Interesting that gcc reproduces the FOUR shifted constants after this
 * exactly (0x380, 0xe00, 0x1880 as 0xe0<<2, 0xe0<<4, 0xc4<<5) without any
 * help. It is only the derived pair it does differently.
 */
#include "gba/types.h"

extern u8 *iwram_3001ed0;
extern void Func_809088c(u8 *a, u8 *b, u8 *c, u32 flags);

/* Blocks until the running fade reaches its target. Returns immediately when
 * no fade state has been allocated.
 */
void Func_8091254(u32 flags)
{
    u8 *base = iwram_3001ed0;

    if (base != NULL) {
        base[0x2a01] = flags;
        base[0x2a02] = 0;
        Func_809088c(base + 0x380, base + 0xe00, base + 0x1880, flags);
    }
}
