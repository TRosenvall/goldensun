/* Func_801ce48  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_15000/rom_1ca1c_a_a.s
 * Best screen: 6 instructions in disagreeing regions, of 16 (rom 16, ours 15).
 *
 * BLOCKER CLASS: an elided copy -- the same floor as the Func_80bf* family, see
 * src/non_matching/rom_b5000/80bf3bc.c.
 *
 *      rom   ldrh r2, [r0] / mov r3, r2 / cmp r3, #0
 *      ours  ldrh r3, [r0] /             cmp r3, #0
 *
 * Because the copy is gone, our decrement is the two-operand `add r3, r2` where
 * the ROM has the three-operand `add r3, r2, r1` -- it still holds the loaded
 * value separately in r2. One elision, and everything downstream inherits it.
 *
 * `t = v;` as its own statement is exactly the copy the ROM shows and does not
 * produce it, which is the same result as all four Func_80bf* siblings.
 *
 * The rest is right: the pointer add is destructive (`p += off`, matching
 * `add r0, r1`), and the decrement is `v + 0xffff` rather than `v - 1` --
 * writing the subtract emits `sub` instead of the ROM's pooled `ldr r1, =0xffff
 * / add`.
 */
void Func_801ce48(void *arg)
{
    unsigned char *p;
    unsigned int off;
    int v;
    int t;

    p = (unsigned char *)arg;
    off = 0x574;
    p += off;
    v = *(unsigned short *)p;
    t = v;
    if (t == 0)
        t = 2;
    else
        t = v + 0xffff;
    *(unsigned short *)p = t;
}
