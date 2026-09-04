/* OvlFunc_924_200a648  --  0x0200a648  [asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_a.s]
 *
 * NOT MATCHING. 5 of 24, LENGTH EXACT.
 *
 * READ src/non_matching/ovl_7ac2d8/200adcc.c FIRST. That park covers this
 * function's twin, predates this file, and contains the whole analysis. This
 * file exists only because the twin was worked separately; everything below is
 * a pointer to it plus a correction of what this file used to claim.
 *
 * A 7-entry palette rotation at 0x5000050, guarded on (iwram_3001e40 & 7) == 0.
 * OvlFunc_924_200adcc is the same routine at 0x50000c2 with a bound of 5. Both
 * screen at 5 of 24 with the body below.
 *
 * THE REMAINING DEFECT IS CONSTANT DERIVATION, diagnosed in the twin's park:
 *
 *     rom   ldr r2, =0x5000052      <- a third independent pool entry
 *     ours  sub r2, #0xc            <- derived from the save target
 *
 * gcc notices the save target and the source pointer are a fixed distance apart
 * and reuses the register rather than taking another pool slot. Five lines move
 * because that one register is live in the wrong form.
 *
 * ==================== CORRECTION, BATCH 204 ====================
 *
 * THIS FILE PREVIOUSLY CLAIMED TWO THINGS THAT WERE WRONG, and both were mine.
 *
 * 1. It presented "an unsigned counter blocks gcc's loop reversal" as a finding
 *    of batch 203. It is not new. The twin's park had already recorded it, with
 *    a better statement of the tell: the ROM's `bls` is itself the evidence,
 *    because an unsigned branch on a loop counter means the counter is
 *    unsigned. I derived it again from scratch without looking.
 *
 * 2. It said "this file parks BOTH functions" and "solving either solves both",
 *    while a separate, older and BETTER park for the twin already existed. Mine
 *    screened at 9 and 10 differing; the existing one screens at 5, because it
 *    also knows that assigning the counter BEFORE the source pointer is worth
 *    four instructions. That ordering is now used here.
 *
 * WHY IT HAPPENED, since it is a process failure and not a compiler one: I
 * triaged these two functions out of tools/shape_groups.py and never grepped
 * src/non_matching for their names before starting. The tree's own rule --
 * locate a function by NAME, not by path or address, recorded in batch 197 --
 * applies to checking whether a park already exists, and I applied it only to
 * finding .s files.
 *
 * THE OPEN WORK is the re-attack the twin's park names and has not been tried:
 * give the three addresses a common symbolic base so they are offsets from one
 * symbol rather than three literals, which changes what lands in the pool and
 * so needs the pool checked as well as the code.
 */

extern int iwram_3001e40;

void OvlFunc_924_200a648(void)
{
    volatile unsigned short *d;
    volatile unsigned short *s;
    unsigned int i;

    if ((iwram_3001e40 & 7) == 0) {
        d = (volatile unsigned short *)0x5000050;
        *(volatile unsigned short *)0x500005e = *d;
        i = 0;
        s = (volatile unsigned short *)0x5000052;
        do {
            *d = *s;
            i++;
            s++;
            d++;
        } while (i <= 6);
    }
}
