/* OvlFunc_971_200808c -- 0x0200808c
 *
 * Link-cable handshake check. If both link bits are set, take the two-bit
 * multiplayer id out of REG_SIOCNT and raise save flag 0x303; otherwise clear
 * it. Then, if the id is valid and the flag really did take, mirror the id's
 * low bit into flag 0x302 and report whether this player's slot in the ewram
 * table already holds the expected word.
 *
 * THE WHOLE FUNCTION IS BLOCK LAYOUT. Written as ordinary nested `if`s the
 * arithmetic all comes out right and the function still misses by 45 of 64,
 * because gcc lays the blocks out in source order and the ROM's order is
 *
 *      entry -> [set arm] -> [BODY] -> [clear arm] -> [join] -> return 0
 *
 * with the join's `bne` reaching BACKWARD into the body. Nothing rearranges an
 * `if`/`else` into that; the source has to name the blocks, so the two arms and
 * the body are `goto` targets in the ROM's own order. That alone took 45 to 27.
 *
 * TWO SMALLER LEVERS, both already in docs/elevation.md and both needed:
 *
 *  - `if (x == y) return 1; return 0;` is the RETURN-A-BOOLEAN idiom, and gcc
 *    if-converts it into seven branchless instructions (`eor / neg / orr /
 *    lsr / sub`). Inverting it to `if (x != y) goto out0; return 1;` restores
 *    the ROM's `cmp / bne / mov #1`.
 *  - Every zero return has to reach ONE `out0:`; letting them be separate
 *    `return 0` statements makes gcc hoist a `mov r0, #0` above the first test
 *    so one path can fall through with the value already set.
 *
 * REG_SIOCNT is read as a WORD at its address, not through the header's `vu16`
 * macro -- the ROM's `ldr` says so, and the same reading is recorded on the
 * sibling parks in src/non_matching/rom_c0/.
 *
 * `t = 3; t &= v;` is the named-constant form -- the AND's destination is the
 * constant, matching `mov r3, #3 / and r3, r2`. The compared word's address is
 * split into `q` and `k` so the ewram base is the register-offset load's BASE
 * and the .L1940 index is its OFFSET; built as one expression the roles swap.
 */
#include "gba/io.h"

extern unsigned char L1940[] __asm__(".L1940");
extern int CHAR_ARRAY_ARRAY_971__02009928[];
extern unsigned char ewram_2002024[];
extern unsigned short iwram_3001f64;

extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern int __GetFlag(int id);

int OvlFunc_971_200808c(int n)
{
    int v;
    int t;
    int s;
    int *p;
    int g;
    int q;
    int k;

    v = iwram_3001f64;
    t = 3;
    t &= v;
    s = -1;
    if (t != 3)
        goto clear303;
    s = (*(volatile unsigned int *)REG_ADDR_SIOCNT << 26) >> 30;
    __SetFlag(0x303);
    goto check;
body:
    p = &CHAR_ARRAY_ARRAY_971__02009928[n];
    if (s != 0)
        __SetFlag(0x302);
    else
        __ClearFlag(0x302);
    g = __GetFlag(0x302);
    g ^= 1;
    q = (int)ewram_2002024 + g * 24;
    k = L1940[n] << 2;
    if (*(int *)(q + k) != *p)
        goto out0;
    return 1;
clear303:
    __ClearFlag(0x303);
check:
    if (s < 0)
        goto out0;
    if (__GetFlag(0x303) != 0)
        goto body;
out0:
    return 0;
}
