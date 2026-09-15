/* Func_80f6038 -- 0x080f6038, asm/rom_f6000/rom_f6008_c_a.s (eight functions;
 * tools/datacheck.py confirms no data section).
 *
 * BLOCKER CLASS: a register rotation across SEVEN values, no spelling found.
 * SIZE EXACT on the FIRST candidate -- 52 instructions against 52, same shape,
 * same order -- with 40 encodings differing, all of them register names.
 *
 * WHAT IT DOES: scales a 15-bit BGR palette run. Each of the three channels is
 * masked out, multiplied by the scale, shifted down 16 and re-masked, then the
 * three are ORed back together.
 *
 * THE READING IS RIGHT AND THE FIRST CANDIDATE PROVED IT: 52 of 52 with the
 * `mul`s, the `lsr #16`s, the re-masks and the loop all in the ROM's order.
 * THE THREE MASKS ARE NAMED LOCALS -- the ROM holds 0x1f, 0x3e0 and 0x7c00 in
 * registers across the whole loop rather than rebuilding them, which is the
 * recorded named-constant lever and is what makes the instruction count match.
 *
 * THE ROTATION. The ROM allocates
 *
 *     src r7   dst r6   scale r5   n r0 (late)   0x1f r8   0x3e0 r14   0x7c00 r12
 *
 * and we get src r6, dst r5, scale r0, n r4, and the three masks in lr, ip and
 * r8 -- the same three registers, assigned to different masks, with everything
 * else shifted one place. Note the ROM does NOT copy `n` out of its argument
 * register for the guard; it tests r3 directly and moves it to r0 only after
 * the masks are set up.
 *
 * MEASURED AND INERT (41 or 40 of 52 throughout):
 *   `if (n <= 0) return 0;` as an early return instead of a guarded block  42
 *   `while (--n != 0)` instead of `n--; ... while (n != 0)`                41
 *   the three masks declared in reverse order                             41
 *   the three masks ASSIGNED in reverse order                             40
 *   both reversed together                                                40
 *
 * WHY SPELLING DOES NOT REACH IT, read out of global.c's find_reg:
 *
 *     COPY_HARD_REG_SET (used, used1);
 *     IOR_COMPL_HARD_REG_SET (used, regs_used_so_far);
 *     ...
 *     "we never allocate a register for the first time in pass 0"
 *
 * PASS 0 MARKS EVERY REGISTER NOT YET USED AS UNAVAILABLE. gcc only reaches a
 * FRESH register in pass 1, so which registers a function ends up spreading
 * across depends on the ORDER allocnos are processed -- `allocno_compare`,
 * priority then allocno number -- and not on anything the statements say. Seven
 * values competing for r0..r8 plus ip and lr is exactly the case where that
 * order decides everything.
 *
 * NEXT: this is the fourth function in two rounds to end on the same question
 * (with GetLocationName, UpdateScreenEdge_V and the three global_alloc parks).
 * GetLocationName is bracketed to TWO instructions and is the better specimen;
 * this one is the cleanest *shape* -- size exact, first candidate -- so it is
 * the best place to TEST a hypothesis once one exists. Do not sweep it again.
 */
#include "gba/types.h"

int Func_80f6038(u16 *src, u16 *dst, u32 s, int n)
{
    u32 m1;
    u32 m2;
    u32 m3;
    u32 c;

    if (n > 0) {
        m1 = 0x1f;
        m2 = 0xf8 << 2;
        m3 = 0xf8 << 7;
        do {
            c = *src;
            *dst = ((((c & m1) * s) >> 16) & m1)
                 | ((((c & m2) * s) >> 16) & m2)
                 | ((((c & m3) * s) >> 16) & m3);
            n--;
            src++;
            dst++;
        } while (n != 0);
    }
    return 0;
}
