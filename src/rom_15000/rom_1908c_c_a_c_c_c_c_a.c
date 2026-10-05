/* Cluster Func_8019944..Func_8019944 extracted from goldensun/asm/rom_15000/rom_1908c_c_a_c_c_c_c_a.s.
 *
 * LANDED in batch 326 (brief B) from a park reading 5 of 41.  Byte-identical:
 * 88 bytes, 41 encodings, 1 relocation, both --func and --whole.  The reference
 * .s holds this one function (one .thumb_func_start) and tools/datacheck.py
 * reports no data section, so NO SPLIT is needed.  No pins, no flags, no
 * devices, no new symbol-table entries.
 *
 * VERIFY: python3 tools/objcmp.py src/rom_15000/rom_1908c_c_a_c_c_c_c_a.c asm/rom_15000/rom_1908c_c_a_c_c_c_c_a.s --func Func_8019944
 *
 * HISTORY: 35 of 41 (batch 319 backfill) -> 17 -> 5 (batch 324) -> 0 (batch 326).
 *
 * ======================================================================
 * FIVE LEVERS.  The last two are a CROSSED PAIR and that is the finding.
 * ======================================================================
 *
 * 1. ONE EXIT, NOT THREE.  `ret` lives in the callee-saved r6, is zeroed at the
 *    top, and the function has a single exit block.  Returning from three places
 *    instead lets gcc keep `ret` in the return register r0 and rematerialise a
 *    `movs r0,#0 / b` block for the i>7 path -- 35 insns against 38.  A `for(;;)`
 *    whose two exits are `break`, with one `return ret;` after the if/else, is
 *    the ROM's shape.  (`goto done` measures identically; `break` is better C.)
 *
 * 2. THE BOUND IS UNSIGNED.  `cmp r1,#7 / bhi` is an unsigned compare, so `i` is
 *    `unsigned int`.  With `int i` it is `bgt`.
 *
 *    Levers 1 and 2 are THEMSELVES a crossed pair, measured in batch 324:
 *    `unsigned int i` alone is exactly inert at 35; the single-exit restructure
 *    alone is exactly inert at 35; TOGETHER they are 17.
 *
 * 3. `o2` IS INITIALISED AFTER `o1`... see lever 4; the init block's order is
 *    load-bearing in three independent ways at once.
 *
 * 4. STATEMENT ORDER: `blk; ret = 0; i = 0; z = 0; o2; o1;`
 * 5. DECLARATION ORDER: `blk; i; o1; o2; ret; z;`  (the loop counter SECOND)
 *
 *    **Lever 4 alone is 12 -- WORSE than the 5 it replaces.  Lever 5 alone is
 *    EXACTLY INERT at 5.  Together they are 0.**  The park had already recorded
 *    "three declaration orders: exactly inert", which is why the pair was never
 *    crossed, and that is the lesson:
 *
 *      A TIE-BREAK LEVER IS INVISIBLE UNTIL SOMETHING ELSE MANUFACTURES THE TIE.
 *
 *    Declaration order reaches the allocator ONLY through the last rung of
 *    allocno_compare (global.c:617, `return v1 - v2`, the allocno index), and
 *    that rung is dead whenever the priority arithmetic above it separates the
 *    two allocnos.  Lever 4 is what makes two priorities exactly equal; lever 5
 *    is what then decides them.  Neither can be screened one at a time.
 *
 * ======================================================================
 * WHY THE INIT ORDER IS PINNED FROM BOTH ENDS -- TWO PASSES, OPPOSITE WAYS
 * ======================================================================
 *
 * The residue at 5 was five prologue insns in the wrong order, and it reduces to
 * two independent questions about the same three statements.
 *
 * (a) WHICH ZERO-HOLDING REGISTER FEEDS `mov ip,rN`.  r12 cannot take a Thumb
 *     immediate, so `z` reaches ip through a copy.  reload_cse_simplify_set
 *     (reload1.c:8003-8058) walks cselib's `val->locs` and takes the first entry
 *     that is cheaper, or equal-cost and a REG; simplify-rtx.c:3088 does
 *         src_elt->locs = new_elt_loc_list (src_elt->locs, dest);
 *     which PREPENDS.  So the head is the MOST RECENTLY SET register holding 0,
 *     and `i = 0;` must be the last zero-set before `z = 0;`.  Hence `ret = 0;`
 *     moves ABOVE `i = 0;`.  (Moving `ret = 0;` BELOW `z = 0;` instead costs
 *     eight whole instructions -- 37 of 41 at 33 insns against 41 -- because it
 *     dissolves the single-exit shape of lever 1.  Measured twice.)
 *
 * (b) WHICH OF `mov ip,r1` AND `ldr r0,=0x12bc` ISSUES FIRST.  rank_for_schedule
 *     (haifa-sched.c:4029) at t=12 of block 0 has the ready list EXACTLY
 *     `28 25`, with insn 28 = `(set (reg ip) (reg r1))` and insn 25 =
 *     `(set (reg r0) (const_int 4796))`.  Priorities tie 1/1; both classify 3
 *     against the just-issued insn 19; both have the single dependent 35.  All
 *     three rungs above the bottom are therefore dead and the tie falls to
 *         return INSN_LUID (tmp) - INSN_LUID (tmp2);          (haifa-sched.c:4112)
 *     where the LOWER LUID wins.  So `z = 0;` must precede `o2 = 0x12bc;`.
 *
 * (a) and (b) together force `ret = 0; i = 0; z = 0; o2 = 0x12bc;` -- and THAT
 * order is what creates the allocno tie that lever 5 then breaks.  A full sweep
 * of all 120 statement orders satisfying (a) FLOORS AT 2 (twelve orders tie at
 * 2, four at 3): the order dimension alone cannot pay for this, which is exactly
 * what the park concluded.  What pays for it is the allocno INDEX.
 *
 * ======================================================================
 * THE ALLOCNO ARITHMETIC, READ OUT OF .17.lreg AND .18.greg
 * ======================================================================
 *
 * Pseudos are numbered in DECLARATION order.  With the shipped order:
 * 32=key 33=flag 34=blk 35=i 36=o1 37=o2 38=ret 39=z.
 *
 * allocno_compare (global.c:597-620) is
 *     pri = (floor_log2 (n_refs) * n_refs / live_length) * 10000 * size
 * with NO frequency or loop-depth term -- the weighting is already in n_refs,
 * because all four REG_N_REFS increment sites in flow.c (:4435, :4948, :5115,
 * :5556) add `pbi->bb->loop_depth + 1`.  So n_refs is read from .17.lreg's
 * `used N times across M insns` line and never counted in the C.  Ties break on
 * the allocno index at :617.
 *
 * The decisive table, every row's printed order reproduced by hand from
 * .17.lreg (o1/o2/i named by their role, not their pseudo number, because the
 * numbering MOVES with lever 5):
 *
 *   body                       o1          o2          i          printed order
 *   park base (5)           10/36=8333   9/40=6750  9/34=7941     blk o1 i  o2
 *   `ret` and `i` swapped   10/36=8333   9/40=6750  9/32=8437     blk i  o1 o2  <- i overtakes o1
 *   `i; o2; z` (2)          10/36=8333   9/36=7500  9/34=7941     blk o1 i  o2
 *   `i; z; o2` (12)         10/36=8333   9/34=7941  9/34=7941     blk o1 o2 i   <- EXACT TIE
 *   the same + lever 5 (0)  10/36=8333   9/34=7941  9/34=7941     blk o1 i  o2  <- tie to i
 *
 * In the 12 the tie goes to `o2` purely because `o2` was declared before `i`, so
 * `o2` takes r1 and `i` takes r0 -- the ROM's pair, reversed.  Declaring `i`
 * first lowers its index below `o2`'s and the same tie resolves the other way:
 * `i` in r1, `o2` in r0, `o1` in r4, as the ROM has them.  The deciding margin
 * in the rows above it is 8333 against 8437, 1.2 percent.
 *
 * SIX declaration orders land it, and the discriminator is nothing more than
 * "is `i`'s pseudo number below `o2`'s": blk-i-o1-o2-ret-z (shipped),
 * blk-o1-i-o2-ret-z, blk-i-o2-o1-ret-z, blk-o1-i-o2-z-ret, i-blk-o1-o2-ret-z,
 * i-o1-o2-blk-ret-z.  The two that keep `o2` ahead of `i` are exactly inert at
 * 12.  The shipped one puts the counter next to the block pointer.
 *
 * MEASURED INERT OR WORSE (all at equal encoding and instruction counts unless
 * noted): the 120-order statement sweep floors at 2, twelve ways, then 3 x4,
 * 5 = the old park, 12 x6, 13, 14 x3, 15 x3, 18.  `ret = 0;` after `z = 0;` or
 * after `o1` -- 37 at 33 insns, RELOC, MEM (eight instructions lost).
 * From batch 324, still true: reversing the key comparison 7; `flag` for
 * `flag != 0` exactly inert; `z` used in the peeled block too, exactly inert.
 */
extern int iwram_3001e8c;

int Func_8019944(int key, int flag)
{
    int blk;
    unsigned int i;
    int o1;
    int o2;
    int ret;
    int z;

    blk = iwram_3001e8c;
    ret = 0;
    i = 0;
    z = 0;
    o2 = 0x12bc;
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
