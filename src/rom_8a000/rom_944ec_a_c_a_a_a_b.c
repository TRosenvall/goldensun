/* Cluster Func_80958a8..Func_80958a8 extracted from goldensun/asm/rom_8a000/rom_944ec_a_c_a_a_a.s.
 *
 * Slotted between rom_944ec_a_c_a_a_a_a.o and the rest of stage1.ld.
 *
 * Allocates a 0x720-byte IWRAM block, zeroes it with DMA3_CLEAR, and starts a
 * task on it.
 *
 * StartTask IS DELIBERATELY LEFT UNDECLARED -- the subtractive side of the
 * declaration lever. Declared, gcc emits its `ldr r0, =Func_8095884` between
 * `mov r1, #0xc8` and that register's `lsl r1, #4`; the ROM emits it after the
 * shift. 2 of 21 with the prototype, exact without it.
 */
#include "dma.h"
extern void *galloc_iwram(int tag, int size);
extern void Func_8095884(void);

void Func_80958a8(void)
{
    void *p;

    p = galloc_iwram(0x38, 0xe4 << 3);
    DMA3_CLEAR(p, 0x720);
    StartTask(Func_8095884, 0xc8 << 4);
}
