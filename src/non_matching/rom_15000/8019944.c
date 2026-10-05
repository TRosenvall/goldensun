/* Func_8019944 (0x08019944) -- NON-MATCHING, 5 of 41 encodings.
 *
 *   SIZE IS EXACT: 88 bytes both sides.  ENCODING COUNT IS EXACT: 41 = 41.
 *   RELOCATIONS ARE EXACT (objcmp prints no RELOCATIONS line).  NO PINS,
 *   NO FLAGS, NO SYMBOLS.  Every encoding from ref index 11 onward is
 *   byte-identical; the whole residue is a permutation of five prologue insns.
 *
 *   WAS 35 of 41 at 84 bytes against 88 with a shifted relocation (batch 319
 *   backfill figure, re-derived in batch 324 and confirmed before changing
 *   anything).  35 -> 17 -> 5 in batch 324.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8019944.c \
 *     asm/rom_15000/rom_1908c_c_a_c_c_c_c_a.s --func Func_8019944
 *
 * SPLIT SHAPE: none needed for a park.  tools/datacheck.py on
 * asm/rom_15000/rom_1908c_c_a_c_c_c_c_a.s reports no data section and no
 * required data export; the file holds this one function
 * (`grep .thumb_func_start` -> 1).  tools/shimcount.py: NONE -- PIN-FREE.
 *
 * ========================================================================
 * THE OLD DIAGNOSIS WAS WRONG.  ALL THREE PARTS OF IT.
 * ========================================================================
 * The previous header said the blocker was "a held zero the source cannot
 * force into a register", that "gcc folds `z` back to an immediate at both
 * stores", that "a named constant cannot create register pressure", and
 * "NEXT: nothing source-level."  Measured in batch 324:
 *
 *   * `z` was ALREADY in a register in that very body (`movs r7,#0` feeding
 *     `str r7,[r4,r2]` / `strh r7,[r5,r2]`).  There was no fold and no held-
 *     zero blocker.  With the shape below it lands in `ip` (r12) exactly as
 *     the ROM does -- `mov ip,rN` then `mov r3,ip` twice.
 *   * The 35 was never one problem.  It was THREE, and two of them were
 *     EXACTLY INERT on their own, which is why one-at-a-time testing had
 *     found nothing:
 *       (1) the ROM is 38 insns and that body was 35 -- so 35 measured
 *           MISALIGNMENT, not distance;
 *       (2) `bgt` against the ROM's `bhi`;
 *       (3) the o1/o2 base registers swapped.
 *
 * ========================================================================
 * THE THREE EDITS, WITH FIGURES
 * ========================================================================
 * 1. ONE EXIT, NOT THREE.  The ROM keeps `ret` in the callee-saved r6,
 *    initialised to 0 at the top, and has a single exit block
 *    `adds r0,r6,#0 / pop {r5,r6,r7} / pop {r1} / bx r1`.  Returning from
 *    three places instead let gcc keep `ret` in the return register r0 and
 *    rematerialise a `movs r0,#0 / b` block for the i>7 path -- 35 insns
 *    against 38 and a 4-byte size deficit.  A `for (;;)` whose two exits are
 *    `break`, with one `return ret;` after the if/else, is the ROM's shape.
 *    (`goto done` measures identically; `break` is the better C.)
 * 2. THE BOUND IS UNSIGNED.  `cmp r1,#7 / bhi` is an UNSIGNED compare, so `i`
 *    is `unsigned int`.  The old header listed this correctly under WHAT IS
 *    RIGHT while the body declared `int i` and emitted `bgt`.
 * 3. o2 IS INITIALISED BEFORE o1.  See below -- worth 12 encodings.
 *
 * MEASURED, AND THIS IS THE CROSSING LAW IN ITS PUREST FORM:
 *   park body                                         35  (84 b, reloc shifted)
 *   park body + `unsigned int i` ALONE                35  EXACTLY INERT
 *   single-exit restructure ALONE (`int i`)           35  EXACTLY INERT
 *   BOTH TOGETHER                                     17  (88 b, reloc exact)
 *   both + o2 initialised before o1                    5
 * Neither edit survives one-at-a-time screening.  Together they are the
 * function.
 *
 * ========================================================================
 * WHY THE INITIALISATION ORDER OF o1 AND o2 IS WORTH 12 ENCODINGS
 * ========================================================================
 * At 17 the shape, the length, the pool and the relocation were already the
 * ROM's.  Nine of the 17 were o1 (the 0x12dc halfword base) and o2 (the
 * 0x12bc word base) holding each other's hard register: the ROM puts o1 in
 * r4 and o2 in r0, we had the reverse.
 *
 * `.18.greg` prints `;; 8 regs to allocate: 34 36 37 35 32 38 33 39`, so
 * N > 0 and this is GLOBAL allocation -- the priorities are exact, not a
 * ranking.  Dispositions were `32 in 5  33 in 7  34 in 2  35 in 0  36 in 4
 * 37 in 1  38 in 6  39 in 12`: key=r5, flag=r7, blk=r2, o1=r0, o2=r4, i=r1,
 * ret=r6, z=ip.  `find_reg` hands out r2 then r4 in priority order (r0/r1 are
 * skipped in pass 0 because `32 preferences: 0`, `33 preferences: 1` and
 * `38 preferences: 0` claim them), so WHICHEVER BASE IS ALLOCATED FIRST TAKES
 * r4, and the one after it falls through to pass 1 and takes r0.
 *
 * `allocno_compare` (`global.c:597-620`) is
 *     pri = (floor_log2 (n_refs) * n_refs / live_length) * 10000 * size
 * -- note there is NO frequency or loop-depth term in this compiler.
 * `.17.lreg` gave o1 `10 refs across 44 insns` and o2 `9 refs across 32`;
 * `floor_log2(10) == floor_log2(9) == 3`, so live_length alone decided it,
 * and o1's range was longer ONLY because its initialisation stood earlier in
 * the source.  Swapping the two assignments swaps the live lengths, swaps the
 * allocno order, and swaps r0/r4.  17 -> 5.
 *
 * Exactly inert (candidate prerequisites, not dead ends): three declaration
 * orders; `flag` for `flag != 0`; `z` used in the peeled block too.
 * Worse: reversing the key comparison (7); every other initialisation
 * permutation (12, 13, 13, 15, 15, 19, 37).
 *
 * ========================================================================
 * THE REMAINING 5, AND THE MECHANISM THAT HOLDS IT
 * ========================================================================
 * Ref indices 6-10 are a PERMUTATION of the same five insns:
 *   ROM   adds r7,r1,#0 | movs r1,#0 | movs r6,#0 | mov ip,r1 | ldr r0,=0x12bc
 *   ours  movs r6,#0 | adds r7,r1,#0 | ldr r0,=0x12bc | movs r1,#0 | mov ip,r6
 * It reduces to ONE question: which zero-holding register reload picks as the
 * source of the `mov ip,rN` that `z` needs, because r12 cannot take a Thumb
 * immediate.  `.23.sched2` at t=9 has ready list `19 6 25` and takes 25:
 *     insn 25  `movs r6,#0`     prio 2   <- ours
 *     insn  6  `adds r7,r1,#0`  prio 1   <- the ROM's choice
 * and 25 carries prio 2 ONLY because insn 28 (`mov ip,r6`) depends on it.
 * Had ip taken r1 instead, 25 drops to 1, three insns tie at 1, and the lower
 * rungs produce the ROM's order.
 *
 * MECHANISM, read in the compiler: `reload_cse_simplify_set`
 * (`reload1.c:8003-8058`) walks the cselib equivalence list `val->locs` and
 * takes the first entry that is cheaper, or equal-cost and a REG; and
 * `simplify-rtx.c:3088` does
 *     src_elt->locs = new_elt_loc_list (src_elt->locs, dest);
 * which PREPENDS, so the head of that list is the MOST RECENTLY SET register
 * holding the value.  `ret = 0;` is the statement immediately before `z = 0;`,
 * so ip gets r6.
 *
 * THIS IS A CROSSED CONSTRAINT, NOT AN UNEXPLORED LEVER.  Putting `i = 0;`
 * immediately before `z = 0;` does make ip take r1 -- and costs 10 elsewhere
 * (measured 15), because the same statement position also reverses i's and
 * ret's allocno order.  One position controls both.
 *
 * NEXT: the five-insn permutation needs the `mov ip` source changed WITHOUT
 * moving `i = 0;`.  Nothing else in this function is open.
 */
extern int iwram_3001e8c;

int Func_8019944(int key, int flag)
{
    int blk;
    int o1;
    int o2;
    unsigned int i;
    int ret;
    int z;

    blk = iwram_3001e8c;
    o2 = 0x12bc;
    i = 0;
    ret = 0;
    z = 0;
    o1 = 0x12dc;
    if (*(unsigned short *)(o1 + blk) == key) {
        ret = *(int *)(o2 + blk);
        if (flag != 0) {
            *(int *)(o2 + blk) = 0;
            *(unsigned short *)(o1 + blk) = 0;
        }
    } else {
        for (;;) {
            i++;
            o2 += 4;
            o1 += 2;
            if (i > 7)
                break;
            if (*(unsigned short *)(o1 + blk) == key) {
                ret = *(int *)(o2 + blk);
                if (flag != 0) {
                    *(int *)(o2 + blk) = z;
                    *(unsigned short *)(o1 + blk) = z;
                }
                break;
            }
        }
    }
    return ret;
}
