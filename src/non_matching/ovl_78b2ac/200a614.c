/* OvlFunc_890_200a614 -- NON-MATCHING, 19 differing STREAM encodings of 313.
 * Batch 343 brief E: 21 -> 19 and 95 pins -> 50.  STILL PINNED, so by the
 * owner's standing PREFER-PIN-FREE policy this is a PARK, not a landing.
 *
 * RE-DERIVED batch 343: 234 = 234 16-bit instructions (IN PHASE, a true
 * DISTANCE), size exact at 784 bytes, RELOCATIONS IDENTICAL.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_78b2ac/200a614.c \
 *     asm/overlays/rom_78b2ac/ovl_30_c_c_c_c_a.s --func OvlFunc_890_200a614
 *
 * DEVICE-FREE FIGURE: 294 of 318, 234/237 instructions, +12 bytes.  So the 95
 * pins plus the r5 register local are worth 273 encodings and a pin-free
 * landing is NOT readily available.  Left for pass 3.
 *
 * ========== WHAT CAME OUT, AND THE PARK'S OWN CONTROL WAS INVALID ==========
 *
 * A 36-site SINGLE-DROP sweep (fold one pin block back to a plain call, keep
 * the other 35) found, against a base of 21:
 *   site 0, the __Func_80933f8 call            **19**  <- BETTER THAN BASE
 *   18 sites INERT at 21: 2,5,6,8,19,20,21,22,24,25,27,29,30,31,32,33,34,35
 *   12 sites mildly load-bearing: 23 at 4,7,9,10,11,12,13,14,16,17,26; 24 at 3
 *   5 sites heavily load-bearing: 295 (1), 190 (15), 176 (18), 151 (23), 115 (28)
 *   the `register int m __asm__("r5")` local alone: 25, so it is worth 4
 * Then the 18 inert sites AND site 0 were dropped JOINTLY -- the batch-341
 * brief D move -- and the joint drop HELD at 19 with relocations still
 * identical and no size or instruction-count change.  45 of the 95 pins are
 * gone for a BETTER figure.
 *
 * > THE PARK'S OWN CONTROL WAS MEASURED WITH THE PINS IN AND IS WRONG BECAUSE
 * > OF IT.  The park records "ALL 48 PERMUTATIONS of the mov order across the
 * > five sites measure EXACTLY 21.  Not one encoding moves", and names
 * > __Func_80933f8 as one of those five with "barriers at __Func_80933f8 are
 * > inert".  It swept the ORDER of that site's fill and never tried REMOVING
 * > the fill: deleting site 0's pins outright is worth 2 encodings and moves
 * > `first` from index 5 to index 39, i.e. it retires the entire first residue
 * > cluster.  An inherited control measured with the device in is UNTRUSTED.
 *
 * AND THE 23-GROUP'S COST IS NOT ADDITIVE -- THIS IS THE REAL BOUND.  Each of
 * the 12 mildly-load-bearing sites costs 2 alone.  Dropped JOINTLY on top of
 * the 19 body they cost 51, not 24 (figure 70), and dropping all but the five
 * heavy sites is 95.  Both of those variants also go RELOC-offsets.  So "worse
 * by 2 alone" does NOT compose, and a depin sweep that only adds up single-drop
 * costs will overestimate how far it can get.  Contrast p1 in this same brief,
 * where the single-drop costs ARE strictly additive up to six sites.
 *
 * MEASURED INERT ON THE NEW 19 BASE (so: recorded negatives, do not re-screen):
 *   the anchor axis, 22 placements across the first 24 top-level statement
 *   boundaries -- 12 inert at 19, the rest 20/21/23/24.  Best stays 19.
 *   the flag axis re-crossed on the deeper joint drops: -fno-strict-aliasing
 *   is worth 4 on them (95 -> 91, 70 -> 66) but they are far worse bases;
 *   -fno-gcse and -fno-rerun-cse-after-loop bit-identically inert;
 *   -fno-schedule-insns2 194-205; -O1 268-277 at -16 bytes.
 *
 * NEXT: the five heavy sites (1, 15, 18, 23, 28) and the r5 local are the whole
 * remaining pin debt.  Site 1 is the only one worth 274 on its own, so it is
 * the one to read the dumps on.
 */

