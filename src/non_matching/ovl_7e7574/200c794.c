/* OvlFunc_959_200c794 -- NON-MATCHING, 6 differing STREAM encodings of 147
 * DEVICE-FREE, AND 4 OF 147 ONCE _MSG_244f AND _MSG_2455 EXIST.  THE BODY IS
 * UNCHANGED FROM THE PARK; what is new is the measurement and the mechanism.
 *
 * FIGURE, re-derived with objcmp at the head of batch 339 (the authority):
 *   6 differing STREAM encodings of 147 (ref 147 / ours 147);
 *   92 / 92 16-bit instructions -- IN PHASE;
 *   *** RELOCATIONS DIFFER ***, which the batch brief's table did not say and
 *   tools/autosweep.py does flag: our object carries two extra R_ARM_ABS32
 *   entries, `_MSG_244f` at 0x184 and `_MSG_2455` at 0x190, and the 47
 *   R_ARM_THM_CALL entries are identical in symbol AND address on both sides.
 *   So the relocation alarm here is ONE finding, not two: two undefined externs,
 *   not a different program.
 *
 * THE REAL DISTANCE IS 4, MEASURED NOT ARGUED.  Probed with a DEVICE (labelled
 * as such, and NOT shipped -- the body below has no `.equ`):
 *     __asm__(".equ _MSG_244f, 0x244f");
 *     __asm__(".equ _MSG_2455, 0x2455");
 *   gives 4 differing encodings of 147, 92/92 instructions, and NO relocation
 *   complaint.  So the park's claim that "those would assemble to identical
 *   bytes the moment the two symbols are defined" is now MEASURED rather than
 *   asserted, and the two symbols are worth exactly 2 encodings plus the
 *   relocation set.  Precedent for the symbols themselves is the elevated
 *   sibling OvlFunc_959_200cbfc.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7e7574/200c794.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_c_c.s --func OvlFunc_959_200c794
 *
 * PIECE (tools/piece.py, verbatim):
 *   asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_c_c.s  --  1 function(s)
 *       PARKED  OvlFunc_959_200c794   6 of 147   src/non_matching/ovl_7e7574/200c794.c
 *     => all 1 parked, so ONE candidate converts the whole TU.  No split needed.
 *
 * THE FOUR REAL ENCODINGS ARE TWO COPIES OF ONE ADJACENT SWAP, at the two
 * __MapActor_Emote sites whose second argument needs mov+lsl:
 *      rom   mov r1,#0x81 / mov r0,#0x19 / lsl r1,#1 / mov r2,#0x1e
 *      ours  mov r1,#0x81 / lsl r1,#1    / mov r0,#0x19 / mov r2,#0x1e
 * (the third Emote site takes `ldr r1, =0x105` from the pool and is identical.)
 *
 * *** THE MECHANISM, READ OUT OF .23.sched2 RATHER THAN INFERRED. ***
 * The park said "both follow conditional branches, so the lever's precondition
 * looks satisfied.  It is not enough."  Here is why, with the dump's own numbers
 * for basic block 4 (`-fsched-verbose=6`, which writes into `t.c.23.sched2` and
 * NOT to stderr):
 *
 *      insn  what              dep  prio  cost  forward dependents
 *      360   r1 = 0x81          0    58    1    215 361
 *      361   r1 = r1 << 1       1    57    1    215 85 80
 *       75   r0 = 0x19          0    57    1    215 83 80
 *       79   r2 = 0x1e          0    57    1    215 87 80
 *       80   call Emote         3    56   32
 *
 *   t = 0: ready {360, 75, 79}; 360 has priority 58 and wins outright.
 *   t = 1: ready {361, 75, 79}; ALL THREE AT PRIORITY 57, so every rung of
 *          rank_for_schedule has to be walked, and every one of them TIES:
 *            - the class rung (haifa-sched.c:4068-4095) ties because
 *              arm_adjust_cost (arm.c:2425-2427) zeroes anti/output dependences
 *              and arm.md:254 gives every `core_cycles single` insn latency 1,
 *              so this is an all-ALU window with nothing for it to separate;
 *            - the DEPENDENT-COUNT rung ties at 3 / 3 / 3, as the table shows;
 *            - so it falls to INSN_LUID, i.e. PRE-SCHEDULING CHAIN ORDER.
 *   The chain order (read from `.20.ce2`) is
 *        ... 334, 360, 361, 75, 77, 79, 80 ...
 *   so the `lsl` sits at a LOWER LUID than `r0 = 0x19` and wins the tie.
 *   To get the ROM, insn 75 must precede insn 361 IN THE CHAIN.  It does not,
 *   because the r1 argument's materialisation is emitted at the r1 argument's
 *   position and the Thumb split of `0x102` (split_all_insns, toplev.c:3376,
 *   which runs AFTER reload_cse_regs and BEFORE sched2 at toplev.c:3473) puts
 *   both halves there.  NOTHING IN THE SOURCE MOVES ONE ARGUMENT'S MOVE ACROSS
 *   ANOTHER ARGUMENT'S MATERIALISATION -- see the measured list below.
 *
 *   The two levers that WOULD reach it are the two rungs above LUID, and both
 *   need a dependent count or a latency this window does not have.  State the
 *   bound that way, not as "the source has no handle": if a spelling can give
 *   `r0 = 0x19` a FOURTH forward dependent while the `lsl` keeps three, the
 *   dependent-count rung decides it before LUID is consulted.  I did not find
 *   one.
 *
 * MEASURED INERT, all 6 of 147 with first diff at index 25 and the same two
 * relocations (20 variants; the park's own two negatives re-measured on this
 * body and 18 new ones):
 *   `0x102` / `0x100` as plain literals; `0x81 * 2`; `0x81 + 0x81`;
 *   the emote id in a named local; THE SHIFT IN PLACE on that local
 *     (`e = 0x81; e <<= 1;`) -- batch 336's lever, inert here because the
 *     pseudo is a local and not a PARAMETER with an assign_parms prologue save;
 *   the slot in a named local, before / after / BETWEEN the two halves of the
 *     shift (`e = 0x81; sl = 0x19; e <<= 1;`), which is the chain order the
 *     scheduler needs and which gcc folds away before it can matter;
 *   an anchor between the two halves of the shift, and after the slot store;
 *   __MapActor_Emote declared `()` (unspecified args), `short slot`,
 *     `unsigned short a`, `long a`; __MessageID's prototype dropped.
 * MEASURED WORSE: no declaration for __MapActor_Emote at all (8), `int
 *   __MapActor_Emote()` (8), no declaration plus plain literals (8),
 *   __Func_809280c's prototype dropped (8).
 *   So the PROTOTYPE axis is closed in both directions here, which is worth
 *   saying because the park's sibling needed exactly that lever.
 *
 * AUTOSWEEP on this body (recorded negative, do not re-screen):
 *   FLAG AXIS: -fno-gcse / -ffixed-r7 / -fno-strict-aliasing /
 *     -fno-strength-reduce / -fno-rerun-cse-after-loop / -fno-rerun-loop-opt
 *     ALL inert at 6; -fno-schedule-insns2 43, -O1 43.
 *     (-ffixed-r7 inert is unusual and worth noting -- this function keeps
 *     nothing in r7.)
 *   ANCHOR AXIS: 40 top-level boundaries, all inert or worse, best 6.
 *   The park's note that CSE_CFLAGS changes nothing because flag 0x313 is read
 *   and set in DIFFERENT basic blocks REPRODUCES: -fno-rerun-cse-after-loop is
 *   inert at 6.
 *
 * WHAT TO DO WITH IT.  The park's standing ruling is not to add _MSG_244f /
 * _MSG_2455 to message.sym until the remaining lines fall, "because a symbol
 * that does not complete a match is an entry that does not pay for itself".
 * That ruling still holds and this round did not earn the symbols.  But the
 * figure to CARRY is now 4, not 6, and the relocation alarm should not be
 * re-investigated.
 */
