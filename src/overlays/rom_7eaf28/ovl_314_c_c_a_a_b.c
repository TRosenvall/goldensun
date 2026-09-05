/* OvlFunc_960_2008c00 -- 0x02008c00, asm/overlays/rom_7eaf28/ovl_314_c_c_a_a.s
 *
 * An escape cutscene: read save byte 0x218 for the companion slot, fetch the
 * actor named by gState+0x1f4 and that companion, start the scene, fade to
 * black, then drift both actors' depth for sixty frames before the map
 * transition -- then pick one of two destination maps from the area id at
 * gState+0x1c0.  Call trace, not a characterisation.
 *
 * A NEAR-TWIN OF OvlFunc_881_200b84c (src/overlays/rom_77a7c8/ovl_30_c_c_a_c_a.c)
 * and that is how it was written.  The whole middle -- the -1 triple, the
 * PlaySound, the five field stores, the sixty-frame loop -- is that function's
 * body unchanged, and every lever recorded in its header transferred verbatim:
 *
 *  - DO NOT NAME THE LOOP-INVARIANT.  0x3333 written as a bare literal at both
 *    use sites pools once and loop.c hoists it; named as `int step = 0x3333;`
 *    gcc rematerialises it, never needs the fourth callee-saved register, and
 *    the function comes out FIVE LINES SHORT (89 against 94) because the r8
 *    save/restore pair vanishes from the prologue and epilogue.  Measured here
 *    at 93 of 94 differing -- the same shortfall the twin describes.
 *  - THE `-1` TRIPLE NEEDS FOUR PINS AND TWO BARRIERS.  n movs needing a given
 *    order need n-1 barriers.  Dropping either barrier costs 2; dropping the
 *    p0..p3 pins costs 75.
 *  - THE COUNTER DECREMENTS AFTER THE CALL.  The ROM's `mov r0,#1 / sub r5,#1 /
 *    bl __WaitFrames` reads as a decrement before the call and is not one.
 *  - The pooled `2` is `_CONST_2` and objcmp reports the documented
 *    RELOCATION-NAME phantom for it; `make compare` is the authority.
 *
 * WHAT IS NEW HERE, on top of the twin, is the head and the tail.
 *
 * THE gState POINTER IS LIVE ACROSS FOUR CALLS AND RE-READ, so it is one local
 * mutated IN PLACE, not two locals.  The ROM builds it as
 * `ldr r5,=gState / mov r1,#0xfa / lsl r1,#1 / add r5,r1` and comes back to it
 * with `ldr r0,[r5]` after __PlaySound.  Two spellings were measured:
 *
 *      g = gState; slot = (int *)(g + (0xfa << 1));   ->  add r6, r3, r1   (8)
 *      g = gState; g += 0xfa << 1;                    ->  add r5, r1       (0)
 *
 * The three-operand `add` is the tell that a second local is holding the base:
 * the mechanism is the same one docs/elevation.md records for the cprop lever
 * (a live re-read wants a register-asm local, an IN-PLACE UPDATE wants `m += k`),
 * and this is the in-place half of it.  The base must still be assigned to a
 * local first or symbol and offset fold into `=gState+500`.
 *
 * THE TAIL gState LOAD IS A SECOND LOCAL, and this is what the whole function
 * turned on.  r5 is recycled as the loop counter, so after the loop the ROM
 * reloads gState into r3.  Reusing `g` for that makes gcc keep ONE pointer
 * variable alive across the loop and the allocation rotates: g->r7, a->r6,
 * f->r5 against the ROM's g->r5, f->r6, a->r7 (16-18 differing).  A separate
 * `h` gives the two DISJOINT LIVE RANGES their own allocnos and every register
 * lands on the ROM's.
 *
 * NEW FINDING -- A PIN CAN BE A SYMPTOM OF A DIFFERENT DEFECT, AND MEASURE
 * LOAD-BEARING RIGHT UP TO THE MOMENT THE REAL DEFECT IS FIXED.  While `g` was
 * still doing double duty, `register unsigned char *g __asm__("r5")` took the
 * screen from 18 differing to 11 and looked like the fix for the allocation.
 * It was not: splitting out `h` fixed the allocation for the right reason, and
 * with `h` in place the r5 pin measures EXACTLY ZERO and does not ship.  The
 * ordinary minimisation discipline ("re-test greedily after every drop, then a
 * fixpoint pass") catches this, but only because the drop was re-tested AFTER
 * an unrelated later edit -- a pin measured against an older candidate is not
 * evidence about the current one.
 *
 * 0xa5 IS `_AREA_a5`.  `ldr r3, =0xa5 / cmp r2, r3` where thumb `cmp Rn,#imm8`
 * covers 0-255, so the pool load is the symbol tell.  The sibling park
 * src/non_matching/ovl_7eaf28/2008d24.c records the same read of the same
 * gState halfword against the same symbol.
 *
 * `mov r1,#0 / ldrsh r2,[r3,r1]` is not a source shape: thumb LDRSH has no
 * immediate-offset form, so the zero register is forced.  `short` (not
 * `unsigned short`) is what picks ldrsh over ldrh.
 *
 * The `&&` did NOT need splitting.  The compound condition lowers to the ROM's
 * two `bne` to one label as written; the goto-split spelling measures identical
 * (0), so the simpler one ships.  The two-way choice of 0x4d/0x1b does not go
 * branchless -- the ROM duplicates the __SetDestMap call and so does the C.
 *
 * MEASURED (94 lines unless noted; 0 = byte-identical):
 *   final                                                        0
 *   ... + goto-split tail instead of `&&`                        0   (identical)
 *   ... + `for (i = 0x3b; i >= 0; i--)` instead of do/while      0   (identical)
 *   ... + `register unsigned char *g __asm__("r5")`              0   (inert)
 *   ... + `p = a + 0x55; *p = z; p += 0xc; *p = one;` chain      0   (inert)
 *   ... + `one` named local for the two 1-stores                 0   (inert)
 *   drop one __asm__ barrier                                     2
 *   drop both __asm__ barriers                                   2
 *   two locals for the gState pointer (`g` and `slot`)           8
 *   ... + `register g __asm__("r5")` on top of that             11
 *   reuse `g` for the tail gState load                          16
 *   `g += k` in place but no separate tail local                18
 *   two locals + gState pointer built BEFORE __GetFlagByte     13
 *   drop the `z` named local (literal 0 at all three stores)    57   (95 lines)
 *   drop the p0..p3 register pins                               75   (92 lines)
 *   name the loop invariant `int step = 0x3333;`                93   (89 lines)
 *
 * VERDICT
 *   OK OvlFunc_960_2008c00 -- 228 bytes, 93 encodings and 18 relocations identical
 *   (objcmp against a copy of the reference with `=2`/`=0xa5` respelled as the
 *   symbols; against the raw asm/ path the only residue is the 2-encoding
 *   _CONST_2/_AREA_a5 relocation phantom.  Both refs select the same flags --
 *   plain -O2, no Makefile rule covers this TU.)
 *
 * LANDING NEEDS A SPLIT.  asm/overlays/rom_7eaf28/ovl_314_c_c_a_a.s holds TWO
 * functions: OvlFunc_960_2008b24 (lines 11-100, 83 instructions), which is
 * PARKED at src/non_matching/ovl_7eaf28/2008b24.c, and this one (lines 111-206).
 * The .o is named twice in overlays/rom_7eaf28/overlay.ld -- line 46 for
 * (.text) and line 58 for (.data) -- so BOTH section lines must be remapped for
 * the new piece even though the .s carries no .data.  No exports.s/imports.s
 * entry references either symbol.
 */
