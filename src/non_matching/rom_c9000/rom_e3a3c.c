/* ===================== BATCH 350 (brief C) -- NEGATIVE-LIST RE-TEST =====================
 * Anim_Attack -- STILL PARKED, 5 OF 39, PIN-FREE.  BODY UNCHANGED.
 *
 * FIGURE RE-DERIVED MYSELF, all three lengths in phase:
 *   XX ENCODINGS differ in 5 place(s) (ref 39, ours 39)  [instructions 27/27]
 *      first at index 12: ref 682b (ldr r3,[r5])  ours 682a (ldr r2,[r5])
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/rom_e3a3c.c \
 *     asm/rom_c9000/rom_e3958_c_c_c_c_a.s --func Anim_Attack
 * Split shape (for when it lands, NOT carried in the manifest): the reference
 * holds three functions; split_s.py --dry-run targeting Anim_Attack cuts it out
 * ALONE into _a_b.s (46 lines) leaving _a_c.s (2 funcs).  datacheck.py: no data
 * section, export list EMPTY.  pins 0 (shimcount.py prints the filename only).
 *
 * ***** THE PARK CLAIMS 12 SPELLINGS.  THEY ARE 3 DISTINCT PROGRAMS. *****
 * Rebuilt every recorded row and swept with object fingerprinting
 * (tools/sweep_variants.py).  TEN OF TWELVE ARE BIT-IDENTICAL TO THE BASE --
 * they measured NOTHING, and must not be read as negatives:
 *     compiler temp `(unsigned)(v - 0x64) <= 0x23` (no named t)   BIT-IDENTICAL
 *     `unsigned v` with `(int)v > 0xc7`                          BIT-IDENTICAL
 *     `t` declared before `v`                                    BIT-IDENTICAL
 *     `v >= 0xc8`                                                BIT-IDENTICAL
 *     `t = v = *p; t -= 0x64;`                                   BIT-IDENTICAL
 *     `t` scoped to an inner block                               BIT-IDENTICAL
 *     `void *p` with a cast                                      BIT-IDENTICAL
 *     `int t` with `(unsigned)t <= 0x23`                          BIT-IDENTICAL
 *     the field read twice instead of held in a local             BIT-IDENTICAL
 *     the subtraction given its own named local                  BIT-IDENTICAL
 *   Only two are real: arms swapped `if (t > 0x23){...}else{...}` 10, RELOCDIFF
 *                      `unsigned char t`                         29, +4 bytes
 * So the park's "no respelling helps" verdict is CORRECT but for a stronger
 * reason than it gave: no spelling of this statement reaches the back end.
 *
 * ***** AND THE AXIS NOBODY HAD VARIED: THE KIND OF CONSTRUCT. *****
 * Every recorded row writes the subtraction EXPLICITLY.  I varied the construct:
 *     `v >= 0x64 && v <= 0x87`                           BIT-IDENTICAL TO BASE
 *     `v > 0x63 && v < 0x88`                             BIT-IDENTICAL
 *     `0x64 <= v && v <= 0x87`                           BIT-IDENTICAL
 *     unsigned v + range                                 BIT-IDENTICAL
 *     range + `v >= 0xc8`                                BIT-IDENTICAL
 *   -- fold-const's build_range_check lowers every literal range test to exactly
 *      this body, so the folded range test and the hand-written subtraction are
 *      ONE PROGRAM.  That closes the whole family in one row.
 *     `switch (v) { case 0x64 ... 0x87: ...; default: if (v>0xc7)... }`
 *                                           *** 4 of 39, first index 13 ***
 *     `switch` with both arms as case ranges              9 of 39, RELOCDIFF
 *
 * READ THE SWITCH ROW CAREFULLY BEFORE BANKING IT.  It is the FIRST construct in
 * eight passes to move index 12: `v` lands in r3, the ROM's register, because
 * stmt.c's case-range lowering creates NO second local quantity and so
 * local-alloc has nothing to take r3 with.  But it emits a different idiom --
 * `cmp #0x87 / bgt / cmp #0x64 / blt` instead of the ROM's
 * `mov r1,r3 / sub r1,#0x64 / cmp r1,#0x23 / bhi` -- so its 4 is a lower figure
 * on the WRONG SHAPE, and it discards the one property the installed body shares
 * with the ROM.  Device-free and pin-free; equal length (27/27, dsize 0).
 * RECORDED, NOT SHIPPED.  Candidate kept at scratch_elev/b350/C/v1b/b03_switch_one_range.c
 *
 * ***** WHY v CANNOT HAVE r3 IN THE SUBTRACTION SHAPE -- file:line, re-read *****
 * `t` is block-local to bb0, so local-alloc owns it and runs BEFORE global-alloc.
 *   find_free_reg (local-alloc.c:1963-2051) builds `used` from fixed_reg_set
 *   (t crosses no calls, :1966-1967), the union of regs_live_at[] over t's
 *   lifetime (:1974-1975) and the complement of min_class (:1977), then walks
 *   REG_ALLOC_ORDER and returns the FIRST free register (:2022-2043).
 *   REG_ALLOC_ORDER, READ not recalled, is arm.h:984-995:
 *       { 3, 2, 1, 0, 12, 14, 4, 5, 6, 7, 8, 10, 9, 11, 13, 15, ... }
 *   so r3 is first and free, and post_mark_life (:2039) then marks it, which is
 *   why global-alloc hands v r2.
 * THE SUGGESTION PATH IS CLOSED, AND THIS IS NEW.  find_free_reg's
 * just_try_suggested phase (:2012-2018) can only reach a register that is in
 * qty_phys_copy_sugg / qty_phys_sugg, and combine_regs sets those ONLY from a
 * copy where one side is already a HARD register (local-alloc.c:1674-1708).
 * `t` is never copied to or from a hard register in this function, so NO source
 * construct can give t a suggestion of r1.  That removes the last local-alloc
 * lever, and it is the one the earlier passes never named.
 *
 * ***** THE PIN LADDER RE-VERIFIED, AND FINGERPRINTING SHARPENS IT *****
 *     pin-free                                               5  @12
 *     register int v __asm__("r3")                            3  @13
 *     register unsigned int t __asm__("r2") + v on r3         3  BIT-IDENTICAL
 *                                                               to the row above
 *     register unsigned int t __asm__("r1")                  *0*  byte-identical
 *     both pinned                                            *0*  bit-identical
 *                                                               to the row above
 * The bit-identity of rows 2 and 3 is a PROOF of the park's step-down claim:
 * blocking r3 moves t to r2 and nowhere else.  So reaching r1 needs TWO
 * simultaneous exclusions across t's two-insn life, and the streams agree
 * instruction for instruction (27/27), so there is no spare instruction to
 * make even one more live value out of.  VERDICT UNCHANGED: park at 5 pin-free,
 * 0 with one pin, pass-3 depin item (reports/pass3-depin.md).
 * DO NOT re-screen spellings, flags or anchors here; all three are now closed
 * with either a bit-identical set or a file:line proof.
 * -- scratch_elev/b350/C
 * =======================================================================================
 */
