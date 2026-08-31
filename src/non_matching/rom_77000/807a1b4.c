/* GiveDjinni -- 0x0807a1b4  (asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_a.s)
 *
 * BLOCKER: ONE instruction -- a register copy before a compare. The 27 of 35
 * in the screen is the SHIFT from that single missing instruction, not 27
 * independent problems; every line after it is correct and displaced by one.
 *
 *     rom   ldrb r4, [r0, r6] / mov r3, r4 / cmp r3, #0x9 / bhi
 *     ours  ldrb r4, [r0, r6] /             cmp r4, #0x9 / bhi
 *
 * SOLVED ON THE WAY: the branch was `bgt` until the count local was declared
 * `unsigned int`. `u[off]` through an `unsigned char *` promotes to int and gcc
 * is free to compare either way; it picks signed unless the destination type
 * says otherwise. That is worth having on its own -- a ROM `bhi`/`bls` on a
 * value loaded by `ldrb` is a tell about the LOCAL's type, not about the load.
 *
 * The copy is not reachable. Measured, all identical at 27 differing and 34
 * lines:
 *     a second local assigned from the first and compared instead
 *     the count declared `unsigned char` rather than `unsigned int`
 *     an `unsigned char` local feeding an `int` one, each used at one site
 *     `n >= 10` instead of `n > 9`
 *
 * gcc coalesces every extra name, which is what the copy tell in
 * docs/elevation.md predicts for a copy whose two halves never diverge: here
 * r3 is written, compared, and never read again, so no C local can be forced
 * to occupy it.
 */
extern void *GetUnit(int id);

int GiveDjinni(int id, int elem, int bit)
{
    unsigned char *u;
    int off;
    int idx;
    unsigned int n;
    int m;

    u = (unsigned char *)GetUnit(id);
    off = elem + (0x8c << 1);
    n = u[off];
    if (n > 9)
        return -1;
    idx = (elem << 2) + 0xf8;
    m = 1 << bit;
    if ((*(int *)(u + idx) & m) != 0)
        return -1;
    u[off] = n + 1;
    *(int *)(u + idx) |= m;
    return 0;
}
