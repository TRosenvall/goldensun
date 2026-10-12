/* OvlFunc_971_2008e10 -- 0x02008e10, asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_c.s
 *
 * NON-MATCHING, 29 of 109 stream encodings, ref 86 / ours 85 16-BIT
 *   INSTRUCTIONS, ref 264 bytes against ours 262.  aligned-equal 99 of 109
 *   (90.8 per cent), 16 differing/ins/del in 6 hunks.  Relocations: EVERY
 *   SYMBOL IDENTICAL and in the same order (checked, not assumed); only the
 *   offsets move -- __Debug_LoadPresetParty at ref 0xa8 against ours 0xb2,
 *   because the two message arms are laid out in the opposite order, and the
 *   six relocations after it by the 2-byte gap.
 *   PINS 0 (tools/shimcount.py).  Device-free.  No split.
 *   *** THE GAP IS ONE 16-BIT `b` AND IT IS BLOCK LAYOUT, NOT A STATEMENT. ***
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7fb4a8/2008e10.c \
 *     asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_c.s --func OvlFunc_971_2008e10
 *   and, because the figure is phase:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/ovl_7fb4a8/2008e10.c \
 *     asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_c.s OvlFunc_971_2008e10 -v
 *
 * ========= BATCH 344, BRIEF A: THE BLOCKER NAMED IN THE PROSE PARK IS
 * ========= SOLVED.  THE BODY IS REPLACED AND THE GAP IS NOW ONE `b`.
 *
 * GAP ARITHMETIC, PER REGION (the number the next batch wants):
 *   pre-loop   idx 0-28   ref 29 / ours 29 instructions   EXACT
 *   in-loop    idx 29-68  ref 40 / ours 40                EXACT
 *   post-loop  idx 69-101 ref 33 / ours 32                -1  <- the whole gap
 *   pool       7 words both sides, SAME ORDER (gState, 0x989, gKeyRepeat,
 *              gKeyPress, 0x98a, 0x98b, 0x98c).  The old body had 0x98a and
 *              0x98b transposed.
 *
 * WHAT THE OLD BODY'S "BLOCKER" ACTUALLY WAS, and the park had it backwards.
 * It wrote: "the ROM merges them one instruction further back, sharing
 * `bl __MessageID` with the id already in r0 ... Writing the shared id as an
 * explicit local to force that merge makes it WORSE (35) ... I have no lever
 * for it."  The merge is a CROSS-JUMP in the FINAL jump pass
 * (toplev.c:3510-3515 calls jump_optimize with JUMP_CROSS_JUMP), and what
 * blocked it was not the id and not the block order:
 *
 *   `flow.c:645-665` (find_basic_blocks_1, case CODE_LABEL) -- and again at
 *   `:698-713` for case BARRIER -- appends a `(use (const_int 0))` NOP to any
 *   basic block that ENDS IN A CALL_INSN and is closed by a label:
 *       if (GET_CODE (end) == CALL_INSN && ! SIBLING_CALL_P (end))
 *         { rtx nop = gen_rtx_USE (VOIDmode, const0_rtx);
 *           end = emit_insn_after (nop, end); }
 *   The old body's `shared:` label sat immediately after `__MessageID(0x98b);`,
 *   so that block ended in a call and got the nop.  `find_cross_jump`
 *   (`jump.c:1444-1470`) walks back from the label skipping ONLY NOTEs and
 *   CODE_LABELs, so i2 was the USE insn, i1 was our `bl __MessageID`,
 *   `GET_CODE (i1) != GET_CODE (i2)` and it broke at `jump.c:1469-1470` with
 *   minimum still 1.  ONE STRAY NOP, EMITTING NOTHING, COST THE MERGE.
 *   Confirmed in the dumps: `scratch_elev/b344/A/dumps/t1_base/
 *   2008e10.c.25.jump2` holds `(insn 284 220 221 (use (const_int 0)) -1)`
 *   between call_insn 220 and code_label 221 ("shared"), and all three of this
 *   function's `(use (const_int 0))` insns sit exactly before a label whose
 *   preceding block ends in a call.
 *
 * THE FIX IS TO STOP SHARING THE TAIL IN SOURCE AND LET CROSS-JUMPING DO IT.
 * Writing `__ActorMessage(9, 0);` in ALL THREE arms (so each of the first two
 * ends in `goto tail`, not in a call followed by a label) removes the nop from
 * both arms; jump2 then matches four insns backwards from the two `b tail`s --
 * `bl __MessageID / movs r0,#9 / movs r1,#0 / bl __ActorMessage` -- collapses
 * the 0x98a arm to `ldr r0,=0x98a / b <shared>` and leaves msg_c's copy
 * duplicated, which is what the ROM has.  Worth 2 bytes, 110 -> 109 stream
 * encodings, and it is what fixes the pool order.
 *
 * WHAT IS LEFT IS ONE `b`, AND IT IS A BLOCK-ORDER FACT:
 *   ref   a4 blt b4 / a6 adds r0,r5 / a8 bl LoadPresetParty / ac b b8
 *         ae [break block] / b4 ldr r0,=0x98a / b6 b be / b8 cmp r0,#0 ...
 *   ours  a4 bge b0 / a6 ldr r0,=0x98a / a8 b bc
 *         aa [break block] / b0 adds r0,r5 / b2 bl LoadPresetParty
 *         b6 cmp r0,#0 ...
 * Identical instruction multiset except the ROM's `b b8`.  In the ROM the
 * LoadPresetParty CALL and its own `cmp r0,#0` are in DIFFERENT blocks with the
 * `sel = -1; break` block and the 0x98a arm between them, so the call block
 * needs a jump to reach its test; in ours the call and the test are adjacent
 * and the 0x98a arm is on the fallthrough.
 *
 * WHY THE OBVIOUS SOURCE FOR THAT DOES NOT REACH IT.  gcc moves the 0x98a arm
 * to the fallthrough during the FIRST jump pass, before any of this.  Expand
 * emits `bge L189 / b L204(msg_a) / barrier / L189: LoadPreset ...`
 * (dumps/t1_v1/t1_v1.c.00.rtl) and `.02.jump` has `bge L189` with msg_a's whole
 * block between it and L189 -- i.e. the block moved, not the condition.  THREE
 * SPELLINGS THAT SHOULD HAVE SPLIT THE CALL'S BLOCK ARE BYTE-IDENTICAL TO EACH
 * OTHER AND TO THE PLAIN FORM (29 encodings, first differing index 33):
 *   t1_v1  `if (sel < 0) goto msg_a;` with three duplicated arms  <- THIS FILE
 *   t1_v3  `ok = __Debug_LoadPresetParty(sel); goto check;` with msg_a between
 *          the assignment and `check:`   (the temp and the goto both vanish)
 *   t1_v4  the same with the test written as an `if` block
 * AND THE OTHER OUTER SHAPE IS WORSE FOR A DIFFERENT REASON:
 *   t1_v2  `if (sel >= 0) { ... goto msg_b; goto msg_c; }` then msg_a
 *          DOES produce the ROM's `blt` to msg_a with LoadPresetParty on the
 *          fallthrough -- but gcc then pulls msg_b's block to the test's
 *          fallthrough, so 0x98b's `ldr` precedes 0x98a's and THE POOL ORDER
 *          INVERTS (29 encodings, pool 0x98b/0x98a/0x98c).  The ROM needs
 *          `blt`-to-msg_a AND msg_a's `ldr` first, and no spelling tried has
 *          both.
 * FLAG AXIS ON THIS BODY (a_flags.sh, all eight groups): -fno-gcse,
 *   -fno-strict-aliasing, -fno-strength-reduce, -fno-rerun-cse-after-loop,
 *   -fno-rerun-loop-opt all INERT at 16/6; -fno-schedule-insns2 22/12;
 *   -ffixed-r7 29/15 and 2 LONG; -O1 46/24 and 12 LONG.
 *
 * NEXT, ONE QUESTION: name the first-jump-pass transform that moves a
 * singly-referenced `goto` target's block to a conditional jump's fallthrough
 * (it is NOT `jump.c:434-476`, whose precondition `prev_active_insn
 * (reallabelprev) == insn` IS satisfied here and whose effect would be
 * `invert_jump` to `blt msg_a`, which is not what `.02.jump` shows; and it is
 * not bb-reorder, which is dead -- `flag_reorder_blocks` is 0 at toplev.c:443
 * and set by no -O level, used only at toplev.c:3496).  Whatever moves it is
 * the lever for the last `b`.
 *
 * KEPT FROM THE PROSE PARK, ALL STILL TRUE AND ALL STILL LOAD-BEARING:
 *   `gKeyRepeat` must be volatile (the two masks are read with no call between
 *   them and CSE into one load otherwise); the gState base needs a named pointer
 *   or the +0x1f4 folds into the pool constant; `sel = 0` is written BEFORE
 *   `last = -1`; and the epilogue `pop {r1} / bx r1` says the function returns a
 *   value, so it is `int` and ends `return __CutsceneEnd();`.
 *   ALSO KEPT: a two-instruction block reached by `goto` is duplicated inline by
 *   gcc unless it sits adjacent to the branch, which is why the five goto
 *   arrangements lost to the plain `for (;;) { ... if (c) break; ... }`.
 *   WITHDRAWN: "the ROM merges them one instruction further back ... I have no
 *   lever for it" -- the lever was flow.c's nop, above.
 */
