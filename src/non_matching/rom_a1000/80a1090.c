/* Func_80a1090  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_a1000/rom_a1050_c_a.s
 * Best screen: 9 instructions in disagreeing regions, of 25 (rom 25, ours 24).
 *
 * BLOCKER CLASS: register allocation, plus one folded address.
 *
 * THE DMA BLOCK MATCHES EXACTLY, and getting there is the useful part.
 * `DMA3_SET(&zero, p, cnt)` with a separate `zero = 0;` gets the transfer right
 * but the surrounding four instructions wrong: the ROM stores the fill value
 * THROUGH the register the helper binds --
 *
 *      mov r0, sp / mov r3, #0x0 / str r3, [r0, #0x0]
 *
 * which is what `DMA3_FILL` in include/dma.h does and `DMA3_SET` does not.
 * Switching to `DMA3_FILL(p, 0, 0xa70)` took this from 15 of 25 to 9 and moved
 * the first difference from instruction 1 to instruction 11. The size argument
 * is the count word decoded: 0x8500029c means 0x29c words, so 0xa70 bytes.
 *
 * This is the SECOND dma.h helper confirmed to reproduce the ROM exactly, after
 * DMA3_SET in batch 64. The header is not a blocker; picking the wrong helper
 * is.
 *
 * WHAT REMAINS is r1/r2 exchanged through the store block, and the last store
 * using the register-offset form `strb r3, [r4, r2]` where the ROM computes
 * `add r2, r4, r1` and stores at offset zero. The named-pointer lever is
 * already applied (`q = p + k; *q = w;`) and does not reach it here.
 */
#include "dma.h"

extern unsigned char *iwram_3001f2c;

void Func_80a1090(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned int k;
    int v;
    int w;

    p = iwram_3001f2c;
    DMA3_FILL(p, 0, 0xa70);
    k = 0x89;
    v = 0xff;
    k <<= 1;
    p[0x1c] = v;
    q = p + k;
    w = 1;
    k += 1;
    p[0x1e] = w;
    p[0x1f] = w;
    *q = w;
    q = p + k;
    *q = w;
}
