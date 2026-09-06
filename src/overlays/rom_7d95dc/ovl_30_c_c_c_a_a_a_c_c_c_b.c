/* OvlFunc_953_2008dcc
 *   [asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_c_c.s, lines 673-1043 --
 *   the SECOND AND LAST of the file's TWO functions (`grep thumb_func_start`
 *   returns exactly two: OvlFunc_953_2008710 at line 9 and this one at 673).
 *   The file holds NO DATA -- zero `.incbin`, `.word`, `.byte`, `.hword`,
 *   `.section`, `.space`, `.ascii` -- so the split is a pure text cut.
 *
 *   LANDING NEEDS A SPLIT, because the .o must come from one source file and
 *   2008710 is still asm.  `tools/split_s.py asm/overlays/rom_7d95dc/
 *   ovl_30_c_c_c_a_a_a_c_c_c.s OvlFunc_953_2008dcc` leaves 2008710 in
 *   `..._c_c_c_a.s` and the target in `..._c_c_c_b.s`, to be replaced by
 *   src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_c_c_b.c.  There is no `_c`
 *   part: the target is last in the file.  NEITHER NAME IS TAKEN --
 *   `ovl_30_c_c_c_a_a_a_c_c_c_a.s`, `..._c_c_c_b.s` and the matching .c do not
 *   exist in asm/ or src/ today (the existing `..._c_c_a` / `..._c_c_b` are
 *   siblings one level up, not children).
 *
 *   THE LINKER LINE, MATCHED ON FULL PATH AND CITED BY CONTENT.
 *   overlays/rom_7d95dc/overlay.ld:32 is
 *       `		asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_c_c.o(.text)`
 *   and it is the ONLY line in ANY .ld naming that .o -- by full path AND by
 *   bare basename (`grep -rnE '(^|/)ovl_30_c_c_c_a_a_a_c_c_c\.o' --include=
 *   '*.ld'` returns that one line and nothing else).  The rom_780898 overlay
 *   carries twenty-four objects whose names BEGIN with this stem
 *   (`ovl_30_c_c_c_a_a_a_c_c_c_a_b.o` and friends at overlay.ld:47-69); none is
 *   an exact basename match, so a basename-matched edit would be safe here but
 *   the full-path form is what is used.  Line 32 becomes TWO lines, in order:
 *   `..._c_c_c_a.o(.text)` then `..._c_c_c_b.o(.text)`, and the ROM layout does
 *   not move.
 *
 *   EVERY SECTION NAMED FOR THIS .o IS ACCOUNTED FOR.  rom_7d95dc/overlay.ld
 *   has exactly two output sections plus /DISCARD/: `.text` (lines 15-53) and
 *   `.data` (55-57).  The .data list holds ONE entry,
 *   `asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_c.o(.data)`, which is a different
 *   object; there is no .bss and no .rodata list in this overlay at all.  So
 *   this .o is named in exactly one place and its split needs no .data,
 *   .rodata or .bss remap.  Let split_s.py rewrite the script.
 *
 *   NO FLAG GROUP, AND NONE CAN ARRIVE BY ACCIDENT.  objcmp prints no
 *   `(built with: ...)` line -- adjust=set(), the tree default
 *   `-O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi -fno-builtin -nostdinc
 *   -ffreestanding -fcall-used-r4`.  The only two Makefile rules that name a
 *   rom_7d95dc object literally name OTHER objects and both use CSE_CFLAGS:
 *   line 837 `asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_a_c_c.o` and line
 *   4654 `..._c_a_a_a.o`.  This object falls to the cross-dir pattern rule
 *   `asm/%.o: src/%.c` at line 146, which is GCC296_CFLAGS -- the default.
 *
 *   WORTH KNOWING FOR A LATER BATCH: solving the file-sibling
 *   OvlFunc_953_2008710 (643 instructions, the same straight-line cutscene
 *   family -- 21 turns, 21 anim changes, 5 dialogue lines, message base 0x2267,
 *   reads save bit 0x962, sets 0x93f) would let the WHOLE .s land with NO SPLIT
 *   AT ALL: one asm/ -> src/ edit at overlay.ld:32 and the .s deleted.  It was
 *   not attempted here.]
 *
 * EXACT, measured as a single-function extract:
 *
 *   OK OvlFunc_953_2008dcc -- 992 bytes, 379 encodings and 106 relocations identical
 *
 * The verdict holds against the REAL asm/ path and against a scratch copy of
 * the same function cut into its own .s.
 *
 * 363 instructions.  A save-flag-guarded cutscene with ONE two-armed branch on
 * a call result: `__GetFlag(0x8d << 2)` gates the whole body, `__SetFlag(0x235)`
 * opens it, and `__Func_8091c7c(0, 0)` chooses between the full 300-instruction
 * script and a three-line fallback that bumps a halfword counter and shows one
 * message.  NINE PINS OF SIXTY-FOUR pinnable sites.
 *
 * READ THE PROLOGUE BY CONTENT.  `push {r5, r6, r7, lr} / mov r7, r10 /
 * mov r6, r8 / push {r6, r7}` -- FIVE callee-saved registers, and what they hold
 * is most of the diagnosis:
 *
 *     r5   0xb0 << 8, then gScript_953__0200af24   TWO unrelated values
 *     r6   0x80 << 8                               a script constant
 *     r7   __Func_8091c7c(0, 0)                    the branch value, and the
 *                                                  zero stored nine times
 *     r8   0xd0 << 8                               a script constant
 *     r10  0xa0 << 8                               a script constant
 *
 * Four held constants and a call result.  No loop variable, no live pointer:
 * a pin function.  Plain C with no pins is 357 differing of 379.
 *
 * ------------------------------------------------------------ THE BLOCK ORDER
 *
 * `cmp r7, #0 / beq .Le64 / b .L1144` with .Le64 IMMEDIATELY FOLLOWING is the
 * thumb long-branch expansion of `bne .L1144`, so the BIG arm is the `if` body
 * and the counter bump is the `else`.  Writing it the other way round --
 * `if (r != 0) { bump } else { script }` -- is 355 differing and TWELVE
 * INSTRUCTIONS LONG; writing it the ROM's way is 157 with the same pins.  This
 * is the recorded "Block layout tells you which branch is the `if` BODY", and
 * at this size it is worth 200 differing on its own.
 *
 * ------------------------------------------------------- THE TWO BIT-FIELD OPS
 *
 * Two `__MapActor_GetActor(8)` calls, each followed by one bit op on byte 0x5a:
 *
 *     bl __MapActor_GetActor / add r0, #0x5a / ldrb r2, [r0] /
 *     mov r3, #0xfe / and r3, r2 / ... / strb r3, [r0]
 *
 * THE AGGREGATE-MEMBER FORM WITH NO NAMED POINTER IS THE ONLY EXACT ONE, and
 * getting there is worth 128 differing.  Measured four ways:
 *
 *     ((struct Actor *)__MapActor_GetActor(8))->f5a &= 0xfe;     0
 *     b = __MapActor_GetActor(8) + 0x5a; *b &= 0xfe;             2
 *     b = &((struct Actor *)__MapActor_GetActor(8))->f5a; *b..;  2
 *     p = (struct Actor *)__MapActor_GetActor(8); p->f5a &= ..;  128 (+2 insns)
 *
 * The named STRUCT POINTER is the expensive one: gcc keeps the call result in
 * r0 and copies it into r1 to add 0x5a, so each of the two blocks is ONE
 * INSTRUCTION LONG and the whole tail misaligns.  Naming an `unsigned char *`
 * AT the field costs only the `orr` operand order.  This is the batch-240
 * correction banner under "## `orr rd, rs` -- which operand becomes the
 * destination" ("Reach for the TYPED FIELD first") reproduced on a second
 * function, with the sharpening that the cost of the pointer here is a whole
 * instruction rather than a transposition -- because the ROM's `add r0, #0x5a`
 * DESTROYS the base, and a named pointer to the object forces gcc to keep it.
 *
 * The `and` needs nothing: `mov r3, #0xfe / and r3, r2` (constant is rd) falls
 * out of the plain form.  The `orr` gets it backwards from the pointer form and
 * right from the field form; `unsigned char m = 1; *b = m | *b;` also reaches
 * it (0), and is the recorded narrow-local remedy -- but it is scaffolding
 * beside a spelling that needs none, so the field form ships.
 *
 * ------------------------------------------------- THE TWO SPLIT-BUILD CONSTANTS
 *
 * The ROM builds r6 and r5 ACROSS a call each:
 *
 *     mov r6, #0x80 / ldr r0, =0x400c / bl OvlFunc_953_2009c48 / lsl r6, #8
 *     mov r5, #0xb0 / mov r1, sl / mov r0, #0x11 /
 *                     bl OvlFunc_953_2009c5c / lsl r5, #8
 *
 * Write the assignment BEFORE its call and gcc schedules the `lsl` into the
 * pre-call slot instead: 2 differing each, 4 for both.  Write it AFTER the call
 * and the ROM's split falls out by itself -- gcc hoists the `mov` back over the
 * call and leaves the `lsl` behind.  This is "## Build a constant AFTER the call
 * if the ROM does" arriving without its push-list symptom: the register count is
 * the same either way, only the schedule moves.  The lever is the STATEMENT
 * POSITION, and the ROM's own split is the thing that names it.
 *
 * ------------------------------------------------------------------- NEW ----
 * TWO CALLEE-SAVED REGISTERS SWAPPED, AND THE CURE IS A HARD REGISTER ON EITHER
 * ONE.  A FOURTH remedy for the recorded "everything is right but two
 * callee-saved registers are swapped" family, for the case the other three
 * cannot reach.
 *
 * With everything else exact the residue was 25 differing, every one of them
 * the same shape at a different site:
 *
 *     ROM    mov r1, r8            (0xd0 << 8 in r8, 0x80 << 8 in r6)
 *     ours   adds r1, r6, #0       (0xd0 << 8 in r6, 0x80 << 8 in r8)
 *
 * Same instructions, same order, same length -- gcc ranked the two held
 * constants the other way round and handed them each other's register.  THE
 * THREE RECORDED REMEDIES ARE ALL INERT HERE, and they are inert at EXACTLY THE
 * SAME COUNT, which is the tell that none of them is the axis:
 *
 *     spelling                                          differing
 *     ------------------------------------------------  ---------
 *     all 24 permutations of the four declarations          25
 *     `register int a6;` with no asm label                  25
 *     `a6 = 0x80; a6 <<= 8;` (shift split off the build)    25
 *     the same split on a8 / a5 / a10 / all four            25
 *     `a6 = 0x8000;` / `a8 = 0xd000;` as plain literals     25
 *     assignment moved earlier (birth order)             25, 5 or 2
 *
 * `register int a6 __asm__("r6")` is 0.  So is
 * `register int a8 __asm__("r8")` -- PINNING EITHER MEMBER OF THE SWAPPED PAIR
 * IS EXACT, because nailing one of two competing pseudos settles the other by
 * elimination.  Pinning a NON-member (a5 -> r5 or a10 -> r10, both of which
 * already hold the right register) is 25, i.e. inert: the pin has to land on
 * the contested pair.
 *
 * THIS IS NOT THE cprop HAZARD.  The recorded `register int m __asm__("r5")`
 * cures ("## THE cprop HAZARD IS SCOPED TO A RE-READ") exist because a plain
 * `int` is REMATERIALISED and the register stops being held at all.  Here both
 * constants stay held in callee-saved registers in every spelling -- the push
 * list is `{r5, r6, r7, lr}` + r8 + r10 throughout -- and only the numbering
 * moves.  Same syntax, different mechanism, and the diagnostic that separates
 * them is whether the value is still in a register at all.
 *
 * The file ships `a6 -> r6`, the low-register half of the pair.
 *
 * ----------------------------------------------------------------- THE PINS --
 *
 * NINE PINS OF SIXTY-FOUR, MINIMAL BY MEASUREMENT.  All 64 multi-argument sites
 * were pinned first, then stripped greedily with a re-test after every drop and
 * a re-grow pass, to a fixpoint from BOTH ENDS; a second full round changes
 * nothing, and NO PAIR of survivors can be dropped together (the best pair-drop
 * is 4).  Each survivor costs between 2 and 357:
 *
 *   call                                       direction  cost   length
 *   -----------------------------------------  ---------  ----  --------
 *   __MapActor_SetSpeed(0, 0x80<<9, 0x80<<8)   ascending   357   381 enc
 *   __MapActor_SetSpeed(0xd, 0x19999, 0xcccc)  ascending   345   389
 *   __MapActor_SetSpeed(0xe, 0x19999, 0xcccc)  ascending   344   389
 *   __Func_80933d4(0x19999, 0x3333)            ascending   336   383
 *   __Func_8092c40(0x8008, 0)                  DESCENDING  333   381
 *   __Func_8092adc(8, 0xc0<<6, 0x14)           ascending   328   383
 *   __Func_80933f8(0xcc<<18, -1, 0xe0<<15, 1)  ascending   325   383
 *   __Func_80921c4(0, 0xda<<2, 0x78)           ascending     2   379
 *   __MapActor_SetSpeed(0x10, 0x19999, 0xcccc) ascending     2   379
 *
 * ONLY TWO OF THE NINE ARE ORDERING PINS.  The other seven change the LENGTH
 * when dropped, by 2 to 10 encodings -- they are CSE kills, and the relocation
 * line stays silent on all of them, which is the recorded warning that the
 * relocation triage sorts by EFFECT and not by class.  Reading the cost column
 * as "how badly ordered" would have been wrong seven times out of nine.
 *
 * EVERY SURVIVOR'S DIRECTION IS LOAD-BEARING: flipping any one of the nine
 * costs 2 to 7.  And the ONE DESCENDING SURVIVOR IS `__Func_8092c40` -- the
 * named descending-fill callee, whose site here is
 * `mov r1, #0 / ldr r0, =0x8008`.  Four other sites were carried descending
 * through the whole sweep (OvlFunc_953_2009c5c(0xb, 0xc0<<6),
 * __MapActor_SetBehavior(0xd, ...) and (0x10, ...), OvlFunc_953_2009c5c(8, 0))
 * and ALL FOUR pruned to plain calls at zero cost once the rest was right --
 * their ROM orders fall out of gcc's own pool-load-first and interleave rules.
 * The all-cheap sites pruned without exception.
 *
 * OVER-PINNING IS MEASURABLY WORSE HERE, which is the opposite of the template.
 * The 64-pin set with the four extra descending sites still in it ties at 0,
 * but the 64-pin set filled UNIFORMLY ASCENDING is 6 -- so "pin everything
 * ascending" is not a safe fallback at this size, and the minimal nine is not
 * merely tidier.  (ovl_30_c_c_c_a_a_c_a_a.c records all 58 of its sites pinned
 * as an exact tie; that is a property of that function, not of the method.)
 *
 * WHAT NEEDED NOTHING.  The nine field stores through
 * `__MapActor_GetActor(0xd)` in the ROM's emitted order, with `q->f64` and
 * `q->f66` as `unsigned short` and the rest `int` in ONE struct tag -- SPLITTING
 * the tag in two (a narrow `ActorFlags` for f5a, a wide `Actor` for the words)
 * is an exact tie, so the file-sibling's UNIFY-A-STRUCT-TAG lever is not needed
 * here and one tag ships because it is simpler.  Writing the stores through
 * `unsigned char *q` with `*(int *)(q + 0x24)` casts is 6, so the TYPED FIELDS
 * are load-bearing even though the tag boundary is not.
 *
 * `r` IS A NAMED LOCAL BECAUSE IT IS THE BRANCH VALUE, NOT BECAUSE IT IS STORED.
 * Spelling the nine stores `q->f6c = 0;` instead of `= r;` is byte-identical --
 * gcc knows r == 0 inside the arm either way.  So the discriminator recorded at
 * OvlFunc_953_20091c4 ("the value SURVIVES A CALL in a callee-saved register")
 * does not discriminate here; `= r` ships as the honest value-flow reading, and
 * the tie is recorded so nobody re-derives it.
 *
 * `gScript_953__0200af24` IS NOT A SOURCE VARIABLE.  The ROM does
 * `ldr r5, =gScript_953__0200af24` once and then `mov r1, r5` at eight
 * __MapActor_SetBehavior sites -- r5's SECOND ROLE, after 0xb0 << 8 dies.
 * Naming it `unsigned char *s` measures EXACTLY ZERO, so by "inert scaffolding
 * must not ship" the bare symbol goes at all eight sites and gcc commons it into
 * r5 by itself.  Note what this does NOT need: the file-sibling's "r5's TWO
 * ROLES MUST BE TWO LOCALS" lever is about two VALUES competing for one slot;
 * here the second role is a symbol address gcc will hold regardless.
 *
 * Also inert, and therefore absent: `__Func_80933f8(0xca<<18, -1, 0xac<<15, 1)`
 * as a bare call reproduces the ROM's `lsl r2 / lsl r0 / neg r1` order even
 * though the FIRST __Func_80933f8 (which is pinned) emits `lsl r0 / neg r1 /
 * lsl r2` -- one spelling, two emitted orders, which is the recorded "DO NOT
 * TRANSCRIBE THE ROM'S SHIFT ORDER".  The fourteen __CutsceneWait, ten
 * __MapActor_SetBehavior, four __MapActor_SetAnim and five __MapActor_DoAnim
 * calls are bare literals.  The else arm is the plain
 * `*(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;` idiom.
 *
 * PROTOTYPES, 12 OF 28 LOAD-BEARING: dropping __Func_8092adc costs 26,
 * __MapActor_SetSpeed 11, OvlFunc_953_2009c5c 10, __MapActor_DoAnim 10,
 * __MapActor_SetAnim 8, __Func_80933d4 7, __Func_80933f8 5, __Func_8093530 5,
 * __Func_809259c 4, __MapActor_SetBehavior 2, __Func_80925cc 2,
 * __Func_80921c4 2.  The other sixteen are inert and stay declared because a
 * complete prototype list is this tree's convention.
 *
 * MEASURED WORSE (against 379 encodings / 992 bytes):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   no pins at all                                          357
 *   the arms written in the other order (plain C)           355 (+12 insns)
 *   named `struct Actor *p` for the two f5a bit ops         128 (+2 insns)
 *   plain `int a6` -- the swapped-register residue           25
 *   all 64 sites pinned, uniformly ascending                  6
 *   `unsigned char *q` + casts for the nine field stores       6
 *   a8 assignment hoisted above __CutsceneWait(0x14)           5
 *   a6 built BEFORE OvlFunc_953_2009c48 / a5 before its call   2 each, 4 both
 *   `unsigned char *b` + `*b &= 0xfe` / `*b |= 1`              2
 *   dropping any one of the nine pins                     2 to 357
 *   flipping any one survivor's fill direction            2 to 7
 *
 *   INERT (tie at 0, so the simpler form ships):
 *     splitting struct Actor into two tags
 *     naming the script pointer `unsigned char *s`
 *     `q->fXX = 0;` instead of `q->fXX = r;`
 *     `register int a8 __asm__("r8")` in place of a6 -> r6
 *     hard registers on all four held constants
 *     all 64 sites pinned with the four extra descending sites kept
 *
 * FLAGS DO NOT REACH ANY OF IT, and none is needed: with the final shape
 * -fno-gcse, -fno-cse-follow-jumps, -fno-expensive-optimizations and
 * -fno-strict-aliasing are all byte-identical, while -fno-schedule-insns2 is 89
 * and -fno-rerun-cse-after-loop is 346 -- in a function with NO LOOP, which is
 * the recorded "`-fno-rerun-cse-after-loop` is not a loop phenomenon" seen
 * again.  The sweep is live and the tree default is right.
 */
