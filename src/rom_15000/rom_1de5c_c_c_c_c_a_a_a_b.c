/* Cluster Func_801ee68..Func_801ee68 extracted from goldensun/asm/rom_15000/rom_1de5c_c_c_c_c_a_a_a.s.
 *
 * Total .text for this TU = 58 bytes (= 0x3a).
 * Slotted between the _a and _c pieces in goldensun/stage1.ld.
 *
 * Fills a rectangle of halfwords in VRAM at 0x6002000 with a value passed on
 * the stack.
 *
 * THE FIRST TWO PARAMETERS ARE UNUSED and are declared anyway. The ROM
 * overwrites r0 with the VRAM address immediately and never reads r1, but the
 * fifth argument is loaded from `[sp, #0xc]`, which only lands there if four
 * register arguments precede it. Dropping the unused pair would move the stack
 * argument and the load offset with it.
 *
 * The row stride is computed once outside the loop as `(0x20 - w) * 2` and
 * added to the pointer at the end of each row, matching the ROM's hoisted
 * `mov r3, #0x20 / sub r3, r2 / lsl r3, #1`.
 *
 * Both loop bounds are UNSIGNED -- the ROM uses `bcs`/`bcc`, not `bge`/`blt`.
 * The inner loop is guarded by its own `if (x < w)` before the do/while so the
 * zero-width case skips it, which is what the ROM's `cmp r1, r2 / bcs` at the
 * head of each row does.
 */
void Func_801ee68(int a, int b, unsigned int w, unsigned int h, int val)
{
    unsigned short *p;
    unsigned int y;
    unsigned int x;
    int stride;

    p = (unsigned short *)0x6002000;
    y = 0;
    if (y >= h)
        return;
    stride = 0x20;
    stride -= w;
    stride <<= 1;
    do {
        x = 0;
        if (x < w) {
            do {
                x++;
                *p = val;
                p++;
            } while (x < w);
        }
        y++;
        p = (unsigned short *)((unsigned char *)p + stride);
    } while (y < h);
}
