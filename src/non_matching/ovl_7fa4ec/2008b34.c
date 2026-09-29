/* OvlFunc_970_2008b34 -- NON-MATCHING, 155 of 255 encodings differ.
 * Unattempted before batch 298.  Reference asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_a_c.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7fa4ec/2008b34.c \
 *       asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_a_c.s --func OvlFunc_970_2008b34
 *
 * Size 624 both; count 254 against 255, so ONE INSTRUCTION SHORT and the figure is
 * not a distance.  aligncmp reads 218 of 255 (50 differing in 39 hunks).  16 pins in
 * 9 blocks, so a landing needs a fakematch.txt row.  The split also serves the
 * already-parked file-mate, so do it once for both.
 * BLOCKER: loop.c's check_dbra_loop reverses the drift loop.  THE TELL IS IN THE
 * POOL, not the code -- ours carries 0x0059ffa6 / 0xffffcccd where the ROM carries
 * 0x0059ffff / 0x00003333 -- and that one reversal takes the step's callee-saved
 * register, kills the spill, and shrinks the frame from 8 to 4.  Six spellings flat.
 */
/* OvlFunc_970_2008b34 -- asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_a_c.s,
 * 0x02008b34, 231 ROM instructions.  The map's big transition cutscene: fade a
 * palette entry up, flash REG_BLDALPHA three times, run two background tasks,
 * drift the two scroll planes for ~0x1c02 frames, re-key the three BG control
 * words, then wipe out with REG_BLDY.
 *
 * NON-MATCHING: 218 of 255 aligned-equal (85.5%), 50 differing in 39 hunks.
 *
 * WHICH FIGURE IS WHICH.  SIZE IS EXACT -- 624 bytes, objcmp prints no SIZE
 * line.  Instruction count is ref 255 against ours 254, ONE SHORT, so objcmp's
 * own count (155) is NOT a distance and the aligned figure above is the measure.
 * Relocations are the same symbols in the same order; their offsets run 4 bytes
 * early from the first shortfall.
 *
 * SHIMS: 16 register pins in 9 blocks (6x PIN3, 3x PIN2) -- tools/shimcount.py.
 * They are the overlay family's own idiom, already landed in
 * src/overlays/rom_7fa4ec/ovl_30_c_c_c_a_a_c.c and
 * src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_c_c.c, and A LANDING
 * WOULD NEED A fakematch.txt ROW.  No .equ shims, no "+r" barriers.
 * SPLIT: text-only; tools/datacheck.py prints nothing.  The .s holds TWO
 * functions (this one and OvlFunc_970_2008da4), so landing needs a split -- and
 * note that OvlFunc_970_2008da4 is ALREADY PARKED at 7 differing in
 * src/non_matching/ovl_7fa4ec/2008da4.c, so the split serves both.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7fa4ec/2008b34.c \
 *     asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_a_c.s --func OvlFunc_970_2008b34
 *
 * ============================================================
 * THE BLOCKER: gcc REVERSES THE DRIFT LOOP AND THE ROM'S COMPILER DID NOT.
 * PASS: loop.c, check_dbra_loop.
 *
 * The ROM counts the accumulator UP and compares it against a pooled bound
 * reloaded every iteration, with the accumulator itself SPILLED around the call
 * because no callee-saved register is left:
 *
 *     add r2, r7            ; acc += 0x3333, r7 holds the pooled step
 *     str r2, [sp]
 *     bl  __WaitFrames
 *     ldr r3, =0x59ffff
 *     ldr r2, [sp]
 *     cmp r2, r3
 *     ble .Lc44
 *
 * We get a COUNTDOWN.  Our literal pool is the tell -- it carries
 * `.word 0x0059ffa6` and `.word 0xffffcccd` (= -0x3333) where the ROM carries
 * `.word 0x0059ffff` and `.word 0x00003333`: gcc turned the biv round, compares
 * against 0 with `bge`, and the whole spill disappears because acc then wins the
 * callee-saved register r7 that the ROM gives to the STEP.  That one exchange is
 * the missing instruction, the `sub sp, #8` against our `sub sp, #4`, and about
 * fifteen of the fifty differing encodings.
 *
 * SIX SPELLINGS AGAINST IT, ALL PLATEAU AT 218 OR WORSE:
 *   un-rotated `goto` loop                       214, and 252 instructions
 *   `while (1) { ... if (acc > K) break; }`       218 (inert)
 *   `while (!(acc > K))` / `acc = acc + k`        218 (inert)
 *   `volatile int acc`                           218 but 259 instructions --
 *       it stops the reversal AND restores the 8-byte frame, but a volatile
 *       read-modify-write adds an `ldr` per iteration that the ROM does not
 *       have.  Declaring it before or after the volatile halfword is inert.
 * The step 0x3333 does not fit a Thumb `add rd,#imm`, so the increment is a
 * register add in BOTH directions -- that is not what decides it.
 *
 * ============================================================
 * WHAT CLOSED 88 ENCODINGS, IN ORDER OF VALUE.
 *
 * 1. THE FAMILY'S PINNED ARGUMENT FILLS, worth 167 -> 207 aligned on their own.
 *    Nine call sites fill their arguments in an order gcc will not reproduce
 *    unpinned, and three of them build the SAME constant that gcc then CSEs
 *    into a callee-saved register:
 *      __Func_8012330 is called four times; three of its fills are
 *      `mov r0,#K / mov r1,#K / mov r2,#K / lsl r1,#n / lsl r2,#n / lsl r0,#n`
 *      -- THREE INDEPENDENT MATERIALISATIONS OF ONE VALUE.  Unpinned gcc emits
 *      `movs r5,#0x80 / lsls r5,#9 / adds r1,r5,#0 / adds r2,r5,#0` and burns a
 *      callee-saved register on it.
 *      __StartTask is called three times, each `mov r1,#0xc8 ... lsl r1,#4`
 *      rebuilt on the spot; unpinned gcc hoists 0xc80 into r9 and the prologue
 *      grows a fourth high register.
 *    A bare `__asm__ volatile ("");` between the pinned assignments is INERT
 *    here (the two remaining `movs r0,#0x80` transpositions are not sched2).
 *
 * 2. ONE LOCAL PER INDEPENDENT STORED VALUE, worth 207 -> 218.  Four separate
 *    "named stored value" sites share nothing in the ROM: `0xfc << 7` to palette
 *    RAM, `0x81 << 4` and the pooled 0x1010 to REG_BLDALPHA, and 0xbf to
 *    REG_BLDCNT.  Reusing one `int n` for them makes it a function-long pseudo
 *    that lands in r7 where the ROM uses the caller-saved r2; four distinct
 *    locals put each back.  Same for the two zeros -- the ROM has one in r8 for
 *    .L17ec/.L17f4 and a DIFFERENT one in r6 for .L17f8/gOvl_020097e8 at the end.
 *
 * ============================================================
 * WHAT WAS READ OUT OF THE ROM AND IS LOAD-BEARING.
 *
 *  - IT RETURNS void.  `pop {r0} / bx r0` -- the return address goes to r0, so
 *    r0 carries nothing out.  (Contrast the file-mate, which pops into r1.)
 *  - THE SIX .L DATA SYMBOLS ARE REACHED BY NAME, the idiom already landed in
 *    this overlay: `extern int L17ec __asm__(".L17ec");` and so on.  All six are
 *    already `.global` in asm/overlays/rom_7fa4ec/ovl_30_c_c_c_c.s, so no
 *    further export is needed.
 *  - THE THREE BG CONTROL WORDS ROUND-TRIP THROUGH A `volatile unsigned short`
 *    at sp+6, exactly as the file-mate park records: `t = (REG_BG3CNT & 0xfffc)
 *    | 3; REG_BG3CNT = t;`.  BG3 and BG2 take `| 3`, BG1 takes `| 2`, and gcc
 *    pools both small constants by itself (`.word 3`, `.word 2`) inside a
 *    mid-function pool the ROM jumps over -- the branch-over-pool shape, which
 *    is reproducible and not a blocker.
 *  - `iwram_3001e70[0]` IS THE SCROLL-PLANE BASE, held in r10 for the whole
 *    function; the two planes are at +0x140 and +0x170 (`0xa0 << 1`,
 *    `0xb8 << 1`), and the SAME two offsets are re-read at the end into
 *    .L1804/.L1808.
 *  - THE PALETTE ADDRESS IS BUILT TWICE.  `0xa0 << 19` = 0x5000000 is held in
 *    r6 across the fade loop and REBUILT into r3 for the single store after it,
 *    so the post-loop store must be a fresh expression, not the loop's pointer.
 *  - THE FADE LOOP ORs INTO THE LEFT OPERAND: `(i << 11) | (i << 5)` gives
 *    `lsl r3,r5,#11 / lsl r2,r5,#5 / orr r3,r2`, destination r3 = the 11-shift.
 *  - BOTH DOWN-COUNTING LOOPS TEST AFTER THE DECREMENT (`sub r5,#1 / cmp r5,#0 /
 *    bge`), so they are do-whiles running one extra iteration at 0.
 *
 * NEXT: the loop reversal, and it wants loop.c's check_dbra_loop read against
 * the .08.loop dump rather than more spellings -- six are on file and the
 * plateau is flat.  If it opens, the two `movs r0,#0x80` transpositions and the
 * three remaining pool-offset shifts are all that is left.
 */
