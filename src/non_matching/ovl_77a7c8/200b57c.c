/* OvlFunc_881_200b57c -- NON-MATCHING, 2 differing STREAM encodings of 94.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/Nb57c_best.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 94 / ours 94 stream encodings; 61/61 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_77a7c8/200b57c.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_a_c.s --func OvlFunc_881_200b57c
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 2 of 94, instruction counts EQUAL, byte size and pool
 *   count equal too (this park is in phase on all three length dimensions, so
 *   its figure IS a real distance).
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: all 29 statement boundaries at every brace depth
 *   measured, all inert or worse.  No variant was capped and none failed to
 *   compile.
 *
 *   SO DO NOT RE-SCREEN THESE.  Every brief so far has spent part of a round on
 *   exactly that.  The residue here is NOT a placement or flag question; it is a
 *   register choice, a tie, or a pass-level effect, and it needs a hypothesis.
 *
 *   Read as "inert against THIS body".  A recorded negative expires silently
 *   when the body changes -- six batches running, the decisive lever has turned
 *   up inside a park's own negative list.
 */
extern char *iwram_3001ebc;
extern unsigned char gScript_881__0200d218[];

struct Actor {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    unsigned char pad20[0x64 - 0x20];
    short f64;
};

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Func_80936a0(int a, int b);
extern void __SetCameraTarget(int a, int b);
extern void __Func_8092950(int a, int b);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, void *s);
extern int __StartTask(void *f, int n);
extern void __Func_8091200(int a, int b);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8091e9c(int n);
extern void OvlFunc_881_200b4a0(void);

void OvlFunc_881_200b57c(void)
{
    struct Actor *e;
    int off;

    e = __MapActor_GetActor(8);
    __CutsceneWait(0x3c);
    __CutsceneStart();
    __Func_80936a0(0x9999, 1);
    e->f18 = 0x13333;
    e->f1c = 0x13333;
    __SetCameraTarget(8, 1);
    __WaitFrames(1);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
    __MapActor_SetSpeed(8, 0x6666, 0x3333);
    e->f64 = 0;
    __MapActor_SetBehavior(8, gScript_881__0200d218);
    __StartTask(OvlFunc_881_200b4a0, 0xc8 << 4);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    __Func_8091200(0x10003, 1);
    off = 0xe4 << 1;
    *(int *)(iwram_3001ebc + off) = 0x20;
    __MapTransitionIn();
    __CutsceneWait(0x78);
    __Func_80936a0(0x16666, 0x96 << 1);
    __CutsceneWait(0x87 << 1);
    *(int *)(iwram_3001ebc + off) = 0x10;
    *(short *)(0xa0 << 19) = 0x7fff;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x6f);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_881_200b57c -- 0x0200b57c,
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_a_c.s (SINGLE-function file)
 *
 * 84 of 84 lines, TWO differing.  Candidate at scratch/Nb57c_best.c.
 *
 * SOLVED, AND THIS IS THE PART WORTH CARRYING: A TYPED HALFWORD FIELD FIXES A
 * STORE, AND THE LITERAL POOL WITH IT.
 *
 * Written with a cast through a `char *`, `*(short *)(e + 0x64) = 0` compiles to
 * `ldr r3, =0x0` -- gcc puts a literal ZERO in the constant pool -- where the ROM
 * has `mov r3, #0`.  That extra pool entry is also what forces gcc to dump the
 * pool in the MIDDLE of the function, emitting a `b`/label pair to jump over it,
 * which the ROM does not have.  Two problems, one cause, and together they were
 * 31 differing across 86 lines against the ROM's 84.
 *
 * Declaring the object with the `struct Actor` already in this overlay
 * (src/overlays/rom_77a7c8/ovl_30_a_a_a_c_b.c) and writing `e->f64 = 0;` fixes
 * BOTH: `mov r3, #0`, the pool goes back to the end of the function, and the
 * count drops 31 -> 2 with the line count exact.  The two int stores at 0x18 and
 * 0x1c were already right either way, so it is specifically the HALFWORD that
 * needs the type.
 *
 * This extends the batch-145 finding, which was about a halfword READ that sched2
 * treated differently once it had an alias set.  It applies to STORES too, and
 * the visible symptom here is not scheduling at all -- it is a pooled constant
 * and a pool dump point.  Reach for the struct whenever a halfword access sits
 * near an unexplained `ldr rN, =<small>` or an unexplained mid-function pool.
 *
 * BLOCKER: argument-setup order at __MapActor_SetSpeed, and only there.
 *      rom   ldr r2, =0x3333 / mov r0, #0x8  / ldr r1, =0x6666
 *      ours  ldr r2, =0x3333 / ldr r1, =0x6666 / mov r0, #0x8
 * `mov r0` wants to sit BETWEEN the two pool loads.  That is the "r0 into the
 * middle" half of the batch-147 pair, whose lever is naming the constant in a
 * block that DOMINATES the call -- and this function is straight-line, so there
 * is no such block.  Naming both speed constants at the top costs seven lines
 * (91 vs 84); naming only the y constant costs one and 59 differing.
 *
 * SCREENED AND UNCHANGED AT 2, so nobody repeats them: the slot 8 as a named
 * local; `0x6666` written as `0x3333 << 1`; unsigned parameter types; an
 * `__asm__` alias for the callee returning int, `char *`, or `long long`;
 * deleting the callee's declaration entirely; and the per-declaration greedy
 * sweep over every other callee (tools/protolever.py).
 *
 * FLAGS, all screened: -fno-schedule-insns (2, unchanged), -fno-strength-reduce
 * (2), -fno-strict-aliasing (2), -ffixed-r7 (2), -fno-rerun-cse-after-loop (2),
 * -fno-gcse (2).  -fno-schedule-insns2 and -O1 both go to 26 differing, so
 * sched2 is producing the ROM's order everywhere else in the function and this
 * one site is not a reason to turn it off.
 *)

*/
#endif
