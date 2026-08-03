/* OvlFunc_935_2008704  [ovl_7bf5a8]  --  0x02008704
 *
 * Source asm: goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_a.s
 *
 * Sets bit 1 of flags on slots 0x10 through 0x15.
 *
 * Blocker: ONE TRANSPOSITION in the loop set-up. Twenty-three instructions
 * against twenty-three, everything else identical including the epilogue:
 *
 *     rom    mov r3,#0 / mov r6,#0x10 / mov r7,#2 / mov r5,#5 / mov r8,r3
 *     ours   mov r3,#0 / mov r6,#0x10 / mov r5,#5 / mov r7,#2 / mov r8,r3
 *
 * The ROM builds the OR mask before the loop counter; gcc builds the counter
 * first. Nothing else in the function differs.
 *
 * WORTH NOTING: gcc reproduces the odd `mov r3, #0 / mov r8, r3` -- a zero
 * parked in r8 and never read -- without being asked. That is a real artefact
 * of this compiler at -O2, not something the source says, and it is a good
 * sign the overall shape is right.
 *
 * TRIED, all still 23-vs-23 diverging at instruction 5 unless noted:
 *
 *   1. the form below (do/while, counter then slot)
 *   2. `for (n = 5; n >= 0; n--)` with the slot advanced in the body
 *   3. `a->flags = a->flags | 2` instead of |=, and the increments swapped
 *   4. slot and counter initialised in their declarations, `--n >= 0` as the
 *      condition, slot incremented in the call argument
 *   5. the mask named in a local and assigned BEFORE the counter -- this is
 *      the obvious move and it makes things WORSE: gcc folds the named mask
 *      away and the function comes out 18 instructions.
 *
 * Attempt 5 is the interesting one. Naming an intermediate is what fixed a
 * folded byte offset (batch 12) and a split mov/lsl pair (batch 10), and it
 * is what fixed the 32-bit width in the narrow_constant blocker. Here it does
 * the opposite: the name gives gcc something to constant-fold rather than
 * something to keep. That is now the second shape where the lever backfires
 * -- see src/non_matching/ovl_common/common2_254.c for the first.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_935_2008704(void)
{
    Actor *a;
    int slot;
    int n;

    slot = 0x10;
    n = 5;
    do {
        a = __MapActor_GetActor(slot);
        a->flags |= 2;
        n--;
        slot++;
    } while (n >= 0);
}
