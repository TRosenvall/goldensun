// fakematch
/* OvlFunc_945_200c254  --  0x0200c254
 *   [asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_c_a_a.s, the whole file]
 *
 * 337 instructions of cutscene: a straight-line opening, one outer save-flag
 * test whose two arms are the whole body, a four-way flag chain inside the
 * taken arm (two of whose arms the compiler cross-jumps), and a one-bit
 * follow-up test.  Built at the TREE DEFAULT -O2 -- objcmp reports
 * `flags adj=[]`, no Makefile prefix captures this path, and NO FLAG GROUP IS
 * INVOLVED.  (The immediate neighbour in overlay.ld, ovl_..._a_a_a_c_a_b.o, is
 * a CSE_CFLAGS rule; this file is not and must not be given one.)
 *
 * THE PROLOGUE IS `push {r5, r6, r7, lr} / mov r7, r8 / push {r7}` -- FOUR
 * call-crossing values, and the fourth costs a HIGH register.  Plain C spends
 * only r5/r6/r7 and comes out 8 bytes and 4 instructions SHORT (900 -> 892 is
 * the wrong direction; the fully-pinned draft is 884).  Two things had to be
 * true at once and they pull against each other:
 *
 *  1. gcse must still common 0x80 << 8 (r6, three of its four sites) and
 *     0xd8 << 1 (r8, both of its sites) -- the ROM keeps those.
 *  2. gcse must NOT common anything else.  Unpinned, it also hoists 0xcb << 1,
 *     0x80 << 9, 0xb0 << 8, 0xc0 << 6 and friends into the same three
 *     registers, which is 287 of 350 encodings differing.
 *
 * THIRTY PINNED SITES, MINIMAL BY MEASUREMENT.  Fifty-five were pinned first;
 * twenty-five are inert one at a time, all twenty-five came out together in a
 * greedy pass with a re-test after every drop, and a second sweep over the
 * thirty survivors -- run FORWARD and REVERSED, both to a fixpoint -- finds
 * none still inert.  Every fill is the uniform ascending q0..q3 one-statement
 * form; writing all thirty descending is 65 of 350.
 *
 * THE FOURTH REGISTER IS BOUGHT WITH A LIVE RANGE, NOT WITH `-ffixed-r7`.
 * `alt` is written 0 in the taken arm and 1 in two of the four flag arms; the
 * ROM's `mov r7, #0` sits AFTER the last read of 0xd8 << 1, so the obvious
 * source order gives the two values disjoint live ranges and gcc hands them
 * ONE register -- the r8 save disappears and with it four instructions.
 * Writing `alt = 0;` anywhere at or BEFORE that last read makes them conflict,
 * and gcc then spends r8 on the commoned 0xd8 << 1 exactly as the ROM does:
 * at the final pin set, 329 of 346 at -8 bytes -> 0 of 350 on that one move.
 * Hoisting it OUT of the taken arm, above the `__GetFlag(0xc0 << 2)` test, is
 * a different failure -- 40 of 350 -- so it wants that arm, not the top of the
 * function.  Nine placements between the
 * `OvlFunc_945_200cfa8` call and the 0xd8 << 1 read are byte-identical (the
 * post-reload scheduler puts the `mov r7, #0` back beside the GetActor call in
 * all of them); the first placement AFTER it is the -8-byte failure again.
 * See "Two constants in DIFFERENT registers means they are simultaneously
 * live" -- same mechanism, and this is its high-register form.  NOTE that the
 * `-ffixed-r7` tell ("the ROM saves a high register and you do not, and the
 * gap is about four") FIRES HERE AND IS WRONG: reserving r7 would leave three
 * call-crossing registers, r5/r6/r8, where the ROM has four.  Count the ROM's
 * callee-saved registers against yours: SAME COUNT, rotated -> flag; ONE MORE
 * -> live range.
 *
 * THE ROM LOADS 0xa01b TWICE FROM THE POOL INSIDE ONE BASIC BLOCK, which the
 * recorded blocker says is unreachable ("Identical constants in ONE basic
 * block": zero of 3235 generated .s files do it).  It is reachable, and it
 * needs BOTH halves:
 *   * a CALL-CLOBBERED PIN on the literal site (`__Func_8093040`), so the
 *     constant cannot survive the `bl`; and
 *   * the named copy `msg = 0xa01b;` assigned AFTER that call, so no
 *     callee-saved register already holds the value when the pin is set.
 * Either half alone is 4 of 350 -- with the pin but `msg` assigned first, cse
 * folds the pin into `mov r0, r5`; with `msg` late but no pin, the same.  With
 * both, sched2 hoists `ldr r5, =0xa01b` back above the `bl` on its own and the
 * two adjacent loads come out.  The slot is exact: `msg` one statement later
 * (after the `__MapActor_Emote`) is 19 of 350.
 *
 * THE FOUR `OvlFunc_945_200c880` SITES MUST NOT BE PINNED.  The ROM finishes
 * the shifted argument before touching r0 (`mov r1 / lsl r1 / mov r0, r5`);
 * an ascending pin puts `mov r0` between them, 6 of 350.  A descending pin
 * ties, and the plain unpinned call ties -- so the pin is dropped rather than
 * inverted.
 *
 * MEASURED AND INERT, so deliberately NOT carried: named `spd`/`px` locals for
 * the two commoned constants (the shape the sibling template
 * src/overlays/rom_79dd90/ovl_30_c_c_c_c_a_c_a_b_c.c uses -- once the thirty
 * pins are in, gcse produces r6 and r8 unaided and the locals are
 * byte-identical scaffolding); `0x1b0`/`0x8000` written flat instead of
 * shifted; `if (p)` / `if (alt)` without the explicit `!= 0`; `0x300` instead
 * of `0xc0 << 2`; the flag chain written as nested `if`s; declaration order.
 *
 * 0x1ebc, 0x1eb7, 0xa01b and 0x201b ARE PLAIN INTEGERS, not message.sym
 * symbols -- settled by relocation, not by line count: the reference object
 * carries exactly 83 records and every one is an R_ARM_THM_CALL.  There is no
 * ABS32 anywhere, so no name can be involved.
 *
 * LANDING NEEDS NO SPLIT.  The .s holds THIS ONE FUNCTION and no data, and
 * exactly one linker line in the whole tree names the .o:
 *   overlays/rom_7cb2c0/overlay.ld:70
 *       asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_c_a_a.o(.text)
 * The object has a .data and a .bss section, both ZERO-LENGTH, and no linker
 * script mentions either -- nothing to remap.  Build via the generic
 * `asm/%.o: src/%.c` rule; add no explicit rule.
 *
 * Thirty register pins => fakematch; needs a fakematch.txt row.
 */
