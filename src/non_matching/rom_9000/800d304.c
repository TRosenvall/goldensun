/* Func_800d304  --  0x0800d304, asm/rom_9000/rom_ca6c_c.s
 *
 * BLOCKER CLASS: a pool tell whose symbol does not exist yet -- and the
 * evidence says what KIND of symbol it is.
 * Status: 21 lines against the ROM's 20. Nineteen of the twenty are exact.
 *
 * WHAT IT DOES
 * The standard "copy an ARM routine to RAM and run it" wrapper: allocate a
 * buffer, DMA3 the routine into it, call it, free it. Identical in shape to
 * DecompressLZ in src/rom_c0/rom_52f4.c, which matches.
 *
 * THE ONE DIFFERENCE IS THE FIRST INSTRUCTION
 *
 *      rom    ldr r5, =0x4e8
 *      ours   mov r5, #0x9d / lsl r5, #3
 *
 * 0x4e8 is 0x9d << 3, so gcc synthesises it in two instructions rather than
 * spending a pool entry. THE ROM POOLED IT, which is the pool tell: the operand
 * was a SYMBOL.
 *
 * AND THE CONVENTION FOR THIS EXACT SYMBOL ALREADY EXISTS IN THE TREE.
 * src/rom_c0/rom_52f4.c declares
 *
 *      extern char _DECOMPRESS_LZ_SIZE[];
 *      #define DECOMPRESS_LZ_SIZE ((u32) _DECOMPRESS_LZ_SIZE)
 *
 * and the value is produced by a macro in the assembly:
 *
 *      asm/rom_c0/rom_2544.s:208
 *      .func_end_emit_size DecompressLZ_ROM, _DECOMPRESS_LZ_SIZE
 *
 * So the missing symbol is the SIZE of the routine being copied, Func_800a494,
 * and the mechanism to define it is `.func_end_emit_size` on that function's
 * end marker -- an assembly edit that emits no bytes, of the same kind as the
 * fourteen `.global` lines HANDOFF.md already records.
 *
 * WHY IT IS NOT DONE HERE, and this is the part to resolve before anyone tries:
 * the size would not come out right. Func_800a494 runs from 0x0800a494 to the
 * `.func_end` at 0x0800a958, which is 0x4c4 bytes. The ROM copies 0x4e8 --
 * THIRTY-SIX BYTES MORE, reaching to 0x0800a97c, which is inside the function
 * the disassembly calls Func_800a958.
 *
 * That is a fact about the ROM, not about the C: either the boundary between
 * those two functions is wrong, or the copied blob deliberately carries the
 * head of the next one. Emitting the size from the current boundary would
 * produce 0x4c4 and a build that does not match, which is worse than a park.
 * Settle the boundary first.
 */

#include "dma.h"

extern void *Func_8004938(unsigned int size);
extern void Func_800a494(void);
extern void free(void *p);

void Func_800d304(void)
{
    unsigned int size = 0x4e8;
    void (*func)(void) = Func_8004938(size);
    DMA3_SET(Func_800a494, func, 0x84000000 | (size / 4));
    func();
    free(func);
}
