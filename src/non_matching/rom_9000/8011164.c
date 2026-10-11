/* Func_8011164 (0x08011164) -- NON-MATCHING.  asm/rom_9000/rom_108e4_c.s
 *
 * NON-MATCHING, 27 of 37 encodings, NO PINS, NO DEVICES
 *   (RE-MEASURED batch 327 brief I; the batch-319/324 figure reproduces).
 *   COUNT DIFFERS (ref 37, ours 35) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  The honest statement is WE ARE TWO
 *   INSTRUCTIONS SHORT.  Read the count before the figure.
 *   SIZE ref 80 bytes, ours 76.  Both pool THREE words.
 *   RELOCATIONS differ only by the 4-byte shift the length deficit causes
 *   (same two symbols, ewram_2020000 and gBuffer).
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/8011164.c asm/rom_9000/rom_108e4_c.s --func Func_8011164
 *
 * NOTE FOR COORDINATORS: `src/non_matching/rom_9000/80111b4.c` is NOT a second
 * park for this function.  It is the park for Func_80111b4, a 248-encoding
 * camera update that merely CALLS this one (declared `extern` at its line 164);
 * both functions happen to live in this same reference .s.  It must never
 * appear in this function's `parks_retire`.
 *
 * ==== THE DECOMPOSITION, counted off both streams ====
 *
 *   ROM 34 instructions + 3 pool words = 37 encodings = 80 bytes
 *   ours 32 instructions + 3 pool words = 35 encodings = 76 bytes
 *
 *   OUTSIDE the loop  ROM 14, ours 15 -- we have ONE EXTRA insn, `ldr r6,.L9+8`
 *     (the gBuffer constant hoisted into r6), and `push {r5, r6, lr}` (b560)
 *     against the ROM's `push {r5, lr}` (b520), which is index 0.
 *   INSIDE the loop   ROM 17, ours 14 -- THREE SHORT, all of it the gBuffer
 *     address work and nothing else:
 *       ROM   ldr r5,=gBuffer / add r3,r2,r5 / ldrh r3,[r3] / add r5,#2
 *             / add r3,r2,r5 / ldrh r3,[r3]                            (6)
 *       ours  ldrh r3,[r6,r2] / add r3,r6,#2 / ldrh r3,[r3,r2]         (3)
 *   TAIL equal.  Net -2.  So ONE mechanism really does account for all three
 *   missing instructions, for the single in-loop pool reference and for
 *   `add r5, #2` at once: the gBuffer constant must get NO hard register, so
 *   reload rematerialises it inside the loop and `reload_cse_move2add`
 *   (reload1.c:8840) collapses the second rematerialisation into `add r5,#2`.
 *
 * Also note the register MAP differs beyond the three: ROM i->r0, dst->r1,
 * src->r4, r5 a scratch -- SIX registers, r6 and r7 never mentioned.  Ours
 * dst->r0, i->r5, src->r4, gBuffer->r6 -- SEVEN.
 *
 * ==== TWO BOUNDS ESTABLISHED THIS BATCH ====
 *
 * BOUND 1 -- **NO SOURCE SHAPE CAN STOP loop.c FROM HOISTING THE POOL LOAD.**
 * Both of `loop_invariant_p`'s MEM doors are structurally shut for a thumb
 * constant-pool load, and the arithmetic gate is not close:
 *   * loop.c:3266 branches
 *       RTX_UNCHANGING_P (x) ? unknown_constant_address_altered
 *                            : unknown_address_altered
 *     and a pool MEM takes the FIRST arm.  `unknown_constant_address_altered`
 *     is set in exactly one place -- `note_addr_stored`, loop.c:3144 -- and
 *     ONLY for a **BLKmode** store whose destination is itself
 *     RTX_UNCHANGING_P.  This loop's stores are HImode, so it can never be set.
 *   * The `loop_store_mems` / `true_dependence` door is shut in alias.c:
 *       if (RTX_UNCHANGING_P (x) && ! RTX_UNCHANGING_P (mem)) return 0;
 *     A constant-pool READ can never conflict with a non-constant store, so no
 *     store placed in this loop can ever block the hoist.
 *   * The arithmetic gate (loop.c:1803) measured rather than estimated:
 *     `.08.loop` prints "Loop from 43 to 95: **13 real insns**", so
 *     insn_count = 13, while threshold = (has_call ? 1 : 2) * (1 + n_non_fixed_regs)
 *     (loop.c:651) is >= 20.  For the gBuffer movable (savings 2, lifetime 6)
 *     the test is 20 * 2 * 6 = **240 >= 13**.  It would take a 240-instruction
 *     loop body to fail.  The previous header said "far too generous to be the
 *     discriminator"; this is the number behind that.
 *
 * BOUND 2 -- **THE PREVIOUS HEADER'S NAMED CAUSE, "greg has a free register and
 * the original build did not", IS REFUTED BY THE ROM'S OWN PROLOGUE.**  The ROM
 * opens `push {r5, lr}` and never mentions r6 or r7 in any of its 34
 * instructions, so the original build's greg had AT LEAST TWO free low
 * registers at the moment it declined to allocate the hoisted constant.
 * Register scarcity cannot be the explanation, and the previous header's stated
 * NEXT MOVE -- "keep one more value genuinely live across the loop body" -- is
 * therefore a DEAD END: adding pressure makes greg reach for r7, it does not
 * make it spill.  Measured from `.18.greg` on THIS body (authoritative; this
 * replaces the batch-324 figures, which were for a different body -- there is
 * no `57 in 7`):
 *     ;; 4 regs to allocate: 34 33 35 57
 *     dispositions  34 in 0   33 in 4   35 in 5   **57 in 6**
 *     ;; 57 conflicts: 33 34 35 37 50 52 53 55 56 57 1 2 3 13
 * r0 (34), r1 (55), r2 (50, 52), r3 (37, 53, 56), r4 (33) and r5 (35) are all
 * held by CONFLICTING allocnos, so r6 is simply the first free register.
 * Denying r6 needs TWO more conflicting live values, and the ROM demonstrably
 * has FEWER live values than we do, not more.
 *
 * Also retained from the previous header, and still true: `local-alloc.c:886`'s
 * `reg_equiv_replace` can NEVER fire for a thumb symbol constant, because
 * SET_SRC is a pool-load MEM `(mem (symbol_ref "*.LC1"))` while the note is the
 * bare `(symbol_ref "gBuffer")`, so `rtx_equal_p` is always false.  And
 * `rtlanal.c:234 get_related_value` DOES return the base for a
 * `CONST (PLUS symbol_ref CONST_INT)`, which is why `cse.c:2074
 * use_related_value` rewrites the second pool load to `(plus 52 2)` and gives
 * reg 52 a third reference.  (`local-alloc.c:871`'s REG_LIVE_LENGTH *= 2 still
 * halves the constant's global priority, so it is already the cheapest allocno
 * to lose -- it just never loses while a register is free.)
 *
 * ==== THE RESIDUE, REFRAMED ====
 *
 *   >> NAMED REMAINING CAUSE: why did a greg with r6 AND r7 free decline to
 *      allocate a hoisted reg_equiv_constant?  loop.c cannot be stopped
 *      (BOUND 1), local-alloc's rematerialise path cannot fire (above), and
 *      pressure is refuted (BOUND 2).  What is left is find_reg's exclusion set
 *      for allocno 57 -- `reg_preferred_class`, `no_global_alloc_regs`, or a
 *      hard_reg_conflict on r6/r7 that our build does not have.  That is the
 *      next thing to read, and it is a NARROWER question than the previous
 *      header's.
 *
 * MEASURED INERT / WORSE (all at production flags, ref 37 / 80 bytes).
 * Batch 327 adds the first four, on axes nothing had tried -- statement ORDER
 * of the base assignment, POINTER TYPE, naming the ADDRESS, and folding the
 * base into the pointer.  All EXACTLY INERT at 27 of 37 / 35 insns / 76 bytes:
 *   `g = gBuffer;` moved ABOVE the `t = *(u16 *)src * 4;` read .. 27 (inert)
 *   `unsigned short *` src/dst with 0x40 element strides ........ 27 (inert)
 *   the address named as its own pointer, `a = t + g; *(u16*)a` . 27 (inert)
 *   `g = gBuffer + t;` then `g += 2;` .......................... 27 (inert)
 *   `gBuffer + t` and `gBuffer + 2 + t` inline ................. 29, -4
 *   `t + gBuffer` / `t + gBuffer + 2` (operand order) .......... 29, -4
 *   `gBuffer + (t + 2)` (move the 2 to the index) .............. 29, -4
 *   two hand-hoisted bases g0/g1 ............................... 29, -4
 *   `unsigned short gBuffer[]`, `g[0]` then `g++` .............. 27, -4 (inert)
 *   one `q = gBuffer + t`, then `q[0]` / `*(q+2)` .............. 27, -4 (inert)
 *   `g = gBuffer;` / `g = g + 2;` (this body) .................. 27, -4
 *   `do { } while (i <= 0x3f)` ................................. 29, -4
 *   one scratch int reused for all the reads .................... 30, +4 (worse)
 *   -fno-gcse / -fno-rerun-cse-after-loop / -fno-strength-reduce  inert
 *   -fno-schedule-insns2 ....................................... worse
 * => the SPELLING of the gBuffer site reaches nothing, which is consistent with
 *    the settled fact that only WHICH PSEUDOS ARE LIVE moves an allocation.
 *
 * INSTRUMENTS (figures ABOUT the blocker, not production figures):
 *   body                       prod      -ffixed-r7     -ffixed-r6 -ffixed-r7
 *   this park                  27, -4    24, 0 bytes    32, +8
 *   single `q` base, q[0]/q[1] 27, -4    27, -4         24, 0 bytes, RELOCS OK
 * One register fewer closes the LENGTH gap exactly, which is what first
 * suggested a pressure story -- but under -ffixed-r7 gcc parks gBuffer+2 in `ip`
 * (`mov ip,r3` / `mov r3,ip`) rather than rematerialising, so it is not the
 * ROM's mechanism, and BOUND 2 shows it cannot be.
 *
 * A DEVICE, kept as a figure about the blocker: two DISTINCT extern symbols
 * (`gBuffer` and a fictitious `gBuffer_PLUS2`) defeats cse's relation and reads
 * 28 WITH THE LENGTH EXACT -- but it pools FOUR words and allocates r6 AND r7,
 * so the relation is not the whole story either.  Not a result; the ROM pools
 * three words, one of them gBuffer.
 *
 * WHAT IS RIGHT and should be kept: the signed `n / 2` expansion
 * (`lsr #31 / add / asr #1`), the `& 0x1f` and `& 0x3e` masks, the unsigned
 * `i <= 0x3f` bound, the 0x80 strides on both pointers, the `dst + 0x40` second
 * store, and the pool CONTENTS and ORDER (ewram_2020000, 0x06004000, gBuffer),
 * which already agree with the ROM's.
 *
 * ============ BATCH 334, BRIEF C: THE CLAIM REPRODUCES; A PIECE-LEVEL
 * ============ BLOCKER AND ONE UNTRIED DIMENSION
 *
 * RE-MEASURED, and the claim line at the top of this file REPRODUCES EXACTLY:
 * ref 37 encodings / ours 35, instructions ref 34 / ours 32, size ref 80 bytes /
 * ours 76, relocations differing only by the shift the deficit causes.  The
 * decomposition below is confirmed off both streams.  Nothing in this header
 * needed correcting.
 *
 * ==================== THE PIECE CANNOT BE LANDED AT ALL YET ====================
 *
 * asm/rom_9000/rom_108e4_c.s holds SEVEN functions and ALL SEVEN ARE PARKED --
 * Func_8010e14, Func_8010ff0, Func_80110e0, this one, Func_80111b4,
 * Func_80113e4, Func_80114a0.  A .c is one TU, so matching this function alone
 * installs nothing: it needs a split, and because its piece-mate Func_80114a0 is
 * also a batch-334 target and is NOT adjacent to it, taking both off the count
 * needs TWO cuts of the same .s rather than one.  Coordinators should treat this
 * park and the Func_80114a0 park as a PAIR whose landing cost includes two
 * byte-neutral splits and a linker-script change, and should not schedule either
 * as a standalone single.  (The note above about Func_80111b4 not being a second
 * park for this function still stands and is unrelated.)
 *
 * ========== THE IN-LOOP DIFFERENCE IS ALSO AN ADDRESSING-MODE DIFFERENCE ==========
 *
 * The decomposition above reads the in-loop deficit entirely as gBuffer failing
 * to get a hard register.  That is one true description of it; here is a second,
 * which names a lever the inert list above does NOT contain.  Set the two bodies
 * side by side at the loads:
 *
 *     rom   add r3,r2,r5 / ldrh r3,[r3]   ... add r3,r2,r5 / ldrh r3,[r3]
 *     ours  ldrh r3,[r6,r2]               ... ldrh r3,[r3,r2]
 *
 * The ROM MATERIALISES EACH ADDRESS and uses an immediate-offset load; we use
 * Thumb's REGISTER-OFFSET form, which is exactly one instruction cheaper per
 * load.  Two loads, two instructions -- the same arithmetic the gBuffer story
 * gives, reached from the addressing mode instead of from the allocation.
 *
 * This is worth recording because it is a lever with a measured precedent in
 * this batch rather than an open question: src/non_matching/rom_a1000/80a3354.c
 * had two stores in the same shape, and NAMING THE ADDRESS AS ITS OWN POINTER
 * LOCAL converted one of them from the register-offset form to the ROM's
 * materialise-then-store form, moving its instruction count by one.  The inert
 * list above does contain "the address named as its own pointer, `a = t + g;
 * *(u16 *)a`" at the parked figure -- so the lever has been tried HERE and did
 * not fire, and that is a real negative.  What was never tried is the lever
 * CROSSED with the pointer walk: in the 80a3354 measurement the site that
 * accepted the edit was the one whose INDEX REGISTER STAYED LIVE past the
 * access, and the site that refused it was the one where the index died there.
 * In this loop both indices die at their load.  So the dimension nobody has
 * varied here is not how the address is spelled -- it is whether `t` survives
 * its second use, and the cheapest probe is a body in which the index is still
 * needed after the second load.
 *
 * NOTE ON docs/agbcc-thumb-regoffset-bug.md: read its scope note before using
 * any of the above.  gcc296 emits register-offset addressing freely (1,156 sites
 * in this tree) and "the offset as a named local" is a recorded lever that EARNS
 * that form; what this park needs is its INVERSE, and the two must not be
 * confused.
 *
 * ============ BATCH 336, BRIEF F: 27 of 37 HELD.  THE PARK'S ONE NAMED
 * ============ UNTRIED LEVER IS NOW REFUTED BY A 15-ROW CROSS, AND THE
 * ============ ADDRESSING MODE IS A CONSEQUENCE, NOT A SECOND LEVER.
 *
 * RE-DERIVED FIRST, in stream terms: 27 of 37 (ref 37 encodings, ours 35),
 * instructions 34/32, SIZE ref 80 bytes against ours 76, relocations differing
 * only by the 4-byte shift the deficit causes (same two symbols,
 * ewram_2020000 and gBuffer, same order).  COUNT DIFFERS, so the 27 measures
 * MISALIGNMENT and the honest figure is STILL "two 16-bit instructions short".
 * tools/aligncmp.py puts the real residue at 18 of 37 aligned-equal (48.6%),
 * 25 differing/ins/del in 9 hunks.  Everything in this header reproduced;
 * nothing needed correcting.
 *
 * THE DECOMPOSITION ALSO REPRODUCES, re-counted off both streams with the loop
 * entry taken from the branch target (ROM 0x1c, ours 0x1e):
 *     head (prologue + both pointer setups + i = 0)   ROM 14   ours 15   +1
 *     loop body                                       ROM 17   ours 14   -3
 *     tail (pop / bx)                                 ROM  3   ours  3    0
 *                                                     ROM 34   ours 32   -2
 * and the head's +1 is specifically a THIRD POOL LOAD: the ROM takes two in the
 * head (pool 0x44, 0x48) and one INSIDE the loop (0x4c); we take all three in
 * the head.  `push {r5,r6,lr}` / `pop {r5,r6}` against `push {r5,lr}` /
 * `pop {r5}` are same-count REPLACEMENTS, not extra instructions -- worth
 * saying because index 0 is one of them and it reads like a length defect.
 *
 * ** ONE FACT NOT PREVIOUSLY RECORDED, AND IT IS THE SHARPEST EVIDENCE HERE.
 * The ROM's r5 is REUSED FOR TWO UNRELATED VALUES: it holds 0x6004000 in the
 * head (idx 9 `ldr r5,=...`, idx 12 `adds r1,r3,r5`) and gBuffer inside the
 * loop (idx 15 `ldr r5,=gBuffer`, idx 19 `adds r5,#2`).  The ROM touches
 * r0-r5 ONLY and never mentions r6 or r7 in any of its 34 instructions.  So in
 * the original build the gBuffer constant held NO REGISTER OF ITS OWN AT ALL --
 * it borrowed a dead one each iteration.  That is stronger than "it was
 * spilled", and it is consistent with BOUND 2 above: the ROM has FEWER live
 * values than we do, so pressure remains refuted. **
 *
 * ======== REFUTED: "the in-loop difference is also an addressing-mode
 * ======== difference", i.e. the batch-334 section's second lever ========
 *
 * That section proposes the addressing mode as a SECOND, INDEPENDENT lever
 * (ROM materialises each address then uses immediate-offset; we use Thumb's
 * register-offset form), cites src/non_matching/rom_a1000/80a3354.c as a
 * precedent, and says what was never tried is that lever CROSSED with the
 * pointer walk.  CROSSED THIS BATCH -- 15 rows, 5 address spellings x 3
 * g-forms -- AND EVERY ONE COMPILES TO A BIT-IDENTICAL OBJECT:
 *
 *   g-form:   g = gBuffer; + g += 2 in loop (this body)
 *           | g = gBuffer; BEFORE the loop, then g and g + 2
 *           | g = gBuffer; in loop, no bump, then g and g + 2
 *   address:  inline (this body)
 *           | one named `unsigned char *a`, reused for both accesses
 *           | one named `unsigned short *q`, reused
 *           | two distinct `a0`, `a1`
 *           | `a` plus a named `d2 = dst + 0x40` for the second store
 *
 * All 15: ndiff 27 of 37, 34/32 instructions, first index 0, aligned-equal 18
 * (48.6%), 25 diffs in 9 hunks -- THE SAME NUMBERS AS THE BASE IN EVERY COLUMN.
 * A second 24-row cross (3 g-forms x 2 operand orders `t+g` / `g+t` x 3 t-forms
 * `int` / `unsigned int` / `<< 2`) is also inert: 18 of the 24 BIT-IDENTICAL to
 * this body, and the 6 two-base rows (`g2 = g + 2`) WORSE at 29 / aligned 17.
 *
 *   >> SO THE ADDRESSING MODE IS NOT AN INDEPENDENT LEVER -- IT IS A
 *      CONSEQUENCE OF THE ALLOCATION.  cse2 canonicalises every one of these
 *      spellings to the same RTL before reload ever sees them, which is why the
 *      objects are BYTE-EQUAL rather than merely equal-scoring.  The batch-334
 *      "second description" is a true description and a FALSE LEVER.  This
 *      function has exactly ONE free variable: whether the gBuffer pool
 *      constant gets a hard register.  Everything else measured here or above
 *      is downstream of it, and no further spelling work is worthwhile. <<
 *
 * NOT RETRIED AND RECORDED SO NOBODY RETRIES THEM: every row in the inert list
 * above, plus all 39 rows of this batch's two crosses.
 *
 * INSTRUMENT WARNING, measured on the piece-mate Func_80114a0 and applying to
 * this park's own -ffixed probes: -ffixed-rN IS NOT A USABLE INSTRUMENT on
 * these bodies.  Denying a high register makes the body SHORTER, because the
 * unused register drops out of the prologue's mov/push save-restore block -- so
 * the flag changes the very region the measurement is about.  This park's own
 * note that -ffixed-r7 produces an `ip` park rather than a rematerialisation
 * already says the figures are not the ROM's mechanism; this is why.
 *
 * ==== PIECE STATUS RE-VERIFIED FROM THE TREE, AND THE "PAIR" FRAMING IS WRONG
 *
 * asm/rom_9000/rom_108e4_c.s holds SEVEN .thumb_func_start and ALL SEVEN ARE
 * STILL PARKED (Func_8010e14 136/201, Func_8010ff0 101/112, Func_80110e0
 * 52/63, this one 27/37, Func_80111b4 151/248 with 3 pins, Func_80113e4 73/91,
 * Func_80114a0 55/97).  Verified against src/non_matching/rom_9000/ and
 * docs/directory/01-parked.md; none of the seven has a landed .c.  The
 * batch-334 section above is CORRECT about that.
 *
 * BUT ITS SCHEDULING ADVICE IS NOT.  The piece is ALREADY sub-split into
 * _a / _b / _c -- stage1.ld:228-230 lists rom_108e4_{a,b,c}.o(.text) and
 * :299-301 the same three for .rodata.  This function is member 4 of 7 and
 * Func_80114a0 is member 7 (LAST).  Once _c is cut, EACH TARGET IS ITS OWN TU
 * and matches or fails INDEPENDENTLY, so:
 *     Func_80114a0 needs ONE cut   (_c = members 1-6, plus 80114a0 alone)
 *     this function needs TWO cuts (it is interior)
 *     both together need THREE cuts, not the "two" stated above
 * and neither cut's cost depends on the other target.
 *
 *   >> CORRECTED SCHEDULING STATEMENT: these are TWO INDEPENDENT SINGLES whose
 *      landing cost is a byte-neutral tools/split_s.py cut plus a stage1.ld
 *      edit -- one cut for Func_80114a0, two for this one.  They should NOT be
 *      gated on each other.  What they are not is landable without touching
 *      stage1.ld. <<
 */
extern unsigned char ewram_2020000[];
extern unsigned char gBuffer[];

void Func_8011164(int n)
{
    unsigned char *src;
    unsigned char *dst;
    unsigned int i;
    int t;
    unsigned char *g;

    src = ewram_2020000 + ((n / 2) & 0x1f) * 4;
    dst = (unsigned char *)0x6004000 + (n & 0x3e);
    for (i = 0; i <= 0x3f; i++) {
        t = *(unsigned short *)src * 4;
        g = gBuffer;
        *(unsigned short *)dst = *(unsigned short *)(t + g);
        g += 2;
        *(unsigned short *)(dst + 0x40) = *(unsigned short *)(t + g);
        dst += 0x80;
        src += 0x80;
    }
}
