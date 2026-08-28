/* OvlFunc_945_20082f4 -- NON-MATCHING.  Blocker class: ADDRESS COMPUTATION
 * SCHEDULED EARLY.  3 of 36, same length.
 *
 * The ROM computes the address of a->flags BETWEEN a value and the store that
 * consumes it; gcc finishes the store first and then computes the address:
 *
 *     rom    and r2,r3 / orr r2,r1 / mov r1,r6 / add r1,#0x23
 *            / strb r2,[r5,#0x15] / ldrb r2,[r1]
 *     ours   and r2,r3 / orr r2,r1 / strb r2,[r5,#0x15]
 *            / mov r1,r6 / add r1,#0x23 / ldrb r2,[r1]
 *
 * Same instructions, one moved across two others.  Everything else -- the
 * sprite byte masks, the flags read-modify-write, both calls -- is exact.
 *
 * SOLVED ON THE WAY, and this is the reusable part: THE SPRITE MUST BE LOADED
 * BEFORE interactFlags IS STORED.  Written the other way round it is 5
 * differing from line 5; this way the first difference is at line 20 and the
 * whole prologue and first call are exact.  Both orders are semantically
 * identical -- a load from a->sprite and a store to a->interactFlags cannot
 * alias each other's result -- so this is pure source order, and the ROM's
 * `ldr r5, [r6, #0x50]` sitting above `strb r3, [r2]` is the tell.
 *
 * Tried for the remaining 3, both WORSE:
 *   - naming `fp = &a->flags` before the s[0x15] statement, which is the
 *     "name the OFFSET, not the base" lever and is where the ROM computes it:
 *     7 differing.  The lever hoists the address too far, not just far enough.
 *   - swapping the two independent updates so the flags RMW comes before the
 *     s[0x15] one: 32 differing and two lines longer.  They are independent
 *     objects so the swap is legal, and it is much worse.
 *
 * The remedy would need gcc to schedule an address computation one slot
 * earlier without hoisting it out of the statement, which no source form
 * expresses.  Worth re-screening if a scheduling lever is ever found; the
 * function is otherwise finished.
 */
#include "gba/types.h"
#include "actor.h"

extern void __Actor_SetSpriteFlags(struct Actor *a, int n);
extern void __Func_80929d8(struct Actor *a, int n);

int OvlFunc_945_20082f4(struct Actor *a)
{
    unsigned char *s;
    int m;

    s = (unsigned char *)a->sprite;
    a->interactFlags = 8;
    __Actor_SetSpriteFlags(a, 0);
    m = -0xd;
    s[9] = (s[9] & m) | 4;
    s[0x15] = (s[0x15] & m) | 4;
    a->flags = (a->flags & 0xfe) | 2;
    __Func_80929d8(a, 0xf);
    return 1;
}
