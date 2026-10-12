/* OvlFunc_959_2008d54 -- asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_a_a.s
 *
 * NON-MATCHING, 13 differing STREAM encodings of 55  (ref 55, ours 55;
 *   16-bit INSTRUCTIONS 49 on BOTH sides, so this is a TRUE DISTANCE).
 *   Relocations clean.  First difference at index 5.
 *   RE-DERIVED batch 338 brief D -- unchanged, and the figure now has an
 *   ARITHMETIC BOUND rather than a description.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/overlays/2008d54.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_a_a.s --func OvlFunc_959_2008d54
 *
 * PINS 0.  DEVICES 0.
 * SHIP SHAPE, AND A CORRECTION TO ANY BRIEF THAT GUESSES FROM THE PATH.
 *   This park lives in `src/non_matching/overlays/` rather than a bank-named
 *   directory, which usually means its piece spans several overlays.  IT DOES
 *   NOT HERE.  tools/piece.py resolves ONE function in the reference;
 *   `grep -rl ovl_9dc_a_c_c_a_a_c_a_a --include='*.ld'` names exactly
 *   `overlays/rom_7e7574/overlay.ld`; datacheck.py is silent.  It converts
 *   WHOLE -- no split, no exports, no ovl_common fan-out.  The directory is a
 *   park-filename artifact and nothing more.
 *
 * ====================================================================
 * BATCH 338 BRIEF D -- THE BLOCKER IS allocno_compare AND THE GAP IS 90
 * ====================================================================
 *
 * The header below is exactly right about the shape: the whole residue is that
 * the ROM loads the table's first word into r7 and the second into r6 and we
 * do the reverse.  13 differences, every one an r6/r7 transposition, nothing
 * else in 49 instructions.  What it did not have is WHICH DECISION, and that
 * is now closed with numbers.
 *
 * THE PSEUDOS.  `.17.lreg`:
 *     Register 35  used 7 times across 32 insns; crosses 3 calls   <- `a`
 *     Register 36  used 6 times across 28 insns; crosses 3 calls   <- `b`
 *     Register 37  used 3 times across 15 insns; crosses 1 call    <- `c`
 * `.18.greg` prints `;; 3 regs to allocate: 35 36 37` -- and that list is
 * printed from `allocno_order` AFTER the qsort (`global.c:542` then `:547`),
 * so it IS the allocation order.  Result: `35 in 6  36 in 7  37 in 5`.
 * The ROM wants 35 in r7 and 36 in r6, i.e. 36 ALLOCATED FIRST.
 *
 * THE ARITHMETIC, `allocno_compare`, global.c:598-620:
 *     pri = (int)((double)(floor_log2 (n_refs) * n_refs) / live_length
 *                 * 10000 * size)
 *   35:  floor_log2(7) * 7 / 32 = 14/32 = 0.4375    -> 4375
 *   36:  floor_log2(6) * 6 / 28 = 12/28 = 0.42857   -> 4285
 *   37:  floor_log2(3) * 3 / 15 =  3/15 = 0.2       -> 2000
 *   `pri2 - pri1` is -90, so 35 sorts first, then 36, then 37 -- THREE FOR
 *   THREE against the dump.  (n_refs is the dump's "used N times" number
 *   itself, NOT uses + sets: local-alloc prints REG_N_REFS there and REG_N_SETS
 *   separately as "set N time".  Reading it as uses+sets predicts 35,37,36 and
 *   disagrees with the dump, which is how this was pinned down.)
 *
 * SO THE GAP IS NINETY PARTS IN 4375, AND THERE ARE EXACTLY FOUR ONE-STEP
 * ESCAPES.  Any one of these flips the order and should give 0:
 *     live_length(a) >= 33   ->  14/33 = 4242 < 4285
 *     live_length(b) <= 27   ->  12/27 = 4444 > 4375
 *     n_refs(b)      == 7    ->  14/28 = 5000 > 4375
 *     n_refs(a)      == 6    ->  12/32 = 3750 < 4285
 * TWO OF THEM ARE ONE INSN OF LIVE RANGE.  This is the narrowest bound this
 * park has ever had, and it is checkable in one dump.
 *
 * WHY NOT r4, since REG_ALLOC_ORDER is {3,2,1,0,12,14,4,5,6,7,...}?
 * Because the production flags include **-fcall-used-r4** (tryc.CFLAGS), so r4
 * is in `call_used_reg_set` and `find_reg` copies that into `used1` for any
 * pseudo with `calls_crossed != 0` (global.c:1033-1038).  r5 is a hard
 * conflict for 35 in the dump.  And `global.c:1015` ORs the COMPLEMENT of
 * `regs_used_so_far` into `used` on pass 0, so the first pass will only hand
 * out a register the function already touches -- r6 and r7 are already live
 * because the prologue uses r7 to save r8.  That is the whole of "why r6
 * before r7": reg_alloc_order, among the already-used callee-saved pair.
 *
 * MEASURED, AND ALL 28 SPELLINGS ARE EXACTLY INERT AT 13 WITH COUNTS EQUAL.
 * This is a CLASS result, not 28 near misses: for four of them I also dumped
 * `.17.lreg` and `.18.greg` and the allocno numbers are BIT-IDENTICAL to the
 * base -- `35 used 7 across 32`, `36 used 6 across 28`, `3 regs to allocate:
 * 35 36 37`, `35 in 6  36 in 7  37 in 5`.  gcc normalises every one of these
 * before local-alloc ever sees them.
 *   `b - 0x2d` written `c - 1`                        13, dump identical
 *   `b - 0x2b` written `c + 1`                        13
 *   both of the above at once                         13
 *   the 3rd call's `c` written `b - 0x2c`             13, dump identical
 *   the 4th call's `c` written `b - 0x2c`             13
 *   `c = b - 0x2c` computed before the first call     13
 *   `a1 = a + 1` named before the 2nd call            13, dump identical
 *   named locals for the 1st call's two stack args    13, dump identical
 *   declaration order b/a/c and c/a/b                 13
 *   `off` declared last / `off` unsigned              13
 *   `tb` typed `int *`                                13
 *   `off = off + 4` for `off += 4`                    13
 *   `off = n << 3` for `n * 8`                        13
 *   an anchor inside the `if (n == 1)` arm            13
 *   `bb = b;` copy carrying all three subtracts       13
 *   `bb = b;` copy carrying the two stack args        13
 *   `aa = a;` copy carrying the if-arm's two uses     13
 *   >> THE COPY ROUTE IS CLOSED: gcc's copy propagation deletes `aa`/`bb`
 *   >> before local-alloc, so an extra pseudo cannot be manufactured this way,
 *   >> and the `allocno[].n_refs` summing that a SHARED allocno would give
 *   >> (the `+%d` suffix in dump_conflicts, global.c:1882-1885) is unreachable
 *   >> from a plain local copy.
 * MEASURED WORSE:
 *   an anchor after `a`'s load / between the two loads             18
 *   an anchor after `b`'s load                                    16
 *   an anchor after `c = b - 0x2c`                                15
 *   `e = b - 0x2b` hoisted above the `if`      28, one instruction short
 *   `a` re-read from the table in the if-arm    63, ten instructions long
 * AUTOSWEEP (batch 338): both axes CLOSED.  All eight flag groups inert or
 * worse (`-ffixed-r7` 59, `-O1` and `-fno-schedule-insns2` 35); all 11 anchor
 * placements inert at 13.  The body has no nested block with its own
 * statements, so for this park the depth-1 anchor axis IS the whole axis.
 *
 * NEXT: one of the four numeric escapes above.  Everything that moves a
 * reference between `a`, `b` and `c` at the SOURCE level is now measured shut,
 * so the move has to come from somewhere gcc cannot normalise -- a genuinely
 * different program shape for the two table reads, not a respelling of this
 * one.
 *
 * ===== CARRIED FORWARD: the batch-319 header, verbatim apart from the
 * ===== [SUPERSEDED FIGURE] markers, which stop parkcheck reading an
 * ===== old figure as this park's claim.
 *
 *
 * [SUPERSEDED FIGURE] 13 differing / 55 encodings  (MEASURED, batch 319 recipe backfill).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/overlays/2008d54.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_a_a.s --func OvlFunc_959_2008d54
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * OvlFunc_959_2008d54 -- asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_a_a.s
 *
 * BLOCKER: register-role swap. 54 lines against 54, 13 differing, and the
 * whole disagreement is that the ROM loads the table's first word into r7 and
 * the second into r6, while we do the reverse. Everything else -- both
 * six-argument calls, the three derived offsets, the conditional fourth call --
 * matches.
 *
 * SETTLED, and this is the transferable part. It took 18 differing to 13:
 *
 *   The ROM reads the two table words with `ldr r7, [r2, r3] / add r3, #4 /
 *   ldr r6, [r2, r3]` -- one base register, an offset that is MUTATED between
 *   the loads. Writing the obvious `t = (int *)(L773c + n * 8); a = t[0];
 *   b = t[1];` gives gcc a single pointer and immediate offsets (`ldr [r3]`,
 *   `ldr [r3, #4]`), which is a different shape entirely.
 *
 *   Keeping the offset in its own int and incrementing it --
 *
 *       off = n * 8;
 *       a = *(int *)(tb + off);
 *       off += 4;
 *       b = *(int *)(tb + off);
 *
 *   -- reproduces the register-offset pair exactly. The `tb` pointer local is
 *   also required; with the array name used directly gcc folds the base and
 *   emits the operands transposed.
 *
 * TRIED AND REJECTED:
 *
 *   * Swapping the declaration order of `a` and `b`, on the theory that
 *     callee-saved assignment follows declaration order. NO CHANGE.
 *  
 */
extern unsigned char L773c[] __asm__(".L773c");
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_959_2008d54(int n)
{
    unsigned char *tb;
    int off;
    int a;
    int b;
    int c;

    tb = L773c;
    off = n * 8;
    a = *(int *)(tb + off);
    off += 4;
    b = *(int *)(tb + off);
    __Func_80105d4(0, 0x4d, 1, 3, a, b);
    __Func_80105d4(1, 0x4d, 1, 1, a + 1, b);
    c = b - 0x2c;
    __Func_8010704(a, b - 0x2d, 1, 1, a, c);
    if (n == 1)
        __Func_8010704(a, c, 1, 1, a, b - 0x2b);
}
