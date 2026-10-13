/* OvlFunc_896_200c260 -- 0x0200c260 -- PARK at 2 differing encodings of 85
 *   (WAS 3).  PIN-FREE, DEVICE-FREE, FLAG-FREE.  BODY CHANGED THIS BATCH.
 *
 * ===== BATCH 349 BRIEF D: 3 -> 2, BY INSTALLING THE PARK'S OWN RECORDED-BUT-
 * ===== UNINSTALLED ANCHOR.  AND THE "-1 DISCOUNT" LEVER IS NOW CLOSED BY
 * ===== reload1.c's OWN abort().
 *
 * THE FOUR NUMBERS, measured this batch (not inherited):
 *   ENCODINGS           2 differing of 85   (ref 85 / ours 85), first index 6
 *   16-BIT INSTRUCTIONS 70 / 70
 *   BYTES               200 / 200
 *   POOL / 32-BIT       15 / 15
 *   RELOCATIONS         clean.   PINS 0 (tools/shimcount.py).
 * All three length dimensions agree, so the figure IS a real distance.
 * ALIGNED RESIDUE 2 in 1 hunk (tools/aligncmp.py) -- DOWN from the park's
 * 3 in 2 hunks, so this is progress on BOTH scales, not a phase artefact.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_78ef88/200c260.c \
 *     asm/overlays/rom_78ef88/ovl_314_c_c_c_c_a.s --func OvlFunc_896_200c260
 *
 * SPLIT: NONE NEEDED.  tools/piece.py: asm/overlays/rom_78ef88/ovl_314_c_c_c_c_a.s
 * holds exactly ONE function and reports NO non-function exports, so it would
 * convert whole.  tools/datacheck.py silent (exit 0).  MANIFEST "split": null --
 * this is a park, not a landing.
 *
 * ===== WHAT CHANGED: THE SECOND ANCHOR, WHICH THIS PARK HAD ALREADY MEASURED
 * ===== AND THEN DECLINED TO INSTALL
 *
 * The batch-338 note in this header reads, verbatim: "tools/autosweep.py (with
 * its nested-boundary fix) finds a single `do { } while (0);' at statement
 * boundary 4 that takes this park from 3 of 85 to 2 of 85 ... NOT installed
 * here -- the body below is unchanged and still reads 3 -- because the anchor is
 * worth recording as a MEASURED ROUTE rather than shipped for one encoding
 * before the remaining residue is understood."
 *
 * IT REPRODUCES, it is the second `do { } while (0);' below (after `z = 0;'),
 * and it is now installed.  The reason to install it rather than keep deferring:
 * it does not merely shave one encoding, it REMOVES ONE OF THE TWO HUNKS.  The
 * park's 3 were a register pair PLUS a transposition; the 2 are the register
 * pair ALONE.  Measured side by side (tools/objcmp.py encodings, disassembled):
 *
 *     idx   REF                | PARK BODY (3)      | NOW (2)
 *      5    4682 mov sl, r0    | 4682 mov sl, r0  ok| 4682 mov sl, r0  ok
 *      6    2000 mov r0, #0    | 2200 mov r2, #0  XX| 2200 mov r2, #0  XX
 *      7    4680 mov r8, r0    | 2016 mov r0, #22 XX| 4690 mov r8, r2  XX
 *      8    2016 mov r0, #22   | 4690 mov r8, r2  XX| 2016 mov r0, #22 ok
 *
 * So the CreateActor argument fill no longer wedges between the pair.  This is
 * batch 336's claim -- "index 7 is NOT a second question ... there is exactly
 * one: get r0 as the reload scratch at insn 30, and all three encodings follow"
 * -- CONFIRMED FROM THE OTHER DIRECTION: fix the schedule and the residue
 * collapses to exactly the register, with nothing else left in it.  The anchor
 * is device-free (it emits nothing and shimcount does not count it), so it
 * carries no pass-3 debt.
 *
 * ===== THE ANCHOR AXIS WAS NEVER FINGERPRINTED, AND 15 OF 27 ROWS MEASURED
 * ===== NOTHING
 *
 * This park is the one of batch 349 brief D's four that did NOT carry a
 * "BOTH MECHANICAL AXES ARE CLOSED" block -- the brief said it did, and it does
 * not; grep for the string.  So the axis was re-run here WITH object
 * fingerprinting (tools/sweep_variants.py), which autosweep.py does not do:
 *
 *     27 anchor placements swept on the park body
 *     12 BIT-IDENTICAL TO THE BASE          -- measured NOTHING
 *      3 bit-identical to another variant   -- measured nothing new
 *     12 DISTINCT PROGRAMS actually tested
 *
 * One of those 12 is the 2 above.  THE AXIS IS NOW SATURATED: a THIRD anchor was
 * swept over all 28 boundaries of the 2-body (14 bit-identical to base, 3 to
 * each other, 11 distinct programs) and NOTHING READS BELOW 2.  Worse rows, for
 * the record, all at 70/70 instructions and relocations clean: 4 (after
 * `has = ...', and after each of the three byte/word stores), 6, 7, 8, 9.
 *
 * ===== THE REMAINING 2: ONE CAUSE, AND ONE OF ITS TWO PROPOSED LEVERS IS NOW
 * ===== CLOSED BY COMPILER SOURCE
 *
 * The cause is unchanged and is stated correctly by batch 348: the reload
 * scratch for `mov r8, #0' at insn 30 is r2 where the ROM has r0.  .18.greg on
 * THIS body is byte-for-byte the park's recorded dump -- the anchor changes the
 * schedule, not the allocation:
 *     ;; 8 regs to allocate: 34 43 46 33 35 37 32 36
 *     Spilling for insn 30.  / Using reg 3 for reload 0
 *     Spilling for insn 89.  / Using reg 2 for reload 2
 *     Spilling for insn 97.  / Using reg 2 for reload 2
 *     41 in 1  43 in 3  46 in 3  48 in 2  50 in 3  53 in 2
 *     56 in 3  58 in 3  59 in 3  60 in 0  62 in 2
 *     ;; Hard regs used:  0 1 2 3 5 6 7 8 9 10 14 25 26
 * Insns 89 and 97 are the two byte stores `o[0x26] = z; o[0x27] = z;', each
 * `(set (mem:QI (reg:SI 3 r3)) (reg:QI 2 r2))' -- .19.flow2, verbatim.
 *
 * BATCH 336 PROPOSED A SECOND, CHEAPER SUFFICIENT CONDITION: "find an insn in
 * this function whose RELOAD OPERAND is hard register r0, which buys the -1
 * discount at reload1.c:1641-1646 and needs no pressure at all."  Batch 348
 * argued its PRECONDITION fails on Thumb (no insn can present r0 as a reload
 * operand, because every `l' constraint accepts r0).  That argument is sound but
 * it is not the strongest one available, and the strongest one was sitting
 * eleven lines further down the same function.
 *
 * find_reg ENDS WITH AN ASSERTION THAT THE WINNER COST ZERO -- reload1.c:1683-1685:
 *       if (spill_cost[best_reg + i] != 0
 *           || spill_add_cost[best_reg + i] != 0)
 *         abort ();
 *       SET_HARD_REG_BIT (used_spill_regs_local, best_reg + i);   [:1686]
 *
 * In any compile that does not ICE, therefore, `best_reg' ALWAYS has
 * spill_cost 0.  The two discounts at :1641-1646 can only ever move a register
 * from cost 0 to cost -1; a register they promote from cost >= 1 to winning
 * would trip this abort.  SO THE DISCOUNT CANNOT SUBSTITUTE FOR FREEING r0 --
 * it can only break a tie BETWEEN ALREADY-FREE REGISTERS.  Batch 336's "needs
 * no pressure at all" is false as stated, and the lever is closed here by the
 * allocator's own invariant rather than by a Thumb-constraint argument.
 *
 * WHAT THAT LEAVES, and it is now exact.  Since the winner must cost 0 and the
 * equal-cost tie-break is inv_reg_alloc_order over REG_ALLOC_ORDER
 * = { 3, 2, 1, 0, ... } (config/arm/arm.h:989-995), r0 is picked at an insn IF
 * AND ONLY IF each of r3, r2, r1 is, at that insn, either in `not_usable'
 * (bad_spill_regs | bad_spill_regs_global | outside the reload class), or in
 * `used_by_other_reload', or of spill_cost > 0 -- while r0 is none of those.
 * Nothing weaker reaches it.  At insn 30 (the prologue) all four of r0..r3 cost
 * 0, so r3 wins outright; at insns 89 and 97 r3 is held by the address reload
 * and r2 is simply the next free register in REG_ALLOC_ORDER, so the scan never
 * reaches r1 or r0.
 *
 * MEASURED THIS BATCH, the park's own cheap boolean screen (compile -da, collect
 * every `Using reg N' in .18.greg, print the WHOLE pick set), on the 2-body:
 *     base (this body)                                      picks {2,3}
 *     address temp `unsigned char *q = o + 0x26' for the
 *       two byte stores, stores through q[0] / q[1]          picks {2,3}
 *     the zero routed through the `o[5] &= m' mask           picks {2,3}
 *     g1's range lengthened across the byte stores           picks {2,3}
 *     the act pointer kept live past the `o' load            picks {2,3}
 *     a byte store indexed by the zero                       picks {2,3}
 *     the two byte stores PERMUTED                           picks {2,3}
 * SIX more variants, ZERO hits, which takes this direction to roughly THIRTY
 * measured variants across batches 336, 348 and 349 with no r0 anywhere.  Three
 * of the six above are greg screens only and are NOT candidate bodies (two are
 * value-preserving but device-flavoured, one is a self-assignment); they are
 * recorded because a pick set is all they were run for.
 *
 * DECLINING TO CLOSE, at 2.  The improvement is real and on both scales; the
 * remaining cause is one register; one of its two proposed levers is now closed
 * by compiler source and the other is the co-liveness condition stated above,
 * which ~30 variants have failed to produce.  The anchor axis is saturated and
 * SHOULD NOT BE RE-SCREENED.
 * -- scratch_elev/b349/D
 */
