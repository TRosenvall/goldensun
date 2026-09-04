/* OvlFunc_924_200adcc -- NON-MATCHING.  Blocker class: CONSTANT DERIVATION.
 *
 * 24 lines against the ROM's 24, 5 differing, ALL of them in the setup.  The
 * loop is byte-exact, including the counter direction and the unsigned test.
 *
 * What it does: a 7-entry palette rotation at 0x50000c2.  Entry 0 is saved to
 * entry 6 at 0x50000ce, then entries 1..6 are copied down over 0..5.  Guarded
 * on (iwram_3001e40 & 7) == 0, so it advances one step every eighth frame.
 *
 * THE ONE DEFECT
 *
 *     rom   ldr r2, =0x50000c4      <- loads the source pointer from the pool
 *     ours  sub r2, #0xa            <- derives it as 0x50000ce - 0xa
 *
 * gcc notices that the save target and the source pointer are 10 apart and
 * reuses the register instead of taking a second pool entry.  The ROM keeps
 * three independent pool constants: 0x50000c2, 0x50000ce and 0x50000c4.
 * Everything else follows from that one register being live in the wrong
 * form, which is why five lines move rather than one.
 *
 * TWO THINGS WERE LEARNED GETTING TO 5, AND BOTH ARE REUSABLE
 *
 * 1. The counter must be UNSIGNED.  With `int i`, gcc reverses the loop into a
 *    countdown (`mov r2, #5 / sub r2, #1 / cmp r2, #0 / bge`) because a signed
 *    counter whose value is otherwise dead can run either way.  Making it
 *    `unsigned int` forbids that -- a countdown ending at 0 cannot express
 *    the unsigned test -- and the loop then matches the ROM's `add r0, #1 /
 *    cmp r0, #5 / bls` exactly.  The ROM's `bls` is itself the tell: an
 *    unsigned branch on a loop counter means the counter is unsigned.
 *
 * 2. Assigning the counter BEFORE the source pointer moves it from 9 differing
 *    to 5.  Assignment order decides which of the two gets r0.
 *
 * Tried and did not help (all 5 or 9 differing, none lower):
 *   - seven orderings of the four assignments and three declaration orders
 *   - a separate named `save` pointer for 0x50000ce
 *   - `volatile` on the pointers, copying the sibling ovl_2dcc_b.c which
 *     matches and whose palette pointer is volatile
 *   - --no-rerun-cse (5), --O1 (7), --no-sched2 (7).  No flag group reaches
 *     it, so this is not the per-file-flags class.
 *
 * THE RE-ATTACK BELOW WAS TRIED IN BATCH 204 AND FAILS. Both forms are much
 * WORSE than the 5 already recorded, and both make the function LONGER than the
 * ROM, which the 5-differing candidate never does:
 *
 *     a common base pointer, `b = 0x5000000` with `b + 0x28` etc.
 *                                              28 lines against 24, 21 differing
 *     the source derived as `s = d + 1`        26 lines against 24, 17 differing
 *
 * The reason is visible once measured: the ROM wants THREE INDEPENDENT POOL
 * ENTRIES, and every construct that relates the addresses to each other pushes
 * gcc further toward deriving them, which is the defect. A common base is more
 * relation, not less. The idea was pointing the wrong way.
 *
 * What would help is the opposite -- something that stops gcc noticing the
 * three literals are close together -- and no C construct for that is known.
 * The remaining question is whether the original source had these addresses
 * behind something opaque, a macro expanding to a cast per use or three
 * separate declarations, rather than three inline literals in one function.
 *
 * Re-attack idea not yet tried: give the three addresses a common symbolic
 * base so they are offsets from one symbol rather than three literals.  That
 * changes what lands in the pool, so it needs the pool checked as well as the
 * code.
 */
extern int iwram_3001e40;

void OvlFunc_924_200adcc(void)
{
    volatile unsigned short *d;
    volatile unsigned short *s;
    unsigned int i;

    if ((iwram_3001e40 & 7) == 0) {
        d = (volatile unsigned short *)0x50000c2;
        *(volatile unsigned short *)0x50000ce = *d;
        i = 0;
        s = (volatile unsigned short *)0x50000c4;
        do {
            *d = *s;
            i++;
            s++;
            d++;
        } while (i <= 5);
    }
}
