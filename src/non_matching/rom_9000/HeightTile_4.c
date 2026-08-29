/* HeightTile_4  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_9000/rom_11ce0_a_c_c_a_a_a.s
 * Best screen: 8 instructions in disagreeing regions, of 28 (rom 28, ours 25).
 *
 * BLOCKER CLASS: an elided register save.
 *
 * The ROM copies the third argument out of r2 (`mov r5, r2`) BEFORE reusing r2
 * to hold the running maximum. gcc puts the maximum in r3 instead, so no save
 * is needed and three instructions vanish.
 *
 * The source below spells the copy out -- `c = b;` before `r = v0;` -- and gcc
 * elides it, because a copy of a value that is still live is free to skip when
 * the register is not being reused. The ROM's save exists only as a consequence
 * of its own register choice, which C does not state.
 *
 * WHAT IS ALREADY RIGHT: the two `ldrsb` reads use named zero and one offsets,
 * which the ISA requires; the `<< 19` scaling is on the sign-extended byte; and
 * the three-way tail (`e == 0xf` returns the maximum, `e > 0xe` returns the
 * second height, otherwise the first) matches the ROM's branch structure.
 */
int HeightTile_4(signed char *p, int a, int b)
{
    unsigned int o;
    int h;
    int v0;
    int v1;
    int c;
    int r;
    int d;
    int e;

    o = 0;
    h = *(signed char *)(p + o);
    v0 = h << 19;
    o = 1;
    h = *(signed char *)(p + o);
    v1 = h << 19;
    c = b;
    r = v0;
    if (v1 > v0)
        r = v1;
    d = c - a;
    e = d;
    e += 0xf;
    if (e == 0xf)
        return r;
    if ((unsigned int)e > 0xe)
        return v1;
    return v0;
}