/* ===== CARRIED FORWARD from the retired duplicate park 80e3a3c.c =====
 * That file was a 30-line HEADER-ONLY park for this same function, naming the
 * pre-split asm/rom_c9000/rom_e3958_c_c_c_c.s and a scratch/Nattack_best.c
 * that no longer exist.  It is retired to toDelete/ as of batch 336; this one
 * note is the only thing in it the live park did not already carry.
 *
 * THIS PARK'S COIN FLIP IS NOT THE SAME COIN FLIP AS THE OTHER FOUR.  The
 * register coin flip parked on ovl_7ced6c/2009c84.c, .../200a16c.c,
 * .../20096a8.c and ovl_7d0e88/2008f58.c is about CALLEE-SAVED registers --
 * which long-lived value earns r5 rather than r6.  This one is entirely in the
 * SCRATCH set: after the third galloc call every one of r0-r3 is free, and gcc
 * simply takes r2 where the ROM took r3.  No push differs, no value is
 * spilled, and nothing about live ranges is in play.  Whether the two share a
 * cause in `find_reg` is NOT established -- recorded separately so nobody
 * assumes it and fixes four parks with one lever that cannot reach this one.
 *
 * Three screened and inert at 5 (from that file, not re-verified here): the
 * subtraction given its own named local; the field read twice instead of held
 * in a local; and `(unsigned)(k - 0x64) > 0x23` with the arms swapped, which
 * is WORSE at 16 because it reorders the branches too.
 */