#include "gba/types.h"
#include "gba/io.h"

extern unsigned char *iwram_3001e70[];
extern int gOvl_020097e8;
extern int L17ec __asm__(".L17ec");
extern int L17f4 __asm__(".L17f4");
extern int L17f8 __asm__(".L17f8");
extern int L17fc __asm__(".L17fc");
extern int L1804 __asm__(".L1804");
extern int L1808 __asm__(".L1808");

extern void OvlFunc_970_2008194(void);
extern void OvlFunc_970_2008168(void);
extern void OvlFunc_970_20080b0(void);
extern void OvlFunc_970_2008430(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __StartTask(int fn, int arg);
extern void __StopTask(int fn);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
extern void __Func_808e118(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_80b04c4(void);

void OvlFunc_970_2008b34(void)
{
    unsigned char *b;
    volatile unsigned short t;
    unsigned short *pal;
    int i;
    int j;
    int m;
    int n0;
    int n1;
    int n2;
    int one;
    int zero;
    int zero2;
    int k;
    int acc;
    int *p;
    int *q;

    b = iwram_3001e70[0];
    __CutsceneStart();
    __Func_808e118();
    { PIN3; q1 = 0x9c; q0 = 0; q1 <<= 1; q2 = 0xe8; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q1 <<= 8; q2 = 0; q0 = 0; __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __PlaySound(0x8c);
    pal = (unsigned short *)(0xa0 << 19);
    i = 0;
    do {
        *pal = (i << 11) | (i << 5);
        __CutsceneWait(0xa);
        i++;
    } while (i <= 0xf);
    n0 = 0xfc << 7;
    *(unsigned short *)(0xa0 << 19) = n0;
    m = 0x1010;
    n1 = 0x81 << 4;
    j = 2;
    do {
        __PlaySound(0xd4);
        REG_BLDALPHA = m;
        __CutsceneWait(3);
        REG_BLDALPHA = n1;
        __CutsceneWait(0x41);
        j--;
    } while (j >= 0);
    one = 1;
    gOvl_020097e8 = one;
    zero = 0;
    L17ec = zero;
    { PIN2; q1 = 0xc8; q1 <<= 4; q0 = (int)OvlFunc_970_2008194; __StartTask(q0, q1); }
    L17f8 = one;
    __CutsceneWait(0x14);
    __PlaySound(0xa3);
    { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q1 <<= 9; q2 <<= 9; q0 <<= 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; L17f8 = one; q1 <<= 10; q2 <<= 9; q0 <<= 10;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0xc0; q1 = 0xc0; q2 = 0x80; q0 <<= 10; q1 <<= 10; q2 <<= 9;
      __Func_8012330(q0, q1, q2); }
    L17f4 = zero;
    p = (int *)(b + (0xa0 << 1));
    q = (int *)(b + (0xb8 << 1));
    k = 0x3333;
    { PIN2; q1 = 0xc8; q1 <<= 4; q0 = (int)OvlFunc_970_2008168; __StartTask(q0, q1); }
    acc = 0;
    do {
        *p += k;
        *q += k;
        acc += k;
        __WaitFrames(1);
    } while (acc <= 0x59ffff);
    __StopTask((int)OvlFunc_970_2008168);
    zero2 = 0;
    L17f8 = zero2;
    t = (REG_BG3CNT & 0xfffc) | 3;
    REG_BG3CNT = t;
    t = (REG_BG2CNT & 0xfffc) | 3;
    REG_BG2CNT = t;
    t = (REG_BG1CNT & 0xfffc) | 2;
    REG_BG1CNT = t;
    gOvl_020097e8 = zero2;
    __PlaySound(0x90 << 1);
    __WaitFrames(1);
    __PlaySound(0x91);
    n2 = 0xbf;
    REG_BLDCNT = n2;
    i = 0;
    do {
        REG_BLDY = i;
        __CutsceneWait(1);
        i++;
    } while (i <= 0x10);
    __CutsceneWait(0x28);
    { PIN3; q0 = 1; q1 = 1; q2 = 0xe666; q0 = -q0; q1 = -q1; __Func_8012330(q0, q1, q2); }
    L1804 = *(int *)(b + (0xa0 << 1));
    L1808 = *(int *)(b + (0xb8 << 1));
    L17fc = 1;
    i = 0x10;
    do {
        REG_BLDY = i;
        __CutsceneWait(8);
        i--;
    } while (i >= 0);
    { PIN2; q1 = 0xc8; q1 <<= 4; q0 = (int)OvlFunc_970_20080b0; __StartTask(q0, q1); }
    __PlaySound(0x50);
    __Func_80b04c4();
    __CutsceneWait(0x14);
    __CutsceneEnd();
    OvlFunc_970_2008430();
}