extern int _MSG_244f;
extern int _MSG_2455;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __Func_80925cc(int a, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_959_200c794(void)
{
    int m;

    __CutsceneStart();
    if (__GetFlag(0x941)) {
        __MessageID(0x2566);
        __ActorMessage(0x12, 0);
        __CutsceneEnd();
        return;
    }
    if (__GetFlag(0x313)) {
        __MessageID(0x2457);
        __Func_8092c40(0x19, 0);
        __CutsceneEnd();
        return;
    }
    __MapActor_Emote(0x19, 0x81 << 1, 0x1e);
    __Func_809280c(0x19, 0, 0);
    m = (int)&_MSG_244f;
    __MessageID(m);
    __ActorMessage(0x19, 0);
    __Func_809280c(0x19, 0x18, 0);
    __Func_8093500(0x18, 1);
    __Func_8093530();
    __CutsceneWait(0x3c);
    __Func_8093500(0, 1);
    __CutsceneWait(0x14);
    __MapActor_Emote(0x19, 0x105, 0x3c);
    __MessageID(m + 1);
    __ActorMessage(0x19, 0);
    __MapActor_Emote(0x19, 0x107, 0x3c);
    __MessageID(m + 2);
    __ActorMessage(0x19, 0);
    __CutsceneWait(0x46);
    __MapActor_Emote(0x19, 0x80 << 1, 0x3c);
    __Func_809280c(0x19, 0, 0);
    __MessageID(m + 3);
    __Func_8092c40(0x19, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __MessageID(m + 4);
        __Func_8092c40(0x19, 0);
    } else {
        __MessageID(m + 5);
        __Func_8092c40(0x19, 0);
    }
    __CutsceneWait(0x3c);
    __MapActor_Emote(0x19, 0x105, 0x3c);
    m = (int)&_MSG_2455;
    __MessageID(m);
    __Func_8092c40(0x19, 0);
    __Func_80925cc(0x19, 1);
    __MessageID(m + 1);
    __Func_8092c40(0x19, 0);
    m += 2;
    __MapActor_DoAnim(0x19, 3);
    __MessageID(m);
    __Func_8092c40(0x19, 0);
    __SetFlag(0x313);
    __CutsceneEnd();
}

