/* Cluster Func_801671c..Func_801671c extracted from goldensun/asm/rom_15000/rom_15e8c_a_c_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_15000/rom_15e8c_a_c_c_c_a.o and asm/rom_15000/rom_15e8c_a_c_c_c_c.o in
 * goldensun/stage1.ld.
 *
 * Fills the 0xF00-byte text scratch at 0x6002500 with 0 (transparent).
 * Func_80008d8 is an ARM routine in IWRAM, so the call goes through a function
 * pointer and lands on the _call_via_r3 veneer.
 *
 * PARKED as "class 5, SCHEDULING" -- nine instructions, nine right, one in the
 * wrong place:
 *
 *     rom    ldr r3, =Func_80008d8 / lsl r1, #4 / mov r2, #0 / ldr r0, =0x6002500
 *     ours   ldr r0, =0x6002500 / ldr r3, =Func_80008d8 / lsl r1, #4 / mov r2, #0
 *
 * It is not scheduling. THE ROM FILLS r0 LAST, which is the declaration lever,
 * and the note's three attempts were all about WHERE THE POINTER LIVED -- via a
 * typedef'd local, via a plain local, with the destination in its own local --
 * rather than about its TYPE.
 *
 * For an indirect call the function pointer's return type IS the declaration;
 * there is no other one. Typing it `int (*)(void *, u32, u32)` instead of
 * `void (*)(...)` keeps r0 live and defers the write. That is the same lever
 * docs/elevation.md already describes for direct calls, reaching through a
 * pointer type.
 *
 * Its twin Func_8016738 sits immediately after this in the original .s and
 * differs only in the fill value; it was never parked and matched the same way.
 *
 * KEEP, and the park note was right about this one: the non-void return type
 * with no return statement is required. gcc pops its return address into r1
 * rather than r0 when the return type is non-void, which is what the ROM does.
 * That rule is in docs/elevation.md and the note cited it correctly -- it was
 * the argument order it misdiagnosed, not the epilogue.
 */
#include "gba/types.h"

extern int Func_80008d8(void *dest, u32 size, u32 value);

s32 Func_801671c(void)
{
    int (*fill)(void *, u32, u32) = Func_80008d8;

    fill((void *)0x6002500, 0xf0 << 4, 0);
}
