/* OvlFunc_971_20092e0 -- NON-MATCHING, 204 encodings of 418, size 1080 against the
 * ROM's 1060 (+20).  392 instructions.
 *
 * Blocker class: BASE-POINTER REMATERIALISATION COUNT.  The body is structurally
 * correct; the entire residue is a register permutation cascading from one fact.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7fb4a8/20092e0.c \
 *     asm/overlays/rom_7fb4a8/ovl_30_c_c_c_c_c.s
 *
 * THIS TARGET NEEDS A TEXT/DATA SPLIT REGARDLESS OF EXACTNESS, and the batch-281
 * brief got that wrong -- it called this a "single-function file" that "converts
 * whole".  It IS one function, but the .s carries a `.section .data` of six .incbin
 * blobs (CHAR_ARRAY_ARRAY_971__02009928, .L1940, gOvl_02009948, gOvl_020099f0,
 * .L19f4, gScript_887__02009c04, gOvl_02009e14) and a `.section .bss`
 * (.lcomm .L1f4c, .L1f50).  gcc cannot emit those, and
 * overlays/rom_7fb4a8/overlay.ld names this .o on THREE lines -- 44 .text, 53
 * .data, 60 .bss.  tools/datacheck.py reports all of this; run it during target
 * selection, not after.
 *
 * THE ROM MATERIALISES `ldr rX, =gState` FIVE TIMES WITH DISJOINT LIVE RANGES --
 * r3 at the top, r6 across the chain, r7 in the t==8 arm tail, r3 in the t==9 tail,
 * r3 in the function tail.  That is what lets r6 be shared by the loop counter,
 * then the base, then `n`.  Every spelling measured forces either ONE long-lived
 * base (which costs a fourth callee-saved register, r8) or, with five source
 * variables, cse2 commons them back together.
 *
 * MEASURED, all objcmp against ref 418 encodings / 1060 bytes:
 *   one base     422 enc / 1072 B
 *   two bases    423 / 1076
 *   five bases   430 / 1092
 *   inline cast  395 / 1016   (the offsets FOLD into the pool word)
 *
 * `gState` REACHED AS AN ARRAY OR STRUCT EXPRESSION FOLDS THE OFFSET INTO THE POOL
 * WORD (`ldr r3, =gState+688`) and comes out 23 instructions short.  The tree's
 * `(unsigned int)&gState` + register-held-offset idiom
 * (src/overlays/rom_78ac38/ovl_30_c_c_b.c) is REQUIRED and reproduces the ROM's
 * `mov r2,#0xac / lsl r2,#2 / add r3,r3,r2` exactly -- verified with a four-way probe.
 *
 * PINNING 0xfa << 2 AND 0xfe << 2 AT THEIR NINE CALL SITES was the single largest
 * win, 74 -> 52 window-divergences: the ROM re-materialises them each time, gcc
 * commons them into r5, and r5 is where a register goes.
 *
 * FLAGS ARE NOT THE ANSWER AND NO ROW SHOULD BE ADDED ON THIS EVIDENCE.
 * -fno-gcse inert; -fno-rerun-cse-after-loop improves the base-commoning but is
 * WORSE OVERALL at every base count (362 / 313 / 295 / 419 window-divergences at
 * 5 / 6 / 7 / 8 bases).
 *
 * ================================================================
 * A NEW HImode SUB-CASE THAT RUNS OPPOSITE TO BATCH 280'S RULE, WITHIN ONE FUNCTION
 * ================================================================
 *
 * The four ewram_2002224 halfword stores of 0x54/0x41/0x4c/0x4b are BARE LITERALS
 * and correctly pool -- which is exactly why the ROM has a mid-body pool with a `b`
 * over it, at pool_range 64.  But the halfword stores of 0 and 2 are `mov r2,#0` /
 * `mov r3,#2` in the ROM, and a bare literal POOLS them; routing those through an
 * `int` local gives the `mov`.
 *
 * SO WITHIN ONE FUNCTION THE BARE LITERAL IS RIGHT FOR SOME HALFWORD CONSTANT
 * STORES AND AN `int` LOCAL FOR OTHERS, AND THE DISCRIMINATOR IS NOT MAGNITUDE --
 * 0x54 is 84, also below 256.  *thumb_movhi_insn alternative 5 (`l` <- `I`) prints
 * `mov` and alternative 1 (`l` <- `mn`) pools, and the Thumb movhi expander
 * force_reg's the constant for a memory destination.  WHICH ALTERNATIVE RELOAD
 * PICKS WAS NOT DETERMINED.  This qualifies batch 280's pooled-zero entry and the
 * mirror case in src/overlays/rom_794ac0/ovl_30_a_c_c_c_c_c_a.c: the rule is not
 * "bare literal for halfword stores" in either direction.
 *
 * Applying that fix takes 424 -> 419 encodings and 1080 -> 1064 bytes -- clearly the
 * right direction -- but raises window-divergence 32 -> 62 through a register
 * cascade.  Both states are kept in scratch: 92e0_BEST_ACD.c (this file, the
 * cleaner window) and 92e0_12.c (the closer OBJECT, 338 of 418 at 1064 bytes).
 *
 * No symbol tells: all 418 relocations are iwram_3001ebc, gState, ewram_2002224,
 * .L1f4c, .L1f50 and the calls.  No per-file Makefile flag override exists for this
 * stem (verified by grep).
 *
 * NEXT: the five-rematerialisation problem is the whole function.  It wants a
 * reading of why cse2 commons five source-level bases, since that -- not the pin
 * count and not the HImode sub-case -- is what costs the fourth callee-saved
 * register.  Do the text/data split first so the object can actually be landed if
 * it closes.
 */
