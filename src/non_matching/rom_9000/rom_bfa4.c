/* Func_800bfa4 -- PARK at 6 differing encodings of 44.
 *   PIN-FREE, DEVICE-FREE, FLAG-FREE.  BODY UNCHANGED THIS BATCH.
 *
 * ===== BATCH 349 BRIEF D: THE FIGURE STANDS.  THE "PLATEAU" IS ONE PROGRAM,
 * ===== THE RECORDED 12-of-44 ROUTE DOES NOT REPRODUCE, AND THE SPLIT'S
 * ===== EXPORTED DATA SYMBOL IS NOW RESOLVED.
 *
 * THE FOUR NUMBERS, measured this batch (not inherited):
 *   ENCODINGS           6 differing of 44   (ref 44 / ours 44), first index 5
 *   16-BIT INSTRUCTIONS 39 / 39
 *   BYTES               equal on --func (no SIZE line)
 *   POOL / 32-BIT       equal
 *   RELOCATIONS         clean on --func.   PINS 0 (tools/shimcount.py).
 * In phase on every length dimension, so the figure IS a real distance.
 *   `--whole' prints SIZE ref 164 / ours 96 because the .s still carries the
 *   .rodata the split separates; the per-function figure is the distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/rom_bfa4.c \
 *     asm/rom_9000/rom_be70_c_c.s --func Func_800bfa4
 *
 * ===== THE SPLIT: ITS EXPORTED DATA SYMBOL IS LIVE, AND THE RECORDED SHAPE
 * ===== ALREADY HANDLES IT.  DO NOT CONVERT THIS PIECE WHOLE.
 *
 * tools/piece.py now reports exported DATA, and on this piece it fires:
 *     asm/rom_9000/rom_be70_c_c.s -- 1 function(s)
 *         PARKED  Func_800bfa4   6 of 44
 *         !! EXPORTS 1 NON-FUNCTION SYMBOL(S): .L1314c
 * THE REFERENCE WAS CHECKED RATHER THAN ASSUMED (`grep -rn '\.L1314c' asm/ src/'):
 *     asm/rom_9000/rom_be70_c_c.s:54   .global .L1314c      <- defined here
 *     asm/rom_9000/rom_be70_c_c.s:56  .L1314c:              <- .incrom 0x1314c, 0x13190
 *     asm/rom_9000/rom_be70_a.s:86     .word .L1314c        <- REFERENCED
 *     src/rom_9000/rom_be70_a.c:7      extern unsigned char scramble[] __asm__(".L1314c");
 * So TWO OTHER TRANSLATION UNITS depend on it, one of them already elevated C.
 * A WHOLE conversion of this piece would delete the definition and take the gate
 * red -- which is exactly the failure batch 345 hit.
 *
 * THE PARK'S RECORDED SHAPE IS CORRECT AND SUFFICIENT, re-verified this batch.
 * tools/split_s.py asm/rom_9000/rom_be70_c_c.s Func_800bfa4 --dry-run:
 *     "holds Func_800bfa4 plus 1 blob(s) and 1 label(s), ALL of it after the
 *      code.  Splitting code from data: the function goes to _b, the data to _c."
 *     would write asm/rom_9000/rom_be70_c_c_b.s  (1 function, 50 lines)  <- this
 *     would write asm/rom_9000/rom_be70_c_c_c.s  (1 function,  6 lines)  <- .rodata
 *     would REMOVE asm/rom_9000/rom_be70_c_c.s, would rewrite stage1.ld
 * The .rodata -- INCLUDING its `.global .L1314c' -- stays in asm as _c.s, so the
 * symbol survives the split and both referencing TUs keep linking.  And
 * tools/datacheck.py confirms no NEW export is needed either:
 *     EXPORTS : .L1314c  (already global -- NOT the set a split needs)
 *     Func_800bfa4  reads no data label -> split needs NO new export
 * Install path on a landing: src/rom_9000/rom_be70_c_c_b.c.
 * MANIFEST carries "split": null -- this is a park, not a landing.
 *
 * ===== THE RECORDED NEGATIVE LIST: 33 VARIANTS, 7 DISTINCT PROGRAMS =====
 *
 * Rebuilt from this header's own prose and swept with tools/sweep_variants.py,
 * which fingerprints each variant's object (encodings + relocations):
 *
 *   33 variants swept   26 BIT-IDENTICAL TO THE BASE   7 distinct programs
 *
 * (1) THE "PLATEAU" IS ONE PROGRAM.  This header reads: "THE 6 IS A PLATEAU, NOT
 *     AN ACCIDENT -- seven further spellings all read 6 of 44, 39/39, first
 *     index 5: cx/cy merged into dx/dy as one variable each; `*cam' for
 *     `cam[0]'; an `unsigned char *' base with casts; pre-masking `cam[1]' only;
 *     the unary-minus spelling `-(cx & mask) + tx'; declaring tx/ty first;
 *     declaring `cam' last."  ALL SEVEN ARE BIT-IDENTICAL TO THE BASE.  They are
 *     not seven spellings at a plateau; they are the base body compiled seven
 *     times.  The identical `first index 5' in every row was the tell.
 *     Same for the three interleavings the header already (correctly) calls
 *     byte-identical -- B A C E D F, B C A E D F, C B A E D F -- and for the
 *     `(unsigned)(dx + 0x1fffff) <= 0x12ffffe' range test recorded as "exactly
 *     inert": inert was the right figure, but it measured nothing.
 *
 * (2) THE RECORDED 12-of-44 ROUTE DOES NOT REPRODUCE, AND IT IS HALF OF THE
 *     PARK'S CENTRAL ARGUMENT.  This header's span arithmetic concludes: "The
 *     ROM needs tx allocated before cx.  For tx (1.00) to outrank cx, cx's span
 *     must reach 8 insns at 4 refs or its refs must drop to 2.  BOTH MEASURED",
 *     and gives the second as "dropping cx to 2 refs by giving the masked value
 *     its own quantity (`mx = cx & mask; dx = tx - mx;') -- 12 of 44, 39/39.  It
 *     DOES move the decision".
 *     MEASURED HERE, that exact spelling -- `mx = cx & mask;' then
 *     `dx = tx - mx;', nothing else changed -- is **BIT-IDENTICAL TO THE BASE AT
 *     6**.  cse folds the temp straight back out, cx keeps its four references,
 *     and nothing reaches the allocator.  So the refs-drop route was NEVER
 *     MEASURED; "BOTH MEASURED" is false, and one of the two arms of the park's
 *     anti-correlation claim rests on a figure that does not exist on this body.
 *     (If that 12 was genuine it was measured against a DIFFERENT interleaving
 *     than the one installed, and recording it against this body is a
 *     misattribution either way.)
 *
 * (3) ROUTE 3 -- THE ONE THAT PAID ON Func_80b6d30 -- IS BIT-IDENTICAL ON EVERY
 *     LOCAL IN THIS FUNCTION.  A dead store (`v = 0;' as its own statement
 *     before the real definition, the shape that took Func_80b6d30 from 4 to 3
 *     purely by the EXISTENCE of a second definition) was tried on every one of
 *     the eight locals -- cx, cy, tx, ty, mask, dx, dy, cam -- plus cx at the top
 *     of the body, cx and tx together, and cx defined late behind a dead store.
 *     ALL ELEVEN BIT-IDENTICAL TO THE BASE.
 *
 * >>> THE HONEST CLOSE, and it redirects the whole axis this park has been
 * >>> grinding: NO SPELLING OF THIS FUNCTION'S LOCAL-VARIABLE STRUCTURE REACHES
 * >>> THE BACK END.  Every local here is a single-assignment temp feeding one
 * >>> expression tree, so cse and combine rebuild identical RTL however it is
 * >>> written -- naming, un-naming, pre-masking, re-associating, re-ordering
 * >>> within a dependency class, and adding dead definitions are all the same
 * >>> program.  That is a BIT-IDENTICAL SET over the entire axis, not a list of
 * >>> negatives, and it means the lever is NOT in the locals.  Do not spend
 * >>> another round on statement order, temp naming, or mask placement.
 *
 * ===== WHAT DOES REACH THE BACK END, MEASURED -- AND ALL OF IT IS WORSE =====
 *
 * Seven distinct programs were produced, by changing the SHAPE of the function
 * rather than the spelling of its temps.  Every row 39/39 unless noted:
 *
 *   base (this body)                                        6 of 44  first 5
 *   `out[0]'/`out[1]' instead of `*out++'                    8        first 5
 *   the two shifted results in locals sx/sy                  8        first 5
 *   `mask' as a literal 0xffff0000 at both uses             13        first 4
 *   early-return spelling (negated test, stores first)      15        first 5
 *   both parameters copied into locals first                40  first 0, +4 bytes, RELOCDIFF
 *   the `&&' chain written as four nested `if's             41  first 1, RELOCDIFF
 *   `cam' dropped for two absolute address expressions      44  first 1, +4 bytes, RELOCDIFF
 *
 * Copying ONE parameter into a local (either one, separately) is bit-identical to
 * the base; copying BOTH costs a callee-saved register and four bytes.  The last
 * three rows are length defects and are dead, not near misses.
 *
 * ===== THE RESIDUE, UNCHANGED AND RESTATED =====
 *
 * Indices 5-10.  Two facts, and this header's anti-correlation question is still
 * the right one -- but note that half its supporting measurement (the 12 row) is
 * now withdrawn, so the coupling is OBSERVED, not demonstrated:
 *        ROM                        ours
 *   5    mov r5, r1                 ldr r2, [r3, #4]   cy -> r2
 *   6    ldr r1, [r3]      cx->r1   ldr r3, [r3]       cx -> r3  (base reused)
 *   7    ldr r2, [r3, #4]  cy->r2   mov r5, r1
 *   8    ldr r3, [r0, #8]  tx->r3   ldr r1, [r0, #8]   tx -> r1
 *   9    and r1, r4                 and r3, r3, r4
 *  10    sub r1, r3, r1    dx->r1   sub r1, r1, r3     dx from tx's register
 *   (1) cx and tx are SWAPPED between r1 and r3.  cx is the highest-priority
 *       local-alloc quantity (QTY_CMP_PRI, local-alloc.c:1496) so it takes the
 *       head of REG_ALLOC_ORDER (arm.h:989, `3, 2, 1, 0, ...') = r3, by reusing
 *       the dead camera base in place.  The ROM puts the OBJECT temps in r3 and
 *       cx in r1.
 *   (2) both camera loads are hoisted above the `mov r5, r1' parameter copy.
 *
 * WHERE THE NEXT ROUND SHOULD AIM, given the bit-identical set above: fact (2)
 * is a STREAM-POSITION question of exactly the kind an anchor reaches -- it is
 * the same shape that took this batch's OvlFunc_896_200c260 from 3 to 2 (the
 * parameter save landing before an independent materialisation).  This header
 * records the anchor axis as swept, 15 placements of which 6 were bit-identical,
 * so 9 distinct programs, all inert -- and that was NOT re-screened here, per
 * the brief.  But the anchor axis was swept on this body alone; it has never
 * been CROSSED with any of the seven distinct programs above, and two of those
 * (the `out[0]/out[1]' and sx/sy rows, both at 8 and both in phase) are the only
 * candidate prerequisites this function has ever produced.  That cross is the
 * cheapest untried thing here.
 *
 * SO THE PARK STANDS AT 6, with its evidence base corrected: the plateau is one
 * program, the refs-drop route was never measured, and the locals axis is closed
 * by a bit-identical set.
 * -- scratch_elev/b349/D
 *
 * ---------------------------------------------------------------------------
 * WHAT FOLLOWS IS THE PARK'S OWN HEADER, CARRIED FORWARD.  Its "seven further
 * spellings", its "12 of 44" mx row and its "BOTH MEASURED" are superseded by
 * the above; the rest was re-measured and still holds.
 * ---------------------------------------------------------------------------
 */
