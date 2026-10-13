/* OvlFunc_901_2008640 -- NOT MATCHING
 *
 * NON-MATCHING, **14 differing STREAM encodings of 48** (was 16).  38/38
 * 16-bit instructions, 10/10 32-bit entries, 116/116 BYTES, IN PHASE, so a
 * true DISTANCE.  THIRTEEN of the 14 are instructions; the FOURTEENTH IS THE
 * SAME PHANTOM the park already documented -- the pool word, ours
 * `.word _CONST_2` (00000000 + R_ARM_ABS32) against the reference's
 * `.word 0x2`.  _CONST_2 = 0x2 is an ABSOLUTE symbol at const.sym:53 and
 * stage1.ld INCLUDEs const.sym (Makefile:65-68), so every overlay resolves it
 * through `-R stage1.o` to the literal 2 and `make compare` CAN pass it.
 * objcmp's RELOCDIFF line here is that symbol and nothing else.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/overlays/2008640.c \
 *     asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_a_a.s --func OvlFunc_901_2008640
 *   -> 14 of 48, instructions 38/38, pool 10/10, size 116/116, first at index 9
 *
 * PIN-FREE, device-free (tools/shimcount.py prints no rows).
 * PIECE (tools/piece.py): asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_a_a.s
 * holds TWO functions, OvlFunc_901_200858c (parked, no claim) and this one;
 * both parked, so one candidate could land them together -- and either cuts
 * out alone (tools/split_s.py cuts before/target/after).  NO data export.
 *
 * ======================================================================
 * BATCH 348, BRIEF B: 16 -> 14, AND THE PARK'S TWO BIGGEST RECORDED-INERT
 * BLOCKS WERE BOTH MEASURING AN AXIS IT HAD NOT ACTUALLY SWEPT.
 * ======================================================================
 *
 * The park recorded 220 inert variants, headed by "ALL 24 PERMUTATIONS of the
 * four entry statements (p / saved / zero / two) CROSSED WITH an anchor at each
 * of 8 boundaries".  **THOSE 24 PERMUTATIONS ALL KEPT `*p = two | *p;` LAST.**
 * The OR statement was never permuted, so the one ordering the ROM's own
 * stream asks for -- the OR *before* `saved = a->f6;` -- had never been
 * generated.  Sweeping the 5-statement permutations with the OR in a
 * non-final position (96 variants, this round) gives 15, and `first` moves
 * 9 -> 8.  That is the brief's route 2b, "move a definition LATER".
 *
 * Then the park's SECOND block -- "a SECOND anchor at every one of 17
 * boundaries on top of this body: all 16" -- EXPIRED with the body, exactly as
 * the recorded-negative rule says it must.  On the new 15 body a second anchor
 * at boundary 2 OR 3 reads **14**.  A third anchor at all 19 boundaries is
 * flat at 14, so the anchor axis is now closed at depth 3.
 *
 * TWO ANCHORS IS A PASS-3 ARTIFACT, NOT A PIN.  Both are device-free
 * `do { } while (0);`; the park already shipped one.  Record for
 * reports/pass3-depin.md: this body carries two.
 *
 * ---- THE RESIDUE, 13 INSTRUCTIONS IN FOUR GROUPS, RE-READ THIS ROUND ----
 *
 * idx  9-12 and 15-17, SEVEN encodings.  The entry block.  ref | ours:
 *        movs r2,#6          | movs r2,#0
 *        ldrsh r1,[r5,r2]    | mov  sl,r2
 *        ldr  r3,=2          | ldrh r2,[r6]
 *        ldrh r2,[r6]        | ldr  r3,=2
 *        orrs r3,r2          | orrs r3,r2      <- 13/14 NOW ALIGN (new)
 *        strh r3,[r6]        | strh r3,[r6]
 *        mov  r8,r1          | movs r2,#6
 *        movs r1,#0          | ldrsh r3,[r5,r2]
 *        mov  sl,r1          | mov  r8,r3
 *      THE ROM SPLITS THE LOAD FROM ITS HIGH-REGISTER COPY: `ldrsh r1` is
 *      issued BEFORE the OR block and `mov r8,r1` AFTER it, so the loaded
 *      value lives across the OR in **r1** -- a register the OR block does
 *      not touch.  Our build never splits that pair, in either statement
 *      order, because our reload register for the ldrsh destination is r2
 *      (OR-last body) or r3 (this body) and BOTH are OR-block registers, so
 *      the motion is blocked by an anti-dependence on the reload register.
 *      Same shape as common1_1354's island A: the register assignment, not
 *      the schedule, is what forbids the motion.
 *      NOTE WHICH REGISTER THE ROM USES TWICE: r1 carries `saved` AND then
 *      `zero` (`movs r1,#0 / mov sl,r1`).  Ours uses r2 for both.  One step.
 * idx 29-30, TWO encodings.  `__Func_8093040(0xf, 0, 0xa)`: ref fills
 *      r1, r0, r2; ours r1, r2, r0.  Pure sched2 ordering of three
 *      independent `movs` with equal INSN_PRIORITY -- rank_for_schedule's
 *      last rung is `INSN_LUID (tmp) - INSN_LUID (tmp2)`
 *      (haifa-sched.c:4113-4115), i.e. original RTL order.
 * idx 32-33, TWO encodings.  ref `mov r2,r8 / strh r2,[r5,#6]`, ours r3.
 * idx 40-41, TWO encodings.  ref `mov r3,sl / strh r3,[r6]`, ours r2.
 *      *** THESE TWO GO OPPOSITE WAYS (ref r2 then r3; ours r3 then r2),
 *      WHICH IS THE SIGNATURE OF A ROUND-ROBIN ONE STEP OUT OF STEP, not of a
 *      register preference. ***  reload's search starts at `i = last_spill_reg`
 *      and advances round-robin over spill_regs (reload1.c:5000-5006), and
 *      `last_spill_reg = i` is set in set_reload_reg (reload1.c:4939); it is
 *      reset per function (reload1.c:821).  So the ROM's rotation was one step
 *      further on when it reached the entry insn, and EVERY LATER SCRATCH
 *      INHERITS THE OFFSET.  The park's diagnosis of this is CONFIRMED, with
 *      the extra evidence that the two directions invert.
 *      >> THE QUESTION IS STILL "WHAT GRABS A RELOAD REGISTER BEFORE THE
 *      >> ldrsh IN THE ROM'S BUILD", and it is NOT reachable by any source
 *      >> order (the 96-permutation sweep is now exhaustive over the five
 *      >> entry statements) nor by any anchor (three depths swept flat).
 * idx 36 is the _CONST_2 phantom.
 *
 * ---- MEASURED THIS ROUND (all at 38/38 instructions, 10/10 pool) ----
 *   96 five-statement permutations x anchor position: floor 15, reached by
 *     p-two-zero-OR-saved, p-zero-two-OR-saved and two-p-zero-OR-saved.
 *     Every ordering with the OR at index 2 (both `zero` and `saved` after
 *     it) is 20 -- so "OR earlier" is not monotonic; it wants `zero` BEFORE
 *     and `saved` AFTER.
 *   second anchor, 19 boundaries on the 15 body: 14 at boundaries 2 and 3,
 *     15 elsewhere.  third anchor, 19 boundaries on the 14 body: all 14.
 *   24 declaration permutations on the 14 body: all 14, exactly.
 *   EXACTLY INERT AT 14 (the park's recorded WORSE rows, re-crossed on the new
 *   body, which is where they stop being negatives): `int saved` (was 18),
 *   `unsigned short saved`, `*p |= two`, `*p = *p | two`, the f6 read and the
 *   f6 restore through `*(short *)((char *)a + 6)`, `zero = two & 0;`, a
 *   second local copying `saved` (route 5), naming 0xa as an int, naming the
 *   0xf slot as an int (was 34 on the old body), `0x0` for the literal 0.
 *   WORSE: `cur = *p; *p = two | cur;` 17 (was 18); all-int locals 27 and the
 *   frame grows 4 bytes; `saved = ((short *)p)[-0x2f]` 35 and ONE INSTRUCTION
 *   LONG; `saved` before the OR again 20; reading `*p` into `cur` 17.
 *
 * CARRIED FORWARD, STILL TRUE: this function has the same `b .L / <pool> /
 * .L:` pool-skip shape as the parked OvlFunc_898_2008acc and HERE GCC PLACES
 * IT CORRECTLY, between `mov r0,#1` and `bl __WaitFrames`.  The pool dump
 * point is not systematically early in this family.
 */
struct A {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x64 - 8];
    unsigned short f64;
};

extern int _CONST_2;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_901_2008640(void)
{
    struct A *a;
    unsigned short *p;
    unsigned short two;
    unsigned short zero;
    short saved;

    a = __MapActor_GetActor(0xf);
    p = &a->f64;
    do { } while (0);
    two = (unsigned short)(int)&_CONST_2;
    zero = 0;
    do { } while (0);
    *p = two | *p;
    saved = a->f6;
    __CutsceneStart();
    __MessageID(0x1cb4);
    __MapActor_SetAnim(0xf, 0);
    __Func_8092848(0xf, 0, 2);
    __Func_8093040(0xf, 0, 0xa);
    a->f6 = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    *p = zero;
}
