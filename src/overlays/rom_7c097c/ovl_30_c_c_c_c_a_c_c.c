/* OvlFunc_936_200b864 -- 0x0200b864
 *
 * Spawns one debris actor at a randomised offset from the given point, gives it
 * randomised drift on both axes, and hands it its script.
 *
 * FOUR SMALL LEVERS, and the interesting part is that two of them are about
 * WHICH VARIABLE, not which expression.
 *
 *  - NAME THE RANDOM RESULT. The first draw feeds one call argument while the
 *    caller's x is adjusted alongside it; written inline, gcc finishes the whole
 *    argument expression before touching x, where the ROM interleaves the
 *    `ldr =0xfff80000 / add` into the middle of it. Naming the draw and
 *    adjusting x as its own statement moved the first divergence from 8 to 29.
 *
 *  - REUSE THE STORED ZERO FOR THE MASK. The ROM writes `mov r3, #0 / strb` and
 *    then `sub r3, #0xd` -- it builds -13 by subtracting from the zero it just
 *    stored, rather than `mov #0xd / neg`. Writing `v = 0; *p = v; v -= 13;`
 *    reproduces that; a fresh `-13` does not. 46 differing to 14.
 *
 *  - BUT DO NOT REUSE IT AGAIN for the later 0x14 store. That store also needs
 *    a named local (as a cast it POOLS the constant -- the narrow-store table
 *    once more), and reusing the SAME variable overshoots: 31 differing, and
 *    the function comes out a line SHORT. A SECOND, separate local is exact.
 *    So "one variable, two ranges" is a real lever and it is not free -- each
 *    reuse has to be the one the ROM actually made.
 *
 *  - FORM THE ADDRESS BEFORE THE VALUE. The final two lines were `add r2, #0x55`
 *    and `mov r3, #0` transposed; binding the destination to a pointer local
 *    first puts them in the ROM's order.
 *
 * Removing the pooled 0x14 also removed a pool-skip branch, which is why the
 * line count fell into place at the same time -- a pooled constant inside a
 * function costs the `b` over its pool as well as the entry.
 *
 * Verified with tools/objcmp.py: 168 bytes, 74 encodings and 9 relocations
 * identical.
 */
extern unsigned int __Random(void);
extern unsigned char *__CreateActor(int id, int x, int y, int z);
extern void __Func_80929d8(unsigned char *n, int a);
extern void __Actor_SetSpriteFlags(unsigned char *n, int f);
extern void __Actor_SetAnim(unsigned char *n, int a);
extern void __Actor_SetScript(unsigned char *n, void *s);
extern unsigned char gScript_936__0200d120[];

void OvlFunc_936_200b864(int x, int y, int z)
{
    unsigned char *n;
    unsigned int r;
    unsigned char *q;
    unsigned char *p;
    unsigned char *p2;
    int w;
    int v;

    r = __Random();
    x = x - 0x80000;
    n = __CreateActor(0xde, x, ((r * 8 >> 16) << 16) + y + (0x80 << 13), z);
    if (n == 0)
        return;
    p2 = n + 0x55;
    v = 0;
    *p2 = v;
    q = *(unsigned char **)(n + 0x50);
    v -= 13;
    v &= q[9];
    v |= 8;
    q[9] = v;
    __Func_80929d8(n, 9);
    __Actor_SetSpriteFlags(n, 0);
    *(int *)(n + 0x24) = ((__Random() * 2 >> 16) - 1) << 16;
    *(int *)(n + 0x28) = ((__Random() * 6 >> 16) - 3) << 16;
    p = n + 0x64;
    w = 0x14;
    *(short *)p = w;
    p -= 3;
    *p = 1;
    __Actor_SetAnim(n, 1);
    __Actor_SetScript(n, gScript_936__0200d120);
}
