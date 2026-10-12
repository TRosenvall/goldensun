/* NintendoLogo -- NOT MATCHING, 22 differing encodings of 202.
 *
 * CLAIM LINE CORRECTED (batch 343).  Brief E took this park 24 -> 22 on a
 * device-free anchor closing the DecompressLZ schedule swap, and the top line
 * lagged the body.  My first correction then mangled the ladder line into
 * "22 -> 22" by replacing the wrong digit; restored.
 *
 * THE TOP LINE IS WHAT parkcheck AND piece.py PARSE, so a stale one hides a
 * close park from target selection -- overlays/200a718.c ranked as 55 for seven
 * batches while measuring 17 for exactly this reason.
 */
/* ===================== BATCH 342 (brief E, the DEPIN brief) ADDENDUM -- READ FIRST
 *
 * NintendoLogo -- STILL NON-MATCHING, IMPROVED 24 -> 22 BY A DEVICE-FREE ANCHOR.
 *
 * THE FIGURE, IN STREAM TERMS, FOR THE BODY IN THIS FILE:
 *   22 of 202 stream encodings; 161 / 161 16-bit instructions (IN PHASE, so a
 *   TRUE DISTANCE), 40/40 32-bit entries, 484 bytes against 484 (dsize 0),
 *   RELOCATIONS DIFFER -- see the device paragraph, which is the whole story.
 *   The previous body reads 24 of 202 in exactly the same columns.
 *
 * Verify with (NOTE: the recipes below both name files that do not exist --
 * `src/non_matching//NintendoLogo.c` with a doubled slash, and
 * `src/non_matching/rom_f2000/f2b70.c`.  THIS one is the installed path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f2000/NintendoLogo.c \
 *     asm/rom_f2000/rom_f2028_c_a.s --func NintendoLogo
 *
 * REFERENCE, DECOMPOSED.  The header below says "202 instructions against 202".
 * 202 IS THE STREAM LENGTH: it is 161 16-bit instructions + 40 32-bit entries
 * (pool words AND `bl`s) + pads.  Both sides are 161/40, which is the real
 * statement and is stronger.  CORRECTED.
 *
 * *** WHAT CHANGED: `do { } while (0)` AFTER THE FIRST DMA3_SET.  24 -> 22. ***
 * Found by tools/autosweep.py's anchor axis (boundary 14) and confirmed with
 * objcmp.  It closes residue 2 of the header's three -- the adjacent schedule
 * swap in DecompressLZ's argument block, where the ROM emits `lsl r3, #1` before
 * `ldr r5, =gBuffer` and gcc emitted them the other way round.  The first
 * differing index moves 36 -> 45.  An anchor emits nothing, is not counted by
 * shimcount and is the preferred form over a barrier (docs/elevation.md), so
 * this is a device-free 2-encoding improvement.  **THE ANCHOR AXIS WAS CAPPED AT
 * 40 OF 79 BOUNDARIES, so 40..78 are UNTESTED, not inert.**
 *
 * *** THIS FILE IS NOT SHIM-FREE, AND shimcount.py CANNOT SEE WHY. ***
 * The header says "Shim-free" and "Shim count: 0 (tools/shimcount.py) --
 * PIN-FREE, no fakematch.txt row".  The register-pin count IS 0 and the three
 * DMA3_SET calls genuinely inherit include/dma.h's declarations (this file does
 * `#include "dma.h"`, it does NOT carry a local copy -- so the batch-337
 * copied-helper shape does not apply here, and dma.h's own comment records that
 * none of its 82 landed users carries a fakematch row).  BUT THE BODY ALSO
 * CARRIES `extern int _FILE_18;` AND READS `id = (int)&_FILE_18;`, AND
 * `_FILE_18` IS DEFINED NOWHERE IN THE TREE -- not in file_table.sym, not in any
 * .sym, not in any linker script.  An UNDEFINED EXTERN USED FOR ITS ADDRESS is a
 * fictitious symbol, i.e. a DEVICE, and shimcount counts only
 * `register ... __asm__` declarations, `__asm__(".equ ...")` and empty
 * `__asm__ volatile ("")` -- THIS THIRD FORM IS A BLIND SPOT IN THE TOOL.
 *
 * AND THE DEVICE IS WORTH 160 ENCODINGS.  Measured: with `id = 0x18;` written as
 * a plain literal the figure is **184 of 202, 161/160 instructions (PHASE),
 * 40/39 32-bit entries, dsize -4** -- the pool word disappears and the whole
 * pool shifts.  (`v4/b_devicefree.c`; with the anchor as well it is 185.)
 * SO: THE DEVICE-FREE FIGURE FOR THIS FUNCTION IS 184 OF 202, AND THE 22 IS A
 * FIGURE ABOUT A FUNCTION PLUS A PENDING SYMBOL-TABLE ENTRY.
 *
 * That measurement CONFIRMS the header's own structural argument rather than
 * undermining it: `*thumb_movsi_insn` (arm.md:3838) lists the 8-bit `I`
 * constraint as ALTERNATIVE 1 and the memory-or-immediate pool path as
 * ALTERNATIVE 6, and `constrain_operands` takes the first alternative needing no
 * reload -- so a small SImode constant can never be pooled, and the ROM's pooled
 * 0x18 proves the source named something.  Writing it as a literal costs a pool
 * word and 160 encodings, which is exactly what that predicts.  `_FILE_18 = 0x18`
 * in file_table.sym is therefore the RIGHT spelling and remains CORRECTLY
 * WITHHELD under the owner's ruling, because it fails the ruling's COMPLETION
 * test: it does not take this function to 0.  Nothing in this batch changes that.
 *
 * WHAT IS LEFT, AFTER THE ANCHOR: 22 = 1 relocation form (the `_FILE_18` word)
 * + 21 for THE r2/r3 ROTATION at the five `gKeyPress & 9` sites.  The ROM reuses
 * the address register for the loaded value and puts the mask in r2
 * (`ldr r3,=gKeyPress / ldr r3,[r3] / mov r2,#9 / and r3,r2`); gcc keeps the
 * address in r3, loads into r2 and puts the mask in r3.  Same instructions, same
 * order, same count, two hard registers transposed.  Blocker, local-alloc: both
 * quantities record a conflict with hard register 3 in `.17.lreg`, so `k` cannot
 * take r3.
 *
 * FOUR MORE LEVERS MEASURED AGAINST THE ROTATION THIS BATCH, none of them on the
 * header's eleven-inert list, ALL INERT OR WORSE (base 22, all at 161/161 unless
 * noted):
 *   - `k` dropped ENTIRELY, each of the five reads its own quantity in the test
 *     block: **106**, and 149/161 -- 12 instructions short, so this is not the
 *     ROM's shape at all.  (The header's mechanism 1 is right: the ROM's shared
 *     test block with the read spelled twice IS the `goto` form with explicit `k`.)
 *   - the test as `if (!k)` instead of `if (k == 0)`: 22, INERT.
 *   - `k` declared `short` instead of `int`: 22, INERT.
 *   - the mask 9 held in a live `int` variable: 187, and 158/161.
 *   - the mask in its own named local, read through a named pointer (on the
 *     device-free body): 194 and 190.
 * Flag axis on the 22 body: `-fno-gcse`, `-fno-strict-aliasing`,
 * `-fno-rerun-cse-after-loop`, `-fno-rerun-loop-opt` all 22 (INERT);
 * `-fno-schedule-insns2` 74; `-fno-strength-reduce` 124; `-O1` 162;
 * `-ffixed-r7` 200 -- which independently re-confirms the header's recorded NO
 * on the -ffixed-r7 row that CamelotLogo's park asks for.  DO NOT ADD IT.
 *
 * SPLIT SHAPE, unchanged: TWO functions in asm/rom_f2000/rom_f2028_c_a.s
 * (NintendoLogo, CamelotLogo), so a text split is required when it lands;
 * datacheck.py reports no required data exports.
 * Harness: scratch_elev/b342/E (e342_sweep.py, e342_flagcross.py; v4/ v4b/).
 * =====================================================================================
 */