extern unsigned char gScript_881__0200cbe4[];
extern unsigned char *__CreateActor(int a);
extern int __CheckPartyItem(int item);
extern int __CheckItem(int a, int item);
extern void __Actor_SetScript(void *a, void *s);
extern unsigned char *__galloc_iwram(int a, int b);
extern void __gfree(int a);
extern void __LoadItemIcon(int id);
extern void __UploadSpriteGFX(int a, int b, void *p);
extern void __PlaySound(int id);
extern void __Func_808f140(void *a, int b);
extern void __GiveItemTo(int a, int b);
extern void __DeleteActor(void *a);
extern void __MapActor_SetAnim(int a, int b);

int OvlFunc_896_200c260(int item)
{
    unsigned char *act;
    unsigned char *o;
    int who;
    int has;
    int z;
    int g1;
    int m;

    do { } while (0);
    g1 = 0xc1 << 3;
    m = -0x21;
    z = 0;
    do { } while (0);
    act = __CreateActor(0x16);
    who = __CheckPartyItem(0xe0);
    has = __CheckItem(who, 0xe0);
    if (act == 0)
        return who;
    __Actor_SetScript(act, gScript_881__0200cbe4);
    o = *(unsigned char **)(act + 0x50);
    o[0x26] = z;
    o[0x27] = z;
    o[5] &= m;
    o[9] &= 0xf;
    *(int *)(act + 0x28) = 0xa0 << 10;
    *(int *)(act + 0x48) = 0x80 << 7;
    z = (int)__galloc_iwram(0x11, g1);
    __LoadItemIcon(item);
    __UploadSpriteGFX(o[0x1c], 0x80, (unsigned char *)z + (0x80 << 3));
    __gfree(0x11);
    __PlaySound(0x53);
    __Func_808f140(act, 3);
    __Func_8078948(who, has);
    __GiveItemTo(who, item);
    __DeleteActor(act);
    __MapActor_SetAnim(0, 1);
    return who;
}
