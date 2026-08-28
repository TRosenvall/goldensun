/* OvlFunc_945_2008058 -- NON-MATCHING.  Blocker class: ORDER OF A LOAD AGAINST
 * AN ARITHMETIC CHAIN.  18 of 42, same length.
 *
 * Two arms, each drawing a random offset and stepping the actor's pos.y, with
 * a shared tail that writes tickSlow.  The structure, the shared tail and both
 * comparisons are right; what differs is where the pos.y load sits:
 *
 *     rom    bl Random / ldr r3, [r5, #0xc] / lsl r0, #15 / lsr r0, #16
 *     ours   bl Random / lsl r3, r0, #15 / lsr r0, r3, #16 / ldr r3, [r5, #0xc]
 *
 * The ROM loads the field first and then narrows the random; gcc narrows
 * first.  It also keeps the arithmetic two-operand (`add r3, r2`) where ours
 * introduces a third register.
 *
 * Tried, and the FIRST form is the best -- the two obvious fixes both make it
 * worse, which is the part worth recording:
 *
 *   - reading pos.y into the accumulator before the shift, and using compound
 *     assignment so the adds are two-operand: 21 differing, up from 18. It
 *     fixes the two instructions it targets and disturbs the surrounding
 *     allocation more than it gains.
 *   - additionally using `a->tickSlow` from actor.h instead of a named `short
 *     *f`: 38 differing. The ROM computes `r6 = a + 0x66` ONCE and reuses it
 *     across both arms and the tail; the member form recomputes it, so the
 *     named pointer has to stay even though the header names the field.
 *
 * That second result is the general one: actor.h field access is the right
 * default and carried several functions this batch, but when the ROM holds a
 * FIELD ADDRESS in a register across a call or a branch, the named pointer is
 * what reproduces it and the member form is a regression.
 */
#include "gba/types.h"
#include "actor.h"

extern unsigned int __Random(void);

int OvlFunc_945_2008058(struct Actor *a)
{
    short *f;
    unsigned int u;
    int r;
    int v;

    f = (short *)((char *)a + 0x66);
    if (*f != 0) {
        u = __Random();
        r = (u << 15) >> 16;
        v = a->pos.y - r + 0xffff8000;
        a->pos.y = v;
        if (v < 0)
            *f = 0;
    } else {
        u = __Random();
        r = (u << 15) >> 16;
        v = a->pos.y + r + (0x80 << 8);
        a->pos.y = v;
        if (v > (0x80 << 12))
            *f = 1;
    }
    return 1;
}
