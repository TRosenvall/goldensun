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
 * ==================== THE NAMED RE-ATTACK IS NOW MEASURED, BATCH 271 ====================
 *
 * The open work was: give the three addresses symbolic names rather than three
 * literals, so what lands in the pool changes. TRIED. It does exactly what the
 * diagnosis predicted and does NOT close the function.
 *
 * THREE INDEPENDENT extern symbols -- not a common base with offsets, which
 * would only hand gcc the differences again -- remove the derivation outright:
 *
 *     extern volatile unsigned short _PLTT_50[], _PLTT_52[], _PLTT_5E[];
 *
 *     ours before   sub r2, #0xc            <- derived from the save target
 *     ours after    ldr r2, =_PLTT_52       <- a third independent pool entry
 *
 * WHY IT WORKS: an extern's value is unknown at COMPILE time, so gcc cannot
 * compute 0x500005e - 0x5000052 and has no derivation available. A `#define`
 * would not do this -- it is a literal at compile time and derives like one.
 * That is also the limit of what this proves about the original source.
 *
 * WHAT IS LEFT is a TWO-REGISTER TRANSPOSITION on the save pair, and the count
 * stays at 5 because the cascade is replaced rather than removed:
 *
 *     rom    ldr r1, =0x5000050 / ldr r3, =0x500005e / ldrh r2, [r1] / strh r2, [r3]
 *     ours   ldr r1, =_PLTT_50  / ldr r2, =_PLTT_5E  / ldrh r3, [r1] / strh r3, [r2]
 *
 * Of the five, TWO are the transposition and THREE are pool-entry SPELLING
 * (`=_PLTT_50` against `=0x5000050`) which would link to identical words -- so
 * the real residue is two instructions, not five. That is a much sharper park
 * than the derivation was.
 *
 * MEASURED AND INERT against the transposition, all 5: the save target named as
 * a local pointer; that pointer assigned BEFORE `d`; the counter assigned first
 * (the twin's ordering lever, which was worth four instructions on the
 * derivation and is worth nothing here). MEASURED AND WORSE: naming the copied
 * halfword in a local, 26 lines and 19 differing.
 *
 * SO THIS IS NOT LANDABLE AS IT STANDS, and the reason is worth stating. Making
 * it exact would need three absolute assignments in wram.sym, and they would be
 * buying a NON-match -- a build-input change for a function that still differs.
 * The LoadUIBanner precedent landed in this batch is the contrast: there the
 * symbol count is forced by a decisive in-function control (one symbol gives
 * nine instructions against the ROM's twenty-nine) and the result is EXACT. Here
 * the symbols improve the shape and something else still blocks it.
 *
 * NEXT: the transposition is the corpus's dominant class, but this is a
 * two-value case in SCRATCH registers rather than callee-saved ones -- the same
 * subfamily as OvlFunc_924_200d158's park. Those two should be read together.
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
