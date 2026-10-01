/* OvlFunc_911_20088ec -- TRIAGE ONLY. NO CANDIDATE WRITTEN, NO objcmp FIGURE.
 *
 * Source asm: asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_c.s
 * Batch 312, brief B. There is deliberately NO "NON-MATCHING, N of M" line and
 * no `Verify with:` recipe -- there is no candidate body here, so there is
 * nothing for objcmp to measure. parkcheck reports this NOFIGURE, correctly.
 *
 * ===== HEADLINE: THIS FUNCTION IS NOT IN THE POPULATION THE BRIEF ASSIGNED =====
 * The brief put it beside two 1900-instruction straight-line cutscene scripts
 * on the strength of "bne 8, signed 0, no frame". The bne and signed counts are
 * right and the conclusion drawn from them is not, because the census stopped
 * at bne. The full branch census on this function's slice:
 *
 *      b 18 | beq 35 | bne 8 | signed (blt/ble/bgt/bge) 0 | unsigned 0
 *
 * 61 branches and 61 labels in 2800 instructions -- about 46 instructions per
 * basic block, which is ORDINARY. By docs/band-800plus.md's own triage axis
 * (straight-line < 10 branches, branch-dense > 30) this is the BRANCH-DENSE
 * population, and that doc's section 8 says the existing 500-instruction lever
 * set should apply to it essentially unchanged -- NOT the straight-line
 * constant-reuse material the brief supplied. Nine of the 18 `b` are pool skips
 * (`b .LX / .pool_aligned / .LX:`); the other nine are real else-arms.
 *
 * ===== WHAT IT ACTUALLY IS: ONE SAVED FLAG GUARDING THIRTY BLOCKS =====
 * The shape is visible in the first ten instructions and it is not script:
 *
 *      bl __CutsceneStart / mov r0,#3 / ldr r5,=.L369c / bl __GetFlag
 *      str r0, [r5]
 *
 * `__GetFlag(3)` is stored to a word of overlay data, and the rest of the
 * function reloads it (`ldr r3, [r5]`) and tests it: THIRTY `cmp r3, #0 / beq`
 * guards. The remaining five conditionals are TWELVE `cmp r0, #0 / beq` NULL
 * guards on __MapActor_GetActor results, in a repeated idiom
 *
 *      a = __MapActor_GetActor(0);
 *      if (a) __MapActor_SetPos(N, a->f8, a->f10);
 *
 * which appears at least a dozen times with different N. There are ZERO loops:
 * all eight `bne` are paired with a `cmp` against a non-zero constant and
 * branch FORWARD. So no loop-shape lever, no induction variable, no
 * `check_dbra_loop`, and -fno-rerun-cse-after-loop must NOT be cited here (not
 * tested, and there is no loop for it to act on -- the same discipline brief E
 * of batch 311 broke).
 *
 * ===== FRAME: THERE IS NONE, AND THAT IS STRONGER THAN "NO sub sp" =====
 * All three greps find nothing, and the stronger statement is available:
 * `grep -c '\bsp\b'` on the 2887-line slice is ZERO. Not one stack reference
 * in 2800 instructions. No spill slots, no aggregates, no outgoing-argument
 * area, so no call takes more than four arguments. The spill-slot lever and
 * the aggregate-ordering lever both have no purchase, and the declaration-order
 * instrument reduces to the aligned figure, which the band doc warns averages
 * slot corrections away -- there being no slots, there is nothing to average.
 *
 * The prologue is a PARTIAL high-save: `push {r5,r6,r7,lr}` then `mov r7,r10 /
 * mov r6,r8 / push {r6,r7}` -- only r8 and r10, not the full r8-r11 bank. Two
 * high registers for 2800 instructions.
 *
 * ===== SPLIT SHAPE: NONE NEEDED =====
 * `tools/datacheck.py` prints nothing (exit 0). `tools/shimcount.py` reports no
 * shims. One `.thumb_func_start` / `.func_end` pair, lines 9-2895, so
 * src/non_matching/ovl_79e5c0/20088ec.c takes the whole file; no
 * tools/split_s.py run needed.
 *
 * ===== POOLED-CONSTANT MULTISET -- 37 distinct over 100 `ldr rX, =` SITES =====
 * iwram_3001ebc x13, .L369c x13, 0x101 x12, 0x9999 x6, 0x105 x6,
 * OvlFunc_911_200a7ac x4, 0x4ccc x3, 0x13333 x3, .L3694 x3, then
 * gScript_911__0200ad7c / ad20 / acfc / ac08 x2 each, OvlFunc_911_200a608 x2,
 * 0x406218 x2, .L36a0 / .L3690 / .L368c x2 each, and 16 singletons.
 *
 * ===== A BLOCKER THAT MUST BE SETTLED BEFORE ANY RECONSTRUCTION =====
 * `.L368c`, `.L3690`, `.L3694`, `.L3698`, `.L369c` and `.L36a0` are loaded from
 * the pool 23 times between them and ARE NOT DEFINED ANYWHERE IN THIS FILE OR
 * IN asm/overlays/rom_79e5c0/ -- and `tools/datacheck.py` confirms the file
 * carries no data. They are addresses of WRITABLE OVERLAY DATA that the
 * disassembler named with local labels. Six distinct words, read and written:
 * `.L369c` alone is written once (`str r0,[r5]`) and read thirty times.
 *
 * They are not literals and they are not in area.sym, const.sym, label.sym,
 * size.sym or wram.sym under those names. Until each has a NAME they cannot be
 * written in C at all, and a candidate that spells them as `*(int *)0x...`
 * would produce pooled integer constants where the reference has relocations --
 * which is the inverse of the batch-311 symbol lever and would be invisible in
 * the encodings. SETTLE THE SIX SYMBOLS FIRST; the relocation sequence is the
 * only instrument that can confirm them.
 *
 * ===== THE EIGHT-BIT-MOVABLE POOLED CONSTANT SCREEN IS INERT =====
 * Smallest numeric pooled value 0x101; all 22 numeric entries above 0xff. No
 * site where the reference pools a word a single `mov #imm8` could have loaded.
 * Across all three of this brief's targets: 120 distinct pooled values, ZERO
 * candidate sites. The brief ranked these three as "the best place in the tree
 * to test this at scale"; they are the wrong population for it, and the screen
 * costs one `grep | sort -u | head -1` to run before assigning it again.
 *
 * ===== THE PURE/MIXED SCREEN: THIS ONE REALLY IS MIXED =====
 * Raw `mov rlo,rhigh` is 11 and the brief's table reads that as the lowest of
 * the three. Partitioned by what the source register holds, it is the HIGHEST:
 *
 *   prologue high-save moves                       2
 *   one-instruction constants (`mov #imm8`)        2   <-- r8 = 1, two copies
 *   MULTI-INSTRUCTION constants                    7   <-- r8 = 0x80<<7, FIVE
 *                                                          copies; r10 = 0xc0<<8,
 *                                                          TWO copies
 *   pointers / symbol addresses                    0
 *
 * MIXED-SIGNAL COUNT = 7, against 0 for OvlFunc_883_20095dc and 1 for
 * OvlFunc_897_2009410. So on the corrected screen the ORDER OF THE BRIEF'S
 * THREE TARGETS IS REVERSED: the one with the fewest raw copies has the most
 * real ones. This reference genuinely commons two-instruction constants into
 * high registers and reuses them -- which means the blanket pin pass that took
 * 20095dc's opcode histogram to a two-instruction residue is the WRONG step 1
 * here, and this is the function in the batch that poses the unsolved mixed
 * case the brief was looking for.
 *
 * ===== 46 DISTINCT CALLEES; THE TWO THAT DOMINATE ARE LOCAL =====
 * 94 __CutsceneWait, 70 OvlFunc_911_200a5c0, 69 OvlFunc_911_200a5a8,
 * 54 __Func_8092adc, 46 __MapActor_GetActor, 43 __MapActor_SetAnim,
 * 36 __MapActor_DoAnim, 33 __MapActor_Emote, 30 __Func_80925cc,
 * 26 __MapActor_SetBehavior, 24 __Func_809259c, 22 __MapActor_Surprise,
 * 20 __Func_8092b08, 19 __MapActor_SetPos.
 *
 * 139 of the ~700 calls go to TWO overlay-local helpers (200a5a8 and 200a5c0)
 * that have no prototype anywhere in src/. Their signatures -- 200a5a8 is
 * called with two arguments and 200a5c0 with three at the sites sampled -- must
 * be read off their own disassembly before a draft is worth screening, because
 * draft_script.py guesses arity from which of r0-r3 were written since the
 * previous call and will mis-type them at 139 sites.
 *
 * ===== WHAT A RECONSTRUCTION COSTS, MEASURED =====
 * `tools/draft_script.py OvlFunc_911_20088ec` emits 1065 lines with 352
 * needing hand work: 270 memory operations and 82 over-guessed arities, across
 * 61 label boundaries -- and draft_script explicitly clears its register model
 * at every label, so with 61 labels in 2800 instructions roughly a fifth of
 * the draft is emitted with unknown registers. That is 3.2x the hand work of
 * 20095dc for 1.4x the instructions. Combined with the six unnamed data
 * symbols, this is the most expensive of the three and should not be assigned
 * until the symbols are settled.
 *
 * ===== RECOMMENDED NEXT TARGET OF THE THREE =====
 * OvlFunc_897_2009410: no split, one real conditional among 21 labels, a
 * mixed-signal count of 1, every pooled symbol already named, and a frame that
 * is two spill slots with two accesses each. Expect the method recorded in
 * PARK_OvlFunc_883_20095dc.c to carry: content-anchored generator, constant-set
 * check first, then the blanket pin, then the per-opcode histogram as the
 * ranking instrument.
 */
