/* PrintNum  --  0x08017dd4, split out of asm/rom_15000/rom_178b0_c.s; the
 * .rodata blob that followed it stays in _c.s. Matched from scratch.
 *
 * Batch 285's loop-shape rules, all three:
 *  - the space scan is `while (...) { if (++i == 12) goto out; }` (or a guard
 *    plus do/while); the `break` spelling does not match;
 *  - the digit loop must be INDEXED, `for (i = 12; i != 0; i--) buf[i] = ...`:
 *    gcc turns it into a pointer loop comparing against a copy of buf in r10,
 *    as the ROM does, and a pointer spelling does not;
 *  - the zero-skip loop needs the `continue` form.
 */
unsigned char *PrintNum(unsigned char *buf, int value, unsigned int width)
{
    int neg = 0;
    unsigned char *p;
    int i;

    if (value < 0) {
        if (width == 0)
            neg = 1;
        value = -value;
    }
    buf[0] = ' ';
    for (i = 12; i != 0; i--) {
        buf[i] = value % 10 + '0';
        value /= 10;
    }
    buf[13] = 0;
    for (i = 1; i != 13; i++) {
        if (buf[i] == '0') {
            if (i != 12)
                buf[i] = ' ';
            continue;
        }
        if (neg)
            buf[i - 1] = '-';
        break;
    }
    if (width == 0) {
        i = 0;
        while (buf[i] == ' ') {
            if (++i == 12)
                goto out;
        }
    out:
        return buf + i;
    }
    if (width > 12)
        width = 12;
    return buf - width + 13;
}
