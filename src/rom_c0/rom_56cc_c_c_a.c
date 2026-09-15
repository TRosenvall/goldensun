/* Func_8005c68 -- 0x08005c68, asm/rom_c0/rom_56cc_c_c.s (text half now _a, data half _b).
 *
 * Reloads three save slots from flash: clears each 0x40-byte header by DMA,
 * reads the slot body if its sector id is valid, then reads a four-byte tail
 * from a second sector or zeroes it. Returns how many bodies were read.
 *
 * WHOLE-FILE CONVERSION -- one function in the .s, no split, stage1.ld:71
 * verbatim with its asm/ prefix (line 98's .rodata entry is unaffected -- the
 * object has none either way), no Makefile rule, no pins.
 *
 * DMA3_SET, NOT DMA3_CLEAR, AND THE CALLER OWNS THE ZERO WORD. This is the
 * whole result and it is worth stating carefully, because DMA3_CLEAR is the
 * obvious reading and produces a function FOUR INSTRUCTIONS SHORT.
 *
 * DMA3_CLEAR declares its own `u32 value;` internally. Inlined in a loop, gcc
 * gives it a stack slot, writes it through `sp` each iteration, and never needs
 * a register for the address -- so the ROM's `mov r7, sp` outside the loop and
 * `str r2, [r7]` inside it have nowhere to come from. Three things follow from
 * the ROM instead:
 *
 *   1. the zero word is a local of THIS function, not of the macro;
 *   2. it is written through a POINTER, because the ROM stores via r7 rather
 *      than via sp -- `*q = 0;` where `q = &value;` is hoisted out of the loop;
 *   3. the transfer is DMA3_SET(q, p, 0x85000010), which takes the source
 *      pointer as an argument, rather than DMA3_CLEAR(p, 0x40), which
 *      manufactures one.
 *
 * Getting (3) alone is 63 instructions against 65; adding (2) is exact. The
 * pointer is what makes the address loop-invariant AND gives the store and the
 * asm operand the same register, which is the extra callee-saved register in
 * the ROM's prologue.
 *
 * TWO SMALLER READINGS:
 *
 *   `cmp r0, #0xf / bhi` followed by `lsl r0, #16 / lsr r0, #16` is an UNSIGNED
 *   local compared, then TRUNCATED FOR THE CALL -- not a u16 return value. The
 *   sector id is `unsigned int`, the guard is `v <= 0xf`, and the narrowing is
 *   ReadFlash's `unsigned short` first parameter doing its job.
 *
 *   `mov r1, #0x88 / lsl r1, #1` is the offset 0x110 built the usual way, and
 *   is spelled `0x88 << 1` here to match how the ROM builds it rather than as
 *   the folded constant.
 *
 * EXACT: 144 bytes, 65 encodings, 5 relocations, measured three times, clean on
 * tools/tryc.py.
 */
#include "gba/types.h"
#include "dma.h"

extern unsigned char *iwram_3001f1c;
extern unsigned int Func_8005b24(int n);
extern void ReadFlash(unsigned short sectorNum, unsigned int offset, void *dest, unsigned int size);

int Func_8005c68(void)
{
    u32 value;
    u32 *q;
    u8 *p;
    unsigned int i;
    int count;
    unsigned int v;

    p = iwram_3001f1c + (0x82 << 5);
    count = 0;
    q = &value;
    for (i = 0; i <= 2; i++) {
        *q = 0;
        DMA3_SET(q, p, 0x85000010);
        v = Func_8005b24(i);
        if (v <= 0xf) {
            ReadFlash(v, 0, p, 0x40);
            count++;
        }
        v = Func_8005b24(i + 3);
        if (v <= 0xf)
            ReadFlash(v, 0x88 << 1, p + 0x38, 4);
        else
            *(int *)(p + 0x38) = 0;
        p += 0x40;
    }
    return count;
}
