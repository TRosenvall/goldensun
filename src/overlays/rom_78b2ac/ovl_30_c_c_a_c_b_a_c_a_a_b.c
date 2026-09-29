/* OvlFunc_890_2008d9c (0x02008d9c) -- 158 encodings, 348 bytes, exact.
 *
 * FAKEMATCH: the PIN3 macro below expands at two sites (six register pins).  The
 * pins are the only remedy found for the `__Func_8012330(0x10000, 0x10000,
 * 0x10000)` call family, and the elevated file-mate uses them for the same
 * callee.  Without them: 344 bytes against 348, 32 of 158.
 *
 * Why naming the arguments cannot replace the pins -- this is the hard boundary
 * of the constant-splitting lever: three separately named `int` locals produce
 * IDENTICAL output, no split at all, because precompute_register_parameters
 * (calls.c:850) has already reduced the three identical CONST_INTs to a single
 * pseudo before allocation ever runs.  The `(-1, -1, 0xe666)` call is the same
 * shape at 16 of 158.
 *
 * The switch arms each need their OWN block-scoped stack-argument locals.  Bare
 * literals are 6, one shared pair across the arms is 8, per-arm is exact --
 * which corrects the note in the elevated file-mate
 * src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_a_b.c, whose "the four
 * __CopyMapTiles calls share two named locals" is right for straight-line code
 * and wrong across switch arms.  Neither the shared pair's declaration order nor
 * its assignment order moves it (both 11).
 *
 * `.L2de4` is 4 bytes of bss read and written only as a halfword, so declaring
 * it `unsigned short` is what gives the ROM's `ldrh`/`strh`.
 */
extern unsigned short L2de4 __asm__(".L2de4");
extern int L2de8 __asm__(".L2de8");

extern int __Random(void);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int x, int y, int z);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_890_2008d9c(void)
{
    int s;

    if ((__Random() & 3) != 0) {
        s = L2de4;
        switch (s) {
        case 0:
            __PlaySound(0xbb);
            { int e = 1; int f = 5; __CopyMapTiles(0x2e, 0x3b, 0x1e, 0x21, e, f); }
            break;
        case 1:
            __CopyMapTiles(0x2e, 0x3b, 0x1e, 0x21, s, s);
            __CopyMapTiles(0x2e, 0x3b, 0x1e, 0x22, s, 5);
            break;
        case 2:
            s = 1;
            __CopyMapTiles(0x2e, 0x3b, 0x1e, 0x22, s, s);
            __CopyMapTiles(0x2e, 0x3b, 0x1e, 0x23, s, 5);
            break;
        case 3:
            s = 1;
            __CopyMapTiles(0x2e, 0x3b, 0x1e, 0x23, s, s);
            __CopyMapTiles(0x2e, 0x3b, 0x1e, 0x24, s, 5);
            break;
        case 4:
            { int v = 2; L2de8 = v; }
            s = 1;
            __CopyMapTiles(0x2e, 0x3b, 0x1e, 0x24, s, s);
            __CopyMapTiles(0x2e, 0x3b, 0x1e, 0x25, s, 5);
            break;
        case 0x50:
            { int e = 1; int f = 0xa; __CopyMapTiles(0x2e, 0x31, 0x1e, 0x21, e, f); }
            break;
        }
        L2de4 = L2de4 + 1;
        s = L2de4;
        if (s > ((unsigned int)(__Random() * 0x28) >> 16) + 0x5a)
            L2de4 = 0;
    }
    if (L2de8 != 0) {
        if (L2de8 == 2)
            { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80;
              q0 <<= 9; q1 <<= 9; q2 <<= 9;
              __Func_8012330(q0, q1, q2); }
        else if (L2de8 == 1)
            { PIN3; q0 = 1; q1 = 1; q0 = -q0; q1 = -q1; q2 = 0xe666;
              __Func_8012330(q0, q1, q2); }
        L2de8 = L2de8 - 1;
    }
}
