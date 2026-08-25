/* Func_801b36c  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_15000/rom_1aeec_a_a_c_a_c.s
 * Best screen: 12 instructions in disagreeing regions, of 22 (rom 22, ours 20).
 *
 * BLOCKER CLASS: constant-CSE. The two offsets this function uses are
 * 0xd2 << 2 = 0x348 and 0x39e, which differ by 0x56, and gcc derives the second
 * from the first:
 *
 *      rom   mov r2, #0xd2 / lsl r2, #2 / ldr r4, =0x39e
 *      ours  mov r1, #0xd2 / lsl r1, #2 / ... / add r1, #0x56
 *
 * It then also merges the two `a + 0x39e` adds that the ROM performs
 * separately, which is where the two missing instructions go.
 *
 * Identical in shape and cause to src/non_matching/rom_b5000/80bf574.c
 * (0x146 and 0x147) and src/non_matching/rom_a1000/80ad69c.c (0x219 added
 * twice). The basic-block lever does not apply -- this is global CSE, which
 * runs before local-alloc and ignores block boundaries; that was settled with
 * two independent confirmations in batch 62.
 *
 * TWIN: Func_80b0694 in asm/rom_b0000/rom_b0070_a_a_c_a_c_c_c.s is the same
 * function instruction for instruction. Solving either solves both, so it is
 * not parked separately.
 */
void *Func_801b36c(unsigned char *a)
{
    unsigned int k;
    unsigned int j;
    unsigned char *p;
    unsigned char *r;
    int n;
    int i;

    k = 0xd2 << 2;
    j = 0x39e;
    p = a + k;
    r = *(unsigned char **)p;
    p = a + j;
    n = *(unsigned short *)p;
    i = 0;
    if (n == 0)
        goto done;
    p = a + j;
    n = *(unsigned short *)p;
    do {
        i++;
        r = *(unsigned char **)(r + 4);
    } while (i != n);
done:
    return r;
}
