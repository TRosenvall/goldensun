/* Func_80bf524 @ 0x080bf524, Func_80bf54c @ 0x080bf54c
 * Source asm: goldensun/asm/rom_b5000/rom_bbb0c_a_c_c_a.s
 *   -- asm/rom_b5000/rom_bbb0c_a_c_c_a.s
 *
 * Blocker class 2 plus a truncation difference. Both functions, identically.
 * Control flow, the shared exit and the constant materialisation are all
 * right; two things are not:
 *
 *   1. the counter pointer lands in r2 rather than r1, which pushes the
 *      loaded byte into r3 and removes the ROM's `mov r3, r2` copy
 *   2. the ROM stores the decremented value UNTRUNCATED and lets strb do the
 *      narrowing, then tests with a bare `lsl r3, #24`. Every formulation
 *      here truncates before the store (`lsl`/`lsr` pair), which costs an
 *      instruction:
 *
 *          rom    add r3, #0xff / strb r3, [r1] / lsl r3, #0x18
 *          ours   add r3, #0xff / lsl r3, #0x18 / lsr r3, #0x18 / strb r3, [r2]
 *
 * Tried: the counter in a u8 local and in an s32 local; the decrement as
 * value-- and as a separate `next` variable; the second test as
 * `value == 0` and as `(u8)value == 0`; early return versus nested if. The u8
 * local always truncates before the store, the s32 local drops the copy.
 *
 * These two are worth retrying together -- they are the same function at two
 * offsets, so whatever fixes one fixes both, and there is a third sibling
 * family (Func_bf250) with the same shape.
 */
#include "gba/types.h"

extern u8 *_GetUnit(s32 unitId);

/* Decrements the counter at +0x13E and returns 1 on the turn it reaches zero.
 * A counter already at zero returns 0 without writing.
 */
s32 Func_80bf524(s32 unitId)
{
    u8 *counter = _GetUnit(unitId) + 0x13e;
    u8 value = *counter;

    if (value != 0) {
        value--;
        *counter = value;
        if (value == 0)
            return 1;
    }
    return 0;
}

/* The same, at +0x13F. */
s32 Func_80bf54c(s32 unitId)
{
    u8 *counter = _GetUnit(unitId) + 0x13f;
    u8 value = *counter;

    if (value != 0) {
        value--;
        *counter = value;
        if (value == 0)
            return 1;
    }
    return 0;
}
