/* Cluster Func_801f680..Func_801f680 extracted from goldensun/asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_a.s.
 *
 * Total .text for this TU = 132 bytes (= 0x84).
 * Preserves the original ROM layout when slotted between
 * asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_a_b.o and asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_b.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 272. EXACT ON THE FIRST SCREEN, no iteration, no
 * pins, no flags.
 *
 * A frame-count-to-"HHH:MM" formatter: divide by 3600, clamp to 59999, split into
 * 60ths, print each with PrintNum. The `+ 100` on the minutes is the leading-zero
 * trick -- PrintNum is asked for 2 digits of a 3-digit number, so the hundreds
 * digit is discarded and a value below 10 still prints as "0N".
 *
 * THE FILE-MATE HEURISTIC PAID DIRECTLY, which is why this took one screen.
 * src/rom_15000/rom_1de5c_c_c_a_b.c, _c_b.c and _c_c_b.c are three already-elevated
 * PrintNum wrappers in the same rom_1de5c family, and they supplied the prototype
 * spelling `char *PrintNum(char *dest, int num, unsigned int width)` and the
 * `char buf[N]` / `char *ret` idiom verbatim.
 *
 * The one thing NOT transferred was the buffer size: the siblings use `buf[16]` and
 * this ROM's `sub sp, #0x40` says `buf[0x40]`. That is the recorded transfer trap --
 * take the idiom, re-derive anything the listing pins down.
 */
char *PrintNum(char *dest, int num, unsigned int width);

char *Func_801f680(unsigned int value, char *out)
{
    char buf[0x40];
    char *s;
    char *d;
    unsigned int t;
    unsigned int h;
    unsigned int m;

    t = value / 3600;
    if (t > 0xea5f)
        t = 0xea5f;
    h = t / 60;
    m = t % 60;
    s = PrintNum(buf, h, 3);
    d = out;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = ':';
    s = PrintNum(buf, m + 100, 2);
    *d++ = *s++;
    *d++ = *s++;
    *d = 0;
    return out;
}
