/* OvlFunc_956_200a330  --  0x0200a330    EXACT
 *
 * Whole-file conversion of asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_a.s, which
 * holds this ONE function and NO data -- no `.section`, no `.data`, no `.word`,
 * no `.align`, no `.incbin`.  162 instructions / 164 encodings / 416 bytes.
 * Cutscene script: the gState 0xe1 halfword guard, __CutsceneStart, a three-way
 * dispatch on OvlFunc_common1_4cc(param, 5), a walk-and-emote body with a
 * 0x78-frame drift loop over the slot-0 actor's field 8, one gState progress
 * byte, and the shared OvlFunc_common1_5e4 / __CutsceneEnd tail.
 *
 * VERDICT
 *   OK OvlFunc_956_200a330 -- 416 bytes, 164 encodings and 36 relocations identical
 *
 * Confirmed three ways: against the ORIGINAL asm/ path with --func, against
 * that path with NO --func (the .s holds one function, so both are the same
 * object), and against a scratch copy of the reference.  objcmp printed no
 * "(built with: ...)" line, i.e. adjust=set().  NO FLAG GROUP IS NEEDED and the
 * Makefile has no rule mentioning rom_7e0928, so nothing narrows to CSE_CFLAGS
 * or GCSE_CFLAGS here.
 *
 * SELECTION.  tools/solved_twins.py reports ZERO across the whole remaining set
 * (2117 solved shapes, 1508 remaining), so this was written from the nominated
 * template plus corpus idiom.  The template src/overlays/rom_7db0c8/
 * ovl_30_c_c_c_c_a_b.c supplied only the FRAME -- the `g` local, the 0xe1<<1
 * guard, the 4cc dispatch, the r==0 / r==1 arms, the 5e4 tail.  NONE of its
 * cures transfer: it is blocked on CONSTANT CSE ACROSS CALLS and cures it by
 * lowering reference counts with named locals, and this function commons
 * nothing -- every constant here is used once.  Its whole named-local block
 * (s1/s2/e1/e2/f1/f2/p1/p2/m/q1/q2/x/z) is inapplicable, and its prototype
 * reading is only half right (see PROTOTYPES below).
 *
 * READ THE PROLOGUE BY CONTENT.  `push {r5,r6,r7,lr} / mov r7,r8 / push {r7}`
 * is ONE `mov rN, r8..r11` copy, destination r7 -- four callee-saved registers,
 * r5, r6, r7, r8, the first four of gcc-2.96 thumb REG_ALLOC_ORDER's call-saved
 * sequence (r5, r6, r7, r8, r10, r9, r11) taken in order.  Each is a source
 * variable and there is no fifth:
 *     r5 = p   the slot-0 actor pointer, live across __WaitFrames in the loop
 *     r6 = i   the loop counter
 *     r7 = a   the parameter
 *     r8 = r   the OvlFunc_common1_4cc result, live to the last call
 * Nothing is left over for a base pointer carried across the body, and that is
 * what decides the gState spellings below.  This is the whole diagnosis: with
 * four registers and four variables, the ONLY residue this function can have is
 * ARGUMENT ORDER, and that is exactly what it had -- 17 differing at 164
 * encodings against 164, SIZE and RELOCATIONS silent from the first screen to
 * the last.  (The recorded partition: an ORDERING problem gives a small
 * encoding count with SIZE and RELOCATIONS both silent; a CSE loss gives 23-600
 * with RELOCATIONS always differing.)
 *
 * THE gState BASE WANTS A LOCAL FOR THE GUARD AND NO LOCAL FOR THE STORE.
 * Both spellings appear in this one function and they are opposite:
 *
 *     g = gState;  ... *(short *)(g + (0xe1 << 1)) == 2      the guard
 *     gState[0xf9 << 1] = 1;                                  the store
 *
 * The guard is the recorded case -- the byte-offset form folds to one pool word
 * `=gState+450` and comes out an instruction short, and a local base blocks the
 * fold (147 differing without it).  The STORE is not: it must NOT go through
 * the entry `g`, because carrying that base from the guard to the store spans
 * the whole body and takes a FIFTH callee-saved register -- 156 differing and
 * FOUR BYTES LONGER, the prologue growing a second `mov rN, rHIGH` copy.  A
 * second local assigned just before the store (`g2 = gState;`) is byte-identical
 * to the bare array, so it is scaffolding and does not ship.
 *
 * NEW: AT THE DECLARED ELEMENT TYPE, WITH A CONSTANT INDEX, THE SUBSCRIPT ALONE
 * SUPPRESSES THE FOLD.  Recorded already are two ways to stop `symbol + offset`
 * folding into one pool word: change the extern's ELEMENT TYPE and index it
 * (`extern short gState[]; gState[0xe1]`), or let the index carry a loop
 * variable (`gState[(0xfc << 1) + i]`).  Neither covers this site, which has the
 * DECLARED element type and a CONSTANT index, and there the two C-equivalent
 * spellings are NOT equivalent to gcc-2.96:
 *
 *     gState[0xf9 << 1] = 1;                   EXACT   (no fold; ARRAY_REF)
 *     gState[498] = 1;                         EXACT   (same, decimal)
 *     gState[0xf9 * 2] = 1;                    EXACT   (same, mul spelling)
 *     *(gState + (0xf9 << 1)) = 1;             55 differing, 1 insn SHORT
 *     *(unsigned char *)(gState + (0xf9 << 1)) = 1;   55 differing, 1 insn SHORT
 *
 * `a[i]` and `*(a + i)` are the same expression in C and different trees in
 * gcc-2.96: the subscript on an array-typed decl builds an ARRAY_REF, which the
 * expander lowers as base-plus-index, while the pointer form is a PLUS_EXPR of
 * two link-time constants and gets folded into the pool address.  AND THE CAST
 * FORM IS NOT A SUBSCRIPT: `((short *)gState)[0xe1]` for the guard folds exactly
 * like the byte-offset form (147 differing, 1 short), because the cast decays
 * the array to a pointer before the index is applied.  So the reachable rule is
 * "subscript the array AS DECLARED"; anything that puts a cast or a `+` between
 * the array name and the index loses it.  The recorded tell still holds and is
 * how this was found: a folded `=sym+offset` where the ROM has a bare symbol,
 * with our length ONE INSTRUCTION UNDER.
 *
 * PROTOTYPES: THE SAME FAMILY WANTS OPPOSITE THINGS, PER SITE.  Three callees
 * of the same shape sit in this function and the polarity splits:
 *
 *     OvlFunc_common1_1078   NO prototype   (ROM puts `mov r0` LAST)
 *     OvlFunc_common1_5e4    NO prototype   (ROM puts `mov r0` LAST)
 *     OvlFunc_956_200a2f4    prototype      (ROM puts `mov r0` in the MIDDLE)
 *
 * Declaring 1078 costs 2 differing and declaring 5e4 costs 3; withholding
 * 200a2f4's costs 2.  The template's list of r0-last callees (1078, 15b8, 5e4)
 * is right about the two it shares with this file and says nothing about the
 * overlay-local one, which is the recorded "prototype presence is a per-site
 * lever, not a per-file one" in its cleanest form.
 *
 * THE 200a2f4 PROTOTYPE REPLACES A PIN, AND THE PIN IS WORSE THAN NOTHING.
 * Worth stating because the sweep found it the hard way.  With the prototype
 * present and the NEXT site (the 0x4ccc __MapActor_SetSpeed) pinned, the
 * 200a2f4 site needs no scaffolding at all.  Add a pinned fill there anyway and
 * the WHOLE-VALUE form (`q0 = 0; q1 = 0xf8 << 2; q2 = 0xb8;`) costs 2 differing
 * -- ACTIVELY WORSE THAN NO PIN -- while the split form with the shift in the
 * ROM's position (`q1 = 0xf8; q1 <<= 2; q0 = 0; q2 = 0xb8;`) ties at 0.  That
 * split form was load-bearing at 26 encodings' distance EARLIER in the sweep,
 * before the 0x4ccc site was pinned, which is the recorded hazard: a lever
 * measured in one shape is only valid in that shape, and a one-at-a-time sweep
 * that had stopped at the first exact candidate would have shipped it.
 *
 * FIVE PINNED BLOCKS AND SEVEN PINNED REGISTERS, MINIMAL BY MEASUREMENT.  The
 * first exact candidate had six blocks and sixteen registers.  Three greedy
 * rounds, re-measuring after every drop, removed one whole block (200a2f4, see
 * above) and nine registers.  A fourth round found nothing: every surviving
 * block costs 2 or 3 differing when dropped, and the shipped set was measured
 * AS A SET, so the "individually-inert pins are not jointly removable" hazard
 * does not apply.  What each pin is FOR, read off the ROM:
 *
 *     __MapActor_SetSpeed(0, 0xc0<<9, 0xc0<<8)   mov r1 / mov r2 / mov r0 /
 *                                                lsl r1 / lsl r2   -- r0 in the
 *                                                MIDDLE; q0 and q1 both needed,
 *                                                q2 inert
 *     __MapActor_SetSpeed(0, 0x4ccc, 0x2666)     mov r0 / ldr r1 / ldr r2 --
 *                                                r0 FIRST, against gcc's own
 *                                                "pool loads come first"; q0
 *                                                and q1 needed, q2 inert
 *     __MapActor_Surprise(0, 0x80<<1)            mov r1 / mov r0 / lsl r1 --
 *                                                q0 alone is enough
 *     __MapActor_Emote(0, 0x105, 0)              mov r0 / ldr r1 / mov r2 --
 *                                                q0 alone
 *     __MapActor_Emote(0, 0x103, 0x3c)           mov r0 / ldr r1 / mov r2 --
 *                                                q0 alone
 *
 * Four of the five are one job: get `mov r0, #0` in front of a POOL LOAD or a
 * shifted build that gcc wants to schedule first.  At both SetSpeed sites the
 * ASSIGNMENT ORDER inside the block is load-bearing -- writing `q1` before `q0`
 * costs 2 differing at each, which is "pin declaration order is argument order"
 * showing up as assignment order under a shared macro.
 *
 * EVERY OTHER PINNABLE SITE MEASURES INERT AND IS LEFT BARE: __Func_80933f8's
 * four-register fill, __Func_80933d4, both OvlFunc_956_200a2c4 sites,
 * OvlFunc_common1_1078, the first __MapActor_Surprise (0x101), and
 * OvlFunc_common1_5e4 -- the last of which is notable because the sibling
 * src/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_c.c SHIPS a two-register pin there
 * for the identical `mov r1 / mov r2 / mov r0` order.  Two remedies for one
 * order, and here the prototype drop already did the job, so the pin is
 * scaffolding.  A sibling's cure being inapplicable is not a reason to add it.
 *
 * THE LOOP IS A PLAIN COUNTED `for`, AND FIVE SPELLINGS TIE.  The ROM has the
 * body first and the test at the bottom -- `.L241c: ... / sub r6,#1 /
 * bl __WaitFrames / cmp r6,#0 / bge .L241c` -- with no entry jump, which is
 * gcc's own lowering of a `for` whose first test is known true.  Neither
 * top-test cure (`do { if (!c) break; } while (1)`, the `goto` loop) is called
 * for.  Measured byte-identical: `for (i = 0x77; i >= 0; i--)`,
 * `i = 0x77; do { ... } while (--i >= 0);`, `for (i = 0; i <= 0x77; i++)`,
 * `*(int *)(p + 8) += 0xfffecccd` instead of `-= 0x13333`, `>= (0xf8<<18)+1`
 * instead of `> (0xf8<<18)`, reading the field into a temp before the test, a
 * `(char *)` cast on the base, and declaring __MapActor_GetActor as returning
 * `int`.  The plain forms ship.  Note that the bound `0xf8 << 18` is REBUILT
 * inside the loop by the ROM and by gcc alike, so there is no loop-invariant
 * question here and naming it would be the recorded "DO NOT NAME A
 * LOOP-INVARIANT" mistake.
 *
 * PROTOTYPES ARE LOAD-BEARING, 8 OF 16 TESTED.  Dropping any one of
 * __MapActor_Emote (6), __MapActor_SetSpeed (6), __ActorMessage (4),
 * __Func_80933d4 (2), __MapActor_SetAnim (2), __MapActor_Surprise (2),
 * __SetCameraTarget (2) or OvlFunc_956_200a2f4 (2) breaks the match.  Inert:
 * __CutsceneWait, __Func_80933f8, __MessageID, __WaitFrames,
 * OvlFunc_956_200a2c4, OvlFunc_common1_1254, _1314 and _588.  They stay
 * declared because a complete prototype list is this tree's convention; the two
 * that must NOT be declared are named above.
 *
 * MEASURED-WORSE TABLE (against 164 encodings / 416 bytes; counts are objcmp
 * ENCODINGS-differing, and SIZE and RELOCATIONS are silent on every row unless
 * noted):
 *
 *   spelling                                                     differing
 *   -----------------------------------------------------------  ---------
 *   no pins, no prototype drops (the first candidate)                 17
 *   prototypes for BOTH 1078 and 5e4                                  17
 *   prototype for 1078 only                                           15
 *   prototype for 5e4 only                                            14
 *   both dropped, still no pins                                       12
 *   drop the __MapActor_SetSpeed 0xc0<<9 pin                           3
 *   drop the __MapActor_SetSpeed 0x4ccc pin                            3
 *   restore the 5e4 prototype                                          3
 *   drop the __MapActor_Surprise 0x80<<1 pin                           2
 *   drop the __MapActor_Emote 0x105 pin                                2
 *   drop the __MapActor_Emote 0x103 pin                                2
 *   restore the 1078 prototype                                         2
 *   withhold the 200a2f4 prototype                                     2
 *   ADD a whole-value pin at 200a2f4                                   2
 *   SetSpeed 0xc0<<9: q0 pinned only (drop q1)                         4
 *   SetSpeed 0x4ccc:  q0 pinned only (drop q1)                         4
 *   SetSpeed 0xc0<<9: `q1 = ...` written before `q0 = 0`               2
 *   SetSpeed 0x4ccc:  `q1 = ...` written before `q0 = 0`               2
 *   `*(gState + (0xf9 << 1)) = 1;` for the store             55, 1 insn SHORT
 *   the store through the entry `g`                          156, +4 BYTES
 *   no `g` local: `*(short *)(gState + (0xe1<<1))` guard     147, 1 insn SHORT
 *   `((short *)gState)[0xe1]` guard, no local                147, 1 insn SHORT
 *   `switch (r)` instead of `if (r == 0) / else if (r == 1)`          108
 *
 *   INERT (tie at 0, so the plain form ships):
 *     the else-form of the guard instead of the early `return`
 *     a second `g2 = gState;` local for the byte store
 *     `((short *)g)[0xe1]` for the guard, with the local kept
 *     the split-fill pin at 200a2f4; pins at 80933f8, 80933d4, both 200a2c4
 *       sites, 1078, __MapActor_Surprise(0x101) and 5e4
 *     q2 at both __MapActor_SetSpeed sites; q1/q2 at both __MapActor_Emote
 *       sites; q1 at __MapActor_Surprise
 *     the seven loop and pointer spellings listed above
 *
 * LANDING.  NO SPLIT.  The .s holds one function and no data at all, so this
 * whole-file .c replaces it and the object keeps its name.  FOUR linker-script
 * lines name the .o, all in overlays/rom_7e0928/overlay.ld, matched on the FULL
 * PATH and cited by content:
 *
 *     line  60   asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_a.o(.text)   in .text
 *     line  98   asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_a.o(.data)   in .data
 *     line 102   asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_a.o(.data1)  in .data
 *     line 109   asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_a.o(.bss)    in .bss
 *
 * ALL FOUR must be repointed at src/overlays/rom_7e0928/ovl_30_c_c_c_c_c_a.o.
 * The .data, .data1 and .bss lines are placeholders for sections this .o does
 * not have and contribute zero bytes today -- exactly the "remap EVERY section,
 * not the one that errors" / "a .data line in the linker script is not evidence
 * of data" trap, and this file is the first in the corpus to carry THREE such
 * placeholders rather than one.  The .text line sits between
 * ovl_30_c_c_c_c_b.o(.text) and ovl_30_c_c_c_c_c_b.o(.text) and the order must
 * not move.  Four other overlay.ld files (rom_7f21b8, rom_7c37ac, rom_79dd90,
 * rom_7ed0a0) contain the same BASENAME on a different full path; they are
 * different files and must not be touched.  The file builds through the
 * existing cross-dir `asm/%.o: src/%.c` rule at the default GCC296_CFLAGS -O2 --
 * no new Makefile rule, and therefore no wildcard that could capture a sibling.
 */
