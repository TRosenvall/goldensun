/* UpdateScreenEdge_V -- 0x0800fec8, asm/rom_9000/rom_f9cc_c.s (four functions;
 * tools/datacheck.py confirms no data section).
 *
 * BLOCKER CLASS: REGISTER PRESSURE -- the ROM spends ip and lr as scratch and
 * we cannot. 74 instructions against the ROM's 66. Partial progress only; this
 * park is a starting point, not a finished analysis.
 *
 * THE READING IS BELIEVED RIGHT. It blits sixteen tile pairs down a screen
 * edge: a tile index out of gBuffer, scaled by 8 (`lsl #20 / lsr #17` is
 * `(t & 0xfff) * 8`), used to fetch two words from ewram_2020000 and
 * ewram_2020004 and store them 0x40 apart in the 0x6002800 map page. The
 * `lsr r3, r2, #31 / add r3, r2, r3 / asr r3, #1` pairs are signed `/ 2` on the
 * two coordinates, and the `& 0x7f` / `& 0x1e` wrap the tile and pixel columns.
 *
 * TWO LEVERS LANDED, 78 instructions -> 74:
 *   * THE DESTINATION IS ONE NAMED POINTER. The ROM computes `[r2]` once and
 *     stores at `[r2]` and `[r2, #0x40]`; recomputing `(cy + cx) << 1` for each
 *     store costs three instructions per iteration.
 *   * THE TWO MASKS ARE NAMED LOCALS. The ROM holds 0x7f and 0x1e in registers
 *     across the loop (`mov r8, r6` and `mov r14, r2`) rather than rebuilding
 *     them; writing them as literals rebuilds both every iteration.
 *
 * WHAT IS LEFT. The ROM uses FOUR registers that need no saving or only one
 * save -- r8, r10, r12 (ip) and r14 (lr) -- holding the destination base and
 * the 0x1e mask in ip and lr respectively. Ours reaches for r11 instead, which
 * costs a push/pop pair, and is eight instructions long overall.
 *
 * This is the same shape as GetLocationName's residue next door
 * (src/non_matching/rom_8a000/808b158.c): gcc declining to use ip for a value
 * that never crosses a call. REG_ALLOC_ORDER is {3,2,1,0,12,14,4,5,...}, so ip
 * and lr are SUPPOSED to come before r4 for such values -- there are no calls
 * at all in this function, so every value qualifies, and gcc still spends
 * callee-saved registers.
 *
 * NEXT: do not sweep spellings here. Two functions now sit on the same
 * question, and one of them (GetLocationName) is bracketed to exactly two
 * instructions, which makes it the better specimen. Read global.c's find_reg
 * and what excludes r12/r14, then come back to this one.
 */
#include "gba/types.h"

extern unsigned int gBuffer[];
extern unsigned char ewram_2020000[];
extern unsigned char ewram_2020004[];

void UpdateScreenEdge_V(int page, int x, int y)
{
    u8 *dst;
    u8 *d;
    int ty;
    int tx;
    int cx;
    int cy;
    int i;
    int o;
    int m1;
    int m2;

    dst = (u8 *)(0x6002800 + (page << 11));
    m1 = 0x7f;
    m2 = 0x1e;
    ty = ((y / 2) & m1) << 7;
    cy = (y & m2) << 5;
    tx = (x / 2) & m1;
    cx = x & m2;
    for (i = 0; i <= 0xf; i++) {
        o = (gBuffer[ty + tx] << 20) >> 17;
        d = dst + ((cy + cx) << 1);
        *(u32 *)d = *(u32 *)(ewram_2020000 + o);
        *(u32 *)(d + 0x40) = *(u32 *)(ewram_2020004 + o);
        tx = (tx + 1) & m1;
        cx = (cx + 2) & m2;
    }
}
