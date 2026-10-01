/* OvlFunc_882_200b1ac -- NON-MATCHING, 907 of 1071.
 *
 * tools/objcmp.py at PRODUCTION flags: 907 encodings differ of the reference's
 * 1071.  NOT saturated -- ours is 1070 encodings against the reference's 1071,
 * so the figure is a real distance.  Separated axes (never summed):
 *     INSTRUCTIONS  ours 1028  ref 1026   (+2)
 *     POOL WORDS    ours   41  ref   45   (-4)
 *     size          ours 2712  ref 2716   (-4)
 *     aligncmp      704 of 1071 aligned (65.7%), 208 hunks
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77dd1c/200b1ac.c \
 *     asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_c_a_c.s --func OvlFunc_882_200b1ac
 *
 * SPLIT SHAPE: NONE NEEDED.  python3 tools/datacheck.py on the reference is
 * silent -- ONE function in the file, no data section, no crossing .L symbol.
 * It converts WHOLE.  python3 tools/shimcount.py reports no shims.
 * One linker row: overlays/rom_77dd1c/overlay.ld:61.
 *
 * FRAME, READ WITH THE THREE GREPS:
 *   `sub sp, #imm`     -> `sub sp, #4`  (line 16), `add sp, #4` (1044).
 *   `mov rX, sp` + `add rX, #K` -> NONE.  No stack aggregate.
 *   `add rX, sp` + a load       -> NONE.  So the 4 bytes are ONE SPILL SLOT,
 *     not argument staging: `str r3, [sp]` at 405 and `ldr r3, [sp]` at 616
 *     spill and reload a SINGLE POINTER, `a13 + 0x55`, across ~210
 *     instructions and ~60 calls.  Our candidate reproduces the one slot.
 *
 * ================================================================
 * THE CONSTANT-SET CHECK IS EXACT BUT FOR ONE VALUE, AND THAT SETTLES
 * CORRECTNESS OF ALL 1026 INSTRUCTIONS BEFORE ANY HUNK IS READ
 * ================================================================
 * band-800plus.md section 5 says to diff the DISTINCT pooled values first.
 * Done here, it is the whole triage:
 *
 *     IN REF NOT OURS:  0x0          (one pooled zero -- see below)
 *     IN OURS NOT REF:  (empty)
 *
 * All 40 of the reference's distinct pooled values -- 31 numeric 16.16
 * fixed-point quantities, 8 `gScript_882__*` tables and 3 task entry points --
 * are present in ours and nothing is present in ours and absent there.  Every
 * constant, actor slot, script pointer and call in the reconstruction is
 * therefore right, and the entire residue is ALLOCATION.
 *
 * POOLED-CONSTANT MULTISET of the reference (56 pool LOADS, 40 distinct):
 *   =0x101 x5  =0x9999 x4  =0x13333 x4  =0x10003 x3  =0x7fff x2
 *   =0x4f90000 x2  OvlFunc_882_200be18 x2  OvlFunc_882_200bce4 x2
 *   and x1 each: 0xcccc 0xe666 0x7ae 0x382/0x395/0x42e/0x43c/0x43e/0x4ac/
 *   0x4cd/0x4e6/0x4f6/0x505/0x50c/0x51f/0x521/0x52d/0x535/0x539/0x58b/
 *   0x594 <<16, OvlFunc_882_200c5b8, gScript_882__0200cd6c/ce04/ce30/ce5c/
 *   ce88/ceb4/cec8/cedc, plus TWO EXPLICIT POOL WORDS written as
 *   `ldr rX, .Lnnnn` rather than `ldr rX, =K`: `.word 0` at 203 and
 *   `.word 2` at 912.
 *
 *   WHY THOSE TWO ARE SPELLED THAT WAY: NO REASON.  I first wrote here that GAS
 *   resolves `ldr rX, =K` for an 8-bit K into `mov rX, #K`, and that the
 *   explicit label form is therefore the only way to WRITE a pooled 0 or 2.
 *   THAT IS FALSE AND I MEASURED IT.  Assembling
 *       ldr r0, =8 / ldr r1, =0xbe / ldr r2, =0x101 / ldr r3, =0x30
 *   with this tree's `arm-none-eabi-as -mthumb-interwork -mcpu=arm7tdmi` gives
 *   FOUR `ldr rN, [pc, #off]` and four `.word`s -- GAS collapses nothing.  The
 *   two spellings are a transcription convention of whoever disassembled each
 *   file, and BOTH are genuine pool loads.  Anyone reading a pooled small
 *   constant off an `ldr rX, =K` line is reading a real pool entry.
 *
 *   SO THE BRIEF'S RULE APPLIES HERE AND COMES OUT NEGATIVE ON ITS OWN TERMS.
 *   `*thumb_movsi_insn` decides pool-versus-`mov` on the VALUE, so gcc can
 *   never pool 0 or 2 by that route, and the rule says to read a RELOCATION
 *   there.  But no symbol reachable from this overlay has value 0 or 2 -- the
 *   `.lcomm`/`.data` objects all have nonzero addresses -- so there is no
 *   candidate symbol to name, and the rule's conclusion is unavailable rather
 *   than merely unproven.  What is left is reload placing a SPILLED constant
 *   pseudo's `reg_equiv_constant` through `force_const_mem`, which puts the
 *   value in the literal pool and loads it back.  That explanation is the one
 *   my measurements support: no source spelling reached the pooled zero (see
 *   the three Z variants below), and the one thing that moved the whole
 *   rematerialisation picture was register PRESSURE.  So the pooled 0 and 2
 *   are pressure artefacts, not relocations, and this is a COUNTER-EXAMPLE to
 *   reading the rule as a biconditional: a pooled 8-bit-movable word means
 *   "not `*thumb_movsi_insn`", which admits reload as well as relocation.
 *
 * ================================================================
 * THE RESIDUE IS SIX VALUES AND A DEFICIT OF ELEVEN POOL LOADS
 * ================================================================
 * The hunk census says the same thing twice: inside the 208 hunks the
 * reference has 52 `ldr rX, <pool>` that we do not, and we have 112
 * `mov rX, rX` against its 83.  Per value, how many times each is LOADED:
 *
 *     value        ref loads   ours   deficit
 *     0x101            5         1       4      (five __MapActor_Emote sites)
 *     0x13333          4         1       3      (four __MapActor_SetSpeed)
 *     0x7fff           2         1       1      (two __Func_8091200)
 *     0x4f90000        2         1       1      (SetPos + __Func_80933f8)
 *     0x2              1         0       1      (one `strh` of 2)
 *     0x0              1         0       1      (the `strb` through [sp])
 *
 * THE MECHANISM IS NOT cse1; IT IS WHETHER cse1'S COMMONED PSEUDO WINS A
 * REGISTER.  band-800plus section 2 established that cse1 commons repeated
 * multi-instruction constants across call boundaries and that no flag reaches
 * it.  Both hold here.  What this function adds is the SECOND half: the
 * reference commons these values too, and then LOSES them at allocation --
 * the commoned pseudo gets no hard register, reload finds its
 * `reg_equiv_constant` and REMATERIALISES the pool load at every use.  Our
 * compile commons them and the pseudo KEEPS a register, so each later use is
 * a `mov rlo, rhigh` copy instead.  The reference's seven call-saved
 * registers are fully subscribed -- r7 a13, r6 a1b/a, r11 p1b/f, r10 p13 then
 * 0x20000 then 0xfe, r9 0 then 0xb000, r8 0 then 0x80000000 then 0xe000, r5
 * the script pointer then 0x80000000 then 0 then -13 -- which is why its
 * constants spill and ours do not.
 *
 * ================================================================
 * MEASURED LEVERS.  Every figure below is tools/objcmp.py at production flags
 * unless the row says otherwise.  Baseline for the ladder is v1, the plain
 * transcription.
 * ================================================================
 *
 *   candidate                                 insns  pool  size  objcmp  aligned
 *   v1  plain transcription                   1020    51   +28    980    67.1%
 *   v2  + lever 1 merges + adjacent carriers  1028    41    -4    907    65.7%  <-- THIS FILE
 *   v3  v2 + constants named & hoisted        1023    45  EXACT   984    66.4%
 *   v4  v2 + REG_N_SETS one-set naming        1028    41    -4    907    65.7%
 *   Z   v2 + named zero locals                1030    40    -4    966    66.1%
 *
 * LEVER 1 (REUSE TO INHERIT A REGISTER) PAID, AND IT PAID AT BAND ENTRY --
 * WHICH CONTRADICTS band-800plus.md SECTION 3.  That section says "Reuse is an
 * endgame lever; it is not a band-entry lever", measured on 2009f3c where
 * merging two constant ranges cost +4 size.  Here, merging the two POINTER
 * ranges -- the actor handle used for slot 0x1b and then for slots
 * 0xa/0x18/0x19/0x1b, and its `[+0x50]` field pointer likewise -- is what took
 * size from +28 to -4 and objcmp from 980 to 907 in one edit, together with
 * the carriers below.  The distinction that matters is WHAT is merged:
 * merging two CONSTANT ranges removes a quantity from the set the count is
 * measuring, but merging two POINTER ranges RAISES the merged variable's
 * reference count, which is what buys it a callee-saved register and drops a
 * spill slot.  Our frame went `sub sp, #0x14` (five slots) -> `sub sp, #0xc`
 * (three) on the merges alone, against the reference's one.
 *
 * THE POOLED-HALFWORD CARRIER PAID, AND v3 FOUND ITS PRECONDITION, WHICH IS
 * NEW.  band-800plus section 3 gives the int carrier for a HImode constant
 * store (`h = 0xf0 << 8; *(short *)p = h;`) and section 3 also advises naming
 * constants and "assigning them ALL AT THE TOP".  THOSE TWO PIECES OF ADVICE
 * ARE IN DIRECT CONFLICT and this function shows which wins:
 *     carrier assigned ADJACENT to its store   -> `mov/lsl`, no pool entry
 *     same carrier HOISTED to one set, 5 uses  -> 0xffffe000 BACK IN THE POOL,
 *                                                 loaded 5 times
 * Hoisting lets constant propagation push the value back down to each store
 * site as a bare HImode constant, and `*thumb_movhi_insn` has no immediate
 * form, so the pool returns.  v3 re-pooled 0xe000 five times and 0xb000 twice
 * this way, which is the whole of its +4 pool-word regression.  THE CARRIER
 * MUST BE ADJACENT.  Its assignment and its store are one unit.
 *
 * MEASURED INERT -- each of these is a BOUND, not a non-result:
 *
 *   * THE `REG_N_SETS` GATE IS INERT HERE, AND BYTE-IDENTICALLY SO.  Naming
 *     0x101, 0x13333, 0x7fff and 0x4f90000 as one-set `int` locals and passing
 *     the local at all 13 use sites (v4) produces a .s BYTE-IDENTICAL to the
 *     bare literals (v2) apart from the `.file` directive.  So the gate does
 *     not decide this; a one-set local earns the REG_EQUIV note and still wins
 *     the same register the literal's commoned pseudo won.  The gate's
 *     documented direction (one set -> rematerialise) is exactly what this
 *     function needs and it does NOT deliver it.
 *   * NAMED ZERO LOCALS DO NOT CREATE THE POOLED ZERO.  Three shapes -- both
 *     zeros named, only the `strb` zero named, only the three-store zero named
 *     -- all compile to the SAME object, and none puts a zero in the pool.
 *     Constant propagation collapses them.  The pooled zero is reload placing
 *     a spilled constant pseudo's equivalent via `force_const_mem`; it is a
 *     consequence of pressure, and no source spelling reached it.
 *
 * ================================================================
 * THE `-fno-rerun-cse-after-loop` QUESTION, SETTLED -- AND THE BRIEF'S
 * PREMISE FOR CALLING IT OPEN IS WRONG
 * ================================================================
 * This function HAS a loop: `08.loop` reports "Loop from 1170 to 1193: 7 real
 * insns", the reference's `.L3662` at 476-482.  So the "it was only ever tried
 * on loopless code" escape hatch does not apply.
 *
 * RESULT: `-fno-rerun-cse-after-loop` is BYTE-IDENTICAL to the default.  Not
 * approximately -- `cmp` on the two .s files returns equal.
 *
 * And it is NOT inert because the flag did nothing.  With `-da`, the flag
 * removes exactly these two lines from `09.cse2`:
 *     ;; Processing block from 2 to 1192, 675 sets.
 *     ;; Processing block from 2570 to 0, 783 sets.
 * i.e. the pass is GENUINELY SKIPPED (the dump file is still written, which is
 * why a dump-presence check cannot detect this -- `09.cse2` exists either
 * way).  The only other content difference anywhere in the 20 dumps is that
 * six insns lose their cached INSN_CODE (`5 {*thumb_addsi3}`,
 * `173 {*thumb_movsi_insn}`, `180 {*thumb_movhi_insn}`, `112 {*thumb_ashlsi3}`,
 * `113 {*thumb_ashrsi3}`, `203 {cbranchsi4}`) because `cse_main` is what calls
 * `recog` on them.  THOSE SIX INSNS ARE THE LOOP.  cse2's entire reach on this
 * function is the 7-insn loop body, and it transforms nothing there because
 * the body is a call plus a reload of a halfword that the call may change.
 *
 * So: the pass runs, the flag disables it, and the object does not move.  This
 * is a BOUND on the flag for this shape -- a call-bound counting loop -- and
 * NOT a general claim.  docs/elevation.md's own section
 * "`-fno-rerun-cse-after-loop` is not free, and LOOPS are where it costs"
 * already shows five functions where it is actively WORSE, and the section
 * "`-fno-rerun-cse-after-loop` is not a loop phenomenon" (measured on
 * `OvlFunc_942_20086c8`, a straight-line cutscene with no loop, where it was
 * the only one of six CSE-family flags that reached the function) already
 * states that the name describes WHEN the pass runs, not what it acts on.
 * THE BRIEF'S PREMISE -- that the flag "controls a pass over loops, so an
 * inert result on a loopless function is evidence of nothing" -- is
 * contradicted by that tracked section.  Park notes citing the flag should
 * stop doing so on shape alone; they should cite a measurement.
 *
 * NO MAKEFILE ROW IS IMPLIED.  No CSE_CFLAGS row, no GCSE_CFLAGS row.
 *
 * FULL FLAG SWEEP on this file, all tools/flagcmp.py, labelled, none in the
 * claim line:
 *     -fno-rerun-cse-after-loop      BYTE-IDENTICAL to default
 *     -fno-gcse                      BYTE-IDENTICAL
 *     -fno-gcse -fno-rerun-cse-after-loop   BYTE-IDENTICAL
 *     -fno-cse-follow-jumps          BYTE-IDENTICAL
 *     -fno-cse-skip-blocks           BYTE-IDENTICAL
 *     -fno-strict-aliasing           BYTE-IDENTICAL
 *     -fno-expensive-optimizations   WORSE  (size +4, count +2, 960)
 *     -fno-schedule-insns2           MUCH WORSE (59.0% aligned against 65.7%)
 * The last row independently re-confirms that `sched2` DOES run in this build.
 * No CSE-family flag reaches this function at all, which is the strongest form
 * of band-800plus section 2's "no flag reaches it".
 *
 * `-ffixed-r8 -ffixed-r9 -ffixed-r10 -ffixed-r11` gives size EXACT and count
 * EXACT (1071 of 1071) at 66.8% aligned, the best figures in the batch.  IT IS
 * NOT A ROUTE AND MUST NOT BE CITED AS ONE.  This REPLICATES on a second
 * function the masking signature that band-800plus section 7 retracted on
 * 2008c28: denying the high bank forces reload to rematerialise the commoned
 * constants from their REG_EQUIV notes, which cancels an excess of commoning
 * instead of preventing it -- the very rematerialisation this function's
 * residue is made of.  Two functions now show it, so the retraction stands and
 * is stronger, and no FIXEDHIGH_CFLAGS row should be written.
 * `-ffixed-r5`, `-ffixed-r5 -ffixed-r6` and `-ffixed-r5 -ffixed-r6 -ffixed-r7`
 * are all progressively WORSE (988 / 1005 / 1071), confirming the pointers
 * genuinely want the low call-saved bank, as in the reference.
 *
 * ================================================================
 * TWO METHOD NOTES THAT COST REAL TIME HERE
 * ================================================================
 * 1. gcc-2.96's `-da` DUMPS ARE NOT BYTE-COMPARABLE ACROSS RUNS.  Every dump
 *    prints a raw heap pointer inside `NOTE_INSN_BLOCK_BEG`/`_END`
 *    (`(note 6 3 10 0x7785793f9b40 NOTE_INSN_BLOCK_BEG -1347440721)`), and
 *    from `18.greg` onward `NOTE_INSN_DELETED` prints an uninitialised int as
 *    well.  So a dump diff has a FOUR-LINE floor of pure noise, and the
 *    206-line diffs this function shows from `18.greg` on are ENTIRELY that
 *    noise -- nothing downstream of local-alloc actually changed.  Any
 *    dump-diffing method must filter those two note forms first or it will
 *    report a flag as transformative when it changed six INSN_CODE fields.
 * 2. DO NOT READ A MID-PIPELINE DUMP AS IF IT WERE THE OUTPUT.  `09.cse2`
 *    shows the loop's halfword test as `movhi` + `ashlsi3 16` + `ashrsi3 16`,
 *    which reads exactly like the documented "`(signed char)*p` folds to
 *    `ldrsb`, `(signed char)p[0]` does not" defect, and cost a four-variant
 *    spelling sweep of the loop condition.  `combine` merges the three into
 *    `extendhisi2` afterwards and ALL FOUR spellings already emit the
 *    reference's `mov r2, #0 / ldrsh r3, [r5, r2]`.  There was never a defect.
 *
 * ================================================================
 * CONFIRMING docs/ANALYSIS_OvlFunc_882_200b1ac.c, AND THE ONE PLACE IT IS WRONG
 * ================================================================
 * CONFIRMED: "take it last" (its memory traffic is the work); one function, no
 * split, the single linker row; the `_umodsi3_RAM` against `__umodsi3`
 * relocation difference IS present and is NOT a defect -- nine of them, the
 * alias at overlays/rom_77dd1c/overlay.ld:103, reported rather than filtered;
 * `% N` written inline rather than through a temp, per its warning from
 * 2008434; and its central claim that the reused quantities are ADDRESSES not
 * constants, which is why naming constants (v3, v4) did nothing and merging
 * pointers (v2) did everything.
 *
 * WRONG IN ONE PLACE: it counts "FOUR branches to a label" and "ONE loop, ZERO
 * if/else", reading `b .L33c4` (199), `b .L382c` (635) and `b .L3adc` (908) as
 * "SKIPS" over pool directives.  They are not skips in the sense of dead
 * control flow -- they are unconditional fallthroughs around mid-function
 * literal pools, and each one IS A BASIC BLOCK BOUNDARY for cse1, whose window
 * is the basic block.  That matters: it is why the reference builds `~0xc` as
 * `mov r1,#0xd / neg r1,r1` at line 385 (block B2) and as `sub r5, #0xd` at
 * line 675 (block B5, where `reload_cse_move2add` knew r5 held 0 and chained
 * off it).  The function has SIX basic blocks, not one straight line, and the
 * block structure is load-bearing for every commoning question above.
 * `reload_cse_move2add` doing this is already documented at elevation.md's
 * "`reload_cse_move2add`: the SPACING between two address assignments" and the
 * "derived by `add` is not by itself symbol evidence" section; this is a third
 * instance and it needs no source change.
 *
 * ================================================================
 * NEXT
 * ================================================================
 * The whole remaining distance is eleven pool loads across six values, and the
 * question is one thing: what makes cse1's commoned pseudo LOSE its register.
 * Flags are exhausted -- all six CSE-family switches are byte-identical.
 * Source-level naming is exhausted in both directions (v3 hoisted, v4 one-set,
 * Z zero locals). What has NOT been tried is raising POINTER pressure further:
 * the reference keeps FOUR pointers in registers (r7/r6/r11/r10) and we keep
 * three with `p13` spilled.  Getting `p13` off the stack and into r10 should
 * saturate the bank and is the one edit with a mechanism behind it.  Follow
 * `0x13333` through `18.greg` -- it is a single quantity, which is the only
 * condition under which band-800plus section 6 says dump reading is tractable.
 */
