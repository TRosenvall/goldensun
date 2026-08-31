/* GetFlag -- 0x08079338, SetFlag -- 0x08079358, ClearFlag -- 0x08079374
 * (asm/rom_77000/rom_79338_a.s -- all three functions in the TU)
 *
 * BLOCKER: the base address is materialised too early, which occupies r3 and
 * forces the destructive form of the index shift.  SetFlag 5 of 11 differ,
 * ClearFlag 5 of 11, both at EXACT length.  See rom_79338_c_a.c for the five
 * sibling accessors in the neighbouring TU, blocked identically.
 *
 * ROOT CAUSE, which took the whole family to see.  The ROM finishes the bit
 * mask BEFORE computing the byte index:
 *
 *     rom   mov r3, #7 / and r3, r0     bitpos -> r3
 *           mov r2, #1 / lsl r2, r3     mask   -> r2, and r3 is now DEAD
 *           lsl r3, r0, #0x14           index temp reuses the freed r3
 *           lsr r0, r3, #0x17
 *
 *     ours  mov r3, #7 / and r3, r0     bitpos -> r3
 *           ldr r1, =gFlags             <-- hoisted here
 *           lsl r0, #0x14               r3 still holds bitpos, so the shift
 *           lsr r0, #0x17                   must clobber r0 instead
 *           mov r2, #1 / lsl r2, r3     mask finished last
 *
 * The two-operand `lsl r0, #0x14` versus the ROM's three-operand
 * `lsl r3, r0, #0x14` is NOT an independent difference -- it is downstream of
 * the ordering.  gcc needs a scratch register for the shift temp; the ROM has
 * r3 free by then and we do not, because our mask is still pending and the
 * gFlags address has taken the other register.  Fixing the order would fix the
 * shift form for free.  Chasing the shift form directly is wasted effort, and
 * three attempts confirmed that (see the c_a file).
 *
 * WHAT WAS TRIED:
 *
 *   mask and index as named locals, mask assigned first     5 differ (best)
 *   base address as a named `unsigned char *p` local        5 differ, no change
 *   --no-sched2                                             4 / 3 differ
 *
 * The named-pointer attempt is the informative failure.  Assigning
 * `p = gFlags;` after both other locals should anchor the address load late,
 * and it changes NOTHING -- gcc folds the local away entirely.  gFlags is a
 * link-time constant address, so it is foldable, and a name given to a foldable
 * value does not survive to influence code generation.  This is the same rule
 * that decided Func_801d94c (named constant, folded, no effect) and
 * Func_80175c0 (named RUNTIME expression, survived, closed the diff).  A symbol
 * address counts as foldable.  Three confirmations now; treat naming as a lever
 * only for values gcc cannot compute at compile time.
 *
 * --no-sched2 moves the needle, which places the hoist in the post-reload
 * scheduler, but it does not close either function and Makefile:178 explicitly
 * warns against adopting that flag on thin evidence -- it breaks four already
 * matching siblings elsewhere.  Not pursued.
 *
 * GetFlag is left in its INLINE form below, not the named-locals form.  The
 * named version is worse in a way worth recording: 16 lines against the ROM's
 * 13, where the inline version is 11.  The branchless tail
 * `neg r0, r3 / orr r0, r3 / lsr r0, #31` is gcc's idiom for `!= 0`, and it is
 * reached only when the AND feeds the test directly; routing the mask through
 * a named local breaks that and gcc emits a longer sequence.  So the naming
 * lever is not merely neutral here, it is actively harmful -- the two halves of
 * this TU want opposite spellings.
 *
 * WORTH THE EFFORT IF REVISITED: GetFlag alone has 180 call sites, the most-used
 * accessor in the game, and all three functions are under 14 instructions with
 * exact lengths already.  The whole family turns on one scheduling decision.
 */
extern unsigned char gFlags[];

int GetFlag(int idx)
{
    return (gFlags[(idx & 0xfff) >> 3] & (1 << (idx & 7))) != 0;
}

void SetFlag(int idx)
{
    int bit;
    int i;

    bit = 1 << (idx & 7);
    i = (idx & 0xfff) >> 3;
    gFlags[i] |= bit;
}

void ClearFlag(int idx)
{
    int bit;
    int i;

    bit = 1 << (idx & 7);
    i = (idx & 0xfff) >> 3;
    gFlags[i] &= ~bit;
}
