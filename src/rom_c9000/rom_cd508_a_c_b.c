/* Func_80cdb24  --  0x080cdb24, split out of asm/rom_c9000/rom_cd508_a_c.s;
 * Func_80cd52c stays in _a.s, AnimStart/AnimStart2/AnimEnd in _c.s. Matched
 * from scratch.
 *
 * - A store through a `volatile short *` keeps the address as base + offset:
 *   loop.c cannot strength-reduce `base + k` into a stepped pointer, giving
 *   the ROM's `add r2,r5,r8 / strh` with k stepped separately (69 -> 3).
 * - A step written in the `for` increment clause lands AFTER loop.c's
 *   inserted strength-reduced adds; the same step at the end of the body lands
 *   before them. That closed the last 3.
 */
extern void AnimStart(int n);

void Func_80cdb24(int n)
{
    int i, j, k;
    short v;

    AnimStart(n);
    *(volatile unsigned short *)0x400000c = n | 0x6784;
    k = 0;
    for (i = 0; i != 16; i++) {
        for (j = 0; j != 8; j++, k += 2) {
            int t = (i * 8 + j) * 2;
            v = ((t + 1) << 8) | t;
            *(volatile short *)(0x6003800 + k) = v;
        }
        for (j = 0; j != 8; j++) {
            *(volatile short *)(0x6003800 + k) = 0;
            k += 2;
        }
    }
}