extern unsigned char gState[];
extern volatile unsigned int gKeyRepeat;
extern volatile unsigned int gKeyPress;

extern void __CutsceneStart(void);
extern int __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __Func_809280c(int a, int b, int c);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern int __CreateUIBox(int a, int b, int c, int d, int e);
extern void __Func_8016478(int box);
extern void __Func_801e9a0(int a, int b, int c, int d, int e);
extern void __CloseUIBox(int box, int n);
extern int __Debug_LoadPresetParty(int n);

int OvlFunc_971_2008e10(int a)
{
    unsigned char *g;
    int box;
    int sel;
    int last;

    __CutsceneStart();
    g = gState;
    __Func_809280c(a, *(int *)(g + (0xfa << 1)), 0);
    __MessageID(0x989);
    __ActorMessage(a, 0);
    box = __CreateUIBox(0, 0, 6, 4, 2);
    sel = 0;
    last = -1;
    for (;;) {
        if (sel != last) {
            __Func_8016478(box);
            __Func_801e9a0(sel, 3, box, 0, 0);
            last = sel;
        }
        if (gKeyRepeat & 0x20)
            sel -= 1;
        if (gKeyRepeat & 0x10)
            sel += 1;
        if (sel < 0)
            sel = 0;
        if (gKeyPress & 1)
            break;
        if (gKeyPress & 2) {
            sel = -1;
            break;
        }
        __WaitFrames(1);
    }
    __CloseUIBox(box, 1);
    if (sel < 0)
        goto msg_a;
    if (__Debug_LoadPresetParty(sel))
        goto msg_b;
    goto msg_c;
msg_a:
    __MessageID(0x98a);
    __ActorMessage(9, 0);
    goto tail;
msg_b:
    __MessageID(0x98b);
    __ActorMessage(9, 0);
    goto tail;
msg_c:
    __MessageID(0x98c);
    __ActorMessage(9, 0);
tail:
    __WaitFrames(0xa);
    return __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_971_2008e10 -- 0x02008e10,
 * asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c.s
 *
 * 114 lines against the ROM's 115, 29 differing, and the first 79 are exact --
 * the whole selection loop, the key handling and the early exit.  Candidate at
 * scratch/N8e10_best.c.
 *
 * THE CORRECTION THIS FUNCTION CARRIES, and it is aimed at batch 148's own
 * advice: "explicit gotos are how you control block order" HAS A BOUNDARY, and
 * I walked past it here.  Five goto arrangements were screened before the plain
 * `for (;;) { ... if (c) break; ... }` was tried at all, and the plain form beat
 * every one of them:
 *
 *      goto form, break block adjacent to the loop        41 differing
 *      goto form + out-of-line message blocks            40
 *      goto form, break block at the ROM's position      79  (gcc DUPLICATES it)
 *      goto form, message id in a shared local           35
 *      plain for(;;) with two breaks                     29   <- best
 *
 * The reason the goto forms lose is worth keeping: a two-instruction block
 * reached by `goto` is DUPLICATED INLINE by gcc unless it happens to sit
 * adjacent to the branch.  The ROM has that block out of line at the end and
 * branches to it, which is exactly what an ordinary `break` produces and what a
 * `goto` to a distant label does not.  Reach for gotos when the ROM's block
 * ORDER cannot be expressed with ordinary control flow -- not before trying it.
 *
 * ALSO SOLVED: `gKeyRepeat` must be volatile (the two masks are read with no
 * call between them and CSE into one load otherwise); the gState base needs a
 * named pointer or the +0x1f4 folds into the pool constant; `sel = 0` is
 * written BEFORE `last = -1` (the reverse order swaps two instructions); and
 * the epilogue `pop {r1} / bx r1` says the function returns a value, so it is
 * `int` and ends `return __CutsceneEnd();`.
 *
 * BLOCKER: which of the two message arms goes out of line.
 *      rom   blt L8 / mov r0, r5 / bl __Debug_LoadPresetParty / b L9
 *      ours  bge L8 / ldr r0, =0x98a / bl __MessageID / b L9
 * The ROM keeps the LoadPresetParty path on the FALLTHROUGH and puts the 0x98a
 * load out of line; we do the reverse, and because of that our two __MessageID
 * calls never cross-jump -- the ROM merges them one instruction further back,
 * sharing `bl __MessageID` with the id already in r0.  Writing the shared id as
 * an explicit local to force that merge makes it WORSE (35), because gcc then
 * spends an instruction moving it into r0.
 *
 * This is the same block-placement question as the `goto` rows above, one level
 * down, and I have no lever for it: `if (sel < 0) goto msg_a;` with the
 * LoadPresetParty code immediately after is already the arrangement that should
 * give the ROM's branch, and gcc lays the arms out the other way regardless.
 *)

*/
#endif