extern int L1f4c __asm__(".L1f4c");
extern int L1f50 __asm__(".L1f50");
extern unsigned char *iwram_3001ebc;
extern unsigned char iwram_3001d08;
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char ewram_2002224[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern int __GetFlag(int id);
extern int __GetFlagByte(int id);
extern void __SetFlagByte(int id, int v);
extern void __SetSoundFXMode(int mode);
extern void __ActorMessage(int a, int b);
extern int __StartTask(void (*fn)(void), int n);
extern void __Func_8004358(void (*fn)(void), int a);
extern void __Func_8005d10(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80118c0(int n);
extern void __Func_8019908(int a, int b);
extern void __Func_807808c(int a);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80bf65c(void);
extern void OvlFunc_971_200803c(void);
extern void OvlFunc_971_2008128(int n);
extern void OvlFunc_971_2008148(void);
extern int OvlFunc_971_2008f30(int n);
extern void OvlFunc_971_20091bc(void);
extern void OvlFunc_971_2009228(void);
extern void OvlFunc_971_2009294(int n);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")

int OvlFunc_971_20092e0(void)
{
    int i;
    int t;
    int n, m;
    int w;
    unsigned short *p, *q;
    unsigned char *b;
    unsigned int gs, gm, g2, g3, g4, e;
    void (*f)(void);

    L1f50 = 0;
    L1f4c = 0;
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201;
    __SetSoundFXMode(0x2);
    gs = (unsigned int)&gState;
    OvlFunc_971_2009294(*(unsigned short *)(gs + (0xac << 2)));
    { int v1 = 0xd; int v2 = 0xa; __Func_8010704(0xb, 0xb, 0x1, 0x1, v1, v2); }
    OvlFunc_971_2008128(0x4);
    __WaitFrames(0x1);
    __Func_80118c0(0x5);
    e = (unsigned int)ewram_2002224;
    *(unsigned short *)(e + 0x8) = 0x54;
    *(unsigned short *)(e + 0xa) = 0x41;
    *(unsigned short *)(e + 0xc) = 0x4c;
    *(unsigned short *)(e + 0xe) = 0x4b;
    for (i = 0; i <= 7; i++) {
        __ClearFlag(i + (0xbc << 2));
        if (OvlFunc_971_2008f30(i) != 0)
            __SetFlag(i + (0xbc << 2));
    }
    gm = (unsigned int)&gState;
    t = *(short *)(gm + (0xe1 << 1));
    if (t == 8) {
        __CutsceneStart();
        __MapTransitionIn();
        __WaitMapTransition();
        OvlFunc_971_2008128(0x5);
        *(unsigned short *)(gm + (0xa9 << 2)) += 1;
        *(unsigned short *)(gm + 0x2aa) += 1;
        { PIN1; q0 = 0xfe; q0 <<= 2; n = (signed char)__GetFlagByte(q0); }
        m = (n << 1) + 2;
        if (m > 0xe)
            m = 0xe;
        { PIN1; q0 = 0xfa; q0 <<= 2; w = __GetFlagByte(q0); }
        if (w == 2) {
            { PIN2; q0 = 0xfa; q0 <<= 2; q1 = 0; __SetFlagByte(q0, q1); }
            n += 1;
            m += 1;
        } else {
            { PIN2; q1 = w + 1; q0 = 0xfa; q0 <<= 2; __SetFlagByte(q0, q1); }
        }
        g2 = (unsigned int)&gState;
        __Func_809280c(0x8, *(int *)(g2 + (0xfa << 1)), 0);
        __MessageID(0x293e + m);
        { PIN2; q1 = 0; q0 = 0x8; __Func_8092c40(q0, q1); }
        if (__Func_8091c7c(0, 0) == 0) {
            if (n > 0x5a)
                n = 0x5a;
            { PIN2; q0 = 0xfe; q0 <<= 2; q1 = n; __SetFlagByte(q0, q1); }
        } else {
            __ClearFlag(0x173);
            { PIN2; q0 = 0xfe; q1 = 1; q0 <<= 2; q1 = -q1; __SetFlagByte(q0, q1); }
            p = (unsigned short *)(g2 + 0x2aa);
            __Func_8019908(*p, 0x5);
            q = (unsigned short *)(g2 + (0xaa << 2));
            if (*q < *p) {
                *q = *p;
                __MessageID(0x293c);
                { PIN2; q1 = 0; q0 = 0x8; __Func_8092c40(q0, q1); }
                OvlFunc_971_2009228();
            } else {
                __MessageID(0x2939);
                __Func_8092c40(0x8, 0);
            }
            OvlFunc_971_2008128(0);
        }
        __CutsceneEnd();
    } else if (t == 9) {
        *(unsigned short *)(gm + 0x2a6) += 1;
        __CutsceneStart();
        __MapTransitionIn();
        __WaitMapTransition();
        OvlFunc_971_2008128(0x5);
        __Func_809280c(0x8, *(int *)(gm + (0xfa << 1)), 0);
        p = (unsigned short *)(gm + 0x2aa);
        __Func_8019908(*p, 0x5);
        q = (unsigned short *)(gm + (0xaa << 2));
        if (*q < *p) {
            *q = *p;
            __MessageID(0x293c);
            { PIN2; q1 = 0; q0 = 0x8; __Func_8092c40(q0, q1); }
            OvlFunc_971_2009228();
        } else {
            __MessageID(0x293a);
            __Func_8092c40(0x8, 0);
        }
        g3 = (unsigned int)&gState;
        *(unsigned short *)(g3 + 0x2aa) = 0;
        __ClearFlag(0x173);
        { PIN2; q0 = 0xfe; q1 = 1; q0 <<= 2; q1 = -q1; __SetFlagByte(q0, q1); }
        OvlFunc_971_2008128(0);
        __CutsceneEnd();
    } else if (t == 0xa) {
        __CutsceneStart();
        __MapTransitionIn();
        __WaitMapTransition();
        OvlFunc_971_2008128(0);
        OvlFunc_971_2008128(0x4);
        { PIN1; q0 = 0xfa; q0 <<= 2; w = __GetFlag(q0); }
        if (w != 0) {
            b = iwram_3001ebc;
            { PIN1; q0 = 0xfa; q0 <<= 2; __ClearFlag(q0); }
            *(unsigned short *)(b + (0xc1 << 1)) = 2;
            { PIN1; q0 = 0xc1; q0 <<= 2; __ClearFlag(q0); }
            __WaitFrames(0x14);
            OvlFunc_971_200803c();
            OvlFunc_971_2008128(0);
            OvlFunc_971_2008128(0x4);
        } else {
            *(unsigned short *)(gm + (0xab << 2)) += 1;
            p = (unsigned short *)(gm + 0x2b2);
            *p += 1;
            q = (unsigned short *)(gm + (0xac << 2));
            if (*q < *p)
                *q = *p;
            OvlFunc_971_2009294(*q);
            OvlFunc_971_20091bc();
            { PIN1; q0 = 0xc1; q0 <<= 2; __SetFlag(q0); }
            __SetFlag(0x305);
        }
        __CutsceneEnd();
    } else if (t == 0xb) {
        __CutsceneStart();
        __MapTransitionIn();
        __WaitMapTransition();
        OvlFunc_971_2008128(0);
        OvlFunc_971_2008128(0x4);
        if (__GetFlag(0x173) == 0) {
            *(unsigned short *)(gm + 0x2ae) += 1;
            *(unsigned short *)(gm + 0x2b2) = 0;
            OvlFunc_971_20091bc();
        }
        { PIN1; q0 = 0xc1; q0 <<= 2; __SetFlag(q0); }
        __ClearFlag(0x305);
        __CutsceneEnd();
    } else {
        __Func_8005d10();
        { PIN1; q0 = 0xb9; q0 <<= 1; __ClearFlag(q0); }
        { PIN2; q0 = 0xfe; q1 = 1; q0 <<= 2; q1 = -q1; __SetFlagByte(q0, q1); }
        b = (unsigned char *)(gm + 0x22a);
        if (*b != 0) {
            __CutsceneStart();
            __MapTransitionIn();
            __WaitMapTransition();
            __Func_809280c(0x8, *(int *)(gm + (0xfa << 1)), 0);
            __MessageID(0x2929);
            __ActorMessage(0x8, 0);
            __CutsceneEnd();
        }
        *b = 0;
        iwram_3001d08 = 0;
        OvlFunc_971_2008128(0);
        OvlFunc_971_2008128(0x4);
    }
    f = OvlFunc_971_2008148;
    __StartTask(f, 0xc8 << 4);
    __Func_8004358(f, 0x1);
    g4 = (unsigned int)&gState;
    if (*(short *)(g4 + (0xe1 << 1)) != 8 || __GetFlag(0x173) == 0) {
        __Func_807808c(0x1);
        __Func_80bf65c();
    }
    return 0;
}