/* ===== BATCH 336 (brief E) -- 5 OF 39 STREAM ENCODINGS.  BODY UNCHANGED. =====
 * RE-MEASURED, not inherited: `XX ENCODINGS differ in 5 place(s) (ref 39, ours
 * 39) ... instructions are 27/27`, first differing index 12, ref 682b against
 * ours 682a.  Stream lengths and instruction counts both exact, so the 5 IS a
 * distance.  PIN-FREE: tools/shimcount.py prints the filename and nothing else.
 * NOT RE-SWEPT -- the brief forbade it and the batch-330 ladder below is
 * closed-form.  The 0 stays a pass-3 depin item.
 *
 * ONE CORRECTION TO THE PIECE NOTE BELOW, which is otherwise exactly right.
 * It says "none of the three converts whole", and that is true, but the batch
 * brief turned it into "neither ships alone", which is FALSE.  tools/split_s.py
 * cuts before/target/after, so ANY member can be cut out alone; my own
 * --dry-run on this .s:
 *     target Anim_Attack      -> _a_b.s (Anim_Attack ALONE, 46 lines)
 *                              + _a_c.s (2 funcs, 1434 lines)
 *     target Anim_CriticalHit -> _a_a.s (2 funcs, 757 lines)
 *                              + _a_b.s (Anim_CriticalHit ALONE, 723 lines)
 * So the "land all three in the same sitting" advice below is a CONVENIENCE,
 * not a constraint -- and since BaseAnim_Attack is at 653 of 688 with size and
 * count both inexact, waiting for it would park the other two indefinitely.
 * Whichever reaches 0 first should be cut out on its own.
 * -- scratch_elev/b336/E
 * =============================================================================
 */
/* ===================== BATCH 327 (brief A) ADDENDUM -- READ FIRST =====================
 * Anim_Attack -- STILL NON-MATCHING, 5 differing encodings of 39.  BODY
 * UNCHANGED.  The pin ladder below is closed-form and the 0 is a pass-3 item
 * (docs/owner-decisions.md standing standard 3), so the pin was NOT re-proposed.
 *
 * FIGURE RE-MEASURED MYSELF: 5 of 39 (ref 39, ours 39), first differing index
 * 12.  Confirmed, pin-free, instruction count exact.
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/rom_e3a3c.c asm/rom_c9000/rom_e3958_c_c_c_c_a.s --func Anim_Attack
 *
 * -------- WHAT BATCH 327 ADDS: THE MODULE EVIDENCE THIS PARK WAS DENIED -------
 * `tools/upstream_module.py Anim_Attack` -> upstream rom_c9000/rom_e3958.s,
 * with SIX LANDED module-mates: rom_e3958_b.c [GetBattleActorPos],
 * _c_b.c [GetBattleActorPos2], _c_c_b.c [GetBattleActorPos3],
 * _c_c_c_a.c [call_via, Func_80e3994], _c_c_c_b.c [Func_80e3a14],
 * _c_c_c_c_b.c [UploadPalette, CopyBuf, Func_80e46f0].
 * The tree-wide claim that "ZERO Anim_* / BaseAnim_* sources have landed" is FALSE
 * -- 148 are landed in src/rom_c9000/ against 52 parked -- and it was reached by
 * checking FILENAMES (all split-named rom_XXXXXX_*.c) instead of definitions.
 * A LANDED HEADER RECORDS WHAT THE TU ACTUALLY WANTED; A PARK RECORDS WHAT
 * SOMEBODY COULD NOT MAKE WORK.
 *
 * -------- AND A SPLIT FACT THAT BINDS THIS PARK TO TWO OTHERS ----------------
 * asm/rom_c9000/rom_e3958_c_c_c_c_a.s HOLDS THREE FUNCTIONS -- Anim_Attack,
 * BaseAnim_Attack AND Anim_CriticalHit -- so none of the three converts whole.
 * Per Anim_CriticalHit's own dry-run evidence the good cut order is
 * BaseAnim_Attack FIRST (it leaves Anim_CriticalHit alone in _c.s and
 * Anim_Attack alone in _a.s); cutting for Anim_CriticalHit first buries
 * Anim_Attack and BaseAnim_Attack together in a two-function _a.s that would
 * have to be split again.  WHOEVER LANDS ANY ONE OF THE THREE SHOULD LAND THEM
 * IN THE SAME SITTING, OFF THE ATTACK-FIRST CUT.
 *
 * -------- THE ONE CRACK I DID NOT CLOSE, stated so nobody mistakes it for one -
 * The bound below rests on "no LOCAL qty can overlap t's lifetime, because a qty
 * live at a block end is not block-local".  A qty BORN inside bb0 and DYING AT
 * bb0'S LAST INSN is still block-local and would overlap, and no row below tests
 * one.  I did not test it either (it needs an extra instruction to be worth
 * anything, and the counts are exact).  TREAT THE BOUND AS STRONG BUT NOT
 * AIRTIGHT ON THAT ONE SHAPE.
 *
 * ======== BATCH 330, BRIEF B: THE ONE ADMITTED CRACK IS NOW CLOSED ========
 * RE-DERIVED: five differing encodings of thirty-nine, index twelve, ref 682b
 * against ours 682a, counts and size equal.  The ladder and the decode above
 * both reproduce exactly.
 *
 * ONE CORRECTION TO THE MACHINERY, read out of the dump rather than recalled:
 * the header says t is `pref LO_REGS`.  It is not.  `.17.lreg` prints
 *     Register 34 used 2 times across 2 insns in block 0; set 1 time; user var;
 *     pref BASE_REGS.
 * BASE_REGS, not LO_REGS.  The conclusion is unaffected -- find_free_reg walks
 * REG_ALLOC_ORDER inside whichever class and r3 is first and free either way --
 * but anyone recomputing the exclusion set from the header's class would get the
 * wrong set.  Confirmed alongside it: `.18.greg` says
 *     ;; 2 regs to allocate: 33 32
 * so t is NOT a global allocno at all, and
 *     ;; Register 34 in 3.
 * so local-alloc is indeed what takes r3, exactly as the header says.
 *
 * THE CRACK THE HEADER LEFT OPEN -- `a qty BORN inside bb0 and DYING AT bb0'S
 * LAST INSN is still block-local and would overlap, and no row tests one` --
 * CANNOT PRODUCE A LANDING, and the header's own ladder is what proves it.
 * Blocking r3 alone is measured at three, not zero: t moves exactly one step
 * down REG_ALLOC_ORDER to r2.  So t reaches r1 only if BOTH r3 AND r2 are in
 * `used` across t's two-insn life, which needs TWO overlapping live values,
 * not one.  And there is no spare value to make even one of them out of: the
 * two instruction streams agree INSTRUCTION FOR INSTRUCTION -- same count, same
 * size, the only difference being which register holds v and which holds t --
 * so any additional live value is an additional instruction, and the count is
 * exact.  The shape may well exist in general; in THIS function there is
 * nothing to put in it.
 *
 * ALSO CHECKED, so it is not re-derived: making t a GLOBAL allocno would not
 * help even if a zero-instruction way to do it existed.  `allocno_compare`
 * (global.c:597-620) gives v (three refs across four insns) a priority of
 * 7500 and t (two refs across two insns) 10000, so t would sort FIRST and take
 * r3 again.  For v to sort first t would need a live length of three or more,
 * which is one more insn.
 *
 * VERDICT UNCHANGED AND NOW BOUNDED FROM BELOW AS WELL AS ABOVE: pin-free this
 * is five, the one-pin body is zero, and the gap is TWO simultaneous register
 * exclusions in a function with no spare instruction to buy either.  Pass-3
 * depin item; do not re-sweep spellings and do not re-propose the pin as a
 * landing.
 * -- scratch_elev/b330/B
 * =====================================================================================
 */
