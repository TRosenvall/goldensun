/* GetPortrait -- 0x08019d2c, asm/rom_15000/rom_19d2c_a.s
 * (tools/datacheck.py confirms no data section).
 *
 * BLOCKER CLASS: strength reduction of a table index. SIZE EXACT ON THE FIRST
 * CANDIDATE -- 60 instructions against 60, both searches, both tails -- with 51
 * encodings differing and one extra register in the prologue.
 *
 * WHAT IT DOES: two `{id, value}` halfword tables terminated by -1, chosen on
 * `id <= 0x13`, with the second table's hit biased by 0x80. The reading is not
 * in doubt: the instruction count, the peeled first comparison, the `bhi`
 * unsigned bound and both `-1` sentinels all reproduce.
 *
 * THE RESIDUE. The ROM keeps the table base FIXED in r4 and walks an INDEX in
 * r2, recomputing the byte offset every iteration:
 *
 *     rom    add r2, #2 / lsl r3, r2, #1 / ldrsh r3, [r4, r3]
 *     ours   add r2, r2, #4 / ldrsh r3, [r2, r4]        (base walks, r6 spent)
 *
 * gcc strength-reduces the index into a moving base and needs a second register
 * for it -- `push {r5, r6, lr}` against the ROM's `push {r5, lr}`.
 *
 * MEASURED AND INERT, all 51: the access written as an explicit byte offset
 * (`*(s16 *)((u8 *)t + i * 2)`), and `unsigned int i`.
 *
 * NOTE the ROM does NOT hold -1 in a register across the function; it rebuilds
 * `mov r5, #1 / neg r5, r5` at each comparison, and only the FIRST one shares
 * r1 with the result's initialiser. That falls out of `r = -1;` plus plain `-1`
 * literals in the comparisons, and needs no help -- worth recording because the
 * named-constant lever would be the wrong reflex here.
 *
 * NEXT: this is the batch-268 `i != N` question in the shape where that lever
 * does NOT apply -- the bound is a sentinel test, not a counter, so there is no
 * `!=` to write. It is the cheapest specimen of index-vs-pointer reduction in
 * the pool (60 instructions, one loop shape repeated twice) and therefore the
 * best place to try whatever eventually works on Sprite_SetAnim, which has the
 * same defect at 83 instructions.
 */
#include "gba/types.h"

extern s16 Data_367e4[];
extern s16 Data_3680c[];

int GetPortrait(u32 id)
{
    s16 *t;
    int r;
    int i;

    r = -1;
    i = 0;
    if (id <= 0x13) {
        t = Data_367e4;
        while (t[i] != -1) {
            if (t[i] == id) {
                r = t[i + 1];
                break;
            }
            i += 2;
        }
    } else {
        t = Data_3680c;
        while (t[i] != -1) {
            if (t[i] == id) {
                r = t[i + 1] + 0x80;
                break;
            }
            i += 2;
        }
    }
    return r;
}
