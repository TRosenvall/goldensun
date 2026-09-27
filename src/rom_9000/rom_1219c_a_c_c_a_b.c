/* Func_8012350  --  0x08012350, split out of asm/rom_9000/rom_1219c_a_c_c_a.s;
 * Func_8012388, Func_80123f4 and Debug_SpriteTest stay in _c.s.
 *
 * The third member of the pre-header load merge (see Func_80064b8 in
 * src/rom_c0/rom_5cf8_a_a_c_c.c for the mechanism): `do { } while (0);` between
 * the pre-header load `v = p->f4;` and the counter's `i = 0;`. Was parked at
 * 24 encodings against 26, four bytes short -- the sunk load.
 */
#include "gba/types.h"

extern u32 iwram_3001e70;
extern void WaitFrames(s32 n);

struct T { s32 pad_00; s32 f4; s32 f8; s32 fc; };

void Func_8012350(void)
{
    struct T *p;
    s32 i;
    s32 v;
    s32 lim;

    p = (struct T *)iwram_3001e70;
    v = p->f4;
    do { } while (0);
    i = 0;
    goto check;
loop:
    WaitFrames(1);
    lim = 0x96 << 1;
    i++;
    if (i >= lim)
        goto out;
    v = p->f4;
check:
    if (v > 0xff)
        goto loop;
    if (p->f8 > 0xff)
        goto loop;
out:
    p->fc = 0;
}
