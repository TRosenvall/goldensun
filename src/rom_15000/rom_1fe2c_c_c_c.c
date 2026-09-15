/* Func_8020150 (0x08020150) -- DrawCursorLabel.
 * Whole-file conversion of asm/rom_15000/rom_1fe2c_c_c_c.s.
 *
 * The .s holds ONE function plus a trailing `.section .rodata` that exports
 * `.L73854` (`.incrom 0x73854, 0x73864`, four words 0x5a..0x5d).  That symbol
 * is referenced from asm/rom_15000/rom_1fe2c_c_a_c.s (`ldr r3, =.L73854`), so
 * it cannot be dropped.  It is emitted from C here, which keeps the file a
 * WHOLE conversion and leaves both stage1.ld lines verbatim:
 *
 *     asm/rom_15000/rom_1fe2c_c_c_c.o(.text)      (stage1.ld:492)
 *     asm/rom_15000/rom_1fe2c_c_c_c.o(.rodata)    (stage1.ld:591)
 *
 * Flags: the generic `asm/%.o: src/%.c` rule, plain GCC296_CFLAGS at -O2.
 * tryc.makefile_flags returns the empty set and WILDCARD_HITS is empty, so no
 * pattern rule bites and no explicit Makefile rule is needed.
 *
 * Retires the park at src/non_matching/rom_15000/8020150.c (batch 177,
 * "allocation-order rotation"), which measured 32 of 35.  Two levers:
 *
 *  1. THE WALKING INDEX, not the walking pointer.  The ROM reads the signed
 *     byte as `ldrb / lsl #24 / asr #24`; a walking pointer (`p++; *p`) emits
 *     `mov r0,#0 / ldrsb r0,[r5,r0]` and a `signed char` local does not change
 *     it.  A walking INDEX (`k++; b[k]`) gives the ROM's three instructions --
 *     gcc still strength-reduces the address into the same `add r5,#1`
 *     induction variable, so only the load form changes.  This is the batch-234
 *     correction on the `ldrb + lsl #24` entry applied to a sign extension
 *     rather than a zero test.
 *
 *  2. THE FIFTH ARGUMENT MUST BE A LITERAL.  Naming 0x10 as `int m` gives it an
 *     allocno that competes for a callee-saved register; gcc then spends TWO
 *     high registers (r8 and r10) and keeps the index in a low one, 33 of 37.
 *     Written as the literal `0x10`, loop invariant motion hoists it on its own
 *     into r8, the callee-saved pool is exhausted at r5/r6/r7/r8, and `n` falls
 *     to CALLER-SAVE in r4 -- the ROM's `str r4,[sp,#4] / bl / ldr r4,[sp,#4]`
 *     around the call, and the `sub sp,#8` (4 bytes of outgoing argument, 4 of
 *     caller-save slot).  -fno-caller-saves confirms the mechanism: 37 differing
 *     of 39.
 *
 * MEASURED (all at the tree's flags, rom 37 lines):
 *   park as written, `(signed char)*p`, 6 args        37 lines, 32 differing
 *   + `signed char` local                             37, 32 (inert)
 *   5-arg prototype, walking pointer                  36, 35
 *   5-arg prototype, walking index, `int m`           37, 33
 *   + -ffixed-r10                                     37, 33 (r9 instead)
 *   + -ffixed-r9/-r10/-r11                            33, 33 (0x10 rebuilt
 *                                                     in the loop instead)
 *   walking index, literal 0x10, do/while             37, 2 -- `mov r8,r3` and
 *                                                     `mov r6,#3` transposed
 *   + six init/declaration orderings                  37, 2 or 7 (inert)
 *   + -fno-schedule-insns / -fno-gcse /
 *     -fno-rerun-cse-after-loop / -fno-strict-aliasing 37, 2 (inert)
 *   -fno-schedule-insns2                              37, 14
 *   ASCENDING `for (i = 0; i < 4; i++)`               EXACT
 *
 * The last step is the whole of the residue: the ROM's `mov r6,#3 / sub r6,#1 /
 * cmp r6,#0 / bge` is a REVERSED loop, not a source-level countdown.  Writing
 * the countdown by hand puts the counter's init at a different point in the
 * preheader and sched2 will not move it; letting loop.c reverse an ascending
 * `for` emits it after the invariant hoist, which is the ROM's order.
 */

extern void Func_801e9d4(int c, int b, void *a, int n, int m);

const int L73854[4] __asm__(".L73854") = { 0x5a, 0x5b, 0x5c, 0x5d };

void Func_8020150(void *a, unsigned char *b)
{
    int k;
    int n;
    int i;

    if (a == 0)
        return;
    k = 0x28;
    n = 0;
    for (i = 0; i < 4; i++) {
        Func_801e9d4((signed char)b[k], 2, a, n, 0x10);
        k++;
        n += 0x18;
    }
}
