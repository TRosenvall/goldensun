/* NintendoLogo -- NON-MATCHING, 24 of 202 encodings differ.
 * Reference asm//rom_f2000/rom_f2028_c_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching//NintendoLogo.c \
 *       asm//rom_f2000/rom_f2028_c_a.s --func NintendoLogo
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
