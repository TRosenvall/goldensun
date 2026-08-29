/* Cluster Func_80f7db4..Func_80f7db4 extracted from goldensun/asm/rom_f6000/rom_f6008_c.s.
 *
 * Total .text for this TU = 58 bytes (= 0x3a).
 * The .s is split in two around this function: the earlier functions move to
 * rom_f6008_c_a.s and the later ones stay in rom_f6008_c.s, which also keeps
 * the .rodata section and its own line in goldensun/stage1.ld.
 *
 * Initialises a dictionary structure in EWRAM: a 0x400-entry table of 12-byte
 * records, each seeded with its own index at +4 and zero at +0, followed by a
 * 0x100-word hash-head table cleared to zero.
 *
 * The first loop stores the INDEX BEFORE INCREMENTING IT and the zero after,
 * matching the ROM's `str r2, [r3, #4] / add r2, #1 / str r1, [r3]`. Writing
 * the two stores in the other order, or incrementing first, moves the add.
 *
 * The second loop walks with a post-increment store (`*(int *)c = y; c += 4;`)
 * which is what produces `stmia r3!, {r1}`, and counts DOWN from 0xff with the
 * decrement before the store, matching `sub r2, #1 / stmia`.
 */
extern unsigned char *ewram_2004c00;

void Func_80f7db4(void)
{
    unsigned char *b;
    unsigned char *c;
    unsigned int k;
    int lim;
    int i;
    int z;
    int n;
    int y;

    b = ewram_2004c00;
    lim = 0x3ff;
    i = 0;
    z = 0;
    b += 4;
    do {
        *(int *)(b + 4) = i;
        i++;
        *(int *)b = z;
        b += 0xc;
    } while (i <= lim);
    c = ewram_2004c00;
    k = 0xc0 << 6;
    c += k;
    y = 0;
    n = 0xff;
    do {
        n--;
        *(int *)c = y;
        c += 4;
    } while (n >= 0);
}