/* ===== THE ORIGINAL PARK HEADER, CARRIED FORWARD ===== */
/* OvlFunc_890_200a614 -- NON-MATCHING, 21 differing STREAM encodings of 313.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch_elev/b258/rich/final/ovl_30_c_c_c_c_a.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 313 / ours 313 stream encodings; 234/234 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_78b2ac/200a614.c \
 *     asm/overlays/rom_78b2ac/ovl_30_c_c_c_c_a.s --func OvlFunc_890_200a614
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
// fakematch
/* OvlFunc_890_200a614  --  0x0200a614   ** PARKED at 21 of 313 encodings **
 *   [asm/overlays/rom_78b2ac/ovl_30_c_c_c_c_a.s, the ONLY function in that .s.
 *    tools/asmfacts.py: WHOLE / convert directly.  The three `.L` symbols in
 *    the reference are all LOCAL to it -- .L288c is a branch target, .L2918 is
 *    the epilogue label after a POOL JUMP, and .L28f0 is a gcc-generated pool
 *    word (`.word 0`) dumped inside the function.  Nothing to export.]
 *
 * VERDICT, re-measured three times on this exact file:
 *
 *   XX ENCODINGS differ in 21 place(s) (ref 313, ours 313)
 *      first at index 5: ref 208f  ours 2286
 *
 * SIZE IS EXACT (784 bytes), the encoding COUNT is exact (313), and
 * RELOCATIONS ARE IDENTICAL -- objcmp prints no relocation line at all.  Every
 * residue is a register name or a scheduling slot inside a matched instruction
 * sequence.
 *
 * LANDING (nothing needs changing):
 *   target   src/overlays/rom_78b2ac/ovl_30_c_c_c_c_a.c
 *   ld line  overlays/rom_78b2ac/overlay.ld:37 already reads
 *            `asm/overlays/rom_78b2ac/ovl_30_c_c_c_c_a.o(.text)`; the cross-dir
 *            rule `asm/%.o: src/%.c` builds it from src/.  Line stays VERBATIM.
 *   HAZARD   the basename `ovl_30_c_c_c_c_a` exists in at least EIGHT other
 *            overlay directories.  Match linker and Makefile lines on the FULL
 *            PATH.  The two explicit rom_78b2ac rules in the Makefile name
 *            ovl_30_c_c_a_a_c_b.o and ovl_30_c_c_a_a_b.o literally and no
 *            wildcard reaches this stem: tryc.makefile_flags() = set().
 *   symbols  `iwram_3001ebc` (0x03001EBC, wram.sym:226) is the only external
 *            datum.  It is declared here as an ARRAY of pointers, see below.
 *
 * THE ONLY TYPE DECISION IN THE FILE, AND IT IS FORCED BY THE ROM.  The ROM
 * holds `&iwram_3001ebc` in r8 for three quarters of the function and then
 * reaches a SECOND iwram pointer through it:
 *
 *     ldr r0, =iwram_3001ebc / ldr r3, [r0]    <- iwram_3001ebc
 *     ...    mov r0, r8      / ldr r3, [r0, #0x14]
 *
 * 0x03001EBC + 0x14 = 0x03001ED0 = `iwram_3001ed0` (wram.sym:231).  gcc cannot
 * derive one absolute symbol's address from another's, so the two MUST be
 * members of one object in the source.  Declaring
 *
 *     extern unsigned char *iwram_ptr[] __asm__("iwram_3001ebc");
 *
 * and writing `iwram_ptr[0]` / `iwram_ptr[5]` reproduces both loads with the
 * base held, and the relocation is still against `iwram_3001ebc`.  The
 * asm-renamed-extern idiom is the one already used for `.L5b84` in
 * src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_b.c.  This is the cheap half
 * of the struct that src/non_matching/ovl_7892c8/200874c.c parks for want of --
 * see the cross-reference at the end.
 *
 * ============================ WHAT CLOSED IT ============================
 *
 * Counted on a NORMALISED diff (register-name and pool-offset aware), because
 * objcmp compares encodings POSITIONALLY and one inserted instruction early in
 * the function misaligns every later index: the first transcription reports
 * "291 of 313 differing" and is in fact about forty instructions away.  A
 * one-instruction count difference makes the objcmp NUMBER useless on this
 * function while leaving SIZE and the relocation list exact and authoritative.
 *
 * | step                                            | normdiff | insns |
 * |-------------------------------------------------|----------|-------|
 * | first transcription, 24 pin blocks, ROM fills   |    75    | 303   |
 * | + `register int m __asm__("r5")` on the `|= 1`  |    72    | 303   |
 * | + `z = 0` hoisted above the mask block          |    52    | 308   |
 * | + one local per GetActor call site              |    32    | 304   |
 * | + the iwram init block reordered                |  **21**  | 303   |
 *
 * THE PROLOGUE IS THE SCOREBOARD, AND IT TOOK TWO SEPARATE LEVERS.  The ROM
 * pushes FIVE call-saved registers (`push {r5,r6,r7,lr}` plus
 * `mov r7,r10 / mov r6,r8 / push {r6,r7}`) holding, in order,
 *   r5  0xfe then the `1` of the actor-flag OR
 *   r6  the zero stored as four halfwords in the tail
 *   r7  the `1` stored as an int early and as three bytes in the tail
 *   r8  &iwram_3001ebc
 *   r10 the offset 0xe4 << 1, used once at the top and once at the very end
 * Plain C holds only FOUR, and the two missing values are two DIFFERENT bugs:
 *
 *   1. THE TWO ONES GET COMMONED.  gcse merges the early held `1` with the OR
 *      mask, so one register does both jobs.  `register int m __asm__("r5")` --
 *      the ROM's own register -- splits them and makes both OR sites BYTE-EXACT
 *      at a stroke: `mov r5,#1 / orr r3,r5 / strb r3` then `orr r5,r3 /
 *      strb r5`, the mask in the destination at the second site exactly as
 *      src/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_a.c records.  Unlike that
 *      file this one needs the HARD register, not just a named local.
 *      MEASURED: `__asm__ volatile ("" : "+r" (one))` on the other one instead
 *      is 79 (worse); an `"+r"` barrier on `m` is 79.
 *   2. THE ZERO MUST BE BORN BEFORE THE MASK BLOCK.  Written at its first use
 *      the zero is local to the tail block, does not conflict with the mask,
 *      takes r5 after it and leaves only four call-saved values.  Written
 *      ANYWHERE above the actor-flag block it conflicts, becomes the fifth
 *      value, and the prologue becomes the ROM's exactly -- `mov r7, sl /
 *      mov r6, r8 / push {r6, r7}` -- while sched2 sinks the `mov r6, #0`
 *      back down to the ROM's own slot, one instruction before the
 *      __MapActor_GetActor it precedes.  Two hoist points (above `n = 0xfe`
 *      and above `__StartEarthquake`) tie at 52; above `one = 1` is 59; the
 *      two points BELOW the mask block are 72, i.e. inert.
 *
 *   > A PUSH MASK ONE REGISTER NARROWER THAN THE ROM'S, WHERE THE MISSING VALUE
 *   > IS A SMALL CONSTANT, IS A BIRTH-POINT QUESTION.  Hoisting the assignment
 *   > out of the block that uses it lengthens the live range until it conflicts,
 *   > and sched2 puts the instruction back where the ROM has it.  This is the
 *   > opposite direction from the recorded "lengthened live range lowers the
 *   > allocno's priority" lever and it is the same knob.
 *
 * ONE LOCAL PER GetActor CALL SITE, AND THE SAME CHANGE INVERTS BETWEEN BASES.
 * The four actor-flag sites read `bl GetActor / add r0, #0x5a / ldrb` in the
 * ROM, with the pointer staying in r0.  Sharing one `unsigned char *p` makes gcc
 * copy it (`mov r1, r0 / add r1, #90`) at all four, four instructions long.
 * Separate `p1..p4` is worth 72 -> 32.  MEASURED ON THE FIRST TRANSCRIPTION THE
 * SAME CHANGE IS WORSE (65 against 75, and eight bytes short): it only pays once
 * the prologue is right.  The recorded harness trap about levers that invert
 * between bases applies to source edits too, not just generators -- re-test a
 * rejected lever after any change to the register allocation.
 *
 * THE iwram INIT BLOCK IS AN ASSIGNMENT-ORDER PROBLEM, WORTH 11.  The ROM's
 * eight instructions are
 *     ldr r0,=iwram_3001ebc / mov r1,#0xe4 / ldr r3,[r0] / lsl r1,#1 /
 *     mov r7,#1 / str r7,[r3,r1] / mov r8,r0 / mov r10,r1
 * -- the two high registers are filled AFTER the store, from the low registers
 * the store already used.  Written `one = 1; o = 0xe4 << 1; p = iwram_ptr[0];`
 * gcc fills r10 first and reads it straight back (`mov r10, r0 / ldr r0, [r1] /
 * mov r2, r10`), nine instructions.  Writing the POINTER FIRST --
 * `p = iwram_ptr[0]; o = 0xe4 << 1; one = 1;` -- is eight and costs 32 -> 21.
 * `o` first then the pointer is 32; the pointer then `one` then `o` is 21 too.
 *
 * ======================= THE RESIDUE, 21 ENCODINGS =======================
 *
 * A. THE ARGUMENT `mov`s ROTATE AT FIVE SITES -- 12 encodings, and the fill
 *    order is PROVABLY INERT.  Each of __Func_80933f8 and the four
 *    __Func_8012330 calls takes two or three arguments built as
 *    `mov rN, #imm8` + `lsl rN, #k`.  The ROM emits the movs ASCENDING and the
 *    shifts in its own order; we get the shifts right and the movs rotated or
 *    reversed.  ALL 48 PERMUTATIONS of the mov order across the five sites
 *    (4! at 80933f8 and 3! at each of the others, swept per site) measure
 *    EXACTLY 21.  Not one encoding moves.  This sharpens the recorded "at a
 *    shift-built operand the ROM's own order is INERT": it is inert to EVERY
 *    order, so the decision is sched2's alone and the pin fill cannot reach it.
 *
 *    AND THE ORDERING BARRIER COSTS A CALL-SAVED REGISTER.  The recorded cure
 *    for n movs needing an order is n-1 `__asm__ volatile ("" : : "r" (q))`
 *    barriers.  Here a SINGLE barrier at the first __Func_8012330 takes the
 *    function from 784 bytes to 776 and the prologue back to four call-saved
 *    registers: r10 disappears and the count goes 21 -> 285.  Same at the
 *    __Func_8012330(-1,-1,...) site.  Barriers at __Func_80933f8 are inert.
 *    NEW: an ordering barrier is not free when the function is at its register
 *    ceiling -- it is a scheduling fence, the fence changes live ranges, and a
 *    value at the margin loses its register.  Check the push mask after adding
 *    one.  Floor for this cluster on this base: 12.
 *
 * B. THE iwram HIGH-REGISTER FILL STILL SITS TWO SLOTS EARLY -- 3 encodings.
 *    ROM `ldr r3,[r0] / ... / mov r8,r0`; ours `mov r8,r0 / ldr r0,[r0]`.
 *    Both are eight instructions and r8/r10 are correct; only the order differs.
 *
 * C. `mov r2, r8 / ldr r1, [r2]` IS SCHEDULED TWO SLOTS EARLY in the closing
 *    store block -- 4 encodings, same instructions.
 *
 * D. THE POOLED ZERO BYTE STORE -- 1 encoding plus its pool word.  The ROM has
 *    `ldr r1, .L28f0` against `.word 0` and stores that byte; we emit
 *    `ldrh r1, .L4` against the same word.  This is the THIRD disagreeing
 *    specimen of the narrow-store-constant question already recorded on
 *    src/non_matching/ovl_7892c8/200874c.c (which movs a value the ROM pools),
 *    src/non_matching/ovl_77a7c8/200b57c.c (which pools a zero the ROM movs)
 *    and OvlFunc_928_2008500 (where the bare literal gives the narrow form and
 *    is right).  Here the ROM takes the WIDE form for a BYTE store and we take
 *    the narrow one.  Spellings not yet tried: a declared struct field, and an
 *    int local.
 *
 * CROSS-REFERENCE WORTH FOLLOWING UP.  The whole closing block of this function
 * -- three halfword zeros at base+0xe00/0xe02/0xe04 and four bytes 0,1,1,1 at
 * base+0x2a00..0x2a03 off an iwram pointer -- is the SAME block, on the same
 * pointer, that src/non_matching/ovl_7892c8/200874c.c is parked on, down to the
 * `add rN, #2` chains and the pooled zero.  That park's "NEXT THING TO TRY:
 * build the struct" is the right next move for BOTH, and this file's
 * `extern unsigned char *iwram_ptr[] __asm__("iwram_3001ebc")` already
 * establishes that the two iwram pointers are one object, which is the first
 * half of that struct.  The ROM here keeps 0xe00 AND 0xe02 in two registers and
 * derives only the third, exactly the discrepancy that park records.
 *
 * DIAGNOSTICS RUN: -fno-schedule-insns2 is not useful as a discriminator on a
 * function whose objcmp number is misaligned, so it was not used; every residue
 * above was read off the normalised diff instead.
 *
 * Harness: scratch_elev/b258/rich/t2 -- dd.py (normalising side-by-side diff),
 * nd.py (normalised difference count -- USE THIS, not objcmp's positional
 * count, while the instruction count is off), sweep.py (the 48-permutation mov
 * order sweep), and the c[1-8].c / e_*.c ladder.
 */
