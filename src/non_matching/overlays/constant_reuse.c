/* Three more overlay functions, three DIFFERENT ways gcc reuses a value the
 * ROM recomputes. All three are single-function files, so they were chosen
 * because they contain no shift at all and therefore cannot hit the
 * interleaved-argument blocker in interleaved_arg_setup.c. They hit this
 * instead, which looks like the same underlying disposition seen from three
 * angles: gcc-2.96 as invoked here is more eager to reuse than the original
 * build was.
 *
 *   OvlFunc_946_2009624  asm/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_c.s
 *       Two stack arguments. The ROM builds both into separate registers and
 *       then stores both; gcc builds one, stores it, and REUSES r3 for the
 *       second.
 *           rom    mov r3, #8 / mov r2, #0x15 / str r3, [sp] / str r2, [sp, #4]
 *           ours   mov r3, #8 / str r3, [sp] / mov r3, #0x15 / str r3, [sp, #4]
 *       Naming the two values as locals before the call does not separate
 *       them; it costs an instruction instead (17 vs 16).
 *
 *   OvlFunc_909_200828c  asm/overlays/rom_79c738/ovl_30_c_c_a_c_a_a_c.s
 *       The save-bit id 0x303 appears twice, once for __GetFlag and once for
 *       __SetFlag. The ROM loads it from the pool BOTH times; gcc common-
 *       subexpressions it into r5, which then has to be callee-saved and
 *       costs a push/pop pair as well.
 *
 *   OvlFunc_965_2009158  asm/overlays/rom_7ef4f4/ovl_30_a_c_c_a_c.s
 *       Three arguments of -1. The ROM materialises each independently
 *       (mov #1 / neg, three times); gcc builds one and copies it, coming out
 *       two instructions SHORTER than the ROM (15 vs 17).
 *
 * COUNTEREXAMPLE, 2026-08-03 -- read this before acting on the paragraph
 * below. Func_80167ac (src/non_matching/rom_15000/rom_167ac.c) goes the OTHER
 * WAY. There the ROM derives its second constant from its first
 * (`ldr r4, =0xeae` ... `sub r4, #2` for 0xeac) and gcc loads each fresh:
 *
 *     rom    ldr r4, =0xeae ... sub r4, #2 ... ldr r1, =0xea8
 *     ours   ldr r3, =0xeae ... ldr r3, =0xeac ... ldr r3, =0xea8
 *
 * So the disposition is NOT uniform, and "gcc reuses more than the original
 * build did" is too simple a statement of it. Something decides case by case,
 * and a single flag or build difference cannot explain both directions. That
 * also explains why eleven flags changed nothing: there is no one knob whose
 * setting would fix all of these, because they do not all want the same
 * setting.
 *
 * The common thread below still holds for the three functions in this file,
 * and is worth stating as a lead rather than three separate dead ends -- but
 * it is a local pattern, not a global one: in each of these three the ROM is
 * LESS eager to reuse a value than gcc is here. Something about the original invocation -- a flag, or a
 * compiler build difference -- suppresses common-subexpression elimination
 * and register reuse that this toolchain performs. Finding it would move all
 * three at once, and probably a great deal of the overlay corpus, which is
 * full of exactly these formulaic call sequences.
 *
 * Screened at -O2 and -O1; both give the same result.
 *
 * FLAG SWEEP: RULED OUT. Eleven flags that plausibly affect reuse or common-
 * subexpression elimination were tried against two of these functions, and
 * every one produced output identical to the baseline:
 *
 *     -fno-cse-follow-jumps      -fno-rerun-cse-after-loop
 *     -fno-expensive-optimizations -fno-caller-saves
 *     -fno-force-mem             -fno-defer-pop
 *     -fno-strength-reduce       -fno-thread-jumps
 *     -fno-peephole              -fno-function-cse
 *
 * (tools/tryc.py grew a --cflags passthrough so this kind of hypothesis can be
 * tested against a known-failing function without editing anything.)
 *
 * So the difference is NOT reachable by a compiler flag. That leaves two
 * candidates: the C here differs from the original in a way not yet guessed,
 * or camelot-gcc's build of gcc-2.96 differs from the one Camelot used in a
 * way these functions expose. The first is far more likely -- the same
 * toolchain reproduces 2169 other files byte-for-byte -- which points back at
 * the reading of the source rather than the invocation.
 */

/* --- OvlFunc_946_2009624 ------------------------------------------------- */
extern void __SetFlag(int flag);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

/* Map edit: one attribute copy, recorded with save bit 0x8C4. */
void OvlFunc_946_2009624(void)
{
    __SetFlag(0x8c4);
    __Func_8010704(0, 0, 1, 1, 8, 0x15);
}

/* --- OvlFunc_909_200828c ------------------------------------------------- */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int flag);
extern void __ActorMessage(int slot, int arg);

/* Talk: line 0x1756, replaced by 0x176C once save bit 0x303 is set -- so the
 * second line is what a returning player hears. Sets the bit on the way out.
 */
void OvlFunc_909_200828c(void)
{
    __CutsceneStart();
    __MessageID(0x1756);
    if (__GetFlag(0x303) != 0)
        __MessageID(0x176c);
    __ActorMessage(0xf, 0);
    __SetFlag(0x303);
    __CutsceneEnd();
}

/* --- OvlFunc_965_2009158 ------------------------------------------------- */
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8092708(int a, int b, int c);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);

void OvlFunc_965_2009158(void)
{
    __Func_80933f8(-1, -1, -1, 0);
    __Func_8092708(0, 6, 0);
    __MapTransitionOut();
    __WaitMapTransition();
}