/* p4 -- Func_800bfa4 -- PARK at 6 differing encodings of 44 (was 8).
 *       PIN-FREE, DEVICE-FREE, FLAG-FREE.  BODY REPLACED THIS BATCH.
 *
 * ===== BATCH 337 BRIEF B: 8 -> 6, AND THE PARK'S OPEN QUESTION IS ANSWERED
 * ===== AND REPLACED.  THE REMAINING TWO FACTS ARE ANTI-CORRELATED.
 *
 * FIGURE, IN STREAM TERMS, MEASURED THIS BATCH: **6 differing stream encodings
 * of 44**, ref 44 / ours 44 encodings, **39/39 16-bit instructions**, first
 * differing index 5.  In phase, so a true distance.  SIZE and RELOCATIONS
 * silent on --func.  (The old body re-measured at 8 of 44, 39/39, first index
 * 7, so the park's figure was right and is now beaten.)
 * `--whole` still prints SIZE ref 164 / ours 96 because the .s carries the
 * .rodata the recorded split would separate; the per-function figure is the
 * distance.  Pins 0 (tools/shimcount.py).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/rom_bfa4.c \
 *     asm/rom_9000/rom_be70_c_c.s --func Func_800bfa4
 *
 * SPLIT SHAPE: unchanged from the park -- a text/data split.  `split_s.py
 * --dry-run` yields rom_be70_c_c_b.s (the function) -> src/rom_9000/
 * rom_be70_c_c_b.c and rom_be70_c_c_c.s (the .rodata) stays asm.  No new
 * export.
 *
 * THE LEVER: AN EXPLICIT LOCAL FOR EACH OBJECT-POSITION LOAD.
 * The park ran what it called "a COMPLETE 16-cell grid over the two axes the
 * park identified as contradictory, crossed with pre-masking".  Those axes are
 * read order x pre-mask x chain order -- three -- and the function has FOUR
 * loads.  `o[2]` and `o[4]` were inline in all sixteen cells, so the grid never
 * varied WHERE THE OBJECT LOAD SITS in the statement sequence.  Promoting them
 * to `tx` / `ty` adds the missing axis.  All **40 dependency-legal
 * interleavings** of the six statements were measured with the dx chain first;
 * three read 6 and emit byte-identical code:
 *
 *     6   B A C E D F   cy=cam[1]; cx=cam[0]; tx=o[2]; dx=..; ty=o[4]; dy=..
 *     6   B C A E D F   cy=cam[1]; tx=o[2]; cx=cam[0]; dx=..; ty=o[4]; dy=..
 *     6   C B A E D F   tx=o[2]; cy=cam[1]; cx=cam[0]; dx=..; ty=o[4]; dy=..
 *     8   C A B E D F        9   A B C E D F / A C B E D F
 *    11   A C E B D F       16   A B C D E F and five more
 *    21-26  the eleven cells that differ at index 0 (an extra callee-saved reg)
 *
 * THE PARK'S OPEN QUESTION IS NO LONGER THE QUESTION.  It asked "what can hold
 * r2 across cx's range?".  In this body **cy IS in r2** -- the ROM's
 * allocation -- and so are dx (r1), dy (r2), ty (r3) and the bias constant
 * (r0).  Indices 11-15 are byte-identical.
 *
 * WHAT THE REMAINING 6 ARE (indices 5-10), as two facts:
 *
 *        ROM                        ours
 *   5    mov r5, r1                 ldr r2, [r3, #4]   cy -> r2
 *   6    ldr r1, [r3]      cx->r1   ldr r3, [r3]       cx -> r3  (base reused)
 *   7    ldr r2, [r3, #4]  cy->r2   mov r5, r1
 *   8    ldr r3, [r0, #8]  tx->r3   ldr r1, [r0, #8]   tx -> r1
 *   9    and r1, r4                 and r3, r3, r4
 *  10    sub r1, r3, r1    dx->r1   sub r1, r1, r3     dx from tx's register
 *
 *   (1) cx and tx are SWAPPED between r1 and r3.  cx is the highest-priority
 *       local-alloc quantity so it takes the head of REG_ALLOC_ORDER
 *       (arm.h:989, `3, 2, 1, 0, ...`) = r3, by reusing the dead camera base
 *       in place.  The ROM puts the OBJECT temps in r3 and cx in r1.
 *   (2) both camera loads are hoisted above the `mov r5, r1` parameter copy.
 *
 * THE ARITHMETIC, with its lines.  QTY_CMP_PRI (local-alloc.c:1496) is
 * `floor_log2 (n_refs) * n_refs * size / (death - birth)`, qty_compare (:1504)
 * sorts descending, and for this body:
 *     cx    4 refs / span  4  ->  2*4/4 = 2.00      <- allocated FIRST
 *     cy    4 refs / span  7  ->  2*4/7 = 1.14
 *     tx    2 refs / span  2  ->  1*2/2 = 1.00
 *     cam   3 refs / span  4  ->  1*3/4 = 0.75
 *     ty    2 refs / span  3  ->  1*2/3 = 0.67
 *     mask  3 refs / span  9  ->  1*3/9 = 0.33
 * The ROM needs tx allocated before cx.  For tx (1.00) to outrank cx, cx's
 * span must reach 8 insns at 4 refs or its refs must drop to 2.  BOTH MEASURED:
 *   * lengthening cx's span -- the only statements available to insert between
 *     the read and the `and` are the other three loads, and every such
 *     interleaving is in the 40-cell table (best 6; A B C D E F reads 16).
 *   * dropping cx to 2 refs by giving the masked value its own quantity
 *     (`mx = cx & mask; dx = tx - mx;`) -- **12 of 44**, 39/39.  It DOES move
 *     the decision: `mov r5, r1` returns to index 5, the ROM's position, which
 *     fixes fact (2) -- and cy then takes r1 and cx r3 again.
 *
 * >>> SO THE OPEN QUESTION IS NOW: facts (1) and (2) ARE ANTI-CORRELATED.
 * >>> Every cell that restores the parameter copy's position loses cy's r2, and
 * >>> every cell that holds cy in r2 hoists the copy.  Find a cell that does
 * >>> both, or show the coupling is forced.  Do NOT re-propose the park's three
 * >>> axes or the 40 interleavings.
 *
 * THE 6 IS A PLATEAU, NOT AN ACCIDENT -- seven further spellings all read 6 of
 * 44, 39/39, first index 5: cx/cy merged into dx/dy as one variable each;
 * `*cam` for `cam[0]`; an `unsigned char *` base with casts; pre-masking
 * `cam[1]` only; the unary-minus spelling `-(cx & mask) + tx`; declaring
 * tx/ty first; declaring `cam` last.
 *
 * MEASURED WORSE THIS BATCH (39/39 unless noted): `mask` assigned after the
 * camera reads 7; cx used as the object temp 17; pre-masking `cam[0]` in this
 * order 12; pre-masking both 31 at 38 instructions with COUNT+SIZE+RELOC
 * (structurally different, dead); ONE shared object temp for both loads 16-24,
 * every cell at first index 0 (an extra callee-saved register);
 * `(unsigned)(dx + 0x1fffff) <= 0x12ffffe` for the range test exactly inert;
 * `out[0]`/`out[1]` for `*out++` 13 and 17; testing dy before dx 18 and 20;
 * dropping the `cam` local for two absolute address expressions 43-45 at 41
 * instructions (the `add r3, #0xe4` is lost), COUNT+SIZE+RELOC.
 *
 * THE PARK'S OWN GRID REPRODUCED EXACTLY where my axes overlap it: base 8,
 * cam[0]-first + dx-first 14, cam[1]-first + dy-first 10, cam[1]-first +
 * dx-first 12.  ONE OF ITS CELLS DESERVED MORE THAN A NUMBER: **cam[1] read
 * first + dx chain first (12 of 44) is the ROM's EXACT INSTRUCTION SCHEDULE
 * under a pure 3-cycle register permutation** (cx r1->r3, cy r2->r1, object
 * temp r3->r2).  Indices 5-15 are structurally identical to the reference.
 * If the plateau above will not break, that cell is the other way in.
 *
 * ---------------------------------------------------------------------------
 * WHAT FOLLOWS IS THE PARK'S OWN HEADER, CARRIED FORWARD UNCHANGED.  Its
 * figure line (8) and its "16-cell grid / NOTHING BELOW 8" are superseded by
 * the above; everything else in it still holds and was re-measured.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 6 of 44, instruction counts EQUAL, byte size and pool
 *   count equal too (this park is in phase on all three length dimensions, so
 *   its figure IS a real distance).
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: 15 placements swept, but 6 WERE BIT-IDENTICAL to
 *   the base, so only 9 DISTINCT PROGRAMS were actually tested.
 *   Those 9 are inert; the other 6 measured nothing.  No variant was capped and none failed to
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
/* p4 -- Func_800bfa4 -- PARK at 8 differing encodings of 44.  PIN-FREE, DEVICE-FREE.
 *
 * Figure I measured (not inherited): 8 differing encodings of 44 (ref 44, ours
 * 44), first differing index 7 -- ref 685a (ldr r2,[r3,#4]) against ours 685b
 * (ldr r3,[r3,#4]); indices 7..14 contiguous.  PINS 0.
 * (`--whole` prints SIZE ref 164 / ours 96 because the .s still carries the
 * .rodata the recorded split would separate; the per-function figure is the
 * distance.)
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/rom_bfa4.c asm/rom_9000/rom_be70_c_c.s --func Func_800bfa4
 *
 * SPLIT SHAPE: unchanged from the park -- it needs a text/data split;
 * split_s.py --dry-run yields rom_be70_c_c_b.s (the function) ->
 * src/rom_9000/rom_be70_c_c_b.c and rom_be70_c_c_c.s (the .rodata) stays asm.
 * No new export is needed.
 *
 * THE RESIDUE is indices 7..14, the whole interleaved middle: the second camera
 * word's register and, following from it, which of the two delta chains runs
 * first.  One run, one cause.
 *
 * *** THE PARK'S NAMED REMAINING CAUSE IS ALREADY SATISFIED IN THE dx-FIRST
 *     FORM, SO THE OPEN QUESTION IS THE WRONG ONE. ***
 * The park ends: "a third local-alloc quantity of higher priority than cam[1]'s
 * would have to occupy r3 across cam[1]'s range, and nothing in the program
 * supplies one."  Measured, `.17.lreg` for the dx-chain-first body gives
 *     cx -> r2    cy -> r1    o[2] -> r3    o[4] -> r3
 * BOTH object reads are already in r3, which is the ROM's allocation -- the third
 * quantity exists and does occupy r3 across the second camera word's range.  What
 * goes wrong in that form is different and twofold:
 *   1. cx and cy are SWAPPED against the ROM (ROM: cx -> r1, cy -> r2).  cx takes
 *      r2 because REG_ALLOC_ORDER's head r3 (arm.h:989, `3, 2, 1, 0, ...`) is
 *      still held by the live camera pointer at cx's birth, and nothing bars r2.
 *   2. Because the LOCAL cy holds r1, the GLOBAL allocno for dx is pushed out to
 *      r6 -- `;; 38 in 6` with `Hard regs used: 0 1 2 3 4 5 6` in `.18.greg` --
 *      which is the extra `push {r5, r6, lr}` the park reports.
 * The 8-body's own dump, for contrast: cx -> r1, cy -> r3, o[4] -> r2, o[2] -> r3,
 * `;; 38 in 1` and no r6.
 *
 * SO THE QUESTION IS NOW: what can hold r2 across cx's range?  Over the ROM's
 * indices 6..10 the register file is r0 = the object pointer, r3 = the camera
 * pointer then o[2], r4 = the mask, r5 = the output pointer -- and r2 is held by
 * nothing but cy, which QTY_CMP_PRI allocates AFTER cx.  QTY_CMP_PRI
 * (local-alloc.c:1496) is floor_log2(n_refs) * n_refs * size / (death - birth)
 * and qty_compare (:1504) sorts it descending, so cx's shorter span beats cy's
 * at equal reference counts (four each: load-set, and-use, and-set, minus-use).
 * Lowering cx to two references needs the masked value to be a SEPARATE quantity,
 * which needs cx live past the `and`, which costs a move.
 *
 * MEASURED THIS ROUND -- a COMPLETE 16-cell grid over the two axes the park
 * identified as contradictory, crossed with pre-masking, which the park had only
 * sampled.  Every cell: read order (which camera word first) x pre-mask
 * (none / x / y / both) x chain order (dx / dy).  NOTHING BELOW 8.
 *   8   cam[0] read first, no pre-mask, dy first  -- the park's body, reproduced
 *   8   cam[0] read first, cam[1] PRE-MASKED, dy first  -- NEW, exactly inert,
 *         44 instructions: a second alternative body at the figure
 *  10   cam[1] read first, dy first -- the ROM'S REGISTER MAP; pre-masking either
 *         or both values is exactly inert on it (four cells all read 10)
 *  12   cam[1] read first, dx first; pre-masking cam[0] exactly inert on it
 *  13   both values pre-masked, dx first
 *  14   cam[0] read first, dx first; pre-masking cam[0] EXACTLY INERT on it --
 *         the cross the span arithmetic predicted would work, and it does not
 *  25/31  the two pre-mask-both and pre-mask-y cells that drop an instruction
 *         (42 encodings, 92 bytes, RELOCDIFF) -- structurally different, dead
 * All dx-first cells differ at index 0, ref b520 against ours b560: the extra
 * callee-saved register.  The park's contradiction is now confirmed cell by cell
 * rather than by sampling.
 */
extern int iwram_3001e70;

int Func_800bfa4(int *o, int *out)
{
    int *cam;
    int mask;
    int cx;
    int cy;
    int tx;
    int ty;
    int dx;
    int dy;

    cam = (int *)(iwram_3001e70 + 0xe4);
    mask = 0xffff0000;
    cy = cam[1];
    cx = cam[0];
    tx = o[2];
    dx = tx - (cx & mask);
    ty = o[4];
    dy = ty - (cy & mask);
    if (dx > -0x200000 && dx < 0x1100000 && dy > 0 && dy < 0xe00000) {
        *out++ = dx >> 16;
        *out = dy >> 16;
        return 0;
    }
    *out++ = 0;
    *out = 0;
    return -1;
}
