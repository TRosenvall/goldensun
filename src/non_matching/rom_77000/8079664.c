/* Func_8079664 -- asm/rom_77000/rom_79460_c_a_a_a_c.s
 *
 * NON-MATCHING, 14 of 44 encodings  (MEASURED, batch 332 D).
 *   SIZE EXACT: 96 bytes both sides, 44 encodings both sides, 39 instructions
 *   both sides.  Relocations identical.  So this figure IS A DISTANCE, for the
 *   first time in this park's history -- read the note on the superseded claim
 *   below before building on anything older.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/8079664.c \
 *     asm/rom_77000/rom_79460_c_a_a_a_c.s --func Func_8079664
 *
 * NOTE ON THE REFERENCE PATH.  The recipe above names the piece as it stands
 * BEFORE batch 332's install.  This park's piece-mate AddPartyMember LANDED in
 * that batch and the piece is split to let it, which deletes the parent and
 * leaves this function in the second half -- so run tools/repoint_parks.py on
 * the parent after the split, exactly as the batches 286-289 session did after
 * every split, and this recipe will follow the function.  If parkcheck reports
 * the reference as not found, that is the step that was skipped.
 *
 * PINS: 0.  No shim, no fakematch row, no flag group.  No device.
 *
 * ---------------------------------------------------------------------------
 * THE SUPERSEDED CLAIM, AND WHY IT READ AS SO MUCH WORSE THAN IT WAS
 *
 * This park used to claim twenty-eight differing of forty-four, with the
 * reference three instructions LONGER than ours.  That was a positional count
 * over two streams of different length, so it was measuring the offset and not
 * the number of wrong decisions.  Its stated blocker -- a two-register swap in
 * the allocator that "nothing reached" -- survives, but it is now the ONLY
 * blocker rather than one of several, and it accounts for roughly eight of the
 * fourteen.  The old table's figures are all pre-alignment and must not be
 * compared against anything below.
 *
 * WHAT THE THREE MISSING INSTRUCTIONS WERE.  All three were in the shift-down
 * tail's address computation, and all three came from one fold:
 *
 *     rom    ldr r3, =gState / mov r4, #0xfc / add r3, r1, r3
 *              / lsl r4, #1 / add r2, r3, r4 / sub r1, r0, r1
 *     old    ldr r3, =gState+504 / sub r0, r0, r1 / add r2, r1, r3
 *
 * The reference builds gState + i + 0x1f8 at runtime from a NAMED base; the old
 * body wrote `&gState[0x1f8 + i]` and gcc folded the whole constant into the
 * pool, which costs the constant's materialisation and the second add.
 *
 * THE PARK ALREADY HAD THIS LEVER IN ITS HANDS AND RECORDED IT AS REFUTED.
 * Its row for the tail address "built at runtime" measured six WORSE, and the
 * park concluded "the lever is real but what decides it is not how the address
 * is spelled at that one site" and that the fold is downstream of the register
 * assignment.  The first half of that is right and the second is wrong.  That
 * probe spelled the sum as an arithmetic expression on the symbol's integer
 * value -- which does not name the base, it just moves the fold.  The base has
 * to be read into a POINTER LOCAL of its own, exactly as the same batch's
 * AddPartyMember landing needed, and then the address built from that local.
 * The park's own AddPartyMember notes had already established that the named
 * base is required and what it costs to drop; nobody carried it across to the
 * function below it in the same piece.
 *
 * SEARCH LOOP: unchanged and still exact.  gcc rotates the counted loop into
 * the reference's shape -- guard, peeled first element test through a
 * register-offset load, then the increment/guard/advance/test body -- with no
 * help at all, and the walking pointer is formed only after the peeled test.
 *
 * ---------------------------------------------------------------------------
 * WHAT THE REMAINING FOURTEEN ARE
 *
 *   (a) roughly eight: the two-register swap the park named.  The reference
 *       puts the id in the lower-numbered callee-saved register and the party
 *       size in the higher; we do the opposite, and it propagates through every
 *       compare in the search loop and into the tail's first subtract.
 *   (b) roughly six: the ORDER of the tail's six setup instructions, and which
 *       register carries the 0xfc constant.  The reference interleaves the
 *       constant's materialisation with the base-plus-index add and puts the
 *       count subtract LAST; ours puts the subtract second.  This body has the
 *       base-plus-index add in the reference's slot.
 *
 * WHERE TO LOOK FOR (a), WITH THE MACHINERY NAMED.  The swap is decided in
 * global.c's allocno_compare at :598-621, which sorts by
 * floor_log2(n_refs) * n_refs / live_length, scaled, and breaks ties by
 * allocno number -- i.e. by pseudo order, which puts a PARAMETER ahead of a
 * local.  Both values have three references here, so floor_log2 times refs
 * ties at three for each and LIVE LENGTH is the only term left.  The id is
 * live from the parameter's set at function entry; the size is live from the
 * first call's return.  The id therefore starts earlier and dies about one
 * instruction earlier, which makes its live range the LONGER of the two by a
 * hair, which loses it the sort -- and the reference wants it to win.  Also
 * note arm.h:989-995's allocation order reaches r4 before r5 before r6, and
 * this object is built with r4 call-clobbered, so the first callee-saved
 * register either value can take is the one the sort hands out first.
 *
 * So the quantity to move is a LIVE LENGTH of one or two instructions, not a
 * declaration order and not an assignment position.  Both of those are now
 * measured inert here:
 *
 * MEASURED IN BATCH 332 D, all at the reference's length, 44 encodings both
 * sides, no relocation or memory-profile divergence:
 *   14   THIS BODY
 *   14   the tail address as `g + i + 0x1f8` in one expression        INERT
 *   14   the tail address as `g + 0x1f8` then `+= i`                  INERT
 *   14   the count subtract before the address, three placements      INERT
 *   15   the tail address as `p = g + i;` then `p += 0x1f8;`
 *   15   the tail address as `&g[i]` then `+= 0x1f8`
 *   18   the count subtract placed last of the tail's statements
 *   21   all 720 permutations of the six local declarations -- EXACTLY
 *        FLAT, every one of them, and that is the whole dimension
 *   21   the id copied to a local assigned first
 *   21   the search loop as a while with an explicit index init
 *   21   the element compare with its operands reversed
 *   26   the size minus one computed before the search loop
 *   42   the ClearFlag call moved before the size call (and the length
 *        breaks, so that row is not even a distance)
 *
 * The 720-row declaration sweep is the useful negative: it closes the
 * dimension the park's old note pointed at ("neither an explicit assignment
 * position nor declaration order moved them") with the full product rather
 * than a sample, and it says the answer is not in the source's ORDER at all.
 *
 * NEXT: a construct that shortens the id's live range by an instruction or two
 * without adding one -- or that lengthens the size's.  The size's last use is
 * the subtract that forms the limit; anything that legitimately reads the size
 * after that point would flip the sort.  Do NOT spend another round on
 * declaration or assignment order; that product is exhausted above.
 */
extern unsigned char gState[];
extern int GetPartySize(void);
extern void ClearFlag(int id);

int Func_8079664(int id)
{
    unsigned char *g;
    unsigned char *p;
    int size;
    int i;
    int last;
    int n;

    size = GetPartySize();
    ClearFlag(id);
    for (i = 0; i < size; i++) {
        if (gState[0x1f8 + i] == id)
            break;
    }
    last = size - 1;
    if (i < last) {
        n = last - i;
        g = gState;
        p = (g + i) + 0x1f8;
        do {
            *p = p[1];
            n--;
            p++;
        } while (n != 0);
    }
    return GetPartySize();
}
