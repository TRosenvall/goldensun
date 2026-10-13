/* Func_8003e58 (FindObjTileRun) -- asm/rom_c0/rom_3e58_a.s
 *
 * STILL PARKED, 49 of 60 stream entries.
 * NOT A TRUE DISTANCE.  ref 57 instructions against our 55 and 124 bytes
 * against 120 -- WE ARE TWO INSTRUCTIONS SHORT.  The stream figure is 2 apart
 * and the byte figure 4 apart rather than 3 and 6 because this function has an
 * ODD 16-bit instruction count ahead of its aligned two-word pool, so the
 * assembler's 2-byte pad absorbs exactly one instruction (57 instructions ->
 * 57 + 1 pad + 2 words = 60 entries, 114 + 2 + 8 = 124 bytes).
 *
 * ===== BATCH 347 BRIEF A: THE SENTENCE BELOW WAS FALSE AND IS NOW GONE ====
 * The header used to read, two lines after correctly saying the counts differ,
 * "Batch 322's re-classification of this figure is WITHDRAWN. *** THAT CLAIM WAS FALSE HERE. *** Re-measured by the coordinator after batch 348: ref 57 / ours 55 16-bit instructions, a gap of -2, so this figure is a POSITIONAL count measuring PHASE, not distance. Batch 322's re-classification sweep made this claim on 54 parks and was WRONG ON 34 OF THEM, this one included; the 18 it got right keep the claim. Close the length gap before reading the figure, then rank on tools/aligncmp.py rather than on the raw count."  It is not, it never
 * was, and it is what told three successive agents that 49-of-60 described the
 * residue.  objcmp prints the instruction counts on the same line as the figure.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c0/8003e58.c \
 *     asm/rom_c0/rom_3e58_a.s --whole
 * Piece: ONE function, datacheck no data, no exported non-function symbols --
 * it converts whole when it closes.  Pin-free and device-free.
 * Aligned residue (tools/aligncmp.py): 37 of 60 equal, 25 differing in 14 hunks.
 *
 * ===== THE GAP IS FOUR SITES, NOT ONE, AND TWO OF THEM CANCEL =============
 *
 *   region (ref label)                    ref  ours  delta
 *   prologue + the slot > 0x5f return       8     8     0
 *   .L3e68  table setup                     6     5    -1   <- `mov r7, r12`
 *   .L3e74  loop head                       9    10    +1   <- our add+ldrb
 *   the `== 0xff` body / inner guard        5     4    -1   <- `add r2, r0, r7`
 *   .L3e90  inner scan                      7     7     0
 *   .L3e9e  fill guard                      3     3     0
 *   .L3ea4  fill loop                       6     6     0
 *   the `lsl r0, #6` + its branch           2     2     0
 *   .L3eb2  skip block                      8     7    -1   <- `mov r2, r12`
 *   epilogue                                3     3     0
 *                                         ---   ---   ---
 *                                          57    55    -2
 *
 * The +1 and one of the -1s are ONE EFFECT and are already settled: the ROM
 * spends a register-offset `ldrb r3,[r7,r4]` at the loop head plus a separate
 * `add r2,r0,r7` for the inner pointer; we spend an address add plus a plain
 * load and then REUSE that address as the inner pointer.  Two against two.  Do
 * not try to "fix" the loop head -- it pays for the inner init.
 *
 * So the real gap is the two remaining -1s, and both are COPIES OF THE TABLE
 * ADDRESS OUT OF ip.  The batch-335 analysis of this holds and is confirmed:
 *   ROM   P1 = pool-loaded, home ip, serving {the fill store, the skip load};
 *         P2 = `(set P2 P1)`, home r7, serving {the loop head, the inner
 *         pointer init}.  Creating P1 costs a low staging load plus `mov r12,
 *         r1`; creating P2 costs `mov r7, r12`; each P1 use costs a move out.
 *         FOUR moves.
 *   ours  P1 = pool-loaded, home r6, serving {the loop head, the inner init,
 *         the skip load}, free to create and free at every use; P2 = copy, home
 *         ip, serving {the fill store} alone, one move to create and one at its
 *         single use.  TWO moves.
 *
 * ===== WHAT BATCH 347 ADDED ==============================================
 *
 * 1. A CLEAN NEGATIVE ON THE `cse.c` ADJACENCY LEVER, which was the obvious
 *    reading of the ROM's `mov r12, r1 / mov r4, #0 / mov r14, r2 / mov r7,
 *    r12`: TWO insns separate P1's definition from the copy, and
 *    `cse.c:5992-5994` gates its copy rewrite on `prev_nonnote_insn (insn)`
 *    being the insn that set the source.  So a NON-ADJACENT copy should
 *    survive.  Swept all 81 cells of {four use sites} x {`u`, `t`, the bare
 *    global} with the copy `t = u;` placed at THREE distances from `u`'s
 *    definition (immediately after, after `i = 0;`, after the slot-table
 *    pointer -- the ROM's own distance).  **The copy distance is BIT-INERT:
 *    all three positions produce identical output in every one of the 27
 *    partitions.**  The merge is not this transform.  gcse's cprop at pass 07
 *    remains the only candidate, as the park said.
 *
 * 2. THE FILL STORE MUST BE THE BARE GLOBAL, now on 81 cells rather than 16.
 *    Every one of the 81 partitions that reaches 55 instructions has the fill
 *    store on the bare global; routing it through either named pointer costs
 *    two (53 instructions).  That is 3^3 x 3 = 81 cells agreeing.
 *
 * 3. AND A CONTRADICTION INSIDE THIS FILE THAT SOMEBODY SHOULD RESOLVE.
 *    The ROM's fill store is `strb r6, [r1, r3]` -- encoding 54ce, Rn = r1 =
 *    the ip copy = THE BASE.  Ours is `strb r7, [r3, r1]` -- 545f, Rn = r3 =
 *    `r + j` = THE INDEX.  This header already records "THE BASE-FIRST/
 *    INDEX-FIRST LEVER DOES NOT REACH A BARE GLOBAL ARRAY; it only works
 *    through a pointer local."  Both cannot be true of the original: either
 *    that note is wrong, or the ROM's fill store is NOT a bare global and the
 *    81-cell result above is a local optimum that the two missing moves are
 *    paying for.  THAT is the next thing to settle, and it is checkable in one
 *    encoding rather than in a count.
 *
 * MEASURED FIGURES FOR THE 27 PARTITIONS (fill on the bare global, objcmp
 * differing / aligned-equal / differing / hunks):
 *    head=g, inner=g, skip=t  (THE BODY BELOW)   49   37 / 25 / 14   <- best
 *    head=g, inner or skip named, 7 other cells  49   37 / 25 / 14   (identical)
 *    head=g, inner=g, skip=g  (no name at all)   52   31 / 32 / 13
 *    head=t or u, all 18 cells                   55   33 / 28 / 15
 * So naming the skip load is worth 3 differing and 6 aligned-equal, and naming
 * the loop head is worth -6; the park's chosen body is the optimum of the axis.
 *
 * ===== CARRIED FORWARD FROM EARLIER BATCHES ==============================
 *
 * SETTLED, and each was worth several instructions:
 *   * A `goto` loop for the outer scan.  The ROM rebuilds both 0x200 and -1
 *     every iteration, which is the documented selection signature; a
 *     `while (1)` leaves the -1 outside.
 *   * A `goto` loop for the fill loop too -- a `for` gets strength-reduced into
 *     a pointer induction, and the ROM recomputes `start + j` each iteration.
 *   * ONE variable serving as both the not-found return value and the run
 *     start, assigned `r = -1` INSIDE the loop body.  gcc-2.96 always merges two
 *     textually separate `return -1;` statements into one shared block (probed
 *     three ways, including with distinct constants), so a ROM that
 *     materialises the same constant at two exits is telling you the source
 *     used one variable, not two returns.
 *   * Guard-then-do/while for the inner scan, which places `add r2,r0,r7` after
 *     the guard.
 *   * THE ORDER OF THE THREE INITIALISERS MATTERS.  The body below writes the
 *     pointer, then the index, then the slot-table pointer, and that is ONE
 *     BETTER than writing both pointers before the index.  Do not tidy it.
 *     (Batch 347 confirms it is also the ROM's own order.)
 *
 * MEASURED AND REJECTED, all of it:
 *   two locals `t` and `u = t`                       CSE merges them
 *   a second local assigned from the global, before
 *     and after the first                            inert
 *   all sixteen local-versus-bare-global combinations
 *     at the four use sites                          50-56 differing, 56/58 stream
 *   the same sixteen re-swept with objcmp            same
 *   a SECOND pointer assigned INSIDE the loop from
 *     the bare global, four partitions x three
 *     spellings of the slot-table access             54-58 stream, 55-59 differing
 *   the second pointer assigned from the first
 *     inside the loop                                same collapse
 *   the slot table as a struct subscript             inert
 *   the slot table as a bare array subscript         two shorter
 *   a local copy of the slot argument, before and
 *     after the early return, x two site combos      all inert
 *   four spellings of the fill store through the
 *     bare global (`G[r+j]`, a named offset,
 *     `*(G + (r+j))`, `*(G + o)`)                    BIT-IDENTICAL
 *   the copy's distance from its source's definition
 *     (three positions x 27 partitions)              BIT-INERT  [batch 347]
 *
 * wram.sym has no second symbol at 0x03001810, so the two-distinct-symbols
 * trick is unavailable.
 *
 * NEXT, in order: (1) settle item 3 above -- is the ROM's fill store really a
 * bare global, given its base-first operand order?  (2) the allocator question
 * the park left: what makes the POOL-LOADED pseudo the one with FEWER
 * references and the copy the one with more.  Do NOT re-sweep the four use
 * sites, the copy position, the fill-store spelling or the slot-table access.
 */
