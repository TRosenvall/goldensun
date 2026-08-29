/* Func_808c44c  --  0x0808c44c
 *
 * Cut out of goldensun/asm/rom_8a000/rom_8ba38_a_a.s, which holds eleven
 * functions.
 *
 * A mode-3 hook: when the game state block says mode 3, clear three flags in the
 * sound block if its pending byte is set, mark the active window dirty, and run
 * the redraw.
 *
 * Two things fall out of ordinary C and are worth noting because they look like
 * levers and are not:
 *
 *   The two adjacent flag offsets 0x53a and 0x53b come out as
 *   `ldr r1, =0x53a / add r3, r0, r1 / add r1, #1 / add r3, r0, r1` -- gcc
 *   derives the second from the first. Plain array subscripts give that.
 *
 *   The pending byte at 0x53d is read as a SIGNED char (`ldrsb`) and written
 *   back as part of the same block, so it is a `signed char *` local; the other
 *   three are plain unsigned byte writes.
 *
 * Matched on the first screen.
 */
extern unsigned char *galloc_ewram(int slot, int size);
extern void _Func_8011590(void);

void Func_808c44c(void)
{
    unsigned char *a;
    unsigned char *b;
    signed char *p;
    unsigned char *q;

    a = galloc_ewram(0x1b, 0xccc);
    if (*(short *)(a + (0xcf << 1)) == 3) {
        b = galloc_ewram(0x1f, 0xa8 << 3);
        if (b != 0) {
            p = (signed char *)(b + 0x53d);
            if (*p != 0) {
                b[0x53a] = 0;
                b[0x53b] = 0;
                b[0x53c] = 1;
                *p = 0;
            }
        }
        q = *(unsigned char **)(a + (0xf0 << 1));
        q[0x5b] = 1;
        _Func_8011590();
    }
}
