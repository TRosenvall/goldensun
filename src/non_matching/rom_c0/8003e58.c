/*
 *
 * NON-MATCHING, 49 of 60 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 60, ours 58) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 124 bytes, ours 120.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c0/8003e58.c \
 *     asm/rom_c0/rom_3e58_a.s --whole
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Func_8003e58 (FindObjTileRun) -- asm/rom_c0/rom_3e58_a.s
 *
 * BLOCKER: the ROM carries one address in TWO registers and we carry it in one.
 * 63 lines against 65.
 *
 * The alloc-table address lives in r12 (skip and fill) with a low copy in r7
 * (loop head and scan). We keep a single register, so `mov r7, r12` is absent
 * and three addressing forms differ downstream:
 *
 *      rom   ldrb r3,[r7,r4]           ours  add r2,r4,r6 / ldrb r3,[r2]
 *      rom   add r2,r0,r7              ours  (absent -- gcc reuses)
 *      rom   mov r2,r12 / ldrb r3,[r2,r4]   ours  ldrb r3,[r6,r4]
 *
 * TRIED AND REJECTED for the second register, all measured: two locals `t` and
 * `u = t` (CSE merges them, 61 lines); a second local assigned from the global
 * (61); that assignment before and after the first (61 both); and ALL SIXTEEN
 * combinations of local-versus-bare-global at the four use sites (59-63 lines,
 * 47-59 differing). The local/global mix is the only thing that yields two
 * pseudos at all, and gcc then splits them across the wrong sites.
 *
 * A BOUNDARY WORTH RECORDING: four spellings of the fill store through the bare
 * global -- `G[r+j]`, a named offset, `*(G + (r+j))`, `*(G + o)` -- are
 * byte-identical to each other. THE BASE-FIRST/INDEX-FIRST LEVER DOES NOT REACH
 * A BARE GLOBAL ARRAY; it only works through a pointer local. The existing
 * entry for that lever does not state this.
 *
 * wram.sym has no second symbol at 0x03001810, so the two-distinct-symbols
 * trick is unavailable.
 *
 * SETTLED, and each was worth several instructions:
 *   * A `goto` loop for the outer scan. The ROM rebuilds both 0x200 and -1
 *     every iteration, which is the documented selection signature; a
 *     `while (1)` leaves the -1 outside.
 *   * A `goto` loop for the fill loop too -- a `for` gets strength-reduced into
 *     a pointer induction, and the ROM recomputes `start + j` each iteration.
 *   * ONE variable serving as both the not-found return value and the run
 *     start, assigned `r = -1` INSIDE the loop body. gcc-2.96 always merges two
 *     textually separate `return -1;` statements into one shared block (probed
 *     three ways, including with distinct constants), so a ROM that
 *     materialises the same constant at two exits is telling you the source
 *     used one variable, not two returns.
 *   * Guard-then-do/while for the inner scan, which places `add r2,r0,r7` after
 *     the guard.
 *
 * ========== BATCH 335 BRIEF D: THE GAP IS LOCATED, AND THE BLOCKER SENTENCE
 * ========== ABOVE IS WRONG =================================================
 *
 * Re-measured whole: 49 of 60 encodings, ours 58, so the reference is TWO
 * instructions longer and the figure above is unchanged.  Pin-free confirmed
 * by tools/shimcount.py -- no register declarations, no .equ, no barriers.
 * Its piece holds exactly ONE function, so it converts alone.
 *
 * BOTH MISSING INSTRUCTIONS ARE COPIES OF THE SAME BASE ADDRESS OUT OF ip, and
 * the blocker sentence above ("the ROM carries one address in TWO registers and
 * we carry it in one") is false: WE CARRY IT IN TWO AS WELL.  What differs is
 * which of the four use sites each pseudo serves, and that decides the count.
 *
 *   ROM   P1 = {fill store, skip load} -> ip.  A pc-relative pool load cannot
 *         target a high register, so creating it costs a low-register staging
 *         load plus a move into ip, and EACH of its two uses costs a move back
 *         out.  P2 = {loop-head load, inner pointer init} -> r7, created by one
 *         move from ip and costing nothing at either use.  FOUR moves.
 *   ours  P1 = {loop-head load, skip load} -> r6, created by the pool load
 *         itself and costing nothing.  P2 = {fill store} -> ip, one move to
 *         create and one at its single use.  TWO moves.
 *
 * Four minus two is the gap exactly.  Note also that the loop-head and inner
 * pair is ALREADY count-neutral: we spend an address add plus a plain load and
 * then reuse that address as the inner pointer, where the ROM spends a
 * register-offset load and a separate pointer add.  Two against two.
 *
 * So the question is not "which instruction is missing" but WHY THE PARTITION
 * DIFFERS, and the answer is that gcse's copy propagation is GLOBAL (pass 07,
 * and no basic-block boundary stops it): every spelling that tries to give the
 * base two source-level names collapses them before the allocator sees two
 * pseudos.
 *
 * MEASURED IN BATCH 335, ADDING TO THE SIXTEEN ABOVE -- all worse or inert:
 *   the four sites re-swept with objcmp rather than line counts, all sixteen
 *     combinations                          50 to 56 differing, 56 or 58 long
 *   -- and the fill store through a pointer local always costs TWO MORE, so the
 *      fill must stay on the bare global; that is the one thing the sixteen
 *      agree on.
 *   a SECOND pointer assigned INSIDE the loop from the bare global, four
 *     partitions by three spellings of the slot-table access
 *                                           54 to 58 long, 55 to 59 differing
 *   -- this refutes the obvious route.  `loop.c` is pass 08 and would hoist an
 *      in-loop invariant into its own pseudo, which is the documented source of
 *      a surviving copy; but gcse's cprop has already collapsed the in-loop
 *      symbol_ref into the pre-loop pointer by then, so loop.c never sees two.
 *   the second pointer assigned from the first inside the loop   same collapse
 *   the slot table reached as a struct subscript rather than a
 *     cast through a byte pointer                                inert
 *   the slot table reached as a bare array subscript             two shorter
 *   a local copy of the slot argument, before and after the
 *     early return, crossed with two site combinations           all inert
 * AND ONE POSITIVE FINDING ABOUT THE EXISTING BODY: the ORDER of the three
 * initialisers matters.  The body below writes the pointer, then the index,
 * then the slot-table pointer, and that is ONE BETTER than writing both
 * pointers before the index.  Do not tidy it.
 *
 * NEXT: the dimension nobody has varied is not another name for the base -- all
 * of those are now measured.  It is whether the two pseudos can be made to
 * CONFLICT differently, since what the allocator needs is a live-range overlap
 * it does not have, not a second spelling.  The ROM's master pseudo is the HIGH
 * one, so the question is what makes the pool-loaded pseudo the one with FEWER
 * references and the copy the one with more.
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