extern unsigned char gState[];
extern int _CONST_2;
extern int _AREA_a5;
extern int __GetFlagByte(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(int a, int f);
extern void __WaitFrames(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void __SetDestMap(int a, int b);

void OvlFunc_960_2008c00(void)
{
    unsigned char *g;
    unsigned char *a;
    unsigned char *b;
    unsigned char *h;
    int f;
    int z;
    int i;
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");
    register int p3 __asm__("r3");

    f = __GetFlagByte(0x86 << 2);
    g = gState;
    g += 0xfa << 1;
    a = __MapActor_GetActor(*(int *)g);
    b = __MapActor_GetActor(f);
    __CutsceneStart();
    p0 = 1;
    __asm__ volatile ("" : : "r" (p0));
    p1 = 1;
    __asm__ volatile ("" : : "r" (p1));
    p2 = 1;
    p2 = -p2;
    p3 = 0;
    p1 = -p1;
    p0 = -p0;
    __Func_80933f8(p0, p1, p2, p3);
    __PlaySound(0xdb);
    __Actor_SetSpriteFlags(*(int *)g, 0);
    z = 0;
    b[0x55] = z;
    a[0x55] = z;
    *(int *)(a + 0x28) = z;
    a[0x61] = 1;
    b[0x61] = 1;
    i = 0x3b;
    do {
        *(int *)(a + 0x28) += 0x3333;
        *(int *)(b + 0x28) += 0x3333;
        __WaitFrames(1);
        i--;
    } while (i >= 0);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
    __SetFlag(0x91 << 1);
    h = gState;
    if (*(short *)(h + (0xe0 << 1)) == (int)(&_AREA_a5)
        && __GetFlagByte(0x86 << 2) == 0xb)
        __SetDestMap((int)&_CONST_2, 0x4d);
    else
        __SetDestMap((int)&_CONST_2, 0x1b);
}