#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

extern unsigned char gState[];

extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __ActorMessage(int slot, int a);
extern void __SetCameraTarget(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Surprise(int slot, int a);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_common1_2c4(void);
extern int OvlFunc_common1_4cc(int a, int b);
extern void OvlFunc_common1_588(int a, int b);
extern void OvlFunc_common1_1254(int a);
extern void OvlFunc_common1_1314(int a);
extern void OvlFunc_956_200a2c4(int a, int b, int c);
extern void OvlFunc_956_200a2f4(int a, int b, int c);

void OvlFunc_956_200a330(int a)
{
    unsigned char *g;
    unsigned char *p;
    int r;
    int i;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
        return;
    }
    __CutsceneStart();
    r = OvlFunc_common1_4cc(a, 5);
    if (r == 0) {
        __MessageID(0x20c3);
        __Func_80933d4(0xc0 << 10, 0xc0 << 7);
        __Func_80933f8(0x87 << 19, -1, 0xa8 << 16, 1);
        __Func_8093530();
        __CutsceneWait(0x1e);
        __ActorMessage(a, 0);
        __ActorMessage(a, 0);
        OvlFunc_common1_1078(0, 0xf6 << 2, 0xb8);
        { PIN2; q0 = 0; q1 = 0xc0 << 9;
          __MapActor_SetSpeed(q0, q1, 0xc0 << 8); }
        OvlFunc_956_200a2f4(0, 0xf8 << 2, 0xb8);
        { PIN2; q0 = 0; q1 = 0x4ccc;
          __MapActor_SetSpeed(q0, q1, 0x2666); }
        OvlFunc_956_200a2c4(0, 0x8c << 3, 0xb8);
        __CutsceneWait(0x78);
        __MapActor_Surprise(0, 0x101);
        __CutsceneWait(0x78);
        OvlFunc_common1_1314(0);
        __MapActor_SetAnim(0, 1);
        { PIN1; q0 = 0;
          __MapActor_Surprise(q0, 0x80 << 1); }
        { PIN1; q0 = 0;
          __MapActor_Emote(q0, 0x105, 0); }
        p = __MapActor_GetActor(0);
        for (i = 0x77; i >= 0; i--) {
            if (*(int *)(p + 8) > (0xf8 << 18))
                *(int *)(p + 8) -= 0x13333;
            __WaitFrames(1);
        }
        { PIN1; q0 = 0;
          __MapActor_Emote(q0, 0x103, 0x3c); }
        OvlFunc_956_200a2c4(0, 0x8c << 3, 0xb8);
        __ActorMessage(a, 0);
        OvlFunc_common1_1314(0);
        gState[0xf9 << 1] = 1;
        OvlFunc_common1_1254(0);
        __SetCameraTarget(0, 0);
        OvlFunc_common1_588(a, 5);
    } else if (r == 1) {
        __MessageID(0x20c2);
        __ActorMessage(a, 0);
    }
    OvlFunc_common1_5e4(r, a, 5);
    __CutsceneEnd();
}
