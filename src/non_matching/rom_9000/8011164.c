/* Func_8011164 (0x08011164) -- NON-MATCHING.  asm/rom_9000/rom_108e4_c.s
 *
 * NON-MATCHING, 27 of 37 STREAM encodings, NO PINS, NO DEVICES
 *   (RE-MEASURED batch 343 brief B; the batch-319/324/327/334/336 figure
 *   reproduces for the sixth time, and nothing in the header needed correcting.)
 *   COUNT DIFFERS -- 34 reference instructions against 32 -- so this positional
 *   figure measures MISALIGNMENT, not distance.  THE HONEST STATEMENT IS
 *   **WE ARE TWO 16-BIT INSTRUCTIONS SHORT**, i.e. OURS IS SHORTER and gcc
 *   performed an optimisation the original build did not.  Read the count first.
 *   SIZE ref 80 bytes, ours 76.  Both pool THREE words, same contents, same
 *   order (ewram_2020000, 0x06004000, gBuffer).
 *   RELOCATIONS differ only by the 4-byte shift the deficit causes: the same two
 *   symbols in the same order, ewram_2020000 at 0x44/0x40 and gBuffer at
 *   0x4c/0x48.
 *   tools/aligncmp.py: 18 of 37 aligned-equal (48.6%), 25 differing in 9 hunks.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/8011164.c asm/rom_9000/rom_108e4_c.s --func Func_8011164
 *
 * ======== BATCH 343, BRIEF B: BOTH CHEAP AXES ARE NOW CLOSED, AND THE ONE
 * ======== MECHANISM-DERIVED HYPOTHESIS LEFT STANDING IS REFUTED ========
 *
 * TWO CHEAP AXES CLOSED (tools/autosweep.py -- a recorded negative, do not
 * re-screen them):
 *   FLAG axis    all eight per-file groups.  -fno-gcse, -ffixed-r7,
 *                -fno-strict-aliasing, -fno-strength-reduce and
 *                -fno-rerun-loop-opt are EXACTLY INERT at 27; -O1 is 33,
 *                -fno-schedule-insns2 is 31 and -fno-rerun-cse-after-loop is 31,
 *                all worse.
 *   ANCHOR axis  12 statement boundaries at every nesting depth (autosweep's
 *                boundaries(), tools/autosweep.py:217, descends into the loop
 *                body, so the in-loop placements ARE covered).  10 measured, all
 *                inert at 27 except one at 26 WITH dsize = -4 -- which is a
 *                deeper misalignment, not an improvement, and autosweep says so.
 *                2 of the 12 produced no row: UNTESTED, not inert.
 *
 * ** REFUTED THIS BATCH: "stop cse pass one and the hoist becomes illegal."
 *
 * The hypothesis was worth stating because it attacks the OTHER conjunct of the
 * same gate the park's BOUND 1 only half-examined.  loop.c:826-837 is:
 *
 *     else if ((tem = loop_invariant_p (loop, src))
 *              && (dependencies == 0 || ...)
 *              && (VARRAY_INT (set_in_loop, REGNO (SET_DEST (set))) == 1
 *                  || (tem1 = consec_sets_invariant_p (...))))
 *
 * BOUND 1 proves the `loop_invariant_p` conjunct cannot be falsified for a Thumb
 * constant-pool MEM, and that proof still stands.  But the SECOND conjunct --
 * `set_in_loop[dest] == 1` -- is a different door, and this park's body DOES
 * assign `g` twice inside the loop (`g = gBuffer;` then `g += 2;`).  The reason
 * it is still one set by the time loop.c runs is that cse pass one rewrites the
 * second access: the park already records `cse.c:2074 use_related_value` turning
 * the second pool reference into `(plus 52 2)`, which leaves `g` with a single
 * set and makes it a movable.  cse.c:5741-5745 flushes cse's whole value table
 * at a volatile bare-ASM_OPERANDS insn, so a `__asm__ volatile ("" :: "i"(0))`
 * between the two accesses should have kept `g` set twice and killed the hoist.
 *
 * IT DOES NOT.  Measured, all against ref 37 / 34 insns / 80 bytes:
 *     flush between the two accesses (after `g += 2;`)  27 of 37, 32 insns, 76 B
 *     flush at the TOP of the loop body                 27 of 37, 32 insns, 76 B
 *     flush after `g = gBuffer;`                        29 of 37, 35 insns, 84 B
 *     flushes after `g = gBuffer;` AND after `g += 2;`  29 of 37, 35 insns, 84 B
 *     flushes before `g = gBuffer;` AND after `g += 2;` 28 of 37, 32 insns, 76 B
 *     the ASM_INPUT spelling `__asm__ volatile ("")`    27 of 37, 32 insns, 76 B
 *     `do { } while (0);` after `g = gBuffer;`          27 of 37, 32 insns, 76 B
 * The two rows that reach 35 instructions LOOK like the hoist was defeated, and
 * they were not: the stream still carries `ldr r6,=gBuffer` OUTSIDE the loop,
 * and the three extra instructions are `mov ip,r1` / `mov r1,ip` /
 * `movs r2,#128` / `add ip,r2` -- a register PARK IN ip, which is exactly the
 * failure mode the park's -ffixed-r7 instrument already showed.  They also pool
 * FOUR words.  So the flush changes the allocation and not the hoist, and the
 * `set_in_loop` door is shut for a different reason than cse: by the time loop.c
 * runs the second set is already gone, and a cse flush does not put it back. **
 *
 * ==== THE DECOMPOSITION, re-counted off both streams (sixth agreement) ====
 *
 *   ROM 34 instructions + 3 pool words = 37 encodings = 80 bytes
 *   ours 32 instructions + 3 pool words = 35 encodings = 76 bytes
 *
 *     head (prologue + both pointer setups + i = 0)   ROM 14   ours 15   +1
 *     loop body                                       ROM 17   ours 14   -3
 *     tail (pop / pop / bx)                           ROM  3   ours  3    0
 *                                                     ROM 34   ours 32   -2
 *
 *   INSIDE the loop the whole deficit is the gBuffer address work:
 *       ROM   ldr r5,=gBuffer / adds r3,r2,r5 / ldrh r3,[r3] / adds r5,#2
 *             / adds r3,r2,r5 / ldrh r3,[r3]                            (6)
 *       ours  ldrh r3,[r6,r2] / adds r3,r6,#2 / ldrh r3,[r3,r2]         (3)
 *   The head's +1 is specifically a THIRD POOL LOAD: the ROM takes two in the
 *   head (pool 0x44, 0x48) and one INSIDE the loop (0x4c); we take all three in
 *   the head.  `push {r5,r6,lr}` / `pop {r5,r6}` against `push {r5,lr}` /
 *   `pop {r5}` are same-count REPLACEMENTS, not extra instructions.
 *   ONE mechanism accounts for all three missing instructions, for the single
 *   in-loop pool reference and for `adds r5,#2` at once: the gBuffer constant
 *   must get NO hard register, so reload rematerialises it inside the loop and
 *   `reload_cse_move2add` (reload1.c:8840) collapses the second rematerialisation
 *   into `adds r5,#2`.
 *
 * REGISTER MAP, re-read this batch and sharper than the previous statement:
 *     ROM   r4 src, r1 dst, r0 i, r2 t, r3 scratch, r5 BOTH constants
 *     ours  r4 src, r0 dst, r5 i, r2 t, r3 scratch, r1 scratch, r6 gBuffer
 * The ROM touches r0-r5 only and never mentions r6 or r7 in any of its 34
 * instructions, and its r5 is REUSED FOR TWO UNRELATED VALUES -- 0x06004000 in
 * the head and gBuffer inside the loop.  So in the original build the gBuffer
 * constant held NO REGISTER OF ITS OWN AT ALL; it borrowed a dead one each
 * iteration.  Note also that the ROM gives `i` the ARGUMENT register r0 (dead
 * after `ands r3,r0`) where we give `i` the callee-saved r5 -- a second,
 * independent difference in the allocation that no spelling has moved either.
 *
 * ==== THE THREE BOUNDS, ALL STILL STANDING ====
 *
 * BOUND 1 -- NO SOURCE SHAPE CAN FALSIFY `loop_invariant_p` FOR A THUMB POOL
 * LOAD.  Both MEM doors are structurally shut and the arithmetic gate is not
 * close:
 *   * loop.c:3266 branches
 *       RTX_UNCHANGING_P (x) ? unknown_constant_address_altered
 *                            : unknown_address_altered
 *     and a pool MEM takes the FIRST arm.  `unknown_constant_address_altered`
 *     is set in exactly one place -- `note_addr_stored`, loop.c:3144 -- and ONLY
 *     for a BLKmode store whose destination is itself RTX_UNCHANGING_P.  This
 *     loop's stores are HImode, so it can never be set.
 *   * The `loop_store_mems` / `true_dependence` door is shut in alias.c:
 *       if (RTX_UNCHANGING_P (x) && ! RTX_UNCHANGING_P (mem)) return 0;
 *     A constant-pool READ can never conflict with a non-constant store.
 *   * The arithmetic gate, MEASURED: loop.c:1801-1803 is
 *       (threshold * savings * m->lifetime) >= insn_count
 *     with `threshold = (loop_info->has_call ? 1 : 2) * (1 + n_non_fixed_regs)`
 *     at loop.c:651.  `.08.loop` prints "Loop from 43 to 95: 13 real insns", so
 *     insn_count = 13 and threshold >= 20; for the gBuffer movable (savings 2,
 *     lifetime 6) the test is 20 * 2 * 6 = 240 >= 13.  It would take a
 *     240-instruction loop body to fail.
 *   >> AMENDED SCOPE (343): BOUND 1 is about the `loop_invariant_p` conjunct
 *      ONLY.  The `set_in_loop == 1` conjunct of the same `else if`
 *      (loop.c:830-836) is a genuinely separate door; it is shut here too, but
 *      for a reason that is NOT in BOUND 1 and NOT reachable by a cse flush
 *      (measured above).  State the bound with that conjunct named, so the next
 *      agent does not re-derive the distinction. <<
 *
 * BOUND 2 -- "greg has a free register and the original build did not" IS
 * REFUTED BY THE ROM'S OWN PROLOGUE.  `push {r5, lr}` and no mention of r6 or
 * r7 anywhere in the 34 instructions means the original build's greg had AT
 * LEAST TWO free low registers at the moment it declined to allocate the hoisted
 * constant.  Register scarcity cannot be the explanation, and "keep one more
 * value genuinely live across the loop body" is a DEAD END: adding pressure
 * makes greg reach for r7, it does not make it spill.  From `.18.greg` on THIS
 * body (authoritative; the batch-324 figures were for a different body and there
 * is no `57 in 7`):
 *     ;; 4 regs to allocate: 34 33 35 57
 *     dispositions  34 in 0   33 in 4   35 in 5   **57 in 6**
 *     ;; 57 conflicts: 33 34 35 37 50 52 53 55 56 57 1 2 3 13
 * r0 (34), r1 (55), r2 (50, 52), r3 (37, 53, 56), r4 (33) and r5 (35) are all
 * held by CONFLICTING allocnos, so r6 is simply the first free register.
 *
 * BOUND 3 -- THE ADDRESSING MODE IS A CONSEQUENCE, NOT A LEVER.  Batch 334
 * proposed it as a second independent lever (the ROM materialises each address
 * and uses immediate-offset loads; we use Thumb's register-offset form, one
 * instruction cheaper per load).  Batch 336 crossed 15 rows -- 5 address
 * spellings x 3 g-forms -- and EVERY ONE COMPILES TO A BIT-IDENTICAL OBJECT:
 * 27 of 37, 34/32, first index 0, aligned-equal 18, 25 diffs in 9 hunks in every
 * column.  A second 24-row cross (3 g-forms x 2 operand orders x 3 t-forms) is
 * also inert, 18 of the 24 bit-identical and the 6 two-base rows WORSE at 29.
 * cse2 canonicalises every spelling to the same RTL before reload sees it.
 *   >> THIS FUNCTION HAS EXACTLY ONE FREE VARIABLE: whether the gBuffer pool
 *      constant gets a hard register.  No further spelling work is worthwhile. <<
 *
 * Also still true: `local-alloc.c:886`'s `reg_equiv_replace` can NEVER fire for
 * a thumb symbol constant, because SET_SRC is a pool-load MEM
 * `(mem (symbol_ref "*.LC1"))` while the note is the bare
 * `(symbol_ref "gBuffer")`, so `rtx_equal_p` is always false.  And
 * `rtlanal.c:234 get_related_value` DOES return the base for a
 * `CONST (PLUS symbol_ref CONST_INT)`, which is why `cse.c:2074
 * use_related_value` rewrites the second pool load to `(plus 52 2)` and gives
 * reg 52 a third reference.  `local-alloc.c:871`'s REG_LIVE_LENGTH *= 2 halves
 * the constant's global priority, so it is already the cheapest allocno to lose
 * -- it just never loses while a register is free.
 *
 *   >> NAMED REMAINING CAUSE, unchanged and still the right question: why did a
 *      greg with r6 AND r7 free decline to allocate a hoisted
 *      reg_equiv_constant?  What is left is find_reg's exclusion set for allocno
 *      57 -- `reg_preferred_class`, `no_global_alloc_regs`, or a
 *      hard_reg_conflict on r6/r7 that our build does not have.  Read
 *      `.18.greg`'s find_reg trace; do not screen spellings. <<
 *
 * MEASURED INERT / WORSE, cumulative (production flags, ref 37 / 80 bytes):
 *   `g = gBuffer;` moved ABOVE the `t = *(u16 *)src * 4;` read .. 27 (inert)
 *   `unsigned short *` src/dst with 0x40 element strides ........ 27 (inert)
 *   the address named as its own pointer, `a = t + g; *(u16*)a` . 27 (inert)
 *   `g = gBuffer + t;` then `g += 2;` .......................... 27 (inert)
 *   `unsigned short gBuffer[]`, `g[0]` then `g++` .............. 27 (inert)
 *   one `q = gBuffer + t`, then `q[0]` / `*(q+2)` .............. 27 (inert)
 *   `g = gBuffer;` / `g = g + 2;` (this body) .................. 27 (inert)
 *   `gBuffer + t` and `gBuffer + 2 + t` inline ................. 29, -4
 *   `t + gBuffer` / `t + gBuffer + 2` (operand order) .......... 29, -4
 *   `gBuffer + (t + 2)` (move the 2 to the index) .............. 29, -4
 *   two hand-hoisted bases g0/g1 ............................... 29, -4
 *   `do { } while (i <= 0x3f)` ................................. 29, -4
 *   one scratch int reused for all the reads .................... 30, +4 (worse)
 *   all 39 rows of batch 336's two crosses ..................... 27 / 29
 *   all 12 anchor placements, batch 343 ........................ 27 (26 at -4)
 *   all eight per-file flag groups, batch 343 .................. 27 / 31 / 33
 *   all seven cse-flush placements, batch 343 .................. 27 / 28 / 29
 *
 * INSTRUMENT WARNING: -ffixed-rN IS NOT A USABLE INSTRUMENT on these bodies.
 * Denying a high register makes the body SHORTER, because the unused register
 * drops out of the prologue's push/pop block -- so the flag changes the very
 * region the measurement is about.  Under -ffixed-r7 gcc parks gBuffer+2 in `ip`
 * rather than rematerialising, which is not the ROM's mechanism.
 *
 * A DEVICE, kept as a figure ABOUT the blocker: two DISTINCT extern symbols
 * (`gBuffer` and a fictitious `gBuffer_PLUS2`) defeats cse's relation and reads
 * 28 WITH THE LENGTH EXACT -- but it pools FOUR words and allocates r6 AND r7,
 * so the relation is not the whole story.  Not a result; the ROM pools three.
 *
 * NOTE ON docs/agbcc-thumb-regoffset-bug.md: read its scope note first.
 * gcc296 emits register-offset addressing freely (1,156 sites in this tree) and
 * "the offset as a named local" is a recorded lever that EARNS that form; what
 * this park would need is its INVERSE, and the two must not be confused.
 *
 * NOTE FOR COORDINATORS: `src/non_matching/rom_9000/80111b4.c` is NOT a second
 * park for this function.  It is the park for Func_80111b4, a 248-encoding
 * camera update that merely CALLS this one (declared `extern` at its line 164);
 * both functions happen to live in this same reference .s.  It must never
 * appear in this function's `parks_retire`.
 *
 * PIECE STATUS, re-verified with tools/piece.py, tools/datacheck.py and
 * tools/split_s.py --dry-run this batch:
 *
 * asm/rom_9000/rom_108e4_c.s holds SEVEN .thumb_func_start and ALL SEVEN ARE
 * STILL PARKED -- Func_8010e14 136/201, Func_8010ff0 101/112, Func_80110e0
 * 52/63, this one 27/37, Func_80111b4 151/248, Func_80113e4 73/91,
 * Func_80114a0 55/97.  The piece is already sub-split into _a / _b / _c
 * (stage1.ld:228-230 for .text, :299-301 for .rodata).  This function is member
 * 4 of 7, so it is INTERIOR: ONE tools/split_s.py invocation writes THREE files
 * (_a = members 1-3, _b = this function, _c = members 5-7).  Func_80114a0 is
 * member 7 and its invocation writes only two.  The two are INDEPENDENT SINGLES
 * and must not be gated on each other.  (Earlier headers have counted this as
 * "two cuts" and "one cut"; the unambiguous statement is INVOCATIONS and FILES.)
 *
 * ** A PREREQUISITE NOBODY HAS RECORDED: THE SPLIT IS CURRENTLY REFUSED.
 * ** tools/split_s.py --dry-run on this function stops with
 * **     REFUSING to split asm/rom_9000/rom_108e4_c.s: 1 local label(s) would
 * **     cross files.  asm/rom_9000/rom_108e4_c_a.s references .L132cc,
 * **     defined in asm/rom_9000/rom_108e4_c_c.s
 * ** datacheck.py agrees and names the owner: the piece carries a .rodata
 * ** section, Func_8010e14 (member 1) READS .L132cc, and the label is defined in
 * ** the tail.  So landing ANY interior member of this piece needs
 * **     .global .L132cc
 * ** exported FIRST, gated on `make compare` on its own (a .global emits no
 * ** bytes), BEFORE the split runs.  The other six members read no data label.
 * ** CHECKED AND CLEAN: the piece has ZERO `.lcomm` and ZERO `.comm` directives,
 * ** so the batch-339 crossing class -- which split_s.py does NOT check for --
 * ** does not apply here.  (All three of this brief's other pieces are also
 * ** zero on both counts.) **
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
