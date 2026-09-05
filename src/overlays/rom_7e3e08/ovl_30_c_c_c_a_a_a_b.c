/* OvlFunc_957_200ac44  --  0x0200ac44
 *   [asm/overlays/rom_7e3e08/ovl_30_c_c_c_a_a_a.s, 4th of 4 -- NEEDS A SPLIT]
 *
 * 809 instructions (821 halfwords, 2168 bytes with its two inline pools) of
 * straight-line cutscene script: 251 calls, one if/else on __Func_8091c7c, and
 * three copies of the fetch-actor-0/walk-to-it/park-it block at the end.
 * EXACT under objcmp with 51 pinned call sites, one carried message base and
 * nothing else.
 *
 *   OK OvlFunc_957_200ac44 -- 2168 bytes, 821 encodings and 251 relocations identical
 *
 * measured both from the scratch path and against the ORIGINAL multi-function
 * asm/overlays/rom_7e3e08/ovl_30_c_c_c_a_a_a.s with --func; objcmp prints no
 * `(built with: ...)` line from either, and tryc's makefile_flags() over the
 * prospective split names ovl_30_c_c_c_a_a_a_a.c / _b.c reports the tree
 * default -O2 with NO wildcard hit, so the flag trap of "a candidate's flags
 * depend on the path you screen it from" does not fire here.
 *
 * THE SPLIT. The .s holds four functions -- OvlFunc_957_2008f94 (111 insns),
 * OvlFunc_957_200909c, OvlFunc_957_20093f8 and this one, LAST in the file --
 * so a whole-file .c replacement is out and it is a TWO-way split:
 * ovl_30_c_c_c_a_a_a_a.s keeps the first three, ovl_30_c_c_c_a_a_a_b.s becomes
 * this .c. The linker script names the object on exactly two lines,
 * overlays/rom_7e3e08/overlay.ld:51 (.text) and :64 (.data); each gains its
 * _a/_b pair in order. Nothing else in the tree names the object -- no
 * Makefile rule, no fakematch.txt row. This function calls no sibling from its
 * own .s, so "a same-file callee never blocks a split" is not even needed.
 *
 * ---------------------------------------------------------------------------
 * THE PROLOGUE, READ BY CONTENT. `push {r5, lr}`. One callee-saved register,
 * and it holds ONE thing: `ldr r5, =0x2165` before the first __MessageID after
 * the if/else merge, then `add r5, #7` ~40 instructions later for the second.
 * Every other repeated constant in the function is rebuilt at every use, so
 * this is a PIN function with a single named local -- not a named-locals
 * function that happens to be wide.
 *
 * PLAIN C AND THE LENGTH TELL, WHICH FIRES HERE. 705 of 821 encodings differ,
 * and the candidate is 2156 bytes / 815 instructions against the ROM's
 * 2168 / 821 -- SHORTER, and the very first encoding is the tell: ref `b520`
 * (`push {r5, lr}`) against ours `b5e0` (`push {r5, r6, r7, lr}`). cse_main
 * commons the eleven repeated multi-instruction constants into pseudos that
 * straddle `bl`, gcc reaches into r8-r11 and stages them through r5/r6/r7 at
 * entry. Note the direction: the removed rematerialisations here OUTWEIGH the
 * widened prologue, where on the template OvlFunc_962_2008240 the two
 * cancelled exactly. The one-way reading still holds -- a length difference in
 * EITHER direction, plus the widened push, is the diagnosis; equality would
 * have proved nothing.
 *
 * THE ELEVEN HELD VALUES, by site count, read off the ROM's own constant
 * builds: 0xc0 << 8 (19 sites), 0x101 pooled (7), 0x81 << 1 (6), 0x80 << 9
 * (5), -0x10 i.e. `mov #0x10 / neg` (4), 0x80 << 1 (4), 0x80 << 8 (3),
 * 0x13333 (3), 0x9999 (3), 0x80 << 10 (2), 0x84 << 1 (2). Everything that
 * needs one `mov #imm8` is left alone by CSE and needs nothing. The 49 sites
 * touching one of those eleven are the pin candidates; four more were added by
 * reading the residue (below), and 95 by the __Func_8092c40 rule, for 54 tried.
 *
 * THE UNIFORM ASCENDING FILL IS ENOUGH EVERYWHERE BUT ONE SITE. One statement
 * per argument, ascending q0..q3, whole value per statement: 705 differing to
 * 14 in one step. sched2 reproduces every transposed order the ROM emits from
 * that one spelling -- `mov r1 / mov r2 / mov r0 / lsl r1`,
 * `mov r2 / mov r0 / ldr r1`, `mov r3 / mov r0 / mov r1 / mov r2 / lsl r3`,
 * `mov r1 / lsl r1 / mov r2 / mov r0` -- the shift and the `neg` land
 * themselves.
 *
 *   TRANSCRIBING THE ROM'S ORDER IS ACTIVELY WORSE HERE, and this is the
 *   sharpest measurement of that on record. Rewriting all 51 fills in the
 *   order the ROM first writes each register measures 71 ENCODINGS DIFFERING
 *   against 0 for uniform ascending. On the template the transcription was
 *   byte-identical and merely redundant; on this function it is a regression.
 *   Uniform is not a tidy default, it is the correct one.
 *
 * THE FIRST RESIDUE WAS 14 INSTRUCTIONS AND ALL OF IT WAS SITES THE PATTERN
 * HAD MISSED -- three of them ORDERING at unique-constant sites (the second
 * job a pin does), plus one generator bug:
 *   - __MapActor_SetSpeed(0, 0xcccc, 0x6666): two singleton pool loads, and
 *     gcc sinks `mov r0, #0` below both. Pinned.
 *   - __MapActor_Emote(8, 0x105, 0x28) and __MapActor_Emote(2, 0x103, 0x28):
 *     singleton pool loads emitted one slot early. Pinned.
 *   - __MapActor_Emote(8, 0x83 << 1, 0x28): singleton shifted byte whose `lsl`
 *     lands one slot early. Pinned.
 * All four are unique-constant sites, so none is a CSE victim.
 *
 * FIFTY-ONE PINS, AND THE SET IS UNIQUE, NOT JUST THE SIZE. Each of the 54
 * candidates was stripped individually under objcmp to a fixpoint; exactly
 * three measure inert and are NOT in this file:
 *   - __Func_8092adc(1, 0xc0 << 8, 0)   (ref.s:196)
 *   - __MapActor_Emote(2, 0x101, 0)     (ref.s:252, last of the four-in-a-row)
 *   - __Func_8092adc(2, 0xc0 << 8, 0)   (ref.s:635, LAST 0xc0<<8 site)
 * A second greedy pass run from the OTHER END of the candidate list dropped
 * the same three and no others, and both passes reach the same 51. That is
 * stronger than the usual "N pins is a size, not a set" result: here the size
 * and the set coincide. Re-adding the three is byte-identical, which is
 * exactly why they do not ship -- scaffolding that measures inert must not.
 * Consistent with the recorded pattern, all three inert sites are a LAST use
 * of their value and never a first.
 * Costs of the 51 when removed range from 2 encodings to 737 encodings plus a
 * 12-byte size change.
 *
 * __Func_8092c40 WANTS THE DESCENDING FILL, the seventh function to show it,
 * and the binary tell reproduces to the encoding: `q1 = 0; q0 = 8;` matches;
 * `q0 = 8; q1 = 0;` measures 2 encodings differing, which is the SAME NUMBER
 * as leaving the site unpinned altogether. Ascending at this callee is not a
 * weaker pin, it is no pin. Descending is also not a general answer -- writing
 * all 51 fills descending measures 114 differing.
 *
 * THE CARRIED MESSAGE BASE IS THE `add r5, #7` SHAPE, AND A PLAIN `int`
 * SURVIVES. Two literals (`__MessageID(0x2165)` / `__MessageID(0x216c)`)
 * measure 401 encodings differing at 820 instructions against 821 -- gcc pools
 * both and never allocates r5. The recorded scope of the cprop hazard puts this
 * in the "m is REDEFINED in place" branch, where cprop has nothing to
 * substitute at the second site, so no `register int m __asm__("r5")` pin is
 * needed and none is used. WHAT IS LOAD-BEARING IS THE `+=`: writing
 * `m = 0x216c;` instead of `m += 7;` measures the SAME 401 differing as the
 * two literals. A re-assignment gives cse a fresh CONST_INT to fold; the
 * increment does not, and `plus(reg, 7)` costs less than materialising 0x216c
 * in Thumb, so cse keeps the register. Recorded here because the existing
 * entry names the shape (`add r5, #k`) without measuring the failure mode of
 * spelling it as a re-assignment.
 *
 * ALL EIGHT POOLED VALUES ARE BARE LITERALS. 0x214f, 0x2164, 0x2165, 0x2168,
 * 0x101, 0x103, 0x105, 0xcccc, 0x6666, 0x13333, 0x9999 -- none is a shifted
 * byte gcc could build with mov+lsl, so each pools unaided, and objcmp reports
 * 251 relocations identical, all R_ARM_THM_CALL. Nothing belongs in const.sym
 * or message.sym for this function.
 *
 * THE THREE TRAILING WALK BLOCKS ARE WRITTEN OUT, NOT LOOPED -- the same shape
 * as OvlFunc_881_2008a8c: set anim, fetch actor 0, travel to it if non-null,
 * wait, zero the position. `unsigned char *p` with
 * `*(short *)(p + 0xa)` / `*(short *)(p + 0x12)` is what the ROM's
 * `mov r3, #0xa / ldrsh r1, [r0, r3]` pair wants and no pin is needed at any
 * of the three.
 *
 * The `b .L3034` over a pool and the `mov r0, #0x11 / b .L33f4` over the
 * second are gcc's own inline pool dumps; nothing in the source expresses them.
 *
 * SPELLINGS MEASURED AND REJECTED OR FOUND FREE (all against the 51-pin file):
 *
 *   spelling                                            differing
 *   ------------------------------------------------    ---------
 *   no pins at all, plain C                              705  (+ size 2156/2168)
 *   ROM's register order transcribed into all 51 fills    71
 *   all 51 fills descending                              114
 *   __MessageID(0x2165)/(0x216c) as literals             401  (+ 820/821 insns)
 *   m = 0x216c;  instead of  m += 7;                     401  (+ 820/821 insns)
 *   ascending fill at __Func_8092c40                       2  (= site unpinned)
 *   __Func_8092c40 unpinned                                2
 *   -- free, and therefore not used: --
 *   q2 = 0x10; q2 = -q2;  for  q2 = -0x10;                 0
 *   0xc000 etc. for 0xc0 << 8 (constant folds either way)  0
 *   q1 = 0xc0; q1 <<= 8;  split out of the one statement   0
 *   short *p with p[5] / p[9] for the two ldrsh            0
 *   the three inert pins re-added (54 pins)                0
 *
 * The last four say something worth keeping: inside a pinned fill the SHIFTED
 * BYTE is about statement COUNT and POSITION, never about how the constant is
 * spelled -- `0xc0 << 8` and `0xc000` are the same rtl after folding, and the
 * two-statement split is free only because it leaves the shift in the same
 * relative position the one-statement form already gives it.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __PlaySound(int id);
extern void __PlayMapMusic(void);
extern void __Func_808e118(void);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809233c(int a, int b, int c, int d);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_957_200ac44(void)
{
    unsigned char *p;
    int m;

    __SetFlag(0x96 << 4);
    __PlaySound(0x18);
    __CutsceneStart();
    __Func_808e118();
    __MessageID(0x214f);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __Func_80933f8(0xf8 << 16, -1, 0xb8 << 16, 1);
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(0, 0xf8, 0xc0);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN4; q0 = 1; q1 = -0x10; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 3; q1 = 0; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q0 = 2; q1 = 0x10; q2 = 0x10; q3 = 0xc0 << 8;
      __Func_809233c(q0, q1, q2, q3); }
    __MapActor_WaitMovement(1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 2; q1 = 0x81 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x28);
    __ActorMessage(2, 0);
    __Func_80925cc(3, 2);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 8; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 2, 0x32);
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    { PIN2; q0 = 8; q1 = 0x81 << 1;
      __MapActor_Surprise(q0, q1); }
    __Func_80925cc(8, 2);
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0x14);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 8; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(3, 4);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 8; q1 = 0x105; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __Func_809280c(1, 0, 0x1e);
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(1, 0);
    __Func_8092adc(1, 0xc0 << 8, 0);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 8; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(8, 0);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(3, 4);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 8; q1 = 0x83 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(2, 0x101, 0);
    __CutsceneWait(0x3c);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __ActorMessage(8, 0);
    __Func_80925cc(3, 2);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 8; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 8;
      __Func_8092c40(q0, q1); }
    __Func_809280c(1, 0, 0);
    __Func_809280c(2, 0, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __MessageID(0x2164);
        __CutsceneWait(0x14);
        { PIN3; q0 = 1; q1 = 0x80 << 1; q2 = 0x28;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0x80 << 10; q2 = 0x80 << 9;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0; q2 = -0x10;
          __Func_8092304(q0, q1, q2); }
        __CutsceneWait(0xa);
        __Func_8092848(1, 0, 0x1e);
        __ActorMessage(1, 0);
    } else {
        __MessageID(0x2168);
        __CutsceneWait(0xa);
        { PIN3; q0 = 1; q1 = 0x80 << 9; q2 = 0x80 << 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 1; q1 = 0; q2 = -0x10;
          __Func_8092304(q0, q1, q2); }
        __CutsceneWait(0xa);
        __Func_8092848(1, 0, 0x1e);
        __MapActor_DoAnim(1, 3);
        __CutsceneWait(0x14);
        __ActorMessage(1, 0);
    }
    m = 0x2165;
    __MessageID(m);
    { PIN3; q0 = 2; q1 = 0x103; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x80 << 10; q2 = 0x80 << 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0; q2 = -0x10;
      __Func_8092304(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_8092848(2, 0, 0x1e);
    __ActorMessage(2, 0);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __Func_809280c(0, 3, 0);
    __Func_809280c(1, 3, 0);
    __Func_809280c(2, 3, 0);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(1, 0, 0x10);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(1, 4);
    __CutsceneWait(0x1e);
    __Func_809280c(1, 0, 0);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0x1e);
    __Func_8092848(3, 2, 0x1e);
    __MapActor_DoAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 2; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092304(2, 0, 0x10);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    m += 7;
    __MessageID(m);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 8; q1 = 0x80 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(2, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 8; q1 = 0x84 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(3, 0);
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __Func_8092848(1, 0, 0x32);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __Func_8092848(3, 2, 0x32);
    { PIN3; q0 = 3; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(2, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0x1e);
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __ActorMessage(2, 0);
    __CutsceneWait(0x14);
    { PIN3; q0 = 8; q1 = 0x84 << 1; q2 = 0x32;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(8, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 8; q1 = 0x81 << 1; q2 = 0x28;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(8, 0);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(3, 4);
    __CutsceneWait(0x14);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __Func_809280c(0, 3, 0x28);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x1e);
    __Func_809280c(1, 3, 0);
    __Func_809280c(2, 3, 0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x1e);
    __PlaySound(0x11);
    { PIN3; q0 = 1; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetAnim(2, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_SetAnim(3, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __PlayMapMusic();
    __CutsceneEnd();
}
