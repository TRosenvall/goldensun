/* OvlFunc_943_200985c  [overlays/rom_7c7b9c]
 *
 * NON-MATCHING, 9 of 72 encodings  (MEASURED, batch 319 recipe backfill).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7c7b9c/200985c.c \
 *     asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c_a_a.s --func OvlFunc_943_200985c
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c.s
 *
 * BLOCKER CLASS: pool-loads-first argument ordering (3 sites), plus one
 * `orr` destination register. 62 lines against 62, NINE differing, and they
 * are only these two shapes:
 *
 *   THREE SITES, all the same:
 *     rom    mov r0, #imm / lsl r1, #0x10 / ldr r2, =X
 *     ours   lsl r1, #0x10 / ldr r2, =X   / mov r0, #imm
 *   The ROM fills the FIRST argument register before finishing the expensive
 *   ones; gcc finishes the expensive ones and sets r0 last. One
 *   __MapActor_SetPos site and both __MapActor_SetSpeed sites.
 *
 *   ONE SITE:
 *     rom    orr r5, r3 / strb r5, [r0]
 *     ours   orr r3, r5 / strb r3, [r0]
 *   r5 holds the shared constant 0x80, r3 the loaded byte. The ROM makes the
 *   CONSTANT's register the destination; gcc makes the loaded byte's register
 *   the destination. Note the FIRST of the two identical `|= 0x80` sites
 *   matches -- only the second differs, so this is not a property of the
 *   expression, it is allocation state at the second site.
 *
 * THE PER-CALL-SITE PROTOTYPE LEVER DOES NOT REACH THIS, and that is worth
 * recording because the sibling file in this very overlay is where the lever
 * is documented. src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_b.c
 * records that for __MapActor_SetPos "five of the seven calls want `mov r0`
 * LAST and two want it FIRST", solved by leaving the callee UNDECLARED for the
 * five (implicit int, r0 last) and routing the other two through `SetPosD`, an
 * __asm__ alias with a real prototype (r0 first).
 *
 * That reads as a two-way switch, and here NEITHER position moves anything:
 *
 *   MEASURED, every one of them 9 differing:
 *     real prototypes for both callees (the natural spelling)      9
 *     __MapActor_SetSpeed left undeclared (implicit int)           9
 *     __MapActor_SetSpeed via an __asm__ alias with a prototype    9
 *     the second `|= 0x80` written `*p = 0x80 | *p`                9
 *
 * The last is a no-op: gcc normalises the operand order back, so the `orr`
 * destination is not reachable by swapping the source operands. The first
 * three say the prototype lever is narrower than the sibling's note implies --
 * it moved r0 on THAT function's SetPos calls and moves nothing on this
 * function's, so whatever selects between the two orderings is not the
 * declaration alone. Do not spend a round re-running these four.
 *
 * BATCH 329 (brief H).  FIGURE RE-DERIVED AND HELD: 9 differing encodings of 72.
 * Exact length -- objcmp prints no SIZE, no INSTRUCTION COUNT and no POOL WORD
 * COUNT line.  Pin-free.  Not in any tools/dupfuncs.py group.
 *
 * THE PIECE HOLDS TWO FUNCTIONS AND BOTH ARE PARKED, so neither can install
 * alone: asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c_a_a.s carries
 * OvlFunc_943_200985c and OvlFunc_943_2009920, and
 * src/non_matching/ovl_7c7b9c/2009920.c is parked at 5 of 50 ON THE IDENTICAL
 * BLOCKER -- "r0 IN THE MIDDLE OF A THREE-ARGUMENT CALL", with the same two
 * shapes, one of them instruction-for-instruction the same call.  Work the two
 * as one target; the piece is worth two functions and the blocker is one.
 *
 * THE DECOMPOSITION, corrected.  Seven of the nine are the r0 slot at three
 * sites and TWO of the nine are the second `orr`; the park's "three sites, all
 * the same" is right about the cause and wrong about the shape, because the
 * ROM does not put `mov r0` FIRST -- it puts it SECOND, in all three:
 *
 *     rom    mov r1, #0xee / mov r0, #0x17 / lsl r1, #0x10 / ldr r2, =0x2720000
 *     rom    ldr r2, =0x4ccc / mov r0, #0x16 / ldr r1, =0x9999
 *     rom    ldr r1, =0xcccc / mov r0, #0x15 / ldr r2, =0x6666
 *     ours   the same insns with `mov r0` LAST, in each case
 *
 * and at the ONE __MapActor_SetPos site that matches, the ROM's `mov r0` is
 * LAST.  So "r0 first" is not the quantity; "r0 second" is, and the matching
 * site proves the discriminator is CONTEXT, not the call -- SetPos site 2
 * (0x17, 0xee << 16, 0x2720000) and site 3 (0x16, 0xcc << 16, 0x2090000) are
 * structurally identical and the ROM orders them differently.
 *
 * THE ONE BOUND THIS BATCH ESTABLISHED, with its evidence.  OUR r0 SLOT IS NOT
 * THE SCHEDULER'S DOING.  Rebuilt with -fno-schedule-insns2 the figure rises to
 * 12 and the three r0 runs are UNCHANGED, same shape, same positions -- so our
 * order is the RTL emission order and sched2 leaves these groups alone.  The
 * ROM's order therefore requires sched2 to MOVE `mov r0` up, which means a
 * rank_for_schedule tie-break we lose, not an emission order we could respell.
 * Every insn in the group feeds only the call, so arm_adjust_cost (arm.c:2416)
 * puts them all at prio(call)+1 and the tie goes down the rungs.  Note the
 * sibling park's `-fno-schedule-insns2` row reads 11 against the same 5, the
 * same direction.
 *
 * THE RUNGS, READ OUT OF THE COMPILER RATHER THAN RECALLED -- and this CORRECTS
 * an earlier sentence in this very append, which said the dependent-count rung
 * was "the only reachable rung".  IT IS NOT, and that is the direction of error
 * that closes a class, so it is worth the space.  rank_for_schedule
 * (haifa-sched.c:4029) has five rungs in this order:
 *
 *   1. priority                                      (:4041)
 *   2. register pressure -- `if (!reload_completed)`, so DEAD in sched2 (:4046)
 *   3. interblock: target-bb, speculative, probability (:4051, same-bb here)
 *   4. RELATION TO last_scheduled_insn                (:4069-4096)
 *   5. dependent count                               (:4097-4111)
 *   6. INSN_LUID, the stable fallback                (:4112-4116)
 *
 * RUNG 4 IS THE ONE THAT WAS MISSED.  It classifies each ready insn against the
 * insn just scheduled and prefers the HIGHEST class:
 *
 *     class 3  link == 0 (independent of it) OR insn_cost(...) == 1
 *     class 1  data dependent on it, cost != 1
 *     class 2  anti/output dependent on it, cost != 1
 *
 * So all three of our group members sit at class 3 only because the
 * `mov r1,#0xee -> lsl r1` latency is 1; the rung collapses and we fall to
 * rung 6, which is why our order IS the emission order.  The rung OPENS as soon
 * as any dependence in the group has latency != 1: the dependent insn drops to
 * class 1 and EVERY class-3 insn jumps ahead of it.
 *
 * AND arm_adjust_cost SAYS WHICH DEPENDENCE CAN DO THAT.  Reading arm.c:2416
 * in full, it returns 0 for anti/output, 1 for a true dependence whose consumer
 * is a CALL_INSN, 1 for a LOAD AFTER A STORE **only when the load's address is
 * CONSTANT_POOL_ADDRESS_P or mentions sp / fp / hard-fp**, and otherwise falls
 * through to the DEFAULT cost.  That last clause is the opening: a load after a
 * store whose address is NOT the pool and NOT the frame keeps its real latency,
 * which is not 1.
 *
 * SO THE DIMENSION NOBODY VARIED ON THIS PARK IS A MEMORY DEPENDENCE.  Every
 * probe recorded here and in the sibling park -- four callee declarations, the
 * shifted argument, arg0's storage three ways, the 0x80 two ways -- is
 * register-only, and rung 4 cannot be reached with registers alone when every
 * latency is 1.  NOT MEASURED, so this is a route and not a result; but it is
 * the first route on this park that is not already known to be inert, and the
 * cheap first probe is whether any ordinary memory traffic already in the
 * function (the `strb` through `p`, the `str r3,[r0,#0xc]`) can be made to sit
 * where one of these argument groups is scheduled.
 *
 * READING OUR OWN EMISSION ORDER, which the next round can use: our order is
 * `precompute_register_parameters` (expensive args, forward) followed by
 * `load_register_parameters` in REVERSE argument order.  That is why SetPos
 * site 2 comes out [mov r1 / lsl r1 / ldr r2 / mov r0] -- `0xee << 16` costs two
 * insns so it is precomputed and hoisted, the rest load backwards -- while
 * SetSpeed site 1 comes out [ldr r2 / ldr r1 / mov r0], pure reverse, nothing
 * precomputed.  Both reconstructions are confirmed against the
 * -fno-schedule-insns2 build.  THE r0 SLOT CANNOT BE REACHED BY MAKING arg0
 * PRECOMPUTED either: that would put `mov r0` FIRST, not second.
 *
 * MEASURED THIS BATCH, all 9 unless noted:
 *   the actor id named as one `int` local, reassigned at the three
 *     differing sites                                              9 (inert)
 *   a separate named `int` local per differing site                9 (inert)
 *   the same at all four sites, including the matching one         9 (inert)
 *   the shared 0x80 as one named `unsigned char` local used by
 *     both `|=` sites           46 instructions against 45, 33 (WORSE, and the
 *                               pad absorbs it -- a MISALIGNMENT figure)
 *   the 0x80 reached through a copy local (regmove
 *     replacement_quality 3 -> 1)                     46 / 45, 33 (WORSE, ditto)
 *
 * The first three close the FIRST argument as a dimension: the park had varied
 * the callee's declaration four ways and the sibling park had varied the SHIFTED
 * argument, and arg0's own storage is now varied too.  All inert.
 *
 * The last two are a real bound on the `orr` run and they cost an instruction,
 * which is worth stating plainly: THE ROM'S SHARED 0x80 IS NOT A NAMED LOCAL.
 * Naming it adds an insn whichever way it is spelled, so the r5 the ROM runs
 * through both sites is gcc's own choice for a propagated literal, and the
 * second site's `orr r5, r3` has to come out of the two-address rewrite on
 * literal operands.  regmove.c:1200-1205 skips the commutative swap when
 * replacement_quality(comm) >= replacement_quality(src); the loaded byte and a
 * propagated constant both score 3, so the swap is skipped -- and the copy-local
 * route to scoring the constant 1 is now measured to cost an instruction.  What
 * is NOT yet tried is reaching quality 1, which is a SHARPER statement than the
 * one this append first made.  replacement_quality (regmove.c:341, gating at
 * :1203-1204) reads, in full:
 *
 *     not a REG, or REG_LIVE_LENGTH < 0        -> 0
 *     NOT COPIED from another register         -> 3
 *     copied from a HARD register              -> 1
 *     copied from a PSEUDO register            -> 2
 *
 * So the loaded byte and the propagated constant both score 3 by the
 * "not copied" clause, 3 >= 3, swap skipped -- that part holds.  But the copy
 * local measured above reached quality **2**, NOT 1, because it copies from a
 * pseudo.  And 2 >= 3 is FALSE, so the gate at :1203-1204 DID open and the swap
 * still did not produce the ROM's shape -- it cost an instruction instead.
 * That is a more useful negative than "the copy route is worse": it says the
 * gate is not the binding constraint, so reaching quality 1 by copying the
 * constant from a HARD register is unlikely to pay either, and the `orr`
 * destination is probably decided downstream of regmove.  Spend the next round
 * on rung 4 above, not on this.
 */
