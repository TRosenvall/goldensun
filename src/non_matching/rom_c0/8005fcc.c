/* Func_8005fcc (0x08005fcc) -- NON-MATCHING.
 * Blocker class: pooled small constants at hardware-register writes, plus an
 * `and` destination.
 *
 * 86 lines against the ROM's 85. A link-cable status read: no calls at all, so
 * nothing forces callee-saved registers and the residue is pure selection.
 *
 * THE ROM BUILDS ITS SMALL CONSTANTS AND gcc POOLS THEM, four times:
 *
 *     rom    mov r3, #0x81 / neg r3, r3      ours  ldr r3, =0xff7f
 *     rom    mov r2, #0x40                   ours  ldr r2, =0x40
 *     rom    mov r3, #0x1                    ours  ldr r3, =0x1
 *     rom    mov r3, #0x1 / sub r3, #0x42    ours  mov r3, #0xbf
 *
 * All four feed halfword writes to I/O registers, which is the pooling
 * exception recorded in batch 175 and worked around in batch 176 by naming the
 * stored value. Naming works here too but is not free:
 *
 *   baseline                                     83 lines, 77 differing
 *   `one = 1` named, block-scoped                86, 70   <- best
 *   `bb = 0x40` named                            83, 77 (inert)
 *   all three of `one`, the mask and 0x40 named  86, 76
 *
 * Each name that lands costs the `mov`/`neg` pair it buys, so the length walks
 * past the ROM's before the differing count converges. That is the shape of a
 * function where several constants each need the treatment and the register
 * file cannot hold them all -- the same bound as the basic-block lever.
 *
 * The other residue is the first mask:
 *
 *     rom    mov r3, #0x88 / mov r4, r5 / and r4, r3
 *     ours   mov r6, #0x88 / and r6, r5
 *
 * The ROM copies the value and masks IN PLACE, keeping the result in the
 * register it later stores from; gcc masks into the constant's register.
 * `t = s; t &= 0x88;` as two statements is byte-identical -- gcc folds it.
 *
 * WHAT IS RIGHT: the 32-bit read of REG_SIOCNT through a `vu32 *`; the byte
 * access `((vu8 *)sio)[1] &= ~0x40` for the high half; `REG_TM3CNT = 0xc963`
 * with gcc deriving 0x400010c from REG_IF by itself (`sub r2, #0xf6`); the
 * `(s << 26) >> 30` bit extraction; and the three `|=` flag bits in the tail.
 *
 * NEXT: nothing found in five probes.
 */
#include "gba/types.h"
#include "gba/io.h"

extern unsigned char ewram_2002240[];

int Func_8005fcc(void)
{
    unsigned char *p;
    vu32 *sio;
    unsigned int s;
    unsigned int v;
    int r;
    int t;

    p = ewram_2002240;
    sio = (vu32 *)&REG_SIOCNT;
    s = *sio;
    if (p[1] == 0) {
        t = s & 0x88;
        if (t == 8) {
            v = (unsigned char)(s & 4);
            if (v == 0 && *(int *)(p + 0x14) == -1) {
                int one = 1;
                REG_IME = v;
                REG_IE = (REG_IE & ~0x80) | 0x40;
                REG_IME = one;
                ((vu8 *)sio)[1] &= ~0x40;
                REG_IF = 0xc0;
                REG_TM3CNT = 0xc963;
                p[0] = t;
            }
            p[1] = 1;
        }
        p[0xb]++;
    }
    r = (p[2] << 8) | p[3];
    if (p[0] == 8)
        r |= 0x80;
    if (p[9] != 0)
        r |= 0x80 << 5;
    if (((s << 26) >> 30) > 1)
        r |= 0x80 << 6;
    return r;
}
