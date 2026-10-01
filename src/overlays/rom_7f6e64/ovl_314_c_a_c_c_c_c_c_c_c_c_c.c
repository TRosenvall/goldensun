/* OvlFunc_969_200cbec  --  0x0200cbec  --  MATCHING.  LANDED IN BATCH 314, BRIEF B.
 * Was parked at "2 of 1068, ONE ADJACENT PAIR SWAPPED, sched2's tie-break".
 * The park asked whoever reopened it to go straight to -fsched-verbose=8 on
 * that one block and read the tie-break.  That was the right instruction, and
 * what the dump says is NOT what the park assumed; see THE READING below.
 *
 * tools/objcmp.py, PRODUCTION FLAGS (-O2 -mthumb -mthumb-interwork
 * -mcpu=arm7tdmi -fno-builtin -nostdinc -ffreestanding -fcall-used-r4
 * -Iinclude -- the tree default for this path.  NO Makefile row is needed and
 * none should be written):
 *   OK OvlFunc_969_200cbec -- 2716 bytes, 1068 encodings and 274 relocations identical
 * objcmp --whole agrees: OK whole file -- 2716 bytes, 1068 encodings, 274 relocations.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_c.c \
 *     asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_c.s --whole
 *
 * SPLIT SHAPE: NONE.  grep -c thumb_func_start on the reference is 1 and
 * tools/datacheck.py is SILENT, so this lands as a WHOLE-FILE conversion to
 * src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_c.c with no .global
 * list.  ONE linker row names the object and the basename is unique to this
 * overlay directory: overlays/rom_7f6e64/overlay.ld:63.
 * PINS: tools/shimcount.py reports 116 register pins over 63 PIN2/PIN3/PIN4
 * sites, so THIS LANDING NEEDS ONE fakematch.txt ROW.  Pass three is depinning;
 * the pin is declared, not hidden.
 *
 * ================= WHAT LANDED IT: ONE TYPE AND A MECHANICAL RULE =========
 *
 *     union U66 { int i; short h; };
 *
 * and every one of the nineteen actor-adjust accesses written through it:
 *     ((union U66 *)(p + 8))->i    += ...;
 *     ((union U66 *)(p + 0x10))->i += ...;
 *     ((union U66 *)(p + 0x66))->h  = zero;
 * replacing the raw `*(int *)` / `*(short *)` casts.  Nothing else changed.
 * The point is the ALIAS SET, not the syntax: a member access takes the
 * record's set, and a union holding both widths puts the +8/+0x10 int stores
 * and the +0x66 short store in CONFLICTING sets -- which is what a
 * struct-typed actor would have given for free, and what the raw casts had
 * taken away.
 *
 * ================= THE READING, FROM -da -fsched-verbose=8 ================
 * The residue was
 *     ref   add r3, r5  /  str r3, [r7, #8]  /  ldr r1, =0xffee0000
 *     ours  add r3, r5  /  ldr r1, [pc, #500]  /  str r3, [r7, #8]
 * at objcmp index 646/647, the actor-3 coordinate adjust.  From the sched2
 * region table (t.c.23.sched2):
 *
 *   ;;  1590  173 0 4 622 2 ... : 2873 1875 1611 1593        <- str r3,[r7,#8]
 *   ;;  2654  173 0 6 622 2 ... : 2873 2738 1615 1611 1597   <- r1=0xffee0000
 *
 * FOUR DEPENDENTS AGAINST FIVE, on EQUAL priority 622.  It was never an
 * INSN_LUID tie: it is settled one key earlier, at rank_for_schedule's
 * DEPENDENT COUNT, and LUID would have picked the store -- 1590 precedes 2654
 * in the insn chain.  So the whole job was to move the count by ONE.
 *
 * PRIORITY IS STRUCTURALLY CLOSED and must not be re-attempted:
 *     prio(1590) = 0 + prio(1593) = 0 + 2 + prio(1597)
 *     prio(2654) = 2 + prio(1597)
 * both route through insn 1597 (the `+=` add), so anything that lengthens or
 * shortens that chain moves both together.  The ANTI dep 1590 -> 1593 is on r3
 * and arm_adjust_cost charges it 0, which is why the two come out equal.
 *
 * 2654's three extra dependents are r1 OUTPUT/ANTI links that CROSS TWO CALLS:
 * 1611 = actor-3's trailing OvlFunc_969_200d688, 1615 = the
 * __MapActor_GetActor(0x17) call_value, 2738 = the later PIN4 `q1 = 0x200000`
 * fill.  A void call never SETS r1 in RTL, so an r1 chain is NOT intercepted by
 * a call the way an r0 chain is by a call_value -- which is why the int-return
 * lever has no purchase on an r1 pair and why the fix had to come from the
 * STORE's side.
 *
 * ================= MEASURED, AND WHY EACH ROUTE WAS TAKEN OR DROPPED ======
 *   candidate                                                        differ
 *   THIS FILE: union at all 19 sites                                   0
 *   union on the +0x66 store at the actor-3 site ALONE                 0
 *   union on the +8 store at the actor-3 site ALONE                    0
 *   union on both at the actor-3 site                                  0
 * All four land.  The uniform rule ships because it is a TYPE DECISION about
 * the actor rather than a device aimed at one swap.
 *
 *   `extern long long OvlFunc_969_200d688(...)`                        4
 * A DImode return SETS r0:DI = r0 AND r1, so the call DOES intercept the r1
 * chain and this DID remove the 646/647 pair -- the direct confirmation that
 * the r1 count is the operative term.  It is rejected because the callee's
 * declaration is TU-wide and the same interception broke two other sites:
 * 623/624 (actor-1's `mov r1,sl` against `adds r3,#0x66`) and 816/817 (actor-10,
 * where the reference ITSELF puts the pool load first, so interception flips a
 * site that was already right).
 *   `extern long long __MapActor_GetActor(...)` via a cast macro at its 24
 *   use sites                                                        402, +2 bytes
 * A DI result in r0/r1 costs code at every use; the whole relocation list
 * shifted.  Rejected outright.
 *
 *   `*(volatile int *)(p + 8) += 0xfffc0000;`   actor-3 site           2  INERT
 *   `*(volatile int *)(p + 0x10) += 0xffee0000;` actor-3 site          2  INERT
 *   `*(volatile short *)(p + 0x66) = zero;`      actor-3 site          2  INERT
 * VOLATILE IS NOT A SCHEDULING BARRIER FOR MEMORY IN THIS gcc, and that is
 * worth recording as a general negative.  The dump of the third variant shows
 * insn 1607 as `(set (mem/v:HI (reg r3) 12) (reg:HI r2))` -- MEM_VOLATILE_P
 * plainly set -- with LOG_LINKS `2660 1604 ANTI 1579` and NO dependence on the
 * store 1590, because the two MEMs sit in alias sets 12 (short) and 11 (int).
 * The alias check runs BEFORE volatility is ever consulted, so volatile buys
 * nothing here and the union is the device that works.
 *
 *   `__asm__ __volatile__("")` between the two statements              24
 *   `__asm__ __volatile__("" : "+r"(p))`                              410
 *   the same on a named `dm`                                           35
 * The barrier is a real device elsewhere in this tree; here it splits the
 * scheduling region far too coarsely.  Byte-identical to this file and hence
 * untested rather than disproved: naming the 0xffee0000 either side of the
 * first statement, and spelling both `+=` out as `x = x + k`.
 *
 * ================= WHAT GOT IT TO TWO, IN ORDER, WITH FIGURES =============
 * First transcription (named held quantities, two pins): -12 / -10 / 82.9% / 152.
 *
 * 1. INT CARRIERS FOR THE TWO HALFWORD STORES, plus pins at the two 0x2015
 *    sites and the two 0x110 sites: -20 / -13 / 84.1% / 148.  `*(short *)x = 0xa`
 *    PUT THE TEN IN THE LITERAL POOL (`.word 0x0000000a`) because
 *    `*thumb_movhi_insn` has no immediate form; the reference has `movs r3, #10`.
 *    That is batch 306's pooled-zero defect firing on a NON-ZERO value, and the
 *    generalisation worth keeping is that the gate is the MODE, not the value.
 * 2. PINS AT THE 0x102 SITES (eight __MapActor_Surprise, one __MapActor_Emote):
 *    -16 / -11 / 84.9% / 141.
 * 3. PINS AT THE SEVEN 0x100 __MapActor_Emote SITES: -28 / -14 / 88.9% / 121.
 * 4. PIN EVERY REMAINING SITE WITH A LITERAL ARGUMENT OUTSIDE 0..255 -- 39 more
 *    sites, mechanically, ascending q0..q3 fills.  THIS IS THE STEP THAT DID IT:
 *    SIZE AND COUNT BOTH WENT EXACT AND THE RELOCATIONS WENT IDENTICAL, 26 of
 *    1068.  The documented starting shape ("pin every site with an argument
 *    outside 0..255") is exactly right for this function and should have been
 *    step 1.
 * 5. THE TWO HALFWORD CARRIERS MUST NOT BE LIVE ACROSS THEIR OWN GetActor CALL.
 *    Hoisting the call into its own statement first drops 26 -> 17.  Both
 *    carriers needed their OWN pointer local: reusing the `+0x5a` pointer for
 *    one of them cost a relocation and went to 28.
 * 6. FILL ORDER AT THE THREE PINNED OvlFunc_969_20088a8 SITES.  The reference
 *    fills r1 and its `lsl` BEFORE r0 at all three; ascending gave r0 first.
 *    Flipping to `q1 = ...; q0 = ...;` at those three: 17 -> 11.
 * 7. `dm` AND `dy` BACK TO BARE LITERALS AT THEIR FIRST SITE.  The reference's
 *    own `ldr r6, =0xffe00000` sits INSIDE the first `+=` statement, i.e. cse
 *    made that pseudo from the literal -- so the literal is the source and the
 *    name was the defect.  11 -> 7, and with 6 together 17 -> 7.
 * 8. `__Func_8092c40` WANTS THE DESCENDING FILL, by name.  `q1 = 0; q0 = 1;`
 *    at the one site: 7 -> 5.
 * 9. THE TWO STACK ARGUMENTS OF THE SIX-ARGUMENT `__Func_8010704` AS TWO NAMED
 *    LOCALS: 5 -> 2.  The reference materialises BOTH into two scratch
 *    registers (r3 = 0xa, r2 = 5) and then stores both; with literals our build
 *    reuses r3 for both, store-materialise-store.
 * 10. THE UNION, above: 2 -> 0.
 *
 * ================= WHICH MECHANISM DOMINATED ==============================
 * THE SAME cse1 CROSS-CALL COMMONING as this batch's 20088b4, and in the SAME
 * direction our build always errs -- WE COMMON, THE REFERENCE REBUILDS -- but
 * here it is almost the WHOLE residue.  The reference's pool-load multiset says
 * so in one command: iwram_3001ebc x3, 0xfff00000 x3, 0x2015 x2, 0x14d x2,
 * everything else x1.  THE DISCRIMINATOR IS THE REFERENCE'S OWN REUSE RATE and
 * it is cheap to measure before writing a line: count `mov rlo, rhigh` against
 * the number of wide constant builds.  20088b4: 29 reuses, three ids reloaded
 * 7-8 times each, so it is MIXED and wants named quantities AND selective pins.
 * 200cbec: 25 high-register mentions in 1041 instructions, nothing reloaded
 * more than three times, so it is almost pure REBUILD and wants pins everywhere.
 *
 * WHAT RULES OUT THE ALTERNATIVES.
 *   * NOT the allocator and NOT register pressure: the prologue, the epilogue
 *     and all four high-register parkings came out right on the first
 *     transcription and never moved.
 *   * NOT the pool: size is exact and both sides' pool words agree, including
 *     the three `=0xfff00000` loads the reference makes where it ALSO holds the
 *     same value in r5 -- the two later sites take the literal, not the name.
 *   * NOT a flag.  -fno-rerun-cse-after-loop was NOT tried and must not be
 *     cited: THIS FUNCTION HAS NO LOOP.  It has two REAL conditionals (the
 *     `bne .L5046` / `b .L507e` if/else at reference line 401 and the
 *     `beq .L5094` guard at 453) and two POOL SKIPS (`b .L5010` at 402 and
 *     `b .L5458` at 848, each immediately before a `.pool_aligned`).  Reading
 *     either skip as control flow would have broken the if/else.
 *   * NOT sched1: it does not run in this build.  The -da sequence is
 *     17.lreg 18.greg 19.flow2 20.ce2 23.sched2 25.jump2 26.mach.
 *
 * ================= THE FRAME IS NOT A DECLARATION LIST =====================
 * `sub sp, #8`, and all sixteen `[sp]` / `[sp, #4]` references are STORES
 * immediately before a call, never read, with NO `add rX, sp` and no
 * `mov rX, sp` anywhere in the reference.  So it is a two-word OUTGOING
 * ARGUMENT BLOCK for the six-argument `__CopyMapTiles` and `__Func_8010704`
 * calls, there are NO spill slots, and sorting the two offsets yields nothing.
 * All five frame greps were run.
 *
 * ================= OTHER PROBES MEASURED AND REJECTED =====================
 *   PINNING THE TWO CARRIERS TO r3 (`register int c3 __asm__("r3")`) -- the
 *   obvious way to ask for the reference's register -- is MUCH WORSE: -8 / -4 /
 *   902 differing.  It loses both axes.  The liveness fix in step 5 is the right
 *   route and the pin is the wrong one, which is worth a row because the pin
 *   LOOKS like the direct answer.
 *   BLOCK-SCOPING the two carriers (`{ int c; c = ...; }`) is BYTE-IDENTICAL to
 *   leaving them at function scope.  Inert, so UNTESTED as a lever, not
 *   disproved -- and consistent with the standing finding that region scoping
 *   cannot reach what the allocator decides.
 *   DROPPING THE NAMED POINTER AT THE TWO `+= 3` BUMP SITES paid 26 -> 22: the
 *   reference consumes the loaded pointer in place (`adds r2, r2, r3`) and a
 *   name forces a second register.  It is IN this file.  The discriminator is
 *   the documented one -- what crosses the call; here NOTHING crosses a call
 *   between the load and the store, so the local buys nothing.
 *
 * ================= TRANSCRIPTION NOTES ====================================
 * 1041 reference instructions, 266 calls over 40 callees.  The if/else sets a
 * flag that the JOIN then re-tests and acts on, so the `+= 3` bump appears
 * TWICE in the source even though only one of the two can run per pass; that is
 * the reference's shape, not a transcription artefact -- `flag` is the r8 range
 * the reference materialises at function ENTRY with a declaration initialiser
 * and reads once at the join.
 *
 * `r = *(unsigned char **)((int)&iwram_3001ebc - 0x30);` is the tree's existing
 * idiom for the reference's `ldr r5, =iwram_3001ebc` ... `sub r5, #0x30`: gcc
 * commons the ADDRESS of the pointer and reaches the neighbouring pointer by
 * offset.  The same spelling is in src/non_matching/rom_a1000/80aafb8.c at
 * -0xa0.  It costs no relocation of its own and the relocation list is
 * identical, so the derivation is reproduced exactly.
 *
 * `OvlFunc_969_200d688(p)` takes the actor pointer and the reference emits no
 * `mov r0, r7` before it, because r0 still holds the GetActor result; writing
 * the call with the named pointer reproduces that at all twelve sites.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_969__0200e074[];
extern unsigned char gScript_969__0200e324[];
extern unsigned char gScript_969__0200e360[];
extern unsigned char gScript_969__0200e39c[];
extern unsigned char gScript_969__0200e3c0[];
extern void OvlFunc_969_2008400(void);

extern void OvlFunc_969_2008894(int a);
extern void OvlFunc_969_20088a8(int a, int b);
extern void OvlFunc_969_200d688(unsigned char *a);

extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __CutsceneWait(int n);
extern void __Func_800c5b4(void);
extern void __Func_800c5fc(void);
extern void __Func_800fe9c(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8019aa0(int a, int b, int c);
extern int  __Func_8091c7c(int a, int b);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __MapActor_DoAnim(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);
extern void __MapActor_Jump(int a, int b, int c);
extern void __MapActor_RunScript(int a, unsigned char *s);
extern void __MapActor_SetAnim(int a, int b);
extern void __MapActor_SetBehavior(int a, unsigned char *s);
extern void __MapActor_SetIdle(int a);
extern void __MapActor_SetSpeed(int a, int x, int z);
extern void __MapActor_Surprise(int a, int b);
extern void __MapActor_WaitScript(int a);
extern void __MapTransitionOut(void);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __WaitMapTransition(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

union U66 { int i; short h; };

void OvlFunc_969_200cbec(void)
{
    int flag = 0;
    unsigned char *p;
    unsigned char *e;
    unsigned char *q;
    unsigned char *r;
    unsigned char *sc;
    unsigned char *g1;
    unsigned char *g2;
    int zero;
    int h5000;
    int h8000;
    int a6000;
    int a4000;
    int dm;
    int dy;
    int df;
    int dh;
    int t12;
    int t17;
    int t14;
    int t8;
    int ten;
    int s1;
    int s2;

    { PIN2; q0 = 0x282e; __MessageID(q0); }
    { PIN2; q0 = 0x15; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 0x2015; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    __Func_80925cc(6, 2);
    __Func_8093040(6, 0, 0x14);
    __MapActor_SetAnim(6, 6);
    __CutsceneWait(0xa);
    __Func_809259c(0x15, 2);
    { PIN3; q0 = 0x2015; q1 = 0; q2 = 0x28; __Func_8093040(q0, q1, q2); }
    __MapActor_SetAnim(6, 7);
    __Func_8093040(6, 0, 0x14);
    __PlaySound(0x11);
    __Func_80925cc(6, 2);
    __MapActor_SetBehavior(6, gScript_969__0200e324);
    __CutsceneWait(0x14);
    { PIN2; q0 = 0x15; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __Func_809259c(0x15, 3);
    __MapActor_WaitScript(6);
    __CutsceneWait(0xa0);
    __Func_80925cc(0x15, 1);
    __CutsceneWait(0x14);
    g1 = __MapActor_GetActor(0x15);
    h5000 = 0xa0 << 7;
    *(short *)(g1 + 6) = h5000;
    __MapActor_SetAnim(0x15, 0);
    __CutsceneWait(0x50);
    __MapActor_DoAnim(0x15, 4);
    __CutsceneWait(0x28);
    __Func_8093040(0x15, 0, 0x28);
    __MapActor_SetAnim(0x15, 5);
    __CutsceneWait(0xa);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x15), 0);
    __MapActor_SetAnim(0x15, 0);
    __MapActor_Jump(0x15, 6, 0);
    { PIN3; q0 = 0x15; q1 = 0x30000; q2 = 0x18000; __MapActor_SetSpeed(q0, q1, q2); }
    e = __MapActor_GetActor(0x15) + 0x5a;
    *e = 0xfe & *e;
    p = __MapActor_GetActor(0x15);
    h8000 = 0x80 << 8;
    *(void **)(p + 0x6c) = (void *)OvlFunc_969_2008400;
    *(short *)(p + 6) = h8000;
    __Func_8092158(0x15, 0xb8, 0xed);
    __MapActor_RunScript(0x15, gScript_969__0200e360);
    __CutsceneWait(0x78);
    OvlFunc_969_2008894(0);
    __PlaySound(0x48);
    __Func_80933d4(0x80 << 11, h8000);
    { PIN4; q0 = 0x1560000; q1 = 0x200000; q2 = 0xd40000; q3 = 1; __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    __Func_80925cc(0, 2);
    __CutsceneWait(0x28);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
    __MapActor_SetAnim(0, 1);
    __MapActor_Jump(0, 6, 0x3c);
    g2 = __MapActor_GetActor(0);
    ten = 0xa;
    *(short *)(g2 + 0x64) = ten;
    __MapActor_SetBehavior(0, gScript_969__0200e074);
    { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0x50; __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(1, 0, 0x28);
    __Func_80925cc(2, 1);
    __CutsceneWait(0x14);
    OvlFunc_969_2008894(2);
    { PIN3; q0 = 3; q1 = 0x81 << 1; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    OvlFunc_969_2008894(3);
    __Actor_SetSpriteFlags(__MapActor_GetActor(1), 1);
    __MapActor_SetAnim(1, 1);
    __MapActor_Jump(1, 6, 0x3c);
    __Func_80925cc(2, 1);
    __CutsceneWait(0x14);
    __Actor_SetSpriteFlags(__MapActor_GetActor(2), 1);
    __MapActor_SetAnim(2, 1);
    __MapActor_Jump(2, 6, 0x28);
    __Actor_SetSpriteFlags(__MapActor_GetActor(3), 1);
    __MapActor_SetAnim(3, 1);
    __MapActor_Jump(3, 6, 0x3c);
    { PIN3; q0 = 1; q1 = 0x6000; q2 = 0x3c; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(1, 4);
    OvlFunc_969_2008894(1);
    { PIN3; q0 = 2; q1 = 0x6000; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    a6000 = 0xc0;
    a6000 <<= 7;
    __MapActor_SetAnim(2, 4);
    OvlFunc_969_2008894(2);
    OvlFunc_969_20088a8(3, a6000);
    a4000 = 0x80;
    a4000 <<= 7;
    __MapActor_SetAnim(3, 4);
    __Func_8093040(3, 0, 0x50);
    OvlFunc_969_20088a8(1, a4000);
    __Func_8093040(1, 0, 0x28);
    { PIN3; q0 = 2; q1 = 0xc000; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_8093040(2, 0, 0x28);
    __Func_809259c(1, 1);
    __Func_80925cc(2, 1);
    __CutsceneWait(0x14);
    __Func_8092adc(1, a6000, 0);
    __Func_8092adc(2, a6000, 0x14);
    __Func_80925cc(3, 1);
    { PIN3; q0 = 3; q1 = 0xe000; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    OvlFunc_969_2008894(3);
    { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0x14; __MapActor_Emote(q0, q1, q2); }
    OvlFunc_969_20088a8(1, a4000);
    OvlFunc_969_2008894(1);
    { PIN3; q0 = 2; q1 = 0xa000; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x19999; q2 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x14d; q2 = 0xc2; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x14d; q2 = 0xce; __Func_80921c4(q0, q1, q2); }
    __Func_8092adc(1, 0, 0x14);
    __Func_80925cc(1, 1);
    __Func_8093040(1, 0, 0x50);
    __MapActor_SetIdle(0);
    __WaitFrames(1);
    *(int *)(p + 0x18) = 0x10000;
    *(int *)(p + 0x1c) = 0x10000;
    __CutsceneWait(0x28);
    { PIN3; q0 = 0; q1 = 0x80 << 1; q2 = 0x50; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0x2000; q0 = 1; OvlFunc_969_20088a8(q0, q1); }
    __MapActor_Jump(1, 4, 0);
    OvlFunc_969_2008894(1);
    { PIN2; q1 = 0xe000; q0 = 1; OvlFunc_969_20088a8(q0, q1); }
    { PIN2; q1 = 0; q0 = 1; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __MapActor_DoAnim(1, 3);
        OvlFunc_969_2008894(1);
        __Func_80925cc(2, 1);
        OvlFunc_969_2008894(2);
        __MapActor_SetAnim(3, 4);
        OvlFunc_969_2008894(3);
        flag = 1;
    } else {
        *(unsigned short *)(iwram_3001ebc + 0x1d8) += 3;
        __CutsceneWait(0x3c);
        OvlFunc_969_2008894(1);
        __Func_80925cc(2, 1);
        OvlFunc_969_2008894(2);
        __MapActor_SetAnim(3, 4);
        OvlFunc_969_2008894(3);
    }
    if (flag != 0) {
        *(unsigned short *)(iwram_3001ebc + 0x1d8) += 3;
    }
    { PIN2; q1 = 0x4000; q0 = 0; OvlFunc_969_20088a8(q0, q1); }
    { PIN2; q0 = 0; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x50);
    __PlaySound(0x11);
    { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0x14; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xa000; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xe000; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x8000; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
    OvlFunc_969_2008894(2);
    { PIN3; q0 = 0; q1 = 0x6000; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x2000; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(1, 4);
    OvlFunc_969_2008894(1);
    { PIN3; q0 = 2; q1 = 0xc000; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x8000; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xa000; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc000; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 3; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    OvlFunc_969_2008894(3);
    __PlaySound(0x8d);
    { PIN3; q0 = 0x60000; q1 = 0x60000; q2 = 0x10000; __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x28);
    __PlaySound(0x91);
    t12 = 0x12;
    t17 = 0x17;
    __CopyMapTiles(0x6e, 0x69, 0x4a, 4, t12, t17);
    t8 = 8;
    __CopyMapTiles(0x5c, 0x56, 0x53, 4, t8, t17);
    __CopyMapTiles(0x4b, 0x1c, 0x4b, 4, t8, t17);
    t14 = 0x14;
    __CopyMapTiles(0x5c, 0x56, 0xb, 0x48, 0x10, t14);
    __CopyMapTiles(0x13, 0x5c, 0x13, 0x44, t8, 0x15);
    p = __MapActor_GetActor(0);
    ((union U66 *)(p + 0x10))->i += 0xffe00000;
    zero = 0;
    ((union U66 *)(p + 0x66))->h = zero;
    OvlFunc_969_200d688(p);
    p = __MapActor_GetActor(1);
    ((union U66 *)(p + 8))->i += 0xfffc0000;
    ((union U66 *)(p + 0x10))->i += 0xffe00000;
    ((union U66 *)(p + 0x66))->h = zero;
    OvlFunc_969_200d688(p);
    p = __MapActor_GetActor(2);
    ((union U66 *)(p + 8))->i += 0xfffc0000;
    ((union U66 *)(p + 0x10))->i += 0xffe00000;
    ((union U66 *)(p + 0x66))->h = zero;
    OvlFunc_969_200d688(p);
    p = __MapActor_GetActor(3);
    ((union U66 *)(p + 8))->i += 0xfffc0000;
    ((union U66 *)(p + 0x10))->i += 0xffee0000;
    ((union U66 *)(p + 0x66))->h = zero;
    OvlFunc_969_200d688(p);
    *(int *)(__MapActor_GetActor(0x17) + 0xc) = 0x380000;
    { PIN4; q0 = 0x1520000; q1 = 0x200000; q2 = 0xb40000; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    { PIN3; q0 = 0; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Jump(0, 6, 0);
    __MapActor_Jump(1, 6, 0);
    __MapActor_Jump(2, 6, 0);
    __MapActor_Jump(3, 6, 0);
    sc = gScript_969__0200e3c0;
    __MapActor_SetBehavior(0, sc);
    __MapActor_SetBehavior(1, sc);
    __MapActor_SetBehavior(2, sc);
    __MapActor_SetBehavior(3, sc);
    { PIN3; q0 = 0x30000; q1 = 0x30000; q2 = 0x10000; __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x50);
    { PIN3; q0 = 0x60000; q1 = 0x60000; q2 = 0x10000; __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x28);
    __PlaySound(0x91);
    s1 = 0xa;
    s2 = 5;
    __Func_8010704(0x6e, 0x6a, 0x12, 0xe, s1, s2);
    __CopyMapTiles(0x6e, 0x69, 0x4a, 4, t12, t17);
    __CopyMapTiles(0x5c, 0x56, 0xb, 0x44, 0x10, t14);
    p = __MapActor_GetActor(0);
    df = 0xfff00000;
    ((union U66 *)(p + 8))->i += df;
    OvlFunc_969_200d688(p);
    p = __MapActor_GetActor(1);
    ((union U66 *)(p + 8))->i += df;
    OvlFunc_969_200d688(p);
    p = __MapActor_GetActor(2);
    ((union U66 *)(p + 8))->i += df;
    OvlFunc_969_200d688(p);
    p = __MapActor_GetActor(3);
    ((union U66 *)(p + 8))->i += df;
    OvlFunc_969_200d688(p);
    p = __MapActor_GetActor(8);
    dh = 0x80;
    dh <<= 13;
    ((union U66 *)(p + 8))->i += dh;
    OvlFunc_969_200d688(p);
    p = __MapActor_GetActor(9);
    ((union U66 *)(p + 8))->i += dh;
    OvlFunc_969_200d688(p);
    p = __MapActor_GetActor(0xa);
    ((union U66 *)(p + 8))->i += 0xfff00000;
    OvlFunc_969_200d688(p);
    p = __MapActor_GetActor(0xb);
    ((union U66 *)(p + 8))->i += 0xfff00000;
    OvlFunc_969_200d688(p);
    { PIN4; q0 = 0x1420000; q1 = 0x200000; q2 = 0xb40000; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    { PIN2; q0 = 0; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 1; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 2; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q0 = 3; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __MapActor_Jump(0, 6, 0);
    __MapActor_Jump(1, 6, 0);
    __MapActor_Jump(2, 6, 0);
    __MapActor_Jump(3, 6, 0);
    { PIN3; q0 = -1; q1 = -1; q2 = 0xe666; __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x50);
    { PIN2; q0 = 0x121; __PlaySound(q0); }
    __CutsceneWait(0x28);
    __MapActor_SetIdle(0);
    __MapActor_SetIdle(1);
    __MapActor_SetIdle(2);
    __MapActor_SetIdle(3);
    __CutsceneWait(0x78);
    { PIN3; q0 = 2; q1 = 0x105; q2 = 0x78; __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(2, 1);
    { PIN3; q0 = 2; q1 = 0xa000; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    OvlFunc_969_2008894(2);
    __MapActor_DoAnim(1, 3);
    { PIN3; q0 = 1; q1 = 0x2000; q2 = 0; __Func_8092adc(q0, q1, q2); }
    OvlFunc_969_2008894(1);
    { PIN3; q0 = 3; q1 = 0xe000; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(3, 4);
    OvlFunc_969_2008894(3);
    __MapActor_Jump(2, 2, 0x14);
    OvlFunc_969_2008894(2);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    OvlFunc_969_20088a8(1, 0);
    OvlFunc_969_2008894(1);
    { PIN3; q0 = 0; q1 = 0x8000; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0x10000; q2 = 0x8000; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x10000; q2 = 0x8000; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x10000; q2 = 0x8000; __MapActor_SetSpeed(q0, q1, q2); }
    sc = gScript_969__0200e39c;
    __MapActor_SetBehavior(1, sc);
    __MapActor_SetBehavior(2, sc);
    __MapActor_SetBehavior(3, sc);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0; q1 = 0x10000; q2 = 0x8000; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x88 << 1; q2 = 0xd8; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x88 << 1; q2 = 0xfe; __Func_809218c(q0, q1, q2); }
    __CutsceneWait(0x50);
    q = iwram_3001ebc;
    *(int *)(q + 0x1c0) = 0x201;
    *(int *)(q + 0x1c8) = 0x10;
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneWait(0x50);
    __Func_800c5b4();
    r = *(unsigned char **)((int)&iwram_3001ebc - 0x30);
    *(short *)(r + 0x12f4) = zero;
    *(short *)(r + 0x12f6) = zero;
    { PIN3; q0 = 0x284f; q1 = 0; q2 = 0; __Func_8019aa0(q0, q1, q2); }
    __Func_800c5fc();
    __CutsceneWait(0x50);
}
