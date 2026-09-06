/* OvlFunc_954_2008db8  --  0x02008db8
 *   [asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_a.s, lines 385-819 of 819]
 *
 * EXACT.  objcmp: "OK OvlFunc_954_2008db8 -- 1116 bytes, 438 encodings and 110
 * relocations identical", against the asm/ path and against a scratch copy of
 * the reference alike, so no Makefile pattern rule is in play.  objcmp printed
 * no "(built with: ...)" line, i.e. adjust=set(): the tree default
 * -O2 -mthumb -mthumb-interwork -fcall-used-r4.  NO FLAG GROUP IS NEEDED.
 *
 * 427 instructions of cutscene script.  Control flow is one `if` around one
 * `do`/`while`, both testing __Func_8093054(a, 0), and three null-guarded
 * __MapActor_TravelTo blocks.  Roughly 100 calls.
 *
 * THE FOUR-WAY BRANCHES ARE LONG-BRANCH EXPANSIONS, NOT SOURCE SHAPES.  Both
 * tests read `cmp r0,#0 / beq <next> / b <far>`, and the two <next> labels are
 * the FALL-THROUGH.  That is the recorded thumb expansion of a single `bne`
 * whose target is out of the +-254-byte conditional range; read as source
 * shapes the entry test (enter when 0) and the bottom test (repeat when != 0)
 * look contradictory, and they are not.
 *
 * READ THE PROLOGUE BY CONTENT.  `push {r5, r6, r7, lr} / mov r7, r10 /
 * mov r6, r9 / mov r5, r8 / push {r5, r6, r7}` -- THREE `mov rN, r8..r10`
 * copies, destinations r7, r6, r5.  Six callee-saved registers, and what they
 * hold is the whole diagnosis:
 *
 *     r7   the parameter `a`
 *     r9   ix = actor->[0xa]   read once at entry, used again at the very end
 *     r10  iz = actor->[0x12]  ditto
 *     r5   iz << 16   } live across the five __MapActor_SetPos calls
 *     r6   ix << 16   }
 *     r8   (iz << 16) - 0x280000, commoned across SetPos slots 1 and 2
 *
 * Plain C spends a FOURTH high register and a stack slot -- `mov r7, r11 /
 * mov r6, r10 / mov r5, r9 / push / mov r7, r8 / push {r7} / sub sp, #4` -- and
 * `str r2, [sp, #0]`.  The extra pressure is not ix/iz: it is gcc commoning
 * `0x80 << 9` and `0x80 << 8` into r5/r6 across the five __MapActor_SetSpeed
 * calls, which the ROM rebuilds at every site.  396 differing, +4 bytes.
 *
 * EIGHTEEN PINNED SITES OF 92 PINNABLE, MINIMAL BY MEASUREMENT.  The recorded
 * routine: pin everything, then strip greedily, re-measuring after EVERY drop,
 * to a fixpoint.  Round 1 removed 69 of the 87 it started from; round 2
 * removed none.  The shipped set was measured AS A SET (that is what the OK line above is), so the
 * "individually-inert pins are not jointly removable" hazard does not apply.
 * Each survivor is load-bearing; the costs are in the table below.
 *
 * THE PIN SET HAS A HOLE, AND THE HOLE IS THE POINT (NEW).  The five
 * __MapActor_SetPos calls must be left BARE.  Every other pinnable site with a
 * repeated or shifted constant wants a pin; those five want none, because they
 * are the one region where the ROM ITSELF commons -- r5, r6 and r8 above are
 * gcc's own CSE and they are correct.  A pin on an argument register is dead
 * across the next `bl`, so pinning there destroys exactly the CSE the ROM
 * performs: it re-lands `ix << 16` in r5 instead of r6, moves the shared
 * `(iz << 16) - 0x280000` from r8's `add r2, r5, r3` form to a repeated
 * `add r2, r8`.
 *
 * This is the recorded "name the ones gcc should NOT hoist and leave the ones
 * it should" polarity, in its PIN form and at REGION granularity rather than
 * per constant.  The practical consequence is that no one-lever-at-a-time
 * sweep finds it:
 *
 *     pin set (halfword store held BARE throughout)   differing
 *     --------------------------------------------   ---------
 *     none                                               396
 *     the five SetSpeed sites only                       401   (WORSE)
 *     all 92                                             347
 *     all 92 MINUS the five SetPos sites (87)            321
 *
 *     ...and then the halfword field lever on top of
 *     the 87-pin row                                       0
 *
 * The second row is the recorded "a pin set that gets worse is not evidence
 * that pins are the wrong lever -- extend it"; this function adds the second
 * half of that instruction: extend it, and then CARVE OUT the region the ROM
 * commons for itself.  Note also that the carve is worth only 26 of 438 on its
 * own -- it is a PRECONDITION for the halfword lever, not a win by itself, and
 * the halfword lever applied to the un-carved 92-pin set is 381.  Nothing in
 * this function is diagnosable one lever at a time.
 *
 * UNIFORM ASCENDING FILL AT ALL EIGHTEEN.  One statement per argument,
 * ascending q0..q3, whole value per statement.  It reproduces every emitted
 * order the ROM has here -- `mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2` at
 * SetSpeed, `mov r2 / mov r0 / ldr r1` at Emote, `mov r0 / mov r1 / mov r2 /
 * lsl r2 / mov r3 / lsl r0 / neg r1` at __Func_80933f8.  Descending was
 * measured at each of the eighteen SEPARATELY and is worse at every one (2 to
 * 6 differing); there is no __Func_8092c40 site here.
 *
 * THE PINS ARE REGISTER PINS, NOT NAMES.  The same eighteen blocks written as
 * plain `int q0, q1, q2;` locals measure 397 -- indistinguishable from no
 * scaffolding at all.  What destroys the CSE is the hard call-clobbered
 * DESTINATION, not the naming.
 *
 * THE HALFWORD STORE WANTS THE TYPED FIELD, AND ONLY THAT.  `actor->f6 =
 * 0xc0 << 8` with 0xc000 >= 0x8000 is the HImode-literal case: written bare it
 * pools (`ldr r3, [pc, #..]`) and the mid-function pool that forces cascades,
 * 321 differing.  The three non-pooling spellings do NOT tie:
 *
 *     ((struct Actor *)__MapActor_GetActor(0))->f6 = 0xc0 << 8;      0
 *     int h = 0xc0 << 8; *(unsigned short *)(... + 6) = h;           4
 *     ...plus a named `unsigned short *q` for the destination        4
 *
 * The ROM builds the value in r3, a SCRATCH register (`mov r3,#0xc0 / lsl
 * r3,#8 / strh r3,[r0,#6]`), and a named int local takes r5 and reorders the
 * following `mov r1, #0`.  That is the template's recorded discriminator --
 * callee-saved in the ROM wants a named local, scratch wants the field --
 * confirmed here with the field as the only zero.  `struct Actor` with 0x6:2
 * is an established name in this tree (75 files, docs/structs.md).
 *
 * THE MID-FUNCTION POOL WAS A LENGTH SYMPTOM, NOT A BLOCKER.  At 440
 * instructions gcc dumped a pool with a `b.n` over it after __Func_809259c and
 * every later `ldr [pc, #..]` shifted.  The ROM's first pool load reaches its
 * end-of-function pool at #964, about 1008 bytes -- just inside the 1020-byte
 * thumb range.  Nothing was done about the pool: it disappeared by itself the
 * moment the body came out at 438 instructions.  Treat a mid-function pool on
 * a function this size as a symptom to re-read after the length is right.
 *
 * WHAT NEEDED NOTHING.  ix/iz as plain `int` locals holding sign-extended
 * `*(short *)(p + 0xa)` reads; the entry __MapActor_GetActor with NO null
 * check (the three at the end have one, and that asymmetry is the ROM's);
 * `(iz << 16) - 0x300000` written as a subtraction (`+ 0xffd00000` is
 * byte-identical, so the readable form ships); the shared
 * `(iz << 16) - 0x280000` written out TWICE rather than named (naming it is
 * 379 differing -- gcc already carries it in r8, and naming destroys the
 * carry); `short` instead of `int` for ix/iz (byte-identical, so the honest
 * `int` ships -- the ROM's later `sub r5, #0x10` is an int subtraction).
 *
 * PROTOTYPES ARE LOAD-BEARING, 13 OF 17 TESTED.  Dropping any one of
 * __ActorMessage (40), __MapActor_SetSpeed (15), __MapActor_SetPos (13),
 * __Func_8092adc (11), __MapActor_Emote (10), __Func_80933f8 (10),
 * __Func_80921c4 (9), __MapActor_DoAnim (8), __Func_80925cc (8),
 * __Func_80933d4 (6), __Func_809280c (4), __SetCameraTarget (2) or
 * __Func_809259c (2) breaks the match.  Only __MapActor_SetAnim,
 * __MapActor_TravelTo, __CutsceneWait and __MessageID are inert; they stay
 * declared because a complete prototype list is this tree's convention and
 * costs nothing.  __Func_8093054 must be declared `int` -- its result is the
 * loop condition.
 *
 * MEASURED WORSE (against 438 encodings / 1116 bytes):
 *
 *   spelling                                             differing
 *   ---------------------------------------------------  ---------
 *   no pins at all                                         396 (+4 bytes)
 *   the five SetSpeed pins alone                           401 (+16 bytes)
 *   all 92 pins                                            347 (+8 bytes)
 *   all 92 pins, halfword as typed field                   381 (-4 bytes)
 *   the 18 pins as plain `int` locals, not register pins   397
 *   halfword store bare (pools; forces a mid-fn pool)      321 (+8 bytes)
 *   halfword store via a named `int` local                   4
 *   halfword store via named `int` + named `unsigned short *` 4
 *   the shared (iz<<16)-0x280000 named                     379 (-4 bytes)
 *   descending fill, all 18 sites                           60
 *   descending fill, the 5 SetSpeed sites                   20
 *   descending fill, the 5 Emote sites                      10
 *   descending fill, any single site                       2 to 6
 *   no prototype for __ActorMessage                         40
 *   no prototype for __MapActor_SetSpeed                    15
 *
 *   INERT (tie at 0, so the plain form ships):
 *     `+ 0xffd00000` instead of `- 0x300000` at the five SetPos deltas
 *     `short ix, iz` instead of `int ix, iz`
 *     dropping the pin on __SetCameraTarget(0, 0) at site 12
 *     dropping the prototype of SetAnim / TravelTo / CutsceneWait / MessageID
 *
 * LANDING NEEDS A SPLIT, AND THE .ld NAMES THE .o TWICE.  The .s holds TWO
 * functions -- OvlFunc_954_2008a3c at line 9 and this one at line 385 -- and
 * declares no section other than .text (the `.align 2, 0` + `.word .Lc70 ...`
 * at lines 259-265 is 2008a3c's jump table, inside .text).  Both lines below
 * are in overlays/rom_7db0c8/overlay.ld and BOTH must be remapped by the
 * split, not just the .text one:
 *
 *     asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_a.o(.text)     (in .text)
 *     asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_a.o(.data)     (in .data)
 *
 * The .data line contributes zero bytes today, which is exactly the "a split
 * that says no data is a claim to CHECK" / "remap EVERY section, not the one
 * that errors" trap.  No other .ld in the tree names this full path.
 * `tools/split_s.py asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_a.s
 * OvlFunc_954_2008db8` gives `_a.s` (2008a3c) and `_b.s` (this one, last in
 * the file, so no `_c`); both child stems are free in asm/overlays/rom_7db0c8.
 * This file then lands at src/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_a_b.c and
 * builds through the existing cross-dir `asm/%.o: src/%.c` rule under the
 * default GCC296_CFLAGS -- no new Makefile rule, and therefore no wildcard
 * that could capture a future sibling.
 */
