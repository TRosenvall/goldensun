/* Func_8003e10, the whole of goldensun/asm/rom_c0/rom_3d04_c.s.
 *
 * Total .text for this TU = 54 bytes (= 0x36). The .s is replaced outright,
 * so no linker-script change was needed.
 *
 * Copies the ARM routine Func_8001dc8 into RAM, runs it on the caller's
 * argument, and frees the buffer.
 *
 * THE `do { } while (0)` WRAPPERS ARE LOAD-BEARING, and that is the whole point
 * of this file. src/rom_c0/rom_52f4.c -- which already matches -- writes every
 * one of these copy-to-RAM wrappers with the allocation, the DMA and the free
 * inside one `do { } while (0)`, and the call inside a second nested one. It
 * reads like a leftover macro expansion and it is easy to write off as noise.
 *
 * IT IS NOT NOISE. Written flat, the ROM's argument saves and its size load come
 * out in the other order:
 *
 *      rom    mov r8, r0 / mov r10, r1 / ldr r5, =SIZE
 *      ours   ldr r5, =SIZE / mov r8, r0 / mov r0, r5 / mov r10, r1
 *
 * -- four differing lines. With the wrappers, nothing else changed, it is
 * exact. The block scope is what decides when the incoming arguments become
 * pseudos relative to the size constant.
 *
 * A named local for the argument does NOT substitute for it; that was tried
 * first and gcc coalesces it away.
 *
 * The size operand is a POOL TELL -- gcc would build this constant with a
 * single `mov` -- and `_FUNC_8001DC8_SIZE` is emitted by `.func_end_emit_size` in
 * asm/rom_c0/rom_1b70.s. See reports/batch-74.md for why that macro measures what it
 * does.
 */

#include "dma.h"

extern void *Func_8004938(unsigned int size);
extern void Func_8001dc8(int a);
extern void free(void *p);
extern char _FUNC_8001DC8_SIZE[];
#define FUNC_8001DC8_SIZE ((u32) _FUNC_8001DC8_SIZE)

void Func_8003e10(int a)
{
    do {
        void (*func)(int) = Func_8004938(FUNC_8001DC8_SIZE);
        DMA3_SET(Func_8001dc8, func, 0x84000000 | (FUNC_8001DC8_SIZE / 4));
        do {
            func(a);
        } while (0);
        free(func);
    } while (0);
}
