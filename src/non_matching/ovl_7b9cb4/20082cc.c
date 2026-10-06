/* OvlFunc_932_20082cc -- asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_a_c.s
 *
 * NON-MATCHING, 2 differing encodings of 74.  RE-DERIVED batch 328, --func AND
 * --whole; size exact, no SIZE line, no INSTRUCTION COUNT line, no POOL WORD
 * COUNT line, relocations clean.  PIN COUNT: 0.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7b9cb4/20082cc.c asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_a_c.s --func OvlFunc_932_20082cc
 *
 * ========== THE RESIDUE (unchanged, indices 29-30 of 74) ==========
 *      rom   asr r3,#0xe | mov r2,#0x80 | lsl r3,#0xe | lsl r2,#0xb | add r3,r2
 *      ours  asr r3,#0xe | mov r2,#0x80 | lsl r2,#0xb | lsl r3,#0xe | add r3,r2
 *
 * ========== BATCH 328: THE PARK'S DIAGNOSIS IS REPLACED ==========
 *
 * The park said "this is the scheduler choosing how to interleave two
 * independent two-instruction sequences, and the source does not appear to
 * reach it".  The first clause is right and the second was a guess.  The rung
 * is NOT a coin flip -- it is rank_for_schedule's DEPENDENT-COUNT rung
 * (haifa-sched.c:4029, the rung order being priority / register pressure
 * (`!reload_completed`, so dead at sched2) / interblock / last-scheduled-insn
 * class / DEPENDENT COUNT / INSN_LUID).
 *
 * .19.flow2 stream:  71 asr r3 | 73 lsl r3 | 216 mov r2,#0x80 | 217 lsl r2 | 77 add
 * .23.sched2 dependence table for that block, VERBATIM:
 *       71  prio 16  core : 185 73
 *      216  prio 16  core : 185 217
 *       73  prio 15  core : 185 77                  <- TWO dependents
 *      217  prio 15  core : 185 90 85 77            <- FOUR dependents
 *       77  prio 14  core : 185 92 90 85 79
 * t=0 {71,216} tie on priority, LUID picks 71.  t=1 216 wins on priority.
 * t=2 {73,217} tie on priority and 217 WINS 4 > 2.  With the counts EQUAL,
 * LUID (73 < 217) would pick 73 -- i.e. the ROM's order.
 *
 * WHY THE COUNTS CAN'T BE EQUALISED FROM SOURCE, with the evidence attached:
 *   * 217 (`lsl r2`) is the last setter of r2 in the block, so it collects an
 *     output dependence from EVERY later r2 writer.  The two extras are insn 85
 *     (`bl __MapActor_SetAnim`, which clobbers r2) and insn 90
 *     (`r2 = *(r5+0xc)`, the __Actor_TravelTo argument).
 *   * 73 (`lsl r3`) is capped at TWO for a structural reason: insn 77
 *     (`add r3,r3,r2`) re-sets r3 immediately, so reg_last_sets[r3] becomes 77
 *     and no later r3 dependence can ever reach 73.
 *   => the counts equalise only if NOTHING after the store writes r2 inside the
 *      scheduling region, i.e. only if the region ENDS at the store.  A free
 *      basic-block boundary there is the one open lever.
 *
 * MEASURED, 18 VARIANTS, EVERY ONE EXACTLY 2 (crossed, not screened singly):
 *   bare __asm__ volatile("") after the store                 2 (first 29)
 *   ... before the store / both sides                          2 (first 29)
 *   do { } while (0) after / before the store                  2 (first 29)
 *   asm BETWEEN the two halves (named `e`)        2, FIRST MOVES TO 28 -- the
 *       barrier kills the interleave outright (asr/lsl r3/mov r2/lsl r2)
 *   `e` and `k` both named + asm before the add                2 (first 28)
 *   `e` and `k` both named, no asm                             2 (first 29)
 *   `k8 = 0x80;` then `(k8 << 11)`                             2
 *   P2 pin r2 = 0x80, shift at the use                         2
 *   P2+P3 whole-value fills, either assignment order           2
 *   P2+P3 two-step fills in the ROM's own order                2
 *   P3 only, `v3 = d >> 14` then shift at the use              2
 * MEASURED WORSE:  `int k8 = 0x80;` at the declaration (cse folds it and the
 *   earlier code moves)                                        10 (first 22)
 * THE PIN LEVER THAT LANDED OvlFunc_927_2009818 IN THIS SAME BATCH IS EXACTLY
 * INERT HERE, in all five placements -- the brief's measured caution, confirmed.
 *
 * CORPUS BOUND, with its evidence.  A regex over every .s in asm/ for a
 * ONE-INSN SANDWICH inside a constant's mov/lsl pair
 * (`mov rY,#imm` | one insn on rX | `lsl rY`) finds:
 *      gcc-GENERATED (landed) .s files : 0 hits
 *      hand-written (still-asm) files  : 4721 hits
 * and the exact five-instruction ROM shape
 * `asr rX | mov rY,#imm | lsl rX | lsl rY | add` occurs ONCE in the whole tree
 * -- in this function's own reference .s.  So across 4,468 landed functions
 * gcc-2.96 under this invocation has never produced this interleave, which is
 * what the structural cap above predicts.  That is a bound ON THIS SHAPE, not
 * an instruction to stop looking: the free-basic-block-boundary question is
 * open, and so is the possibility that the ROM object was not built this way.
 *
 * SETTLED, so it is not re-derived: the poll loop at the end re-reads both
 * actors' +0xc every iteration WITHOUT volatile, because __WaitFrames sits
 * between the reads and gcc must assume the call writes through the pointers.
 *
 * ========== BATCH 329 (brief F): THE ONE OPEN LEVER IS MEASURED CLOSED ==========
 *
 * RE-DERIVED: 2 differing encodings of 74, first differing index 29, size
 * 188 = 188, instruction count 53 = 53, pool word count 20 = 20, relocations
 * clean, PIN COUNT 0.  The stream differs at EXACTLY indices 29 and 30 and
 * nowhere else.  The park's whole sched2 reading reproduces verbatim: block 2's
 * table gives 71/216 priority 16 and 73/217 priority 15, t=0 picks 71 on LUID,
 * t=1 picks 216 on priority, and t=2 faces {73 prio 15, 217 prio 15} where 217
 * wins FOUR dependents to TWO.
 *
 * THE PARK LEFT ONE LEVER OPEN -- "the counts equalise only if the region ENDS
 * at the store".  That is now QUANTIFIED, and the cheap route to it is closed.
 *
 *   do { } while (0) immediately AFTER the a+0x28 store     2 of 74 (idx 29,30)
 *
 * It does what the park predicted and still loses.  Read out of .23.sched2:
 * with the barrier, insn 230 (`lsl r2,#11`) has dependents `198 94 77` and
 * insn 73 (`lsl r3,#14`) has `198 77`.  The barrier anchor (insn 94) absorbs
 * reg_last_sets for every register, so the SetAnim call's r2 clobber and the
 * TravelTo argument's r2 load lose their output dependences on 230 -- FOUR
 * BECOMES THREE -- but THE ANCHOR ITSELF BECOMES THE THIRD DEPENDENT, because
 * 230 is still the last setter of r2 when the anchor is analysed.  Meanwhile 73
 * stays capped at 2 for the park's own structural reason: the add re-sets r3
 * immediately, so reg_last_sets[r3] is the add and never 73.
 *
 * So the floor is 3 against 2 and the gap cannot be closed from this direction.
 * 230 reaches 2 only if NOTHING after it writes r2 anywhere in the scheduling
 * region, the anchor included -- which needs a REGION END, not an anchor.
 *
 * AND THAT IS A DISTINCTION THE TREE HAD WRONG.  docs/elevation.md records
 * "A basic-block boundary -- do { } while (0); re-regions the scheduler".
 * IT DOES NOT RE-REGION ANYTHING.  In every dump taken here the anchor sits
 * INSIDE one block: this function's barrier variant is "basic block 2 from 68
 * to 198" with the anchor at insn 94 inside it, and on OvlFunc_969_200b600 the
 * whole function is "basic block 0 from 92 to 114" with both loop notes inside.
 * What the idiom plants is a reg_pending_sets_all TOTAL-ORDER ANCHOR: the first
 * insn after the notes is the one sched_analyze_insn then treats it as a scheduling barrier
 * (haifa-sched.c:3714 onward, CONFIRMED IN THE COMPILER SOURCE): at :3744 it
 * gets a REG_DEP_ANTI on every reg_last_uses entry, at :3748 and :3751 a
 * dependence of type 0 -- REG_DEP_TRUE, NOT anti -- on every reg_last_sets and
 * reg_last_clobbers entry, at :3756 flush_pending_lists also clears the memory
 * lists, and at :3754 reg_pending_sets_all is set.  Then :3780-3789 assigns
 * reg_last_sets[i] = this insn FOR EVERY REGISTER i, which is exactly why the
 * next insn to write any register takes a REG_DEP_OUTPUT on it.
 * That is why the same idiom
 * LANDED OvlFunc_969_200b600 and OvlFunc_969_200db90 this batch (where only the
 * anchor's POSITION mattered) and cannot touch this function (which needs the
 * region to end).  Treat "barrier" and "basic-block boundary" as two different
 * instruments from here on.
 *
 * ALSO MEASURED, barrier before the add with the halves named, three spellings
 * (`int k = 0x80 << 11`, `int k = 0x40000`, `int k = 0x80; k <<= 11;`):
 *   all 2 of 74 with the residue MOVED to indices 28,29 -- ours becomes
 *   asr r3 | lsl r3 | mov r2 | lsl r2 against the ROM's interleave.  The dump
 *   says why, and it is not what it looks like: cse folds the named constant,
 *   so the `mov r2` is materialised at the add and the barrier's anchor turns
 *   out to BE the `mov r2` (insn 235).  These three rows are therefore the
 *   park's own "asm BETWEEN the two halves" row re-derived, not a new family.
 *
 * NOTHING IN THE BODY BELOW IS CHANGED BY THIS BATCH.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_808e118(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __WaitFrames(int n);
extern void __MapActor_WaitMovement(int slot);
extern void OvlFunc_932_200b850(int a, int b);
extern void __Func_809202c(void);

void OvlFunc_932_20082cc(int arg)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    int d;

    a = __MapActor_GetActor(0);
    b = __MapActor_GetActor(8);
    __Func_808e118();
    __CutsceneStart();
    __MapActor_SetAnim(0, 0x16);
    __CutsceneWait(0xa);
    __PlaySound(0x98);
    __MapActor_SetSpeed(0, 0x33333, 0x19999);
    d = *(int *)(b + 0xc) - *(int *)(a + 0xc);
    if (d < 0)
        d = *(int *)(a + 0xc) - *(int *)(b + 0xc);
    *(int *)(a + 0x28) = ((d >> 14) << 14) + (0x80 << 11);
    __MapActor_SetAnim(0, 7);
    __Actor_TravelTo(a, *(int *)(b + 8), *(int *)(b + 0xc), *(int *)(b + 0x10));
    __WaitFrames(0xa);
    p = *(unsigned char **)(a + 0x50);
    p[9] |= 0xc;
    __MapActor_WaitMovement(0);
    while ((*(int *)(b + 0xc) >> 14) < (*(int *)(a + 0xc) >> 14))
        __WaitFrames(1);
    __CutsceneEnd();
    __PlaySound(0x9f);
    OvlFunc_932_200b850(arg, 0);
    __WaitFrames(0x14);
    __Func_809202c();
}
