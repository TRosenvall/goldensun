/* OvlFunc_897_2009410 -- TRIAGE ONLY. NO CANDIDATE WRITTEN, NO objcmp FIGURE.
 *
 * Source asm: asm/overlays/rom_791794/ovl_30_c_c_a_c_a_a_c.s
 * Batch 312, brief B. There is deliberately NO "NON-MATCHING, N of M" line and
 * no `Verify with:` recipe, because there is no candidate body here for objcmp
 * to measure. parkcheck should report this NOFIGURE, which is correct.
 *
 * The batch's reconstruction budget went to OvlFunc_883_20095dc (size exact and
 * instruction count exact under CSE_CFLAGS; see PARK_OvlFunc_883_20095dc.c) and
 * to a cross-check of its two findings on OvlFunc_883_200b4c8. Everything below
 * is measured off this project's own disassembly.
 *
 * ===== SPLIT SHAPE: NONE NEEDED =====
 * `tools/datacheck.py` on the file prints nothing (exit 0, no interleaved
 * data). `tools/shimcount.py` reports no shims. `.thumb_func_start` /
 * `.func_end` bracket exactly ONE function, lines 9-1978, so the install path
 * src/non_matching/ovl_791794/2009410.c takes the whole file and no
 * tools/split_s.py run is needed. 1932 instructions.
 *
 * ===== FRAME, READ WITH THE THREE GREPS -- AND THE BRIEF'S READING OF IT =====
 * ===== WAS WRONG: 0x8 IS ALL SPILL, NOT ARGUMENT STAGING =====
 * Grep 1: `sub sp, #8`, one hit. Grep 2 (`mov rX, sp`): NOTHING -- no
 * aggregates. Grep 3 (`add rX, sp`): NOTHING. Every sp reference in 1932
 * instructions is one of four accesses, and all four are inside ONE loop:
 *
 *      str r1, [sp, #4] / str r2, [sp]      before `bl __WaitFrames`
 *      ldr r1, [sp, #4] / ldr r2, [sp]      after it, then `cmp r5,#0x13 / bls`
 *
 * So the frame is TWO SPILL SLOTS and ZERO outgoing-argument words -- no call
 * in the function takes more than four arguments. A 0x8 frame on a function
 * this size would normally be read as argument staging; here it is the
 * opposite, and the band doc's spill-slot lever therefore DOES apply (two
 * slots, each with two accesses).
 *
 * ===== LOOP-COMPARISON CENSUS, RUN ON THIS FUNCTION'S SLICE ONLY =====
 *   b 6 | beq 2 | bne 7 | bls 7 | signed (blt/ble/bgt/bge) 0
 * 21 labels, 22 branches, 1932 instructions. SEVEN `bls` -- the loop counters
 * are UNSIGNED, so `while (++i <= N)` / `do ... while (i <= N)`, not `<`.
 * FIVE of the six `b` are pool skips (`b .LX / .pool_aligned / .LX:` at 365,
 * 719, 1126, 1313, 1720); the sixth, `b .L16a4` at 265, is a REAL
 * if/else -- the else arm of a two-way `__MessageID` choice (`cmp r0,#0 /
 * bne .L1696`, then `add r0,r5,#1` on one arm and `add r0,r5,#2` on the
 * other). The brief's census for this function ("bne 5, signed 0") missed
 * all seven `bls` and both `beq`.
 *
 * ===== POOLED-CONSTANT MULTISET -- 34 distinct over 65 `ldr rX, =` SITES =====
 * OvlFunc_897_200b00c x7, 0x40c x6, 0x121 x5, OvlFunc_897_200a93c x4,
 * 0x20119e x4, OvlFunc_897_200a970 x3, 0xe666 x3, 0x7fff x3, 0x6666 x3,
 * 0x3333 x2, 0x101 x2, then 23 singletons including the message-id block
 * 0x10f8 / 0x10fb / 0x10fd / 0x10fe / 0x10ff / 0x1103 / 0x1104 / 0x110c /
 * 0x110d, the save bits 0x814 and 0x83f, and iwram_3001ec4.
 *
 * The highest-reload entries are SYMBOLS, not numbers: three task/callback
 * pointers account for 14 of the 65 sites. 0x20119e is an ADDRESS in the
 * overlay's own 0x0200xxxx range reloaded four times and will want a symbol,
 * not a literal.
 *
 * ===== THE EIGHT-BIT-MOVABLE POOLED CONSTANT SCREEN IS INERT =====
 * The smallest numeric pooled value is 0x101. All 20 numeric entries are above
 * 0xff, so there is no site where the reference pools a word it could have
 * loaded with one `mov #imm8`, and therefore no hidden relocation of the kind
 * that bought OvlFunc_969_20088b4 its count. (Same screen, same answer, on
 * both of the batch's other two targets.)
 *
 * ===== THE PURE/MIXED SCREEN: THIS IS VERY NEARLY A PURE REBUILD =====
 * The raw `mov rlo,rhigh` count is 16 and the brief read that as mixed. It is
 * not. Partitioned by what the source register holds (the method is written up
 * in PARK_OvlFunc_883_20095dc.c section 4):
 *
 *   prologue high-save moves                       3
 *   POINTERS (__MapActor_GetActor return values)  10   <-- r10 = actor 0xf x8,
 *                                                          r8 = actor 0   x2
 *   one-instruction constants (`mov #imm8`)        2   <-- a zero, in r8
 *   MULTI-INSTRUCTION or POOLED constants          1   <-- r8 = 0x6666, one copy
 *
 * MIXED-SIGNAL COUNT = 1. Ten of the sixteen copies are one pointer being read
 * out of a high register, which carries no information about constant reuse at
 * all. The standing prediction from 20095dc's two measured points (mixed-signal
 * 0 -> a 2-opcode pin residue; mixed-signal 11 -> a 17-opcode pin residue) is
 * that the blanket pin pass should very nearly close this function.
 *
 * ===== THE ONE LEVER THAT IS FORCED HERE, READ OFF THE PROLOGUE =====
 * The first four instructions after the high-save are
 *
 *      ldr r3, =iwram_3001ec4 / ldr r3, [r3] / mov r0, #0xf / sub sp, #8
 *      mov r9, r3             / bl __MapActor_GetActor / mov r10, r0
 *
 * r9 holds the VALUE of the global `iwram_3001ec4` -- not its address -- and is
 * used as `add rX, r9` at lines 56, 587, 877, 1180, 1545 and 1943, i.e. across
 * hundreds of calls. This is the corpus's "a pointer read from a global and
 * used across a call must be a source local" lever in its strongest form: cse
 * can never reuse the load because every call clobbers memory, so the six use
 * sites can only share one load if a pseudo holds it. ONE local, declared and
 * assigned at the top. r10 is the same shape for a pointer (actor 0xf) with
 * eight uses.
 *
 * Note the CONTRAST with 20095dc, which holds the global's ADDRESS in r11 and
 * reloads `[r11]` at each use. Same global family, opposite choice, so the
 * spelling is not transferable between the two and has to be read off each
 * reference.
 *
 * ===== WHAT A RECONSTRUCTION COSTS, MEASURED =====
 * `python3 tools/draft_script.py OvlFunc_897_2009410` drafts 677 lines and gets
 * the arithmetic of the call arguments right, leaving 185 lines needing hand
 * work: 129 memory operations and 34 over-guessed arities, across 21 label
 * boundaries. For comparison 20095dc drafted 595 lines with 109 to fix and
 * four of its labels were pool skips, so this function is roughly 1.7x the
 * hand work for 64 fewer instructions. The hand work, not the length, is the
 * cost -- which is the frame-triad triage lesson from batch 311 restated for
 * the script population: COUNT THE DRAFT'S UNRESOLVED LINES, not the
 * instructions.
 *
 * Recommended method, which is what worked on 20095dc: build the body with a
 * generator whose fix table is CONTENT-ANCHORED (each entry asserts a substring
 * of the draft line it replaces and the generator exits non-zero if it does not
 * match). A line-anchored fix table silently shifted by one twice during this
 * batch and both times produced a file that compiled.
 *
 * ===== 57 DISTINCT CALLEES, AND THE ONES THAT NEED CARE =====
 * 113 __CutsceneWait, 56 __Func_8092adc, 26 __WaitFrames, 19 __ActorMessage,
 * 17 __MapActor_GetActor, 17 __Func_8012330, 15 __MapActor_Emote,
 * 14 __PlaySound, 12 each of __MapActor_Jump / __Func_8091254 / __Func_8091200.
 * Ten overlay-local callees (OvlFunc_897_200ad94 x8, 200ad48 x7, 200a84c x6,
 * 200ac1c x4, 200a9a4 x4, 200a8dc x3, 200a820 x2, 200b000, 200aff0, 200ac9c)
 * have no prototype anywhere in src/ and must be declared from their own
 * disassembly. One __galloc_iwram / __gfree pair and one
 * __CreateActor / __DeleteActor pair bracket a sprite upload
 * (__LoadItemIcon / __UploadSpriteGFX) -- that region is the only part of the
 * function that is not script.
 */
