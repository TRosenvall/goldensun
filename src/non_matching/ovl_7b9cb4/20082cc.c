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

 * ========== BATCH 336 (brief H): THE SCHEDULER'S CLASS RUNG, AND THE ADDEND ORDER ==========
 *
 * RE-DERIVED AS FOUND: 2 differing ENCODINGS of a 74-ENTRY STREAM (ref 74,
 * ours 74), first differing index 29, ref 039b ours 02d2, instructions 53 = 53,
 * pool words 20 = 20, relocations clean, PIN COUNT 0 (tools/shimcount.py).
 * The park's figure, its residue and its sched2 reading all stand.
 *
 * ----- A RUNG ABOVE DEPENDENT COUNT THAT THIS PARK NEVER CHECKED -----
 *
 * This park reasons from rank_for_schedule's DEPENDENT-COUNT rung.  There is a
 * rung ABOVE it that it does not name, and it is live on this target even
 * though it happens to tie here.  haifa-sched.c:4068-4095 classifies every
 * ready insn against last_scheduled_insn:
 *     class 3  no edge to it, OR insn_cost (last, link, cand) == 1
 *     class 2  anti/output edge
 *     class 1  data edge
 * and `if ((val = tmp2_class - tmp_class)) return val;` takes the HIGHEST
 * class, BEFORE the dependent count at :4096-4110 and before INSN_LUID at
 * :4113-4115.  Why it ties is a TARGET fact, not a structural one:
 *   * arm_adjust_cost (config/arm/arm.c:2414-2452) returns 0 for REG_DEP_ANTI
 *     and REG_DEP_OUTPUT, so an anti/output edge gives class 2;
 *   * for a data edge it returns the function-unit cost, and arm.md:254 gives
 *     every `core_cycles single' insn latency 1, so an ALU edge gives class 3;
 *   * the ONLY non-unit latencies are arm.md:260-294 -- `load' and `store1' 2
 *     (ldsched is "no" here: arm_ld_sched = (tune_flags & FL_LDSCHED) != 0,
 *     arm.c:165 and :568, and arm7tdmi carries no FL_LDSCHED), `mult' 16,
 *     `store2/3/4' 3/4/5.
 * SO, GENERALLY: a candidate that depends on the last-scheduled insn through a
 * LOAD, a MULTIPLY or a MULTI-REGISTER STORE falls to class 1 and loses to any
 * independent candidate, above the dependent count.  Here every insn in the
 * contested window is a single-cycle ALU op, so at t=2 (last_scheduled_insn =
 * 216, `mov r2,#0x80') insn 73 is class 3 by having no edge and insn 217 is
 * class 3 by having a unit-latency data edge.  THE CLASS RUNG CANNOT BE MADE
 * TO DISCRIMINATE HERE, and the fall-through to dependent count is real.
 * (Also recorded so it is not re-asked: SCHED_SORT at haifa-sched.c:4017-4021
 * calls qsort(rank_for_schedule) only when n_ready > 2 and swap_sort otherwise,
 * but swap_sort at :4120-4134 calls rank_for_schedule itself, so the comparator
 * IS reached at n_ready == 2 -- which is this function's ready set at t=2.)
 *
 * PRIORITY, the top rung, is closed for the same reason: priority() adds
 * insn_cost along the chain, 73's only consumer is the add (insn 77) and
 * insn_cost (73, link, 77) is 1 by arm.md:254, so prio 73 = 15 = prio 217.
 * Register pressure is dead (`! reload_completed'), interblock is same-bb.
 * So the free variables are EXACTLY TWO: the dependent count (which needs the
 * region end this park already measured unavailable) and WHICH REGISTER THE
 * ADD RE-SETS.  The second one is new, and this batch closes it.
 *
 * ----- THE ADDEND ORDER IS NOT A LEVER: fold CANONICALISES IT -----
 *
 * The park's structural cap is "73 is capped at TWO because insn 77
 * (`add r3,r3,r2') re-sets r3 immediately".  That cap is SYMMETRIC: if the add
 * re-set r2 instead, then 217 would be capped at one real dependent and 73
 * would collect the SetAnim call's r3 clobber, and the dependent-count rung
 * would INVERT in the ROM's favour with no region end needed at all.  Nobody
 * had varied the addend order.  MEASURED, SEVEN NEW ROWS, ALL AGAINST 2:
 *
 *   `*(a+0x28) = (0x80 << 11) + ((d >> 14) << 14);'        2 (idx 29, unmoved)
 *   the same + bare __asm__ volatile ("") AFTER the store   2 (idx 29)
 *   the same + bare __asm__ volatile ("") BEFORE the store  2 (idx 29)
 *   the same with `(d >> 14) << 14' in a named local `e'    2 (idx 29)
 *   `goto L1; L1: ;' immediately after the store            2 (idx 29)
 *   addends reversed + that goto/label                      2 (idx 29)
 *   `|' instead of `+' (same value, 0x40000 disjoint)       3  WORSE
 *
 * The reversal is INERT because fold canonicalises a PLUS_EXPR to put the
 * constant on the RIGHT, so `(0x80 << 11) + X' and `X + (0x80 << 11)' reach
 * expand as the same tree and the add's destination is still the shifted
 * value's register.  THE ADD'S DESTINATION IS THEREFORE NOT REACHABLE FROM
 * SOURCE, and with it the symmetric escape from the dependent-count cap.
 *
 * ----- AND THE REGION-END PROBE, TAKEN DIRECTLY -----
 *
 * `goto L1; L1: ;' right after the store is the cheapest possible region end:
 * a label with nothing between it and the jump.  It is byte-inert, which
 * re-derives on THIS function the control OvlFunc_927_2009818 recorded on its
 * own -- `jump' (pass 02) deletes the unconditional jump and then the
 * unreferenced label, so NOTHING reaches pass 23.  A sched2 region end needs a
 * CODE_LABEL alive at pass 23, and no zero-instruction source construct leaves
 * one: jump removes unreferenced labels at pass 02 and cleanup_cfg merges
 * fall-through blocks before .10.cfg.  THAT is the shape of the remaining
 * blocker, and it is not a spelling of the store.
 *
 * STILL 2 of 74 pin-free, device-free, flag-free.  Declining to close, with
 * both free variables now closed by measurement rather than by argument: the
 * region end (no surviving label) and the add's destination (fold's
 * canonicalisation).  What is left is the park's own open possibility that the
 * ROM object was not built from this spelling.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 2 of 74, instruction counts EQUAL, byte size and pool
 *   count equal too (this park is in phase on all three length dimensions, so
 *   its figure IS a real distance).
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: 23 placements swept, but 22 WERE BIT-IDENTICAL to
 *   the base, so only 1 DISTINCT PROGRAMS were actually tested.
 *   Those 1 are inert; the other 22 measured nothing.  No variant was capped and none failed to
 *   compile.
 *
 *   SO DO NOT RE-SCREEN THESE.  Every brief so far has spent part of a round on
 *   exactly that.  The residue here is NOT a placement or flag question; it is a
 *   register choice, a tie, or a pass-level effect, and it needs a hypothesis.
 *
 *   Read as "inert against THIS body".  A recorded negative expires silently
 *   when the body changes -- six batches running, the decisive lever has turned
 *   up inside a park's own negative list.
 
 * ===== BATCH 350 BRIEF A: 2 of 74 HELD.  36 ROWS, SEVEN PROGRAMS, AND BOTH
 * ===== REMAINING ROUTES CLOSED -- ONE OF THEM AS SELF-DEFEATING.
 *
 * RE-DERIVED FIRST: **2 differing encodings of 74**, ref 74 / ours 74,
 * instructions 53 / 53, first at index 29 (ref 039b `lsl r3,#14`, ours 02d2
 * `lsl r2,#11`), no SIZE / INSTRUCTION COUNT / RELOCATIONS line.  The figure IS
 * a distance.  BODY UNCHANGED.  Pin-free, device-free, flag-free.
 *
 * ---- THE CENSUS: 36 ROWS SWEPT, 29 OF THEM BIT-IDENTICAL.  SEVEN PROGRAMS.
 *
 * Round 1 -- the park's own eighteen recorded rows rebuilt: **FOUR distinct
 * programs**, fifteen rows bit-identical.
 *   BIT-IDENTICAL TO BASE (measured NOTHING): bare `__asm__ volatile ("")`
 *     after / before / both sides of the store;  `do { } while (0)` after and
 *     before;  `e` and `k` both named with no asm;  `k8 = 0x80;` as a statement;
 *     the addends reversed;  reversed + asm after;  reversed + asm before;
 *     reversed with `e` named;  `goto L1; L1: ;`;  reversed + the goto;
 *     `v3 = d >> 14` with the shift at the use.
 *   DISTINCT: asm BETWEEN the halves 2 (first moves to 28) -- and `e`+`k` named
 *     with an asm before the add is THE SAME PROGRAM as it;  `int k8 = 0x80;`
 *     at the declaration 10 (first 22);  `|` for `+` 3.
 *   So the park's "18 variants, every one exactly 2, crossed not screened" is
 *   honest about its figures and is **four programs**.  In particular
 *   `do { } while (0)` after the store, which batch 329 wrote a whole dependence
 *   reading against, is BIT-IDENTICAL TO THE BASE OBJECT.
 *
 * ---- NEW CLOSE 1: THE ADDEND ORDER IS UNREACHABLE EVEN AS A NON-CONSTANT TREE
 *
 * Batch 336 closed the symmetric escape on `fold`'s PLUS canonicalisation
 * (constant on the right).  That invites an obvious crossing of two rows from
 * this park's own list -- put the constant on the LEFT but make it a
 * NON-CONSTANT TREE, which `fold` has no constant to canonicalise.  Eight such
 * rows, **all eight BIT-IDENTICAL TO BASE**:
 *     `k8 = 0x80;    ... = (k8 << 11) + ((d >> 14) << 14);`
 *     `k = 0x80 << 11; e = (d>>14)<<14;  ... = k + e;`
 *     `k = 0x80 << 11;                   ... = k + ((d>>14)<<14);`
 *     `k8` left with `e` named;  `k8` left with `v3 = d >> 14` named;
 *     the same with the two definitions in the other order;
 *     `k8` on the RIGHT again (control, rejoins base);
 *     `e = -((d>>14)<<14);  ... = (0x80 << 11) - e;`  (the subtraction form)
 * So it is not only `fold`: cse/combine re-canonicalise the all-register form
 * too, and **no source order of the two addends survives to the back end**.
 *
 * ---- NEW CLOSE 2: THE SYMMETRIC ESCAPE WAS SELF-DEFEATING ANYWAY
 *
 * Worth the row because two batches chased it.  The escape is "make the add
 * re-set r2 instead of r3, so insn 217 is the capped one and insn 73 collects
 * the later r3 writers".  But the add IS `adds r3, r3, r2` and **that encoding
 * currently MATCHES** -- it is not in the residue.  An add that wrote r2 would
 * be `adds r2, r3, r2`, a different encoding at a position that is currently
 * exact.  So even if source could reach it, the best case trades two differing
 * encodings for at least one new one.  The dependent-count rung cannot be
 * inverted for free on this shape; the only free form of it is to shorten 217's
 * list, which needs the region end.
 *
 * ---- NEW CLOSE 3: THERE IS NO ZERO-INSTRUCTION WAY TO ADD A DEPENDENCE *EDGE*
 *
 * The rung needs insn 73 (`lsl r3,#14`) to collect two more dependents while
 * nothing else changes.  An insn that READS r3 between 73 and the add would be
 * exactly that, and the only zero-byte constructs that can read a value are
 * `asm` statements.  Neither kind works, and the reason is in the compiler:
 *   * **haifa-sched.c:3567-3597.**  The `ASM_OPERANDS / ASM_INPUT /
 *     UNSPEC_VOLATILE` case takes the barrier branch when
 *     `code != ASM_OPERANDS || MEM_VOLATILE_P (x)` -- and an asm with NO OUTPUT
 *     OPERANDS is implicitly volatile, so `__asm__ volatile ("" :: "r" (e))`
 *     takes it too.  The branch adds a dependence on EVERY `reg_last_uses`,
 *     `reg_last_sets` and `reg_last_clobbers` entry and sets
 *     `reg_pending_sets_all`, i.e. it is a TOTAL-ORDER ANCHOR, not an edge.
 *     MEASURED: one, two and three `"r" (e)` inputs, and two inputs in one asm,
 *     are **ONE program**, figure 2 with `first` moved 29 -> 28 -- the anchor
 *     kills the interleave outright instead of tuning it.  (`"r" (k)` 4,
 *     `"r" (d)` 7 -- both worse, both distinct programs, both the same anchor
 *     effect at a different position.)
 *   * a NON-volatile `ASM_OPERANDS` is the only form that escapes :3580's
 *     condition, and it does not survive to the back end.  MEASURED, **all four
 *     BIT-IDENTICAL TO BASE**: `__asm__ ("" : "+r" (e));` once and twice,
 *     `__asm__ ("" : "=r" (d) : "r" (e));` (writing a variable that is dead),
 *     and `__asm__ ("" : "=r" (t) : "r" (e));` with `t` unused.  A non-volatile
 *     asm whose outputs are dead is deleted before scheduling, so it adds
 *     nothing -- including nothing to the dependence graph.
 *   >> SO THE DEPENDENT-COUNT RUNG HAS NO SOURCE-LEVEL INSTRUMENT IN EITHER
 *      DIRECTION.  `volatile` asm is an anchor (too strong); non-volatile asm is
 *      deleted (too weak); and there is nothing in between in gcc-2.96. <<
 *
 * ---- WHERE THAT LEAVES THE PARK
 *
 * Every rung is now closed with a citation: priority (`arm.md:254`, unit
 * latency), class (`haifa-sched.c:4068-4095` with `arm_adjust_cost` at
 * `arm.c:2414-2452`), dependent count (region end unavailable -- no CODE_LABEL
 * survives pass 02; addend order unreachable AND self-defeating; no asm can add
 * an edge), LUID (already favourable, never reached).  36 rows, 7 programs,
 * nothing below 2.  **2 of 74 is the honest figure and this park is bounded
 * rather than untried.**  If it is scheduled again, the only live possibility
 * left is the park's own last sentence: that the ROM object was not built from
 * this spelling.
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
