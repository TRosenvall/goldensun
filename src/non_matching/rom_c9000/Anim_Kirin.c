/* Anim_Kirin (asm/rom_c9000/rom_eb754_a.s:93, 0x080eb754, 1026 ROM
 * instructions / 1095 encodings) --
 * NON-MATCHING, 1059 of 1095 encodings differ.
 *
 * READ THE FIGURE CORRECTLY.  1059 is objcmp's PRODUCTION-FLAG number and it is
 * SATURATED: our count is 1102 against the reference's 1095, SEVEN OVER, so the
 * figure is not a distance and must never be ranked against another function.
 *
 *   SIZE   2456 bytes against 2444 (+12).
 *   COUNT  1102 encodings against 1095 (+7).
 *   FRAME  0xb8 against the ROM's 0xb0 -- TWO SLOTS OVER.  Carried in every
 *          probe table below; it is the ranking instrument on this function.
 *   tools/aligncmp.py, reported SEPARATELY and never on the claim line:
 *       aligned-equal 647 of 1095 = 59.1%, 524 differing/ins/del in 190 hunks
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Kirin.c \
 *     asm/rom_c9000/rom_eb754_a.s --func Anim_Kirin
 *
 * SPLIT SHAPE: NONE NEEDED.  tools/datacheck.py prints NOTHING for this stem and
 * `tools/split_s.py --dry-run asm/rom_c9000/rom_eb754_a.s Anim_Kirin` answers
 * "holds only Anim_Kirin and no data; convert it directly, no split needed".
 * The two data labels it reads, `.Leef56` and `.Leef5f`, live in
 * asm/rom_c9000/rom_eb754_c_c.s and are ALREADY `.global` there (lines 1744-5),
 * so there is no asm prerequisite of any kind.  THIS IS THE ONLY ONE OF BRIEF
 * G's FOUR WITH ZERO ASM WORK IN FRONT OF IT -- see the triage correction below.
 *
 * SHIMS: ZERO.  tools/shimcount.py prints the filename and nothing else.
 * PIN-FREE.  The two `__asm__(".Leef56")` / `__asm__(".Leef5f")` declarations are
 * the decls.h symbol-alias convention, not register pins.
 *
 * ================================================================
 * THE FRAME, FOUR GREPS AND A FIFTH -- AND IT CLOSES TO THE BYTE
 * ================================================================
 *
 * G1 `sub sp,#imm`      : 0xb0 = 176.  Under the 508-byte Thumb-1 cap.
 * G2 `(add|sub) sp,rN`  : NONE.  (None on any of brief G's four -- all four
 *                         frames are 0x9c..0x164, so G2 cannot fire.  G2 is
 *                         still the right grep; it just has no work here.)
 * G3 `mov rX,sp` x3 (674, 677, 695) and `add rX,sp,#K` x7 (0xa0, 0x5c, 0x38,
 *    0x94, 0x64, 0x54, 0x94) -- 10 SITES.
 * G4 `str rX,[sp]` with no load: 7 sites at +0x00 AND 5 at +0x04.
 *
 * *** G4 HAS A SILENT BUG IN ITS USUAL SPELLING AND IT COST ME +0x04. ***
 * This tree's disassembly prints stack offsets below 0x10 in DECIMAL with NO
 * `0x` prefix -- `[sp, #4]`, not `[sp, #0x4]`.  A histogram keyed on `#0x`
 * DROPS slots +0x04 and +0x08 without a word of complaint.  My first map read
 * "+0x00 only, 4 bytes of outgoing args"; the truth is +0x00 AND +0x04, 8 bytes,
 * because the blitter is called with SIX arguments.  Key the offset regex
 * `#(0x[0-9a-f]+|\d+)` or the accounting cannot close.  (POSIX awk also has no
 * `strtonum`, so do this in python -- an awk version silently printed nothing.)
 *
 * G5, A FIFTH FORM THE FOUR-GREP LIST DOES NOT NAME: `mov rX,sp` FOLLOWED BY
 * `add rX,#K`.  Three of Kirin's ten G3 sites are this pair (r4,#0x74 at 674;
 * r7,#0x84 at 695; and BaseAnim_Meteor's r2,#0x94 in its prologue).  The
 * elevation doc mentions it only inside the "phantom holes" caution; it belongs
 * in the grep list, because `add rX,sp,#K` alone misses these aggregates
 * entirely.
 *
 * *** G3 IS AN UPPER BOUND, NOT A COUNT, AND THE REASON IS THE MISSING
 * ADDRESSING MODES. *** Thumb-1 has no sp-relative `ldrh`/`strh`/`ldrb`/`strb`,
 * so EVERY SUB-WORD STACK SCALAR must materialise `add rX, sp, #K` before it can
 * be touched -- indistinguishable at the grep from a genuine aggregate.  Kirin's
 * +0x38 is exactly that (see the INERT (a) bound below): an ordinary `int` slot
 * read narrow, counted by the grep as an aggregate.  RESOLVE EVERY SITE BY
 * READING THE REGION, and do it BLOCK-AWARE -- stop at any label or branch,
 * because a scan that crosses a loop head reads a walking pointer as a scalar.
 * tools-free resolver used here: scratch_elev/b313g/aggres.py.  Classify on the
 * FIRST USE of the materialised register:
 *     sub-word load/store at [rX]        -> SUB-WORD SCALAR, not an aggregate
 *     `cmp rY, rX`                       -> LOOP END SENTINEL, not an object
 *     varying offsets / passed as an arg
 *       / copied to a callee-saved reg
 *       / walked                         -> GENUINE AGGREGATE
 * Four sites of Kirin's ten resolve only by reading, because the use is past a
 * label; a mechanical scan must report them unresolved rather than guess.
 *
 * MEASURED OVER-COUNT ACROSS BRIEF G's FOUR (raw G3 sites -> true aggregates):
 *     BaseAnim_Bite_Sting   6 -> 5      Anim_Kirin        10 -> 7
 *     BaseAnim_Meteor      14 -> 8      Anim_Procne       19 -> 10
 * The inflation GROWS with the raw figure, so the figure is least trustworthy
 * exactly where it looks most alarming.  AND ALL FOUR TRUE COUNTS ARE CONFIRMED
 * BY A CLOSED BYTE ACCOUNTING, which is the whole point of the check:
 *     Bite_Sting  8 + 23w(92) + 5 aggr(56)  = 156 = 0x9c
 *     Kirin       8 + 19w(76) + 7 aggr(92)  = 176 = 0xb0
 *     Meteor      8 + 18w(72) + 8 aggr(204) = 284 = 0x11c
 *     Procne      8 + 24w(96) + 10 aggr(252)= 356 = 0x164
 * Four for four, to the byte, with no slack anywhere to hide a miscount.
 *
 * *** AND HERE IS THE CHECK THAT VALIDATES AN AGGREGATE COUNT. *** A grep
 * cannot be verified, but a BYTE ACCOUNTING CAN: 8 + 76 + 92 = 176 = 0xb0
 * closes only if the aggregate set is exactly the seven below.  Six or eight
 * objects cannot be made to sum to the frame.  Never report an aggregate count
 * that has not been closed against `sub sp,#imm`.
 *
 * AND THE CONVERSE OF THE SENTINEL CASE -- G3 OVERCOUNTS HERE TOO.  Two of Kirin's ten "aggregate" sites are
 * ARRAY-LOOP END SENTINELS, not aggregate bases:
 *   `mov r2,sp / add r2,#0x82`  is `hit + 14`, the clear loop's bound;
 *   `add r6,sp,#0x94`           is `cols + 16`, the fill loop's bound.
 * 0x82 is not an object at all.  0x94 happens to ALSO be a real aggregate
 * (`pos`, addressed at line 881), so one site is a sentinel AND a base.
 * SO: 10 SITES, 7 DISTINCT AGGREGATES.  Count OBJECTS, not sites.
 *
 * PHANTOM HOLES AND PHANTOM LOAD-ONLY SLOTS, THE SAME TRAP TWICE.  A
 * `[sp, #imm]` census CANNOT SEE A STORE MADE THROUGH A MATERIALISED BASE
 * REGISTER.  BaseAnim_Bite_Sting shows it cleanly: its census calls +0x84 and
 * +0x88 LOAD-ONLY (4 and 2 loads, zero stores), which reads as two frame holes.
 * They are neither -- they are the first two words of the 12-byte aggregate
 * BASED at +0x84, whose stores all go through the `mov r2,sp / add r2,#0x84`
 * base register that the census is blind to.  Re-check every
 * "loaded but never stored" slot against the `add rX, sp` sites BEFORE calling
 * it a hole.  Kirin's four store-only slots are the mirror case and all four
 * are real: +0x38 (the narrow-read scalar) and +0x54/+0x5c/+0x60 (the two
 * 8-byte scale structs, read only through their passed-in base).
 *
 * THE ACCOUNTING, which names every one of the 176 bytes:
 *     +0x00 .. +0x07   8   outgoing argument area (the 6-arg blitter calls)
 *     +0x08 .. +0x50  76   19 spilled scalar words
 *     +0x54 .. +0xaf  92   the 7 aggregates
 *                    ---
 *                     176 = 0xb0   EXACT
 *
 * ================================================================
 * THE TWO RULES THE BRIEF ASKED ABOUT -- BOTH HOLD
 * ================================================================
 *
 * (1) THE REVERSED-AGGREGATE RULE HOLDS EXACTLY, and on this frame it is not a
 * heuristic but a measured identity.  Declared in the order
 *
 *     scale2, scale1, spr2, hit[14], cols[16], pos, spr1
 *
 * the candidate's aggregates land at +0x5c, +0x64, +0x6c, +0x7c, +0x8c, +0x9c,
 * +0xa8 -- ASCENDING in declaration order, i.e. first-declared LOWEST, the
 * reverse of the scalars.  The ROM's are at +0x54, +0x5c, +0x64, +0x74, +0x84,
 * +0x94, +0xa0.  THE INTERNAL LAYOUT IS IDENTICAL: 8, 8, 16, 16, 16, 12, 16 =
 * 92 bytes on both sides, same order, same sizes, same padding (hit[14] is
 * padded to 16 by the 4-byte stack alignment on both sides).  The whole block
 * is displaced by exactly +8 because the scalars below it are two words too
 * many.  A 7-aggregate / 10-site frame does not weaken the rule at all.
 *
 * (2) THE SLOT ACCESS-COUNT TABLE IS THE INSTRUMENT, and it localised the
 * residue in one pass where the aligned figure could not.  Reading both maps
 * descending from the top and matching on the str/ld signature:
 *
 *     ROM            OURS           quantity
 *     +0x50 s1/l5    +0x58 s1/l5    buf
 *     +0x4c s1/l25   +0x54 s1/l22   base      (-3 loads, see cse1 below)
 *     +0x48 s1/l5    +0x50 s1/l5    blit
 *     +0x44 s1/l4    +0x4c s1/l4    gfx
 *     +0x40 s2/l4    +0x48 s2/l4    sx
 *     +0x3c s2/l5    +0x44 s2/l4    climb     (-1 load, see the ring below)
 *     +0x38 s1/l0    +0x40 s1/l0    saved
 *     +0x34 s1/l2    +0x3c s1/l2    cam
 *     +0x30 s3/l4    +0x38 s3/l4    rate
 *     +0x2c s2/l2    +0x34 s2/l2    sway
 *     +0x28 s1/l1    +0x30 s1/l1    c28
 *     +0x24 s1/l1    +0x2c s1/l1    c24
 *     ----- the top TWELVE agree in order and in signature -----
 *     +0x20 s1/l1    +0x28 s2/l3    divergence starts here
 *
 * So the declaration order of the twelve longest-lived scalars is CONFIRMED
 * CORRECT, not guessed.  Below c24 the ROM runs c20, front, hitp, colp, curt,
 * poff and one pass-created word at +0x08; ours hoists `front` above `c20` and
 * carries TWO EXTRA pass-created words at the bottom.  That is the entire
 * residue, and it is an allocation residue: 21 spilled words against 19.
 *
 * THE FRAME DID NOT MOVE UNDER ANY OF THE EIGHT PROBES.  Every variant below
 * compiles to `sub sp, sp, #184`.  That is why the frame is a column here --
 * see the K5 trap.
 *
 * ================================================================
 * EVERY PROBE, WITH ITS FIGURE.  THE NEGATIVES ARE THE POINT.
 * ================================================================
 *
 *   id  change                              size  count  frame  aligncmp
 *   --  ----------------------------------  ----  -----  -----  --------
 *   K0  first candidate off the reference   +12    +7    0xb8    59.3%
 *   K1  `iwram_3001ad0[2] = saved;` with
 *       NO `&` (plain assignment)           +12    +7    0xb8    59.3%  INERT
 *   K2  drop the shared `int mask`, use
 *       literals 0x1f / 0xff                 +4    +3    0xb8    56.8%  (!)
 *   K3  three block-scoped `int mask`        +4    +3    0xb8    56.7%  (!)
 *   K4  base via char-pointer subtraction
 *       + `c28` reused in c24               +12    +7    0xb8    59.1%  KEPT
 *   K5  `State **slot` named FUNCTION-WIDE   -4     0    0xb8    54.3%  REJECT
 *   K6  `slot` named in the wave-front only +16    +9    0xb8    58.9%
 *   K7  `slot` named in the PROLOGUE only   +12    +7    0xb8    59.1%  INERT
 *   K8  K4's base fix ALONE (no c28)        +12    +7    0xb8    59.1%  = K4
 *
 * *** K5 IS A RUNG-3 TRAP, CAUGHT BY THE FRAME AND BY aligncmp TOGETHER. ***
 * Naming `slot` function-wide makes the ENCODING COUNT EXACT -- 1095 of 1095 --
 * and brings the size to within 4 bytes.  It is WRONG.  aligncmp falls 59.1% ->
 * 54.3%, a 4.8-point collapse, and the frame stays 8 bytes over.  The exactness
 * is the candidate's own 7 extra instructions cancelling a 7-instruction deficit
 * elsewhere.  This is the cleanest demonstration in the family that COUNT
 * EXACTNESS AND STRUCTURE CAN MOVE IN OPPOSITE DIRECTIONS, and it is why "carry
 * frame size as a column in every probe table" is not bureaucracy.  Had I ranked
 * on objcmp alone I would have installed K5.
 *
 * *** K2/K3 ARE THE SAME TRAP IN MINIATURE, POINTING THE OTHER WAY. ***
 * Dropping the named `mask` IMPROVES both axes (+12/+7 -> +4/+3) and LOSES 2.5
 * aligncmp points.  Removing it does NOT free a stack slot -- the frame is 0xb8
 * either way, so `mask` was register-resident in both spellings and the four
 * instructions it costs are rematerialisation, not spill traffic.  The named
 * form is kept because the structure is better; the count is saturated and is
 * not the arbiter.  (This is the d9ab8 `int mask = 0x7f;` idiom holding on a
 * fourth function, with the caution that it is a COUNT cost.)
 *
 * ================================================================
 * WHAT PAID, WHAT IS INERT, AND TWO BOUNDS WORTH AS MUCH
 * ================================================================
 *
 * PAID (verified instruction-for-instruction against the reference, not just by
 * figure): `base = *(unsigned char **)((char *)iwram_3001ef0 - 4);`.
 * A NEGATIVE ARRAY INDEX IS NOT THE NEGATIVE-OFFSET SPELLING.
 * `iwram_3001ef0[-1]` compiles to THREE instructions --
 *     movs r3,#4 / negs r3,r3 / ldr r3,[r5,r3]
 * -- because gcc materialises the index and uses the reg+reg mode.  The ROM has
 * TWO:
 *     subs r3,r5,#4 / ldr r3,[r3,#0]
 * and the char-pointer subtraction reproduces them exactly.  This is the bank's
 * recorded negative-offset spelling (d9ab8_StatDown's `*(void **)((char *)g -
 * 0x6c)`) and the array-index form is a trap that looks more idiomatic and is
 * one instruction worse.  It costs 0.2 aligncmp points, which is NOISE at this
 * length -- the doc's own precedent is a reorder that moved aligned by two
 * encodings while correcting a seven-encoding misassignment -- so the verified
 * form is kept over the better-reading figure.
 *
 * INERT, MEASURED BYTE-IDENTICAL (bounds on three rules):
 *
 *  (a) `*(unsigned short *)&saved` and `iwram_3001ad0[2] = saved` ARE THE SAME
 *      CODE.  Both emit `add r4,sp,#K / ldrh r4,[r4] / strh r4,[r3,#4]`.
 *      THE MECHANISM IS NOT ADDRESS-TAKING, IT IS THE MODE: Thumb-1 `ldrh` has
 *      NO sp-relative addressing mode, so a HImode read of ANY stack slot must
 *      materialise sp+K into a register first.  The ROM's lone store-only slot
 *      at +0x38 with an `add rX,sp,#0x38` beside it is therefore an ORDINARY
 *      `int` spill slot that happens to be read narrow -- NOT an address-taken
 *      aggregate.  It is the int-carrier lever read in reverse: the carrier
 *      forces the SImode store, the destination's u16 type forces the HImode
 *      load, and the missing addressing mode forces the third instruction.
 *      Do not spend a probe on the `&`.
 *  (b) Naming `slot` in the prologue only (K7) is byte-identical to not naming
 *      it (K4).  cse1 commons `base + 0x7828` either way.
 *  (c) Reusing `c28` inside c24's expression (`(c28 + climb) * 4 + 0x30` for
 *      `(climb * 2 + climb) * 4 + 0x30`) is byte-identical.  gcc keeps c28 in a
 *      register and never spills it, so the reuse has nothing to bite on.
 *
 * NEGATIVE, AND IT ANSWERS A STANDING FAMILY QUESTION:
 * `base + 0x7828` MUST NOT BE A NAMED LOCAL ON Anim_Kirin.  Function-wide
 * -4.8 aligncmp, wave-front-only -0.2, prologue-only inert.  KIRIN SIDES WITH
 * Anim_Gaia (471 -> 505), NOT with Anim_Ragnarok's named `slot`.  The ROM's own
 * registers are what made the prologue-only hypothesis worth testing and they
 * were misleading: the reference DOES hold base+0x7828 in r6 across the
 * prologue (`add r6,r3,r4 / str r0,[r6]` ... `ldr r0,[r6] / bl Func_80d6750`),
 * so naming it there looked forced.  It is not -- gcc produces that register
 * form from the plain spelling unprompted.  Per-site, not per-family, and not
 * readable off the reference's register usage either.
 *
 * gcse IS NOT THE PASS.  `-fno-gcse` as a diagnostic leaves the sl parking in
 * place (65 `sl` references against 64) and does not move the frame.  So the
 * commoning of `base + 0x7828` into a callee-saved high register is cse1 or the
 * allocator, not gcse.  Run the flag before naming the pass -- the doc says so
 * and here it would have been named wrongly.
 *
 * ================================================================
 * WHAT IS ALREADY RIGHT AND READ OFF THE REFERENCE -- DO NOT RE-DERIVE
 * ================================================================
 *   - THE TWO FRAME LOOPS HAVE DIFFERENT SHAPES.  Loop one is a `goto` loop
 *     entered at its TEST (`b .Lebc56`), with TWO separate back-branches to the
 *     body (`(gKeyRepeat & 3) == 0` and `frame <= 0x10`); loop two is a plain
 *     do-while entered at its TOP with the test at the bottom.  Writing loop one
 *     as a while/do-while cannot produce the two back-edges.
 *   - `scale1 = Data_edad8;` / `scale2 = Data_edae0;` are STRUCT ASSIGNMENTS from
 *     8-byte data blobs, not two subscript stores.  gcc's own block move emits
 *     the ROM's `ldr [+4] / ldr [+0] / str [+0] / str [+4]` -- HIGH WORD LOADED
 *     FIRST.  Two element assignments give low-then-high and cannot match.
 *     (The brief's 557 -> 579 note, holding on a fifth function.)
 *   - *** THE RAW MNEMONIC CENSUS IS THE WRONG INSTRUMENT FOR LOOP FORM, AND
 *     ON THIS FUNCTION IT POINTS THE WRONG WAY. ***  The raw counts are
 *     bne=33 beq=5 ble=16 bgt=8 bge=1 bhi=2, blt=0 -- "33 bne against 27
 *     signed", which reads as mixed and as a reason NOT to import Gaia's `!=`
 *     rule.  THAT INFERENCE IS FALSE.  A mnemonic census counts COMPARISONS;
 *     loop form is decided only by the BACKWARD EDGES.  Classified
 *     (scratch_elev/b313g/backedge.py -- a branch whose target label is defined
 *     EARLIER than the branch):
 *         Anim_Kirin has 19 BACKWARD EDGES: 16 close on `bne`, 3 are
 *         unconditional `b`, and ZERO CLOSE ON A SIGNED COMPARE.
 *     So EVERY loop in this function is a `!=` loop and Gaia's rule transfers
 *     COMPLETELY.  The 27 signed compares are clamps (`if (rate > 0x18)`),
 *     signed-division corrections (`p->vy * 48 / 64`) and frame-phase tests
 *     (`frame <= 0x95`) -- none of them closes a loop.
 *     The candidate was already right on this axis because the loops were read
 *     off the reference's actual back-edges rather than off the census; every
 *     `do {...} while` in it is `!=`.  The census only ever endangered the
 *     PROSE.  Measured the same way across brief G's four:
 *         Anim_Kirin           19 back-edges, 16 bne,  0 signed, 3 b
 *         BaseAnim_Meteor      23 back-edges, 20 bne,  1 signed, 2 b
 *         Anim_Procne          19 back-edges, 15 bne,  0 signed, 4 b
 *         BaseAnim_Bite_Sting  16 back-edges, 13 bne,  1 signed, 2 b
 *     All four are `!=`-closing. Classify back-edges; never rank on mnemonics.
 *     The two `bhi` ARE still the two UNSIGNED range guards and are the only
 *     places a cast belongs: `(unsigned)(frame - 0x18) <= 0x1f` and
 *     `(unsigned)(frame - 8) <= 0x17`.
 *   - `*(int *)(base + 0x77a8) = frame;` at `frame == 8` stores the VARIABLE
 *     (the ROM's `str r4,[r3]` reuses the compare's register), the Ragnarok rule.
 *   - the `wob` sin() shift is DUPLICATED IN BOTH ARMS, 6 for frame <= 8 and 5
 *     for frame > 8, each arm recomputing `(frame << 11) +/- 0x4000` from
 *     scratch.  Hoisting `a` above the if computes it once and loses two sites.
 *     The `bgt / b` pair between them is a POOL SKIP, not a third block.
 *   - `+0x08` is PASS-CREATED on both sides, not a declared quantity: it is
 *     `wob` saved across the `_UpdateSprite` call in loop two's inner loop
 *     (`str r4,[sp,#8]` ... `ldr r4,[sp,#8]`).  18 declared scalars + 1 = the
 *     ROM's 19.  Counting it as a variable breaks the accounting.
 *   - `Data_ede48[r - 1]` with `r * 2` hoisted into a register as the sixth
 *     argument -- the d9ab8 `w - 1` / `w * 2` pair, third sighting.
 *   - the wave-front loop's bound `(*slot)->f14` is RE-READ every iteration and
 *     guarded before entry; that entry guard is jump.c's duplicate_loop_exit_test
 *     copy, so it is a real `while`, never a do-while.
 *   - `(0xa0 << 19)`, `(0xef << 7)`, `(0xe1 << 7)`, `(0x90 << 3)`, `(0x90 << 15)`,
 *     `(0xe0 << 16)`, `(0xff << 16)`, `(0xe0 << 2)` all left UNFOLDED: each is
 *     gcc's own `mov #k / lsl #n` const synth and folding them pools a word.
 *   - `p->vy * 48 / 64` as a SIGNED divide (the ROM's `cmp #0 / bge / add #0x3f /
 *     asr #6` bias), and `(unsigned)Random() % 0x30` as UNSIGNED (`__umodsi3`)
 *     while `i % 3` is SIGNED (`__modsi3`) eleven lines later.  Both remainders
 *     are in this function and they have different signedness.
 *
 * ================================================================
 * TWO MNEMONIC-GREP TRAPS SCREENED, BOTH CLEAN, BOTH WORTH RECORDING
 * ================================================================
 *
 * THE TYPE-CARRIER SCREEN PASSES.  Reference `ldrsh` = 6, ours = 6, and the six
 * sites correspond one-for-one; `lsl #16` is 11 both sides and `lsr #16` is 0
 * both sides.  So the `unsigned short v = *src++;` defect (which costs an
 * `ldrsh` + `lsl #16` + `lsr #16` triple per site) is NOT present here: the
 * `*(short *)((char *)p + 6)` spellings carry the right type already.
 *
 * BUT THE SAME GREP SHOWS `ldrh` 20 OURS AGAINST 6 REFERENCE, AND THAT IS NOT
 * A DEFECT.  Our six DATA `ldrh` match the reference's six site for site.  The
 * other fourteen are `ldrh rX, .LNNN` POOL LOADS, which ASSEMBLE IDENTICALLY to
 * the `ldr rX, .LNNN` the reference listing prints -- the behaviour already
 * recorded in src/non_matching/rom_c9000/80ecef4.c.  The arithmetic settles it
 * independently: the candidate is +7 encodings in total, so fourteen extra
 * instructions is impossible.
 * *** THEREFORE A RUNG-8 PER-OPCODE HISTOGRAM MUST BE TAKEN OVER ENCODINGS
 * (objdump -dz), NEVER OVER THE ASSEMBLER LISTING TEXT. ***  `ldr`/`ldrh` is a
 * second listing-level aliasing on top of the known `.call_via` one.
 *
 * AND THE `.call_via` CORRECTION FACTOR FOR THIS FAMILY, since every one of
 * brief G's four uses the macro and none of them has an expanded `mov ip,pc`
 * in the listing.  A raw-text histogram under-counts `mov` AND `bx` by one per
 * site:   Anim_Kirin 5,  BaseAnim_Meteor 9,  Anim_Procne 14,
 *         BaseAnim_Bite_Sting 9.
 * Expand the veneers before trusting a `mov` or `bx` column.
 *
 * ================================================================
 * TWO MORE FRAME-IDIOM RESULTS, ONE NEGATIVE
 * ================================================================
 * THE SIXTH IDIOM, `mov rX,#K` THEN `add rX,sp` (immediate first), DOES NOT
 * OCCUR IN ANY OF BRIEF G's FOUR -- zero sites across all four bodies.  The
 * idiom is real and belongs in the recipe; it simply has no work here, so a
 * frame reading of these four that omits it loses nothing.
 *
 * THE SEVENTH CLASSIFICATION CASE -- an ADDRESS-TAKEN SCALAR, which is neither
 * an aggregate nor a sub-word scalar -- has exactly ONE sighting in brief G's
 * four, and it is NOT in the `f(&a)` form: BaseAnim_Meteor's +0x094 is a 4-byte
 * slot whose ADDRESS is computed (`mov r2,sp / add r2,#0x94`) and STORED into
 * slot +0x03c, which is then the function's maxreload at 28 loads.  `p = &a`,
 * not `f(&a)`.  Every `add rX,sp,#K`-into-an-argument-register site in all four
 * functions resolves to a 12-byte vec3 (GetBattleActorPos3, Func_80e3944), so
 * the `f(&a)` scalar form is absent here.  Both forms need the same handling in
 * a classifier; only the second is a false aggregate at the grep.
 *
 * ================================================================
 * WHAT TO TRY NEXT -- the residue is 2 pass-created spill words
 * ================================================================
 * The aggregate block is right, the outgoing area is right, the top twelve
 * scalars are right in order and signature.  What is left is register pressure:
 * cse1 parks `base + 0x7828` in a callee-saved high register function-wide,
 * which is where `base`'s three missing loads went, and that costs two spill
 * words elsewhere.  Naming the quantity is measured negative three ways (K5-K7)
 * and `-fno-gcse` is measured inert, so the remaining handle is to make the
 * address a loop.c strength-reduction product rather than a cse1 common --
 * the documented `c->v[k]`-instead-of-`q = &c->v[2]` move.  The candidate
 * reads `(*(State **)(base + 0x7828))->f14` as a loop BOUND, which is the one
 * site where an induction-variable spelling is available.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

typedef struct { int a, b; } Scale;
typedef struct { int x, y, z; } Vec;

extern void *iwram_3001ef0[];
extern u16 iwram_3001ad0[];
extern int gPhysVec[];
extern void *gPtrs[];
extern Part gBuffer[];
extern Part ewram_2010018[];
extern unsigned short Data_ede48[];
extern Scale Data_edad8;
extern Scale Data_edae0;
extern unsigned char Leef56[] __asm__(".Leef56");
extern unsigned char Leef5f[] __asm__(".Leef5f");
extern volatile unsigned int gKeyRepeat;

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void Func_80c9048(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void AnimTransitionOut(int a, int b);
extern void _AnimTransitionIn(int a, int b, int c);
extern void CreateSummonSprite(int a, int b, int c);
extern void Func_80d6750(State *s);
extern void Func_80d67dc(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void WaitFrames(unsigned int n);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int  Random(void);
extern int  sin(int a);
extern int  cos(int a);
extern void _PlaySound(int id);
extern void _UpdateSprite(void *sprite, int *pos, Scale *scale, int mode);
extern void _DeleteSprite(void *sprite);
extern void GetBattleActorPos3(int id, Vec *out);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void _Func_80bd7dc(int a);
extern void gfree(int tag);

void Anim_Kirin(State *st)
{
    Scale scale2;
    Scale scale1;
    int spr2[4];
    unsigned char hit[14];
    unsigned char cols[16];
    Vec pos;
    int spr1[4];

    void *buf;
    unsigned char *base;
    DrawFn blit;
    unsigned char *gfx;
    int sx;
    int climb;
    int saved;
    int *cam;
    int rate;
    int sway;
    int c28;
    int c24;
    int c20;
    int front;
    unsigned char *hitp;
    unsigned char *colp;
    int curt;
    int poff;

    int frame;
    int i;
    int mask;
    void **pp;
    Part *p;

    buf  = iwram_3001ef0[0];
    base = *(unsigned char **)((char *)iwram_3001ef0 - 4);
    gfx  = (unsigned char *)iwram_3001ef0[1];
    *(State **)(base + 0x7828) = st;
    AnimStart(0);
    Func_80c9048();
    REG_BG2CNT = 0x784;
    *(volatile unsigned short *)(0xa0 << 19) = 0;
    *(volatile unsigned short *)((0xa0 << 19) + 2) = 0;
    *(int *)(base + (0xef << 7)) = 0;
    StartTask(Task_BlitAnim, 0x90 << 3);
    AnimTransitionOut(1, 0);
    CreateSummonSprite(9, 0x175, 1);
    gPhysVec[4] = 0xf0;
    Func_80d6750(*(State **)(base + 0x7828));
    REG_WININ = 0x2737;
    REG_WIN0H = 0xca;
    WaitFrames(1);
    _AnimTransitionIn(1, 0x3a, 0);
    AnimTransitionOut(1, 1);
    LoadVFXFile(FILE_73, gfx, 0, 0);
    LoadVFXFile(FILE_95, base, 1, 1);
    REG_DISPCNT = 0x7741;
    REG_BG2PA = 0x80;
    REG_BLDALPHA = 0x100e;
    REG_BLDCNT = 0x3f44;
    sx = 0;
    climb = 0;
    saved = iwram_3001ad0[2];
    cam = (int *)iwram_3001ef0[4];
    rate = 0;
    *(int *)(base + (0xef << 7)) = 1;

    *(int *)(base + 0x7784) = sx;
    cam[4] = 1;
    p = (Part *)(base + (0xe1 << 7));
    i = 0;
    mask = 0x1f;
    do {
        p->x  = (Random() & mask) + 0x10;
        p->y  = ((Random() & mask) + 0x30) << 16;
        p->vy = ((Random() & mask) - 0x10) << 16;
        p->t  = (int)((unsigned int)Random() % 0x30) + 2;
        i++;
        p++;
    } while (i != 0x40);

    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
    blit = (DrawFn)gPtrs[0x2e];
    REG_BG2CNT = 0x786;
    frame = 0;
    goto test1;

body1:
    if ((unsigned int)(frame - 0x18) <= 0x1f) {
        rate++;
    }
    if (rate > 0x18) {
        rate = 0x18;
    }
    if (frame <= 0x87) {
        iwram_3001ad0[2] -= rate;
        climb += rate;
    }
    if (frame <= 0x95) {
        int shift;
        scale1 = Data_edad8;
        shift = 0;
        if (frame > 0x67) {
            shift = (frame << 4) - 0x680;
        }
        if ((unsigned int)(frame - 8) <= 0x17) {
            sx = sx + rate - 8;
        }
        if (frame > 7) {
            int amp = 0x60;
            int a;
            if (frame <= 0x68) {
                amp = 0x20;
            }
            a = ((frame << 10) - 0x2000) & 0xffff;
            if (a > 0x8000) {
                a += -0x8000;
            }
            sway = (amp * sin(a)) >> 16;
            if ((frame & 0x1f) == 8) {
                *(int *)(base + 0x77a8) = 4;
            }
        }
        spr1[3] = 0;
        spr1[1] = 0xff << 16;
        pp = (void **)(base + 0x77d8);
        i = 0;
        do {
            spr1[0] = ((sx + Leef56[i] - shift) << 16) + (0xe0 << 16);
            spr1[2] = ((Leef5f[i] - sway) << 16) + (0x90 << 15);
            _UpdateSprite(*pp++, spr1, &scale1, 0);
            i++;
        } while (i != 9);
    }
    if (frame <= 0x1a) {
        int n = frame * 8;
        int r = climb + 4;
        if (r > 0xa) {
            r = 0xa;
        }
        if (n > 0x40) {
            n = 0x40;
        }
        i = 0;
        if (n != 0) {
            int r2;
            c28 = climb * 2;
            c24 = (c28 + climb) * 4 + 0x30;
            c20 = r / 2;
            r2 = r * 2;
            do {
                int ang = i << 10;
                int x = ((c28 + 8) * sin(ang) >> 16) + climb;
                int y = (c24 * cos(ang) >> 16) + 0x40;
                blit(buf, gfx + Data_ede48[r - 1], x + 0x60 - c20, y - r, r, r2);
                i++;
            } while (i != n);
        }
    }
    if (frame == 0x18) {
        *(int *)(base + (0xef << 7)) = 2;
        *(int *)(base + 0x7784) = 0x32;
    }
    if (frame == 0x1c) {
        REG_BG2CNT = 0x784;
    }
    if (frame > 0x11) {
        p = (Part *)(base + (0xe1 << 7));
        i = 0;
        mask = 0x1f;
        do {
            if (p->t == 0) {
                int w = i % 3 + 1;
                int w2 = w * 2;
                int vy;
                blit(buf, gfx + Data_ede48[w - 1], p->x,
                     *(short *)((char *)p + 6) - w, w, w2);
                vy = p->vy;
                p->x += 2;
                p->y += vy;
                p->vy = vy * 48 / 64;
            } else {
                p->t = p->t - 1;
            }
            if (p->x > 0x80 || p->t == 1) {
                p->x  = (Random() & mask) + sx + 0xac;
                p->y  = ((Random() & mask) - sway + 0x38) << 16;
                p->vy = ((Random() & mask) - 0x10) << 15;
            }
            i++;
            p++;
        } while (i != 0x30);
    }
    if (frame > 0x1f) {
        int d = (frame - 0x20) / 2;
        int y;
        int top;
        if (d > 0x28) {
            d = 0x28;
        }
        i = 0;
        y = 0;
        top = 0x78;
        do {
            int g = Random() & 3;
            blit(buf, base + ((g * 2 + g) << 9), top - d, y, 0x30, 0x20);
            i++;
            y += 0x12;
        } while (i != 6);
    }
    {
        int *sh = (int *)(base + 0x77a8);
        if (*sh > 0) {
            *sh = *sh - 1;
            iwram_3001ad0[3] = (Random() & 7) + 0x1c;
        } else {
            iwram_3001ad0[3] = 0x20;
        }
    }
    *(int *)(base + 0x7824) = 1;
    WaitFrames(1);
    frame++;
test1:
    if (frame == 0x78) {
        goto after1;
    }
    sway = 0;
    if (frame == 0) {
        _PlaySound(0x88);
    }
    if (frame == 0x1a) {
        _PlaySound(0x8d);
    }
    if (frame == 0x28) {
        _PlaySound(0x9a);
    }
    if (frame == 0x48) {
        _PlaySound(0x9a);
    }
    if (frame == 0x68) {
        _PlaySound(0x9a);
    }
    if ((gKeyRepeat & 3) == 0) {
        goto body1;
    }
    if (frame <= 0x10) {
        goto body1;
    }
after1:
    iwram_3001ad0[2] = saved;
    cam[4] = 0;
    Func_80d67dc();
    REG_WIN0H = 0xf0;
    pp = (void **)(base + 0x77d8);
    i = 0;
    do {
        unsigned char *a = (unsigned char *)*pp++;
        a[9] |= 0xc;
        i++;
    } while (i != 9);
    hitp = hit;
    front = 0xe0;
    {
        unsigned char *q = hit;
        do {
            *q = 0;
            q++;
        } while (q != hit + 14);
    }
    colp = cols;
    {
        unsigned char *c = colp;
        do {
            *c = Random() & 0x1f;
            c++;
        } while (c != cols + 16);
    }
    {
        Part *e = ewram_2010018;
        i = 0;
        do {
            i++;
            e->x = 0;
            e++;
        } while (i != 0xa0 * 2);
    }
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    REG_BG2CNT = 0x784;
    REG_BLDALPHA = 0x1010;
    curt = -0x1e0;
    frame = 0;
    do {
        if (frame <= 0x17) {
            int wob;
            scale2 = Data_edae0;
            front -= 0x10;
            if (frame > 8) {
                int a = (frame << 11) + 0x4000;
                if (a > 0x8000) {
                    a = (frame << 11) - 0x4000;
                }
                wob = (sin(a) << 5) >> 16;
            } else {
                int a = (frame << 11) + 0x4000;
                if (a > 0x8000) {
                    a = (frame << 11) - 0x4000;
                }
                wob = (sin(a) << 6) >> 16;
            }
            spr2[3] = 0;
            spr2[1] = 0xff << 16;
            pp = (void **)(base + 0x77d8);
            i = 0;
            do {
                spr2[0] = (front + Leef56[i]) << 16;
                spr2[2] = ((Leef5f[i] - wob) << 16) + (0x90 << 15);
                _UpdateSprite(*pp++, spr2, &scale2, 0);
                i++;
            } while (i != 9);
        }
        if (frame == 8) {
            *(int *)(base + 0x77a8) = frame;
            _PlaySound(0x91);
        }
        if (frame == 0xb) {
            _PlaySound(0x91);
        }
        if (frame == 0x2e) {
            _PlaySound(0x89);
        }
        i = 0;
        if ((*(State **)(base + 0x7828))->f14 != 0) {
            int k;
            poff = 0;
            k = 0x24;
            do {
                if (hitp[i] == 0) {
                    GetBattleActorPos3(
                        *(short *)((char *)*(State **)(base + 0x7828) + k), &pos);
                    if (pos.x > front) {
                        int j;
                        hitp[i] = 1;
                        p = (Part *)((char *)gBuffer + poff);
                        j = 0;
                        mask = 0xff;
                        do {
                            int v;
                            p->x = pos.x << 15;
                            p->y = (pos.y - 0x10) << 16;
                            p->vx = ((Random() & mask) - 0x80) << 10;
                            v = (Random() & mask) - 0xc0;
                            p->vy = v << 11;
                            p->x += p->vx * 4;
                            p->y += v << 13;
                            j++;
                            p->t = (Random() & 0xf) + 8;
                            p++;
                        } while (j != 0x20);
                        _SetBattleActorKnockback(
                            *(short *)((char *)*(State **)(base + 0x7828) + k), 1);
                        _PlaySound(0x86);
                    }
                }
                poff += 0xe0 << 2;
                k += 2;
                i++;
            } while (i != (*(State **)(base + 0x7828))->f14);
        }
        {
            Part *g = gBuffer;
            int w = 3;
            int h = 6;
            i = 0;
            do {
                if (g->t > 0) {
                    blit(buf, gfx + Data_ede48[2],
                         *(short *)((char *)g + 2) - 1,
                         *(short *)((char *)g + 6) - 3, w, h);
                    g->x += g->vx;
                    g->y += g->vy;
                    g->t = g->t - 1;
                }
                i++;
                g++;
            } while (i != 0xc0);
        }
        if (frame == 0x30) {
            _PlaySound(0x88);
        }
        if (frame > 0x28) {
            int x;
            *(int *)(base + (0xef << 7)) = 0;
            *(int *)(base + 0x7784) = 0x4b;
            i = 0;
            x = -8;
            do {
                int g = Random() & 3;
                blit(buf, base + ((g * 2 + g) << 9),
                     colp[i] - curt + 0x78, x, 0x30, 0x20);
                i++;
                x += 8;
            } while (i != 0x10);
        }
        if (frame > 0x40) {
            *(int *)(base + (0xef << 7)) = 2;
        }
        if (frame == 0x3a) {
            i = 0;
            if ((*(State **)(base + 0x7828))->f14 != 0) {
                int k = 0x24;
                do {
                    Func_80d6888(
                        *(short *)((char *)*(State **)(base + 0x7828) + k),
                        0xe, 5, -1, 0);
                    i++;
                    k += 2;
                } while (i != (*(State **)(base + 0x7828))->f14);
            }
        }
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
        curt += 0xc;
    } while (frame != 0x60);

    _Func_80bd7dc(0x86);
    pp = (void **)(base + 0x77d8);
    i = 0;
    do {
        _DeleteSprite(*pp++);
        i++;
    } while (i != 9);
    StopTask(Task_BlitAnim);
    gfree(0x2e);
    AnimEnd();
}
