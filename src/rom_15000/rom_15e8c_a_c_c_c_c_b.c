/* Cluster Func_8016738..Func_8016738 extracted from goldensun/asm/rom_15000/rom_15e8c_a_c_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_15000/rom_15e8c_a_c_c_c_c_a.o and asm/rom_15000/rom_15e8c_a_c_c_c_c_c.o in
 * goldensun/stage1.ld.
 *
 * Twin of Func_801671c next door, differing only in the fill value: this one
 * writes 0x44444444 into the same 0xF00-byte text scratch at 0x6002500, where
 * that one writes 0. The fill value is large enough to be pooled, which is why
 * this one has `ldr r2, =0x44444444` where its twin has `mov r2, #0`.
 *
 * See src/rom_15000/rom_15e8c_a_c_c_c_b.c for the two type choices that decide
 * both functions -- the function pointer's return type (which is the
 * declaration lever, reaching through a pointer) and the non-void return type
 * on the function itself (which decides `pop {r1}` over `pop {r0}`).
 *
 * This one was never parked; it came along with its twin.
 */
#include "gba/types.h"

extern int Func_80008d8(void *dest, u32 size, u32 value);

s32 Func_8016738(void)
{
    int (*fill)(void *, u32, u32) = Func_80008d8;

    fill((void *)0x6002500, 0xf0 << 4, 0x44444444);
}