extern unsigned char *iwram_ptr[] __asm__("iwram_3001ebc");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __PlaySound(int id);
extern void __StartEarthquake(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_800fe9c(void);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8091200(int a, int b);
extern void __Func_8091220(int a, int b);
extern void __Func_8091254(int a);
extern int  __Func_8091c7c(int a, int b);
extern void __Func_8091ff0(int a);
extern void __Func_8092adc(int a, int b, int c);
extern int  __Func_8092c40(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_890_200a5fc(int a, int b);

#define R0 register int q0 __asm__("r0")
#define R1 register int q1 __asm__("r1")
#define R2 register int q2 __asm__("r2")
#define R3 register int q3 __asm__("r3")

void OvlFunc_890_200a614(void)
{
    unsigned char *p, *p1, *p2, *p3, *p4, *b, *q;
    int one, o, z, n;
    register int m __asm__("r5");

    __CutsceneStart();
    __Func_80933f8(((0x8f) << 17), (-(1)), ((0x86) << 16), (0));
    __Func_800fe9c();
    __WaitFrames(1);
    { R0; R1; q0 = 0x7fff; q1 = 0;
      __Func_8091220(q0, q1); }
    __Func_8091200((0x7fff), (0));
    __Func_8091254(1);
    __WaitFrames(1);
    { R0; R1; R2; q1 = 0x8c; q2 = 0x86; q0 = 1; q1 <<= 17; q2 <<= 16;
      __MapActor_SetPos(q0, q1, q2); }
    { R0; R1; R2; q1 = 0xa0; q2 = 0; q0 = 1; q1 <<= 8;
      __Func_8092adc(q0, q1, q2); }
    p = iwram_ptr[0];
    o = 0xe4 << 1;
    one = 1;
    *(int *)(p + o) = one;
    __MapTransitionIn();
    __WaitMapTransition();
    __Func_8091200((0x2051cc), (1));
    __Func_8091254(0x78);
    __WaitFrames(0x78);
    __SetFlag(0x201);
    __ClearFlag(0x80 << 2);
    __ClearFlag(0x202);
    __Func_8091200(((0x80) << 9), (2));
    __Func_8091254(0x3c);
    __WaitFrames(0x64);
    { R0; R1; q1 = 0x81; q0 = 0; q1 <<= 1;
      __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise((1), ((0x81) << 1));
    __CutsceneWait(0x3c);
    { R0; R1; R2; q1 = 0x80; q0 = 0; q1 <<= 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { R0; R1; R2; q1 = 0xe0; q0 = 1; q1 <<= 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { R0; R1; R2; q1 = 0x90; q0 = 0; q1 <<= 8; q2 = 0x28;
      __Func_8092adc(q0, q1, q2); }
    { R0; R1; R2; q1 = 0xa0; q0 = 1; q1 <<= 7; q2 = 0x50;
      __Func_8092adc(q0, q1, q2); }
    { R0; R1; R2; q1 = 0x80; q0 = 0; q1 <<= 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    { R0; R1; R2; q1 = 0x80; q0 = 1; q1 <<= 5; q2 = 0x3c;
      __Func_8092adc(q0, q1, q2); }
    { R0; R1; R2; q0 = 0xc0; q1 = 0xc0; q2 = 0x80; q1 <<= 10; q2 <<= 9; q0 <<= 10;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    { R0; R1; R2; q1 = 0xc0; q0 = 0; q1 <<= 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { R0; R1; R2; q1 = 0xe0; q0 = 1; q1 <<= 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { R0; R1; R2; q0 = 1; q1 = 1; q2 = 0xe666; q1 = -q1; q0 = -q0;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    __Func_80925cc((1), (2));
    __MessageID(0x1119);
    __Func_8092c40((1), (0));
    __Func_8091c7c((0), (0));
    __MapActor_SetAnim((0), (1));
    __CutsceneWait(0x28);
    __PlaySound(0x6b);
    { R0; R1; R2; q0 = 0x80; q1 = 0x80; q2 = 0x80; q1 <<= 11; q2 <<= 9; q0 <<= 11;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x28);
    __StartEarthquake();
    z = 0;
    n = 0xfe;
    p1 = __MapActor_GetActor(0);
    p1[0x5a] &= n;
    p2 = __MapActor_GetActor(1);
    n &= p2[0x5a];
    p2[0x5a] = n;
    __MapActor_Jump((0), (4), (0));
    __MapActor_Jump((1), (4), (0));
    { R0; R1; R2; q1 = 0x96; q0 = 0; q1 <<= 1; q2 = 0x82;
      __MapActor_TravelTo(q0, q1, q2); }
    __MapActor_TravelTo((1), ((0x85) << 1), (0x90));
    __MapActor_WaitMovement(1);
    __CutsceneWait(0x28);
    p3 = __MapActor_GetActor(0);
    m = 1;
    p3[0x5a] |= m;
    p4 = __MapActor_GetActor(1);
    m |= p4[0x5a];
    p4[0x5a] = m;
    __Func_8091ff0(0x8d);
    { R0; R1; R2; q0 = 0x80; q1 = 0x80; q2 = 0x80; q2 <<= 9; q1 <<= 9; q0 <<= 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x28);
    __Func_80925cc((1), (2));
    OvlFunc_890_200a5fc((1), (0xa));
    __MapActor_SetAnim((0), (3));
    __MapActor_DoAnim((1), (3));
    __MapActor_SetAnim((1), (2));
    p = __MapActor_GetActor(0);
    if (p != 0) {
        __MapActor_TravelTo((1), (*(short *)(p + 0xa)), (*(short *)(p + 0x12)));
    }
    __MapActor_WaitMovement(1);
    __MapActor_SetPos((1), (0), (0));
    b = iwram_ptr[5];
    *(short *)(b + 0xe00) = z;
    *(short *)(b + 0xe02) = z;
    *(short *)(b + 0xe04) = z;
    b[0x2a00] = 0;
    b[0x2a01] = one;
    b[0x2a02] = one;
    b[0x2a03] = one;
    *(short *)0x5000000 = z;
    q = iwram_ptr[0];
    *(int *)(q + 0x1c0) = 0x204;
    *(int *)(q + o) = 0x10;
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_890_200a614 -- 0x0200a614, asm/overlays/rom_78b2ac/ovl_30_c_c_c_c_a.s
 *
 * TWENTY-ONE differing of 313, at EXACT size (784 bytes), exact encoding count,
 * and RELOCATIONS IDENTICAL -- objcmp prints no relocation line at all.
 * Candidate: scratch_elev/b258/rich/final/ovl_30_c_c_c_c_a.c. Normalised-diff
 * ladder 75 -> 72 -> 52 -> 32 -> 21.
 *
 * BLOCKER: the `mov` order at a shift-built argument site, floor 12, plus three
 * small scheduling/ordering slots.
 *
 * AND THE ORDER THERE IS INERT TO *EVERY* PERMUTATION -- all 48 measured
 * EXACTLY 21, not one encoding moved. So this is not a fill-order problem and
 * another sweep will not find it. Worse, one ordering barrier at that site COST
 * A CALLEE-SAVED REGISTER: r10 dropped and the count went 21 -> 285.
 *
 * A PUSH MASK ONE REGISTER NARROW WITH A SMALL CONSTANT MISSING IS A
 * BIRTH-POINT QUESTION. Hoisting `z = 0` above the block it conflicts with
 * makes the prologue exact, and sched2 then puts the `mov` back where the ROM
 * has it by itself. That is already in the candidate.
 *
 * CROSS-REFERENCE WORTH FOLLOWING: this function's iwram tail is the SAME BLOCK
 * that src/non_matching/ovl_7892c8/200874c.c is parked on, and this file PROVES
 * the two iwram pointers are one object -- the ROM reaches iwram_3001ed0 as
 * `[&iwram_3001ebc + 0x14]`. That is the first half of that park's "build the
 * struct" note, established here rather than assumed.
 *
 * BASENAME HAZARD: this file's basename exists in EIGHT other overlay
 * directories. Match on the full path; funcindex.py resolves it correctly.
 *
 * asmfacts.py says WHOLE -- convert directly; makefile_flags() is empty; all
 * three `.L` symbols are local; overlay.ld:37 already names the asm/ path and
 * MUST STAY VERBATIM.
 *)

*/
#endif
