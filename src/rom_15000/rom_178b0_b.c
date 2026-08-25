/* Cluster Func_8017a64..Func_8017a64 extracted from goldensun/asm/rom_15000/rom_178b0.s.
 *
 * Total .text for this TU = 70 bytes (= 0x46).
 * Placed in the run in goldensun/stage1.ld.
 *
 * Measures the pixel width of a NUL-terminated halfword string: a space is 4,
 * anything above 0xff is a flat 10, two control codes at 0xde and 0xdf cost
 * nothing, and every other character takes the halfword at the head of its
 * 32-byte glyph record in Data_32224.
 *
 * THE BLOCK ORDER IS THE WHOLE PUZZLE. Written as the obvious chain --
 *
 *     if (c == 0x20) ... else if (c > 0xff) ... else if (...) table
 *
 * -- gcc puts the cheap `add r1, #0xa` INLINE and branches away to the table
 * case. The ROM does the opposite: it branches away to the `+0xa` block and
 * keeps the table case falling through. Inverting the test to `c <= 0xff` with
 * the table work inside it and `+0xa` in a trailing `else` reproduces the ROM
 * exactly. Every other instruction was already right; this was 12 of 35.
 *
 * That is the guard-inversion lever from batch 64, and the rule of thumb it
 * gives is: whichever arm the ROM lets FALL THROUGH is the arm the source's
 * `if` was true for.
 *
 * TWO THINGS THAT NEEDED NO HELP, worth knowing before trying to force them:
 *
 *   `sub r3, #0xde / cmp r3, #1 / bls` then `add r3, #0xbe` -- gcc derives the
 *   `- 0x20` from the `- 0xde` it already has. Written as two independent
 *   expressions, `(c - 0xde)` and `(c - 0x20)`, the chain appears on its own.
 *
 *   `ldrh r3, [r3, r4]` has the INDEX as the base operand and the table address
 *   as the offset. That falls out of `Data_32224 + (c - 0x20) * 32`; no operand
 *   reordering was needed.
 *
 * NOTE FOR RESCREENING: tryc.py warns that the reference keeps its pool inside
 * the function. Cleared by `make compare`, not by the screen.
 */

extern unsigned char Data_32224[];

int Func_8017a64(unsigned short *s)
{
    int w;
    unsigned int c;

    c = *s;
    w = 0;
    s++;
    while (c != 0) {
        if (c == 0x20) {
            w += 4;
        } else if (c <= 0xff) {
            if ((unsigned int)(c - 0xde) > 1)
                w += *(unsigned short *)(Data_32224 + (c - 0x20) * 32);
        } else {
            w += 0xa;
        }
        c = *s;
        s++;
    }
    return w;
}
