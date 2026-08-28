/* Func_801a910 (AllocScreenSlot) -- NON-MATCHING.
 * Blocker class: BRANCH POLARITY -- which arm falls through and which is the
 * branch target.  31 of 49, same length.
 *
 *     rom    cmp r3, #0 / bne <increment> ... <return> falls through
 *     ours   cmp r3, #0 / beq <return>    ... <increment> falls through
 *
 * gcc lays a loop out so the body falls through and the early exit is the
 * branch target; the ROM does the opposite.  Writing the source with an
 * explicit `goto` past the return -- the batch-53/55 lever, a branch in the
 * SOURCE stopping a rewrite -- does NOT hold: gcc re-lays-out the blocks
 * anyway and the polarity is unchanged (31 differing with the goto, 33
 * without, so the goto is worth two lines elsewhere and nothing here).
 *
 * WHAT WAS SOLVED, and the length is the evidence:
 *
 *   THE LOOP CARRIES AN OFFSET, NOT A SECOND POINTER.  Written as a walking
 *   `struct Rec *` over a 0x34-stride array with the flag at +0xa -- the
 *   spelling that matched Func_801c9c8 in this same batch -- this function
 *   comes out 35 lines against the ROM's 49, FOURTEEN SHORT.  The ROM keeps
 *   three values per loop: a counter, a pointer to the flag, and a separate
 *   integer offset used to build the RETURN address (`add r0, r4, r0` then
 *   +0x1d4).  A pointer walk collapses the last two, so a third of the
 *   function disappears.  Restoring the explicit offset brings it to 49
 *   against 49.
 *
 *   That is the same reading as OvlFunc_957_2008f10 in batch 140: a stream
 *   that is N lines SHORT means missing structure, not a missing instruction,
 *   and the fix is to give the source the values the ROM is carrying.
 *
 *   ASSIGNMENT ORDER fixed the prologue: `i = 0;` before `q = b + 0x1de;`
 *   matches the ROM's `mov r1, #0 / add r2, r4, r3` and moved the first
 *   difference from line 7 to line 13.
 *
 * The two arms are the same shape over different arrays -- 7 records at 0x68
 * and 5 at 0x1d4, which is exactly where the first array ends -- so one fix
 * takes both.
 */
extern char *iwram_3001e98;

char *Func_801a910(int alloc)
{
    char *b;
    char *q;
    int i;
    int off;

    b = iwram_3001e98;
    if (alloc != 0) {
        i = 0;
        q = b + (0xef << 1);
        off = 0;
        do {
            if (*(unsigned short *)q != 0)
                goto nextA;
            return b + off + (0xea << 1);
        nextA:
            i++;
            q += 0x34;
            off += 0x34;
        } while (i != 5);
        return 0;
    }
    i = 0;
    q = b + 0x72;
    off = 0;
    do {
        if (*(unsigned short *)q != 0)
            goto nextB;
        return b + 0x68 + off;
    nextB:
        i++;
        q += 0x34;
        off += 0x34;
    } while (i != 7);
    return 0;
}
