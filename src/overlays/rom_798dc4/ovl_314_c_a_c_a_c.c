// fakematch
/* OvlFunc_903_200867c  --  0x0200867c
 *   [asm/overlays/rom_798dc4/ovl_314_c_a_c_a_c.s, 1st of 1]
 *
 * 626 instructions of straight-line cutscene: 186 call sites, two
 * `__MapActor_GetActor` null guards, and one save-flag fork whose short arm
 * ends the scene early.  Message base 0x138f, reads save bit 0x855, sets
 * 0x865.
 *
 * LANDING NEEDS NOTHING BUT THE FILE.  The .s holds exactly ONE function, and
 * overlays/rom_798dc4/overlay.ld:34 already names the .o:
 *
 *     asm/overlays/rom_798dc4/ovl_314_c_a_c_a_c.o(.text)
 *
 * That is the ONLY line in any linker script that names this .o -- grepped
 * across the whole tree, `.text` is the object's only section (the .s emits no
 * `.data`, `.rodata` or `.bss` at all), and the `.data` block of that same
 * script names only `ovl_314_c_c_c_c_c.o`, so there is no section to remap and
 * no line to add.  Landing is: add this .c, delete the .s.
 *
 * NO FLAG GROUP.  `tryc.makefile_flags("src/overlays/rom_798dc4/
 * ovl_314_c_a_c_a_c.c")` returns the EMPTY set -- the single Makefile line
 * mentioning rom_798dc4 (line 4582) is for `ovl_314_a_c_a_a.o`, and no
 * wildcard reaches this stem.  The TU falls to the tree default
 * `asm/%.o: src/%.c` at -O2, and objcmp run against the ORIGINAL asm path (not
 * just a scratch copy of it) prints no `(built with: ...)` line and is
 * byte-identical.  The match does NOT depend on any flag.
 *
 * THE PROLOGUE IS `push {lr}` AND THAT IS THE WHOLE STORY.  Not one register
 * is callee-saved, so no value is HELD anywhere in 626 instructions: every
 * constant is rebuilt at its own call site.  Read by CONTENT, that is the
 * purest possible pin function -- there is no held-value range to name, and
 * nothing a pin can collapse.  Plain C gets it wrong in exactly that place:
 * unpinned, gcc hoists 0xcccc / 0x6666 / 0x100 / 0x102 / 0x105 into r5-r7,
 * pays `push {r5, r6, r7, lr}`, and 582 of 641 encodings differ.
 *
 * FORTY-TWO PINS, UNIFORM ASCENDING FILL, AND THE FILL IS NOT THE ROM'S ORDER.
 * Every pinned site writes q0, q1, q2 in ARGUMENT order.  Transcribing the
 * ROM's own emitted register order instead (`|ROM:` column of calls.txt, e.g.
 * `m1 m0 l1 m2` at site 10) costs 49 encodings; a uniform DESCENDING fill
 * costs 243 and comes out FOUR BYTES SHORT, which is the register-pressure
 * tell, not a cheaper encoding.  So on this ROM the ascending fill is CORRECT,
 * and the mid-group `mov` in the reference is sched2 rearranging a correctly
 * allocated group -- a pin constrains ALLOCATION, and the schedule follows.
 *
 * THE HOLE IN THE PIN SET IS EXACTLY TWO SITES, AND THEY ARE THE TWO THAT
 * CARRY THE ROM'S OWN HELD VALUE.  Both `__MapActor_GetActor` guards feed the
 * returned pointer straight into the next call as a load base:
 *
 *     rom      cmp r0, #0 / beq / ldr r1, [r0, #8] / ldr r2, [r0, #0x10]
 *                                                  / mov r0, #1 / bl
 *     pinned   adds r3, r0, #0 / cmp r3, #0 / beq / mov r0, #1
 *                                                  / ldr r1, [r3, #8] / ...
 *
 * The pin claims r0 for the slot number, so the pointer has to be copied out
 * to r3 first: one extra instruction in each guard, +4 bytes, and every
 * pc-relative load in the function shifts.  Pinning EITHER guard alone costs
 * 566 encodings -- the same as pinning both -- and no fill order rescues it
 * (`1,2,0` and `2,1,0` were both compiled: 565).  These two sites must stay
 * BARE.  They are the only two in the function that must.
 *
 * MINIMISED TO A FIXPOINT FROM BOTH ENDS, AND BOTH ENDS AGREE.  Starting from
 * all 181 orderable sites pinned -- which ALSO matches -- pins were dropped one
 * at a time, re-testing under objcmp after every drop, until no further drop
 * held.  Three independent drop orders -- ascending, descending, and a
 * shuffled one -- converge on the SAME 42, and a second full pass over the
 * survivors finds every one of them REQUIRED.  The
 * elevation notes warn that the sweep has a direction and may need running from
 * both ends twice; here it does not, and that is worth recording as a negative.
 *
 * THE PIN SET IS SLACK, THE HOLE IS NOT.  Sixteen further pins were added back
 * to the 42 one at a time -- the nine expensive-argument sites the sweep had
 * dropped, and seven all-cheap ones -- and every one of the sixteen is a TIE.
 * The landscape is not monotone: from the 42, dropping site 5 fails while
 * adding site 4 succeeds.  So 42 is a minimal set along this path and NOT a
 * unique one, and the only load-bearing pin decision in the whole function is
 * the hole at the two guards.  The 42 ship because a fixpoint is the only
 * defensible stopping point and inert scaffolding must not ship.
 *
 * NEW (measured here): AN ALL-CHEAP CALL SITE NEVER NEEDS AN ORDERING PIN.
 * Classify each site by whether ANY argument is expensive -- a shifted build,
 * a pool load, or a memory operand -- as opposed to a bare `mov rN, #imm8`.
 * 133 of the 186 sites are all-cheap, and NOT ONE of them appears in either
 * fixpoint, from either direction.  Re-running the minimisation from the 53
 * expensive-argument sites alone converges on the identical 42.  The test is
 * NECESSARY and not sufficient -- 11 of the 53 come out bare -- so it does not
 * choose the pins, but it prunes the sweep from 186 candidates to 53 at no
 * cost, which is a 3.5x cut in compiles.  docs/elevation.md already has the
 * mechanism twice, at "Naming a shifted value works against a POOL LOAD, not
 * against a cheap mov" and in the `script_candidates.py` note that a bare
 * `mov rN, #imm8` is rematerialised for free and must not be counted; what is
 * new is that the same cheapness test governs the PIN SET and not merely the
 * repeated-constant ranking, and that here it is exact in one direction.
 *
 * THE STACK-ARGUMENT PAIR IS REQUIRED, and it is the sibling's lever.  The ROM
 * builds BOTH outgoing stack words before storing either:
 *
 *     rom    mov r3, #0x49 / mov r2, #0xb / str r3, [sp] / str r2, [sp, #4]
 *     ours   mov r3, #0x49 / str r3, [sp] / mov r3, #0xb / str r3, [sp, #4]
 *
 * Same instruction count, one register instead of two.  Two named locals
 * assigned before the call restore it; written as plain literals the function
 * is 3 encodings wrong.  This is the third site in this overlay to want it --
 * see src/overlays/rom_798dc4/ovl_314_c_a_c_a_b.c.
 *
 * TEARDOWN.  Every knob was removed from the finished file and re-measured:
 *
 *     the 42 pins removed entirely            582 of 641 differing, wide push
 *     ROM-order fill at the 42 sites           49 differing
 *     descending fill at the 42 sites         243 differing, 4 bytes SHORT
 *     the stack-argument pair                   3 differing
 *     `int` return with `return 0;`             4 differing
 *     pinning either GetActor guard           566 differing, 4 bytes LONG
 *
 * And four spellings are exact TIES, so none of them is a lever: `0xe0 << 8`
 * against `0xe000` at every site (the shift is folded before the pin is
 * placed), `if (p != 0)` against `if (p)`, and
 * `if (__GetFlag(0x855) == 0)` against `if (!__GetFlag(0x855))`.  Whole values
 * are shipped because they read as the ROM's own constants.
 *
 * -- generated from scratch_elev/b242/f200867c/{extract,mkbody,gen}.py; the
 *    site table is calls.txt, the pin set is the second argument to gen.py.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int id);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80917d0(int a, int b);
extern void __Func_809202c(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void OvlFunc_903_2008314(int a, int b);
extern void OvlFunc_903_2008d68(void);
extern void OvlFunc_903_2008fc8(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_903_200867c(void)
{
    unsigned char *p;
    int s0;
    int s1;

    __Func_809202c();
    __CutsceneStart();
    __CutsceneWait(0x1e);
    __MessageID(0x138f);
    { PIN3; q0 = 0x0; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0xe000; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    p = __MapActor_GetActor(0);
    if (p != 0) {
        __MapActor_SetPos(0x1, *(int *) (p + 8), *(int *) (p + 0x10));
    }
    { PIN3; q0 = 0x1; q1 = 0x108; q2 = 0xa8;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x6000; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x1, 0x4);
    __CutsceneWait(0x14);
    __Func_8093040(0x1, 0x0, 0x14);
    __MapActor_DoAnim(0x0, 0x3);
    __CutsceneWait(0x14);
    if (__GetFlag(0x855) == 0) {
        __MapActor_SetAnim(0x1, 0x2);
        p = __MapActor_GetActor(0);
        if (p != 0) {
            __MapActor_TravelTo(0x1, *(short *) (p + 0xa), *(short *) (p + 0x12));
        }
        __MapActor_WaitMovement(0x1);
        __MapActor_SetPos(0x1, 0x0, 0x0);
        __CutsceneEnd();
    } else {
        { PIN3; q0 = 0x2; q1 = 0x1680000; q2 = 0xf80000;
          __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q0 = 0x2; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0x2; q1 = 0x110; q2 = 0xf8;
          __Func_80921c4(q0, q1, q2); }
        { PIN3; q0 = 0x2; q1 = 0x110; q2 = 0xd0;
          __Func_80921c4(q0, q1, q2); }
        { PIN3; q0 = 0x2; q1 = 0xa000; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(0x2, 0x2);
        __CutsceneWait(0x14);
        __Func_8093040(0x2, 0x0, 0x14);
        { PIN3; q0 = 0x0; q1 = 0x2000; q2 = 0x0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x1; q1 = 0x4000; q2 = 0x0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x14);
        { PIN3; q0 = 0x0; q1 = 0x100; q2 = 0x0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x1; q1 = 0x100; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        __MapActor_DoAnim(0x2, 0x3);
        { PIN3; q0 = 0x2; q1 = 0x108; q2 = 0xc8;
          __Func_80921c4(q0, q1, q2); }
        __Func_809218c(0x0, 0xf8, 0xa8);
        __Func_80921c4(0x2, 0xf8, 0xb8);
        __MapActor_WaitMovement(0x0);
        { PIN3; q0 = 0x0; q1 = 0x6000; q2 = 0x0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x1; q1 = 0x6000; q2 = 0x0;
          __Func_8092adc(q0, q1, q2); }
        __Func_80921c4(0x2, 0xe8, 0xb8);
        __CutsceneWait(0x14);
        { PIN3; q0 = 0x2; q1 = 0x105; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x2; q1 = 0xe000; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        __MapActor_DoAnim(0x2, 0x4);
        __CutsceneWait(0x14);
        __Func_8093040(0x2, 0x0, 0x14);
        __MapActor_SetAnim(0x0, 0x3);
        __MapActor_DoAnim(0x1, 0x3);
        __CutsceneWait(0x14);
        { PIN3; q0 = 0x2; q1 = 0x8000; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        __Func_8093040(0x2, 0x0, 0x78);
        { PIN3; q0 = 0x0; q1 = 0x105; q2 = 0x0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x1; q1 = 0x105; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        __Func_8092848(0x0, 0x1, 0x0);
        __CutsceneWait(0x3c);
        { PIN3; q0 = 0x0; q1 = 0x6000; q2 = 0x0;
          __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(0x1, 0x6000, 0x0);
        __CutsceneWait(0x3c);
        { PIN3; q0 = 0x2; q1 = 0x106; q2 = 0x0;
          __MapActor_Emote(q0, q1, q2); }
        __Func_809259c(0x2, 0x1);
        __CutsceneWait(0x1e);
        __Func_8093040(0x2, 0x0, 0x1e);
        { PIN3; q0 = 0x2; q1 = 0xe000; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        __Func_809259c(0x0, 0x2);
        __Func_80925cc(0x1, 0x2);
        __CutsceneWait(0x14);
        __Func_8093040(0x2, 0x0, 0x14);
        __MapActor_SetAnim(0x0, 0x3);
        __MapActor_DoAnim(0x1, 0x3);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x2, 0x3);
        __CutsceneWait(0x14);
        { PIN3; q0 = 0x2; q1 = 0x8000; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        OvlFunc_903_2008d68();
        __MapActor_SetAnim(0x2, 0x1);
        __CutsceneWait(0x14);
        __Func_809202c();
        { PIN3; q0 = 0x0; q1 = 0x100; q2 = 0x0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x1; q1 = 0x100; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        __MapActor_Jump(0x1, 0x2, 0x0);
        __CutsceneWait(0x14);
        OvlFunc_903_2008314(0x1, 0x14);
        __Func_80921c4(0x2, 0x108, 0xb8);
        __CutsceneWait(0xa);
        __Func_809280c(0x2, 0x1, 0x0);
        __Func_809280c(0x1, 0x2, 0x0);
        __Func_809280c(0x0, 0x2, 0x0);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x2, 0x3);
        __CutsceneWait(0x14);
        OvlFunc_903_2008314(0x2, 0x3c);
        { PIN3; q0 = 0x0; q1 = 0x105; q2 = 0x0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x1; q1 = 0x105; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x0; q1 = 0x101; q2 = 0x0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x1; q1 = 0x101; q2 = 0x0;
          __MapActor_Emote(q0, q1, q2); }
        __CutsceneWait(0x3c);
        { PIN3; q0 = 0x1; q1 = 0x8000; q2 = 0x0;
          __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(0x0, 0x0, 0x0);
        __CutsceneWait(0x3c);
        { PIN3; q0 = 0x1; q1 = 0x4000; q2 = 0x0;
          __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(0x0, 0x2000, 0x0);
        __CutsceneWait(0xa);
        OvlFunc_903_2008314(0x1, 0x14);
        __MapActor_Emote(0x2, 0x105, 0x0);
        __CutsceneWait(0x3c);
        __MapActor_DoAnim(0x2, 0x4);
        __CutsceneWait(0x14);
        OvlFunc_903_2008314(0x2, 0x14);
        __Func_809280c(0x0, 0x2, 0x0);
        __Func_809280c(0x1, 0x2, 0x0);
        { PIN3; q0 = 0x0; q1 = 0x102; q2 = 0x0;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0x1; q1 = 0x102; q2 = 0x0;
          __MapActor_Emote(q0, q1, q2); }
        __CutsceneWait(0x3c);
        __Func_80925cc(0x2, 0x2);
        __CutsceneWait(0x14);
        OvlFunc_903_2008314(0x2, 0x1e);
        { PIN3; q0 = 0x0; q1 = 0x101; q2 = 0x0;
          __MapActor_Emote(q0, q1, q2); }
        __MapActor_Emote(0x1, 0x101, 0x0);
        __CutsceneWait(0x50);
        __MapActor_DoAnim(0x2, 0x3);
        __CutsceneWait(0x14);
        OvlFunc_903_2008314(0x2, 0x14);
        __Func_809259c(0x0, 0x1);
        __Func_809259c(0x1, 0x1);
        { PIN2; q0 = 0x0; q1 = 0x102;
          __MapActor_Surprise(q0, q1); }
        __MapActor_Surprise(0x1, 0x102);
        __CutsceneWait(0x3c);
        __MapActor_DoAnim(0x2, 0x4);
        __CutsceneWait(0x14);
        OvlFunc_903_2008314(0x2, 0x14);
        __Func_8092adc(0x0, 0x0, 0x0);
        { PIN3; q0 = 0x1; q1 = 0x8000; q2 = 0x0;
          __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x50);
        __Func_809280c(0x0, 0x2, 0x0);
        __Func_809280c(0x1, 0x2, 0x0);
        __CutsceneWait(0x1e);
        __MapActor_DoAnim(0x2, 0x3);
        __CutsceneWait(0x14);
        OvlFunc_903_2008314(0x2, 0x1e);
        __MapActor_DoAnim(0x2, 0x4);
        __CutsceneWait(0x14);
        OvlFunc_903_2008314(0x2, 0x14);
        __Func_809259c(0x0, 0x2);
        __Func_80925cc(0x1, 0x2);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x2, 0x3);
        __CutsceneWait(0x14);
        OvlFunc_903_2008314(0x2, 0x28);
        __MapActor_SetAnim(0x0, 0x3);
        __MapActor_DoAnim(0x1, 0x3);
        __CutsceneWait(0x14);
        __Func_80917d0(0x2, 0x1);
        __CutsceneWait(0x3c);
        OvlFunc_903_2008fc8();
        __Func_80925cc(0x2, 0x1);
        __CutsceneWait(0x14);
        __Func_80921c4(0x2, 0xf8, 0xb8);
        __CutsceneWait(0x14);
        OvlFunc_903_2008314(0x2, 0x14);
        { PIN3; q0 = 0x0; q1 = 0x8000; q2 = 0x0;
          __Func_8092adc(q0, q1, q2); }
        __Func_8092adc(0x1, 0x8000, 0x0);
        __CutsceneWait(0x78);
        OvlFunc_903_2008314(0x2, 0x1e);
        __Func_809280c(0x0, 0x2, 0x0);
        __Func_809280c(0x1, 0x2, 0x0);
        __Func_809280c(0x2, 0x0, 0x0);
        __CutsceneWait(0x14);
        __MapActor_SetAnim(0x0, 0x3);
        __MapActor_SetAnim(0x1, 0x3);
        __MapActor_DoAnim(0x2, 0x3);
        __CutsceneWait(0x32);
        { PIN3; q0 = 0x1; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0x2; q1 = 0xcccc; q2 = 0x6666;
          __MapActor_SetSpeed(q0, q1, q2); }
        __Func_809218c(0x1, 0xf8, 0xa8);
        __Func_80921c4(0x2, 0xf8, 0xa8);
        __MapActor_SetPos(0x2, 0x0, 0x0);
        __MapActor_WaitMovement(0x1);
        __MapActor_SetPos(0x1, 0x0, 0x0);
        s0 = 0x49;
        s1 = 0xb;
        __Func_8010704(0x4a, 0xb, 1, 1, s0, s1);
        __SetFlag(0x865);
        __CutsceneEnd();
    }
}
