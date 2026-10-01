/* repro_commoning.c -- THE 800+ STRAIGHT-LINE COMMONING BLOCKER, REPRODUCED IN
 * FIVE FUNCTIONS AND EIGHT SOURCE LINES, with the mechanism decided rather than
 * exhibited.  Batch 311 brief D.
 *
 * Build:
 *   docker run --rm -v "$PWD:/work" -w /work goldensun-build sh -c \
 *     'cd /work/scratch_elev/b311d && /opt/gcc296/xgcc -B/opt/gcc296/ -O2 \
 *        -mthumb -mthumb-interwork -mcpu=arm7tdmi -fno-builtin -nostdinc \
 *        -ffreestanding -fcall-used-r4 -I/work/include -da -S -o repro.s \
 *        repro_commoning.c'
 * (production flags, from tools/tryc.py CFLAGS; add -da for the dumps cited.)
 *
 * THE QUESTION THIS ANSWERS.  docs/band-800plus.md section 2 established that
 * cse.c pass 1 commons repeated multi-instruction constants across call
 * boundaries and that no flag reaches it.  Brief D narrowed the remaining
 * candidates to "the cost model or the RTL shape".  IT IS THE RTL SHAPE, and
 * the cost model is ruled out by f_narrow_pseudo below.  Measured output:
 *
 *   function           what it passes twice      prologue        commoned?
 *   ---------------------------------------------------------------------------
 *   f_wide             0xba<<18, mov+lsl         push {r5,lr}    YES, r5
 *   f_pool             0x6666, pool load         push {r5,lr}    YES, r5
 *   f_narrow           0x12, mov #imm8           push {lr}       NO, rebuilt
 *   f_wide_once        two wide, each used once  push {lr}       n/a
 *   f_narrow_pseudo    0x12 as an `orr` operand  push {r5,lr}    YES, r5
 *
 * READ THOSE FIVE ROWS IN PAIRS AND THE MECHANISM FALLS OUT:
 *
 *   f_wide vs f_narrow says the discriminator correlates with the constant's
 *   WIDTH, which is what the band doc assumed.
 *
 *   f_wide_once says the width is NOT itself the cost.  A wide constant used
 *   once goes straight into r1 as `mov r1,#186 / lsl r1,r1,#18` and costs no
 *   call-saved register at all.  So the expense is cse1 finding a SECOND
 *   occurrence, not the constant being wide.
 *
 *   f_narrow_pseudo DECIDES IT, and it falsifies the width reading.  0x12 is a
 *   one-instruction constant -- the cheapest thing in the instruction set -- and
 *   as an `orr` operand it is commoned into r5 across three calls anyway:
 *       mov r5, #18 ... orr r3,r3,r5 ... orr r3,r3,r5 ... orr r3,r3,r5
 *   A cost model that declines to common `mov #imm8` in f_narrow cannot also
 *   common it here.  So cse's cost comparison plays NO ROLE.
 *
 * WHAT ACTUALLY DIFFERS IS WHICH REGISTER CLASS THE EXPANDER TARGETS, and
 * `-da`'s 00.rtl shows it in one screen.  Inside a SINGLE argument setup of
 * OvlFunc_964_200a59c's own `__MapActor_SetPos(0x12, 0xba<<18, 0x88<<16)`:
 *
 *   (insn 835 (set (reg:SI 172)   (const_int 48758784)))   <- PSEUDO
 *   (insn 837 (set (reg:SI 173)   (const_int 8912896)))    <- PSEUDO
 *   (insn 839 (set (reg:SI 0 r0)  (const_int 18)))         <- HARD REG, direct
 *   (insn 841 (set (reg:SI 1 r1)  (reg:SI 172)))
 *   (insn 843 (set (reg:SI 2 r2)  (reg:SI 173)))
 *
 * and the repro's own 00.rtl shows the same split with nothing else in the
 * function: f_wide gets `(set (reg:SI 32) (const_int 48758784))` plus
 * `(set (reg:SI 1 r1) (reg:SI 32))`, while f_narrow gets
 * `(set (reg:SI 1 r1) (const_int 18))` with no pseudo anywhere.
 *
 * THE RULE, in four steps, each attributable to a named place:
 *
 *   1. EXPAND.  A CONST_INT the Thumb backend can move in one instruction is
 *      emitted directly into the hard argument register.  One it cannot -- wide
 *      (`mov`+`lsl`) or pooled -- is force_reg'd into a fresh pseudo and copied.
 *      This decision is made from the VALUE and the ARGUMENT POSITION alone.
 *   2. cse.c pass 1.  `invalidate_for_call` invalidates HARD REGISTERS ONLY.
 *      A pseudo holding a constant therefore survives every intervening call;
 *      a hard argument register does not.
 *   3. So cse1 deletes the second and later BUILDS of any pseudo-held constant
 *      in the block and rewrites those sites as register copies carrying
 *      REG_EQUAL, leaving one pseudo live across every call between them.
 *      Counted per dump on 964, the constant 48758784: 00.rtl 3, 01.sibling 3,
 *      02.jump 3, 03.cse 7 -- the 3 -> 7 step names the pass in one command.
 *      Narrow constants in argument position are rebuilt at every site.
 *   4. REGISTER ALLOCATION must park the surviving pseudo somewhere call-saved.
 *      With -fcall-used-r4 only r5/r6/r7 are call-saved low registers, so the
 *      overflow lands in r8-r11, which Thumb-1 cannot use as operands: a
 *      `mov rlo,rhigh` per use, plus the six-instruction high-save prologue and
 *      its epilogue.
 *
 * WHY NO SOURCE SHAPE CAN REACH IT.  The pseudo is invented at step 1, before
 * any pass a source spelling could steer, and the commoning at step 3 operates
 * on that pseudo rather than on anything the source named.  This PREDICTS the
 * band doc's two negative results instead of merely recording them:
 * one-variable-per-region and declaration order are byte-identical because
 * "region-scoping cannot reach a pseudo the compiler invented" is precisely
 * this pseudo.  It also predicts the one thing that DOES move a candidate --
 * changing the SET of long-lived quantities -- because that is the only thing
 * source controls: how many OTHER values compete with the commoned pseudo for
 * r5/r6/r7 before the allocator reaches r8.  On OvlFunc_964_200a59c un-naming
 * four locals went +44/+21 -> +8/+3 and bought the reference's exact prologue.
 *
 * ========== HOW MUCH THE REFERENCES COMMON: A CORRECTION, MADE IN BATCH ======
 *
 * The first version of this file claimed the references "essentially never
 * common a wide constant in argument position", on a count of which REGISTER
 * each wide build targets: over all five of brief D's references, 445 of 450
 * wide builds go into an argument register r0-r3 and only 5 into a callee-saved
 * one.  THAT CLAIM IS WRONG, and it is retracted here rather than anywhere else
 * so that the number it rested on cannot be cited without the correction.
 *
 * It was caught by tracing ONE value instead of trusting the aggregate.  In
 * OvlFunc_888_200888c the value 0x90<<5 is built FOUR times -- at lines 636,
 * 667 and 894 into r5, and at 658 and 885 into r1 inside the function's two
 * loops -- and fed to arguments as `mov r1,r5` at lines 642, 647, 671, 676, 831,
 * 836, 898 and 903.  That is the reference commoning a wide ARGUMENT constant
 * into a callee-saved register, repeatedly.  The build-register count missed it
 * because a value built once and copied eight times is one build and eight
 * copies, so counting builds counts the wrong thing.
 *
 * THE RIGHT MEASURE IS THE COPIES, and it is still one grep: `mov rARG, rSAVED`
 * is a commoned value entering argument position.
 *
 *   reference            insns   copies   registers used
 *   OvlFunc_925_2009af0   1876       2     r5
 *   OvlFunc_924_200bd20   1210       5     r5 r6 r7 r8
 *   OvlFunc_888_200888c   1524       8     r5
 *   OvlFunc_964_200a59c    993      10     r5 r8
 *   OvlFunc_959_200b054   2123      57     r5
 *
 * So the references common, and OvlFunc_959_200b054 commons HARDER than we do --
 * 57 copies out of one callee-saved register, on a `push {r5,lr}` prologue.
 * The difference is not whether commoning happens but HOW MANY DISTINCT VALUES
 * it happens to.  Ours spreads it across the whole call-saved bank; the
 * references concentrate it in one or two registers and rebuild the rest.
 *
 * ON OvlFunc_964_200a59c, MEASURED:  reference 10 copies out of r5 and r8.
 * Our candidates:
 *
 *   cand  size  count  copies   prologue high-register saves
 *   v1    +56    +26     46     r8 AND sl   (two)
 *   v2    +44    +21     46     r8 AND sl   (two)
 *   v4    +12     +5     33     r8 only  -- MATCHES THE REFERENCE
 *   v7     +8     +3     33     r8 only
 *   v10    +8     +3     33     r8 only
 *   ref     --     --     10     r8 only
 *
 * The copy count is therefore a REAL SCREEN and it is one grep: it fell 46 -> 33
 * at exactly the edit that fixed the prologue, which is the same edit that took
 * size and count from +44/+21 to +12/+5.  It correlates with the structural win
 * rather than with the saturated aligned figure, which is what brief D's
 * "figures that lie" ladder asks of an instrument.
 *
 * ITS LIMIT, stated so it is not over-trusted: it is COARSE at the end.  It
 * cannot tell v4 (+12/+5) from v10 (+8/+3) -- all three of the last candidates
 * read 33.  Use it to find the big structural gap, then go back to size-and-count.
 *
 * AND IT DOES NOT CLOSE THE GAP.  Our best candidate still makes 33 copies
 * against the reference's 10, and no source edit found in this batch moves that
 * remainder, for the reason given above: the pseudo is the expander's, not the
 * source's.  What the lever reaches is the OTHER long-lived quantities competing
 * for r5/r6/r7, which is why un-naming four locals bought the prologue and then
 * stopped buying anything.
 *
 * THE FIVE CALLEE-SAVED WIDE BUILDS are still worth having inspected, because
 * four of them are NOT cse commoning and should not be read as such:
 *   - 888_200888c lines 667 and 894 and 924_200bd20 lines 570-571 are builds at
 *     a LOOP HEAD, where the value has to live across the loop body whatever cse
 *     does.  888's own loop BODIES rebuild the same value into r1 every
 *     iteration (lines 658-660, 885-887), which is the giveaway.
 *   - 925_2009af0 line 84 is a STORE operand, not an argument, so a pseudo by
 *     step 1 -- exactly what f_narrow_pseudo predicts.
 *
 * HOW TO SCREEN A CANDIDATE FOR IT, one command, no dumps:
 *   grep -cE '^\tmov\t(r[0-3]), (r[5-7]|r8|sl|fp)$' ref_<F>.s <cand>.gen.s
 * Ours MANY TIMES the reference's copy count IS the blocker.  widecheck.py in
 * this directory counts builds instead and is the weaker of the two -- see the
 * correction above before using it.
 * The immediate multiset says the same thing even faster and was the signal that
 * found it here: every deficit a `mov` base or an `lsl` shift, ours short and
 * never over.
 *
 * WHAT IS STILL OPEN.  Whether step 1's force_reg can be reached at all without
 * editing the compiler.  Twelve flag and -O settings are already measured inert
 * (band doc section 2) and the mechanism explains why: they all tune cse, and
 * cse is behaving correctly.  A flag that changes the ARGUMENT EXPANSION path is
 * what would be needed, and none is known in gcc 2.96.  Until one is found, the
 * honest status of the straight-line population is: the arithmetic is reachable,
 * the constant SET is reachable, the prologue is reachable by balancing the
 * long-lived set, and the last few instructions per repeated wide argument are
 * NOT reachable from C.
 */
