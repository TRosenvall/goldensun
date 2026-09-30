/* OvlFunc_881_2008c28 -- 0x02008c28   (overlay 881, rom_77a7c8)
 *
 * NON-MATCHING, 819 of 888 encodings differ.
 *   (objcmp AT PRODUCTION FLAGS -- plain -O2, no Makefile row for this file.)
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so the 819 is SATURATED and CANNOT RANK:
 *   size  ref 2356 bytes, ours 2384  (+28)
 *   count ref 888 encodings, ours 902  (+14)
 * Rank this function with aligncmp instead:
 *   aligned-equal 681 (76.7% of ref), 304 differing/ins/del in 182 hunks.
 * shimcount: clean, exit 0 -- PIN-FREE.  No `register asm`, no barrier, no
 * fakematch row needed.
 * datacheck on the reference: SILENT.  No data section, no label needs .global.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77a7c8/2008c28.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_a_c.s \
 *     --func OvlFunc_881_2008c28
 *
 * SPLIT SHAPE: NONE NEEDED.  ovl_30_c_a_c_c_a_c_a_a_a_a_a_a_c.s holds this
 * ONE function and nothing else (`grep -c thumb_func_start` = 1), so landing
 * is a WHOLE-FILE conversion to
 *   src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_a_c.c
 * with NO tools/split_s.py run and NO `.global` requirements at all.  The
 * fourteen gScript_881__* symbols and OvlFunc_881_20097a4 are already
 * `.global` in asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_c.s, so the externs
 * below resolve without touching any other file.
 *
 * ----------------------------------------------------------------------------
 * THE PROGRAM IS RIGHT.  THE BLOCKER IS cse.c PASS 1 COMMONING CONSTANTS
 * ACROSS CALLS, AND IT IS A BAND EFFECT, NOT A SPELLING ERROR.
 * ----------------------------------------------------------------------------
 *
 * CHEAPEST CONFIRMATION FIRST, and at 834 instructions it is the only early
 * signal worth having: the RELOCATION SYMBOL SEQUENCE matches the reference
 * essentially entry for entry (only literal-pool DUMP POSITIONS differ), and
 * the set of DISTINCT pooled values is IDENTICAL -- all 30 numeric literals
 * and all 15 symbols, ours against the reference's, no value in one and not
 * the other.  So every constant, every actor slot, every script pointer and
 * every call in this reconstruction is correct.  What differs is entirely
 * WHERE gcc keeps those constants.
 *
 * (1) WHAT THE REFERENCE DOES.  It RELOADS each repeated constant from the
 *     pool at every use site: `ldr rX, =0x6666` appears 7 times, `=0x19999`
 *     6, `=0x2008` 7, `=0x16d80000` 5, `=0xcccc` 5.  No constant is ever held
 *     in a callee-saved register.  Its frame is `push {r5, r6, lr}` -- TWO
 *     call-saved registers for TWO long-lived quantities (the actor pointer
 *     from __MapActor_GetActor(0xf), and one register that serves three
 *     disjoint ranges: the short* into that actor, the 0xc0<<11 constant, and
 *     the gScript_881__0200caf4 pointer).
 *
 * (2) WHAT WE DO, AND THE PASS THAT DOES IT.  From the -da dumps:
 *       c28_v1.c.00.rtl  -- FOUR independent (set (reg:SI N) (const_int 26214))
 *                           insns, one per __MapActor_SetSpeed call site.
 *       c28_v1.c.03.cse  -- ONE real set survives, insn 137 (set (reg:SI 56)
 *                           (const_int 26214)) {*thumb_movsi_insn}; the other
 *                           three sites are rewritten to
 *                           (set (reg:SI 2 r2) (reg:SI 56)) carrying an
 *                           (expr_list:REG_EQUAL (const_int 26214)) note.
 *     cse pass 1 commons the constant and reg 56 then LIVES ACROSS FOUR CALLS.
 *     A call does not invalidate cse's constant table, and this function is
 *     straight-line script -- 834 instructions in SIX basic blocks (5 branches
 *     total) -- so cse1's window is effectively the whole function and every
 *     repeated constant becomes one call-crossing pseudo.
 *
 * (3) THE CONSEQUENCE IS THE HIGH REGISTER BANK.  With -fcall-used-r4 only
 *     r5/r6/r7 are call-saved low registers.  The commoned constants take
 *     r5 and r6, the actor pointer is displaced to r7, and the remaining
 *     commoned constants go to r8/r9/sl/fp -- which Thumb-1 cannot use as
 *     operands, so each one costs a `mov rlo, rhigh` copy AND gcc must emit
 *     the six-instruction Thumb high-register save:
 *         push {r5, r6, r7, lr} / mov r7, fp / mov r6, sl / mov r5, r9
 *         push {r5, r6, r7}     / mov r7, r8 / push {r7}
 *     against the reference's single `push {r5, r6, lr}` -- a 12-byte
 *     prologue gap and a matching epilogue gap.  47 high-register mentions in
 *     the generated .s.  THAT is what the 819 is mostly counting.
 *
 * ----------------------------------------------------------------------------
 * WHAT RULES THE ALTERNATIVES OUT -- MEASURED, NOT ASSUMED
 * ----------------------------------------------------------------------------
 *
 * NO FLAG REACHES cse1.  Every one of these was compiled and the high-register
 * prologue survived all of them (mentions of fp/sl/r9/r8 in the generated .s):
 *     -O2 baseline                       47
 *     -fno-rerun-cse-after-loop          43   (and objcmp 819 -> 798 raw,
 *                                              aligned 74.9% -> 74.5%: WORSE)
 *     -fno-gcse                          47
 *     -fno-rerun-cse-after-loop -fno-gcse 47
 *     -fno-cse-follow-jumps              47
 *     -fno-cse-skip-blocks               43
 *     -fno-expensive-optimizations       47
 *     -fno-force-mem                     47
 *     -fno-caller-saves                  47
 *     -fno-omit-frame-pointer            45
 *     -O1                                43
 *     -Os                                47
 * cse pass 1 is unconditional at -O1, -O2 and -Os; the two -fcse-* switches
 * only tune its reach ACROSS blocks, and the commoning here is INTRA-block,
 * which is why both are inert.  So NO CSE_CFLAGS ROW SHOULD BE WRITTEN for
 * this file -- lever 5's precondition is absent and the flag measures worse.
 *
 * NOT sched2, and not an ordering problem at all.  The residue is a SIZE gap
 * of 28 bytes and a COUNT gap of 14 encodings; an ordering pass cannot change
 * either.  (sched1 does not run in this build in any case.)
 *
 * NOT THE REUSE LEVER, and this is worth recording because it is the first
 * thing the lever set says to try.  The reference's r5 does carry three
 * disjoint ranges -- exactly the batch-304 signature -- but reuse CANNOT be
 * the lever here because its FIRST precondition fails: the count is not
 * already exact, and more importantly the registers our candidate needs to
 * win are not held by our declared variables at all.  Our r5/r6 are held by
 * cse's constant pseudos; our four declared long-lived variables are not
 * competing with each other, they are competing with the compiler's own
 * temporaries.  Merging them would free a register that cse would immediately
 * take.  Re-derive this only after (4) below is settled.
 *
 * ----------------------------------------------------------------------------
 * THE ONE LEVER THAT PAID, AND THE ONE THAT WOULD
 * ----------------------------------------------------------------------------
 *
 * LEVER THAT PAID -- THE POOLED ZERO, worth 16 bytes and 5 encodings.
 *   `*p = 0` on a `short *` expands to (set (mem:HI) (const_int 0)) and
 *   *thumb_movhi_insn has no immediate form, so gcc-2.96 put the ZERO IN THE
 *   LITERAL POOL and loaded it back with `ldrh r3, .L12` where the reference
 *   has `mov r3, #0`.  That is batch 306's HImode ours-extra pool defect, and
 *   the escape is an int carrier -- `z = 0; *p = z;` -- which makes the store
 *   source a SImode register instead of a HImode constant.
 *       before: size 2400 count 907 aligned 665 (74.9%)  objcmp raw 831
 *       after:  size 2384 count 902 aligned 681 (76.7%)  objcmp raw 819
 *   The `0x0` pool word is gone from our .word list; the reference has none.
 *   THE BODY BELOW CARRIES THIS FIX.  Note the shape: the `.word 0` in the
 *   pool listing is the tell, and it is visible long before any hunk is.
 *
 * (4) *** STRUCK IN THE SAME BATCH -- READ THE RETRACTION BELOW BEFORE THE
 *     CLAIM. ***  The figures in (4) are real; the INFERENCE drawn from them
 *     was wrong, and OvlFunc_945_2009f3c (batch 307, same band) disproves it.
 *     Taking r8-r11 out of the allocation pool collapses almost the entire
 *     residue:
 *
 *       extra flags                              size        count      aligned
 *       (none, production)                       2384 (+28)  902 (+14)   76.7%
 *       -ffixed-r9 -ffixed-r10 -ffixed-r11       2384 (+28)  899 (+11)   78.8%
 *       -ffixed-r8 -r9 -r10 -r11                 2352 ( -4)  886 ( -2)   80.0%
 *
 *     With the whole high bank fixed, HIGH=0 -- the six-instruction prologue
 *     becomes `push {r5, r6, r7, lr}` and SIZE AND COUNT COME WITHIN 4 BYTES
 *     AND 2 ENCODINGS of the reference, from +28 and +14.  The mechanism is
 *     reload rematerialisation: a commoned constant pseudo that gets NO hard
 *     register is re-emitted from its REG_EQUIV note at each use, which is
 *     EXACTLY the reference's `ldr rX, =const`-at-every-site shape.
 *
 *     THE RETRACTION.  I read that as evidence about the original toolchain's
 *     Thumb register pool -- "maybe r8-r11 were never allocatable".  THAT IS
 *     FALSE.  OvlFunc_945_2009f3c, 807 instructions in the same band, has the
 *     six-instruction high-save prologue IN THE REFERENCE
 *         push {r5,r6,r7,lr} / mov r7,r11 / mov r6,r10 / mov r5,r9
 *         push {r5,r6,r7}    / mov r7,r8  / push {r7}
 *     and 45 high-register mentions in its body, parking constants in r9 and
 *     fp exactly as our candidate here does.  Tree-wide, 467 hand-written .s
 *     files carry that prologue and 210 ALREADY HAVE A LANDED .c SIBLING.  The
 *     high bank IS in gcc's pool and the ROM uses it freely.
 *
 *     SO WHAT -ffixed ACTUALLY DOES HERE IS MASK, NOT FIX.  Denying the high
 *     bank forces reload to rematerialise commoned constants from their
 *     REG_EQUIV notes, which cancels an EXCESS of commoned constants that our
 *     reconstruction has and the original did not.  It treats the symptom.
 *     DO NOT WRITE A FIXEDHIGH_CFLAGS ROW, and do not cite these numbers as
 *     REG_ALLOC_ORDER evidence -- they are not.
 *
 *     THE REAL STATEMENT IS THEREFORE NARROWER AND HARDER: our reconstruction
 *     gives cse1 MORE repeated constants to common, in one basic block, than
 *     the original source did.  The open question is which source shape denies
 *     cse1 the commoning -- see the NEXT STEP note at the bottom, which is
 *     unchanged and is still the right target.
 *
 * ----------------------------------------------------------------------------
 * ONE REGION IS ALREADY EXACT, WHICH IS WHY THE PROGRAM READING IS TRUSTED
 * ----------------------------------------------------------------------------
 * The actor-field store block (reference 0x520..0x52a) comes out instruction
 * for instruction under -ffixed, modulo the one register name:
 *     ref   ldr r3,=0x1999 / str r3,[r6,#0x48] / str r3,[r6,#0x44]
 *           mov r3,#0xc0 / lsl r3,#9 / mov r5,r6 / str r3,[r6,#0x18]
 *           str r3,[r6,#0x1c] / add r5,#0x64 / mov r3,#0 / strh r3,[r5]
 *           mov r3,#0x80 / lsl r3,#15 / ldr r2,[r6,#0x50] / str r3,[r6,#0xc]
 *           mov r3,#0xf0 / lsl r3,#8 / strh r3,[r2,#0x1e]
 *     ours  same sequence with r7 for r6.
 * Two details in it are deliberate and should not be "simplified":
 *   - `v = 0x1999;` then two stores: the reference loads the pool word ONCE
 *     for both, so the value must be a named local with two uses.
 *   - `h = 0xf0 << 8;` then `*(short *)(q + 0x1e) = h;`: naming it is what
 *     keeps 0xf000 out of the pool, the same halfword-store lever the landed
 *     sibling ovl_30_c_a_c_c_a_c_a_a_a_a_a_a_a.c documents.
 *
 * NEXT STEP FOR WHOEVER PICKS THIS UP.  Do not re-sweep spellings; the
 * distinct-constant set already proves the program. Either (a) settle the
 * -ffixed question above, or (b) find a source shape that denies cse1 the
 * commoning in the first place. For (b) the target is insn 137 in 03.cse: any
 * candidate that leaves FOUR (set reg (const_int 26214)) insns in 03.cse has
 * solved it, and that is one grep, not a rebuild.
 *
 * Idioms copied, not re-derived, from landed files in this same overlay:
 * `extern unsigned char gScript_881__*[]`, `__MapActor_GetActor` returning
 * `unsigned char *`, and the `0xc0 << 7` spelling for mov/lsl constants (all
 * from src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_a_a_a_a.c).
 */
