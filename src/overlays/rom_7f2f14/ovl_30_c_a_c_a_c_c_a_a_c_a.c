/* OvlFunc_968_2009780  --  0x02009780  MATCHING (fakematch: one r0 register pin)
 *
 * 53 instructions, 136 bytes, exact.  Measured 3x.
 *
 * THIS TU NEEDS AN EXPLICIT -O2 RULE IN THE MAKEFILE, the EIGHTH for this
 * family.  It is caught by the mis-scoped rom_7f2f14/ovl_30_c_a_c_a_c_c%
 * wildcard, which applies O1_CFLAGS: 15 differing at -O1 (the park's plain C is
 * 13 at -O1 and 2 at -O2), EXACT at -O2.  Copy the rule that the file-mate
 * ovl_30_c_a_c_a_c_c_a_a_c_b.o used in batch 258:
 *
 *     asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_c_a.o: \
 *             src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_c_a.c
 *             $(GCC296_CC) $(GCC296_CFLAGS) -S -o $(@:.o=.s) $<
 *             ...
 *
 * Without it this screens green and builds red.
 *
 * WHOLE-FILE CONVERSION.  asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_c_a.s
 * holds exactly one .thumb_func_start (its file-mate 2009808 was split off in
 * batch 258), carries no .data/.bss/.word/.byte, and asmfacts reports
 * "WHOLE  convert directly".  overlays/rom_7f2f14/overlay.ld:62 keeps its line
 * VERBATIM, asm/ prefix included:
 *
 *     asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_a_c_a.o(.text)
 *
 * THE PARK WAS RIGHT ABOUT THE RESIDUE AND WRONG ABOUT THE CEILING.  The old
 * note (src/non_matching/ovl_7f2f14/2009780.c -- a comment-only stub that
 * compiles to nothing) ended at "argument-construction interleave, and NO LEVER
 * IS AVAILABLE".  At -O2 the residue really was two adjacent instructions,
 * swapped, at the one __Func_8092adc call:
 *
 *     rom   mov r1, #0xe0 / mov r2, #0 / mov r0, #0xa / lsl r1, #8
 *     ours  mov r1, #0xe0 / mov r2, #0 / lsl r1, #8   / mov r0, #0xa
 *
 * and the lever is a PIN1 on r0 -- which the park never tried, because it had
 * reasoned the lever must be "name the constant in a dominating block" and this
 * function is straight-line with `push {lr}` alone.
 *
 * WHY THE r0 PIN WORKS, AND WHAT IT IS ACTUALLY DOING.  The pinned value ends
 * up in r0 either way -- it is the first argument, so gcc was always going to
 * put it there.  The pin buys nothing in ALLOCATION; it changes the ORDER THE
 * MOVE IS EMITTED IN, which is what sched2 breaks its ties on.  Compare the
 * pre-sched2 stream (-fno-schedule-insns2) with and without the pin:
 *
 *     no pin   mov r1,#224 / lsl r1,#8 / mov r0,#10 / mov r2,#0
 *     pinned   mov r0,#10  / mov r1,#224 / lsl r1,#8 / mov r2,#0
 *
 * Unpinned, the shifted constant is PRECOMPUTED into a pseudo before the simple
 * arguments are loaded, so the whole mov+lsl pair sits at LUID 0-1 and `mov r0`
 * lands after it.  (The `lsl` itself is created post-reload, by the thumb
 * shiftable-const define_split, at the position of the single `mov r1,#0xe000`
 * insn -- so the split inherits that LUID.)  The pin makes the r0 store a
 * statement of its own AHEAD of the argument list, so `mov r0` gets LUID 0 and
 * wins the scheduler's tie against `lsl r1`.  The whole function is ONE basic
 * block -- no labels, no jumps -- so sched2 sees all 53 instructions at once and
 * only the LUID order separates the equal-priority argument moves.
 *
 * This is the same site shape, same constant and same callee as the pinned
 * `{ PIN1; q0 = 9; __Func_8092adc(q0, 0xe0 << 8, 0); }` in
 * src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_c_c_c_b.c, which is where
 * `tools/whodoesthis.py` found it.
 *
 * AXES ELIMINATED, all measured at -O2 against 53 encodings / 136 bytes:
 *
 *     the park's plain C (baseline)                          2
 *     `int slot = 0xa;` at the top, used at all five sites    2  (INERT)
 *     `slot = 0xa;` immediately before the call               2  (INERT)
 *     `int x = 0xe0 << 8;` at the top                        40  (WORSE, +2
 *                                                               insns: it wins
 *                                                               a callee-saved
 *                                                               register and
 *                                                               buys push/pop)
 *     `x = 0xe0 << 8;` immediately before the call            2  (INERT)
 *     `0xe000` written flat instead of `0xe0 << 8`            2  (INERT)
 *     dropping the __Func_8092adc prototype                   2  (INERT)
 *     `int z = 0;` at the top for the third argument         11  (WORSE)
 *     `do { } while (0);` before the call                     3  (WORSE)
 *     -fno-schedule-insns                                     2  (INERT)
 *     -fno-schedule-insns2                                   13  (REGRESSES --
 *                                                               sched2 is
 *                                                               already right
 *                                                               about the other
 *                                                               four call sites)
 *     -fno-peephole2 / -fno-gcse / -fno-rerun-cse-after-loop  2  (all INERT)
 *     -fno-strict-aliasing / -fno-expensive-optimizations     2  (both INERT)
 *     -fno-force-mem / -fno-cse-follow-jumps / -fno-caller-saves
 *                                                             2  (all INERT)
 *     -fno-delayed-branch / -fno-function-cse                 2  (both INERT)
 *     -fno-sched-interblock / -fno-sched-spec / -fno-regmove  2  (all INERT)
 *     -fno-strength-reduce / -fno-thread-jumps / -fno-defer-pop
 *                                                             2  (all INERT)
 *     -fno-inline / -fno-optimize-sibling-calls               2  (both INERT)
 *     -fno-reorder-blocks                                     2  (INERT)
 *     -fno-omit-frame-pointer                                47  (WORSE)
 *     { PIN1; q0 = 0xa; __Func_8092adc(q0, 0xe0 << 8, 0); }   EXACT
 *
 * So NO FLAG reaches it and no plain-C spelling reaches it; the pin is the only
 * thing measured that does.  PIN1 is already the narrowest pin (r0 only) and
 * the only site pinned.
 *
 * THE TWO HALFWORD STORES STILL NEED THE TYPED FIELDS, and that part of the
 * park's note is correct and is kept.  Written through a cast,
 * `*(short *)(p + 0xcba) = 0` pools the literal -- `ldr r3, =0x0` where the ROM
 * has `mov r3, #0` -- and the same at +0xcb6 with the 1.  Declaring the object a
 * struct with `short` members at those two offsets gives the `mov` form for
 * both, in a scratch register, at no cost.
 *
 * THE DEBT.  The pin is scaffolding; the name goes in fakematch.txt.  What
 * would retire it is any plain-C construct that gets the first argument's move
 * emitted ahead of the shifted argument's precompute.  Everything tried above
 * either leaves the precompute first or costs a register.
 *
 * Harness: scratch_elev/b262/OvlFunc_968_2009780 -- screen_all.sh (olevel at
 * -O2 over a list of candidates), dump.sh (side-by-side pre/post-sched2
 * streams), v1..v9 the measured spellings.
 */

struct Ctx {
    unsigned char pad000[0xcb6];
    short fcb6;
    unsigned char padcb8[2];
    short fcba;
};

extern struct Ctx *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __ActorMessage(int a, int b);

#define PIN1 register int q0 __asm__("r0")

void OvlFunc_968_2009780(void)
{
    struct Ctx *p;

    p = iwram_3001ebc;
    p->fcba = 0;
    p->fcb6 = 1;
    __CutsceneStart();
    __MessageID(0x267d);
    __Func_809280c(0xa, 0, 0);
    __CutsceneWait(0xa);
    __Func_8093040(0xa, 0, 0x14);
    /* FAKEMATCH.  See the header: the pin is a SCHEDULING lever, not an
     * allocation one -- r0 is where this argument goes anyway, and the pin only
     * gets its `mov` emitted ahead of the shifted second argument. */
    { PIN1; q0 = 0xa;
      __Func_8092adc(q0, 0xe0 << 8, 0); }
    __Func_80933d4(0x80 << 9, 0x80 << 6);
    __Func_80933f8(0xe0 << 17, -1, 0xd8 << 17, 1);
    __Func_8093530();
    __ActorMessage(0xa, 0);
    __CutsceneEnd();
}
