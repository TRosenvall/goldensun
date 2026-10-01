/* OvlFunc_883_200bfb0 -- TRIAGE ONLY, NO CANDIDATE WRITTEN, NO objcmp FIGURE.
 *
 * Nothing was compiled, so there is deliberately no `N of M` line and no
 * objcmp, aligncmp or shimcount figure in this file.  Do not read one in.
 *
 * Source asm: asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_c.s
 *
 * When a candidate exists, verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_780898/200bfb0.c \
 *     asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_c.s --func OvlFunc_883_200bfb0
 *
 * SPLIT SHAPE: A TWO-WAY CUT, AND THAT MAKES IT CHEAPER THAN IT LOOKS.  The .s
 * holds TWO functions -- OvlFunc_883_200b4c8 at ref line 6 and this one at
 * 1047 -- and THIS ONE IS THE LAST.  Per batch 312's positional-asymmetry
 * finding, cutting for the LAST function needs only a two-way split, where
 * cutting for a middle function necessarily leaves a piece on each side.  So
 * the landing cost here is one `split_s.py` run and two linker rows, not three.
 * Dry-run only was used; nothing was cut.  datacheck.py is silent for the file,
 * so there is no cross-file data symbol to export -- but re-check that after
 * the split, because datacheck's label set was the thing batch 312 found
 * under-reporting a `.lcomm`-defined symbol and the printed recipe then failed
 * to link.
 *
 * ITS FILE-MATE IS ALREADY PARKED AND CARRIES THE MECHANISM.
 * src/non_matching/overlays/200b4c8.c is the same overlay and the same script
 * shape, parked at 960 of 1027.  READ IT FIRST.  What transfers:
 *   - `-fno-expensive-optimizations` takes THAT function to an exact
 *     instruction count (1027/1027) and is the flag its park records -- but it
 *     is INERT on OvlFunc_883_20095dc in the same overlay, so it is a
 *     per-function sweep and not an overlay property.  Batch 312 also measured
 *     all six CSE-family flags BYTE-IDENTICAL on 200b1ac, and
 *     `-fno-rerun-cse-after-loop` byte-identical on 200b4c8 itself.
 *   - its struct A layout (pad00[6], f6, f8, fc, f10, f18, f1c, f55, f6c) and
 *     the `extern char *iwram_3001ebc` convention.
 *   - its established NON-signals, which must not be re-derived: the three
 *     shared coordinate constants are LITERALS not named locals; the two
 *     task-function pointers are NOT named; `__MapActor_GetActor(9)` before the
 *     second __Actor_TravelTo has its result DISCARDED.
 *   - and the warning that its EXACT SIZE IS A COINCIDENCE: 2792 bytes both
 *     sides with 26 extra `mov` cancelling 29 missing `ldr`/`lsl`.  Carry the
 *     per-opcode histogram as a column from the first compile.
 *
 * WORK DENSITY 6.3%, NOT 4.5%.  Dataflow re-measurement: 2126 instructions =
 * 1414 argument fill + 513 calls + 134 WORK + 44 genuine copies + 15 branch + 6
 * prologue.  134 work instructions against 884's 30.  By unresolved draft
 * lines it needs ~171 hand fixes against 884's 44:
 *       function  draft  calls  mem-ops  arity  raw-reg  label marks  HAND FIXES
 *       884         322    293      24      7       13        3            44
 *       969         457    363      80      0       37       12           117
 *       883         639    513     111     11       49       13           171
 *       896         717    556     143      2       69       16           214
 * Its 44 genuine copies are the MOST of the four -- the brief's `mov rlo,rhigh`
 * column reported 27, the fewest -- because 8 of them are one pointer
 * (`mov r7, r0`), which is the exact failure mode docs/elevation.md records
 * ("ten of one function's sixteen copies were a single pointer").
 *
 * FRAME, ALL FOUR GREPS -- AND THE FOURTH ONE MATTERS HERE:
 *   sub sp,#imm 1 (`sub sp, #0x1c`) | (add|sub) sp,rN 0 | mov rX,sp 0 |
 *   add rX,sp,#K 0 | str rX,[sp] at offset 0: ELEVEN, with no matching load
 * Eleven outgoing-argument store sites at offset 0 -- by far the most of the
 * four, and every one of them invisible to the first three greps because
 * offset 0 forms no address.  0x1c bytes of frame with no aggregate and no
 * spill: the whole frame is outgoing arguments for 5-or-more-argument calls.
 * Its file-mate's park notes the ROM "spills NOTHING, its whole frame is
 * outgoing arguments" at the same 0x1c, which is consistent.
 *
 * EVERY WIDE CONSTANT IS REBUILT: ZERO ARE HELD.  Build-multiplicity screen
 * over all 51 distinct values: NOT ONE is built once into a callee-saved
 * register.  29 are rebuilt more than once -- 0x8000 x18, 0xc000 x13,
 * 0x10000 x11, 0x5000 x11, 0x4000 x9, 0xc80 x9, 0xd000 x9, 0x102 x8,
 * 0xa00000 x6, 0x340 x6.
 * So the blanket pin pass IS the right step 1 here, as the brief says -- and
 * note this is the OPPOSITE of what its file-mate 200b4c8 needs, where the
 * blocker is that gcc hoists fifteen constants the ROM reloads.  Same overlay,
 * same script shape, opposite direction: 200b4c8 must be stopped from
 * hoisting, and this one must be stopped from nothing.  A per-function screen,
 * not an overlay property.
 *
 * POOLED MULTISET: 54 distinct, 94 load sites.
 *   45 numeric, 9 symbolic: OvlFunc_883_200d5c0 / 200d5e0 / 200d5f0 / 200d600 /
 *   200da08, gScript_883__0200e590 / e5cc / e614, iwram_3001ebc.
 *   Top: 0x4ccc x6, 0x2666 x5, 0x101 x5, gScript_883__0200e590 x4,
 *   iwram_3001ebc x4, OvlFunc_883_200da08 x4, 0x1999 x4, 0x34b x4, 0x105 x4.
 *   FOUR of the nine symbols are loaded more than once, and two of those are
 *   FUNCTION POINTERS passed to __StartTask -- so the Lever 1 pointer-reuse
 *   question is live here in a way it is not on 896.  On 884 the one held
 *   pointer was worth 553 differing positions; read these four before pinning
 *   anything that carries them.
 *   THE EIGHT-BIT-MOVABLE SCREEN HAS ONE SITE: a pooled 0 (`ldr r2, =0` at ref
 *   ~1130, moved into r8).  Per batch 312 that is a SITE WORTH READING and NOT
 *   a symbol -- `force_const_mem` on a spilled constant pseudo explains it as
 *   well as a relocation does, and a NAMED ZERO LOCAL DOES NOT CREATE A POOLED
 *   ZERO (three shapes, one object, measured).  NO `.sym` ENTRY ON THIS
 *   EVIDENCE, and check relocation parity before considering any symbol: this
 *   function has 9 symbols already and an added entry is wrong if the count is
 *   exact.
 *
 * CONTROL FLOW: 13 labels, SIX of them pool skips, EIGHT real branches --
 *   bls at ref 250, 379, 518  (UNSIGNED comparisons)
 *   bne at ref 715, 773, 1294, 1391, 1500
 * All eight forward, no backward branch, so NO LOOP -- which differs from its
 * file-mate 200b4c8, whose only real control flow is a 60-iteration do/while.
 * Three `bls` means three unsigned tests; read their polarity rather than
 * assuming, and cite no loop flag.
 *
 * NEXT.  Take it SECOND of the four, after 884 and before 969 if the file-mate
 * evidence is being used, since reading 200b4c8's park once serves both.  Order
 * of work: two-way split dry-run and `make compare` green BEFORE any .c (a
 * layout mistake and a bad decompilation look identical at the end); then the
 * eight branch polarities; then a blanket pin pass excluding the four
 * multiply-loaded symbols; then sweep `-fno-expensive-optimizations` per
 * function because its file-mate records it and the overlay does not transfer it.
 */
