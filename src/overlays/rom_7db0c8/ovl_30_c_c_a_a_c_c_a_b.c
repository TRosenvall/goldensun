/* OvlFunc_954_2008270  --  0x02008270    EXACT
 *
 * Cut from asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a.s (the FIRST of that
 * file's three functions).  81 instructions, 85 encodings, 204 bytes.  A
 * camera cutscene: set save bit 0x301, take actor 0xd, move the camera, walk
 * two actors to fixed speeds and wait, then one six-argument UI call.
 *
 * VERDICT
 *   OK OvlFunc_954_2008270 -- 204 bytes, 85 encodings and 14 relocations identical
 * (against the ORIGINAL asm/ path.)
 *
 * THE `neg` HALF OF THE INTERLEAVE CLASS IS REACHABLE FROM A BARE CALL.  This
 * is the headline.  docs/elevation.md's "The `neg` interleave family is SOLVED
 * as a class" concludes that the eleven-member family's ten unsolved members
 * are all straight-line and therefore belong to the 98 that are out of reach.
 * The site here is that exact shape, and this function has no branch before it:
 *
 *     mov r0,#0x96 / mov r1,#1 / mov r2,#0xc8 / lsl r2,#16 / mov r3,#1 /
 *     lsl r0,#18 / neg r1,r1
 *     bl  __Func_80933f8
 *
 * Three split builds (two mov+lsl and one mov+neg) with a fourth whole-value
 * argument threaded through them.  What produces it is
 *
 *     __Func_80933f8(0x96 << 18, -1, 0xc8 << 16, 1);
 *
 * -- A PLAIN CALL WITH WHOLE-VALUE ARGUMENTS.  No pins, no locals, no barrier,
 * no dominating block.  A PIN4 fill in ascending order is also exact but
 * measures EXACTLY INERT and is not shipped.  So for this shape the cure is
 * not "pin the single-instruction arguments" -- it is "DO NOT TRANSCRIBE THE
 * ROM'S SHIFT ORDER", write each argument as one whole value, and let sched2
 * produce the interleave.  Pinning is what you reach for when that alone is
 * not enough, not the starting point.
 *
 * A FLAG THAT TURNED OUT TO BE A SYMPTOM OF A PIN.  The first working
 * candidate needed `-ffixed-r7` (FIXEDR7_CFLAGS, Makefile:287): the ROM's
 * callee-saved set is r5/r6/r8/r10 and skips r7, and with the pin set of the
 * day gcc used r5/r6/r7/r8 and came out four lines short.  After the pin
 * minimisation below, the SAME SOURCE is exact with and without the flag, and
 * what ships needs no Makefile rule.  Recorded because it inverts the usual
 * direction of "a pin can be a symptom of a different defect": here a FLAG was
 * the symptom, and the defect it was papering over was an over-pinned call.
 * RE-TEST THE FLAG GROUP AFTER MINIMISING, not only before.
 *
 * WHAT IS ACTUALLY LOAD-BEARING, at a fixpoint over four rounds:
 *   1. TWO ACTOR VARIABLES.  The two __MapActor_GetActor results go to `e` and
 *      `e2` even though the ROM reuses r5 for both.  One reassigned variable is
 *      81 of 85 differing and FOUR LINES SHORTER.  This is the recorded "Two
 *      results of the same call need two pointer variables", and the ROM
 *      sharing the register is not evidence against it.
 *   2. ONE PIN, ON r0, AT THE FIRST __Actor_TravelTo.  Dropping it costs 2;
 *      pinning r1, r2 or r3 as well is inert.  The second TravelTo needs none.
 *   3. TWO NAMED STACK-ARGUMENT LOCALS at __Func_8010704.  Inline literals cost
 *      3: the ROM materialises 0x29 and 0xc into two registers and stores both,
 *      ours reuses r3.  The recorded per-call-site stack-argument rule.
 *   4. THE TWO FIELD STORES WANT OPPOSITE SOURCE ORDERS AT THE TWO SITES.
 *      `e->f34 = vy; e->f30 = vx;` at the first actor and `e2->f30 = vx;
 *      e2->f34 = vy;` at the second.  Making both the same costs 5 or 8
 *      depending on which way.  Same two stores, same two values, same struct,
 *      opposite order -- "A SIBLING'S CURE FOR A SAME-SHAPED BLOCK CAN BE
 *      ACTIVELY WRONG", now confirmed WITHIN one function.
 *   5. THE THREE CONSTANTS ARE NAMED (`z`, `vx`, `vy`).  The ROM holds 0,
 *      0xcccc and 0x6666 in r10/r6/r8 across the calls, which is the ROM doing
 *      the constant-CSE this project usually fights.  Inline literals at both
 *      sites are 25 differing.  When the ROM's prologue saves registers that
 *      hold CONSTANTS rather than pointers, name them -- that is the mirror
 *      image of "read the prologue by content".
 *
 * MEASURED-WORSE TABLE (out of 85 encodings, against the final source):
 *   spelling                                                        differing
 *   ---------------------------------------------------------------  -------
 *   one actor variable instead of two   81, and 4 LINES SHORTER
 *   first __Actor_TravelTo: no pins                                         2
 *   first __Actor_TravelTo: r1/r2/r3 pinned as well                         0 (inert)
 *   __Func_80933f8: PIN4 ascending fill                                     0 (inert)
 *   __Func_80933d4: PIN2 fill                                               0 (inert)
 *   __Func_8010704: stack args as inline literals                           3
 *   both field-store pairs f34-first                                        5
 *   both field-store pairs f30-first                                        8
 *   z/vx/vy as inline literals                                             25
 *   vx/vy assigned before z            54, and 1 LINE LONGER
 *
 * LANDING NEEDS A SPLIT.  The .s holds THREE functions and no data at all --
 * no .section, .data, .word, .byte or .incbin:
 *     line  13  .thumb_func_start OvlFunc_954_2008270   (.func_end line 95)
 *     line 104  .thumb_func_start OvlFunc_954_200833c   (.func_end line 211)
 *     line 219  .thumb_func_start OvlFunc_954_200842c   (.func_end line 263)
 * This function is FIRST and carries NO local labels at all; the other two own
 * .L3e0/.L3ea/.L3f2/.L44c, so the label sets are disjoint and nothing has to be
 * exported.  Exactly ONE linker-script line names the object, and it is a .text
 * line:
 *     overlays/rom_7db0c8/overlay.ld:27
 *         asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a.o(.text)
 * MATCH ON THE FULL PATH: the basename `ovl_30_c_c_a_a_c_c_a` also appears in
 * rom_7ced6c, rom_7987ac and rom_7d30e0 and those are different files.
 * The split replaces that line with, in order,
 *     src/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a_a.o(.text)   (2008270, this)
 *     asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a_b.o(.text)   (200833c + 200842c)
 * Both names are free in rom_7db0c8.  Default -O2 rule; no flag group.
 */
