/* Func_8003e10  --  0x08003e10, asm/rom_c0/rom_3d04_c.s
 *
 * BLOCKER CLASS: setup-instruction placement.
 * Status: 26 lines against 26, TWO transposed, everything else exact.
 *
 *      rom    mov r8, r0 / ldr r5, =0xe0
 *      ours   ldr r5, =_FUNC_8001DC8_SIZE / mov r8, r0
 *
 * WHAT IT DOES
 * Copies the ARM routine Func_8001dc8 into RAM, runs it on the caller's
 * argument, and frees the buffer -- the same wrapper as Func_800d304, which
 * matches.
 *
 * THE SIZE SYMBOL IS SOLVED AND IS IN THE BUILD. `ldr r5, =0xe0` where gcc
 * synthesises `mov r5, #0xe0` in one instruction is the pool tell.
 * `_FUNC_8001DC8_SIZE` is now emitted by `.func_end_emit_size` in
 * asm/rom_c0/rom_1b70.s, in the same file and by the same macro as the three
 * `_BLITFADE_*_SIZE` symbols that were already there. Func_8001dc8 runs from
 * 0x08001dc8 to 0x08001ea8 -- 0xe0 exactly, with no veneers or pool to account
 * for, unlike Func_800a494.
 *
 * WHAT IS LEFT is where gcc puts the argument save. The ROM parks the incoming
 * argument in r8 before loading the size; gcc loads the size first. Both are
 * function setup with no dependency between them.
 *
 * WHAT WAS TRIED
 *   - the argument copied into a named local before the allocation call, to
 *     make it the earlier-born pseudo: byte-identical, gcc coalesces it
 *   - -fno-gcse, -fno-strict-aliasing, -fno-caller-saves: byte-identical
 *   - --no-sched2: WORSE, 4 differing lines rather than 2
 *
 * The last one is the informative negative: the scheduler is not what puts them
 * in this order, so turning it off only breaks something else.
 */

#include "dma.h"

extern void *Func_8004938(unsigned int size);
extern void Func_8001dc8(int a);
extern void free(void *p);
extern char _FUNC_8001DC8_SIZE[];
#define FUNC_8001DC8_SIZE ((u32) _FUNC_8001DC8_SIZE)

void Func_8003e10(int a)
{
    void (*func)(int) = Func_8004938(FUNC_8001DC8_SIZE);
    DMA3_SET(Func_8001dc8, func, 0x84000000 | (FUNC_8001DC8_SIZE / 4));
    func(a);
    free(func);
}