struct Actor {
    unsigned char pad_00[8];
    int x;                      /* 0x08 */
    int y;                      /* 0x0c */
    int z;                      /* 0x10 */
};

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern int __GetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapTransitionIn(void);
extern void __MessageID(int id);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8091e9c(int n);
extern int OvlFunc_945_200cfa8(int a, int b);
extern void OvlFunc_945_200c86c(int a);
extern void OvlFunc_945_200c880(int a, int b);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void OvlFunc_945_200b7d8(int a);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_945_200c254(int a)
{
    struct Actor *p;
    int actor;
    int alt;
    int msg;

    __CutsceneStart();
    OvlFunc_945_200c8e8(0x18, 0, 0);
    OvlFunc_945_200c890(0, 0xd8 << 1, 0x86, 0x80 << 8);
    OvlFunc_945_200b7d8(1);
    __WaitFrames(1);
    __MapTransitionIn();
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xcb << 1; q2 = 0x86; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xcb << 1; q2 = 0x98; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x1a5; q2 = 0x98; __Func_80921c4(q0, q1, q2); }
    __Func_80925cc(0x1b, 1);
    __CutsceneWait(0x14);
    __Func_8092848(0x1b, 0, 0xa);
    if (__GetFlag(0xc0 << 2)) {
        actor = OvlFunc_945_200cfa8(a, 0);
        __Func_80925cc(0x1b, 1);
        __CutsceneWait(0x14);
        __Func_8092848(0x1b, 0, 0xa);
        __MessageID(0x1ebc);
        OvlFunc_945_200c86c(0xa01b);
        __MapActor_DoAnim(0, 3);
        { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
        alt = 0;
        __Func_80921c4(0, 0xd8 << 1, 0xa8);
        __Func_8092adc(0, 0xc0 << 8, 0);
        p = __MapActor_GetActor(0);
        if (p != 0) {
            __MapActor_SetPos(actor, p->x, p->z);
        }
        { PIN3; q0 = actor; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = actor; q1 = 0xe0 << 1; q2 = 0xa8; __Func_80921c4(q0, q1, q2); }
        { PIN3; q0 = actor; q1 = 0xb0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0x1b; q1 = 0xc0 << 6; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        __MapActor_SetAnim(0x1b, 3);
        OvlFunc_945_200c86c(0x1b);
        { PIN3; q0 = actor; q1 = 0x81 << 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
        __Func_80925cc(0x1b, 1);
        __MapActor_SetAnim(0x1b, 3);
        OvlFunc_945_200c86c(0x1b);
        __MapActor_DoAnim(actor, 3);
        if (__GetFlag(0x92b)) {
            { PIN3; q0 = 0; q1 = 0x80 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
            { PIN3; q0 = 0x1b; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
            { PIN3; q0 = actor; q1 = 0xeb << 1; q2 = 0xcc; __Func_80921c4(q0, q1, q2); }
            OvlFunc_945_200c880(actor, 0xb0 << 8);
        } else if (__GetFlag(0x92a)) {
            { PIN3; q0 = 0; q1 = 0xd3 << 1; q2 = 0x9a; __Func_80921c4(q0, q1, q2); }
            { PIN3; q0 = 0; q1 = 0xc0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
            { PIN3; q0 = 0x1b; q1 = 0xa0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
            { PIN3; q0 = actor; q1 = 0xcd << 1; q2 = 0xcc; __Func_80921c4(q0, q1, q2); }
            alt = 1;
            OvlFunc_945_200c880(actor, 0xd0 << 8);
        } else if (__GetFlag(0x929)) {
            { PIN3; q0 = 0; q1 = 0x80 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
            { PIN3; q0 = 0x1b; q1 = 0xc0 << 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
            { PIN3; q0 = actor; q1 = 0xeb << 1; q2 = 0xac; __Func_80921c4(q0, q1, q2); }
            OvlFunc_945_200c880(actor, 0xb0 << 8);
        } else {
            { PIN3; q0 = 0; q1 = 0xd3 << 1; q2 = 0x9a; __Func_80921c4(q0, q1, q2); }
            { PIN3; q0 = 0; q1 = 0xc0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
            { PIN3; q0 = 0x1b; q1 = 0xa0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
            { PIN3; q0 = actor; q1 = 0xcd << 1; q2 = 0xac; __Func_80921c4(q0, q1, q2); }
            alt = 1;
            OvlFunc_945_200c880(actor, 0xd0 << 8);
        }
        __Func_8092848(0x1b, 0, 0x14);
        OvlFunc_945_200c86c(0x201b);
        __MapActor_DoAnim(0, 3);
        __MapActor_DoAnim(0x1b, 3);
        { PIN3; q0 = 0x1b; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
        if (alt != 0) {
            { PIN3; q0 = 0x1b; q1 = 0xd6 << 1; q2 = 0xa4; __Func_80921c4(q0, q1, q2); }
            { PIN3; q0 = 0x1b; q1 = 0xcc << 1; q2 = 0xa4; __Func_80921c4(q0, q1, q2); }
        }
        { PIN3; q0 = 0x1b; q1 = 0xcc << 1; q2 = 0x86; __Func_80921c4(q0, q1, q2); }
        __Func_809218c(0x1b, 0xdc << 1, 0x86);
        __CutsceneWait(0x28);
        OvlFunc_945_200c8e8(9, 0xa, 0);
    } else {
        __MessageID(0x1eb7);
        { PIN3; q0 = 0xa01b; q1 = 0; q2 = 0x28; __Func_8093040(q0, q1, q2); }
        msg = 0xa01b;
        __MapActor_Emote(0x1b, 0x101, 0x3c);
        OvlFunc_945_200c86c(msg);
        __MapActor_Surprise(0, 0x81 << 1);
        __CutsceneWait(0x3c);
        { PIN3; q0 = 0x1b; q1 = 0x103; q2 = 0x28; __MapActor_Emote(q0, q1, q2); }
        __Func_809259c(0x1b, 2);
        OvlFunc_945_200c86c(msg);
        __MapActor_Emote(0x1b, 0x105, 0x28);
        OvlFunc_945_200c86c(msg);
        __MapActor_DoAnim(0x1b, 4);
        OvlFunc_945_200c86c(msg);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0x14);
        __Func_8091e9c(4);
    }
}