/* Minimal reproduction of the band-800plus straight-line commoning blocker.
   f_wide passes a WIDE constant (needs mov+lsl) twice across a call.
   f_narrow passes a NARROW constant (fits mov #imm8) twice across a call. */
extern void g(int slot, int x);

void f_wide(void)
{
    g(1, 0xba << 18);
    g(2, 0xba << 18);
}

void f_narrow(void)
{
    g(1, 0x12);
    g(2, 0x12);
}

/* Third class: a constant too wide for mov+lsl, so it must come from the
   literal pool.  888/925/959 reload one of these at every site. */
void f_pool(void)
{
    g(1, 0x6666);
    g(2, 0x6666);
}

/* Control: the wide constant used ONCE only -- no commoning is possible,
   so this isolates the cost of the commoning from the cost of the value. */
void f_wide_once(void)
{
    g(1, 0xba << 18);
    g(2, 0x88 << 16);
}

/* CONTROL that decides between "cse cost model" and "which register class the
   expander targets".  0x12 is narrow, but as an `orr` operand it is NOT a call
   argument, so expand must give it a pseudo.  If it is commoned across the
   calls, cost is irrelevant and the destination register class is everything. */
extern unsigned char *h(int);
void f_narrow_pseudo(void)
{
    *h(1) |= 0x12;
    *h(2) |= 0x12;
    *h(3) |= 0x12;
}