/* Anim_Attack  [rom_c9000]  --  5 of 39, MEASURED batch 326 (brief H).
 *
 * NON-MATCHING, 5 of 39 encodings.  INSTRUCTION COUNT 39 = 39 and SIZE equal,
 * so the 5 IS a true distance (no pad absorbing a length difference).
 * objcmp verbatim:
 *     XX ENCODINGS differ in 5 place(s) (ref 39, ours 39)
 *        first at index 12: ref 682b  ours 682a
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/rom_e3a3c.c \
 *     asm/rom_c9000/rom_e3958_c_c_c_c_a.s --func Anim_Attack
 *
 * THE RESIDUE IS TWO REGISTERS, AND NOTHING ELSE (aligncmp, 34/39 aligned-equal):
 *     ref   ldr r3,[r5] / adds r1,r3,#0 / subs r1,#0x64 / cmp r1,#0x23 ... cmp r3,#0xc7
 *     ours  ldr r2,[r5] / adds r3,r2,#0 / subs r3,#0x64 / cmp r3,#0x23 ... cmp r2,#0xc7
 * ROM: v=r3, t=r1.  Ours: v=r2, t=r3.
 *
 * ============ WHY, SETTLED FROM local-alloc.c AND CONFIRMED BY LADDER ========
 * `.17.lreg`: pseudo 33 = v (3 refs / 4 insns, live in bb0 AND bb2 -> global),
 * pseudo 34 = t (2 refs, LOCAL TO bb0, pref LO_REGS, no suggestion), pseudos
 * 35/36/37 = the three galloc size arguments (copy-suggested r1, and all three
 * DIE BEFORE t is born).  `.17.lreg` ends `;; Register 34 in 3.` and `.18.greg`
 * says `32 in 5  33 in 2  34 in 3`, so LOCAL-alloc took r3 for t and global then
 * had to give v r2.
 *
 * `block_alloc` (local-alloc.c:1337-1348) runs a SUGGESTED pass first, then a
 * life-length pass calling `find_free_reg` (:1441).  `find_free_reg`
 * (:1963-2050) builds `used` from `fixed_reg_set` (t crosses no calls), the
 * union of `regs_live_at[]` over t's lifetime, and the complement of min_class,
 * then walks REG_ALLOC_ORDER = {3,2,1,0,12,14,4,5,...} (arm.h:989) and returns
 * the FIRST free register.  r3 is free, so t is r3.
 *
 * For t to be r1, BOTH r3 AND r2 must be in `used` across t's lifetime -- and
 * t's lifetime is insns 39-41, the LAST TWO INSNS OF bb0.  No *local* qty can
 * overlap it (a qty live at a block end is not block-local and goes to global
 * alloc instead), and the only hard regs live there are r5 (p) and sp.  So the
 * walk is deterministic and the source has nothing to put in r3 or r2.
 *
 * MEASURED LADDER -- this is the confirmation, not the argument:
 *     no instrument                      t -> r3 (1st in order)    5
 *     register int v __asm__("r3")        t -> r2 (2nd in order)   *3*
 *     register unsigned int t __asm__("r1")                        *0*
 *     both pinned                                                  *0*
 * Blocking r3 alone moves t exactly one step down REG_ALLOC_ORDER.  That is the
 * prediction, and it is why no respelling helps.
 *
 * 0 OF 39 WITH ONE PIN: `register unsigned int t __asm__("r1")` on the range
 * temporary.  Device-free; r1 is call-clobbered and t crosses no calls, so the
 * prologue push set is unchanged.  NOT LANDED: pin policy prefers a pin-free
 * body, so this is parked at 5 pin-free with the 0 recorded beside it.
 * See reports/pass3-depin.md.
 *
 * MEASURED FLAT AT EXACTLY 5 (sweep_variants, byte-identical to this file):
 *   compiler temp `(unsigned)(v - 0x64) <= 0x23`; `unsigned v` with
 *   `(int)v > 0xc7`; `t` declared before `v`; `v >= 0xc8`; `t = v = *p; t -= 0x64`;
 *   `t` scoped to an inner block; `void *p` with a cast; `int t` with
 *   `(unsigned)t <= 0x23`.
 * MEASURED WORSE: inverted `if (t > 0x23) {...} else {...}` = 10 and RELOCDIFF;
 *   `unsigned char t` = 29.
 *
 * WHAT IS RIGHT (unchanged from the earlier header, and re-confirmed): the range
 * check is a SUBTRACTION into an unsigned local followed by ONE compare, and the
 * second bound is a separate SIGNED compare, so the two must not be fused.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 5 of 39, instruction counts EQUAL, byte size and pool
 *   count equal too (this park is in phase on all three length dimensions, so
 *   its figure IS a real distance).
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: ALL 10 PLACEMENTS WERE BIT-IDENTICAL TO THE BASE,
 *   so THIS AXIS MEASURED NOTHING.  gcc canonicalises every one of these
 *   spellings away before the back end.  That is NOT a negative and must
 *   not be read as one -- the anchor lever is UNTESTED here.  No variant was capped and none failed to
 *   compile.
 *
 *   SO DO NOT RE-SCREEN THESE.  Every brief so far has spent part of a round on
 *   exactly that.  The residue here is NOT a placement or flag question; it is a
 *   register choice, a tie, or a pass-level effect, and it needs a hypothesis.
 *
 *   Read as "inert against THIS body".  A recorded negative expires silently
 *   when the body changes -- six batches running, the decisive lever has turned
 *   up inside a park's own negative list.
 */
extern void *galloc_ewram(int tag, int size);
extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void BaseAnim_SpecialAttack(void *p);
extern void Anim_CriticalHit(void *p);
extern void BaseAnim_Attack(void *p);

void Anim_Attack(int *p)
{
    int v;
    unsigned int t;

    galloc_ewram(0x29, 0x60e);
    galloc_iwram(0x27, 0x782c);
    galloc_iwram(0x28, 0x80 << 7);
    v = *p;
    t = v - 0x64;
    if (t <= 0x23)
        BaseAnim_SpecialAttack(p);
    else if (v > 0xc7)
        Anim_CriticalHit(p);
    else
        BaseAnim_Attack(p);
    gfree(0x28);
    gfree(0x27);
    gfree(0x29);
}
