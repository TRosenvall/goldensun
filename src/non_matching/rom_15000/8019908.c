/* Func_8019908  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_15000/rom_1908c_c_a_c.s
 * Best screen: 9 instructions in disagreeing regions, of 27 (streams same length).
 *
 * BLOCKER CLASS: register allocation on the two parameter copies.
 *
 * THE ADDRESSING IS SOLVED. The ROM indexes a parallel array with the OFFSET
 * first, `ldrh r3, [r4, r1]`, and the pointer-typed-operand lever reproduces it
 * exactly -- the walking offset is declared `unsigned char *` and the loaded
 * base is a plain `unsigned int`. See docs/elevation.md.
 *
 * What remains is which registers the two arguments land in. The ROM saves the
 * second argument first (`mov r7, r1`) because r1 is about to be reloaded with
 * the base, then the first argument later (`mov r6, r0`). gcc does it in the
 * other order and uses the opposite pair.
 *
 * WHAT WAS TRIED
 *   Explicit local copies of both parameters, assigned in the ROM's order --
 *   the second before the global load, the first after. BYTE-IDENTICAL.
 *   gcc assigns argument registers by its own rule and statement order does not
 *   reach it, which is the same result as src/non_matching/rom_b5000/80be02c.c.
 */
extern unsigned char *iwram_3001e8c;

void Func_8019908(int a, int b)
{
    unsigned int base;
    unsigned char *off;
    unsigned char *p;
    unsigned int k;
    int i;
    int lim;
    int h;

    base = (unsigned int)iwram_3001e8c;
    k = 0x12bc;
    off = (unsigned char *)0x12dc;
    lim = 8;
    i = 0;
    p = (unsigned char *)(base + k);
    do {
        h = *(unsigned short *)(off + base);
        if (h == 0) {
            *(int *)p = a;
            *(unsigned short *)(off + base) = b;
            return;
        }
        i++;
        p += 4;
        off += 2;
    } while (i != lim);
}
