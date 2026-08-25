/* Cluster Func_800d304..Func_800d304 extracted from goldensun/asm/rom_9000/rom_ca6c_c.s.
 *
 * Total .text for this TU = 42 bytes (= 0x2a).
 * Slotted between the _a and _c pieces in goldensun/stage1.ld; the _c piece
 * keeps the .rodata blob.
 *
 * Copies an ARM routine into RAM, runs it, and frees the buffer -- the same
 * shape as DecompressLZ in src/rom_c0/rom_52f4.c.
 *
 * UNPARKED BY DEFINING THE SIZE SYMBOL, and by working out what it actually
 * measures. `ldr r5, =0x4e8` where gcc synthesises `0x9d << 3` in two
 * instructions is the pool tell: the operand was a symbol. The convention was
 * already in the tree -- rom_52f4.c uses `_DECOMPRESS_LZ_SIZE`, emitted by the
 * `.func_end_emit_size` macro -- but applying it to Func_800a494 gives 0x4c4,
 * not 0x4e8, and would have produced a build that does not match.
 *
 * THE THIRTY-SIX MISSING BYTES ARE VENEERS AND A POOL. Func_800a494 calls
 * `bl Func_800a958`, and Func_800a958/a960/a968 are 8-byte `ldr r4, =X / bx r4`
 * thunks sitting immediately after it, followed by the 12-byte pool those three
 * read from:
 *
 *      0x4c4  Func_800a494 body
 *      0x018  three veneers it calls through
 *      0x00c  their pool
 *      -----
 *      0x4e8  exactly what this function copies
 *
 * So the blob is the routine PLUS everything it needs to run from RAM, and the
 * size has to be measured from the end of that pool rather than from
 * Func_800a494's own `.func_end`. `_FUNC_800A494_SIZE` is defined that way in
 * asm/rom_9000/rom_92b8.s. It emits no bytes -- an absolute symbol definition,
 * the same class of assembly edit as the `.global` lines HANDOFF.md records.
 *
 * The name is a placeholder keyed to the address, in the same spirit as
 * `_AREA_xx` and `_MSG_xx`: it asserts what the symbol MEASURES, not what the
 * routine is for.
 */

#include "dma.h"

extern void *Func_8004938(unsigned int size);
extern void Func_800a494(void);
extern void free(void *p);
extern char _FUNC_800A494_SIZE[];
#define FUNC_800A494_SIZE ((u32) _FUNC_800A494_SIZE)

void Func_800d304(void)
{
    void (*func)(void) = Func_8004938(FUNC_800A494_SIZE);
    DMA3_SET(Func_800a494, func, 0x84000000 | (FUNC_800A494_SIZE / 4));
    func();
    free(func);
}
