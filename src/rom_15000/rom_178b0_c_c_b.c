/* Func_8017c8c  --  0x08017c8c, split out of asm/rom_15000/rom_178b0_c.s.
 * Matched from scratch; its jump table is inline in .text.
 *
 * - Func_8018efc must be declared returning `int`: with `void` the argument
 *   moves come out permuted (2 encodings). A value-returning call sets r0, so
 *   the post-call write to r0 depends on the call rather than on the pre-call
 *   `mov r0,rX`, which changes that move's dependent count and so the
 *   rank_for_schedule tie. When argument moves are permuted with the SAME
 *   registers, try the callee's return type.
 * - `for (;;) { v = *s++; if (v << 16 == 0) break; ch = v; ... }` with v an
 *   `int` gives the ROM's split zero-extension (shift in the exit test, the
 *   right shift at the top of the body); tests on the u16 value do not.
 */
extern unsigned char *iwram_3001e8c;
extern int Func_8018efc(int a, int ch, int b, int c, int e);

void Func_8017c8c(short *s, int a, int b, int c)
{
    unsigned char *base = iwram_3001e8c;
    short b0 = b;
    unsigned short *q;
    unsigned short ch;
    int v;
    int off;

    if (s == 0) {
        q = (unsigned short *)(base + 0x12b2);
        s = (short *)(base + 0xeb0);
        off = (*q << 1) + 0xeb0;
        *(unsigned short *)(base + off) = 0;
        *q = (*q + 1) & 0x1ff;
    }
    for (;;) {
        v = *s++;
        if (v << 16 == 0)
            break;
        ch = v;
        if (ch <= 0x1e) {
            switch (ch) {
            case 3:
                b = b0;
                c++;
                break;
            case 14: case 15: case 28:
                s++;
            case 7: case 8: case 9: case 10: case 11: case 12: case 17: case 29:
                s++;
                break;
            }
        } else {
            Func_8018efc(a, ch, b, c, 0);
            if ((unsigned short)(ch - 0xde) > 1)
                b++;
        }
    }
    base[0xea3] = 1;
}
