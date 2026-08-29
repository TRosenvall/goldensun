/* OvlFunc_common1_16cc  --  asm/overlays/common/common1_a_c_c_c.s
 *
 * BLOCKER CLASS: register birth order.
 * Status: 21 lines against 21, SIX differing, and all six are r4 and r5
 * exchanged throughout.
 *
 * WHAT IT DOES
 * Formats an unsigned int as eight hex digits into a buffer, back to front,
 * NUL-terminating at +8. The digit characters come from the table at .L6.
 *
 * THE DIFFERENCE
 *      rom    ldr r5, =.L6  ...  mov r4, #0xf
 *      ours   ldr r4, =.L6  ...  mov r5, #0xf
 *
 * REG_ALLOC_ORDER reaches r4 before r5, so whichever pseudo is born first takes
 * r4. The ROM gives r4 to the MASK, so its mask was created first; gcc creates
 * the table address first, because the loop optimiser hoists that out of the
 * loop before the mask is ever seen.
 *
 * WHAT WAS TRIED
 *   - the mask assigned at the very top of the function, before the buffer
 *     arithmetic: WORSE, 8 differing lines
 *   - the mask assigned immediately before the counter: also 8
 * Moving it earlier does not help because the competing pseudo is not created
 * by a statement at all -- it is created by loop-invariant hoisting, which
 * happens before any of these orderings matter.
 *
 * SETTLED AND WORTH KEEPING: the buffer pointer is advanced IN PLACE
 * (`buf += 8; *buf = 0;`), not indexed. Written `p = buf + 8; *p = 0;` gcc
 * folds it to `strb r3, [r0, #8]` and the ROM's `add r0, #8` disappears.
 */

extern unsigned char L6[] __asm__(".L6");

void OvlFunc_common1_16cc(char *buf, unsigned int v)
{
    int i;
    int m;

    buf += 8;
    *buf = 0;
    i = 7;
    buf--;
    m = 0xf;
    do {
        *buf = L6[v & m];
        i--;
        v >>= 4;
        buf--;
    } while (i >= 0);
}
