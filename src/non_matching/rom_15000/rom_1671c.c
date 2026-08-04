/* Func_801671c @ 0x0801671c -- asm/rom_15000/rom_15e8c_a_c_c_c.s
 *
 * Source asm: goldensun/asm/rom_15000/rom_15e8c_a_c_c_c.s
 *
 * Blocker class 5, SCHEDULING. Nine instructions, nine right, one in the
 * wrong place:
 *
 *     rom    ldr r3, =Func_80008d8 / lsl r1, #4 / mov r2, #0 / ldr r0, =0x6002500
 *     ours   ldr r0, =0x6002500 / ldr r3, =Func_80008d8 / lsl r1, #4 / mov r2, #0
 *
 * gcc hoists the destination address above the function-pointer load. Tried:
 * the pointer via a typedef'd local, via a plain local, and with the
 * destination in its own local assigned after it; the size argument as a
 * literal and as a named local. The address moves to the front every time.
 *
 * Note the non-void return type with no return statement -- that IS required,
 * and getting it wrong is worth two instructions (pop r0/bx r0 instead of
 * pop r1/bx r1). See docs/elevation.md.
 */
#include "gba/types.h"

extern void Func_80008d8(void *dest, u32 size, u32 value);

/* Fills the 0xF00-byte text scratch at 0x6002500 with 0 (transparent).
 * Func_80008d8 is an ARM routine in IWRAM, so the call goes through a
 * function pointer and lands on the _call_via_r3 veneer.
 */
s32 Func_801671c(void)
{
    void (*fill)(void *, u32, u32) = Func_80008d8;

    fill((void *)0x6002500, 0xf00, 0);
}
