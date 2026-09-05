/* OvlFunc_955_20099bc -- asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c.s
 *
 * VERDICT: BYTE-EXACT.
 *   OK OvlFunc_955_20099bc -- 372 bytes, 151 encodings and 33 relocations identical
 * Verified against scratch ref.s AND against the original asm/ path; the two
 * agree, so no Makefile pattern rule is biting this unit (no -O1, no
 * no-sched2, no flag adjustment -- objcmp printed no "(built with: ...)" line).
 *
 * SHAPE.  push {r5,r6,r7,lr} + push {r7=r8}: a WIDE push, and by content a PIN
 * function -- r7 = the parameter `a`, r8 = the OvlFunc_common1_4cc result, r5
 * and r6 = the two carried coordinates 0xcc<<1 and 0x84<<1 (r5 is later updated
 * in place with `sub r5,#0x20`, so it is written `m -= 0x20;`, NOT re-read
 * through an __asm__ register -- the cprop rule: an in-place update needs no
 * hard register, only a live RE-READ does).
 *
 * WHERE IT STARTED.  The inherited best (cand4.c) was 8 differing of 151, not
 * the 11 of 153 the handoff recorded -- re-measure, never trust a carried
 * number.  All 8 were pure argument-fill ORDER, in three clusters, every one an
 * adjacent transposition of two seed `mov`s:
 *
 *   __Func_80933f8 #1  ref  r0, r1, r2 seeds   ours  r0, r2, r1
 *   __Func_80933d4 #2  ref  r0, r1           ours  r1, r0
 *   __Func_80933f8 #2  ref  r0, r1, r2 seeds   ours  r0, r2, r1
 *
 * THE LEVERS THAT SHIPPED, AND THEIR MECHANISM
 *
 * 1. PIN2 on __Func_80933f8 #1, whole-value statements (`q0 = 0xa4 << 17;
 *    q1 = -1;`).  This is the one-statement pinned fill: each large constant
 *    splits into mov+shift only AFTER expand, so every seed mov acquires a
 *    dependent and sits at sched depth 2 while the shifts and the `neg` sit at
 *    depth 1.  sched2 takes the depth-2 class first and breaks ties by insn
 *    order = argument order, so the ROM's r0,r1,r2 falls out with no barrier.
 *    (Documented under "A ONE-STATEMENT PINNED FILL CAN REPLACE A SCHEDULING
 *    BARRIER"; the ROM here puts every lsl AFTER the seed group, which is
 *    exactly the case that form expresses.)
 *
 * 2. PIN1 on __Func_80933d4 #2 (`q0 = 0xc0 << 9;`, second argument left bare).
 *    Writing r0's value straight into the hard register destroys the pseudo
 *    that sched2 was otherwise free to order after r1's.  PIN2 here measures
 *    identical, so PIN1 is the minimal spelling and PIN2 would be scaffolding.
 *    The FIRST __Func_80933d4 call needs no pin at all -- same callee, same
 *    shape, opposite requirement.
 *
 * 3. __Func_80933f8 #2 needs NO PIN.  It was 2 differing on its own, and
 *    pinning site #1 CURED IT: the two sites share the `-1`, and with the first
 *    use written straight into r1 there is no pseudo left for CSE to hand the
 *    second site, which then rebuilds mov#1/neg in place.  This is "ONE PIN AT
 *    THE FIRST USE COVERS THE LATER ONES", and it is why the shipped set is two
 *    pins and not three.
 *
 * 4. Named locals s1,s2,t1,p1,p2 assigned in the DOMINATING block, above the
 *    `if (r == 0)`.  This is the argument-order/rematerialisation lever: gcc
 *    will not keep the constants live across the guard, so it rebuilds each at
 *    its use and the rebuilt sequence interleaves the ROM's way.  The guard is
 *    load-bearing -- these are not decoration, see the table below.
 *
 * 5. THREE CALLEES ARE DELIBERATELY LEFT IMPLICIT: OvlFunc_common1_1078,
 *    OvlFunc_common1_15b8 and OvlFunc_common1_5e4.  Everything else is fully
 *    prototyped.  Prototype presence is a per-site lever in both directions and
 *    it must be measured per callee; adding these three costs 2, 8 and 3
 *    respectively, and all three together cost 13 (8 -> 21).  Conversely
 *    REMOVING the prototype of __Func_80933d4, __Func_80933f8 or __Func_8092adc
 *    costs 2 each -- the no-prototype lever points the WRONG WAY here.
 *
 * MEASURED-WORSE SPELLINGS (differing of 151; the shipped file is 0)
 *
 *   spelling                                                         differing
 *   ----------------------------------------------------------------  -------
 *   SHIPPED (final.c)                                                       0
 *   drop the __Func_80933f8 #2 pin ......... (already dropped; inert)       0
 *   PIN3 or PIN4 instead of PIN2 at __Func_80933f8 #1 ...............       0
 *   PIN2 instead of PIN1 at __Func_80933d4 #2 ......................       0
 *   named local `e = -1` instead of the literal ....................       0
 *   cand4.c, the inherited best -- no pins .........................       8
 *   drop the __Func_80933d4 #2 pin .................................       4
 *   drop the __Func_80933f8 #1 pin .................................      21
 *   PIN1 (q0 only) at __Func_80933f8 #1 ............................      20
 *   PIN1 (q0 only) at both __Func_80933f8 sites ....................      20
 *   drop t1 (bare 0x80 << 8 at __Func_8092adc) .....................       2
 *   drop p1,p2 (bare literals at __MapActor_SetPos) ................       2
 *   drop s1,s2 (bare literals at __MapActor_SetSpeed) . 157 insns vs 151  144
 *   drop m,n (bare literals at OvlFunc_common1_1078) .. 145 insns vs 151  136
 *   drop the `g` local, deref gState directly ......... 149 insns vs 151  141
 *   prototype OvlFunc_common1_1078 .................................       2
 *   prototype OvlFunc_common1_15b8 .................................       8
 *   prototype OvlFunc_common1_5e4 ..................................       3
 *   all three of the above prototyped ..............................      21
 *   drop __Func_80933d4's prototype ................................      10
 *   drop __Func_80933f8's prototype ................................      10
 *   drop __Func_8092adc's prototype ................................      10
 *   drop both __Func_80933d4 and __Func_80933f8 prototypes .........      12
 *
 * MINIMISED.  Greedy strip-one-and-retest to fixpoint, four rounds, each drop
 * re-measured under objcmp on THIS source (never against a saved index list).
 * Round 3 removed the third pin only because round 2 had reduced the first
 * from PIN4 to PIN2 -- "N pins is a size, not a set" in miniature.  Nothing
 * inert survives: every one of the 8 constructs above is load-bearing.
 *
 * NEW FINDING -- A PARTIAL PIN CAN BE WORSE THAN NO PIN AT ALL.
 * Pinning only q0 at __Func_80933f8 #1 is 20 differing; pinning NOTHING there
 * is 21 but with a completely different failure, and pinning nothing at all
 * anywhere (cand4) is 8.  The q0-only pin is not a partial win, it is a
 * regression of a different kind: with r0's value forced into the hard register
 * the block's pressure drops far enough that CSE's `-1` pseudo now survives
 * allocation into a CALLEE-SAVED register, and the ROM's per-site
 * `mov r1,#1 / neg r1,r1` becomes `mov r5,#1 / neg r5,r5 / ... / add r1,r5,#0`
 * -- one extra instruction, and every following relocation shifts.  So a pin
 * set must reach THROUGH any constant the site shares with a later site; a pin
 * that stops short of it hands that constant to CSE.  The existing rules ("pin
 * the first use", "one pin at the first use covers the later ones") are about
 * WHICH SITE to pin; this is about HOW FAR ALONG THE ARGUMENT LIST to pin, and
 * it is not currently in docs/elevation.md.
 *
 * LANDING NEEDS A SPLIT (3-way).  asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c.s
 * holds, in order: OvlFunc_955_2009898 (line 13, still asm, 123 instructions),
 * OvlFunc_955_20099bc (line 146, this function), then `.section .data` with
 * .L40c0 and `.section .data1` with gOvl_0200c414 / gOvl_0200c474 /
 * gOvl_0200c48c / .L4834 / .L4838 / gOvl_0200c83c.  Following the precedent set
 * by commit 438ec2e4 (which produced this very file), split into:
 *     asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c_a.s   OvlFunc_955_2009898
 *     src/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c_b.c   this function
 *     asm/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c_c.s   the .data/.data1 blobs
 * overlays/rom_7ddb88/overlay.ld line 55 becomes three .text lines (_a, _b, _c
 * in that order); line 97 (.data) and line 106 (.data1) retarget from
 * ovl_30_c_c_c_c_c_c_c.o to ovl_30_c_c_c_c_c_c_c_c.o.
 * Register in fakematch.txt: 2 pins.
 */