struct Actor { unsigned char pad00[0x24]; int f24, f28, f2c;
               unsigned char pad30[8]; int f38, f3c, f40;
               unsigned char pad44[0x16]; unsigned char f5a;
               unsigned char pad5b[9]; unsigned short f64, f66;
               unsigned char pad68[4]; int f6c; };

extern unsigned char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_Surprise(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int n);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_953_2009c48(int a);
extern void OvlFunc_953_2009c5c(int a, int b);
extern unsigned char gScript_911__0200ae20[];
extern unsigned char gScript_953__0200aed4[];
extern unsigned char gScript_953__0200ae5c[];
extern unsigned char gScript_953__0200af24[];

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")


void OvlFunc_953_2008dcc(void)
{
    int r;
    int a5;
    register int a6 __asm__("r6");
    int a8;
    int a10;
    struct Actor *q;

    if (__GetFlag(0x8d << 2) == 0)
        return;
    __SetFlag(0x235);
    __CutsceneStart();
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xda << 2; q2 = 0x78;
      __Func_80921c4(q0, q1, q2); }
    a10 = 0xa0 << 8;
    OvlFunc_953_2009c5c(0, a10);
    { PIN2; q0 = 0x19999; q1 = 0x3333;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xcc << 18; q1 = -1; q2 = 0xe0 << 15; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN3; q0 = 8; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(8, 1);
    __MessageID(0x2125);
    { PIN2; q1 = 0; q0 = 0x8008;
      __Func_8092c40(q0, q1); }
    r = __Func_8091c7c(0, 0);
    if (r == 0) {
        __CutsceneWait(0x14);
        a8 = 0xd0 << 8;
        OvlFunc_953_2009c5c(0xc, a8);
        __Func_80925cc(0xc, 1);
        __MessageID(0x212b);
        OvlFunc_953_2009c48(0x400c);
        a6 = 0x80 << 8;
        OvlFunc_953_2009c5c(0x11, 0);
        OvlFunc_953_2009c5c(0, a6);
        __MapActor_DoAnim(0x11, 3);
        __MapActor_DoAnim(0, 3);
        __MapActor_SetAnim(0xf, 1);
        __Func_8092adc(0xf, a8, 0x14);
        __Func_80925cc(0xf, 1);
        OvlFunc_953_2009c48(0xf);
        OvlFunc_953_2009c5c(0x10, a6);
        __MapActor_DoAnim(0x10, 3);
        __Func_80925cc(0x11, 2);
        OvlFunc_953_2009c5c(0x11, a10);
        a5 = 0xb0 << 8;
        OvlFunc_953_2009c48(0x4011);
        OvlFunc_953_2009c5c(0x12, a5);
        __Func_809259c(0x12, 2);
        OvlFunc_953_2009c48(0x4012);
        OvlFunc_953_2009c5c(0xb, 0xc0 << 6);
        __MapActor_Surprise(0xb, 0x81 << 1);
        __CutsceneWait(0x3c);
        __Func_809259c(0xb, 2);
        OvlFunc_953_2009c48(0x800b);
        __MapActor_SetIdle(0xd);
        __WaitFrames(1);
        __Func_809259c(0xd, 2);
        __Func_809259c(0xe, 2);
        __Func_80925cc(0x10, 2);
        __CutsceneWait(0x14);
        __Func_8092adc(0xd, 0, 0);
        __Func_8092adc(0xe, a6, 0);
        __Func_8092adc(0x10, a5, 0x28);
        __MapActor_SetAnim(0xd, 3);
        __MapActor_SetAnim(0xe, 3);
        __MapActor_DoAnim(0x10, 3);
        { PIN3; q0 = 0xd; q1 = 0x19999; q2 = 0xcccc;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0xe; q1 = 0x19999; q2 = 0xcccc;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0x10; q1 = 0x19999; q2 = 0xcccc;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetBehavior(0xd, gScript_911__0200ae20);
        __MapActor_SetBehavior(0x10, gScript_953__0200aed4);
        __CutsceneWait(0x14);
        __Func_8092adc(0xf, a8, 0);
        __Func_8092adc(0x11, a5, 0);
        __Func_8092adc(0, a10, 0);
        __Func_8092adc(0xc, a8, 0);
        __Func_8092adc(0x12, a5, 0);
        __MapActor_RunScript(0xe, gScript_953__0200ae5c);
        __CutsceneWait(0x14);
        __Func_8092adc(8, 0, 0);
        __Func_8092adc(0xb, a6, 0x28);
        __MapActor_SetAnim(8, 3);
        __MapActor_DoAnim(0xb, 3);
        __Func_80933f8(0xca << 18, -1, 0xac << 15, 1);
        __Func_8093530();
        __Func_80925cc(8, 2);
        OvlFunc_953_2009c48(8);
        __MapActor_SetSpeed(8, 0x80 << 9, a6);
        ((struct Actor *)__MapActor_GetActor(8))->f5a &= 0xfe;
        __Func_80921c4(8, 0xc6 << 2, 0x48);
        __CutsceneWait(1);
        ((struct Actor *)__MapActor_GetActor(8))->f5a |= 1;
        OvlFunc_953_2009c5c(8, 0);
        __MapActor_SetIdle(0xd);
        q = (struct Actor *)__MapActor_GetActor(0xd);
        q->f6c = r;
        q->f64 = r;
        q->f66 = r;
        q->f24 = r;
        q->f28 = r;
        q->f2c = r;
        q->f38 = 0x80 << 24;
        q->f3c = 0x80 << 24;
        q->f40 = 0x80 << 24;
        __WaitFrames(1);
        __MapActor_SetBehavior(0xf, gScript_953__0200af24);
        __CutsceneWait(0x14);
        __MapActor_SetBehavior(0xd, gScript_953__0200af24);
        __CutsceneWait(0x14);
        __MapActor_SetBehavior(0x11, gScript_953__0200af24);
        __CutsceneWait(0x14);
        __MapActor_SetBehavior(0xe, gScript_953__0200af24);
        __CutsceneWait(0x14);
        __MapActor_SetBehavior(0x10, gScript_953__0200af24);
        __CutsceneWait(0x14);
        __MapActor_SetBehavior(0xc, gScript_953__0200af24);
        __CutsceneWait(0x14);
        __MapActor_SetBehavior(0x12, gScript_953__0200af24);
        __CutsceneWait(0x3c);
        __MapActor_SetBehavior(0, gScript_953__0200af24);
        __CutsceneWait(0x50);
        __Func_8091e9c(0x42);
    } else {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0x8008, 0);
    }
    __CutsceneEnd();
}
