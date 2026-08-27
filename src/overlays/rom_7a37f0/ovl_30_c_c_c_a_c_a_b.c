/* OvlFunc_916_2008b8c  --  asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a.s
 *
 * FindRegionContaining: walk 0x0c-byte records until the halfword at +0 is -1,
 * return the first whose x/z bounds contain (x, z).  The flag at +6 chooses
 * which axis gets the three-unit extension.
 *
 * THE COPY ORDER IS LOAD-BEARING and it is a register-allocation lever, not a
 * dataflow one.  Written as `x0 = p->x0; ...; x1 = x0;` the function is exact
 * in shape but pushes r7 as a fourth callee-saved register (19 differing of
 * 50, every line a register rename).  Written the ROM's way round --
 * ldrsh into the MUTABLE copy first, then `x0 = x1; z0 = z1;` -- gcc lands on
 * the ROM's own assignment (x in r5, z left in r1, -1 in r8, the saved bounds
 * in r12/r14) and pushes three.  The ROM's `mov r12, r4 / mov r14, r2` right
 * after the three ldrsh is the tell: those two movs ARE the two copies.
 * No --cflags.
 */
struct Region { short f0; short x0; short z0; short flag; int f8; };

struct Region *OvlFunc_916_2008b8c(struct Region *p, int x, int z)
{
    int x0, z0, x1, z1, f;

    while (p->f0 != -1) {
        x1 = p->x0;
        z1 = p->z0;
        f = p->flag;
        x0 = x1;
        z0 = z1;
        if (f == 0)
            x1 += 3;
        else
            z1 += 3;
        if (x >= x0 && x <= x1 && z >= z0 && z <= z1)
            return p;
        p++;
    }
    return 0;
}
