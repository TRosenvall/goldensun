/* Cluster OvlFunc_919_200815c..OvlFunc_919_200815c -- the whole of
 * goldensun/asm/overlays/rom_7a67d8/ovl_30_a_c_c_c_a_c_c_c_c_c_c_c.s,
 * confirmed data-free by split_s.py.
 *
 * Total .text for this TU = 88 bytes.
 *
 * From the branch-over-pool class; the pool emitted at the ROM's position
 * unaided, with the pool words in the ROM's order, reached with NO int
 * intermediate. The recipe recorded elsewhere in the docs is not the only
 * route to that order.
 */
#include "gba/types.h"
#include "gba/io.h"

struct S {
    unsigned char pad00[0x52a];
    unsigned short f52a;
    unsigned char pad52c[8];
    unsigned short f534;
    unsigned short f536;
};

extern struct S *iwram_3001ecc;
extern void __Func_808fe38(int n);

void OvlFunc_919_200815c(void)
{
    __Func_808fe38(9);
    REG_BLDCNT = 0x3f42;
    REG_BLDALPHA = 0xc04;
    iwram_3001ecc->f534 = 0x3f3f;
    iwram_3001ecc->f536 = 0x1f;
    iwram_3001ecc->f52a = 0xa;
}