/* NintendoLogo -- NON-MATCHING, 24 of 202 encodings differ.
 * Reference asm/rom_f2000/rom_f2028_c_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching//NintendoLogo.c \
 *       asm/rom_f2000/rom_f2028_c_a.s --func NintendoLogo
 *
 * A TRUE DISTANCE: 484 == 484 bytes and 202 == 202 instructions.  Shim-free.
 * BLOCKER, local-alloc: both key-read quantities conflict with hard reg 3, so `k`
 * cannot take r3.  Eleven levers inert.
 *
 * A DECISION, RECORDED AS A NEGATIVE: src/non_matching/rom_f2000/80f2d54.c asks for
 * -ffixed-r7 to be tested on this object.  THE ANSWER IS NO -- do not add the row.
 * This ROM's prologue SAVES r7 and uses it to reach r8; under -ffixed-r7 this source
 * saves three callee-saved values where the ROM saves four and comes out 180
 * instructions against 202.  Both functions build from rom_f2028_c_a.o, so the row
 * would damage both.  CamelotLogo's park very likely has the same source-level
 * register-pressure bug this one had -- re-screen it with every loop written as a
 * goto loop BEFORE reaching for a flag.
 * file_table.sym would need _FILE_18 = 0x18 (pool-load signature; it fills the hole
 * beside CamelotLogo's existing _FILE_19).  Not added -- it does not complete this
 * function.
  *
 * *** BATCH-305 CORRECTION: THIS FILE ATTRIBUTES A RESIDUE TO sched1, AND sched1 DOES NOT
 * *** RUN IN THIS BUILD.  Verified with -da at production flags: the dump sequence is
 * *** 17.lreg 18.greg 19.flow2 20.ce2 23.sched2 25.jump2 26.mach -- there is NO sched1 dump,
 * *** because flag_schedule_insns is off at -O2 here, so only the post-reload scheduler runs.
 * *** Re-attribute to sched2 (rank_for_schedule), to combine, or to the ALLOCATION that fixed
 * *** the order.  Relatedly, any "-fno-schedule-insns is inert" note below rules nothing out:
 * *** that flag controls a pass that never runs.  The sched2 tie-break is priority ->
 * *** dependent count (more wins) -> INSN_LUID (lower wins), and LUID preserves EXPAND order.
 * *** See "sched1 DOES NOT RUN IN THIS BUILD" in docs/elevation.md.
*/
/* NintendoLogo (0x080f2b70) -- NON-MATCHING.
 * NON-MATCHING: 24 encodings of 202 differ (objcmp).
 * SIZE AND INSTRUCTION COUNT BOTH MATCH -- 484 bytes against 484, 202
 * instructions against 202 -- so 24 IS a distance, and the whole instruction
 * stream is reproduced.  The 24 are THREE things and nothing else:
 *
 *   1. ONE RELOCATION FORM, not a residue.  `ldr r5, =_FILE_18` against the
 *      reference's plain pooled literal 0x18.  A zero-addend R_ARM_ABS32 fills
 *      that word with 0x18 once file_table.sym carries _FILE_18, so the LINKED
 *      bytes are already exact; take it to `make compare`.
 *   2. ONE ADJACENT-INSN SCHEDULE SWAP (2 encodings), in DecompressLZ's
 *      argument block: the ROM emits `lsl r3, #1` then `ldr r5, =gBuffer`,
 *      gcc emits them the other way round.
 *   3. ONE REGISTER ROTATION (21 encodings), r2 <-> r3, at the FIVE
 *      `gKeyPress & 9` sites.  The ROM reuses the address register for the
 *      loaded value and puts the mask in r2:
 *          ldr r3, =gKeyPress / ldr r3, [r3] / mov r2, #9 / and r3, r2
 *      gcc keeps the address in r3, loads into r2 and puts the mask in r3:
 *          ldr r3, =gKeyPress / ldr r2, [r3] / mov r3, #9 / and r2, r3
 *      Same instructions, same order, same count -- only the two hard
 *      registers are transposed.  BLOCKER: LOCAL-ALLOC.  In `.17.lreg` both
 *      quantities are block-local (they never reach `.18.greg`'s "10 regs to
 *      allocate: 39 34 106 38 37 43 35 40 42 32"), and every one of them
 *      records a conflict with hard register 3 (`;; 86 conflicts: ... 3 13`),
 *      so `k` cannot be given r3 and takes r2 while the mask constant takes
 *      r3.  Same class as src/non_matching/rom_c0/800615c.c.
 *
 * ELEVEN LEVERS MEASURED AGAINST THE ROTATION, ALL INERT (all 24): declaration
 * order of every local; `k` as `unsigned int`; `k` split into k1/k2/k3, one per
 * wait loop (the documented one-variable-per-region lever); the two-step read
 * `k = gKeyPress; k &= 9`; mask operand order `9 & gKeyPress`; gKeyPress as
 * `volatile int`, as non-volatile, and as `volatile unsigned int[]` with `[0]`;
 * GetFile implicit; DecompressLZ argument naming; `f += 0x1c0` as its own
 * statement.
 *
 * ================================================================
 * WHAT CLOSED THE OTHER 178 -- four mechanisms, all of them reusable
 * ================================================================
 *
 * 1. EVERY LOOP IN THIS FUNCTION IS A `goto` LOOP.  The tell is the recorded
 *    one: a constant REBUILT INSIDE THE BODY.  The ramp nest rebuilds 0x10000
 *    (`mov r4, #0x80 / lsl r4, #9`) and the three wait loops rebuild both
 *    `=gKeyPress` and `mov r2, #9`.  Structured spellings hoist all of those.
 *    Measured, full function: all-structured 222 instructions against the
 *    ROM's 202; wait loops as `goto` 212; ramp nest as `goto` too 196.
 *    A plain `while ((gKeyPress & 9) == 0)` is NOT the ROM's shape at all --
 *    gcc-2.96's `duplicate_loop_exit_test` turns it into a do-while with a
 *    CONDITIONAL branch in the preheader and hoists the address and the mask
 *    into callee-saved registers.  The ROM's `b` into a SHARED test block with
 *    the read spelled twice is the `goto` form with an explicit `k`.
 *
 * 2. THE RAMP COUNTER IS AN `int` WITH THE 16-BIT WRAP WRITTEN OUT, AND THE
 *    DISTRIBUTED SHIFT IS THE WHOLE POINT.  The ROM computes
 *        t = v;  v = ((t << 16) + 0x10000) >> 16;  *p++ = t;
 *    -- shift FIRST, then add 0x10000.  `short v; *p++ = v++;` is the obvious
 *    reading and gives `add r3, r2, #1 / lsl r3, #16 / asr r3, #16`: gcc never
 *    distributes the shift over the `+1` when combine sees the whole
 *    `(ashiftrt (ashift (plus v 1) 16) 16)` at once.  A 48-CELL SWEEP
 *    (outer/inner x for/do-while/while/goto x six increment spellings) never
 *    produced it; writing the shifted form in the SOURCE does, exactly, and it
 *    is what costs the extra register the ROM's prologue saves.
 *    THIS ONE RESIDUE WAS WORTH SIX INSTRUCTIONS, NOT TWO: the 0x10000 temp
 *    takes the eighth low register, which pushes `mode` into r8 and so pays
 *    `mov r7, r8 / push {r7}` at entry and the matching pop -- 2 in the body,
 *    2 in the prologue, 2 in the epilogue.  A prologue that is one
 *    callee-saved register short is a REGISTER-PRESSURE report about the loop
 *    bodies, not a flag problem.
 *
 * 3. TWO CONSTANTS IN THIS FUNCTION ARE `int`s, AND THE TELL IS `ldr` vs
 *    `ldrh`.  The 0x1ff row markers and the clear loop's zero are both stored
 *    through a `short *`, so spelled as literals gcc materialises them in
 *    HImode (`ldrh r6, .L54`, and a `.word 0` pool entry the ROM does not
 *    have).  The ROM has `ldr r7, =0x1ff` and `mov r2, #0` -- SImode.  Naming
 *    them as `int m` and `int z` removes the spurious pool word, and removing
 *    it is what let the second literal pool fall where the ROM's does and
 *    dropped the extra pool-skip `b`.
 *
 * 4. THREE PLACEMENT LEVERS, each worth real encodings:
 *      - `m = 0x1ff` assigned EARLY (just after `id`, not next to the loop):
 *        45 -> 30.  The ROM schedules `ldr r7, =0x1ff` in among GetFile's DMA
 *        block, ~25 instructions before its only use; that is sched1 inside one
 *        basic block (the pool-skip `b` is emitted at final and is not a CFG
 *        edge), and it only reaches there if the RTL assignment is that early.
 *      - the clear loop REUSING the ramp nest's outer counter: 30 -> 28.
 *      - `z = 0` written INSIDE the clear loop body so LICM hoists it into the
 *        preheader AFTER the counter init: 26 -> 24.  Assigned before the loop
 *        it lands before the counter init, which is the wrong order.
 *      - `i`/`j` swapped in the ramp nest (inner counter is the LATER-declared
 *        one): 51 -> 45.
 *    And the argument-fill lever from the tree: making DecompressLZ IMPLICIT
 *    (lever 3 of the four, "make the mismatching call implicit") fixed the
 *    `mov r1, r5 / mov r0, r4` order, 28 -> 26.  Prototyping it, and making
 *    GetFile implicit instead, are both worse.
 *
 * ================================================================
 * -ffixed-r7 IS WRONG FOR THIS TRANSLATION UNIT -- src/non_matching/rom_f2000/80f2d54.c
 * ================================================================
 *
 * That park (CamelotLogo, the file-mate at 0x080f2d54) says its TU "looks like
 * a -ffixed-r7 file" and asks for the test to be run on 0x080f2b70 -- this
 * function -- before either is landed.  IT IS RUN, AND THE ANSWER IS NO.
 *
 * NintendoLogo's ROM prologue is `push {r5, r6, r7, lr} / mov r7, r8 /
 * push {r7}`: it SAVES r7 and then uses it as the scratch to reach r8.  r7 is
 * allocated, so the register cannot have been reserved.  Compiling this source
 * with -ffixed-r7 gives `push {r5, r6, lr} / mov r6, r8 / push {r6}` -- three
 * callee-saved values where the ROM has four -- and 180 instructions against
 * the ROM's 202.  Both functions are built from asm/rom_f2000/rom_f2028_c_a.o,
 * so a flag row on that object cannot be right for both, and it is this one it
 * would break.  DO NOT ADD THE ROW.
 *
 * WHAT THAT MEANS FOR CamelotLogo, which is worth more than the row: its park
 * reads "gcc reaches for the cheaper r7" as a compiler fact, and mechanism 2
 * above shows the same symptom on THIS function was a source-level
 * register-pressure bug in the candidate -- fixed by the loop shapes, not by a
 * flag.  CamelotLogo's park still spells its animation loop as a `while` with a
 * peeled tail and its iwram_3001ad0 clear loop as a `for`; both are the
 * structured shapes that cost this function two registers.  Re-screen it with
 * every loop as a `goto` loop and the 0x1ff/zero constants named as `int`s
 * BEFORE treating its prologue as a flag question.
 *
 * ================================================================
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f2000/f2b70.c asm/rom_f2000/rom_f2028_c_a.s \
 *     --func NintendoLogo
 * TWO functions in asm/rom_f2000/rom_f2028_c_a.s (NintendoLogo, CamelotLogo),
 * so a text split is required.  tools/datacheck.py reports NO required data
 * exports.  Shim count: 0 (tools/shimcount.py) -- PIN-FREE, no fakematch.txt row.
 * The three DMA3_SET calls pick up include/dma.h's own `register ... __asm__`
 * declarations, which are the shared header's and are not counted per file.
 * Needs _FILE_18 = 0x18 in file_table.sym; the run _FILE_13..17, _FILE_1a,
 * _FILE_1c already there leaves 0x18 and 0x19 as the hole, and CamelotLogo's
 * park wants _FILE_19 for the same reason.
 *
 * THE REFERENCE'S PROSE IS CORRECT about this function: r0 = 0 long / non-zero
 * short, returns -1 on skip and 0 on completion, the 0x77/0x3b/0xb3 frame
 * budgets, the (gKeyPress & 9) A-or-Start test and the 8-frame-vs-0x3c fade all
 * check out.  One detail it does not say: on the LONG path (r0 == 0) the
 * function RETURNS after its 0x77-frame wait and never reaches either fade;
 * only the short path fades.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern u8 iwram_3001d18;
extern short iwram_3001ad0[];
extern volatile unsigned int gKeyPress;
extern unsigned char gBuffer[];
extern int _FILE_18;
extern void _PlaySound(int id);
extern void ClearTasks(void);
extern void Func_8003b70(int a);
extern void Func_8003bb4(int a);
extern void ClearVRAM(void);
extern void WaitFrames(int n);
extern void *GetFile(int id);
extern void Func_800479c(void);
extern void Func_8003ce0(void);

int NintendoLogo(int mode)
{
    unsigned char *f;
    short *p;
    int v;
    int t;
    unsigned int i, j;
    int k;
    int ret;
    int id;
    int m;
    int z;

    _PlaySound(0x6e);
    iwram_3001d18 = 1;
    id = (int)&_FILE_18;
    m = 0x1ff;
    ClearTasks();
    Func_8003b70(1);
    ClearVRAM();
    WaitFrames(1);
    REG_BG2CNT = 0x681;
    REG_DISPCNT = 0x1440;
    ret = 0;
    iwram_3001ad0[5] = 0;
    f = GetFile(id);
    DMA3_SET(f, (void *)0x5000000, 0x84000070);
    do { } while (0);
    DecompressLZ(f + 0x1c0, gBuffer);
    DMA3_SET(gBuffer, (void *)0x6004000, 0x84002580);
    p = (short *)0x6003000;
    v = 0x100;
    i = 0;
rows:
    j = 0;
ramp:
    t = v;
    v = ((t << 16) + 0x10000) >> 16;
    *p++ = t;
    j++;
    if (j <= 29)
        goto ramp;
    *p++ = m;
    *p++ = m;
    i++;
    if (i <= 19)
        goto rows;
    for (i = 0; i < 4; i++) {
        z = 0;
        iwram_3001ad0[i * 2 + 1] = z;
        iwram_3001ad0[i * 2] = z;
    }
    DMA3_SET(iwram_3001ad0, (void *)REG_ADDR_BG0HOFS, 0x84000004);
    Func_800479c();
    ClearVRAM();
    REG_DISPCNT = 0x1540;
    if (mode == 0) {
        Func_8003bb4(1);
        Func_8003ce0();
        k = gKeyPress & 9;
        i = 0;
        goto t1;
    b1:
        WaitFrames(1);
        i++;
        if (i > 0x77)
            goto done;
        k = gKeyPress & 9;
    t1:
        if (k == 0)
            goto b1;
        ret = -1;
        goto done;
    }
    k = gKeyPress & 9;
    i = 0;
    goto t2;
b2:
    WaitFrames(1);
    i++;
    if (i > 0x3b)
        goto fade_in;
    k = gKeyPress & 9;
t2:
    if (k == 0)
        goto b2;
    ret = -1;
fade_in:
    if (ret != 0)
        Func_8003bb4(8);
    else
        Func_8003bb4(0x3c);
    Func_8003ce0();
    if (ret != 0)
        goto out8;
    k = gKeyPress & 9;
    i = 0;
    goto t3;
b3:
    WaitFrames(1);
    i++;
    if (i > 0xb3)
        goto fade_out;
    k = gKeyPress & 9;
t3:
    if (k == 0)
        goto b3;
    ret = -1;
fade_out:
    if (ret == 0)
        goto out3c;
out8:
    Func_8003b70(8);
    goto last;
out3c:
    Func_8003b70(0x3c);
last:
    Func_8003ce0();
done:
    return ret;
}
