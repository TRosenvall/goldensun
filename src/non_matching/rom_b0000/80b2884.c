/* Func_80b2884  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b0000/rom_b0070_c_c_a_a.s
 * Best screen: 18 instructions in disagreeing regions, of 30 (rom 30, ours 22).
 *
 * BLOCKER CLASS: the pool tell, in a form worth recognising -- an UNFOLDED
 * SUBTRACTION OF TWO POOLED CONSTANTS.
 *
 * Three times the ROM does this:
 *
 *      ldr r3, =0xd2e / ldr r2, =0xd24 / sub r3, r2 / add r0, r3
 *
 * Four instructions to add ten. Written as literals, gcc folds the difference
 * at compile time and emits `add r0, #0xa` -- one instruction, which is why our
 * stream is eight shorter. **A compiler does not leave `0xd2e - 0xd24`
 * unfolded. Both operands were symbols.**
 *
 * This is the same shape as the per-area flag run in
 * src/non_matching/ovl_7ced6c/2009494.c, where `0x8c8 - (int)(&_AREA_7e)`
 * reproduced the ROM exactly once the symbol existed. The mechanism is
 * identical; only the namespace is unknown here.
 *
 * WHAT IS NEEDED: names for 0xd24, 0xd2e, 0xd38 and 0xd42. None is defined in
 * any of the four .sym files. They are not area ids -- the selector reads a
 * signed byte at iwram_3001f2c+0x3aa, not gState+0x1C0 -- and 0xd24 is used as
 * a BASE that the other three are measured from, which is what a table of
 * consecutive ids looks like.
 *
 * NOT NAMED HERE. Identifying the namespace needs a consumer, and this function
 * only adds the result to its argument and returns it; the caller is where the
 * evidence would be. Guessing from the values is exactly what
 * tools/sym_candidates.py refuses to do, and 95 small values already collide
 * across the existing namespaces.
 */
extern unsigned char *iwram_3001f2c;

int Func_80b2884(int base)
{
    unsigned char *p;
    unsigned int o;
    int k;
    int d;

    o = 0x3aa;
    p = iwram_3001f2c + o;
    k = *(signed char *)(p + (unsigned int)0);
    if (k == 1) {
        d = 0xd2e - 0xd24;
        base += d;
    }
    if (k == 2) {
        d = 0xd38 - 0xd24;
        base += d;
    }
    if (k == 3) {
        d = 0xd42 - 0xd24;
        base += d;
    }
    return base;
}
