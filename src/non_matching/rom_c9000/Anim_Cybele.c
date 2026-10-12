/* Anim_Cybele -- NOT MATCHING, a SIZE mismatch: ref 3308 bytes against ours 3156.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch_elev/b313e/PARK_Anim_Boreas.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/Anim_Cybele.c \
 *     asm/rom_c9000/rom_d6970.s --func Anim_Cybele
 *
 * The original prose park is preserved below.
 */
/* Anim_Boreas (asm/rom_c9000/rom_d6970.s:1744, 0x080d765c, 1340 ROM
 * instructions / 1416 encodings) --
 * NON-MATCHING, 1263 of 1416 encodings differ.
 *
 * READ THE FIGURE CORRECTLY.  1263 is objcmp's PRODUCTION-FLAG number for the
 * body delivered below and it is SATURATED BY ONE: the instruction count is
 * 1417 against 1416.  It is therefore NOT a distance and must never be ranked
 * against another function.  Rank by aligncmp.
 *
 *   SIZE IS EXACT -- 3156 bytes both sides, objcmp prints no SIZE line.
 *   COUNT is 1417 against 1416 (one over).
 *   tools/aligncmp.py, reported SEPARATELY and never on the claim line:
 *       aligned-equal 661 of 1416 = 46.7%,  884 differing/ins/del in 267 hunks
 *   RELOCATIONS: the SYMBOL SEQUENCE is the ROM's entry for entry apart from
 *   the two items in BLOCKERS 2 and 3 -- every callee, every pooled data
 *   label, both `__modsi3` groups, the single `__umodsi3` and every
 *   `_call_via` veneer is present, once, in the ROM order.  First candidate
 *   already had that.
 *
 * A BETTER objcmp FIGURE EXISTS AND IS DELIBERATELY NOT THE DELIVERED BODY.
 * Declaring `Scale sc; int pos[4]; Site sites[0x20];` (the natural order)
 * reads objcmp 1227 of 1416 and aligncmp 681 of 1416 (48.1%) -- BETTER on both
 * -- but SIZE 3168 against 3156 and COUNT 1423 against 1416, and its frame
 * lays the three aggregates out as sc / sites / pos, which is NOT the ROM's.
 * The delivered order gives the ROM's sc / pos / sites and is ONE defect from
 * the ROM's complete frame (see NEXT MOVES 1).  The 1227 body is kept in
 * scratch_elev/b313e/v1.c.  Both figures are stated so the next pass can pick
 * on structure rather than on the inflated positional number.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Boreas.c \
 *     asm/rom_c9000/rom_d6970.s --func Anim_Boreas
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_c9000/Anim_Boreas.c \
 *     asm/rom_c9000/rom_d6970.s Anim_Boreas
 *
 * SHIMS: NONE.  `python3 tools/shimcount.py <this file>` reports the filename
 * and no rows -- PIN-FREE, no register pin, no barrier, no per-file flag
 * override, no fakematch.txt row.
 *
 * ================================================================
 * SPLIT SHAPE -- AND THE BOREAS/CYBELE ASYMMETRY, MEASURED BOTH WAYS
 * ================================================================
 *
 * `tools/datacheck.py asm/rom_c9000/rom_d6970.s`:
 *     data sections : .rodata
 *     functions     : Anim_Cybele, Anim_Boreas          (exactly these two)
 *     NOT ONE `.global` in the whole file.
 *     Anim_Cybele reads 13 labels: .Lee910 .Lee916 .Lee920 .Lee925 .Lee92a
 *                 .Lee930 .Lee934 .Lee93e .Lee943 .Lee948 .Lee952 .Lee958
 *                 .Lee966
 *     Anim_Boreas reads 18 labels: .Lee974 .Lee994 .Lee998 .Lee99e .Lee9a1
 *                 .Lee9a4 .Lee9a8 .Lee9b3 .Lee9be .Lee9d4 .Lee9d7 .Lee9da
 *                 .Lee9e0 .Lee9e6 .Lee9e9 .Lee9ec .Lee9ef .Lee9f2
 *
 * THE TWO CUT ORDERS ARE NOT SYMMETRIC AND BOTH WERE DRY-RUN.
 * `tools/split_s.py --dry-run` refuses both, and the two refusals differ:
 *
 *   split_s.py --dry-run ... Anim_Boreas  -> REFUSES, 31 labels would cross.
 *       Boreas is the SECOND function (k=1), so groups are
 *         _a = Anim_Cybele      _b = Anim_Boreas      _c = .rodata
 *       THREE parts.  Cutting Boreas out ALSO cuts Cybele off from the data,
 *       so BOTH label sets must be exported: 13 + 18 = 31 `.global` lines.
 *
 *   split_s.py --dry-run ... Anim_Cybele  -> REFUSES, 13 labels would cross.
 *       Cybele is the FIRST function (k=0), so _a is EMPTY and
 *         _b = Anim_Cybele      _c = Anim_Boreas + .rodata
 *       TWO parts (empty parts are not written).  Boreas stays WITH its data,
 *       so only Cybele's 13 labels need exporting.
 *
 * WHICH ORDER TO TAKE IS A REAL TRADE-OFF, NOT A DOMINANCE, and the brief's
 * framing ("cutting for one member can leave the other needing no split while
 * the reverse buries it") is right but points the OTHER way from cheapness:
 *
 *   - BOREAS FIRST costs 31 exports now and leaves Cybele needing NO SPLIT AT
 *     ALL.  rom_d6970_a.s would hold Cybele alone with no data, which is
 *     split_s.py's own "holds only <target> and no data; convert it directly,
 *     no split needed" exit (tools/split_s.py:266).  One expensive cut, then
 *     both functions are free.
 *   - CYBELE FIRST costs 13 exports now, but then Boreas sits in
 *     rom_d6970_c.s still beside the .rodata and needs a SECOND two-part
 *     split with its own 18 exports.  Same 31 total, split across two
 *     batches, and two `make compare` gates instead of one.
 *
 * RECOMMENDATION: take BOREAS FIRST.  The export count is identical in total,
 * the asymmetry is purely in how many cuts you pay for, and Boreas-first is
 * the only order that ever reaches a one-function-no-data file.
 * A `.global` emits no bytes; verify `make compare` AFTER the exports and
 * BEFORE the split, as split_s.py's own recipe says, so the two changes stay
 * separable.  `--dry-run` ALWAYS -- the real run deletes a tracked .s.
 *
 * ================================================================
 * THE FRAME: FIVE GREPS, NOT FOUR -- AND IT BALANCES TO THE BYTE
 * ================================================================
 *
 *     sub sp, #0x14c                         332 bytes, reproduced EXACTLY
 *     (add|sub) sp, rN                       NONE
 *     add r2, sp, #0x4c                      aggregate
 *     add r2, sp, #0x3c                      aggregate
 *     mov r2, sp / add r2, #0x34             aggregate -- FOURTH FORM, and the
 *                                            plain `mov rX,sp` grep sees only
 *                                            the `mov`; the +#0x34 is on the
 *                                            NEXT line and is what names the
 *                                            slot.  Scan the line after.
 *     str rX, [sp]  with no matching load    13 (outgoing args 5/6/7)
 *
 * THE FIFTH FORM, FOUND HERE AND ABSENT FROM BOREAS BUT PRESENT IN TWO OF THE
 * OTHER THREE: `add rX, sp` -- register PLUS sp, with the offset built first
 * (`mov r2,#0x98 / lsl r2,#1 / add r2, sp` in Anim_Thor; `ldr r2,=0x14f /
 * add r2, sp` in the same function; `mov r6,#0x6c / add r6, sp` in
 * Anim_Judgment).  It is NOT the brief's grep 2 (`add sp, rN`, which changes
 * sp) and NOT grep 3 -- it matches NEITHER, so a four-grep frame MISSES IT.
 * It is forced whenever the offset is not word-aligned (`sp+0x14f` cannot be
 * an `add rX, sp, #imm` at all) and chosen whenever the constant is already
 * in a register.  ADD IT TO THE FRAME GREP SET:
 *     grep -nE '^\s+add\s+r[0-9]+,\s*sp\s*$'
 * IT COMES IN TWO ORDERINGS and both must be read: SHIFTED
 * (`mov r2,#0x98 / lsl r2,#1 / add r2, sp`, Thor) and IMMEDIATE-FIRST
 * (`mov r6,#0x6c / add r6, sp`, Judgment).  The grep above catches both,
 * because it anchors on the `add`, not on what builds the offset.
 *
 * AND A SEVENTH CLASSIFICATION CASE WHEN RESOLVING THESE SITES: an
 * ADDRESS-TAKEN SCALAR (`f(&a)`) materialises a base exactly like an
 * aggregate and like a sub-word scalar, and is NEITHER.  The discriminator is
 * the object's SIZE from the next base up: a single word between one base and
 * the next is an address-taken scalar, not a one-element aggregate.  None of
 * Boreas's three sites is one -- they are 8, 16 and 256 bytes -- but the
 * two-word objects in Cybele and Judgment must be checked against it.
 *
 * AGGREGATE COUNTS IN THE BRIEF ARE AN UPPER BOUND -- RESOLVED HERE.
 * Thumb-1 has no sp-relative sub-word load or store, so every sub-word stack
 * SCALAR also materialises a base with `add rX, sp, #K` and is
 * indistinguishable from an aggregate at the grep.  Each of Boreas's three
 * sites was resolved BLOCK-AWARE (first use, stopping at any label or branch)
 * and all three are GENUINE aggregates, every access word-wide:
 *     sp+0x34  `Scale sc`      2 words  -- +0 and +4, and &sc is argument 3
 *                                          of _UpdateSprite
 *     sp+0x3c  `int pos[4]`    4 words  -- +0,+4,+8,+0xc, and &pos is
 *                                          argument 2 of _UpdateSprite
 *     sp+0x4c  `Site sites[]`  0x20 x 2 words -- base WALKED `add r2,#8`
 *                                          across 0x20 iterations
 * So Boreas's upper bound of 3 was TIGHT, and the arithmetic is the
 * independent confirmation -- 0x34 + 8 + 16 + 256 = 0x14c, the frame to the
 * byte, with nothing left over.  `Data_eda80` being an 8-byte `.incdata`
 * (rom_eda78.s: `Data_eda80, 0xeda80, 0xeda88`) independently fixes `Scale`
 * at two words, exactly as it did for Gaia's `Data_edab0`.
 * The other three targets' bounds were NOT tight; see the triage note at the
 * foot of this header.
 *
 * THE SPILL-SLOT MAP IS THE DECLARATION LIST, and the ROM has TEN scalar
 * slots.  Reference access counts, scalars DESCENDING (the census regex must
 * accept DECIMAL immediates -- the reference writes `[sp, #4]`, not
 * `[sp, #0x4]`, and a hex-only regex silently drops slots 0x4 and 0x8):
 *
 *     slot   quantity                       ref accesses   ours
 *     0x30   ctx     = p[0]                      11          11
 *     0x2c   base    = p[-1]                     29          29
 *     0x28   frame                               50           -- MISSING
 *     0x24   blitA   = p[6]                       9           9
 *     0x20   blitB   = p[7]                       3           3
 *     0x1c   scratch = p[1]                       4           4
 *     0x18   winstep                              4           4
 *     0x14   the j counter of the 3x3 loop        3          15
 *     0x10   &sc, spilled                         3           8
 *     0x0c   the 0x3f mask, spilled over Random   2           3
 *     0x00   outgoing arguments 5, 6 and 7   13 + 10 + 2  13 + 10 + 2
 *
 * SIX OF THE TEN MATCH ON THE NOSE and the outgoing-argument triple matches
 * exactly.  The one defect is the whole residue; see BLOCKERS 1.
 *
 * ================================================================
 * THE PER-FUNCTION COMPARISON CENSUS -- GAIA'S `!=` RULE DOES TRANSFER HERE,
 * AND THE RAW COUNTS IN THE BRIEF ARE NOT THE CENSUS
 * ================================================================
 *
 * `grep -coE '\b(bne|blt|ble|bgt|bge)\b'` over the reference gives
 * bne 48, ble 15, bgt 9, blt 4, bge 4 -- the brief's "48 bne / 32 signed".
 * THAT IS NOT THE CENSUS THE RULE NEEDS.  Classified by BACKWARD EDGE (target
 * label defined earlier in the stream), Boreas has 27 back edges: 24 close on
 * `bne`, 3 are unconditional `b` (loop rotation), and NOT ONE signed branch
 * closes a loop.  All 32 signed branches are
 * STRAIGHT-LINE frame-window tests (`cmp #0x67 / ble`, `cmp #0xd0 / ble`,
 * `cmp #0x47 / bgt`) plus three signed-division biases and two `blt` guards
 * on `g->t`.  The seven `bhi` are unsigned RANGE tests on a biased
 * subtraction (`sub r3,#0xb0 / cmp r3,#3 / bhi`), which the brief's own caveat
 * excludes.
 *
 *
 * THE INSTRUMENT, CORRECTED -- A RAW MNEMONIC CENSUS IS THE WRONG TOOL.
 * `grep -coE '\b(bne|blt|ble|bgt|bge)\b'` counts COMPARISONS, not loops, and
 * on every one of this batch's four functions it is a DECOY.  The right
 * instrument classifies BACKWARD EDGES: a branch whose target label is
 * defined EARLIER in the stream is a loop closure; everything else is a
 * clamp, a signed-division correction or a frame-phase test.  Run that and
 * the four read:
 *     function     backward edges   bne   unconditional b   SIGNED closures
 *     Anim_Boreas        27          24          3                 0
 *     Anim_Cybele        28          25          2                 1
 *     Anim_Thor          24          21          3                 0
 *     Anim_Judgment      29          25          4                 0
 * NINETY-FIVE OF NINETY-SIX conditional backward edges in the batch are
 * `bne`.  The brief's framing that these four are "signed-dominant" (32/40/
 * 34/70 signed compares) is UNSAFE: those are clamps and division biases.
 * GAIA'S `!=` RULE TRANSFERS, essentially completely, TO ALL FOUR.
 * The single exception in the batch is Cybele's `cmp r6, #0xb8 / bgt`.
 * The unconditional `b` back edges are loop rotation, not a comparison.
 *
 * SO: every loop in this function is `i != N`, and `Anim_Gaia`'s rule
 * transfers WHOLE.  Writing them all `!=` was in the first candidate and is
 * part of why that candidate opened at objcmp 1227 rather than the 1400s.
 * THE LESSON IS ABOUT THE INSTRUMENT, NOT THE ANSWER: a whole-function
 * bne-vs-signed ratio is a decoy on a function with many frame-window tests.
 * Boreas reads "48 against 32" and is as uniformly `!=` as Gaia, which read
 * "all bne".  Count the backward edges, not the branches.
 *
 * ================================================================
 * THE RESIDUE IS ONE QUANTITY, AND THE PER-OPCODE HISTOGRAM IS WHAT PROVED IT
 * ================================================================
 *
 * RUNG 8, carried as required.  Per-opcode, ref against ours, delivered body:
 *     ldr   231 / 183   -48        mov   311 / 350   +39
 *     str   120 / 126    +6        asr    13 /  15    +2
 *     lsr     8 /   6    -2        neg    15 /  13    -2
 *     ldrh    8 /  17    +9        add   135 / 134    -1
 *     bgt     9 /  10    +1        bhi     7 /   6    -1
 * sum|delta| = 111 against a total of 1340.
 *
 * THE `.call_via` HISTOGRAM TRAP DOES NOT APPLY TO THIS COLUMN, and that was
 * checked rather than assumed.  `include/macros.inc:64` does define a
 * `.call_via` macro that expands to `mov r12,pc` + `bx`, which would
 * under-count a raw reference's `mov` and `bx` by one per site -- but
 * `rom_d6970.s` contains NO macro line: it reaches its ten veneers with a
 * plain `bl _call_via_rN` to an external symbol.  Veneer sites 10 against 10
 * and `bx` 1 against 1 on both sides, so the `mov +41` column is clean.
 *
 * THE CARRIER-TYPE SCREEN ALSO PASSES, and it independently re-confirms (i).
 * A wrong `unsigned short` carrier shows up as `ldrsh` plus `lsl #16` plus
 * `lsr #16` where the reference has none.  Here `ldrsh` is 12 against 12 and
 * `lsl` is 60 against 60 -- BOTH EXACT -- so `((short *)g)[1]` is the right
 * spelling and the carrier width is not in question.  That matters for (i):
 * a halfword-carrier defect would have inflated `lsl`/`lsr` alongside the
 * `ldrh`, and `lsl` is exact, so the `ldrh +9` cannot be a width problem.
 * NOTE FOR THE OTHER THREE: Anim_Judgment has `ldrsh` 1 against `ldrh` 17 and
 * `lsl` 131, so it is the member where a wrong halfword carrier would be
 * most expensive -- screen it with that one grep first.
 *
 * TWO THINGS COME OUT OF THAT, AND ONE OF THEM CORRECTS A RECORDED PARK.
 *
 * (i) THE `ldrh +9 / ldr -9` PAIR IS A MNEMONIC-TEXT ARTEFACT WITH IDENTICAL
 *     ENCODINGS, AND A .s-TEXT HISTOGRAM MUST NORMALISE IT.
 *     gcc prints a pooled halfword constant's load as `ldrh rX, <pool-label>`;
 *     the project's disassembly of the same encoding prints `ldr rX, [pc,
 *     #imm]`.  Thumb-1 has no PC-relative `ldrh`, so gas assembles gcc's text
 *     to the ordinary `ldr` and THE BYTES ARE THE SAME.
 *     PROVED, not assumed: aligncmp reports hunks at ref[41:42] and then
 *     nothing until ref[68:69], and indices 42..67 are exactly the
 *     `REG_WININ = 0x2137`, `AnimTransitionOut`, `REG_WIN0H = 0xf0f0` and both
 *     `LoadVFXFile` instructions -- i.e. every one of the pooled-halfword
 *     sites is BYTE-IDENTICAL.
 *     THIS IS A CORRECTION TO src/non_matching/rom_c9000/Anim_Gaia.c BLOCKER
 *     4, which attributes "3 encodings" to `ldrh <pool>` against `ldr <pool>`
 *     at 0xcc / 0xaa / 0x1f and flags the mechanism as unexplained.  The width
 *     difference is in the TEXT ONLY; if Gaia really loses 3 encodings there
 *     the cause is something else (most likely the pool WORD's own emission
 *     order), and that park's residue attribution should be re-measured before
 *     anyone spends on it.  It also means Ramses's
 *     "every halfword store of a constant is pooled, and that is correct here"
 *     is right for the reason it gives and the int-carrier lever must NOT be
 *     applied to these sites.
 *
 * (ii) WITH THAT REMOVED THE WHOLE RESIDUE IS `ldr -39 / mov +41` AND IT IS
 *     ONE VARIABLE: `frame` is in a hard register here and in a SPILL SLOT in
 *     the ROM.  The ROM reads slot 0x28 fifty times -- it reloads `frame` at
 *     every single one of its twenty-odd window tests -- and each of those is
 *     an `ldr`.  We keep `frame` in r11 and pay a `mov rX, fp` instead, which
 *     is the +41.  Nothing else in the histogram is bigger than 6.
 *     THIS IS WHY THE EXACT INSTRUCTION COUNT IS A TRAP HERE.  A third
 *     variant (`int frame = 0;` at the declaration, scratch_elev/b313e/v3.c)
 *     reads COUNT 1416 against 1416 -- EXACT -- and SIZE 3152 against 3156,
 *     and it is WORSE: objcmp 1359, aligncmp 630.  Its slot map is unchanged.
 *     The exactness is a coincidence between `ldr -48` and `mov +41`, which is
 *     rung 8's "fifty misplaced instructions summing to zero" reproduced on a
 *     real candidate.  COMPARE THE HISTOGRAM, NOT THE TOTAL.
 *
 * ================================================================
 * LEVERS, WITH FIGURES -- INCLUDING THE THREE THAT PAID NOTHING
 * ================================================================
 *
 * (0) THE STARTING POINT.  The first candidate -- written straight off the
 *     disassembly with Anim_Ramses's declaration block, `State`/`Part`/`Scale`
 *     shapes and `void **p = iwram_3001ef0; ctx = p[0]; base = p[-1];
 *     scratch = p[1];` idiom -- read SIZE 3168/3156, COUNT 1423/1416,
 *     objcmp 1227 of 1416, aligncmp 681 of 1416 (48.1%), with the relocation
 *     SYMBOL SEQUENCE already right.  Anim_Ramses is the oracle for this
 *     function, not Anim_Gaia: it has the same `iwram_3001ef0` prologue, the
 *     same 0x7828 bank rule and nearly the same extern list.
 *
 * (1) THE THREE-AGGREGATE DECLARATION ORDER, AND IT NARROWS A RECORDED RULE.
 *     Anim_Gaia records "expand assigns declared locals last-first and the
 *     frame grows downward", so declared-EARLIER lands at the LOWER sp offset.
 *     That held for Gaia's TWO aggregates.  With THREE it is wrong, measured
 *     both ways on this function:
 *         declared  sc, pos,   sites   ->  frame  sc@0x34, sites@0x3c, pos@0x13c
 *         declared  sc, sites, pos     ->  frame  sc@0x30, pos@0x38,   sites@0x48
 *     The ROM is sc / pos / sites ascending.  So the FIRST-declared aggregate
 *     takes the lowest offset and THE REST ARE LAID OUT IN REVERSE
 *     DECLARATION ORDER ABOVE IT.  Gaia's two-aggregate case cannot tell the
 *     two rules apart; this one can.
 *     FIGURES: SIZE 3168 -> 3156 (EXACT) and COUNT 1423 -> 1417 (+7 -> +1),
 *     against objcmp 1227 -> 1263 and aligncmp 681 -> 661.  THE SEPARATED AXES
 *     SAY TAKE IT AND THE POSITIONAL FIGURES SAY DO NOT; the frame layout
 *     breaks the tie, and this is Gaia's lever-(8) trap in a sharper form --
 *     there a correct change moved the aligned figure by +2, here a correct
 *     change moves it by -20.  THE SLOT MAP AND THE FRAME ARITHMETIC ARE THE
 *     INSTRUMENT.  Do not re-derive this by figures alone.
 *
 * (2) `base + 0x7828` IS NOT A NAMED LOCAL -- Gaia's lever (a) transfers
 *     unchanged and cost nothing to apply.  The ROM keeps `base + 0x7828` in
 *     r8 across `AnimStart` with no help (it is a pseudo-based ADDRESS, which
 *     cse commons free) and RE-DERIVES it from a pooled 0x7784 at the
 *     `.Ld8144` actor loop, reloading `base` from its slot each time.  Written
 *     out at all four sites, never named.  Anim_Ragnarok's `slot` local is
 *     still the documented exception and still must not be copied here.
 *
 * (3) `sc = Data_eda80;` IS A STRUCT ASSIGNMENT -- Gaia's lever (b), and the
 *     tell is in the reference verbatim:
 *         ldr r4,[r3,#4] / ldr r3,[r3] / str r3,[sp,#0x34] / str r4,[sp,#0x38]
 *     loads BOTH then stores BOTH, with DIRECT sp-relative stores and
 *     `mov r2,sp / add r2,#0x34` materialised only LATER, at the
 *     `_UpdateSprite` call that takes `&sc`.  That is `emit_block_move`.  In
 *     the first candidate and correct on the first try.
 *
 * (4) A LOOP-INVARIANT MASK IS HOISTED BY loop.c WHEN THE LOOP CONTAINS A
 *     CALL, AND NO SOURCE SPELLING IS NEEDED.  The reference has the SAME
 *     source expression come out two ways:
 *         `and r0, r7`  (value in rd)  where 0x7f / 0x1f are hoisted into
 *                                     call-saved registers in the preheader
 *         `mov r3,#0x7f / and r3, r0`  (constant in rd) where the mask is
 *                                     used ONCE
 *     Both are `Random() & 0x7f` as a plain literal.  The discriminator is
 *     USE COUNT, not operand order: `scan_loop`'s threshold is halved when the
 *     loop has a call, so a cost-1 `mov` of a constant used twice clears it.
 *     THIS BOUNDS Anim_Gaia's LEVER (9).  Gaia reads an operand order back out
 *     of `and` ("a constant in rd cannot be the preserved loop-invariant
 *     copy") and concludes the ROM needed two quantities.  On this function
 *     that inference would be WRONG at five sites: `mov r4,#3 / and r4,r0`,
 *     `mov r3,#1 / and r3,r2`, `mov r3,#0x7f / and r3,r0`,
 *     `mov r2,#0x3f / and r2,r0` are all plain literals used once.  Check the
 *     USE COUNT before reading a spelling out of operand order.
 *     The one site that genuinely needs a named mask is the 0xae teardown,
 *     `mov r4,#0xd / neg r4,r4` in the preheader and `mov r3,r4 / and r3,r2`
 *     inside: the hoisted mask is COPIED INTO rd, which only happens when the
 *     AND's result is a pseudo distinct from both inputs, i.e. the mask is the
 *     FIRST and-operand.  `s[9] = m9 & s[9];` with `m9 = ~0xc;` in the
 *     preheader reproduces it; `s[9] &= m9;` does not.  This is Gaia's
 *     lever (4) with the operand order the other way round.
 *
 * (5) THE `_AnimTransitionIn` SECOND ARGUMENT IS POOLED IN THE ROM AND WE
 *     `mov` IT -- 1 instruction and 1 pool word, and it is the ENTIRE 4-byte
 *     size difference of the v1 body.  See BLOCKERS 2; this is Anim_Ramses's
 *     recorded nine-site bank-wide anomaly and `rom_d6970.s` is one of the
 *     nine it names.  NOT a defect introduced here.
 *
 * MEASURED AND INERT -- BYTE-IDENTICAL OBJECTS, DO NOT RESPEND:
 *  - RAISING `yy`'s REG_N_SETS to two (`yy = 0x3e0; yy = -yy;` instead of
 *    `yy = -0x3e0;`) to make it beat `frame` for r11 in loop B:
 *    BYTE-IDENTICAL to v1.  The brief's two-sets lever does not reach a
 *    global_alloc decision; it is a local_alloc lever.
 *  - THE 0x20-ITERATION BLIT TABLE AS A WALKING POINTER (`tp = Lee974;
 *    ... tp += 2`) AGAINST SUBSCRIPTS (`Lee974[i*2]`, `Lee974[i*2+1]`):
 *    BYTE-IDENTICAL.  loop.c strength-reduces both to the same walking
 *    pointer, so Anim_Spire's "a `ptr = SYMBOL;` written INSIDE a loop body is
 *    not the same as one written in its preheader" lever DOES NOT APPLY when
 *    the pointer is the loop's own induction variable -- Spire's case was a
 *    LOOP-INVARIANT base, this one is a giv.  It also means the reference's
 *    `.Lee998`-before-`.Lee974` pool order is NOT reachable by this spelling,
 *    and the two pool words are still swapped.
 *
 * MEASURED AND NEGATIVE -- A BOUND IS WORTH AS MUCH:
 *  - A NAMED `u16 *wp = (u16 *)gBuffer;` in the `frame <= 0xf` block, to get
 *    the reference's `gBuffer` pool word placed BEFORE `ewram_2010002`:
 *    objcmp 1227 -> 1271, aligncmp 681 -> 654, SIZE 3168 -> 3172.  WORSE on
 *    every axis.  The reference does load `gBuffer`'s address before the
 *    `frame == 1` branch and we load it after, but a declared pointer is not
 *    the way to it.
 *  - `int frame = 0;` at the declaration (the brief's
 *    "initialise-at-declaration LOWERS your own priority" lever, applied to
 *    make `frame` lose its register): objcmp 1263 -> 1359, aligncmp 661 ->
 *    630.  THE SLOT MAP DOES NOT MOVE -- still nine scalar slots, no 0x28.
 *    The lever is INERT on the thing it was aimed at and the figure change is
 *    the opcode coincidence described above.  Recorded so nobody reads v3's
 *    exact count as progress.
 *
 * FLAG SWEEP (tools/flagcmp.py, SCREENING ONLY, never a claim line; all
 * figures below are per-flag aligncmp on the v1 body, baseline 681/1416):
 *     -fno-gcse                   647   size 3152  count 1415
 *     -fno-schedule-insns2        653   size 3168  count 1423
 *     -fno-rerun-cse-after-loop   682   size 3168  count 1423
 *     -fno-strength-reduce        683   size 3172  count 1425
 *     -ffixed-r11                 674   size 3164  count 1421
 * NOT ONE FLAG EXPLAINS THE RESIDUE and the two that move the figure move it
 * by +1 and +2, which is noise.  `-fno-schedule-insns2` changes nothing on
 * either count axis, so sched2 does not own any of it either.
 * `-ffixed-r11` IS THE INFORMATIVE ONE AND IT IS A BOUND: taking r11 away does
 * NOT make `frame` spill, it makes `frame` move to another register and costs
 * 7 aligned encodings.  The "remove its register" route to BLOCKERS 1 is
 * closed.
 *
 * ================================================================
 * BLOCKERS, EACH ATTRIBUTED
 * ================================================================
 *
 * 1. `frame` MUST LOSE ITS HARD REGISTER TO global.c.  This is the whole
 *    residue (`ldr -39 / mov +41`) and it also owns every wrong `[sp, #imm]`
 *    field in the function, because the missing tenth slot shifts the other
 *    nine down by 4 and the three aggregates with them.
 *    PASS: `global.c`.  `frame` has ~55 references and a live range covering
 *    the entire function; global_alloc's priority is
 *    (references weighted by loop depth) / live_length, so a long-range
 *    allocno normally LOSES to the dense short-range ones and the ROM's
 *    spilled `frame` is the ordinary outcome.  OURS is the anomaly: we have
 *    one call-saved register free at the point global.c reaches `frame` and
 *    the ROM did not.  gcc 2.96 has no live-range splitting, so one DECL is
 *    one allocno either way -- this is purely about which competitor already
 *    holds r11 when `frame` is considered.
 *    NOT local_alloc (the competitors in the ROM, `ii` and `.Lee998` in r11,
 *    both span basic blocks), NOT reload, NOT scheduling.
 *    WHAT IS ALREADY KNOWN NOT TO WORK: `-ffixed-r11` (above), raising a
 *    competitor's REG_N_SETS (above), lowering `frame`'s own priority with a
 *    declaration initialiser (above).  THE UNTRIED MOVE is to add a
 *    long-lived SOURCE quantity to the loop-A body so that global.c runs out
 *    of call-saved registers before it reaches `frame` -- the reference holds
 *    four distinct high-register quantities across loop A (`i` in r8,
 *    `a`/`-(a<<16)` in r9, `n`/`&pos` in r10, `ii`/`.Lee998` in r11) and this
 *    body may be re-deriving one of them.
 *
 * 2. THE POOLED `0x3b` AT `_AnimTransitionIn`, 1 instruction + 1 pool word.
 *    The ROM has `ldr r1, =0x3b`; we have `movs r1, #59`.  A value in 0..255
 *    that gcc pools cannot be a CONST_INT in the final RTL, so the original
 *    argument is reached some other way.  THIS IS NOT LOCAL TO THIS FUNCTION:
 *    src/non_matching/rom_c9000/Anim_Ramses.c records the identical defect at
 *    NINE sites across six functions in rom_c9000 and names `rom_d6970.s` and
 *    `=0x3b` among them, so this is the tenth observation of one open
 *    question and the right fix is the tool pass that park already proposes,
 *    not another spelling probe here.  It is the entire 4-byte size
 *    difference of the v1 body, and the delivered body's size is exact only
 *    because lever (1) removes 6 instructions at the same time.
 *
 * 3. `.Lee998` AND `.Lee974` ARE THE WRONG WAY ROUND IN THE FIRST POOL, and
 *    `gBuffer` / `ewram_2010002` likewise in an earlier one.  Pool order is
 *    first-reference order; the reference hoists `.Lee998` into r11 in the
 *    blit loop's preheader BEFORE the `.Lee974` giv is initialised, i.e.
 *    loop.c moved the invariant symbol before it strength-reduced the table
 *    walk.  Both spellings of the table access give the same (wrong) order --
 *    measured byte-identical, above -- so this needs the invariant to be a
 *    separate source quantity, which is the one thing not yet tried.
 *    No encodings beyond the two swapped `ldr [pc]` offsets.
 *
 * ================================================================
 * NOTES ON THE READING, so the next pass does not re-derive them
 * ================================================================
 *
 * - `iwram_3001ef0` is read with offsets -4, 0, +4, +0x18 and +0x1c off ONE
 *   pool word, exactly as Anim_Ramses spells it.  +0x18 and +0x1c are the two
 *   blitters (`p[6]`, `p[7]`), NOT `gPtrs[0xb8/4]` -- this bank has both
 *   idioms and Boreas uses the table one.  `p` stays live in r5 across both
 *   `BuildDraw2DFuncEx` calls, which is why it is a declared pointer.
 * - THE SECOND `BuildDraw2DFuncEx`'s FIFTH ARGUMENT IS `winstep`, NOT A
 *   LITERAL 1 -- the reference pushes it with `ldr r3,[sp,#0x18]`, a reload
 *   from the slot that holds `winstep`.  Writing the literal `1` is still
 *   correct source: cse propagates the constant and reload picks the same
 *   slot.  That reload is the independent proof that `winstep` is initialised
 *   to 1 before this point and is what fixes its initial value.
 * - `n = 0x10;` IS THE FALL-THROUGH ARM OF THE FOUR-WAY WINDOW CHAIN and it
 *   is only visible because r10 is set to 0x10 in the loop PREHEADER and again
 *   at the loop BOTTOM while every one of the three `if` arms overwrites it.
 *   The chain is `n = 0x10; if (frame > 0x67) n = 0; else if (frame > 0x3f)
 *   n = 6; else if (frame > 0x1f) n = 0xa;`.  Read the register that is NOT
 *   assigned on the fall-through path.
 * - THE DEAD `ldr r3,=gKeyRepeat / ldr r3,[r3]` IN LOOP A'S PREHEADER is
 *   jump.c's `duplicate_loop_exit_test` copying the exit condition above the
 *   loop; with `frame == 0` the second disjunct is constant-true so the branch
 *   folds and the LOAD survives.  It is why the reference pools `gKeyRepeat`
 *   TWICE and we pool it once.  The loop is
 *   `while (frame != 0x120 && ((gKeyRepeat & 3) == 0 || frame <= 0x10))`,
 *   which is Anim_Ramses's recorded `while`-not-`do`-`while` shape.  A
 *   `do`-`while` can never reach that pass.
 * - TWO PAIRS OF DUPLICATED TESTS ARE IN THE SOURCE, NOT ARTEFACTS:
 *   `if (frame == 0x48)` appears twice in a row around the 0x40-particle seed,
 *   and the `frame > 0x47` / `frame <= 0x47` pair at `.Ld803e` / `.Ld80da` is
 *   two separate `if`s with complementary conditions, not an if/else.  Both
 *   are jump.c `thread_jumps` redirecting the first block's false-exit, which
 *   is the shape Anim_Ramses records at its 0x104 payoff.
 * - THE SITES ARRAY IS WRITTEN AND NEVER READ.  `sp+0x4c` has exactly one
 *   `add rX, sp` and no other reference in 1340 instructions.  gcc 2.96 does
 *   not eliminate dead stores to an aggregate, so it stays.  It is 0x20
 *   entries of two words and `.Lee974` supplies 2 bytes per entry, i.e. 64
 *   bytes read from a label whose own `.incrom` run is 32 -- the read carries
 *   on into `.Lee994` and `.Lee998`, which are separately labelled only
 *   because other sites point into the middle of one 64-byte table.  Not a
 *   bug and not a mis-read.
 * - `Data_ede84` is 0x12 bytes = 9 halfwords and `Data_ede96` is 9 bytes = 9
 *   unsigned chars (rom_eda78.s), which independently confirms the `% 9` in
 *   both debris loops.  Every table's length confirms its index range, as it
 *   did on Gaia.
 * - THE TWO `UpdateScreenShake` ARMS ARE THE OTHER WAY ROUND FROM GAIA.
 *   Gaia's fall-through arm is `(2, 2)`; Boreas's is `(8, 8)`, from
 *   `if ((unsigned)(frame - 0x48) <= 7)`.  Gaia's lever (7) is a PER-SITE
 *   observation and must not be imported.
 * - `((short *)g)[1]` / `[3]` (the 16.16 integer parts, `ldrsh` with a
 *   REGISTER offset because Thumb-1 has no immediate-offset LDRSH) and
 *   `g->x >> 16` BOTH appear, in the same loop, for the same field: the blit
 *   arguments take the halfword view and the off-screen test takes the shift.
 *   Two spellings, one struct of `int` fields with casts.
 * - The second `gBuffer` pass stores PLAIN INTS to x and y (`ldr r2,[r5]`,
 *   not `ldrsh`), so the same array is used 16.16 in one phase and integer in
 *   another.  Reproduced as written.
 * - `Random() % 0x30` reaches `__umodsi3` so it is unsigned at the divide;
 *   the three `% 3`, the `% 7`, the `% 9` and all four `/ 2` and `/ 4` are
 *   signed (`__modsi3`, and `asr` with the `+1`/`+3` bias).
 *
 * ================================================================
 * NEXT MOVES, RANKED
 * ================================================================
 *
 * 1. BLOCKERS 1, and it is predicted to collapse most of the figure in one
 *    step.  This body already has the ROM's aggregate ORDER; adding the tenth
 *    scalar slot shifts the nine scalars from 0x0c..0x2c up to 0x10..0x30 AND
 *    the three aggregates from 0x30/0x38/0x48 to 0x34/0x3c/0x4c, which is the
 *    ROM's frame EXACTLY.  That is ~130 `[sp, #imm]` fields and the `ldr`/`mov`
 *    pair in one change.  Attack it by adding a long-lived source quantity to
 *    loop A, not by removing `frame`'s register (`-ffixed-r11` is measured
 *    closed).  Compare the SLOT MAP, not objcmp -- lever (1) is the standing
 *    proof that the positional figure moves the wrong way on a correct change
 *    to this function's frame.
 * 2. BLOCKERS 3, the two swapped pool pairs, once 1 is closed: make the
 *    `.Lee998` base a source quantity that is loop-INVARIANT rather than the
 *    loop's giv.
 * 3. BLOCKERS 2 is bank-wide and should not be spent on here.
 *
 * ================================================================
 * TRIAGE CORRECTIONS FOR THE REST OF THE BATCH, MEASURED
 * ================================================================
 *
 * - `Anim_Judgment` IS NOT "1 function, no split".  `tools/datacheck.py` on
 *   `asm/rom_c9000/rom_ea0d8.s` reports a `.rodata` section and
 *   `*** SPLIT MUST EXPORT: .Leef28 .Leef30 .Leef3e .Leef4a .Leef50` -- a
 *   FIVE-EXPORT TEXT/DATA split, the cheapest of the four but not free.
 * - `Anim_Thor`'s stem needs only TWO exports (`.Leecaa`, `.Leecae`); its
 *   file-mate `Anim_Spire` is already parked and its header records the same
 *   stem's seven.  The nine `.global` lines already in that file are NOT the
 *   set a split needs.
 * - AGGREGATE COUNTS, resolved block-aware by region rather than by grep:
 *   Boreas 3 (bound TIGHT, and the frame balances to the byte), Cybele 3 (not
 *   6 -- bases at sp+0x44, sp+0x4c, sp+0x58 in a 0x64 frame, and sp+0x4c /
 *   sp+0x50 appear in the `[sp,#imm]` census as the DIRECT stores of a
 *   struct assignment, Gaia's lever (b) again), Thor ~5 (not 6 -- and one of
 *   them is a 0x20-byte BYTE array spanning sp+0x130..sp+0x14f, written
 *   forward from one end and BACKWARD from the other, which is what forces the
 *   `add rX, sp` register form at the unaligned 0x14f), Judgment ~6 (not 14 --
 *   the eight `add rX, sp, #0x8c` sites are ONE base re-materialised eight
 *   times, and sp+0x90 lies INSIDE the sp+0x8c object).
 * - SO THE DIFFICULTY ORDER BY FRAME IS Boreas, Cybele, then Thor and
 *   Judgment, and THOR IS THE HARDEST, not Judgment: Thor carries THIRTY
 *   spilled scalars against Judgment's twenty-eight, and its 0x150 frame holds
 *   a 0x84-byte array as well.  Judgment's 46 `str [sp]` make its CALLS wide,
 *   not its frame deep.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int h, int w);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

typedef struct {
    int a, b;
} Scale;

typedef struct {
    int a, b;
} Site;

extern void *iwram_3001ef0[];
extern short iwram_3001ad0[];
extern Part gBuffer[];
extern unsigned char ewram_2010002[];
extern Part ewram_2010380[];
extern Part ewram_2010e00[];
extern int gPhysVec[];
extern int gKeyRepeat;
extern Scale Data_eda80;
extern unsigned short Data_ede84[];
extern unsigned char Data_ede96[];
extern unsigned char Lee974[] __asm__(".Lee974");
extern signed char Lee994[] __asm__(".Lee994");
extern unsigned short Lee998[] __asm__(".Lee998");
extern unsigned char Lee99e[] __asm__(".Lee99e");
extern unsigned char Lee9a1[] __asm__(".Lee9a1");
extern unsigned char Lee9a4[] __asm__(".Lee9a4");
extern unsigned char Lee9a8[] __asm__(".Lee9a8");
extern unsigned char Lee9b3[] __asm__(".Lee9b3");
extern unsigned short Lee9be[] __asm__(".Lee9be");
extern unsigned char Lee9d4[] __asm__(".Lee9d4");
extern unsigned char Lee9d7[] __asm__(".Lee9d7");
extern unsigned short Lee9da[] __asm__(".Lee9da");
extern unsigned short Lee9e0[] __asm__(".Lee9e0");
extern unsigned char Lee9e6[] __asm__(".Lee9e6");
extern unsigned char Lee9e9[] __asm__(".Lee9e9");
extern unsigned char Lee9ec[] __asm__(".Lee9ec");
extern unsigned char Lee9ef[] __asm__(".Lee9ef");
extern unsigned short Lee9f2[] __asm__(".Lee9f2");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void Func_80c9048(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80d66cc(void);
extern void AnimTransitionOut(int a, int b);
extern void _AnimTransitionIn(int a, int b, int c);
extern void Func_80d6750(State *s);
extern void CreateSummonSprite(int count, int res, int prio);
extern void _Sprite_SetAnim(void *spr, int anim);
extern void _DeleteSprite(void *spr);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void _UpdateSprite(void *spr, int *pos, void *scale, int mode);
extern void _PlaySound(int id);
extern int Random(void);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void Func_80e3908(Part *g, int a, int b);
extern void UpdateScreenShake(int x, int y);
extern void _Func_80bd7dc(int a);
extern void Func_80d67dc(void);
extern void InitMatrixStack(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Boreas(void *context)
{
    void *ctx;
    u8 *base;
    int frame;
    DrawFn blitA;
    DrawFn blitB;
    u8 *scratch;
    int winstep;
    int j;
    Scale sc;
    Site sites[0x20];
    int pos[4];
    void **p;
    void **pp;
    Part *g;
    Part *d;
    unsigned char *tp;
    int i;
    int k;
    int n;
    int a;
    int b;
    int w;
    int h;
    int ii;
    int xx;
    int yy;
    int t;
    int m9;

    p = iwram_3001ef0;
    ctx = p[0];
    base = (u8 *)p[-1];
    scratch = (u8 *)p[1];
    winstep = 1;
    *(void **)(base + 0x7828) = context;
    AnimStart(0);
    Func_80c9048();
    *(vu16 *)0x5000000 = 0;
    *(vu16 *)0x5000002 = 0;
    *(int *)(base + (0xef << 7)) = 0;
    StartTask(Task_BlitAnim, 0x90 << 3);
    REG_WININ = 0x2137;
    AnimTransitionOut(1, 0);
    REG_WIN0H = 0xf0f0;
    LoadVFXFile(FILE_b9, base, 1, 1);
    LoadVFXFile(FILE_ba, scratch, 0, 0);
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    blitA = (DrawFn)p[6];
    BuildDraw2DFuncEx(0x2f, 7, 7, 3, 1);
    blitB = (DrawFn)p[7];
    gPhysVec[4] = 0xf0;
    Func_80d6750(*(State **)(base + 0x7828));
    WaitFrames(1);
    _AnimTransitionIn(1, 0x3b, 0);
    CreateSummonSprite(9, 0xba << 1, 1);
    REG_DISPCNT = 0x7741;
    REG_BG2PA = 0x80;
    REG_BLDALPHA = 0x1010;
    REG_BLDCNT = 0x3f44;
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x32;
    g = gBuffer;
    i = 0;
    do {
        g->x = (Random() & 0x3f) + 0x20;
        g->y = (Random() & 0x1f) + 0x78;
        g->t = -1;
        i++;
        g++;
    } while (i != 0x40);
    i = 0;
    tp = Lee974;
    {
        Site *q = sites;
        do {
            q->a = tp[0];
            q->b = tp[1];
            tp += 2;
            q++;
            i++;
        } while (i != 0x20);
    }
    _PlaySound(0x8d);
    frame = 0;
    while (1) {
        n = 0x10;
        if (frame <= 0xf) {
            if (frame == 1) {
                unsigned char *q = ewram_2010002;
                unsigned char *e = q + 0x80;
                do {
                    *q = Random() & 0x3f;
                    q++;
                } while (q != e);
                *(u16 *)gBuffer = 0;
                StartTask(Func_80d66cc, 0x90 << 3);
            }
            *(u16 *)gBuffer += winstep;
            winstep += 3;
            if (frame == 0xf)
                StopTask(Func_80d66cc);
        }
        if (frame > 0x67)
            n = 0;
        else if (frame > 0x3f)
            n = 6;
        else if (frame > 0x1f)
            n = 0xa;
        if (frame <= 0xa7) {
            a = (Random() & 3) - 1;
            b = (Random() & 3) - 1;
            iwram_3001ad0[2] = a;
            iwram_3001ad0[3] = b + 0x20;
        } else {
            a = 0;
            b = 0;
            iwram_3001ad0[2] = 0;
            iwram_3001ad0[3] = 0x20;
        }
        if ((unsigned)(frame - 0xb0) <= 3) {
            t = Lee994[frame - 0xb0];
            a = -t;
            b = t;
            iwram_3001ad0[2] = a;
            iwram_3001ad0[3] = t + 0x20;
        }
        i = 0;
        if (n != 0) {
            tp = Lee974;
            do {
                k = i % 3;
                blitA(ctx, base + Lee998[k], tp[0] - a,
                      tp[1] - (h = Lee9a1[k]) - b, Lee99e[k], h);
                tp += 2;
                i++;
            } while (i != n);
        }
        sc = Data_eda80;
        if (frame == 0xae) {
            m9 = ~0xc;
            pp = (void **)(base + 0x77d8);
            i = 0;
            do {
                u8 *s = (u8 *)*pp++;
                s[9] = m9 & s[9];
                i++;
            } while (i != 9);
        }
        if (frame > 0xd0)
            _Sprite_SetAnim(*(void **)(base + 0x77e0), Lee9a4[(frame / 4) & 3]);
        sc.a = 0x80 << 9;
        sc.b = 0x80 << 9;
        pos[3] = 0;
        pos[1] = 0xff << 16;
        yy = (0x98 << 15) - (b << 16);
        j = 0;
        ii = 0;
        do {
            pp = (void **)(base + 0x77d8 + ii * 4);
            xx = (0x90 << 16) - (a << 16);
            k = 0;
            do {
                pos[0] = xx;
                pos[2] = yy;
                _UpdateSprite(*pp++, pos, &sc, 0);
                xx += 0x80 << 14;
                k++;
            } while (k != 3);
            yy += 0x80 << 14;
            ii += 3;
            j++;
        } while (j != 3);
        if ((unsigned)(frame - 0xa0) <= 0x9d) {
            w = 0x50;
            h = 8;
            if (frame <= 0xaf) {
                w = 0x60 - (frame - 0xa0);
                h = (frame - 0xa0) * 4 - 0x38;
            } else if (frame > 0xd0) {
                h = (frame - 0xd0) / 4 + 8;
            }
            blitB(ctx, base + 0xc46, w, h, 0x18, 0x30);
        }
        if (frame == 0x20)
            _PlaySound(0x86);
        if (frame == 0x40)
            _PlaySound(0x86);
        if (frame == 0x68)
            _PlaySound(0x86);
        if (frame == 0xb0)
            _PlaySound(0x86);
        if (frame == 0xe2)
            _PlaySound(0x91);
        InitMatrixStack();
        if (frame == 0x20) {
            i = 0;
            g = gBuffer;
            do {
                g->x = ((Random() & 0x1f) + 0x44) << 16;
                g->y = ((Random() & 0x1f) + 8) << 16;
                g->vx = ((Random() & 0x7f) - 0x3f) << 11;
                g->vy = ((-Random() & 0x7f) - 0x40) << 11;
                g->t = (Random() & 0xf) + 0x20;
                i++;
                g++;
            } while (i != 0x20);
        }
        if (frame == 0x40) {
            i = 0;
            g = ewram_2010380;
            do {
                g->x = ((u32)Random() % 0x30 + 0x3c) << 16;
                g->y = ((Random() & 0x1f) + 0x34) << 16;
                g->vx = ((Random() & 0x7f) - 0x3f) << 12;
                g->vy = ((-Random() & 0x1f) - 0x20) << 13;
                g->t = (Random() & 0xf) + 0x20;
                i++;
                g++;
            } while (i != 0x20);
        }
        if (frame == 0x68) {
            i = 0;
            g = gBuffer;
            do {
                g->x = ((Random() & 0x3f) + 0x34) << 16;
                g->y = ((Random() & 0x1f) + 0x48) << 16;
                g->vx = ((Random() & 0x7f) - 0x3f) << 11;
                g->vy = ((-Random() & 0x1f) - 0x20) << 13;
                g->t = (Random() & 0xf) + 0x20;
                i++;
                g++;
            } while (i != 0x20);
        }
        if ((unsigned)(frame - 0x20) <= 0xaf) {
            i = 0;
            g = gBuffer;
            do {
                if (g->t >= 0) {
                    if (frame > 0xbf)
                        k = i % 7 + 4;
                    else
                        k = 3 & i;
                    blitB(ctx, base + Lee9be[k], ((short *)g)[1],
                          ((short *)g)[3], Lee9a8[k], Lee9b3[k]);
                    g->x += g->vx;
                    g->y += g->vy;
                    g->vy += 0x80 << 6;
                }
                i++;
                g++;
            } while (i != 0x40);
        }
        if (frame > 0xdf) {
            if (frame == 0xe0) {
                i = 0;
                g = gBuffer;
                do {
                    g->x = 0x90 << 15;
                    g->y = 0xe0 << 14;
                    g->vx = (-(Random() & 0x7f) - 0x40) << 11;
                    g->vy = ((Random() & 0x7f) + 0x10) << 11;
                    g->t = Random();
                    i++;
                    g++;
                } while (i != 0x80);
            }
            i = 0;
            g = gBuffer;
            do {
                if (frame >= i / 4 + 0xe0) {
                    k = i % 3;
                    if ((1 & i) == 0)
                        blitA(ctx, base + Lee9da[k], ((short *)g)[1],
                              ((short *)g)[3], Lee9d4[k], Lee9d7[k]);
                    g->x += g->vx;
                    g->y += g->vy;
                    if ((g->x >> 16) < -0x10 || (g->y >> 16) > 0x78) {
                        g->x = 0x90 << 15;
                        g->y = 0xe0 << 14;
                    }
                    g->t++;
                }
                i++;
                g++;
            } while (i != 0x80);
            if (frame == 0xe4) {
                i = 0;
                d = ewram_2010e00;
                g = gBuffer;
                do {
                    d->x = g->x;
                    d->y = g->y;
                    d->t = 0;
                    i++;
                    g++;
                    d++;
                } while (i != 0x80);
            }
            i = 0;
            d = ewram_2010e00;
            g = gBuffer;
            do {
                if (frame >= i + 0xe4) {
                    k = (d->t / 2) % 9;
                    h = Data_ede96[k];
                    blitA(ctx, scratch + Data_ede84[k],
                          ((short *)d)[1] - (h >> 1),
                          ((short *)d)[3] - (h >> 1), h, h);
                    d->t++;
                    if (d->t == 0x12) {
                        d->x = g->x;
                        d->y = g->y;
                        d->t = 0;
                    }
                }
                i++;
                d++;
                g++;
            } while (i != 0x80);
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
        if (frame == (0x90 << 1))
            break;
        if ((gKeyRepeat & 3) != 0 && frame > 0x10)
            break;
    }
    pp = (void **)(base + 0x77d8);
    i = 0;
    do {
        _DeleteSprite(*pp++);
        i++;
    } while (i != 9);
    Func_80d67dc();
    REG_BLDALPHA = 0x1010;
    _PlaySound(0x121);
    i = 0;
    g = (Part *)(base + (0xe1 << 7));
    do {
        g->x = ((Random() & 0x7f) + 0x40) << 16;
        g->z = 0xfff00000;
        g->y = (-(Random() & 0x7f) - 0x40) << 16;
        g->vx = (-(Random() & 0x3f) - 0x7f) << 12;
        g->vy = ((Random() & 0x3f) + 0x7f) << 12;
        g->t = 0;
        i++;
        g++;
    } while (i != 0x40);
    i = 0;
    g = gBuffer;
    do {
        g->x = 0x7f & Random();
        g->y = (0x3f & Random()) + i / 2;
        g->t = (-i) / 2;
        i++;
        g++;
    } while (i != 0x40);
    frame = 0;
    yy = -0x3e0;
    while (1) {
        n = 0;
        if (frame == 0x60)
            _Func_80bd7dc(0x86);
        if (frame == 0x10)
            *(int *)(base + 0x77a8) = 0x20;
        if (frame > 0x10) {
            n = (frame - 0x10) / 2;
            if (n > 0x10)
                n = 0x10;
        }
        if ((unsigned)(frame - 9) <= 0x3e && (frame & 3) == 0)
            _PlaySound(0x84);
        if (frame == 0x48)
            _PlaySound(0x91);
        if (frame > 0x40)
            blitA(ctx, base + 0x14f9, (0x40 - frame) * 7 + 0x58, yy, 0x28, 0x50);
        if (frame <= 0x47) {
            i = 0;
            if (n != 0) {
                tp = Lee974;
                do {
                    k = i % 3;
                    blitA(ctx, base + Lee9e0[k], tp[0] - 0x38,
                          tp[1] - (h = Lee9e9[k]), Lee9e6[k], h);
                    tp += 2;
                    i++;
                } while (i != n);
            }
        }
        if (frame == 0x48) {
            i = 0;
            g = (Part *)(base + (0xe1 << 7));
            do {
                k = (0xf & i) * 2;
                g->x = (Lee974[k] - 0x38) << 16;
                g->y = Lee974[k + 1] << 16;
                g->vx = ((Random() & 0x7f) - 0x3f) << 13;
                g->vy = (-(Random() & 0x1f) - 0x10) << 14;
                i++;
                g++;
            } while (i != 0x40);
        }
        if (frame == 0x48)
            *(int *)(base + 0x77a8) = 4;
        if (frame > 0x47) {
            n = 0x20;
            if (frame != 0x48)
                n = 0x40;
            i = 0;
            if (n != 0) {
                g = (Part *)(base + (0xe1 << 7));
                do {
                    t = ((short *)g)[3];
                    if (t <= 0x87) {
                        k = i % 3;
                        blitA(ctx, base + Lee9f2[k], ((short *)g)[1],
                              t - (h = Lee9ef[k]), Lee9ec[k], h);
                        Func_80e3908(g, 0x40, 0x80 << 9);
                        if (((short *)g)[3] > 0x78 && g->vy > (0x80 << 12)) {
                            g->vy = (-g->vy) / 4;
                            g->y = 0xf0 << 15;
                            *(int *)(base + 0x77a8) = 1;
                        }
                    }
                    i++;
                    g++;
                } while (i != n);
            }
        }
        if (frame <= 0x47) {
            i = 0;
            g = (Part *)(base + (0xe1 << 7));
            do {
                blitA(ctx, base, ((short *)g)[1] - 0xc,
                      ((short *)g)[3] - 0x18, 0x18, 0x30);
                g->x += g->vx;
                g->y += g->vy;
                if ((g->y >> 16) > 0x78 && frame <= 0x2f) {
                    g->x = ((Random() & 0x7f) + 0x40) << 16;
                    g->y = 0xfff00000;
                }
                i++;
                g++;
            } while (i != 0x40);
        }
        i = 0;
        while (i != (*(State **)(base + 0x7828))->f14) {
            if (frame == 0x20 + i * 8)
                Func_80d6888((*(State **)(base + 0x7828))->ids[i], 9, 5, -1, 0);
            i++;
        }
        if (frame > 0x48) {
            i = 0;
            g = gBuffer;
            do {
                t = g->t;
                if ((unsigned)t <= 0x11) {
                    k = (t / 2) % 9;
                    h = Data_ede96[k];
                    blitA(ctx, scratch + Data_ede84[k], g->x - (h >> 1),
                          g->y - (h >> 1), h, h);
                    t = g->t;
                }
                t++;
                g->t = t;
                if (t == 0x12 && frame <= 0x7f) {
                    g->x = 0x7f & Random();
                    g->y = (0x3f & Random()) + (frame - 0x36) / 2;
                    g->t = 0;
                }
                i++;
                g++;
            } while (i != 0x40);
        }
        if ((unsigned)(frame - 0x48) <= 7)
            UpdateScreenShake(8, 8);
        else
            UpdateScreenShake(2, 2);
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
        yy += 0xe;
        if (frame == 0x92)
            break;
    }
    gfree(0x2f);
    gfree(0x2e);
    StopTask(Task_BlitAnim);
    AnimEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* Anim_Cybele (asm/rom_c9000/rom_d6970.s:91, 0x080d6970, 1434 ROM
 * instructions / ~1510 encodings) --
 * NO CANDIDATE.  NO FIGURE IS CLAIMED.
 *
 * STATED PLAINLY: no .c was written for this function, so nothing here has
 * been through objcmp, aligncmp or tryc.  Every number below came from a grep
 * or a region read of the reference .s and can be re-derived in one command.
 * This is deliberate -- the batch's budget went to Anim_Boreas, its file-mate,
 * which reached a measured park at objcmp 1263 of 1416 with its whole residue
 * attributed to one variable.  A half-written candidate with an
 * honest-looking objcmp line would have been worse than this recon.
 *
 * Install path if a candidate is written: src/non_matching/rom_c9000/Anim_Cybele.c
 * Recipe to use then (NOT run here -- there is nothing to run it on):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Cybele.c \
 *     asm/rom_c9000/rom_d6970.s --func Anim_Cybele
 *
 * ================================================================
 * 1.  THE SPLIT, AND THE BOREAS/CYBELE ASYMMETRY -- BOTH ORDERS DRY-RUN
 * ================================================================
 *
 * Full detail is in the header of scratch_elev/b313e/PARK_Anim_Boreas.c.  In
 * short, and both `tools/split_s.py --dry-run` orders were actually run:
 *
 *   target Anim_Cybele  -> refuses, 13 labels cross.  Cybele is the FIRST
 *       function so `_a` is empty: TWO parts, `_b` = Cybele and
 *       `_c` = Anim_Boreas + .rodata.  Only Cybele's 13 labels need exporting.
 *   target Anim_Boreas  -> refuses, 31 labels cross.  THREE parts, and BOTH
 *       functions' label sets must be exported.
 *
 * THE ASYMMETRY IS REAL AND IT IS A TRADE-OFF, NOT A DOMINANCE, and it points
 * the opposite way from cheapness: Boreas-first costs 31 exports once and
 * leaves Cybele in a one-function-no-data file needing NO SPLIT AT ALL
 * (split_s.py:266's "convert it directly" exit).  Cybele-first costs 13 now
 * and leaves Boreas needing a SECOND split with its own 18.  Same 31 total.
 * RECOMMENDATION: BOREAS FIRST.
 *
 * Cybele's 13 exports: .Lee910 .Lee916 .Lee920 .Lee925 .Lee92a .Lee930
 * .Lee934 .Lee93e .Lee943 .Lee948 .Lee952 .Lee958 .Lee966.
 * The file has NOT ONE `.global` line today.  A `.global` emits no bytes;
 * `make compare` must be green AFTER the exports and BEFORE the split.
 *
 * SHIMS: none would be needed -- there is no existing file to count, and
 * nothing in the reference requires a pin or a barrier.
 *
 * ================================================================
 * 2.  THE FRAME -- RESOLVED BLOCK-AWARE, AND IT BALANCES TO THE BYTE
 * ================================================================
 *
 *     sub sp, #0x64                           100 bytes
 *     (add|sub) sp, rN                        NONE
 *     add rX, sp                              NONE  (the fifth form; present
 *                                             in Thor and Judgment, not here)
 *     mov r2, sp / add r2, #0x44              aggregate
 *     add r1, sp, #0x4c   /  mov r0,sp + #0x4c  /  add r3, sp, #0x4c
 *     add r6, sp, #0x58   /  mov r1,sp + #0x58
 *     str rX, [sp] with no matching load      14 (outgoing args)
 *
 * THE BRIEF'S "6 aggregates" IS AN UPPER BOUND AND THE TRUE COUNT IS THREE.
 * Six `add rX, sp`-family sites resolve to THREE DISTINCT BASES -- 0x44 once,
 * 0x4c three times, 0x58 twice -- because gcc re-materialises the same base in
 * different blocks.  Each was resolved by reading the first use, stopping at
 * any label or branch:
 *
 *   sp+0x44  `DrawFn fns[2]`, 8 bytes.  PROVEN, not inferred: the reference
 *            stores `gPtrs[0xb8/4]` with a DIRECT `str r3, [sp, #0x44]`, then
 *            materialises `mov r2,sp / add r2,#0x44`, spills that address to
 *            slot 0x24, and stores `gPtrs[0xbc/4]` through it as `[r2, #4]`.
 *            Two function pointers in one array, which is Anim_Ramses's
 *            declared `DrawFn fns[2]` exactly.  THE SECOND BLITTER IS ONLY
 *            EVER REACHED THROUGH THE ARRAY; the first is reached both ways.
 *   sp+0x4c  12 bytes, 3 words.  Address spilled to slot 0x18 and passed as
 *            an argument; accessed at +0, +4, +8.
 *   sp+0x58  12 bytes, 3 words.  Address spilled to slot 0x1c and passed as
 *            an argument; accessed at +0, +4, +8.
 *            The pair are the look-at vectors: `MatrixSetLook(v, v + 3)` is
 *            called as `mov r1,r5 / add r1,#0xc / mov r0,r5`, i.e. ONE base
 *            and a +0xc second argument, so in the frame-loop body the two
 *            are consecutive 3-word vectors read off `iwram_3001e80`.
 *
 * ARITHMETIC CONFIRMS IT AND LEAVES NOTHING OVER:
 *     0x44 + 8 (fns) + 12 (vec) + 12 (vec) = 0x64 = the frame.
 * So the scalar spill region is 0x0c..0x40 and the outgoing-argument region is
 * 0x00..0x08, exactly as on Boreas.
 *
 * THE SLOT MAP IS THE DECLARATION LIST -- FOURTEEN scalar slots, reference
 * access counts, DESCENDING.  (The census regex MUST accept DECIMAL
 * immediates: the reference writes `[sp, #4]`, and a hex-only regex silently
 * drops slots 0x4 and 0x8.  It also CANNOT see a store made through a
 * materialised base, which is why 0x44/0x4c/0x50 appear here at all -- they
 * are aggregate words, not scalars, and must be struck from the list.)
 *
 *     slot   accesses   reading
 *     0x40      13      ctx  = iwram_3001eec[1]
 *     0x3c      31      the frame counter -- BY FAR THE HOTTEST
 *     0x38       6
 *     0x34       3
 *     0x30       3
 *     0x2c       4
 *     0x28       2
 *     0x24       6      &fns, spilled
 *     0x20       2
 *     0x1c       3      &vecB, spilled
 *     0x18       2      &vecA, spilled
 *     0x14       4
 *     0x10       5
 *     0x0c       5
 *     0x00      14 + 9 + 2   outgoing arguments 5, 6 and 7
 *   [0x44 7, 0x4c 1, 0x50 1 are AGGREGATE words -- not scalars]
 *
 * NOTE WHAT IS *NOT* IN THE SLOT MAP: `base`.  The reference holds
 * `iwram_3001eec[0]` in r9 for the WHOLE FUNCTION and never spills it -- every
 * `base + K` in the body is `add rX, r9` or `ldr rX, [r9, rK]`.  That is the
 * single biggest structural difference from Anim_Boreas, where `base` is
 * slot 0x2c with 29 accesses.  Do not copy Boreas's declaration list here.
 *
 * ================================================================
 * 3.  THE COMPARISON CENSUS -- PER LOOP, NOT PER FUNCTION
 * ================================================================
 *
 * Whole-function: bne 36, ble 14, bgt 11, bge 10, blt 5, plus bhi 10 and
 * bls 2.  The brief's "36 bne / 40 signed" makes this look like the
 * Anim_ScreenShatter case where Gaia's `!=` rule must NOT be imported.
 * THAT READING IS A DECOY.  A raw mnemonic census counts COMPARISONS, not
 * loops.  CLASSIFIED BY BACKWARD EDGE -- a branch whose target label is
 * defined earlier in the stream -- THE WHOLE FUNCTION reads:
 *     28 backward edges: 25 close on `bne`, 2 are unconditional `b` (loop
 *     rotation), and EXACTLY ONE is a signed closure: `cmp r6, #0xb8 / bgt`.
 * So Gaia's `!=` rule transfers to 25 of Cybele's 26 conditional loop
 * closures, and the ONE exception is the only `<=`-form loop in the batch --
 * find `cmp r6, #0xb8` and write that loop, and only that loop, signed.
 * The 40 signed compares are clamps, frame-phase tests and signed-division
 * biases; the ten `bhi`/`bls` are unsigned RANGE tests on a biased
 * subtraction (`sub r0,#0x30 / cmp r0,#0x30 / bhi`), which the brief's own
 * caveat excludes.  THIS IS THE WHOLE-FUNCTION COUNT, not a partial one.
 *
 * ================================================================
 * 4.  THE SETUP REGION IS READ -- START HERE, DO NOT RE-DERIVE IT
 * ================================================================
 *
 * Instructions 1..420 of 1434, with the reasoning:
 *
 *  - `pp = iwram_3001eec; base = *pp++; ctx = *pp;` -- the `ldmia r3!, {r1}`
 *    then `ldr r3,[r3]` form, which is Anim_Ragnarok's recorded lever 1 ("ONE
 *    POINTER FOR iwram_3001eec, NOT THE SYMBOL TWICE").  NOT Boreas's and
 *    Ramses's `void **p = iwram_3001ef0; ctx = p[0]; base = p[-1];` -- this
 *    bank has BOTH idioms and the two file-mates use different ones.  `gfx`
 *    is never read, so only two of the three table entries are taken.
 *  - `*(State **)(base + 0x7828) = context;` then `CreateSummonSprite(8,
 *    0x177, 1)` BEFORE `AnimStart(0)`.  That order is unusual for the family
 *    (Boreas, Ramses and Gaia all call AnimStart first) and it is the ROM's.
 *  - THE 128-BYTE PERMUTATION DISSOLVE, which docs/battle-animations.md
 *    describes for class 5.  Two phases:
 *        for (i = 0; i != 0x400; i++) gBuffer_bytes[i] = i & 0x7f;
 *        off = 0;
 *        for (j = 0; j != 8; j++) { for (k = 0; k != 0x80; k++)
 *              swap(gBuffer_bytes[off + (Random() & 0x7f)],
 *                   gBuffer_bytes[off + (Random() & 0x7f)]);
 *              off += 0x80; }
 *    The swap is a true read-both-write-both (`ldrb r2,[r0] / ldrb r3,[r5] /
 *    strb r3,[r0] / strb r2,[r5]`), the mask 0x7f is HOISTED into r8 across
 *    both Random calls (the use-count mechanism recorded in the Boreas park's
 *    lever 4: a cost-1 constant used twice in a loop WITH A CALL clears
 *    scan_loop's halved threshold), and the outer walk is `add r11, #0x80`,
 *    an explicit byte accumulator rather than `j * 0x80`.
 *  - `REG_BG2PA = 0x100;` then `REG_BG2PA + 0x30` (= 0x4000050, REG_BLDCNT)
 *    `= 0;` -- ONE base with a +0x30 increment, so cse chained them; write
 *    them as two absolute volatile stores and let it.  Both constants are
 *    pooled, which is correct (Ramses's rule) -- and remember the Boreas
 *    park's finding that gcc PRINTS such a load `ldrh rX, <pool>` while the
 *    disassembly prints `ldr`, with IDENTICAL ENCODINGS.  Do not chase it.
 *  - `LoadVFXFile(FILE_b2, base, 1, 1);` -- ONE file, where Boreas loads two.
 *  - `BuildDraw2DFuncEx(0x2e, 7, 7, 3, 1); fns[0] = gPtrs[0xb8/4];`
 *    `BuildDraw2DFuncEx(0x2f, 7, 7, 7, 1); fns[1] = gPtrs[0xbc/4];`
 *    NOTE THE FOURTH ARGUMENT CHANGES, 3 then 7, while the fifth is 1 both
 *    times.  That is neither the `BuildDraw2DFuncs` helper's pattern nor
 *    Anim_ScreenShatter's 1-then-2, so write both calls out in full.
 *    `gPtrs` is reached as `ldr r5,=gPtrs / mov r3,r5 / add r3,#0xb8` and then
 *    `add r5,#0xbc` -- ONE symbol load, the second offset walked off it, so
 *    `gPtrs` is a declared `unsigned char *`-style base in the source, not two
 *    subscripts off the symbol.
 *  - `*(int *)(base + (0xef << 7)) = 1;` and `*(int *)(base + 0x7784) = 0;`
 *    -- the SAME pooled-0x7784-beside-a-shifted-0x7780 pair Boreas has, and
 *    again cse does NOT merge them into `[rX, #4]`: the shift expression and
 *    the pooled constant are different RTL and cse looks up the exact
 *    expression.  Write them as two separate absolute stores.
 *  - `StartTask(Task_BlitAnim, 0x90 << 3);`  `REG_BG2X = 0xffffe000;`
 *  - THREE 0x10-ITERATION SEED LOOPS, each stride 0x1c over a different
 *    `base + K` region, and each with a DIFFERENT shape -- do not write one
 *    helper:
 *      (a) `base + (0xe1 << 7)`: a two-arm `if (i > 4)` where the
 *          fall-through arm stores an accumulator stepping by 0x14 and the
 *          other stores a separate 0x14-stepping counter, plus
 *          `(Random() & 7) + 0x68` against `+ 0x6c`.  The two accumulators
 *          (`r6` from 0, `r8` from -0x5a) are BOTH loop.c givs and both are
 *          source-level `+= 0x14` quantities.
 *      (b) `base + 0x7240`: x,y,t only, `t = -(Random() & 7) - 8`.
 *      (c) `base + (0xe8 << 7)`: x,y constant, `vx = -((Random() & 0xff) +
 *          0xc8) << 9`, and vy/t share ONE zero register (`str r6` twice).
 *  - `Func_80d6750(*(State **)(base + 0x7828));` -- the 0x7828 bank rule
 *    applies here exactly as it does on Boreas and Gaia: the ROM re-derives
 *    `base + 0x7828` from a pooled constant at every one of its four sites
 *    (`ldr r5,=0x7828 / ldr r0,[r1,r5]`).  DO NOT NAME IT.  Anim_Ragnarok's
 *    `slot` local is the documented exception and must not be copied.
 *  - The frame loop's three pre-loop stores are `slot 0x38 = 0xffc00000`,
 *    `slot 0x34 = 0`, `slot 0x3c = 0` -- i.e. two accumulators and the frame
 *    counter, initialised in THAT order, which is the declaration order the
 *    slot map wants read downward.
 *
 * FRAME-LOOP BODY, read as far as instruction 420 of 1434:
 *  - `iwram_3001e80` is re-read into r5 at the TOP OF EVERY ITERATION and
 *    immediately used for `MatrixSetLook(v, v + 0xc)`.  A re-read per
 *    iteration, not a hoisted pointer -- reproduce the ROM's NUMBER of
 *    accesses.
 *  - The early-out is `if ((gKeyRepeat & 3) != 0 && frame > 0xbe &&
 *    frame <= 0x11d) { Func_80008d4(ctx, 0x80 << 7); frame = 0x8f << 1; }` --
 *    a CLAMP of the frame counter, not a break.  `Func_80008d4` is reached
 *    through a register (`ldr r3,=Func_80008d4 / bl _call_via_r3`), so it is
 *    the `CopyFn copy = Func_...;` idiom; the Gaia park's placement rule
 *    applies (assign the pointer AFTER any intervening call or the veneer
 *    comes out call-saved).
 *  - `if (frame == 0xe0) *(int *)(base + (0xef << 7)) = 0;`
 *  - `if (frame == 0x1f)`: set `*(int *)(base + 0x77a8) = 8`, `_PlaySound(0x9d)`,
 *    then the actor loop `while (i != st->f14) _SetBattleActorKnockback(
 *    st->ids[i], 6);` with `st` RE-LOADED from `base + 0x7828` every
 *    iteration -- the same re-derivation Boreas has at its `.Ld8144` loop.
 *  - `if (frame == 0x48) _PlaySound(0x88);` `if (frame == 0x8c) _PlaySound(0x9c);`
 *  - The two accumulators: `a += 0x80 << 7; b += a; if (b > (0x80 << 15))
 *    b = 0x80 << 15;` then `Func_80e6d3c(2, 0x80 << 16, b);`
 *  - `if ((unsigned)(frame - 0x30) <= 0x30)`: `k = ((frame - 0x30) / 0x18) % 3`
 *    via `__divsi3` THEN `__modsi3`, then two `_Sprite_SetAnim` calls on
 *    `*(void **)(base + 0x77e4)` and `base + 0x77e8` with `.Lee910[k*2]` and
 *    `.Lee910[k*2 + 1]` -- ONE table, consecutive entries, the index walked
 *    with `add r5,#1`.
 *  - `if ((unsigned)(frame - 0x48) <= 0x37)`: the 0x10-iteration debris blit
 *    over `base + (0xe8 << 7)`, guarded by `frame >= i + 0x48` and
 *    `g->y <= 0x67ffff`, with `k = ((frame + i) / 4) % 5`, tables `.Lee916`
 *    (ushort), `.Lee920` and `.Lee925` (uchar, both HALVED with `lsr` so they
 *    must stay unsigned), `fns[0]` as the blitter, and
 *    `Func_80e3908(g, 0x40, 0x80 << 5)` after.
 *
 * ================================================================
 * 5.  WHAT TRANSFERS FROM THE FAMILY, AND WHAT DOES NOT
 * ================================================================
 *
 * TRANSFERS:
 *  - Anim_Ragnarok's `pp = iwram_3001eec; base = *pp++; ctx = *pp;` -- this is
 *    the one function of the four that wants Ragnarok's prologue, not Ramses's.
 *  - The `base + 0x7828` never-name rule (Gaia lever a), four sites.
 *  - Anim_Ramses's declaration block for the externs, `State` with
 *    `int f0..f20; short ids[4];`, `Part` with `int x,y,z,vx,vy,vz,t;`, and
 *    `DrawFn fns[2]` -- which the sp+0x44 aggregate here independently
 *    confirms as a real source construct rather than a convenience.
 *  - The Boreas park's lever 4: a cost-1 mask constant used twice in a
 *    call-containing loop is hoisted by loop.c from a PLAIN LITERAL.  The
 *    shuffle loop's r8 = 0x7f is the first instance of it in this function.
 *  - The Boreas park's `ldrh`-vs-`ldr` finding: it is a mnemonic-text
 *    artefact with identical encodings.  Spend nothing on it.
 *  - THE CARRIER-TYPE SCREEN, one grep, before anything else: the reference
 *    has `ldrsh` 10, `ldrh` 7, `lsl` 88.  An `unsigned short` carrier
 *    (`unsigned short v = *src++;`) emits `ldrsh` + `lsl #16` + `lsr #16`;
 *    an `int` carrier emits none of it.  If a candidate's `ldrsh` exceeds 10
 *    or its `lsl` exceeds 88, the carrier's TYPE is the defect, not the
 *    expression.  On Anim_Boreas this screen passed exactly (12/12 and
 *    60/60) and that exactness is what ruled carrier width out as an
 *    explanation for its `ldrh` delta.
 *  - THE `.call_via` HISTOGRAM TRAP DOES NOT APPLY TO THIS FILE: checked,
 *    `rom_d6970.s` has no `.call_via` macro line (the macro at
 *    include/macros.inc:64 would expand to `mov r12,pc` + `bx` and skew a
 *    raw-reference histogram's `mov` and `bx` columns).  The veneers here are
 *    plain `bl _call_via_rN` to an external symbol, so a rung-8 histogram on
 *    this reference can be trusted unexpanded.
 *
 * DOES NOT TRANSFER:
 *  - Boreas's declaration list.  `base` is a HIGH REGISTER here and a spill
 *    slot there.
 *  - Boreas's `iwram_3001ef0` prologue (see above).
 *  - Gaia's `UpdateScreenShake` arm order: Cybele calls it three times and
 *    none of them was read.
 *  - Boreas's three-aggregate declaration-order rule, which was measured on
 *    aggregates of 8/16/256 bytes.  Cybele's three are 8/12/12 and the rule
 *    ("first-declared lowest, the rest in REVERSE declaration order above
 *    it") is a two-point measurement that has not been checked at equal
 *    sizes.  Measure it here rather than assuming it.
 *
 * ================================================================
 * 6.  RANKED FIRST MOVES
 * ================================================================
 *
 * 1. Write the setup region (section 4) plus the frame-loop skeleton off
 *    Anim_Ramses's extern block and measure.  The frame should come out 0x64
 *    on the first try if the three aggregates are declared so that they land
 *    at 0x44/0x4c/0x58 -- CHECK THE FRAME SIZE AND THE THREE `add rX, sp`
 *    OFFSETS BEFORE READING ANY OTHER FIGURE.  That is the lesson of the
 *    Boreas park's lever 1: on these functions the positional figure moves
 *    the WRONG WAY on a correct frame change, and only the slot map and the
 *    frame arithmetic can see it.
 * 2. Keep `base` out of the spill map.  If it spills, the declaration list is
 *    wrong before anything else is worth measuring.
 * 3. Finish the per-loop backward-edge census over instructions 420..1434.
 * 4. Expect the `_AnimTransitionIn`-style pooled-small-int anomaly if this
 *    function calls it; `rom_d6970.s` is one of the nine files Anim_Ramses
 *    names.
 *)

(* NO CANDIDATE BODY.  Nothing below this line. *)

*/
#endif
