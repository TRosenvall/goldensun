/* Cluster OvlFunc_916_200836c..OvlFunc_916_200836c extracted from goldensun/asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_a.s.
 *
 * Total .text for this TU = 60 bytes (= 0x3c).
 * Preserves the original ROM layout when slotted immediately after
 * asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_a_a.o in
 * goldensun/overlays/rom_7a37f0/overlay.ld.
 *
 * The same function as src/overlays/rom_7d0e88/ovl_1528_a_a_a_c_c.c with a
 * different VCOUNT bound (0x34 rather than 0x2e) and different tables. It was
 * elevated by taking that source and changing the three constants, and matched
 * on the first screen -- the two are the same code compiled into two overlays.
 * The commentary there applies here unchanged.
 */
#include "gba/io.h"

extern unsigned int iwram_3001ad4[];
extern unsigned int L20dc[] __asm__(".L20dc");
extern unsigned int L20d0[] __asm__(".L20d0");
extern int __Random(void);

void OvlFunc_916_200836c(void)
{
    unsigned int *s;
    unsigned int *d;
    int v;
    int r;
    unsigned int x;
    unsigned int lim;

    v = REG_VCOUNT;
    s = iwram_3001ad4;
    d = (unsigned int *)REG_ADDR_BG1HOFS;
    if (v != 0xe3 && (unsigned int)v > 0x34)
        goto out;
    r = __Random();
    x = 0x64;
    x *= r;
    lim = *L20dc;
    x >>= 16;
    if (x >= lim)
        goto out;
    s = L20d0;
out:
    *d = *s++;
    d = (unsigned int *)REG_ADDR_BG2HOFS;
    *d++ = *s++;
    *d = *s;
}
