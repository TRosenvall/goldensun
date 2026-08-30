/* Cluster Func_8097a7c..Func_8097a7c -- the whole of
 * goldensun/asm/rom_8a000/rom_97384_c_c_c_a.s, confirmed data-free by
 * split_s.py, so no split was needed.
 *
 * Total .text for this TU = 96 bytes.
 *
 * TWO LEVERS, and the second is the general one.
 *
 * `volatile` ON THE STORE POINTER STOPS THE FINAL-INCREMENT FOLD. The previous
 * park concluded "nothing in the source keeps a pointer live past its last
 * use, so the fold cannot be refused". A `volatile unsigned short *` gives the
 * ROM's `add r3, #2 / strh r2, [r3]` for the last store instead of folding it
 * to `strh r2, [r3, #2]`, and fixed a scheduling difference for free.
 *
 * THE FILL VALUE MUST BE A BARE LITERAL, NOT AN int LOCAL. This is the same
 * operand-mode lever that decides pool placement elsewhere: `int v = 0x739c`
 * makes an SImode pool entry, whose range is 1020 bytes, so the whole pool
 * moves past the epilogue. The literal written into the halfword store is
 * HImode, range 64, which puts 0x739c FIRST in the pool, forces the mid-body
 * dump, and makes the `b` over the pool appear. The resulting pool word order
 * is not reference order, and matching it is what confirms the shape.
 */
extern char *iwram_3001e8c;
extern void Func_8097868(void);
extern void StartTask(void (*f)(void), int n);

void Func_8097a7c(void)
{
    char *p;
    char *d;
    volatile unsigned short *q;
    int off;
    int n;

    p = iwram_3001e8c;
    off = 0xea4;
    d = p + off;
    off = 1;
    *d = off;
    q = (volatile unsigned short *)0x50001e2;
    *q = 0x739c;
    q += 2;
    *q++ = 0x739c; *q++ = 0x739c; *q++ = 0x739c; *q++ = 0x739c; *q++ = 0x739c;
    *q++ = 0x739c; *q++ = 0x739c; *q++ = 0x739c; *q++ = 0x739c; *q++ = 0x739c;
    n = 0x90 << 3;
    StartTask(Func_8097868, n);
}
