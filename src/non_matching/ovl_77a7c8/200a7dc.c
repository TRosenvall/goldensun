/* OvlFunc_881_200a7dc -- NON-MATCHING.
 *
 * NON-MATCHING, 7 of 31 encodings  (MEASURED, batch 338 brief B).
 *   Was 12 of 31.  INSTRUCTION COUNTS AGREE -- ref 30, ours 30, stream 31/31,
 *   so this is a DISTANCE, not phase.  Relocations identical, dsize 0.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77a7c8/200a7dc.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_c_a.s --func OvlFunc_881_200a7dc
 *
 * Pin-free (tools/shimcount.py silent).  The piece holds this function alone
 * (tools/piece.py: 1 function), tools/datacheck.py is silent, so a landing is a
 * whole-file conversion to
 * src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_c_a.c with NO split.
 *
 * A scan over 12-byte records for one whose first word is 2 and whose halfword
 * at +4 is 0x8a, stopping at a first word of -1.
 *
 * ========== BATCH 338: 12 -> 7, AND THE BODY GOT SIMPLER ==========
 *
 * Two of the park's own levers were COSTING five encodings, and dropping them
 * removed two locals at the same time.  The 12-of-31 body carried
 *   -  `w = base;` plus a store through `w`, and
 *   -  `neg1 = 1; ... neg1 = -neg1;` with the sentinel comparing `neg1`.
 * Both are inert-or-worse once the OTHER two axes below are right, and the
 * header's own dump reading says why `w` was always inert: gcse cprops it, so
 * the `mov r7, r0` in the ROM is NOT that copy -- it is loop.c hoisting the
 * IN-LOOP SYMBOL REFERENCE into its own pseudo, which cse2 then rewrites to a
 * register copy.  Only ONE access may name the symbol in the loop.
 *
 * MEASURED, 180 variants crossing five axes in one container
 * (`scratch_elev/b338/B/b_gen_p1.py`, results in `logs/p1_sweep1.txt`):
 *   store address    {base, w, in-loop symbol}
 *   sentinel         {symbol, base, base-as-long, w, w-as-long}
 *   loop-top read    {base, symbol, base-as-long}
 *   setup order      {k before one, one before k}
 *   the -1           {explicit neg1 local, plain literal -1}
 *
 *   * THE LITERAL -1 IS WORTH 2 and `one` BEFORE `k` IS WORTH 3.  The two
 *     cross cleanly: 12 (k-first + neg1 local) -> 10 (one-first + neg1 local)
 *     -> 9 (k-first + literal) -> 7 (one-first + literal).  `one` before `k`
 *     is what puts **k in ip**, which is the ROM's choice: the two constants
 *     TIE in allocno_compare (both 2 refs, both 32 insns live -- 1*2/32*10000
 *     = 625 each, global.c:598-620) and the tie falls to the lower pseudo
 *     number, which is the one assigned FIRST in the setup block.  ip is
 *     reachable at all because REG_ALLOC_ORDER (arm.h:989) is
 *     `3,2,1,0,12,14,4,5,6,7` and `k`'s only uses are a move and a store, so
 *     its class admits HI_REGS.
 *   * NAMING THE SYMBOL AT THE STORE IS 22-34, NOT 7, and some spellings add a
 *     SECOND POOL WORD (dsize 4, RELOCDIFF).  loop.c strength-reduces the
 *     store address into a third induction variable.  So the hoist must sit on
 *     a READ.  Which read does not matter: symbol-at-sentinel and
 *     symbol-at-loop-top both read 7.
 *   * STORE THROUGH `base` AND THROUGH `w` ARE BYTE-IDENTICAL at every point
 *     of the cross -- `w` is dead source text.  Dropped.
 *   * THE ALIAS SPLIT IS NO LONGER NEEDED.  `long` on either read is inert at
 *     7 (it was load-bearing at 13).  The in-loop symbol alone keeps the
 *     second `ldr`.
 *
 * autosweep (batch 338): BOTH CHEAP AXES CLOSED on the 12-of-31 body -- 8 flag
 * groups and 9 anchor placements, best 12.  Re-crossed the flag axis against
 * the NEW bodies as well: -fno-strength-reduce, -fno-gcse, -fno-rerun-loop-opt
 * and -fno-rerun-cse-after-loop are all inert at 7, and -fno-schedule-insns2
 * is 12.  One crossing result worth keeping: -fno-gcse is **32 with a
 * RELOCDIFF** on the old body and **inert at 7** on this one, i.e. this body no
 * longer depends on gcse to keep the sentinel load.
 *
 * ========== WHAT THE REMAINING 7 ARE: THREE CONDITIONS, NOT ONE ==========
 *
 *               neg1  ldrsh-zero  one   k     hoisted copy
 *      rom      r4    r5          r6    ip    r7
 *      ours     r4    r6          r7    ip    r5
 *
 * neg1 and k are now the ROM's.  The other three move together, and they are
 * ONE fact about r5:
 *
 *   (1) THE ldrsh ZERO IS A RELOAD IN OUR BUILD AND MUST BE A BLOCK-LOCAL
 *       PSEUDO.  Thumb `ldrsh` has no immediate-offset form, so the index 0
 *       needs a register.  In our build no pseudo carries it -- it is absent
 *       from .17.lreg and .18.greg and appears only in the final asm -- so it
 *       is picked by reload AFTER global alloc, from whatever is left, and
 *       that is r6.  In the ROM it is r5, i.e. it was claimed BEFORE the three
 *       globals were placed.  That is exactly the local-alloc-first mechanism
 *       that landed OvlFunc_946_200add0 this batch (see
 *       src/overlays/rom_7ced6c/...200add0, and FINDINGS for batch 338 B):
 *       a value whose uses are confined to one basic block is allocated by
 *       local-alloc (local-alloc.c:1341-1420) and global.c then sees the
 *       register as a hard conflict.
 *   (2) WITH r5 BLOCKED the two remaining LO globals fall to r6 and r7 in
 *       REG_ALLOC_ORDER, which is the ROM's `one` -> r6, copy -> r7 -- PROVIDED
 *       the copy is the LOWER priority of the two.  It is not today: the copy
 *       has 3 refs over 22 insns (1*3/22*10000 = 1363) against `one`'s 625, so
 *       it is placed first.  The copy must be reduced to 2 refs (one set, one
 *       use) to tie at 625, after which the pseudo-number tie-break orders
 *       them.
 *   (3) The ROM's copy serves the STORE and ours serves a READ.  That follows
 *       from (2) -- a copy with a single use.
 *
 * SO THE NEXT PROBE IS NOT ANOTHER POINTER SPELLING.  It is: find a spelling
 * of the halfword compare whose zero index is a pseudo local to the
 * then-block.  Every spelling tried reads 7 or worse and none creates one:
 * `*(short *)p`, `*(short *)(p + 0)`, `*(short *)(p - 0)` all 7;
 * `*(short *)(off + (int)base + 4)`, `*(short *)(4 + off + (int)base)` and
 * `*(short *)(gOvl_0200e3f4 + off + 4)` are all 32 with an extra pool word,
 * because naming base or the symbol a third time inside the loop hoists again.
 *
 * MEASURED INERT OR WORSE, CARRIED FORWARD FROM BATCH 334 (all figures are
 * about the 13-of-31 body and a negative is a measurement of ONE BODY):
 *   * `w = gOvl_0200e3f4;` as a second symbol reference in the SETUP block is
 *     byte-identical to `w = base`; cse commons the two pool loads inside the
 *     one entry block.  Naming the symbol twice only pays INSIDE THE LOOP.
 *   * `off + base` / `off + w` as POINTER arithmetic: byte-identical.  The C
 *     front end rebuilds it pointer-first; only the integer cast flips the rtl.
 *   * a declaration-order sweep, eight orders including `off` first: all 13.
 *     (Batch 338 note: that swept DECLARATIONS.  It is the ASSIGNMENT order of
 *     `one` and `k` that pays, and it had never been tried.)
 *   * `unsigned int` for the sentinel read: byte-identical, same alias set.
 *   * two distinct struct types for the two reads: byte-identical.  A
 *     COMPONENT_REF's alias set here is the field type's, so it does not split.
 *   * `volatile int` on both reads reads ours 31 instructions: it buys the load
 *     and the copy but loop.c then strength-reduces the store address into a
 *     third induction variable, so it overshoots.
 */
extern char gOvl_0200e3f4[];

void OvlFunc_881_200a7dc(void)
{
    char *base;
    char *p;
    int off;
    int k, one;

    base = gOvl_0200e3f4;
    one = 1;
    k = 0x21;
    p = base + 4;
    off = 0;
    for (;;) {
        if (*(int *)(off + (int)base) == 2 && *(short *)p == 0x8a) {
            *(int *)(off + (int)base) = one;
            *(int *)(p + 4) = k;
            return;
        }
        if (*(int *)(gOvl_0200e3f4 + off) == -1)
            return;
        p += 0xc;
        off += 0xc;
    }
}