struct SpriteSlot { unsigned short size; unsigned short vramOffset; };
extern struct SpriteSlot gSpriteSlots[];
extern unsigned char gSpriteAllocTable[];

int Func_8003e58(unsigned int slot, unsigned int size)
{
    unsigned int n;
    int i;
    int r;
    unsigned int end;
    unsigned int j;
    unsigned char *p;
    unsigned char *t;
    unsigned char *s;

    n = size >> 6;
    if (slot > 0x5f)
        return -1;
    t = gSpriteAllocTable;
    i = 0;
    s = (unsigned char *)gSpriteSlots;
loop:
    r = -1;
    if (i >= 0x200)
        goto done;
    if (*(gSpriteAllocTable + i) == 0xff) {
        r = i;
        end = n + r;
        if (r < end) {
            p = r + gSpriteAllocTable;
            do {
                if (*p++ != 0xff)
                    goto skip;
                i++;
            } while (i < end);
        }
        j = 0;
        if (j >= n)
            goto shift;
    fill:
        *(gSpriteAllocTable + (r + j)) = slot;
        j++;
        if (j < n)
            goto fill;
    shift:
        r = r << 6;
        goto done;
    }
skip:
    i += *(unsigned short *)(s + (*(t + i) << 2)) >> 6;
    goto loop;
done:
    return r;
}
