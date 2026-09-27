/* DecompressSpriteLZ  --  0x0800a97c, split out of asm/rom_9000/rom_a97c.s; UpdateSpriteAnim
 * stays in _c.s. Matched from scratch; the ROM keeps its pool inside the
 * function and objcmp confirms it byte-identical.
 *
 * - POST-INCREMENT ORDER: in `x = *p++ | K` the increment is emitted AFTER the
 *   orr; `x = *p++; x |= K;` emits it before. That flips sched2's source-order
 *   tie-break.
 * - `base = lit;` moved above the first flag load makes the setup and refill
 *   blocks end identically, so jump2 merges their tail -- the ROM's `b` into
 *   the middle of the refill. The two together: 17 differing -> exact.
 */
unsigned char *DecompressSpriteLZ(unsigned char *src, unsigned char *dst)
{
    unsigned char *lit;
    unsigned char *base;
    unsigned char *ret;
    unsigned char *from;
    unsigned int flags;
    int token;
    int len;

    token = src[0] | (src[1] << 8);
    lit = src + 2;
    if (token == 0)
        return lit;
    src += token;
    base = lit;
    ret = dst;
    flags = *src++;
    flags |= 0x100;
    goto next;
copy:
    len = token >> 12;
    if (len == 0)
        len = *src++ + 0x10;
    len += 2;
    from = base - (token & 0xfff);
    while (len != 0) {
        *dst++ = *from++;
        len--;
    }
shift:
    flags >>= 1;
    if (flags == 0) {
        flags = *src++;
        flags |= 0x100;
    }
next:
    if (flags & 1) {
        if (flags == 1)
            goto shift;
        *dst++ = *lit++;
        goto shift;
    }
    token = *src++ << 8;
    token |= *src++;
    if (token != 0)
        goto copy;
    return ret;
}
