// fakematch
/* Cluster Task_BlitLuckyWheelsAnim..Task_BlitLuckyWheelsAnim extracted from goldensun/asm/rom_f6000/rom_f6008_c.s.
 *
 * Total .text for this TU = 168 bytes (= 0xa8). Never attempted before batch 275.
 *
 * FAKEMATCH: ONE register pin, at one call site. Row added to fakematch.txt.
 *
 * MINIMUM IS r0 ALONE, and that AMENDS A RECORDED RULE. Measured: r0 alone EXACT;
 * r0 + r1 exact; r1 alone INERT at 2; no pin 2. src/non_matching/.../OvlFunc_942_20087dc's
 * entry records pin-r0-alone as inert and r0+r1 as the minimum for this class, and
 * generalises it to "pin the pair". That is NOT a property of the class -- here the
 * asymmetry runs the other way. SCREEN r0 ALONE FIRST.
 *
 * Without the pin the residue is exactly two instructions, `mov r0, r4` and `lsl r1, #8`
 * transposed: a three-way rank_for_schedule tie at priority 2 with dependent counts 1/1/1
 * (from -fsched-verbose=6), so pure LUID, and the cheap register-copy argument is always
 * emitted last.
 *
 * TWO ORDINARY LEVERS, neither scaffolding:
 *   - `DMA3_SET` RATHER THAN `DMA3_COPY` is a real choice, not a synonym. DMA3_SET's extra
 *     `"r0"` clobber is what produces the second `mov r0, rN` before the following call;
 *     with DMA3_COPY gcc knows r0 still holds the value and the function comes out one
 *     instruction SHORT. 46 differing to 11.
 *   - A 2-CASE DISPATCH THE ROM LAYS OUT AS COMPARE-CHAIN-FIRST, ARMS-AFTER IS A `switch`,
 *     not an if/else-if chain. 59 differing to 46 on the same body.
 *
 * AND A FLAG QUESTION ANSWERED IN THE NEGATIVE: `-fno-gcse` is INERT here (2 differing with
 * and without), and this file's neighbour src/rom_f0000/rom_f0254_a_b.c carries GCSE_CFLAGS,
 * which is what prompted the check. Both rom_f6008 functions landed this batch are exact
 * under the default flags, so NO Makefile row is justified for this file.
 */
#include "gba/types.h"
#include "dma.h"

extern int Func_80008d8(void *dst, u32 size, u32 value);
extern void BlitFadeAlt_Div2(u32 *a, u32 *b, u32 c);
extern void BlitFadeAlt_Div4(u32 *a, u32 *b, u32 c);
extern u8 *iwram_3001eec[];

void Task_BlitLuckyWheelsAnim(void)
{
    int (*fp)(void *, u32, u32);
    u8 *base;
    u8 *buf;
    int mode;
    u32 n;

    base = iwram_3001eec[0];
    if (*(int *)(base + 0x7824) == 1) {
        mode = *(int *)(base + 0x7780);
        buf = iwram_3001eec[1];
        switch (mode) {
        case 1:
            DMA3_SET(buf, (void *)0x6003500, 0x84002000);
            n = *(int *)(base + 0x7784);
            fp = Func_80008d8;
            {
                register void *a0 __asm__("r0") = buf;
                fp(a0, 0x80 << 8, n);
            }
            break;
        case 2:
            if (*(int *)(base + 0x7784) == 0x32)
                BlitFadeAlt_Div2((u32 *)buf, (u32 *)0x6003500, 0x80 << 8);
            else
                BlitFadeAlt_Div4((u32 *)buf, (u32 *)0x6003500, 0x80 << 8);
            break;
        }
        *(int *)(base + 0x7824) = 0;
        *(int *)(base + 0x7820) = 1;
    } else {
        *(int *)(base + 0x7820) += 1;
    }
}
