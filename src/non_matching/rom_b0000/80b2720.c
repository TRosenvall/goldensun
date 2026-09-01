/* Func_80b2720 (0x080b2720) -- NON-MATCHING.
 * Blocker class: SCRATCH-REGISTER SELECTION plus a peeled first load.
 *
 * 32 lines against the ROM's 32, 23 differing. The instruction sequence is
 * right; the base pointer, the destination cursor and the counter sit in a
 * rotated set of registers, and gcc peels the loop's first `ldrh` above the
 * loop where the ROM keeps it inside.
 *
 * MEASURED, both 32 lines and 23 differing:
 *   `*d = *p;` written directly
 *   the loaded value named (`v = *p; ... *d = v;`) to stop the peel
 *
 * WHAT IS RIGHT: the whole shape on the first screen -- the `(id * 32 + id) * 2`
 * record offset into .Lb41ac (the same table Func_80b26cc uses), the
 * base-first `ldrsh` for the initial emptiness test, the OFFSET-first
 * `*(unsigned short *)(n * 2 + (int)dst) = 0` terminator that gives the ROM's
 * `strh r3, [r2, r5]`, the `ldrh` copy against the `ldrsh` sentinel test, and
 * the 0x17 cap with the count returned.
 *
 * NEXT: nothing source-level. One probe past the point where the diff became a
 * register rotation at exact length, per the batch-171 recognition rule.
 */
extern unsigned char Lb41ac[] __asm__(".Lb41ac");

int Func_80b2720(int id, unsigned short *dst)
{
    unsigned char *base;
    unsigned short *p;
    unsigned short *d;
    int off;
    int n;

    off = (id * 32 + id) * 2;
    base = Lb41ac;
    n = 0;
    if (*(short *)(base + off) != 0) {
        d = dst;
        p = (unsigned short *)(off + (int)base);
        do {
            *d = *p;
            n++;
            p++;
            d++;
            if (n > 0x17)
                break;
        } while (*(short *)p != 0);
    }
    *(unsigned short *)(n * 2 + (int)dst) = 0;
    return n;
}
