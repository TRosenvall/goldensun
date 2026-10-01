/* OvlFunc_917_2008488 (0x02008488) -- NON-MATCHING, 1055 of 1164 differ (objcmp,
 * production flags, SATURATED -- see below).  1,122 instructions.  NEVER ATTEMPTED
 * BEFORE BATCH 310: a census bug matched park FILENAMES against function names by
 * address suffix, and because every overlay loads at the same base address, the park
 * src/non_matching/ovl_7a4370/2008488.c (OvlFunc_890_2008488) marked this one parked.
 *
 * COUNT IS EXACT.  ref 1164 encodings, ours 1164.  SIZE ref 3048 bytes, ours 3044 --
 * FOUR BYTES SHORT, and the shortfall is pool geometry, not the instruction stream:
 * 319 `bl` on both sides, 334 relocations on both sides, and both carry three pools.
 * aligncmp: aligned-equal 946 (81.3% of ref), 316 differing/ins/del in 213 hunks.
 * objcmp's 1055 is SATURATED -- the streams diverge at index 0 on the prologue, so
 * every later index counts as differing.  Rank this on size-and-count, not on 1055.
 * shimcount: 0 shims -- NO PIN, NO FAKEMATCH ROW NEEDED.
 *
 * Verify with (this file, where it sits today):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b310g/PARK_OvlFunc_917_2008488.c \
 *     asm/overlays/rom_7a4370/ovl_30_c_c_c_a_c_c.s --func OvlFunc_917_2008488
 * Intended install path is src/non_matching/ovl_7a4370/2008488.c, beside the already
 * parked sibling 2009070.c; substitute that path for the scratch one after the move.
 *
 * SPLIT SHAPE.  asm/overlays/rom_7a4370/ovl_30_c_c_c_a_c_c.s holds TWO functions, this
 * one FIRST, so the split is TWO-WAY, not three.  `tools/split_s.py --dry-run` (always
 * --dry-run; it DELETES a tracked .s) reports:
 *     ovl_30_c_c_c_a_c_c_b.s  1 function, 1156 lines   <- this function
 *     ovl_30_c_c_c_a_c_c_c.s  1 function,  159 lines   <- OvlFunc_917_2009070
 *   plus a rewrite of overlays/rom_7a4370/overlay.ld and removal of the original.
 * tools/datacheck.py reports NO data section, so no text/data split is involved.  The
 * sibling park 2009070.c records that the SECOND function carries an inline pool a
 * split must preserve; this one does not constrain the split.
 *
 * ===== THE PROGRAM IS PROVEN RIGHT BY TWO WHOLE-FUNCTION CHECKS =====
 *
 * 1. THE DISTINCT-CONSTANT SET IS IDENTICAL.  All 29 entries -- every numeric literal
 *    (0x8008 0x4009 0x8002 0x4008 0x105 0x103 0x101 0x845 0x7fff 0x6666 0x33333
 *    0x406218 the four message bases 0x14ed/0x14ee/0x14fb/0x1501/0x1519 and the
 *    80933f8 coordinate words) and every symbol (iwram_3001ebc, .L1dc0, .L1dcc,
 *    .L1dd4, the three gScript_917 bases, ActorCmd_ARRAY_917__02009ab8,
 *    OvlFunc_917_20092b4, OvlFunc_917_20095a0) -- appears on both sides and nothing
 *    appears on one side only.  One `grep | sort | uniq -c` per side.  This is the
 *    band-800plus section 5 check and it is still the cheapest signal there is.
 *
 * 2. THE RELOCATION SEQUENCE MATCHES 334 FOR 334, IN ORDER, with ONE offset: the four
 *    R_ARM_ABS32 pool words (iwram_3001ebc, .L1dcc, .L1dc0, OvlFunc_917_20095a0) sit
 *    BEFORE the OvlFunc_917_20092f4 / __Func_8092c40 pair in the ROM and AFTER it in
 *    ours.  That is WHERE THE POOL IS CUT, not a different call order -- the 319 `bl`
 *    entries are in identical order on both sides.  No `_call_via_rN` veneer on either
 *    side (0 and 0), so the closeness is not a veneer artefact.
 *
 * ===== WHICH POPULATION: THE STRAIGHT-LINE MECHANISM DOMINATES, DESPITE 16 BRANCHES ==
 *
 * The brief asked which of the two band-800plus populations dominated.  ANSWER: the
 * STRAIGHT-LINE one, and the branch count was misleading about it.  Of the 17 `.L`
 * labels in the reference, THREE (.L926-adjacent, .Lad8-adjacent, .Leec) are reached by
 * an UNCONDITIONAL `b` that exists only to jump over a `.pool_aligned` -- gcc's own
 * pool placement, not control flow the source wrote.  The real source structure is six
 * `if`s, one `while` spin loop and one counted loop in 1,122 instructions, so the
 * basic blocks still run 60-250 instructions and section 2's commoning applies in full.
 * ORDINARY PER-REGION STRUCTURE CONTRIBUTED NOTHING MEASURABLE HERE (see the inert
 * rows below).  TRIAGE LESSON: count branch TARGETS that are not pool jumps, because
 * `grep -c` on branch mnemonics over-counts this function by three.
 *
 * ===== THE WHOLE RESIDUE IS cse1 CONSTANT COMMONING, AND IT IS A REGISTER-SET DEFECT ==
 *
 * The reference's prologue is `push {r5, r6, lr}` and the reference contains ZERO
 * mentions of r7, r8, r9, sl or fp.  TWO call-saved quantities, total:
 *
 *     r6  = __GetFlag(3), live the whole function, three uses
 *     r5  = FIVE DISJOINT RANGES -- &.L1dd4 (store + two spin-loop loads), the counted
 *           loop's counter, &iwram_3001ebc (two dereferences across a branch),
 *           OvlFunc_917_20095a0 as a task handle (TWICE, two separate ranges), and
 *           gScript_917__02009b6c (three uses)
 *
 * Ours pushes {r5, r6, r7, lr} AND the six-instruction Thumb high-save prologue, with
 * r5=68 r6=31 r7=29 r8=5 r9=12 sl=5 fp=10 mentions against the reference's r5=22 r6=6.
 * The extra quantities are COMMONED CONSTANTS, traced exactly as band-800plus section 6
 * prescribes -- go in with a named quantity, do not look around.  Following 32776
 * (0x8008, used 18 times) through the `-da` dumps:
 *
 *     00.rtl 18  01.sibling 18  02.jump 18  03.cse 26  07.gcse 40  08.loop 29
 *     09.cse2 26 ... 18.greg 25  23.sched2 25  26.mach 24
 *
 * `00.rtl` holds EIGHTEEN independent `(set (reg:SI N) (const_int 32776))`, one per call
 * site, each already a pseudo (expand does not put a pool constant straight into r0).
 * `03.cse` leaves a few real `{*thumb_movsi_insn}` sets and rewrites the rest as copies
 * carrying `(expr_list:REG_EQUAL (const_int 32776))`.  The ROM reloads `ldr r0, =0x8008`
 * all 18 times.  THIS FUNCTION IS A STRONGER WITNESS FOR SECTION 2 THAN 2008c28 WAS,
 * because its reference has NO high-register use at all -- so the excess cannot be
 * explained away as a shape the ROM also has.
 *
 * ===== TWENTY FLAG SETTINGS MEASURED, ALL INERT ON THE PROLOGUE =====
 *
 * Screened on the final candidate; every row below leaves `push {r5, r6, r7, lr}` and
 * 31-38 high-register mentions.  flagcmp-class screening only -- NOT a claim line.
 *
 *   baseline -O2 (hi=32) | -O1 34 | -Os 38 | -fno-rerun-cse-after-loop 37
 *   -fno-gcse 35 | -fno-cse-follow-jumps 35 | -fno-cse-skip-blocks 31
 *   -fno-expensive-optimizations 35 | -fno-force-mem 35 | -fno-caller-saves 35
 *   -fno-schedule-insns2 32 | -fno-function-cse 32 | -fno-peephole 32
 *   -fno-defer-pop 32 | -fno-regmove 32 | -fno-strength-reduce 32
 *   -fno-thread-jumps 32 | -fno-delete-null-pointer-checks 32
 *   -fno-optimize-sibling-calls 32 | -fno-reorder-blocks 32 | -fno-inline-functions 32
 *
 * NO CSE_CFLAGS ROW AND NO -O1 ROW SHOULD BE WRITTEN FOR THIS FUNCTION.  -O1 is a
 * legitimate landing route in this very directory (ovl_30_c_c_c_c_a_a_b.c is built at
 * -O1) and it does NOT help here, which is worth knowing before anyone tries it.
 * `-ffixed-r8..r11` was NOT measured and must NOT be: band-800plus section 7 retracted
 * it as a MASKING flag, and this candidate is already size-and-count close enough that
 * a masked figure would read as a false near-match.
 *
 * ===== WHAT ACTUALLY PAID: TWO PARTITION EDITS, AND THEY ARE NOT ADDITIVE =====
 *
 * All rows against ref 3048 bytes / 1164 encodings.  Size-and-count ranked first,
 * aligncmp only to break ties, per band-800plus section 4.
 *
 *   candidate                                    size   count   aligned   hunks
 *   all-literals baseline                        +4     +4      80.2%     225
 *   + &.L1dd4 as a pointer local (P2)            EXACT  +2      80.3%     225
 *   + task handle SPLIT into two locals (P3)     EXACT  +2      81.2%     213
 *   + BOTH (THIS FILE)                           -4     EXACT   81.3%     213
 *   + THIS + &iwram_3001ebc as a pointer local   -4     EXACT   81.3%     213
 *   + THIS + L1dc0 base as a named pointer       -4     EXACT   81.3%     213
 *   three GetActor pointers split three ways     +4     +4      80.2%     225
 *
 * THE TWO THAT PAID ARE BOTH PARTITION EDITS AND THE DIRECTION IS *SPLIT*, NOT UNIFY:
 *
 *   (a) `int *p = &L1dd4;` then `*p = 0;` and `while (*p != 0x18) __WaitFrames(1);`.
 *       The ROM's `ldr r5, =.L1dd4 / str r3,[r5] / ... / ldr r3,[r5]` twice puts the
 *       ADDRESS in a call-saved register across roughly twenty calls while the VALUE
 *       reloads.  Spelling the accesses as bare `L1dd4` leaves gcc to invent that
 *       pseudo itself and it lands in r7.  The idiom is precedented in the same
 *       directory (ovl_30_c_c_c_c_a_a_b.c, same word) -- worth 2 encodings and it is
 *       what makes SIZE exact on its own.
 *
 *   (b) ONE LOCAL PER StartTask/StopTask PAIR, not one reused across both.  The ROM
 *       loads `=OvlFunc_917_20095a0` into r5 TWICE and the two ranges never overlap.
 *       Two variables beat one by 2 encodings and 12 hunks.  THE EVIDENCE FOR SPLIT AND
 *       FOR UNIFY IS THE SAME EVIDENCE -- a register repeating across disjoint ranges --
 *       and here SPLIT won; the discriminator the brief names (the reference's
 *       reference count on that register) is what settles it: 22 mentions of r5 in the
 *       reference against our 68 says we have too MANY long-lived things, and unifying
 *       two short ranges into one long one moves that count the wrong way.
 *
 *   (c) AND THEY ARE NOT ADDITIVE, exactly as the brief warned.  (a) alone and (b)
 *       alone each give size-exact/+2.  TOGETHER they give count-exact and overshoot
 *       size by -4.  Neither one predicted the pair.  APPLY THE WHOLE PARTITION BEFORE
 *       CONCLUDING -- three of the seven rows above would have read "inert" in
 *       isolation.
 *
 * ===== THREE LEVERS MEASURED INERT, WORTH RECORDING SO THEY ARE NOT RETRIED =====
 *
 *   `unsigned char **g = &iwram_3001ebc;` across the `.L8a4`/`.L8f8` pair.  The ROM
 *   reuses r5 for `ldr r2,[r5]` on BOTH sides of the inner `if`, which looks exactly
 *   like the brief's "a pointer read from a global and used across a call must be a
 *   source local" lever.  BYTE-IDENTICAL to leaving the two sites as plain
 *   `*(unsigned short *)(iwram_3001ebc + (0xec << 1))` accesses.  The reason is that
 *   the lever is about the POINTER VALUE, and here the ROM keeps the pointer's ADDRESS
 *   (a constant, which cse1 commons for free) while RELOADING the value at each site --
 *   which the plain spelling already produces.  REPRODUCING THE ROM'S NUMBER OF
 *   ACCESSES, two, is what matters; naming the base adds nothing.
 *
 *   `int *q = L1dc0;` for the three-word coordinate store.  Byte-identical.  The array
 *   spelling already gives the ROM's `ldr r2,=.L1dc0 / str r3,[r2] / str r3,[r2,#4] /
 *   str r3,[r2,#8]` addressing mode.
 *
 *   Three separate `unsigned char *` locals for the three `__MapActor_GetActor(0)`
 *   regions instead of one reused local.  Byte-identical to the one-local form (+4/+4,
 *   80.2%, 225 hunks both ways).  Nothing in those three ranges crosses a call, so the
 *   allocator never needed a call-saved register for any of them and the partition has
 *   no purchase.  THIS IS THE CONTROL that makes (a) and (b) above meaningful: the
 *   partition lever pays ONLY where a range genuinely spans calls.
 *
 * ===== READING NOTES FOR WHOEVER PICKS THIS UP =====
 *
 * `.L1dc0` (0xc bytes), `.L1dcc` (4) and `.L1dd4` (4) are `.lcomm` BSS objects declared
 * in asm/overlays/rom_7a4370/ovl_30_c_c_c_c_c_c_c_c.s -- overlay-local statics, reached
 * with the tree's established `extern int L1dcc __asm__(".L1dcc");` idiom.  THE
 * ASM-LABEL CAPTURE HAZARD WAS CHECKED AND IS CLEAR: the generated `.s` emits labels
 * .L3..L30, all one- or two-digit decimal, and the three externs are four hex digits
 * each containing a HEX LETTER (1dc0, 1dcc, 1dd4), so no generated label can collide
 * with one.  That is the only reason these are safe to use as externs here.
 *
 * The function is `void`: the epilogue is `pop {r5, r6} / pop {r0} / bx r0` with NOTHING
 * written to r0, so there is no `return 0;` to add -- contrast the sibling 2009070,
 * whose `mov r0,#0` before the pop makes its `int` return load-bearing.
 *
 * `char` is unsigned in this toolchain, and the halfword accumulators here are written
 * `unsigned short` for that reason; the two `+= 2` / `+= 1` sites are `ldrh/add/strh`
 * in the ROM, so the unsigned spelling is both shorter and correct.
 *
 * STILL OPEN, and it is band-800plus section 8 item 3 unchanged: which source shape
 * denies cse1 the cross-call commoning of a repeated pool constant.  The test is one
 * grep -- a candidate that leaves 18 separate `(set (reg) (const_int 32776))` insns in
 * `03.cse` has solved it.  Twenty flag settings do not, three pointer-local spellings
 * do not, and the partition edits above do not touch it either; they only removed the
 * quantities the SOURCE owns, which is what took size and count to exact.  The
 * remaining 213 hunks are all register-name substitutions downstream of that one pass.
 
 *
 * *** INSTALL PATH CORRECTED. *** This park's recipe named
 * src/non_matching/ovl_78b2ac/2008488.c, which ALREADY HOLDS OvlFunc_890_2008488's park --
 * a DIFFERENT function that shares this address because every overlay loads at the same
 * base.  Installing there would have destroyed that park.  OvlFunc_917_2008488 lives in
 * asm/overlays/rom_7a4370/, so the correct path is src/non_matching/ovl_7a4370/2008488.c.
 * This is the same non-uniqueness that broke census attribution in batch 302 and needed a
 * bank gate; it bites park PATHS as well as park NAMES.  Derive an overlay park's directory
 * from the REFERENCE's bank, never from the address alone.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __StartTask(void (*f)(void), int n);
extern void __StopTask(void (*f)(void));
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_801776c(int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

extern unsigned char *iwram_3001ebc;
extern int L1dc0[3] __asm__(".L1dc0");
extern int L1dcc __asm__(".L1dcc");
extern int L1dd4 __asm__(".L1dd4");

extern unsigned char ActorCmd_ARRAY_917__02009ab8[];
extern unsigned char gScript_917__02009af4[];
extern unsigned char gScript_917__02009b30[];
extern unsigned char gScript_917__02009b6c[];

extern void OvlFunc_917_20092b4(void);
extern void OvlFunc_917_20092f4(int a, int b);
extern void OvlFunc_917_20095a0(void);
extern void OvlFunc_917_200972c(int a, int b);
extern void OvlFunc_917_20098b8(int a);

void OvlFunc_917_2008488(void)
{
    int flag;
    int *p;
    unsigned int i;
    unsigned char *a;
    unsigned char *s;
    void (*task)(void);
    void (*task2)(void);

    flag = __GetFlag(3);
    __Func_80921c4(0, 0xa4 << 1, 0xd4);
    __Func_8092adc(0, 0xc0 << 8, 0x14);
    __PlaySound(0x11);
    __Func_801776c(0x14ed, 1);
    __MapActor_SetSpeed(1, 0x80 << 9, 0x80 << 8);
    __MapActor_SetSpeed(2, 0x80 << 9, 0x80 << 8);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(1, *(int *)(a + 8), *(int *)(a + 0x10));
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(2, *(int *)(a + 8), *(int *)(a + 0x10));
    __MapActor_SetBehavior(1, ActorCmd_ARRAY_917__02009ab8);
    __MapActor_SetBehavior(2, gScript_917__02009af4);
    if (flag != 0) {
        __MapActor_SetSpeed(3, 0x80 << 9, 0x80 << 8);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_SetPos(3, *(int *)(a + 8), *(int *)(a + 0x10));
        __MapActor_SetBehavior(3, gScript_917__02009b30);
    }
    __MapActor_WaitScript(2);
    __CutsceneWait(0x28);
    OvlFunc_917_20098b8(0);
    __Func_8091254(0x20);
    __WaitFrames(0x28);

    p = &L1dd4;
    *p = 0;
    __StartTask(OvlFunc_917_20092b4, 0xc8 << 4);
    __CutsceneWait(0x28);
    __Func_8092adc(1, 0xc0 << 7, 0x14);
    __Func_80933d4(0x33333, 0x6666);
    __Func_80933f8(0x80 << 17, -1, 0xfe << 16, 1);
    __Func_8093530();
    __PlaySound(0xf6);
    __CutsceneWait(0x28);
    __Func_8092adc(2, 0x80 << 6, 0x14);
    __Func_80933f8(0x19d << 16, -1, 0x105 << 16, 1);
    __Func_8093530();
    __PlaySound(0xf6);
    __CutsceneWait(0x28);
    __Func_8092adc(0, 0x80 << 7, 0);
    __Func_8092adc(3, 0x80 << 7, 0x14);
    __Func_80933f8(0xa3 << 17, -1, 0xc0 << 17, 1);
    __Func_8093530();
    __PlaySound(0xf6);
    while (*p != 0x18)
        __WaitFrames(1);
    __StopTask(OvlFunc_917_20092b4);
    __WaitFrames(0xa);

    for (i = 0; i < 4; i++) {
        OvlFunc_917_20098b8(0);
        __Func_8091254(6);
        __WaitFrames(6);
        OvlFunc_917_20098b8(1);
        __Func_8091254(6);
        __WaitFrames(6);
    }
    OvlFunc_917_20098b8(0);
    __Func_8091254(0x28);
    __WaitFrames(0x50);
    __Func_80933f8(0xa4 << 17, 0x80 << 12, 0xd4 << 16, 1);
    __Func_8093530();
    __CutsceneWait(0x28);
    OvlFunc_917_20092f4(0xa, 1);
    __CutsceneWait(0x28);
    __PlaySound(7);
    __MessageID(0x14ee);
    __ActorMessage(8, 0);
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    __Func_809259c(3, 2);
    __Func_80925cc(2, 2);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __Func_8092adc(1, 0xc0 << 8, 0);
    __Func_8092adc(3, 0xc0 << 8, 0);
    __Func_8092adc(2, 0xc0 << 8, 0x14);
    OvlFunc_917_20092f4(0xa, 2);
    __CutsceneWait(0x14);
    OvlFunc_917_20092f4(0xa, 3);
    __CutsceneWait(0x28);
    OvlFunc_917_20092f4(0xa, 1);
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    __MapActor_Emote(0, 0x105, 0);
    __MapActor_Emote(1, 0x105, 0);
    __MapActor_Emote(3, 0x105, 0);
    __MapActor_Emote(2, 0x105, 0x28);
    __Func_80933f8(0xea << 16, 0, 0xe8 << 16, 1);
    __Func_8093530();
    __CutsceneWait(0x28);
    OvlFunc_917_20092f4(0xb, 1);
    __CutsceneWait(0x28);
    OvlFunc_917_20092f4(0xb, 3);
    __CutsceneWait(0x14);
    __Func_8093040(0x4009, 0, 0x14);
    OvlFunc_917_20092f4(0xb, 2);
    __CutsceneWait(0xa);
    __Func_8092adc(0, 0xc0 << 7, 0);
    __Func_8092adc(1, 0xc0 << 7, 0);
    __Func_8092adc(2, 0xc0 << 7, 0);
    __Func_8092adc(3, 0xc0 << 7, 0x14);
    OvlFunc_917_20092f4(0xb, 3);
    __CutsceneWait(0x14);
    OvlFunc_917_20092f4(0xb, 2);
    __CutsceneWait(0x14);
    OvlFunc_917_20092f4(0xb, 3);
    __Func_8093040(0x4009, 0, 0xa);
    OvlFunc_917_20092f4(0xa, 0);
    __CutsceneWait(0x14);
    __ActorMessage(0x8008, 0);
    OvlFunc_917_20092f4(0xa, 1);
    __CutsceneWait(0x14);
    __Func_8092c40(0x8008, 0);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __Func_8092adc(1, 0xe0 << 8, 0);
    __Func_8092adc(2, 0xa0 << 8, 0);
    __Func_8092adc(3, 0xc0 << 8, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __ActorMessage(0x4009, 0);
        __ActorMessage(0x8008, 0);
    } else {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 2;
        __MapActor_Emote(3, 0x103, 0);
        __MapActor_Emote(1, 0x103, 0);
        __MapActor_Emote(2, 0x103, 0x28);
        __MapActor_SetAnim(1, 4);
        __ActorMessage(1, 0);
        if (flag != 0) {
            __Func_80925cc(3, 2);
            __ActorMessage(3, 0);
        } else {
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        }
        __MapActor_DoAnim(2, 3);
        __ActorMessage(2, 0);
        __ActorMessage(0x4009, 0);
        __ActorMessage(0x8008, 0);
    }
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    __MapActor_DoAnim(2, 3);
    __Func_80933f8(0xa4 << 17, 0x80 << 12, 0xd4 << 16, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    OvlFunc_917_20092f4(0xa, 0);
    __CutsceneWait(0x14);
    OvlFunc_917_20098b8(0);
    __Func_8091254(1);
    __WaitFrames(1);
    __Func_8091200(0x406218, 1);
    __Func_8091254(0x28);
    __CutsceneWait(0x3c);

    L1dcc = 0;
    L1dc0[0] = 0xa4 << 17;
    L1dc0[1] = 0xc0 << 14;
    L1dc0[2] = 0xcd << 16;
    task = OvlFunc_917_20095a0;
    __StartTask(task, 0xc8 << 4);
    __CutsceneWait(0x64);
    __StopTask(task);
    __Func_8091200(0x7fff, 0);
    __Func_8091254(0x3c);
    __CutsceneWait(0x64);
    OvlFunc_917_20098b8(0);
    __Func_8091254(0x14);
    __CutsceneWait(0x28);
    OvlFunc_917_20092f4(0xa, 1);
    __CutsceneWait(0xa);
    __MessageID(0x14fb);
    __ActorMessage(0x8008, 0);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    __MapActor_DoAnim(2, 3);
    __Func_80933f8(0xea << 16, 0, 0xe8 << 16, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    __ActorMessage(0x4009, 0);
    __Func_8093040(0x8008, 0, 0xa);
    __Func_80925cc(1, 2);
    __Func_8092adc(0, 0xc0 << 7, 0);
    __Func_8092adc(1, 0xe0 << 8, 0xa);
    __Func_8092c40(1, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __MapActor_Emote(1, 0x81 << 1, 0x28);
    } else {
        __MapActor_DoAnim(1, 4);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    }
    __ActorMessage(1, 0);
    OvlFunc_917_20092f4(0xa, 4);
    __CutsceneWait(0x14);
    __MessageID(0x1501);
    __ActorMessage(0x8008, 0);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __Func_8092adc(1, 0xc0 << 8, 0);
    __ActorMessage(0x8008, 0);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    __MapActor_DoAnim(2, 3);
    OvlFunc_917_20092f4(0xa, 4);
    __CutsceneWait(0x14);
    __Func_8093040(0x8008, 0, 0x14);
    OvlFunc_917_20092f4(0xb, 0);
    __Func_8093040(0x4009, 0, 0x14);
    OvlFunc_917_20092f4(0xb, 3);
    __CutsceneWait(0x28);
    OvlFunc_917_20092f4(0xb, 1);
    __CutsceneWait(0x14);
    __Func_8093040(0x4009, 0, 0x14);
    OvlFunc_917_20092f4(0xa, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x8008, 0);
    __MapActor_Emote(0, 0x81 << 1, 0);
    __MapActor_Emote(1, 0x81 << 1, 0);
    __MapActor_Emote(3, 0x81 << 1, 0);
    __MapActor_Emote(2, 0x81 << 1, 0x50);
    OvlFunc_917_20092f4(0xb, 5);
    __CutsceneWait(0x3c);
    OvlFunc_917_20092f4(0xb, 3);
    __CutsceneWait(0x14);
    __Func_8093040(0x4009, 0, 0x14);
    OvlFunc_917_20092f4(0xa, 5);
    __CutsceneWait(0x28);
    OvlFunc_917_20092f4(0xa, 2);
    __CutsceneWait(0x14);
    __Func_8093040(0x4008, 0, 0x14);
    __Func_80925cc(1, 2);
    __Func_8092adc(1, 0x80 << 8, 0xa);
    __ActorMessage(1, 0);
    __Func_8092adc(2, 0x80 << 8, 0x14);
    __ActorMessage(0x8002, 0);
    OvlFunc_917_20092f4(0xb, 4);
    __CutsceneWait(0x14);
    __Func_8093040(0x4009, 0, 0x14);
    __Func_8092adc(1, 0xe0 << 8, 0);
    __Func_8092adc(0, 0xc0 << 7, 0xa);
    __Func_8093040(1, 0, 0x14);
    OvlFunc_917_20092f4(0xa, 1);
    __Func_8093040(0x8008, 0, 0xa);
    OvlFunc_917_20092f4(0xa, 2);
    __CutsceneWait(0x14);
    OvlFunc_917_20092f4(0xb, 3);
    __CutsceneWait(0x28);
    OvlFunc_917_20092f4(0xb, 0);
    __CutsceneWait(0x14);
    OvlFunc_917_20098b8(0);
    __Func_8091254(1);
    __WaitFrames(1);
    __Func_8091200(0x406218, 1);
    __Func_8091254(0x28);
    __CutsceneWait(0x3c);

    L1dcc = 0;
    L1dc0[0] = 0x88 << 16;
    L1dc0[1] = 0xa0 << 13;
    L1dc0[2] = 0x81 << 17;
    task2 = OvlFunc_917_20095a0;
    __StartTask(task2, 0xc8 << 4);
    __CutsceneWait(0x64);
    __Func_8092adc(0, 0xc0 << 7, 0);
    __Func_8092adc(1, 0xc0 << 7, 0);
    __Func_8092adc(3, 0xc0 << 7, 0);
    __Func_8092adc(2, 0xc0 << 7, 0x28);
    __Func_809259c(2, 1);
    __MapActor_Emote(2, 0x80 << 1, 0x14);
    __Func_8093040(0x8002, 0, 0xa);
    __Func_80925cc(0, 2);
    __Func_8092adc(0, 0x80 << 6, 0xa);
    __MapActor_DoAnim(0, 3);
    OvlFunc_917_20092f4(0xa, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x8008, 0);
    __MapActor_Emote(2, 0x101, 0x3c);
    __Func_8092adc(2, 0xc0 << 8, 0xa);
    __Func_8093040(0x8002, 0, 0xa);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __Func_8092adc(3, 0xc0 << 8, 0);
    __Func_8092adc(1, 0xc0 << 8, 0x14);
    __Func_8093040(0x8008, 0, 0xa);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0xa);
    __Func_8092adc(0, 0xc0 << 7, 0);
    __Func_8092adc(1, 0xc0 << 7, 0);
    __Func_8092adc(3, 0xc0 << 7, 0);
    __Func_8092adc(2, 0xc0 << 7, 0x78);
    __StopTask(task2);
    __CutsceneWait(0x3c);
    OvlFunc_917_20098b8(0);
    __Func_8091254(0x28);
    OvlFunc_917_20092f4(0xa, 2);
    __CutsceneWait(0x14);
    __Func_8093040(0x8008, 0, 0x14);
    OvlFunc_917_20092f4(0xb, 3);
    __ActorMessage(0x4009, 0);
    __ActorMessage(0x8008, 0);
    OvlFunc_917_20092f4(0xb, 4);
    __CutsceneWait(0x14);
    __Func_8093040(0x4009, 0, 0xa);
    __Func_809259c(0, 2);
    __Func_809259c(1, 2);
    __Func_809259c(3, 2);
    __Func_80925cc(2, 2);
    OvlFunc_917_20092f4(0xa, 1);
    __Func_8092c40(0x8008, 0);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __Func_8092adc(1, 0xe0 << 8, 0);
    __Func_8092adc(3, 0xc0 << 8, 0);
    __Func_8092adc(2, 0xa0 << 8, 0);
    if (__Func_8091c7c(0, 0) == 1)
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    __CutsceneWait(0xa);
    OvlFunc_917_20092f4(0xa, 2);
    __CutsceneWait(0x14);
    OvlFunc_917_20092f4(0xb, 3);
    __CutsceneWait(0x28);
    OvlFunc_917_20092f4(0xa, 1);
    __CutsceneWait(0x14);
    __Func_8093040(0x8008, 0, 0xa);
    __Func_8092adc(0, 0x80 << 7, 0);
    __Func_8092adc(1, 0, 0);
    __Func_8092adc(3, 0xc0 << 8, 0);
    __Func_8092adc(2, 0x80 << 8, 0xa);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(3, 3);
    __MapActor_DoAnim(2, 3);
    __PlaySound(0x11);
    s = gScript_917__02009b6c;
    __MapActor_SetBehavior(1, s);
    if (flag != 0)
        __MapActor_SetBehavior(3, s);
    __MapActor_RunScript(2, s);
    OvlFunc_917_20092f4(0xa, 4);
    OvlFunc_917_20092f4(0xa, 4);
    __CutsceneWait(0x14);
    __MessageID(0x1519);
    __ActorMessage(0x8008, 0);
    OvlFunc_917_20092f4(0xb, 4);
    OvlFunc_917_20092f4(0xb, 4);
    __CutsceneWait(0x14);
    __Func_8093040(0x4009, 0, 0xa);
    __MapActor_DoAnim(0, 3);
    __SetFlag(0x845);
    __PlaySound(1);
    OvlFunc_917_200972c(0xb8, 0xb9);
}
