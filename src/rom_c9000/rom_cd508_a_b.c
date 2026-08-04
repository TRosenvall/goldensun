/* Cluster Func_80cd508..Func_80cd508 extracted from goldensun/asm/rom_c9000/rom_cd508_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_c9000/rom_cd508_a_a.o and asm/rom_c9000/rom_cd508_a_c.o in
 * goldensun/stage1.ld.
 *
 * Ten instructions. Clears eight bytes at +0x7818 into the block whose address
 * lives at iwram_3001eec, through Func_80008d4 on the `_call_via_r3` veneer.
 *
 * Third function matched out of the `_call_via_rN` class -- see
 * docs/elevation.md. Two details worth noting because they differ from the
 * three that came before it:
 *
 *  1. THE POINTER'S RETURN TYPE IS `void` HERE, not `int`. The rule is not
 *     "always use int"; it is that the return type decides whether r0 is filled
 *     first or last, and this function wants it FIRST -- the ROM computes r0
 *     before loading the pointer:
 *
 *         ldr r3, =iwram_3001eec / ldr r0, [r3] / ldr r3, =0x7818
 *         mov r1, #8 / add r0, r3 / ldr r3, =Func_80008d4 / bl _call_via_r3
 *
 *     Func_80b63b0 and Func_801671c both wanted it last and both needed `int`.
 *     Read the order off the ROM rather than reaching for the lever.
 *
 *  2. The function-pointer load lands AFTER the arguments, which falls out of
 *     the same choice -- gcc emits the pointer where it emits any other operand
 *     it is free to schedule.
 *
 * 0x7818 is pooled and added at runtime, so it is written as its own statement
 * over a typed base rather than folded into the offset; folded, it becomes part
 * of the addressing mode and the `add` disappears.
 */
#include "gba/types.h"

extern void Func_80008d4(void *dst, s32 len);
extern u32 iwram_3001eec;

void Func_80cd508(void)
{
    void (*fp)(void *, s32);
    u8 *p;
    u32 off;

    p = (u8 *)iwram_3001eec;
    off = 0x7818;
    p += off;
    fp = Func_80008d4;
    fp(p, 8);
}
