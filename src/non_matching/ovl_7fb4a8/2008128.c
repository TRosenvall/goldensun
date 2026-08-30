/*
 * OvlFunc_971_2008128 -- asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: operand order in two register-offset accesses. 9 lines against 9,
 * 7 differing. Both the load and the store use the scaled INDEX as the
 * addressing base and the array as the offset -- `ldr r2, [r1, r2]` and
 * `str r2, [r3, r4]` -- and the pointer-inversion spelling below gets the
 * shape but not the register assignment.
 *
 * The three pool loads (.L1940, the char array, ewram_2002224) are emitted in a
 * different order from the ROM's, which is where the seven come from.
 */
extern unsigned char L1940[] __asm__(".L1940");
extern int CHAR_ARRAY_ARRAY_971__02009928[];
extern unsigned char ewram_2002224[];

void OvlFunc_971_2008128(int i)
{
    int off;
    int k;

    off = i * 4;
    k = L1940[i] << 2;
    *(int *)(k + (int)ewram_2002224) = *(int *)(off + (int)CHAR_ARRAY_ARRAY_971__02009928);
}
