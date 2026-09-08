// fakematch
/* ovl_30_c_c_c_a_c_c_c_a_c_c_c_b.c  --  OvlFunc_882_2008a10
 *
 *   OK OvlFunc_882_2008a10 -- 844 bytes, 321 encodings and 84 relocations
 *   identical.  Re-measured three times on the 26-pin set and three more on
 *   the width-narrowed set that ships.
 *
 * LANDING SHAPE -- NEEDS A SPLIT.  asm/overlays/rom_77dd1c/
 * ovl_30_c_c_c_a_c_c_c_a_c_c_c.s carries THREE .thumb_func_starts
 * (OvlFunc_882_2008434, OvlFunc_882_2008a10, OvlFunc_882_2008d5c) and only the
 * middle one is closed here, so `python3 tools/asmfacts.py` on that path
 * reports "3 functions  split first".  Run
 *
 *     python3 tools/split_s.py \
 *       asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_c_c_c.s OvlFunc_882_2008a10
 *
 * which leaves the target in <stem>_b.s and rewrites overlay.ld:38 into the
 * three _a/_b/_c lines VERBATIM as asm/... .o(.text); this .c then replaces
 * <stem>_b.s.  makefile_flags() on this path is the EMPTY set (objcmp prints
 * no `built with:` line), so the rule that fires is `asm/%.o: src/%.c` at the
 * tree default -O2 -mthumb -mthumb-interwork -fcall-used-r4.  The .s carries
 * NO .section/.data/.bss/.lcomm/.word/.byte.
 *
 * ONE CROSS-FILE `.L` SYMBOL, ALREADY EXPORTED.  `__Func_8010560` is passed
 * `.L578a`, which lives in ovl_30_c_c_c_c_c_c_c_c_c.s and is ALREADY
 * `.global` there; the sibling src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_b.c
 * already reaches it as `extern unsigned char L578a[] __asm__(".L578a");` and
 * that spelling is reused verbatim.  Nothing new needs exporting.
 *
 * hi = 0, hiv = 0, and the FIRST PLAIN TRANSCRIPTION IS +36 BYTES WITH A WIDER
 * PUSH MASK -- `push {r5, r6, lr}` plus four high registers against the ROM's
 * `push {r5, lr}`.  That is the recorded "a push mask wider than the ROM's is
 * a COMMONING tell" at its loudest: gcc hoisted 0xcccc, 0x6666, 0x37a0000,
 * 0x389, 0xab<<17, 0xab<<1 and 0x101 into r5/r6/r8/sl and reused them, where
 * the ROM rebuilds every one at every site.  The relocation LIST was already
 * exactly right (84 symbols, same order) on that first transcription -- only
 * the offsets moved -- which is what said the transcription was sound and the
 * whole residue was eviction.
 *
 * THE LADDER, in the order the mechanism sizes said to try it:
 *
 *   plain C, no pins                                       319  (+36 bytes)
 *   all 72 call sites pinned                                 9  (exact size,
 *                                                               relocs differ)
 *   + `register int v __asm__("r5")` and `v += 8;`            6
 *   + a barrier before `v = 0xe5c;`                           2  (relocs SILENT)
 *   + the one 0x3c __Func_8092adc site filled DESCENDING      0
 *   minimised 72 -> 26 pins, then narrowed by width           0
 *
 * THE HELD MESSAGE BASE IS THE cprop CASE, NOT THE use_related_value CASE.
 * The ROM loads 0xe5c into r5, passes it to __MessageID, and 55 instructions
 * later does a bare `adds r5, #8` before passing it again.  A plain `int v`
 * with `v += 8` is NOT enough here even though the recorded rule says a
 * REDEFINED-in-place base survives cprop: gcse folds v to a constant, cse then
 * finds r5 still live and emits `adds r0, r5, #0 / adds r0, #8` -- the
 * destructive form never appears.  `register int v __asm__("r5")` plus the
 * `v += 8;` STATEMENT (not `v + 8` at the call) is what produces `adds r5, #8`.
 * NEW, and it sharpens "THE cprop HAZARD IS SCOPED TO A RE-READ": the
 * redefined-in-place exemption does NOT hold when the redefinition is 55
 * instructions and ~30 calls away from the definition, because by then the
 * value has been spilled into a callee-saved register that cse can see.  The
 * discriminator is DISTANCE, not shape.
 *
 * THE TWO __MessageID(v) SITES MUST STAY BARE.  Pinning them (PIN1 on q0 = v)
 * is 9 differing against 6: `mov r0, r5` is what a bare call already emits and
 * the pin adds nothing but a chance for the scheduler to move the ldr.
 *
 * A SOURCE-LEVEL BARRIER IS REQUIRED, AND `do { } while (0)` IS ENOUGH.  With
 * everything else right the `ldr r5, =0xe5c` sits FIVE INSTRUCTIONS TOO EARLY,
 * hoisted by sched2 over the whole __Func_8092adc(9, 0xc0 << 8, 0x3c) call
 * setup; relocations differ because the pool moves with it.  `do { } while (0)`
 * immediately before `v = 0xe5c;` is exact and `__asm__ volatile ("")` in the
 * same place is exact too -- they are INTERCHANGEABLE here, so the pure-C form
 * ships.  Removing it is 6 differing.  Worth recording because the barrier
 * budget entries all use the asm form: for a plain ORDERING barrier the empty
 * statement is as good and costs no fakematch.
 *
 * ONE SITE WANTS A DESCENDING FILL, AND IT IS THE LAST TWO BYTES.
 * __Func_8092adc(9, 0xc0 << 8, 0x3c) is `mov r1 / mov r2 / lsl r1 / mov r0` in
 * the ROM -- the r0 seed LAST, the recorded width-zero shape -- but here the
 * site MUST be pinned (dropping the pin is 204 differing and FOUR BYTES SHORT)
 * and the pin must be filled q2, q1, q0.  All six permutations were measured:
 * 210 is 0, 120 and 201 are 2, 012 is 2, 021 and 102 are 3.  NEW: the
 * width-zero "leave it bare" rule and the descending-fill rule are the same
 * observation seen from two sides -- when the site has to be pinned for
 * eviction reasons, spell the fill in the ROM's own materialisation order
 * rather than dropping the pin.
 *
 * PIN MINIMISATION AGREED IN BOTH DIRECTIONS, MEMBERSHIP INCLUDED.  Forward
 * and reverse drop-to-fixpoint both land on the SAME 26 sites of 70 (sites 32
 * and 60, the two __MessageID(v) calls, are excluded from the pinnable set).
 * That is the opposite outcome from the sibling ovl_30_c_a_c_c_c_a_a.c, where
 * the two directions agreed on cardinality and disagreed on membership; so
 * "agreement in size is not proof the set is canonical" stands, but agreement
 * in BOTH is reachable and is worth checking before shipping a set.  Width
 * narrowing then dropped 13 of the 26 to PIN1 or PIN2.
 *
 * MEASURED WORSE / INERT (against 321 encodings / 844 bytes):
 *
 *   spelling                                             differing
 *   --------------------------------------------------  ---------
 *   plain C, no pins                                          319  (+36 bytes)
 *   all 72 sites pinned                                         9
 *   ... with `v += 8;` but a plain `int v`                      9
 *   ... with `register int v __asm__("r5")`, sites 32/60 bare   6
 *   ... + barrier, 0x3c site filled ASCENDING                   2
 *   dropping the 0x3c site's pin entirely                     204  (-8 bytes)
 *   dropping the barrier                                        6
 *   `__asm__ volatile ("")` instead of `do { } while (0)`        0  (INERT)
 *
 * Harness: scratch_elev/b255/a3 -- gen.py (site table + pin/width/fill
 * generator), minimise.py (drop-to-fixpoint, fwd/rev), narrow.py (width pass),
 * dis.py + d.sh (side-by-side objdump diff), r.sh (one-line objcmp).
 * FAKEMATCH: register-pin idiom, so the name goes in fakematch.txt.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __StartRain(void);
extern void __StartThunder(void);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __PlaySound(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8010560(void *s, int a, int b);
extern void __Func_8095240(void);
extern void __Func_8095268(void);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __Func_809202c(void);
extern void __Func_809218c(int slot, int x, int y);
extern void __Func_80921c4(int slot, int x, int y);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern int __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern int __Func_8091c7c(int a, int b);
extern unsigned char *iwram_3001ebc;
extern unsigned char L578a[] __asm__(".L578a");

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_882_2008a10(void)
{
    unsigned char *p;
    register int v __asm__("r5");

    __CutsceneStart();
    __StartRain();
    __StartThunder();
    __Func_8095240();
    __WaitFrames(0x3c);
    { PIN1; q0 = 0x80 << 7;
      __Func_80933d4(q0, 0x80 << 4); }
    __Func_80933f8(0x9e << 17, 0xa0 << 16, 0xdc << 18, 1);
    { PIN2; q0 = 0xa; q1 = 0x93 << 17;
      __MapActor_SetPos(q0, q1, 0xd9 << 18); }
    __MapActor_SetPos(0, 0, 0);
    p = iwram_3001ebc;
    *(int *)(p + 0x1c0) = 0x100;
    *(int *)(p + 0x1c8) = 0x10;
    __MapTransitionIn();
    __WaitMapTransition();
    __Func_8095268();
    __PlaySound(0x9e);
    __Func_8010560((void *)L578a, 0x32, 0x2c);
    { PIN2; q0 = 0x16; q1 = 0x101;
      __MapActor_Surprise(q0, q1); }
    { PIN3; q0 = 9; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN2; q0 = 0xa; q1 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, 0x6666); }
    { PIN3; q0 = 9; q1 = 0xab << 17; q2 = 0x37a0000;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xab << 1; q2 = 0x389;
      __Func_80921c4(q0, q1, q2); }
    __Func_809202c();
    { PIN3; q0 = 9; q1 = 0x94 << 1; q2 = 0x389;
      __Func_809218c(q0, q1, q2); }
    { PIN2; q0 = 0; q1 = 0xab << 17;
      __MapActor_SetPos(q0, q1, 0x37a0000); }
    { PIN2; q0 = 0; q1 = 0xab << 1;
      __Func_809218c(q0, q1, 0x37a); }
    { PIN3; q0 = 0; q1 = 0xab << 1; q2 = 0x389;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x9f << 1; q2 = 0x389;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(9, 1);
    __Func_80925cc(9, 1);
    { PIN3; q2 = 0x3c; q1 = 0xc0 << 8; q0 = 9;
      __Func_8092adc(q0, q1, q2); }
    do { } while (0);
    v = 0xe5c;
    __MessageID(v);
    __ActorMessage(9, 0);
    __Func_80921c4(0xa, 0x93 << 1, 0x346);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0xa, 4);
    __ActorMessage(0xa, 0);
    __Func_8092848(0, 9, 0);
    __CutsceneWait(0x28);
    { PIN1; q0 = 0xa;
      __Func_8092adc(q0, 0x80 << 7, 0); }
    __Func_8093040(0xa, 0, 0x14);
    { PIN1; q0 = 9;
      __MapActor_Emote(q0, 0x101, 0x14); }
    { PIN2; q0 = 9; q1 = 0xc0 << 8;
      __Func_8092adc(q0, q1, 0xa); }
    __Func_8093040(9, 0, 0xa);
    __MapActor_DoAnim(0xa, 4);
    __ActorMessage(0xa, 0);
    __MapActor_Surprise(9, 0x81 << 1);
    __CutsceneWait(0x1e);
    __Func_8092adc(9, 0, 0x32);
    { PIN2; q0 = 9; q1 = 0xc0 << 8;
      __Func_8092adc(q0, q1, 0xa); }
    { PIN3; q0 = 9; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN2; q0 = 9; q1 = 0x121;
      __Func_80921c4(q0, q1, 0x373); }
    { PIN1; q0 = 9;
      __Func_8092adc(q0, 0xe0 << 8, 0); }
    __ActorMessage(9, 0);
    __Func_80925cc(0xa, 2);
    __ActorMessage(0xa, 0);
    __MapActor_DoAnim(9, 4);
    __Func_8093040(9, 0, 0xa);
    __Func_8092adc(9, 0x80 << 6, 0xa);
    v += 8;
    __MessageID(v);
    __Func_8092c40(9, 0);
    { PIN2; q0 = 0; q1 = 0x97 << 1;
      __Func_80921c4(q0, q1, 0x389); }
    { PIN1; q0 = 0;
      __Func_8092adc(q0, 0xc0 << 8, 0); }
    while (__Func_8091c7c(0, 0) == 1) {
        __Func_80925cc(9, 1);
        __MessageID(0xe65);
        __Func_8092c40(9, 0);
    }
    __MapActor_DoAnim(9, 3);
    __MessageID(0xe66);
    __Func_8093040(9, 0, 0xa);
    __MapActor_DoAnim(0, 3);
    { PIN2; q0 = 0xa; q1 = 0xc0 << 9;
      __MapActor_SetSpeed(q0, q1, 0xc0 << 8); }
    { PIN3; q0 = 0xa; q1 = 0x129; q2 = 0x2ee;
      __Func_809218c(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN2; q0 = 9; q1 = 0x129;
      __Func_80921c4(q0, q1, 0x2ee); }
    __MapActor_SetPos(9, 0, 0);
    __MapActor_SetPos(0xa, 0, 0);
    __MapActor_SetAnim(0xa, 1);
    __MapActor_SetAnim(0x15, 2);
    __MapActor_SetAnim(0x16, 5);
    __ClearFlag(0x12f);
    __SetFlag(0x87b);
    __SetFlag(0x205);
    __CutsceneEnd();
}
