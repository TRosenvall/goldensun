/*
 * Func_8093168  (ShowMessageAtXY) -- asm/rom_8a000/rom_92950_c_c_c_a.s
 *
 * BLOCKER: comparison canonicalisation, and it is probably NOT REACHABLE.
 * 57 lines against 57. Four differing, and they are one compare:
 *
 *      rom   cmp r0, #0x8 / bge
 *      ours  cmp r0, #0x7 / bgt
 *
 * Semantically identical. gcc-2.96 canonicalises `x >= 8` to `x > 7` when it
 * inverts a branch, and every spelling of the clamp reaches the same place.
 *
 * MEASURED CORPUS-WIDE, which is what makes this a conclusion instead of a
 * shrug. Across all 3205 generated `.s` files in the tree, the sequence
 * `cmp rN, #<nonzero> / bge` appears exactly ONCE --
 * asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_b.s -- and that instance is
 * SWITCH DISPATCH (`cmp r3, #11 / bgt` then `cmp r3, #10 / bge`), not a
 * comparison expression. Its C is a plain `switch` on an area id.
 *
 * So the compiler does emit this shape, but only from switch lowering. From an
 * `if`, 3204 files produce zero instances. Nobody should spend another round
 * guessing at if-spellings for this.
 *
 * TRIED AND REJECTED, all measured, all landing on the same two instructions:
 *
 *   * `if (x <= 7)` instead of `if (x < 8)`      -- identical output
 *   * `if (!(x >= 8))`                            -- identical output
 *   * `if (x >= 8) goto skip; v = 8; skip:`       -- identical output
 *   * `if (x < 8) v = 8; else v = x;` (dropping the earlier `v = x`)
 *                                                 -- WORSE, 13 differing
 *
 * SETTLED, and it is what took this from 56 differing to 4:
 *
 *   The parameters are copied into LOCALS before use, and the ROM shows all
 *   three copies: `mov r0, r2` (x), `mov r2, r3` (y), `mov r1, r0` (v = x).
 *   Using the parameters directly makes gcc compare the incoming argument
 *   register and saves a move, leaving the function one instruction SHORT.
 *   The load of iwram_3001ebc sits between the first and second copy, so the
 *   source order is: x = a; p = iwram_3001ebc; y = b; v = x;
 */
extern unsigned char *iwram_3001ebc;
extern int _Func_8017658(int id, int a, int b, int c);
extern int _Func_8017394(int h);
extern void WaitFrames(int n);

void Func_8093168(int p0, int p1, int a, int b)
{
    unsigned char *p;
    short *c;
    int v;
    int h;
    int x;
    int y;

    x = a;
    p = iwram_3001ebc;
    y = b;
    v = x;
    if (y > 0x77)
        y += 0x20;
    else
        y -= 0x20;
    if (x < 8)
        v = 8;
    if (v > (0x9c << 1))
        v = 0x9c << 1;
    if (y < 0x14)
        y = 0x14;
    if (y > 0xdc)
        y = 0xdc;
    h = _Func_8017658(*(short *)(p + (0xec << 1)), v, y, 1);
    while (_Func_8017394(h) == 0)
        WaitFrames(1);
    c = (short *)(p + (0xec << 1));
    (*c)++;
}
