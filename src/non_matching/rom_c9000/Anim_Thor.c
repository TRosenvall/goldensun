/* Anim_Thor (asm/rom_c9000/rom_e0564_c.s:79, 0x080e15e8, 1640 ROM
 * instructions) --
 * NO CANDIDATE.  NO FIGURE IS CLAIMED.
 *
 * STATED PLAINLY: no .c was written, so nothing here has been through objcmp,
 * aligncmp or tryc.  Everything below is a grep or a region read of the
 * reference and re-derivable in one command.  The batch's depth went to
 * Anim_Boreas (measured park, objcmp 1263 of 1416, whole residue attributed)
 * and Anim_Cybele (structural recon).
 *
 * Install path if a candidate is written: src/non_matching/rom_c9000/Anim_Thor.c
 * Recipe to use then:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Thor.c \
 *     asm/rom_c9000/rom_e0564_c.s --func Anim_Thor
 *
 * ================================================================
 * THIS IS THE HARDEST OF THE FOUR -- A CORRECTION TO THE BRIEF'S RANKING
 * ================================================================
 *
 * The brief ranked Anim_Judgment last on 14 aggregates.  Resolved by region
 * reading rather than by grep, the true counts are Boreas 3, Cybele 3,
 * Thor 5, Judgment 6 -- and the SCALAR counts invert the order:
 *
 *     function   spilled scalars   true aggregates   frame
 *     Boreas           10                3           0x14c
 *     Cybele           14                3           0x64
 *     Judgment         24                6           0xb8
 *     Thor             30                5           0x150
 *
 * THIRTY spilled scalars is the most of the four by a clear margin, and one of
 * Thor's five aggregates is a 0x84-BYTE ARRAY (33 words).  Rank it LAST.
 * The brief's "6 aggregates" for Thor was an upper bound from the `add rX, sp`
 * grep, which cannot tell a genuine aggregate from a sub-word stack scalar
 * (Thumb-1 has no sp-relative `ldrb`/`strb`, so every byte scalar materialises
 * a base too).
 *
 * ================================================================
 * THE FRAME: SIX FORMS, AND IT BALANCES TO THE BYTE
 * ================================================================
 *
 *     sub sp, #0x150                      336 bytes
 *     (add|sub) sp, rN                     NONE
 *     add rX, sp, #K                       0x94 (twice), 0xa0, 0x124
 *     mov rX, sp  then  add rX, #K         0x88, 0xa0
 *     add rX, sp                           TWO SITES -- THE FIFTH FORM
 *     str rX, [sp] with no matching load   29 (outgoing args)
 *
 * THE FIFTH FORM IS THE FINDING AND A FOUR-GREP FRAME MISSES IT ENTIRELY.
 * `add rX, sp` is register-PLUS-sp with the offset built first.  It is neither
 * the brief's grep 2 (`add sp, rN`, which changes sp) nor grep 3.  Both sites
 * are here:
 *     mov r2,#0x98 / lsl r2,#1 / add r2, sp      ->  sp+0x130
 *     ldr r2,=0x14f           / add r2, sp      ->  sp+0x14f
 *     mov r4,#0x92 / lsl r4,#1 / add r4, sp      ->  sp+0x124
 * The grep that finds it:
 *     grep -nE '^\s+add\s+r[0-9]+,\s*sp\s*$'
 * WHY IT IS FORCED, and this is the generalisable part: `add rX, sp, #imm`
 * encodes only WORD-ALIGNED immediates, so `sp+0x14f` CANNOT be expressed
 * that way at all.  It is also merely PREFERRED whenever the offset is
 * already in a register from a cost-2 `mov`+`lsl` -- which is why sp+0x124
 * appears BOTH ways in this one function (`add r6, sp, #0x124` at one site,
 * `mov r4,#0x92 / lsl r4,#1 / add r4, sp` at another).
 *
 * RESOLVED AGGREGATES, block-aware (first use, stopping at any label or
 * branch), with the arithmetic that confirms each boundary:
 *     sp+0x88   3 words   (0x88 + 0xc = 0x94)   address spilled to slot 0x60
 *     sp+0x94   3 words   (0x94 + 0xc = 0xa0)   written +0,+4,+8 as words,
 *                                               twice, once right before
 *                                               `bl MatrixTranslatev`
 *     sp+0xa0   0x84 bytes = 33 words (0xa0 + 0x84 = 0x124)
 *                                               address spilled to slot 0x34
 *     sp+0x124  3 words   (0x124 + 0xc = 0x130)
 *     sp+0x130  0x20 BYTES (0x130 + 0x20 = 0x150 = the frame)
 * TOTAL 0x88..0x150 = 0xc8 bytes, nothing left over.
 *
 * THE sp+0x130 OBJECT IS A BYTE ARRAY WRITTEN FROM BOTH ENDS AT ONCE, and
 * that is what forces the unaligned `add rX, sp`.  The reference holds
 * sp+0x130 in r5 and sp+0x14f in r4 and then, in one loop body,
 * `strb r1,[r0]` (r0 = r5, walking UP) and `strb r1,[r4] / sub r4,#1`
 * (walking DOWN), with `ldrb r3,[r0] / cmp r3,#0x3f / bls` between them.
 * 0x130..0x14f inclusive is exactly 0x20 bytes, so it is a `u8 buf[0x20]`
 * filled from the middle outward.  Write it as two pointers, not one index.
 *
 * ONE APPARENT HOLE, AND IT IS NOT A PHANTOM I CAN DISMISS: the scalar slots
 * run 0x0c..0x84 step 4, which is 31 offsets, and the census finds 30 --
 * sp+0x70 is never touched.  Checked against EVERY `add rX, sp`-family site
 * (0x88, 0x94, 0xa0, 0x124, 0x130, 0x14f): none is 0x70, so this is NOT a
 * store made through a materialised base and NOT the phantom the coordinator
 * warned about.  Treat it as a genuine unknown -- one allocated and unused
 * word, or one quantity whose only accesses this census cannot see.  Do not
 * build a declaration list that assumes 31 scalars.
 *
 * THE SLOT MAP IS THE DECLARATION LIST.  The hot end, reference access counts
 * descending (the census regex MUST accept DECIMAL immediates):
 *     0x84  3     iwram_3001f00[0]
 *     0x80  31    iwram_3001f00[-5]   -- `base`, and `base + 0x7828` is
 *                                        derived from it at the prologue
 *     0x7c  17    iwram_3001f00[-4]
 *     0x78  6     iwram_3001f00[-3]
 *     0x74  34    THE HOTTEST -- the frame counter
 *     then 0x6c:6 0x68:9 0x64:2 ... 0x0c:2, and 0x8:8 0x4:15 0x0:29 as
 *     outgoing arguments.
 * FOUR TABLE ENTRIES ARE READ, not three: `ldr r3,=iwram_3001f00` then
 * offsets -0x14, -0x10, -0xc and 0, each spilled.  So the source reads a
 * four-element window of the allocation table, which is one more than Gaia,
 * Ramses or Boreas take.  Spell it as ONE pointer with four subscripts -- the
 * reference re-derives the base with `mov r2,r3 / sub r2,#0x14` style copies,
 * which is cse working off a single symbol load.
 *
 * ================================================================
 * SPLIT, AND IT IS ALREADY HALF-DOCUMENTED
 * ================================================================
 *
 * `tools/datacheck.py asm/rom_c9000/rom_e0564_c.s`:
 *     data sections : .rodata      functions : Anim_Thor, Anim_Spire
 *     EXPORTS already global: .Leec5f .Leec63 .Leec68 .Leec70 .Leec74
 *                            .Leec7d .Leec86 .Leec98 .Leeca1
 *                            -- NOT the set a split needs
 *     Anim_Thor reads .Leecaa and .Leecae
 *     *** SPLIT MUST EXPORT: .global .Leecaa .global .Leecae
 * TWO EXPORTS -- the cheapest split of the four targets.  Its file-mate
 * `Anim_Spire` is ALREADY PARKED (src/non_matching/rom_c9000/Anim_Spire.c, 249
 * of 432) and that header records this same stem's TEXT/DATA shape and
 * Spire's own seven exports; read it before cutting.  Thor is the FIRST
 * function in the file, so by the Boreas/Cybele asymmetry recorded in
 * scratch_elev/b313e/PARK_Anim_Boreas.c, cutting Thor gives a TWO-part split
 * (`_b` = Thor, `_c` = Spire + .rodata) needing only Thor's two labels, while
 * cutting Spire first would need all nine.  `--dry-run` was NOT run on this
 * stem; run it before touching anything.
 *
 * ================================================================
 * WHAT ELSE IS KNOWN, AND THE FIRST MOVES
 * ================================================================
 *
 *  - `AnimStart(0x80 << 6)` -- a NON-ZERO argument, unlike Boreas, Cybele,
 *    Gaia and Ramses, which all pass 0.  Judgment passes the same 0x80 << 6.
 *  - `REG_BG2PA = 0x100;` pooled, which is correct (Ramses's rule).  And
 *    remember the Boreas park's finding: gcc PRINTS such a load as
 *    `ldrh rX, <pool>` where the disassembly prints `ldr`, with IDENTICAL
 *    ENCODINGS.  It is a mnemonic-text artefact.  Spend nothing on it.
 *  - `ldr r5, =0xbc` FOR A FILE ID, then `mov r0,r5` into TWO LoadVFXFile
 *    calls.  0xbc is in 0..255 and gcc pools it -- this is the SAME anomaly
 *    Anim_Ramses records at nine sites across six functions in rom_c9000
 *    (there on `_AnimTransitionIn`'s second argument), and it appears here on
 *    a LoadVFXFile FIRST argument.  That widens the anomaly beyond one
 *    callee and strengthens Ramses's case that these arguments are reached
 *    through a symbol or a memory load rather than as literals.  Worth the
 *    tool pass that park proposes; NOT worth a spelling probe.
 *  - NINE `LoadVFXFile` calls, TWELVE `BuildDraw2DFuncEx`, ELEVEN `gfree`,
 *    six `sin`/six `cos`, four `MatrixPitch`, two `MatrixRoll`, two
 *    `MatrixTranslatev` -- this is a 3-D matrix-stack animation, so the two
 *    3-word aggregates at sp+0x88 and sp+0x94 and the one at sp+0x124 are
 *    vectors, and the 0x84-byte object at sp+0xa0 is most likely a matrix
 *    stack frame or a 33-entry table.  Identify it before writing anything.
 *  - COMPARISON CENSUS -- DONE PROPERLY, AND THE ANSWER IS UNIFORM `!=`.
 *    The whole-function mnemonic ratio (bne 37 against ble 17, bge 9, blt 5,
 *    bgt 3, plus bhi 4 / bls 3) counts COMPARISONS, not loops, and is a
 *    decoy.  CLASSIFIED BY BACKWARD EDGE -- target label defined earlier in
 *    the stream -- Thor has 24 back edges: 21 close on `bne`, 3 are
 *    unconditional `b` (loop rotation), and ZERO are signed closures.
 *    GAIA'S `!=` RULE TRANSFERS WHOLE.  Write every loop `i != N`.  The 34
 *    signed compares are clamps, frame-phase tests and the biases of this
 *    function's unusually heavy signed division (24 `asr`).
 *  - THE CARRIER-TYPE SCREEN, one grep, before anything else: the reference
 *    has `ldrsh` 8, `ldrh` 4, `lsl` 72, `asr` 24.  An `unsigned short`
 *    carrier emits `ldrsh` + `lsl #16` + `lsr #16` where an `int` carrier
 *    emits none; if a candidate's `ldrsh` exceeds 8 or its `lsl` exceeds 72,
 *    the carrier's TYPE is the defect, not the expression.  One sibling brief
 *    measured that single token at 36 bytes and 25 encodings.
 *  - A rung-8 histogram on this reference needs the `.call_via` check: the
 *    macro at include/macros.inc:64 expands to `mov r12,pc` + `bx`, so a
 *    histogram over an UNEXPANDED reference under-counts `mov` and `bx` by
 *    one per macro site.  `rom_d6970.s` was checked and has none; this stem
 *    was NOT checked -- Thor has 12 `_call_via_r4` plus five more veneer
 *    registers, so verify before trusting a `mov` column.
 *
 * FIRST MOVES, RANKED
 * 1. Identify the sp+0xa0 0x84-byte object and the sp+0x130 byte array from
 *    their uses.  The frame cannot be right until both are named, and on
 *    these functions the frame is the only instrument that works early (see
 *    the Boreas park's lever 1: the positional figure moves the WRONG WAY on
 *    a correct frame change).
 * 2. Resolve the sp+0x70 hole before building the declaration list.
 * 3. Take Anim_Spire's extern block and struct shapes verbatim -- same file,
 *    already parked.
 * 4. Per-loop backward-edge census.
 */

/* NO CANDIDATE BODY.  Nothing below this line. */