struct Actor {
    unsigned char pad0[8];
    int f8;                             /* 0x08 */
    int fc;                             /* 0x0c */
    int f10;                            /* 0x10 */
    unsigned char pad14[0x30 - 0x14];
    int f30;                            /* 0x30 */
    int f34;                            /* 0x34 */
    unsigned char pad38[0x55 - 0x38];
    unsigned char f55;                  /* 0x55 */
};

extern void __SetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Actor_SetAnim(struct Actor *a, int anim);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct Actor *a);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_954_2008270(void)
{
    struct Actor *e;
    struct Actor *e2;
    int z;
    int vx;
    int vy;

    __SetFlag(0x301);
    e = __MapActor_GetActor(0xd);
    __CutsceneStart();
    __Func_80933d4(0x80 << 10, 0x80 << 7);
    __Func_80933f8(0x96 << 18, -1, 0xc8 << 16, 1);
    __Actor_SetAnim(e, 3);
    __Func_8093530();
    z = 0;
    e->f55 = z;
    vx = 0xcccc;
    vy = 0x6666;
    e->f34 = vy;
    e->f30 = vx;
    { register struct Actor *q0 __asm__("r0"); q0 = e;
      __Actor_TravelTo(q0, e->f8, 0x80 << 12, e->f10); }
    e2 = __MapActor_GetActor(0xe);
    e2->f55 = z;
    e2->f30 = vx;
    e2->f34 = vy;
    __Actor_TravelTo(e2, e2->f8, 0x80 << 14, e2->f10);
    __Actor_WaitMovement(e2);
    __CutsceneWait(0x2d);
    { int s1 = 0x29; int s2 = 0xc; __Func_8010704(0x2b, 0xc, 1, 1, s1, s2); }
    __CutsceneEnd();
}