extern unsigned char gState[];

extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __SetCameraTarget(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8092adc(int a, int b, int c);
extern int OvlFunc_common1_4cc(int a, int b);
extern void OvlFunc_common1_2c4(void);
extern void OvlFunc_common1_1254(int a);
extern void OvlFunc_common1_588(int a, int b);
extern void OvlFunc_955_2009898(int a, int b, int c);
/* OvlFunc_common1_1078, OvlFunc_common1_15b8 and OvlFunc_common1_5e4 are
   deliberately NOT declared -- see the write-up above. */

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")

void OvlFunc_955_20099bc(int a)
{
    unsigned char *g;
    int r;
    int m, n;
    int s1, s2;
    int t1, p1, p2;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
        return;
    }
    __CutsceneStart();
    r = OvlFunc_common1_4cc(a, 5);
    s1 = 0xc0 << 9;
    s2 = 0xc0 << 8;
    t1 = 0x80 << 8;
    p1 = 0xc4 << 17;
    p2 = 0xd0 << 16;
    if (r == 0) {
        __MessageID(0x20ae);
        __Func_80933d4(0x80 << 10, 0x80 << 7);
        { PIN2; q0 = 0xa4 << 17; q1 = -1;
          __Func_80933f8(q0, q1, 0x84 << 17, 1); }
        __Func_8093530();
        __CutsceneWait(0x1e);
        { PIN1; q0 = 0xc0 << 9;
          __Func_80933d4(q0, 0xc0 << 6); }
        __Func_80933f8(0x9c << 17, -1, 0xb0 << 16, 1);
        m = 0xcc << 1;
        n = 0x84 << 1;
        __Func_8093530();
        __ActorMessage(a, 0);
        OvlFunc_common1_1078(0, m, n);
        __MapActor_SetSpeed(0, 0xc0 << 9, 0xc0 << 8);
        OvlFunc_common1_15b8(0, m, 0xd8);
        __Func_8092adc(0, t1, 0xa);
        __ActorMessage(a, 0);
        OvlFunc_955_2009898(0x10, 0xb4 << 1, 0xd0);
        __MapActor_Emote(0, n, 0x2d);
        __MapActor_SetSpeed(0, s1, s2);
        m -= 0x20;
        OvlFunc_common1_15b8(0, m, 0xd8);
        OvlFunc_common1_15b8(0, m, 0xf8);
        OvlFunc_common1_15b8(0, 0x9c << 1, 0xf8);
        __ActorMessage(a, 0);
        OvlFunc_common1_1254(0);
        __SetCameraTarget(0, 0);
        __MapActor_SetPos(0x10, p1, p2);
        OvlFunc_common1_588(a, 5);
    } else if (r == 1) {
        __MessageID(0x20ad);
        __ActorMessage(a, 0);
    }
    OvlFunc_common1_5e4(r, a, 5);
    __CutsceneEnd();
}