struct Actor { unsigned char pad00[6]; unsigned short f6; };

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __SetCameraTarget(int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern int __Func_8093054(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

void OvlFunc_954_2008db8(int a)
{
    unsigned char *p;
    int ix, iz;

    p = __MapActor_GetActor(a);
    ix = *(short *)(p + 0xa);
    iz = *(short *)(p + 0x12);
    __CutsceneStart();
    { PIN3; q0 = a; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetPos(0, (ix << 16), (iz << 16) - 0x300000);
    __MapActor_SetPos(1, (ix << 16) - 0x100000, (iz << 16) - 0x280000);
    __MapActor_SetPos(2, (ix << 16) + 0x100000, (iz << 16) - 0x280000);
    __MapActor_SetPos(3, (ix << 16), (iz << 16) - 0x200000);
    __MapActor_SetPos(a, (ix << 16), (iz << 16) - 0x500000);
    ((struct Actor *)__MapActor_GetActor(0))->f6 = 0xc0 << 8;
    __SetCameraTarget(0, 0);
    __MapTransitionIn();
    __WaitMapTransition();
    __MessageID(0x20cb);
    __ActorMessage(a, 0);
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(3, 0);
    __Func_809259c(a, 3);
    __ActorMessage(a, 0);
    { PIN3; q0 = 2; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(2, 0);
    __Func_809280c(a, 2, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(a, 3);
    __ActorMessage(a, 0);
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(1, 0);
    { PIN3; q0 = 3; q1 = 0x101; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __ActorMessage(3, 0);
    { PIN3; q0 = a; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    if (__Func_8093054(a, 0) == 0) {
        do {
            __MessageID(0x20d5);
            __MapActor_SetAnim(2, 3);
            __CutsceneWait(2);
            __MapActor_SetAnim(1, 3);
            __CutsceneWait(2);
            __MapActor_SetAnim(3, 3);
            __CutsceneWait(1);
            __MapActor_DoAnim(0, 3);
            __MapActor_DoAnim(a, 3);
            __ActorMessage(a, 0);
            __Func_8092adc(a, 0xa0 << 8, 0);
            __CutsceneWait(0x14);
            __ActorMessage(a, 0);
            { PIN2; q0 = 0xc0 << 10; q1 = 0xc0 << 7;
              __Func_80933d4(q0, q1); }
            { PIN4; q0 = 0x9c << 17; q1 = -1; q2 = 0xd0 << 15; q3 = 1;
              __Func_80933f8(q0, q1, q2, q3); }
            __Func_8093530();
            __ActorMessage(a, 0);
            __Func_80933d4(0xc0 << 9, 0xc0 << 6);
            { PIN4; q0 = 0xc2 << 18; q1 = -1; q2 = 0xd0 << 15; q3 = 1;
              __Func_80933f8(q0, q1, q2, q3); }
            __ActorMessage(a, 0);
            __Func_8093530();
            __ActorMessage(a, 0);
            { PIN2; q0 = 0xc0 << 10; q1 = 0xc0 << 7;
              __Func_80933d4(q0, q1); }
            { PIN4; q0 = 0x9b << 19; q1 = -1; q2 = 0xa8 << 16; q3 = 1;
              __Func_80933f8(q0, q1, q2, q3); }
            __Func_8093530();
            __Func_809280c(a, 0xc0 << 7, 0);
            __ActorMessage(a, 0);
            __Func_80933f8(0xa3 << 19, -1, 0xa8 << 16, 1);
            __Func_8093530();
            __Func_809280c(a, 0, 0);
            __ActorMessage(a, 0);
            __ActorMessage(a, 0);
            __ActorMessage(a, 0);
            __SetCameraTarget(0, 0);
            __Func_80925cc(a, 2);
        } while (__Func_8093054(a, 0) != 0);
        __Func_80925cc(a, 2);
        __MessageID(0x20d4);
        __ActorMessage(a, 0);
    }
    __MessageID(0x20e1);
    __Func_80925cc(a, 2);
    __ActorMessage(a, 0);
    { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(1, 0, 0);
    { PIN3; q0 = 2; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(1, 2);
    __ActorMessage(1, 0);
    __Func_80925cc(2, 2);
    __ActorMessage(2, 0);
    __MapActor_DoAnim(3, 3);
    __ActorMessage(3, 0);
    __MapActor_SetAnim(3, 3);
    __CutsceneWait(1);
    __MapActor_SetAnim(1, 3);
    __CutsceneWait(2);
    __MapActor_SetAnim(2, 3);
    __CutsceneWait(1);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(6);
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_SetAnim(2, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_SetAnim(3, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __Func_80921c4(a, ix - 0x10, iz - 0x40);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_SetPos(3, 0, 0);
    __Func_80921c4(a, ix - 0x10, iz - 0x10);
    __Func_80921c4(a, ix, iz);
    { PIN3; q0 = a; q1 = 0xc0 << 8; q2 = 0xa;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneEnd();
}
