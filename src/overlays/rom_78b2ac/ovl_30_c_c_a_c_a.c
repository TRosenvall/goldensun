/* OvlFunc_890_2008238  --  0x02008238
 * OvlFunc_890_20082cc  --  0x020082cc
 * OvlFunc_890_2008360  --  0x02008360
 * OvlFunc_890_20083f4  --  0x020083f4
 *
 * Cut from goldensun/asm/overlays/rom_78b2ac/ovl_30_c_c_a_c.s.
 *
 * Four map repaints, each four independent `if (save bit) copy a metatile
 * block`. The four functions differ only in which bit range they read and which
 * two rows they write, so they are one template with sixteen constants.
 *
 * FOUND BY SHAPE (tools/find_shape.py). Solving one gave the other three.
 *
 * THE TWO STACK ARGUMENTS ARE BLOCK-SCOPED LOCALS, and where they are declared
 * is the whole trick. With literals gcc computes and stores each in turn,
 * reusing r3 -- three differing lines per block, twelve in all -- where the ROM
 * builds both before storing either:
 *
 *     rom    mov r3, #2 / mov r2, #1 / str r3, [sp] / str r2, [sp, #4]
 *     ours   mov r3, #2 / str r3, [sp] / mov r3, #1 / str r3, [sp, #4]
 *
 * Declared INSIDE each `if`, they reproduce the ROM exactly. Declared once at
 * the top of the function they are worse than literals -- 56 lines against 61,
 * 53 differing -- because gcc keeps them live across all four calls in
 * callee-saved registers and the ROM rebuilds them per block. Batch 83's
 * stack-argument lever, and the scope is as load-bearing as the name.
 */
extern int __GetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);

void OvlFunc_890_2008238(void)
{
    if (__GetFlag(0x80b)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2d, 0x1c, 0x22, 0xa, e5, e6);
    }
    if (__GetFlag(0x80c)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2f, 0x1c, 0x24, 0xa, e5, e6);
    }
    if (__GetFlag(0x80d)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2d, 0x1d, 0x22, 0xb, e5, e6);
    }
    if (__GetFlag(0x80e)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2f, 0x1d, 0x24, 0xb, e5, e6);
    }
}
void OvlFunc_890_20082cc(void)
{
    if (__GetFlag(0x826)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2d, 0x1c, 0x22, 0xa, e5, e6);
    }
    if (__GetFlag(0x827)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2f, 0x1c, 0x24, 0xa, e5, e6);
    }
    if (__GetFlag(0x828)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2d, 0x1d, 0x22, 0xb, e5, e6);
    }
    if (__GetFlag(0x829)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2f, 0x1d, 0x24, 0xb, e5, e6);
    }
}
void OvlFunc_890_2008360(void)
{
    if (__GetFlag(0x80b)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2d, 0x1e, 0x22, 0xa, e5, e6);
    }
    if (__GetFlag(0x80c)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2f, 0x1e, 0x24, 0xa, e5, e6);
    }
    if (__GetFlag(0x80d)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2d, 0x1f, 0x22, 0xb, e5, e6);
    }
    if (__GetFlag(0x80e)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2f, 0x1f, 0x24, 0xb, e5, e6);
    }
}
void OvlFunc_890_20083f4(void)
{
    if (__GetFlag(0x826)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2d, 0x1e, 0x22, 0xa, e5, e6);
    }
    if (__GetFlag(0x827)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2f, 0x1e, 0x24, 0xa, e5, e6);
    }
    if (__GetFlag(0x828)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2d, 0x1f, 0x22, 0xb, e5, e6);
    }
    if (__GetFlag(0x829)) {
        int e5 = 2;
        int e6 = 1;
        __CopyMapTiles(0x2f, 0x1f, 0x24, 0xb, e5, e6);
    }
}
