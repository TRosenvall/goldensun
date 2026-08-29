/* Func_80c1084 (SortSceneDrawList) -- NON-MATCHING.
 * Blocker class: REGISTER CHOICE for the base pointer, plus one extra label.
 * 39 lines against the ROM's 38, 9 differing.
 *
 *     rom    ldr r0, [r3, #0x0] / cmp r0, #0 ... add r0, r3
 *     ours   ldr r1, [r3, #0x0] / cmp r1, #0 ... add r0, r1, r3
 *
 * The ROM loads the state pointer into r0 -- free here, since the function
 * takes no arguments and returns nothing -- and later adds the 0x64e offset IN
 * PLACE, destroying it. gcc uses r1 and a three-operand add.
 *
 * The extra line is a second label: the ROM emits `b L0 / L0: / pop / bx`
 * where ours has `b L3 / L3: / L0: / pop / bx`, one label for the jump over
 * the mid-function pool and another for the return join.
 *
 * Tried, all identical at 9 differing:
 *   - `p += 0x64e;` before taking the pointer, which is the offset-clobber
 *     lever and is exactly the ROM's in-place add. gcc keeps both values live
 *     because the result feeds a differently-typed pointer, so the clobber
 *     never happens.
 *   - the two guards collapsed into one `if (a != 0 && b != 0) { ... }` so the
 *     function has a single exit, which is the early-return lever. The extra
 *     label survives it.
 *
 * Everything else is exact, including both pooled halfword constants -- 0x3f90
 * and 0x10 written as plain literals into REG_BLDCNT and REG_BLDALPHA, which
 * is the const.sym halfword exception working as documented -- the signed-byte
 * table lookup, and the wrap arithmetic.
 */
#include "gba/types.h"
#include "gba/io.h"

extern char *iwram_3001e74;
extern signed char Lc5c10[] __asm__(".Lc5c10");

void Func_80c1084(void)
{
    char *p;
    unsigned short *q;
    unsigned short n;
    int v;

    p = iwram_3001e74;
    if (p == 0)
        return;
    if (*(unsigned short *)(p + (0xca << 3)) == 0)
        return;
    REG_BLDCNT = 0x3f90;
    REG_BLDALPHA = 0x10;
    q = (unsigned short *)(p + 0x64e);
    REG_BLDY = Lc5c10[*q];
    n = *q;
    v = (n + 1) & 0xf;
    if (n > 0xe)
        v |= 0x10;
    *q = v;
}
