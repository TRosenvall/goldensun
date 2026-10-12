/* OvlFunc_959_200a06c (0x0200a06c) -- STILL NON-MATCHING, device-free.
 *
 * THIS BODY'S OWN FIGURE: 17 of 38 encodings.  RE-DERIVED batch 340 brief C and
 * UNCHANGED from the park's claim, with the detail the claim needs:
 *     ENCODINGS differ in 17 of 38   INSTRUCTIONS ref 27 / ours 28
 *     SIZE equal at 96 bytes         RELOCATIONS DIFFER (same symbols, shifted)
 *   OURS IS ONE INSTRUCTION LONG, so this figure is a PHASE reading, not a
 *   distance; an alignment pad absorbs the extra halfword, which is why the
 *   sizes agree.  PINS 0, shims 0, devices 0.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7e7574/200a06c.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_a_c.s --func OvlFunc_959_200a06c
 *
 * Split shape: none needed for this function.  Pins: 0.
 *
 * ========== THE BATCH-332 VERDICT IS REFUTED: PINS ARE NOT THE ONLY ROUTE ======
 *
 * The header this replaces ended "THE ONLY ROUTE THAT REACHES ZERO IS PINS" on
 * nine `register ... __asm__` declarations.  That verdict was reached without
 * trying the one construct that reaches cse's hash table.  Batch 336 had already
 * established it and batch 340 measured it here:
 *
 *     __asm__ volatile ("" :: "i"(0));   between the first and second
 *                                        __MapActor_SetPos call
 *
 *     17 of 38, relocations DIRTY, 28 instructions   ->
 *      8 of 38, relocations CLEAN, 27 instructions
 *
 * ONE statement, no register named, and the length defect and the relocation
 * shift both disappear.  THE MECHANISM, read on disk: `cse_insn` flushes the
 * whole hash table at cse.c:5741-5745 when, and only when,
 * `GET_CODE (PATTERN (insn)) == ASM_OPERANDS && MEM_VOLATILE_P (PATTERN (insn))`.
 * A bare `__asm__ volatile ("")` has an ASM_INPUT pattern and a
 * `::: "memory"` form has a PARALLEL, so neither reaches it -- and both were
 * measured here, at that same position, reaching only 16 of 38: one better than
 * the base, relocations STILL DIRTY and the stream STILL 28 instructions, so
 * neither touches the length defect.  A `do { } while (0);` anchor there reads
 * the same 16.  The `:: "i"(0)` spelling is the only one that pays.
 *
 * THE PARK'S READING OF *WHAT* IS COMMONED STANDS: cse unifies the whole
 * SHIFTED value `0xb0 << 15` (const_int 5767168) across the first two calls, the
 * unified pseudo is live across a call, r4 is call-used under this tree's flags,
 * so the value lands in call-saved r5 and the epilogue pays `pop {r5}`.  Net one
 * instruction.  Its cost argument from cse.c:509-514 also stands, and explains
 * why no spelling of the two numbers can present them as two rtxes.
 *
 * ========== AND THE REST OF THE RESIDUE IS A SCHEDULING TIE, NOT A LENGTH ======
 *
 * Once the length is closed, the remaining 8 are PURE ORDER, the same shape at
 * all three call sites:
 *     ROM    mov r1,#K / mov r2,#K / mov r0,#slot / lsl r1,#18 / lsl r2,#15
 *     ours   mov r1,#K / mov r2,#K / lsl r1,#18   / lsl r2,#15 / mov r0,#slot
 * Read out of .23.sched2: the two `mov` halves carry INSN_PRIORITY 74, the two
 * `lsl` halves and `mov r0` all carry 73.  Among the three at 73 the class rung
 * (haifa-sched.c:4069-4094) ties -- all class 3, because arm.md gives the
 * mov->lsl dependence cost 1 -- and the dependent-count rung
 * (haifa-sched.c:4098-4110) ties at three dependents each, so it falls through
 * to INSN_LUID (haifa-sched.c:4113-4116) and `mov r0` loses because it has the
 * LARGEST luid.
 *
 * WHY IT HAS THE LARGEST LUID, and the lever that fixes it.
 * `precompute_register_parameters` (calls.c:845-855) copies any argument whose
 * `rtx_cost (value, SET) > 2` into a pseudo BEFORE `load_register_parameters`
 * emits the hard-register loads, and in thumb that test is unconditional
 * (SMALL_REGISTER_CLASSES is TARGET_THUMB, arm.h:1061, and *reg_parm_seen is set
 * by argument 0 itself at calls.c:816).  `0xac << 18` is thumb_shiftable_const
 * so it costs COSTS_N_INSNS (2) = 8 (arm.c:2077-2082) and IS precomputed; `0xc`
 * is under 256 so it costs 0 and is NOT.  The cheap argument's `mov` is
 * therefore always emitted last.
 *
 * THE FIX IS TO MAKE THE SLOT A NAMED LOCAL assigned before the call, so its def
 * insn sits at that statement's position and takes the SMALLEST luid -- but only
 * if cse is stopped from propagating the constant into the argument load and
 * killing that early def, which is the SECOND job the same volatile asm does.
 * With the slot and both coordinates as named locals and a flush before each of
 * the first two calls:
 *
 *     17 of 38  ->  2 of 38, instructions 27/27, relocations clean
 *
 * and sites one and two are EXACT.  The variant is c_v4d/c_oon.c in this brief's
 * scratch directory.
 *
 * SITE THREE IS THE LAST TWO ENCODINGS AND IT IS A GENUINE CONFLICT.  Its ROM
 * order is NOT the other two sites' -- `mov r1 / mov r2 / lsl r2 / mov r0 / lsl
 * r1` -- because with the window left open `lsl r2` has SIX dependents against
 * three (the following __MapActor_SetAnim insns anti-depend on r2) and wins the
 * dependent-count rung outright.  So site three needs the window OPEN for
 * `lsl r2` to win and the early slot def PRESERVED for `mov r0` to beat `lsl r1`
 * on luid -- and the only construct that preserves the def is the volatile asm,
 * which closes the window.  Measured: a device at site three gives 3 of 38, no
 * device gives 2, and the floor over 78 further site-three bodies (its slot and
 * coordinate locals present or absent, three statement orders, both device kinds
 * at every internal position and after the call) is 2.
 *
 * WHY THIS IS NOT INSTALLED.  `__asm__ volatile ("" :: "i"(0))` is a DEVICE, not
 * a result.  tools/shimcount.py counts it in its `operand` class, and it has no
 * landed precedent in this tree: it appears in thirteen files, every one of them
 * a park under src/non_matching, while the bare `__asm__ volatile ("")` -- which
 * docs/elevation.md:15143-15145 records as carrying NO fakematch row -- ships in
 * thirty-four landed files.  Under the pin policy (owner decisions, batch 319)
 * this body ships device-free at its honest 17 of 38 and the two-device route is
 * recorded for pass 3, where it should REPLACE the nine-pin route the old header
 * named: two statements that reserve no register are strictly less scaffolding
 * than nine hard-register declarations, and reach 2 rather than 0.
 *
 * THE DEVICE-FREE FLOOR, measured: 15 of 38 (still 28 instructions, relocations
 * still dirty, so still a phase reading and NOT a better distance).  It comes
 * from a `do { } while (0);` anchor immediately after __CutsceneStart, found by
 * tools/autosweep.py, and it does not improve when crossed with named locals for
 * the slot or the coordinates -- 66 bodies, floor 15.  It is recorded rather than
 * installed because an anchor with no mechanism behind it would make the park's
 * claim line harder to read, not easier.
 *
 * ALSO MEASURED IN BATCH 340 and inert or worse (pin-free, device-free):
 *   per-site named locals for the two coordinates, every flush position   8 floor
 *   the coordinate shifts split into `x = K; x <<= S;`                    8 floor
 *   one reused slot local for all three sites                             8 floor
 *   all eight per-file flag groups on the parked body (autosweep)         17, except
 *     -O1 and -fno-schedule-insns2, both 18 -- worse
 * The pre-reload scheduler is UNREACHABLE on this target and the old header's
 * "the pre-reload scheduling flag -- inert" row has a reason:
 * `if (TARGET_THUMB && flag_schedule_insns) flag_schedule_insns = 0;` at
 * arm.c:634-638 turns it off unconditionally.  Nobody should re-measure it.

 * ========================================================================
 * BATCH 344 BRIEF D -- FIGURE RE-DERIVED; THE GAP IS LOCATED TO FOUR SITES;
 * AND THE CLASS MECHANISM IS NOW NAMED, WHICH EXPLAINS WHY PINS ARE THE ONLY
 * ROUTE AND NO PIN-FREE SPELLING CAN BE
 * ========================================================================
 *
 * RE-DERIVED FROM SCRATCH: 17 of 38, ref 27 instructions against ours 28,
 * 10 against 10 thirty-two-bit entries, 96 bytes both, relocations DIRTY.
 * Unchanged from the claim above, to the digit.
 *
 * THE +1 IS FOUR SITES, NOT ONE, and they cancel.  Aligned listing:
 *     prologue    ref  push {lr}                            ours push {r5, lr}
 *                                                           ours movs r5, #0xb0
 *                                                           ours lsls r5, r5, #15
 *     site 1      ref  movs r2,#0xb0 / lsls r2,#15          ours adds r2, r5, #0
 *     site 2      ref  movs r2,#0xb0 / lsls r2,#15          ours adds r2, r5, #0
 *     epilogue    ref  --                                   ours pop {r5}
 * So: PROLOGUE +2, SITE 1 -1, SITE 2 -1, EPILOGUE +1.  Net +1.  Anyone reading
 * the net as "one wrong decision" will look in the wrong place; the single
 * decision is the COMMONING, and it costs three and saves two.
 *
 * THE MECHANISM OF THE WHOLE CUTSCENE-SCRIPT CLASS, read on disk.  `cse_insn`
 * calls `invalidate_for_call ()` at cse.c:5710-5715, and `invalidate_for_call`
 * (cse.c:2018-2029) walks the HARD REGISTERS clobbered by the call and removes
 * them from the quantity chains and the hash table.  IT NEVER TOUCHES A PSEUDO.
 * So:
 *   - an argument held in a PINNED hard register (r0/r1/r2) loses its
 *     equivalence at every `bl`, and the constant is rebuilt at each site;
 *   - an argument held in a PSEUDO keeps it across any number of calls.
 * `precompute_register_parameters` (calls.c:845-855, quoted in full below the
 * park's own reading) puts every argument whose `rtx_cost (value, SET) > 2`
 * into a pseudo, unconditionally in Thumb -- so `0xb0 << 15` is ALWAYS a
 * pseudo unless the source pins it.  THAT is why the old header's nine
 * `register ... __asm__` declarations reach zero and why nothing pin-free does.
 *
 * REACHABILITY, MEASURED OVER THE LABELLED CORPUS -- and it is the strongest
 * evidence in this park.  Across all 4,562 byte-matching generated `.s` files
 * in asm/, searching for the same shiftable constant built twice (`mov rX,#K`
 * followed by `lsl rX,#S`) inside ONE function:
 *     482 functions do it SOMEWHERE
 *       0 do it with no basic-block boundary and no call between the two builds
 *      56 do it with only `bl` calls between the two builds
 * Reading the `.c` beside one of the 56 -- asm/overlays/rom_7892c8/
 * ovl_30_c_c_a_a_a_c_c_a_c_a_b.s, `0xd0 << 8` built fresh at five
 * `__Func_8092adc` sites separated only by `__CutsceneWait` calls -- gives the
 * recipe, and it is PINS:
 *     #define PIN1 register int q0 __asm__("r0")
 *     #define PIN2 PIN1; register int q1 __asm__("r1")
 *     #define PIN3 PIN2; register int q2 __asm__("r2")
 *     { PIN3; q0 = 0x1; q1 = 0xd0 << 8; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
 * That is a LANDED, byte-matching file.  So the shape is reachable, the recipe
 * is known, and it is the pinned one.
 *
 * NINE PIN-FREE SPELLINGS OF THE SAME IDEA, MEASURED, ALL INERT OR WORSE.  The
 * idea was to kill the equivalence by REUSE instead of by pinning -- overwrite
 * the pseudo that holds 0xb0<<15 before site 2 reads it, which is what the
 * landed analogue gets for free because its two sites alternate values:
 *     one reused local for the z argument at all three sites      36  (29 insns)
 *     three reused locals q0/q1/q2, the analogue's shape pin-free 36  (29 insns)
 *     one reused local, z only, all three sites                   36  (29 insns)
 *     two locals whose ROLES SWAP between sites 1 and 2           39  (32 insns)
 *     the same with q0/q1/q2                                      39  (32 insns)
 *     the same with `unsigned` carriers                           39  (32 insns)
 *     one local reused for site 2's x only (the minimal kill)     37  (29 insns)
 *     per-site locals in braced blocks                            17  (inert)
 *     the site-1 z carrier reused for a later SetAnim slot        17  (inert)
 * EVERY one that actually changes the register's history makes the stream
 * LONGER, not shorter: a pin-free local becomes an allocno and costs more than
 * the two instructions it saves.  A pin does not, because it is a hard register
 * the call clobbers anyway.
 *
 * >> SO THE BOUND, WITH ITS EVIDENCE ATTACHED: on this shape, the only thing
 * >> that stops cse commoning a repeated shiftable constant across a call is a
 * >> CALL-CLOBBERED HARD REGISTER CARRIER (cse.c:5710-5715 + 2018-2029), and
 * >> the only C construct that produces one is `register ... __asm__`.  Nine
 * >> pin-free spellings measured; 0 of 4,562 generated files show the shape
 * >> without a call or block boundary between the builds.
 *
 * NOTHING ELSE TRIED.  Both cheap axes were already closed by batch 340 (all
 * eight flag groups, and the anchor axis whose floor is the recorded 15) and
 * were not re-run.  The device route to 2 and the pin route to 0 both stand as
 * recorded; this body still ships device-free and pin-free at its honest 17.
 *
 * NEXT: this is a PASS-3 DECISION, not a search.  Two statements of
 * `__asm__ volatile ("" :: "i"(0))` reach 2 and nine pins reach 0; the pin
 * route now has a LANDED PRECEDENT in this tree for exactly this shape
 * (ovl_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_b.c, PIN3).  Nobody should spend
 * another round on pin-free spellings of the carrier.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_809280c(int a, int b, int c);
extern void __MapTransitionIn(void);

void OvlFunc_959_200a06c(void)
{
    __CutsceneStart();
    __MapActor_SetPos(0xc, 0xac << 18, 0xb0 << 15);
    __MapActor_SetPos(0xd, 0xb0 << 18, 0xb0 << 15);
    __MapActor_SetPos(0xe, 0xb4 << 18, 0xc0 << 15);
    __MapActor_SetAnim(0xc, 5);
    __MapActor_SetAnim(0xd, 5);
    __MapActor_SetAnim(0xe, 5);
    __Func_809280c(0, 0xd, 0);
    __CutsceneEnd();
    __MapTransitionIn();
}