extern unsigned char gScript_882__0200cd6c[];
extern unsigned char gScript_882__0200ce04[];
extern unsigned char gScript_882__0200ce30[];
extern unsigned char gScript_882__0200ce5c[];
extern unsigned char gScript_882__0200ce88[];
extern unsigned char gScript_882__0200ceb4[];
extern unsigned char gScript_882__0200cec8[];
extern unsigned char gScript_882__0200cedc[];

extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int n);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern unsigned int __Random(void);
extern void __StartTask(void (*f)(void), int n);
extern void __StopTask(void (*f)(void));
extern void __Func_800fe9c(void);
extern void __Func_80118a8(int a);
extern void __Func_80118c0(int a);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_8091890(int a);
extern void __Func_809202c(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern unsigned char *__Func_8093554(void);
extern void __Func_8095214(void);
extern void __Func_8095240(void);
extern void __Func_8095268(void);
extern void OvlFunc_882_2008134(void);
extern void OvlFunc_882_200bc48(void);
extern void OvlFunc_882_200bce4(void);
extern void OvlFunc_882_200be18(void);
extern void OvlFunc_882_200c0f0(void);
extern void OvlFunc_882_200c5b8(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_882_200b1ac(void)
{
    unsigned char *a13;
    unsigned char *a1b;
    unsigned char *p13;
    unsigned char *p1b;
    unsigned char *p;
    unsigned char *q;
    unsigned char *s;
    int h;
    int n;

    a13 = __MapActor_GetActor(0x13);
    a1b = __MapActor_GetActor(0x1b);
    p1b = *(unsigned char **)(a1b + 0x50);
    p13 = *(unsigned char **)(a13 + 0x50);
    __Func_80933d4(0x80 << 9, 0x80 << 6);
    __Func_80933f8(0xdc << 15, -1, 0x58b0000, 1);
    __MapActor_SetSpeed(8, 0x13333, 0x9999);
    __MapActor_SetSpeed(0x1a, 0x13333, 0x9999);
    __MapActor_SetSpeed(0, 0x13333, 0x9999);
    __MapActor_SetSpeed(0x16, 0x13333, 0x9999);
    s = gScript_882__0200cd6c;
    __MapActor_SetBehavior(8, s);
    __CutsceneWait(0xa);
    __MapActor_SetBehavior(0x1a, s);
    __Func_8095240();
    __CutsceneWait(0xa);
    __MapActor_SetBehavior(0, s);
    __CutsceneWait(0xa);
    __Func_8095214();
    __MapActor_SetBehavior(0x16, s);
    __CutsceneWait(0x80);
    OvlFunc_882_2008134();
    __Func_80933f8(0xae << 16, -1, 0x5940000, 1);
    __CutsceneWait(0x68);
    __Func_80933f8(0x99 << 16, -1, 0x52d0000, 1);
    __Func_80921c4(9, 0x9e, 0x9f << 3);
    __Func_8092adc(9, 0x80 << 6, 0);
    __MapActor_WaitScript(8);
    __MapActor_SetBehavior(8, gScript_882__0200ce04);
    __MapActor_SetBehavior(0x1a, gScript_882__0200ce30);
    __MapActor_SetBehavior(0, gScript_882__0200ce5c);
    __MapActor_RunScript(0x16, gScript_882__0200ce88);
    __Func_8012330(0x80 << 11, 0x80 << 11, 0x80 << 9);
    __PlaySound(0x91);
    __CutsceneWait(0x14);
    __Func_8012330(0x80 << 9, 0x80 << 9, 0x80 << 9);
    __CutsceneWait(0x3c);
    __MapActor_Emote(0, 0x101, 0);
    __MapActor_Emote(0x1a, 0x101, 0);
    __MapActor_Emote(0x16, 0x101, 0);
    __MapActor_Emote(8, 0x101, 0);
    __MapActor_Emote(9, 0x101, 0x3c);
    __Func_8092848(0x1a, 8, 0);
    __Func_8092848(0x16, 0, 0);
    __CutsceneWait(0x14);
    p = __MapActor_GetActor(0);
    *(short *)(p + 0x64) = __Random() % 0x14 + 0x14;
    p = __MapActor_GetActor(0x16);
    *(short *)(p + 0x64) = __Random() % 0x14 + 0x14;
    p = __MapActor_GetActor(0x1a);
    *(short *)(p + 0x64) = __Random() % 0x14 + 0x14;
    p = __MapActor_GetActor(8);
    *(short *)(p + 0x64) = __Random() % 0x14 + 0x14;
    p = __MapActor_GetActor(9);
    *(short *)(p + 0x64) = __Random() % 0x14 + 0x14;
    s = gScript_882__0200ceb4;
    __MapActor_SetBehavior(9, s);
    __CutsceneWait(0x1e);
    __MapActor_SetBehavior(0, s);
    __MapActor_SetBehavior(0x1a, s);
    __MapActor_SetBehavior(0x16, s);
    __MapActor_SetBehavior(8, s);
    __CutsceneWait(0xa);
    __PlaySound(0x11);
    __Func_8012330(0xc0 << 10, 0xc0 << 10, 0x80 << 9);
    __PlaySound(0x91);
    __CutsceneWait(0x1e);
    __Func_8012330(0x80 << 9, 0x80 << 9, 0x80 << 9);
    __CutsceneWait(0x78);
    __Func_8012330(0xc0 << 10, 0xc0 << 10, 0x80 << 9);
    __PlaySound(0x91);
    __CutsceneWait(0x28);
    __Func_8012330(0x80 << 10, 0x80 << 10, 0x80 << 9);
    __CutsceneWait(0x3c);
    __Func_8012330(0x80 << 11, 0x80 << 11, 0x80 << 9);
    __PlaySound(0x91);
    __CutsceneWait(0x14);
    __Func_8012330(0x80 << 9, 0x80 << 9, 0x80 << 9);
    __CutsceneWait(0x3c);
    __Func_8012330(0xc0 << 10, 0xc0 << 10, 0x80 << 9);
    __PlaySound(0x91);
    __CutsceneWait(0x28);
    __Func_8012330(0x80 << 9, 0x80 << 9, 0x80 << 9);
    __CutsceneWait(0x3c);
    __Func_8095214();
    __Func_8012330(0x80 << 9, 0x80 << 9, 0x80 << 9);
    __CutsceneWait(1);
    __Func_8012330(-1, -1, 0xe666);
    __Func_80933d4(0x80 << 12, 0x80 << 12);
    __Func_80933f8(0xd9 << 16, -1, 0x43c0000, 1);
    __Func_8091200(0, 0);
    __Func_8091254(0x28);
    __WaitFrames(0x28);
    __Func_8092950(0x13, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x13), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1b), 0);
    *(int *)(a1b + 0x18) = 0xcccc;
    *(int *)(a1b + 0x1c) = 0xcccc;
    a1b[0x23] &= 0xfe;
    p1b[9] = (p1b[9] & ~0xc) | 4;
    *(int *)(a13 + 8) = 0xc8 << 16;
    *(int *)(a13 + 0xc) = 0xc8 << 16;
    *(int *)(a13 + 0x38) = 0xc8 << 16;
    *(int *)(a13 + 0x3c) = 0xc8 << 16;
    *(int *)(a13 + 0x10) = 0x3820000;
    *(int *)(a13 + 0x40) = 0x3820000;
    q = a13 + 0x55;
    *q = 0;
    a13[0x23] &= 0xfe;
    p13[9] &= ~0xc;
    *(int *)(__Func_8093554() + 0x38) = 0x80 << 24;
    *(int *)(__Func_8093554() + 0x3c) = 0x80 << 24;
    *(int *)(__Func_8093554() + 0x40) = 0x80 << 24;
    *(int *)(__Func_8093554() + 0x24) = 0;
    *(int *)(__Func_8093554() + 0x28) = 0;
    *(int *)(__Func_8093554() + 0x2c) = 0;
    __WaitFrames(1);
    __Func_80933f8(0xf7 << 16, 0x80 << 16, 0x3950000, 0);
    __Func_800fe9c();
    __WaitFrames(1);
    __Func_8091200(0x10003, 1);
    __Func_8091200(0x80 << 9, 2);
    __Func_8091254(0x1e);
    __WaitFrames(0x1e);
    __StartTask(OvlFunc_882_200bce4, 0xc8 << 4);
    __MapActor_SetBehavior(0x13, gScript_882__0200cedc);
    __Func_80933d4(0x80 << 10, 0x7ae);
    __Func_80933f8(0xaf << 16, 0xc0 << 15, 0x43e0000, 1);
    do {
        __WaitFrames(1);
    } while (*(short *)(a13 + 0x66) != 8);
    __Func_8091200(0, 0);
    __Func_8091254(0x3c);
    __WaitFrames(0x3c);
    __Func_8012350();
    *(int *)(__Func_8093554() + 0x38) = 0x80 << 24;
    *(int *)(__Func_8093554() + 0x3c) = 0x80 << 24;
    *(int *)(__Func_8093554() + 0x40) = 0x80 << 24;
    *(int *)(__Func_8093554() + 0x24) = 0;
    *(int *)(__Func_8093554() + 0x28) = 0;
    *(int *)(__Func_8093554() + 0x2c) = 0;
    __StopTask(OvlFunc_882_200bce4);
    __MapActor_SetIdle(0x13);
    __WaitFrames(1);
    __MapActor_SetAnim(0x13, 0);
    *(int *)(a1b + 0x18) = 0xa0 << 9;
    *(int *)(a1b + 0x1c) = 0xa0 << 9;
    p1b[0x23] = 2;
    *(int *)(p1b + 0x18) = 0xa0 << 9;
    *(int *)(a13 + 0x18) = 0x80 << 10;
    *(int *)(a13 + 0x1c) = 0x80 << 10;
    *(int *)(a13 + 8) = 0;
    *(int *)(a13 + 0x10) = 0;
    *(int *)(a13 + 0x38) = 0;
    *(int *)(a13 + 0x40) = 0;
    __WaitFrames(1);
    __MapActor_SetAnim(0x17, 8);
    __MapActor_SetPos(9, 0xa9 << 16, 0x9e << 19);
    __Func_8092adc(9, 0xc0 << 8, 0);
    __MapActor_SetAnim(9, 9);
    __MapActor_SetPos(0x1a, 0x97 << 16, 0x50c0000);
    __Func_8092adc(0x1a, 0x80 << 8, 0);
    __MapActor_SetAnim(0x1a, 5);
    __MapActor_SetPos(8, 0xaa << 16, 0x5210000);
    __Func_8092adc(8, 0xc0 << 7, 0);
    __MapActor_SetAnim(8, 5);
    __MapActor_SetPos(0, 0xb9 << 16, 0x5350000);
    __Func_8092adc(0, 0x80 << 6, 0);
    __MapActor_SetAnim(0, 0x11);
    __MapActor_SetPos(0x16, 0xa9 << 16, 0xad << 19);
    __Func_8092adc(0x16, 0x80 << 7, 0);
    __MapActor_SetAnim(0x16, 0);
    __Func_80933f8(0xa6 << 16, 0, 0x5390000, 0);
    __Func_800fe9c();
    *q = 0;
    *(int *)(a13 + 0x38) = 0x80 << 24;
    *(int *)(a13 + 0x3c) = 0x80 << 24;
    *(int *)(a13 + 0x40) = 0x80 << 24;
    OvlFunc_882_200bc48();
    __MapActor_SetPos(0x1b, 0xda << 16, 0x93 << 19);
    __Func_80933f8(0xd2 << 16, 0, 0x4ac0000, 0);
    __Func_800fe9c();
    *(int *)(a1b + 0x18) = 0x80 << 10;
    *(int *)(a1b + 0x1c) = 0x80 << 10;
    __StartTask(OvlFunc_882_200be18, 0xc8 << 4);
    __MapActor_SetIdle(0xa);
    __MapActor_SetIdle(0x18);
    __MapActor_SetIdle(0x19);
    __WaitFrames(1);
    a1b = __MapActor_GetActor(0xa);
    p1b = *(unsigned char **)(a1b + 0x50);
    a1b[0x23] &= 0xfe;
    *(int *)(a1b + 0x18) = 0x80 << 9;
    *(int *)(a1b + 0x1c) = 0x80 << 9;
    h = 0xd0 << 8;
    *(short *)(a1b + 6) = h;
    p1b[9] &= ~0xc;
    __MapActor_SetAnim(0xa, 0);
    a1b = __MapActor_GetActor(0x18);
    p1b = *(unsigned char **)(a1b + 0x50);
    a1b[0x23] &= 0xfe;
    *(int *)(a1b + 0x18) = 0x80 << 9;
    *(int *)(a1b + 0x1c) = 0x80 << 9;
    p1b[9] &= ~0xc;
    h = 0xb0 << 8;
    *(short *)(a1b + 6) = h;
    __MapActor_SetAnim(0x18, 5);
    a1b = __MapActor_GetActor(0x19);
    p1b = *(unsigned char **)(a1b + 0x50);
    a1b[0x23] &= 0xfe;
    *(int *)(a1b + 0x18) = 0x80 << 9;
    *(int *)(a1b + 0x1c) = 0x80 << 9;
    h = 0xb0 << 8;
    *(short *)(a1b + 6) = h;
    p1b[9] &= ~0xc;
    __MapActor_SetAnim(0x19, 5);
    a1b = __MapActor_GetActor(0x1b);
    p1b = *(unsigned char **)(a1b + 0x50);
    OvlFunc_882_200bc48();
    *(int *)(a13 + 0xc) = 0xc0 << 14;
    *(int *)(a13 + 8) = 0xd6 << 16;
    *(int *)(a13 + 0x10) = 0x98 << 19;
    *(int *)(a13 + 0x38) = 0x80 << 24;
    *(int *)(a13 + 0x3c) = 0x80 << 24;
    *(int *)(a13 + 0x40) = 0x80 << 24;
    p1b[9] = (p1b[9] & ~0xc) | 4;
    __MapActor_SetPos(0x1b, 0xd6 << 16, 0x98 << 19);
    __Func_8092adc(0x18, 0xc0 << 8, 0);
    __Func_8092adc(0x19, 0xc0 << 8, 0x14);
    __SetFlag(0xb3 << 1);
    __Func_80118c0(0);
    __Func_80118c0(1);
    __Func_80118c0(2);
    __Func_80118c0(3);
    __Func_80118c0(4);
    __Func_80118c0(5);
    __Func_8091200(0x10003, 1);
    __Func_8091200(0x80 << 9, 2);
    __Func_8091254(0x78);
    __WaitFrames(0xa0);
    __Func_8091200(0x7fff, 1);
    __Func_8091200(0x7fff, 2);
    __Func_8091254(0x50);
    __CutsceneWait(0x50);
    __CutsceneWait(0x64);
    __StopTask(OvlFunc_882_200be18);
    *(int *)(p1b + 0x18) = *(int *)(a1b + 0x18);
    __ClearFlag(0xb3 << 1);
    __Func_80118a8(0);
    __Func_80118a8(1);
    __Func_80118a8(2);
    __Func_80118a8(3);
    __Func_80118a8(4);
    __Func_80118a8(5);
    OvlFunc_882_200c0f0();
    __MapActor_SetPos(9, 0xa5 << 16, 0x4cd0000);
    __MapActor_SetAnim(9, 1);
    p = __MapActor_GetActor(9);
    h = 0xe0 << 8;
    *(short *)(p + 6) = h;
    s = gScript_882__0200cec8;
    *(short *)(p + 0x64) = __Random() % 0x5a + 0x3c;
    n = 1;
    *(short *)(p + 0x66) = n;
    __MapActor_SetBehavior(9, s);
    __MapActor_SetPos(0x1a, 0xa5 << 16, 0x4e60000);
    __MapActor_SetAnim(0x1a, 1);
    p = __MapActor_GetActor(0x1a);
    h = 0xe0 << 8;
    *(short *)(p + 6) = h;
    *(short *)(p + 0x64) = __Random() % 0x5a + 0x3c;
    *(short *)(p + 0x66) = 2;
    __MapActor_SetBehavior(0x1a, s);
    __MapActor_SetPos(0x16, 0x98 << 16, 0x5050000);
    __MapActor_SetAnim(0x16, 1);
    p = __MapActor_GetActor(0x16);
    h = 0xe0 << 8;
    *(short *)(p + 6) = h;
    *(short *)(p + 0x64) = __Random() % 0x5a + 0x3c;
    n = 3;
    *(short *)(p + 0x66) = n;
    __MapActor_SetBehavior(0x16, s);
    __MapActor_SetPos(8, 0xb4 << 16, 0x51f0000);
    p = __MapActor_GetActor(8);
    h = 0xe0 << 8;
    *(short *)(p + 6) = h;
    *(short *)(p + 0x64) = __Random() % 0x5a + 0x3c;
    n = 4;
    *(short *)(p + 0x66) = n;
    __MapActor_SetBehavior(8, s);
    __MapActor_SetAnim(8, 6);
    __MapActor_GetActor(0x16)[0x23] &= 0xfe;
    __MapActor_GetActor(8)[0x23] &= 0xfe;
    __StartTask(OvlFunc_882_200c5b8, 0xc8 << 4);
    __MapActor_SetPos(0, 0xb5 << 16, 0x4f90000);
    h = 0xe0 << 8;
    *(short *)(__MapActor_GetActor(0) + 6) = h;
    __MapActor_SetAnim(0, 1);
    __Func_80933f8(0xb5 << 16, 0, 0x4f90000, 0);
    __Func_800fe9c();
    __MapActor_SetPos(0xa, 0, 0);
    __MapActor_SetPos(0x13, 0, 0);
    __MapActor_SetPos(0x18, 0, 0);
    __MapActor_SetPos(0x19, 0, 0);
    __MapActor_SetPos(0x17, 0, 0);
    __MapActor_SetPos(0x1b, 0, 0);
    __MapActor_SetPos(0x11, 0x90 << 16, 0x42e0000);
    __MapActor_SetPos(0x12, 0x8a << 17, 0x4f60000);
    __WaitFrames(0x3c);
    __Func_8091200(0x10003, 1);
    __Func_8091200(0x80 << 9, 2);
    __Func_8091254(0x50);
    __CutsceneWait(0x3c);
    __Func_809202c();
    __CutsceneWait(0x3c);
    __Func_8091890(1);
    __Func_8095268();
}
