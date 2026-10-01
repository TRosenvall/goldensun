/* Anim_Cybele (asm/rom_c9000/rom_d6970.s:91, 0x080d6970, 1434 ROM
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
 */

/* NO CANDIDATE BODY.  Nothing below this line. */
