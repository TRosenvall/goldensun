/* Cluster OvlFunc_911_20080a0..OvlFunc_911_20080a0 extracted from
 * goldensun/asm/overlays/rom_79e5c0/ovl_30_a_c_a_a_c.s.
 *
 * Total .text for this TU = 46 bytes (= 0x2e).
 * Placed in the run in goldensun/overlays/rom_79e5c0/overlay.ld.
 *
 * Initialises nine 0x18-byte slots starting at +0x48: a type halfword that is
 * 0x69 for every slot except the seventh and eighth, a byte at +0x16, and a
 * word at +4.
 *
 * TWO UNSIGNED TESTS, AND THE LOOP BOUND IS THE ONE THAT IS EASY TO MISS.
 *
 *   The slot test is `(unsigned)(i - 6) <= 1` -- the ROM's
 *   `sub r3, r2, #6 / cmp r3, #1 / bhi`. That is the fused compound form, and
 *   here it is what the ROM wants.
 *
 *   The LOOP COUNTER also has to be unsigned. With `int i`, `i <= 8` gives the
 *   signed `ble`; the ROM has `bls`. That was the entire remaining difference
 *   -- one instruction out of twenty-three -- and it is invisible unless you
 *   read the branch suffix. Worth checking before diagnosing anything else:
 *   `bls`/`bhi` on a loop bound means the counter was unsigned in the source.
 */

struct E {
    short a;
    unsigned char pad02[2];
    int b;
    unsigned char pad08[0xe];
    unsigned char c;
    unsigned char pad17;
};

void OvlFunc_911_20080a0(char *base)
{
    struct E *e;
    unsigned int i;

    e = (struct E *)(base + 0x48);
    i = 0;
    do {
        e->a = 0x69;
        if ((unsigned int)(i - 6) <= 1)
            e->a = 0x6e;
        i++;
        e->c = 2;
        e->b = 1;
        e++;
    } while (i <= 8);
}
