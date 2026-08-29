/* Func_80167e0 (ScrollTextBuffer) -- NON-MATCHING.
 * Blocker class: REGISTER PRESSURE -- the ROM SPILLS a value this source keeps
 * in a register. 59 lines against the ROM's 60, 46 differing.
 *
 * The structure is right: the shift chain (lines*3, then *2 and *8), the DMA
 * per row, the indirect call, the four pointers advancing by 0x80, and the
 * countdown from 0x1d all reproduce. What differs is allocation.
 *
 * The ROM opens `sub sp, #8` and keeps the 0x6002500 cursor ON THE STACK,
 * reloading and restoring it every iteration. gcc has a register free -- it
 * puts the cursor in r10 -- so our function needs no frame and every later
 * register assignment shifts.
 *
 * Both hold eight values across the loop, and both save r5-r7 and r8-r11, so
 * the difference is not the number of live values but that gcc found one more
 * register than the ROM's compiler did. The likely cause is the control word:
 * the ROM recomputes `orr r2, r4` from two registers INSIDE the loop every
 * iteration, where the OR is loop-invariant and gcc hoists it -- freeing a
 * register and removing the need to spill.
 *
 * That is the inverse of the usual problem. Normally the source is not
 * carrying enough values and the stream comes out SHORT; here the source
 * carries the right values and gcc is simply better at placing them.
 *
 * Tried: the control word and count as separate locals ORed at the call site,
 * which is the ROM's exact shape. gcc hoists it anyway, since nothing in the
 * loop changes either operand.
 *
 * SOLVED and worth keeping: the indirect call. `fp = Func_80008d8; fp(...)`
 * reproduces the ROM's `ldr r3, =Func_80008d8 / bl _call_via_r3` -- see
 * docs/elevation.md, "Indirect calls: _call_via_r3 is a solved shape".
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern void Func_80008d8(void *p, int n, int v);

void Func_80167e0(int lines)
{
    char *src;
    char *dst;
    char *cur;
    int n3, n6;
    int stride, cnt, ctl, off;
    int i;
    void (*fp)(void *, int, int);

    n3 = lines * 3;
    dst = (char *)0x6002520;
    n6 = n3 * 2;
    stride = n3 * 8;
    src = (char *)0x6002520 + stride;
    cur = (char *)0x6002500;
    cnt = 0x18 - n6;
    ctl = 0x84000000;
    off = (0x20 - n6) * 4;
    i = 0x1d;
    do {
        DMA3_SET(src, dst, ctl | cnt);
        fp = Func_80008d8;
        fp(cur + off, stride, 0);
        i--;
        cur += 0x80;
        dst += 0x80;
        src += 0x80;
    } while (i >= 0);
}
