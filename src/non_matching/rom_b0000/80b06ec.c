/* Func_80b06ec -- asm/rom_b0000/rom_b0070_a_a_c_a_c_c_c_a.s
 *
 * SCREENED ONCE, NOT PURSUED. 32 of 42, and OURS IS FOUR LINES SHORT (38).
 * Recorded at the depth actually reached -- one screen -- so the next round
 * knows what was and was not tried.
 *
 * Four groups of four bytes copied into a tile layout at offsets 0, 1, 0x1e,
 * 0x1f, each group abandoned at the first zero byte, four iterations counted
 * down with `bge`. The nested-if reading of the `beq` chain is almost
 * certainly right -- every `beq` targets the LOOP TAIL, not the exit, so a
 * zero skips the rest of the group and still advances the destination.
 *
 * WHY IT IS SHORT, and it is the thing to start from next time: the ROM emits
 *
 *     ldrb r2, [r0] / mov r3, r2 / cmp r3, #0
 *
 * at all four tests -- a redundant copy into r3 before the compare, four times,
 * which is exactly the four missing lines. gcc compares the loaded byte
 * directly. That `mov` is the signature of the value being read into one
 * variable and TESTED THROUGH ANOTHER, or of a char/int width change at the
 * test. Getting those four `mov`s to appear is the whole problem; the rest of
 * the body is close.
 *
 * Not parked as a studied blocker -- one spelling was tried.
 */
extern unsigned char Lb3d40[] __asm__(".Lb3d40");
extern unsigned short Lb413c[] __asm__(".Lb413c");

void Func_80b06ec(int a, int b, int c)
{
    unsigned char *src;
    unsigned char *dst;
    int i;
    int v;

    src = Lb3d40 + a * 32;
    dst = (unsigned char *)(b + Lb413c[c] + 2);
    i = 3;
    do {
        v = *src;
        if (v != 0) {
            dst[0] = v;
            src++;
            v = *src;
            if (v != 0) {
                dst[1] = v;
                src++;
                v = *src;
                if (v != 0) {
                    dst[0x1e] = v;
                    src++;
                    v = *src;
                    if (v != 0) {
                        dst[0x1f] = v;
                        src++;
                    }
                }
            }
        }
        i--;
        dst += 4;
    } while (i >= 0);
}
