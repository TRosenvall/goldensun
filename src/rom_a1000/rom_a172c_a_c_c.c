/* Cluster Func_80a17c4..Func_80a17c4 extracted from goldensun/asm/rom_a1000/rom_a172c_a_c_c.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched.
 *
 * Resets a sprite's animation state: sets a flag byte, copies a 9-bit field
 * from +6 into the low bits of the halfword at +0x16, copies the byte at +8 to
 * +0x14, and clears bit fields in the bytes at +0x17 and +0x15.
 *
 * EVERY MASK IS THE DESTINATION OF ITS OWN AND, which is what keeps the ROM's
 * operand order:
 *
 *      m = 0x1ff;  m &= h;         ->  ldr r2, =0x1ff / and r2, r3
 *      x = 0x3f;   x = -x; x &= w; ->  mov r3, #0x3f / neg r3, r3 / and r3, r2
 *
 * The two negated masks are built as `K; -K` rather than written as `-K`, which
 * is what gives the separate `mov` and `neg`; a negative literal is folded into
 * a single pooled constant.
 *
 * The read of +0x17 happens BEFORE the store to +0x14, matching the ROM's
 * interleave (`ldrh r3, [r0, #8] / ldrb r2, [r0, #0x17] / strb r3, [r0, #0x14]`).
 * Reading it after moves the load.
 */
void Func_80a17c4(unsigned char *a)
{
    int t;
    int m;
    int n;
    int h;
    int h2;
    int v;
    int w;
    int x;
    int y;
    int z;

    if (a == 0)
        return;
    t = 1;
    a[5] = t;
    m = 0x1ff;
    h = *(unsigned short *)(a + 6);
    h2 = *(unsigned short *)(a + 0x16);
    m &= h;
    n = 0xfffffe00;
    n &= h2;
    n |= m;
    *(unsigned short *)(a + 0x16) = n;
    v = *(unsigned short *)(a + 8);
    w = a[0x17];
    a[0x14] = v;
    x = 0x3f;
    x = -x;
    x &= w;
    a[0x17] = x;
    y = a[0x15];
    z = 4;
    z = -z;
    z &= y;
    a[0x15] = z;
}
