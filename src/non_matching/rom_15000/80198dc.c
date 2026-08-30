/*
 * Func_80198dc (ClearCallbackTable) -- asm/rom_15000/rom_1908c_c_a_c_c_b.s
 *
 * NOTE: this function was SPLIT OUT of a seven-function file this round, so it
 * now has its own .s and is individually elevatable. The split is byte-neutral
 * and make compare is green.
 *
 * BLOCKER: the allocation is shifted one register down, whole. 18 lines against
 * 18, 12 differing, and every difference is the same rotation:
 *
 *      value            rom   ours
 *      iwram base        r3    r2
 *      offset            r4    r3
 *      halfword pointer  r2    r1
 *
 * The ROM starts allocating at r3 and uses r4 (which is call-used here under
 * -fcall-used-r4, so it is free); we start at r2.
 *
 * TRIED AND REJECTED, all measured: the two pool loads in either order (12 and
 * 13 -- putting the offset first is what fixed the ORDER, and is kept below);
 * the counter and zero initialised before the pointer (12); their declarations
 * swapped (13); their assignments swapped (12); the second pointer built
 * DESTRUCTIVELY from the base (`b += off; p = (int *)b;`), which is what the
 * ROM's `add r3, r4` looks like (13).
 *
 * SETTLED: assigning `off = 0x12dc` BEFORE reading the global is what puts both
 * pool loads ahead of the dereference, matching the ROM. And 0x12bc is DERIVED
 * (`sub r4, #0x20`), not a second constant -- the derive-the-offset lever, with
 * the first offset added to a pointer loaded from memory.
 */
extern unsigned char *iwram_3001e8c;

void Func_80198dc(void)
{
    unsigned char *b;
    int *p;
    short *q;
    int off;
    int i;
    int z;

    off = 0x12dc;
    b = iwram_3001e8c;
    q = (short *)(b + off);
    off -= 0x20;
    i = 0;
    z = 0;
    p = (int *)(b + off);
    do {
        i++;
        *p++ = z;
        *q = z;
        q++;
    } while (i != 8);
}
