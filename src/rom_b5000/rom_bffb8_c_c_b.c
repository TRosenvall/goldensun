/* Cluster Func_80c1084..Func_80c1084 extracted from goldensun/asm/rom_b5000/rom_bffb8_c_c.s.
 *
 * Total .text for this TU = 100 bytes.
 *
 * From the branch-over-pool class; the pool needed no help.
 *
 * DELETING A LOCAL WON THIS ONE. The previous park kept a separate
 * `unsigned short *q` alongside the base pointer, which put the base in r1 and
 * forced a three-operand `add`. Dropping `q` and doing everything on `p` --
 * the offset-clobber `p += 0x64e`, then casting at each use -- gives the ROM's
 * in-place `add r0, r3` and moves the state pointer to r0. That is the
 * documented "a named local used once can cost the preferred register" lever;
 * the park had tried `p += 0x64e` while KEEPING q, which is why it never fired.
 *
 * The split needed `.global .Lc5c10` first (committed separately): a `.L`
 * symbol does not survive into the object's symbol table, so the data table
 * this function indexes had to be exported before code and data could live in
 * different objects.
 */
#include "gba/types.h"
#include "gba/io.h"

extern char *iwram_3001e74;
extern signed char Lc5c10[] __asm__(".Lc5c10");

void Func_80c1084(void)
{
    char *p;
    unsigned short n;
    int v;

    p = iwram_3001e74;
    if (p == 0)
        return;
    if (*(unsigned short *)(p + (0xca << 3)) == 0)
        return;
    REG_BLDCNT = 0x3f90;
    REG_BLDALPHA = 0x10;
    p += 0x64e;
    REG_BLDY = Lc5c10[*(unsigned short *)p];
    n = *(unsigned short *)p;
    v = (n + 1) & 0xf;
    if (n > 0xe)
        v |= 0x10;
    *(unsigned short *)p = v;
}