extern unsigned char L5160[] __asm__(".L5160");
extern unsigned char gScript_943__0200c58c[];
extern unsigned char gScript_943__0200c628[];

extern void __CutsceneStart(void);
extern int __CutsceneEnd(void);
extern void __LoadFieldActors(unsigned char *p);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int who, int x, int z);
extern unsigned char *__MapActor_GetActor(unsigned int slot);
extern void __MapActor_SetSpeed(int who, int a, int b);
extern void __MapActor_SetBehavior(int who, unsigned char *s);
extern int __GetFlag(int id);
extern void OvlFunc_943_200c218(void);

void OvlFunc_943_200985c(void)
{
    unsigned char *p;

    __CutsceneStart();
    __LoadFieldActors(L5160);
    __WaitFrames(1);
    __MapActor_SetPos(0x14, 0, 0);
    __MapActor_SetPos(0x17, 0xee << 16, 0x2720000);
    __MapActor_SetPos(0x16, 0xcc << 16, 0x2090000);
    *(int *)(__MapActor_GetActor(0x16) + 0xc) = 0x80 << 13;
    p = __MapActor_GetActor(0x16) + 0x59;
    *p |= 0x80;
    __MapActor_SetSpeed(0x16, 0x9999, 0x4ccc);
    __MapActor_SetBehavior(0x16, gScript_943__0200c58c);
    p = __MapActor_GetActor(0x15) + 0x59;
    *p = 0x80 | *p;
    __MapActor_SetSpeed(0x15, 0xcccc, 0x6666);
    __MapActor_SetBehavior(0x15, gScript_943__0200c628);
    if (__GetFlag(0x109) != 0)
        OvlFunc_943_200c218();
    __CutsceneEnd();
}