extern unsigned char gScript_881__0200c9e4[];
extern unsigned char gScript_881__0200ca78[];
extern unsigned char gScript_881__0200cac4[];
extern unsigned char gScript_881__0200caf4[];
extern unsigned char gScript_881__0200cb50[];
extern unsigned char gScript_881__0200cbe4[];
extern unsigned char gScript_881__0200cc30[];
extern unsigned char gScript_881__0200cc74[];
extern unsigned char gScript_881__0200cd08[];
extern unsigned char gScript_881__0200cd54[];
extern unsigned char gScript_881__0200cd98[];
extern unsigned char gScript_881__0200ce2c[];
extern unsigned char gScript_881__0200ce78[];
extern unsigned char gScript_881__0200cebc[];

extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __CutsceneEnd(void);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __Func_808c44c(void);
extern void __Func_808c4c0(void);
extern void __Func_8091e9c(int n);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_80936a0(int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __StartTask(void (*f)(void), int n);
extern void __StopTask(void (*f)(void));
extern void __WaitFrames(int n);
extern void __WaitMapTransition(void);

extern void OvlFunc_881_200955c(void);
extern void OvlFunc_881_2009680(void);
extern void OvlFunc_881_20097a4(void);

void OvlFunc_881_2008c28(void)
{
    unsigned char *a;
    unsigned char *t;
    unsigned char *q;
    short *p;
    unsigned char *s;
    int v;
    int w;
    int h;
    int c;
    int z;

    a = __MapActor_GetActor(0xf);
    __CutsceneStart();
    __Func_80936a0(0xa0 << 9, 1);
    __WaitFrames(4);
    __MapTransitionIn();
    __WaitMapTransition();
    __Func_808c44c();
    __Func_80933f8(-1, -1, -1, 0);
    __WaitFrames(1);
    __MapActor_SetSpeed(0, 0x19999, 0xcccc);
    __MapActor_SetSpeed(1, 0x19999, 0xcccc);
    __Func_80921c4(0, 0x16fc, 0xc5 << 3);
    __Func_8092adc(0, 0xc0 << 7, 0);
    __MapActor_SetPos(8, 0x16d80000, 0xc5 << 19);
    __WaitFrames(1);
    __Func_8092950(8, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
    __MapActor_SetSpeed(0xa, 0x19999, 0x6666);
    __MapActor_SetSpeed(0xb, 0x19999, 0x6666);
    __MapActor_SetSpeed(0xc, 0x19999, 0x6666);
    __MapActor_SetSpeed(0xd, 0x19999, 0x6666);
    __PlaySound(0x8d);
    __MapActor_SetBehavior(0xa, gScript_881__0200c9e4);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0xb, gScript_881__0200cb50);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0xc, gScript_881__0200cc74);
    __CutsceneWait(0x14);
    __MapActor_RunScript(0xd, gScript_881__0200cd98);
    __PlaySound(0x121);
    t = __MapActor_GetActor(0);
    if (t != 0)
        __MapActor_SetPos(1, *(int *)(t + 8), *(int *)(t + 0x10));
    __Func_80921c4(1, 0x1704, 0xc8 << 3);
    __Func_8092adc(0, 0x80 << 6, 0);
    __Func_8092adc(1, 0xa0 << 8, 0x14);
    __MapActor_Emote(0, 0x101, 0);
    __MapActor_Emote(1, 0x101, 0x3c);
    __MapActor_SetPos(8, 0x16d80000, 0xc7 << 19);
    __WaitFrames(1);
    __MessageID(0x1215);
    __Func_8093040(8, 0, 0xa);
    __Func_8092adc(0, 0xc0 << 7, 0);
    __Func_8092adc(1, 0xc0 << 7, 0x28);
    __Func_8092adc(0, 0x80 << 6, 0);
    __Func_8092adc(1, 0xa0 << 8, 0x3c);
    __Func_8092adc(0, 0xc0 << 7, 0);
    __Func_8092adc(1, 0xc0 << 7, 0xa);
    __PlaySound(0x8d);
    __MapActor_SetBehavior(0xa, gScript_881__0200ca78);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0xb, gScript_881__0200cbe4);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0xc, gScript_881__0200cd08);
    __CutsceneWait(0xa);
    __Func_8092adc(0, 0x80 << 7, 0);
    __Func_8092adc(1, 0x80 << 7, 0xa);
    __MapActor_RunScript(0xd, gScript_881__0200ce2c);
    __PlaySound(0x121);
    __CutsceneWait(0x14);
    __Func_808c4c0();
    __MapActor_SetAnim(0xa, 1);
    __MapActor_SetAnim(0xb, 1);
    __MapActor_SetAnim(0xc, 1);
    __MapActor_SetAnim(0xd, 1);
    __Func_80933f8(0x16080000, -1, 0xdf << 19, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    __Func_808c44c();
    __MapActor_SetPos(9, 0x16080000, 0xdb << 19);
    __WaitFrames(1);
    __MapActor_SetSpeed(9, 0x13333, 0x9999);
    __Func_80921c4(9, 0x1608, 0xd9 << 3);
    __Func_80921c4(9, 0x15f8, 0xd9 << 3);
    __Func_80921c4(9, 0x15f8, 0xdf << 3);
    __CutsceneWait(0x14);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    __MapActor_Surprise(9, 0x81 << 1);
    __CutsceneWait(0x3c);
    __Func_8092adc(9, 0, 0x14);
    __Func_809259c(9, 3);
    __Func_8093040(9, 0, 0x14);
    __MapActor_SetPos(8, 0x16180000, 0xdf << 19);
    __WaitFrames(1);
    __Func_8092950(8, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(8), 1);
    __MapActor_SetSpeed(8, 0xcccc, 0x6666);
    __Func_80921c4(8, 0x1608, 0xdf << 3);
    __CutsceneWait(0x14);
    __Func_809259c(8, 2);
    __Func_8093040(0x2008, 0, 0xa);
    __Func_8092adc(8, 0xc0 << 6, 0x3c);
    __Func_8092adc(8, 0x80 << 8, 0xa);
    __MapActor_Surprise(8, 0x81 << 1);
    __CutsceneWait(0x3c);
    __Func_8092adc(9, 0xc0 << 6, 0);
    __Func_8092adc(8, 0xc0 << 6, 0x28);
    __MapActor_Surprise(8, 0x81 << 1);
    __CutsceneWait(0x3c);
    __Func_809259c(8, 2);
    __Func_8093040(0x2008, 0, 0x28);
    __Func_80925cc(9, 1);
    __Func_8092adc(9, 0, 0xa);
    __Func_8093040(9, 0, 0xa);
    __MapActor_Emote(8, 0x105, 0x3c);
    __Func_8093040(0x2008, 0, 0xa);
    __Func_80925cc(8, 1);
    __MapActor_DoAnim(8, 3);
    __Func_8093040(0x2008, 0, 0xa);
    __MapActor_Emote(9, 0x101, 0x3c);
    __Func_8093040(9, 0, 0x14);
    __MapActor_DoAnim(8, 3);
    __Func_808c4c0();
    __PlaySound(0x6b);
    __Func_80933d4(0x80 << 11, 0x80 << 11);
    OvlFunc_881_200955c();
    __PlaySound(0x121);
    __MapActor_Emote(8, 0x80 << 1, 0);
    __MapActor_Emote(9, 0x80 << 1, 0);
    __Func_8092adc(8, 0x80 << 8, 0);
    __Func_8092adc(9, 0, 0x28);
    __Func_8092adc(8, 0xb0 << 8, 0);
    __Func_8092adc(9, 0xb0 << 8, 0);
    __Func_80933d4(0x80 << 9, 0x80 << 6);
    __Func_80933f8(0x15e80000, -1, 0xd9 << 19, 1);
    __Func_8093530();
    __MapActor_SetPos(0xe, 0x15a80000, 0xd5 << 19);
    __WaitFrames(1);
    __MapActor_SetSpeed(0xe, 0x4ccc, 0x2666);
    __MapActor_SetBehavior(0xe, gScript_881__0200cebc);
    __CutsceneWait(0xa0);
    v = 0x1999;
    *(int *)(a + 0x48) = v;
    *(int *)(a + 0x44) = v;
    w = 0xc0 << 9;
    *(int *)(a + 0x18) = w;
    *(int *)(a + 0x1c) = w;
    p = (short *)(a + 0x64);
    z = 0;
    *p = z;
    *(int *)(a + 0xc) = 0x80 << 15;
    q = *(unsigned char **)(a + 0x50);
    h = 0xf0 << 8;
    *(short *)(q + 0x1e) = h;
    __Actor_SetSpriteFlags(a, 0);
    __Actor_SetAnim(a, 2);
    __WaitFrames(1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xf), 0);
    __StartTask(OvlFunc_881_20097a4, 0xc8 << 4);
    do {
        __WaitFrames(1);
    } while (*p == 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xf), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
    __CutsceneWait(0xa);
    t = __MapActor_GetActor(9);
    c = 0xc0 << 11;
    *(int *)(t + 0x28) = c;
    t = __MapActor_GetActor(8);
    *(int *)(t + 0x28) = c;
    __PlaySound(0x91);
    __Func_80933d4(0x80 << 11, 0x80 << 11);
    OvlFunc_881_2009680();
    OvlFunc_881_2009680();
    __CutsceneWait(0x3c);
    __Func_80933d4(0x80 << 10, 0x80 << 7);
    __Func_80933f8(0x16080000, -1, 0xdf << 19, 1);
    __Func_8093530();
    __Func_808c44c();
    __MapActor_Surprise(9, 0x81 << 1);
    __MapActor_Surprise(8, 0x81 << 1);
    __CutsceneWait(0x3c);
    __StopTask(OvlFunc_881_20097a4);
    __WaitFrames(1);
    __MapActor_SetPos(0xe, 0, 0);
    __MapActor_SetPos(0xf, 0, 0);
    __Func_8092adc(8, 0x80 << 8, 0xa);
    __MapActor_Jump(8, 4, 0x28);
    __Func_8093040(0x2008, 0, 0xa);
    __Func_8092adc(9, 0, 0xa);
    __Func_8093040(9, 0, 0x14);
    __Func_8092adc(8, 0xc0 << 8, 0x28);
    __Func_8093040(0x2008, 0, 0x14);
    __MapActor_Jump(9, 4, 0x14);
    __Func_8093040(9, 0, 0xa);
    __Func_80925cc(8, 1);
    __Func_8092adc(8, 0x80 << 8, 0xa);
    __Func_8093040(0x2008, 0, 0xa);
    __MapActor_Surprise(9, 0x81 << 1);
    __CutsceneWait(0x50);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __Func_80925cc(9, 1);
    __MapActor_DoAnim(9, 3);
    __Func_80921c4(8, 0x1618, 0xdf << 3);
    __MapActor_SetPos(8, 0, 0);
    __Func_80921c4(9, 0x15f8, 0xd9 << 3);
    __Func_80921c4(9, 0x1608, 0xd9 << 3);
    __Func_80921c4(9, 0x1608, 0xdb << 3);
    __MapActor_SetPos(9, 0, 0);
    __PlaySound(0x8d);
    __MapActor_SetBehavior(0xa, gScript_881__0200cac4);
    __MapActor_SetBehavior(0xb, gScript_881__0200cc30);
    __CutsceneWait(0x28);
    __MapActor_SetBehavior(0xc, gScript_881__0200cd54);
    __CutsceneWait(0x28);
    __MapActor_RunScript(0xd, gScript_881__0200ce78);
    __Func_808c4c0();
    __MapActor_SetPos(0, 0x170c0000, 0xc5 << 19);
    __MapActor_SetPos(1, 0x17140000, 0xc8 << 19);
    __Func_80933d4(0x80 << 11, 0x80 << 8);
    __Func_80933f8(0x16d80000, -1, 0xc9 << 19, 1);
    __Func_8093530();
    s = gScript_881__0200caf4;
    __MapActor_SetBehavior(0xa, s);
    __CutsceneWait(0x14);
    __Func_80933d4(0x6666, 0xccc);
    __Func_80933f8(0x16d80000, -1, 0xc1 << 19, 1);
    __MapActor_SetBehavior(0xb, s);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0xc, s);
    __CutsceneWait(0x14);
    __Func_8092adc(0, 0x80 << 8, 0);
    __Func_8092adc(1, 0x80 << 8, 0);
    __MapActor_SetBehavior(0xd, s);
    __CutsceneWait(0x28);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __Func_8092adc(1, 0xc0 << 8, 0);
    __MapActor_WaitScript(0xd);
    __PlaySound(0x121);
    __Func_80933d4(0x80 << 11, 0x80 << 8);
    __Func_80933f8(0x16f80000, -1, 0xc9 << 19, 1);
    __Func_8093530();
    __Func_8092adc(0, 0x80 << 6, 0);
    __Func_8092adc(1, 0xa0 << 8, 0x50);
    __MapActor_SetAnim(0, 3);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(1, 2);
    t = __MapActor_GetActor(0);
    if (t != 0)
        __MapActor_TravelTo(1, *(short *)(t + 0xa), *(short *)(t + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __Func_80933d4(0xcccc, 0x1999);
    __Func_80933f8(0x16d80000, -1, 0xc9 << 19, 1);
    __MapActor_SetSpeed(0, 0xcccc, 0x6666);
    __Func_80921c4(0, 0x16d8, 0xc5 << 3);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x85a);
    __Func_8091e9c(3);
    __CutsceneEnd();
}
