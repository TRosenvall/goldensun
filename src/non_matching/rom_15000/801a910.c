/* Func_801a910 (0x0801a910) -- NON-MATCHING, 10 of 43 encodings.
 *
 * SIZE 88 == 88, ENCODINGS 43 == 43, 1 RELOCATION IDENTICAL, so the 10 is a
 * TRUE DISTANCE.  (The park this replaces measured 27 of 43 -- re-derived, its
 * claim reproduced.)  THE PROLOGUE AND THE WHOLE alloc != 0 ARM ARE BYTE-EXACT:
 * the first differing encoding is index 25, the second arm's preheader.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801a910.c \
 *     asm/rom_15000/rom_1a66c_c_a_a.s --func Func_801a910
 *
 * SPLIT: NONE NEEDED if it ever lands.  asm/rom_15000/rom_1a66c_c_a_a.s holds
 * exactly ONE function (one .thumb_func_start) and tools/datacheck.py reports no
 * data section and no required exports.  PINS: 0 -- the `register ... __asm__`
 * below is described only as a measurement and is NOT in the shipped body.
 *
 * ================== A CORRECTION TO THE BRIEF, BEFORE ANYTHING ELSE ==========
 * Batch 324's brief says this function "HAS TWO OTHER PARKS THAT DEFINE A BODY",
 * src/non_matching/rom_15000/801be80.c and .../801ba68.c.  IT DOES NOT.  Both
 * files carry only `extern struct P *Func_801a910(int a);` -- a declaration.
 * There is exactly ONE body in the tree, so there is nothing for
 * tools/crossfire.py to cross and no "race in which the file nobody compiles
 * wins the headline".
 *
 * What those two parks DO contribute, and it is worth having, is the RETURN TYPE
 * and the pool-verified `struct P` / `struct W` layouts.  The second independent
 * witness is the LANDED sibling src/rom_15000/rom_1a66c_a_c.c, which declares
 * `extern struct Node *Func_801a910(int alloc);` and
 * `extern unsigned char *iwram_3001e98;`, and shows this bank's house spelling
 * for displacements off that object (`p + (0xe5 << 2)`).
 *
 * ================== WHAT THE 27 DECOMPOSED INTO ==============================
 * The park's 27 looks like one defect and reads as one giant misaligned run:
 * encodings 0-11 agree and 12-36 all differ.  It is TWO causes, and the first
 * one hides the second completely.
 *
 * ---- CAUSE 1: loop.c MOVES THE EARLY-EXIT BLOCK OUT OF THE LOOP (worth 14) --
 *
 * The ROM lays each loop out as
 *     cmp r3,#0 / bne <increment> / <return code> / <increment>
 * -- the early exit FALLS THROUGH and the increment is the branch target.  Every
 * `do/while` or `for` spelling of this function produces the opposite polarity
 * with the return code relocated behind the loop, which is what the park
 * recorded as "Blocker class: BRANCH POLARITY" and what it tried and failed to
 * fix with a source-level `goto`.
 *
 * The `-da` dumps name the pass exactly.  Grepping each dump for `code_label`
 * and for the `const_int 468` that is 0x1d4:
 *     .04.addressof   label 68, const 468, const 468, label 52, const 52, const 52
 *     .08.loop        label 68, const 52, const 52, label 169, const 468, const 468, label 170
 * The pre-`.08.loop` order IS the ROM's: loop top, return code inline, then the
 * increment.  `.08.loop` rewrites it and invents two labels to do so.
 *
 * The transformation is in `find_and_verify_loops`, loop.c:2751-2944.  It
 * matches exactly this shape
 *     cond_jump p -> L_after        (any_condjump_p (p) && onlyjump_p (p), and
 *     <block>                       next_real_insn (JUMP_LABEL (p)) == our_next)
 *     uncond jump or RETURN         (line 2753-2757: out of THIS loop)
 *     L_after:
 * then calls `invert_jump (p, new_label, 1)` and `reorder_insns` to move the
 * block to a BARRIER at the target's loop depth.  Its own comment: "the benefits
 * of removing rarely executed instructions from inside the loop usually
 * outweighs the cost of the extra unconditional jump outside the loop".
 *
 * NONE OF ITS OTHER GUARDS IS A LEVER.  `insns_safe_to_move_p`
 * (rtlanal.c:2356-2398) only checks whole exception regions.  The barrier search
 * always succeeds in a function with an epilogue.  But the FIRST condition,
 * line 2752, is `this_loop` -- and `uid_loop` is built from the front end's
 * `NOTE_INSN_LOOP_BEG` / `NOTE_INSN_LOOP_END` notes, which ONLY a `for`,
 * `while` or `do` statement emits.
 *
 *   >> A LOOP WRITTEN AS A LABEL PLUS A BACKWARD `goto` EMITS NO LOOP NOTES, SO
 *   >> loop.c SEES NO LOOP AT ALL AND THIS TRANSFORMATION CANNOT FIRE.
 *
 * Measured: 27 of 43 with `do {...} while (i != N);` -> 13 of 43 with
 * `loopA: ... if (i != N) goto loopA;`.  This is a different thing from the
 * park's failed `goto`, which kept the `do/while` and only jumped over the
 * return; the loop STATEMENT itself has to go.
 *
 * It also explains two things the park could not.  The ROM builds 0x1d4 INSIDE
 * the loop body at the exit instead of hoisting it to the preheader: with no
 * loop notes there is no loop-invariant motion.  And `tools/sweep_variants.py`
 * on `i * 0x34` index arithmetic reads 43 of 43 and +4 bytes -- there is no
 * strength reduction to turn an index into the ROM's two walkers either.
 *
 * ---- CAUSE 2: THE FLAG POINTER IS INCREMENTED BEFORE THE TEST (worth 3) -----
 *
 * The ROM's second loop body is
 *     ldrh r3,[r0] / add r0,#0x34 / cmp r3,#0 / bne <increment>
 * and `add r0,#0x34` CANNOT be scheduling: sched2 runs per basic block and the
 * increment block is a different block.  So the source reads the flag, advances
 * the flag pointer, and only then tests -- `f = *q; q += 0x34; if (f == 0)`.
 * Worth 13 -> 11, and hoisting `q`'s initialisation above `i = 0;` a further 1.
 *
 * ================== THE REMAINING 10, AND WHY IT NEEDS TWO THINGS AT ONCE ====
 * Arm A and the prologue are byte-exact.  The residue is arm B's SHAPE: the ROM
 * carries TWO POINTERS there where arm A carries an integer offset --
 *     mov r2,r4 / mov r0,r4 / mov r1,#0 / add r2,#0x68 / add r0,#0x72
 *     ...  bne <inc> / mov r0,r2 / b <end>      <inc>: add r1,#1 / add r2,#0x34
 * i.e. `p = b + 0x68` is returned directly and `q = b + 0x72` walks the flags.
 * The park's headline finding, "THE LOOP CARRIES AN OFFSET, NOT A SECOND
 * POINTER", is RIGHT FOR ARM A AND WRONG FOR ARM B.
 *
 * Written that way the figure goes UP to 39-40, for two independent reasons,
 * and neither can be fixed alone:
 *
 *  1. `b` LEAVES LO_REGS.  REG_ALLOC_ORDER on ARM is `3, 2, 1, 0, 12, 14, 4, 5,
 *     ...` (config/arm/arm.h:989-995), so **r12 (ip) ranks AHEAD of r4**.  With
 *     both arms offset-carrying, `b` has an in-loop `add r0,r4,r0` in each arm;
 *     .17.lreg says `Register 33 ... pref LO_REGS`, which excludes ip, and
 *     .18.greg gives `33 in 4`.  Give arm B a pointer instead and b's only
 *     in-loop LO_REGS-requiring use in that arm disappears: .17.lreg flips to
 *     `Register 33 pref BASE_REGS`, .18.greg reports `33 in 12` and
 *     `Hard regs used: 0 1 2 3 12 25 26` -- NO r4 AT ALL.  Every
 *     `add r0,r4,r0` becomes `add r0,r0,ip` and the prologue's `ldr r4,[r3]`
 *     becomes `ldr r3,[r3] / mov ip,r3`, so the first difference moves to
 *     encoding 2 and the count saturates.
 *
 *  2. `p` TAKES r0 FROM THE RETURN COPY PREFERENCE.  Pinning b with
 *     `register char *b __asm__("r4")` -- used here as an INSTRUMENT, not
 *     shipped -- fixes (1) and produces the ROM's exact five-instruction arm-B
 *     preheader.  But `p` is the returned value, so `(set (reg:SI 0) p)` donates
 *     a hard-register copy preference for r0 (`set_preference`, global.c:1016).
 *     `p` lands in r0, the exit block's `mov r0,r2` folds away, and the body
 *     ends `beq <end>`.  That is TWO INSTRUCTIONS SHORT: measured 20 of 43, 84
 *     bytes against 88.  The ROM needs `p` in r2 and the FLAG pointer in r0 --
 *     the opposite of what the preference asks for.
 *
 *     So the pin is not a route to a landing here; it is a measurement.  20 is a
 *     figure ABOUT blocker (1), with blocker (2) still standing behind it, and
 *     it is recorded as such rather than shipped.
 *
 * NEXT MOVE: break `p`'s r0 copy preference WITHOUT letting `b` out of LO_REGS.
 * Both have to give in the same edit; every spelling that fixes one alone
 * measures worse (39, 40, or 20-and-short).
 *
 * MEASURED INERT OR WORSE (arm A held fixed, `goto` loops throughout): two
 * pointers 39; two pointers with the early increment 40; `q = p + 0xa` 39;
 * `unsigned short *` flag pointer with `q += 0x1a` 40; declaration order swapped
 * 39; `i` initialised last 39; `do/while` for arm B only 40; the early increment
 * in arm A as well 30; one pointer with the flag read as `p[0xa]` 22 and 8 bytes
 * short; `i * 0x34` index arithmetic 43 and 4 bytes long; all 36 permutations of
 * the two preheaders' three statements, best 10 and worst 15.
 *
 * The two arrays are adjacent and that is confirmed arithmetic, not a guess:
 * 7 records of 0x34 from 0x68 end at 0x1d4, which is where the 5-record array
 * starts, and the flag sits at +0xa in both (0x68+0xa = 0x72, 0x1d4+0xa = 0x1de).
 * The ROM builds 0x1de as `0xef << 1` and 0x1d4 as `0xea << 1`, which is why
 * they are spelled that way below.
 */
extern char *iwram_3001e98;

char *Func_801a910(int alloc)
{
    char *b;
    char *q;
    int i;
    int off;
    int f;

    b = iwram_3001e98;
    if (alloc != 0) {
        i = 0;
        q = b + (0xef << 1);
        off = 0;
    loopA:
        if (*(unsigned short *)q == 0)
            return b + off + (0xea << 1);
        i++;
        q += 0x34;
        off += 0x34;
        if (i != 5)
            goto loopA;
        return 0;
    }
    q = b + 0x72;
    i = 0;
    off = 0;
loopB:
    f = *(unsigned short *)q;
    q += 0x34;
    if (f == 0)
        return b + off + 0x68;
    i++;
    off += 0x34;
    if (i != 7)
        goto loopB;
    return 0;
}
