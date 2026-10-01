/* LuckyDiceMain -- NON-MATCHING, NO CANDIDATE, NO FIGURE.  Honest triage park.
 * Unattempted before batch 313.  Reference asm/rom_f4000/rom_f4008_c_c_c.s.
 *
 * NO C WAS WRITTEN, SO THERE IS NO N-of-M AND I AM NOT QUOTING ONE.  This park
 * exists to correct the frame census that made this function look like the
 * hardest in the tree, and to record the install shape, which is the real cost.
 *
 * Verify with (once a candidate exists):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f4000/LuckyDiceMain.c \
 *     asm/rom_f4000/rom_f4008_c_c_c.s --func LuckyDiceMain
 *
 * THE 768-BYTE FRAME IS REAL.  Built `ldr r5,=0xfffffd00 / add sp, r5` (-0x300)
 * and released `mov r3,#0xc0 / lsl r3,#2 / add sp, r3` (+0x300).  Thumb-1
 * `sub sp,#imm` caps at 508 bytes so this function cannot use that form at all,
 * and with only the `sub sp,#imm` grep it reads as FRAMELESS.  Confirmed the
 * largest frame of the four.  `sub sp,#imm` count: ZERO.
 *
 * "27 AGGREGATES" IS AN OVER-COUNT.  THE RESOLVED NUMBER IS ELEVEN.
 * Thumb-1 has no sp-relative `strh`/`ldrh`/`strb`/`ldrb`, so every SUB-WORD
 * stack scalar must materialise a base with `add rX, sp, #K` first, and at the
 * grep that is indistinguishable from an aggregate.  Resolving all 29
 * materialisation sites by READING THE REGION -- first use of the materialised
 * register -- gives:
 *
 *   ELEVEN GENUINE AGGREGATES (distinct objects, by sp offset):
 *     0x78   walked output array; reached as (sp+0x300) + (-0x288) and written
 *            with `stmia r0!, {r1}`
 *     0x80   address STORED to sp+0x2c (`mov r0,sp / add r0,#0x80 /
 *            str r0,[sp,#0x2c]`); and sp+0x84 is the same array walked DOWNWARD
 *            (`add r6,sp,#0x84` ... `str r1,[r6] / sub r6,#4`)
 *     0x88   address STORED to sp+0x28
 *     0x90   address STORED to sp+0x34, and written at offset 4
 *     0x98   address STORED to sp+0x24 AND sp+0x14
 *     0xc8   vec3 PASSED AS AN ARGUMENT to PhysMove; `ldr/str [r5,#8]`
 *     0xd4   vec3 PASSED AS AN ARGUMENT to PhysMove; written at 0, 4 and 8
 *            (sp+0xdc is inside this object, not a twelfth)
 *     0xe0   ARRAY -- register-indexed (`ldr r3,[r3,r5]`, `str r5,[r2,r3]`) and
 *            read at two offsets (`ldr r2,[r1] / ldr r3,[r1,#4]`)
 *     0xf0   address STORED to sp+0x3c, then walked with `ldmia r1!, {r3}`
 *     0xf8   written at offset 0 twice and walked with `add r7,#4`
 *     0x100  address stored into a DMA descriptor by `stmia r3!, {r0,r1,r2}`
 *            with `ldr r2,=0x84000080`, and passed to Func_80f4100 and two more
 *            calls
 *
 *   FOUR SUB-WORD SCALARS MISCOUNTED AS AGGREGATES -- the trap, confirmed:
 *     0xa0, 0xa2, 0xa4, 0xa6.  TWO BYTES APART, each base materialised
 *     separately (`add r0,sp,#0xa0` / `add r4,#0xa2` / `add r5,sp,#0xa4` /
 *     `add r7,#0xa6`) and each used with EXACTLY ONE `strh`/`ldrh` AT OFFSET 0:
 *         strh r3,[r7] / strh r2,[r5] / strh r1,[r4] / strh r3,[r0]
 *         ldrh r2,[r7]
 *     Four `short` locals, not four aggregates.
 *
 *   THREE SITES THAT ARE NOT AN OBJECT AT ALL -- A CLASS NOTHING NAMES YET:
 *     `add r7, sp, #0x300` (three sites, lines 1921/1932/1937) materialises the
 *     FRAME TOP as a base, and a POOLED NEGATIVE displacement is then added to
 *     reach a real object: `ldr r5,=0xfffffde0 / mov r12,r5 / add r7,r12` is
 *     sp+0x300-0x220 = sp+0xe0, and `ldr r1,=0xfffffd78 / add r0,r6,r1` is
 *     sp+0x78.  gcc does this when loop strength reduction expresses an
 *     induction base that way.  A frame-top materialisation is NOT a stack
 *     aggregate and must be excluded, or a 768-byte frame will always look like
 *     it has an object at its very end.
 *
 *   The remaining sites are REPEAT materialisations of objects already counted
 *   (sp+0xe0 alone accounts for five, sp+0x100 for four, sp+0xc8 for three).
 *
 * THE FRAME RECIPE NEEDS A FIFTH GREP, AND THIS FUNCTION IS WHERE IT SHOWS.
 * `add rX, sp` -- the TWO-OPERAND form, rX += sp -- occurs twice (lines 1110
 * and 1238, both reaching sp+0xc8 via `mov r5,#0xc8 / add r5, sp`).  Neither
 * `mov rX, sp` nor `add rX, sp, #K` matches it.  Counts for this function:
 *   sub sp,#imm 0 | (add|sub) sp,rN 2 | mov rX,sp 8 | add rX,sp,#K 19 |
 *   add rX,sp (2-op) 2 | str rX,[sp] 14
 *
 * AND THE `sp0` COLUMN IS RIGHT HERE BUT FOR THE RIGHT REASON ONLY AFTER
 * PAIRING.  sp+0 is 14 stores / 0 loads -> genuine outgoing argument space for
 * five-or-more-argument calls.  But the `[sp,#K]` census also reports 0x84 and
 * 0x90 as store-only, which looks like two dead slots or two phantom holes.
 * THEY ARE NEITHER: both are aggregate bases from the list above, written
 * through the materialised register where an `[sp,#K]` grep cannot see it.
 * Re-check every store-only offset against the `add rX,sp` sites before calling
 * it a hole.
 *
 * SPILL-SLOT ACCESS-COUNT TABLE (store/load), the ranking instrument:
 *   0x6c: 1/27   0x70: 1/16   0x44: 7/11   0x50: 7/7   0x60: 6/6
 *   0x5c: 2/8    0x48: 5/4    0x4c: 5/4    0x58: 3/6   0x40: 1/7
 *   0x08: 4/4    0x54: 6/1    0x1c: 2/4    0x20: 2/4   0x28: 1/4
 *   0x00: 14/0 (outgoing args)
 * THIRTY-THREE distinct [sp,#K] offsets.  0x6c and 0x70 are 1-store/many-load
 * -- the two `galloc_iwram` returns stored in the prologue
 * (`str r0,[sp,#0x74]` is a third) and read all through the function.  Those
 * are the first declarations.
 *
 * INSTALL SHAPE, AND IT IS THE MOST EXPENSIVE OF THE FOUR.  datacheck.py exits
 * 1: the .s carries a `.section .rodata` with five `.incrom` blocks, and
 * LuckyDiceMain reads FOUR data labels.  A conversion needs a TEXT/DATA SPLIT
 * and must first export
 *     .global .Lf53fc  .global .Lf5400  .global .Lf5408  .global .Lf541a
 * `A .global emits no bytes`, so verify `make compare` after the export and
 * BEFORE the split so the two changes stay separable.
 *
 * NO DISPATCH AT ALL.  `grep -cE '^\t(mov|ldr|add)\tpc'` = 0, and the jump-table
 * entry count is 0.  Every switch lever in the brief is INAPPLICABLE here; the
 * brief already said so and the measurement confirms it.  This function's cost
 * is the frame, the data split and the spill map, nothing else.
 *
 * LOOP CENSUS, BY BACKWARD EDGE, AND IT IS THE CLEANEST SIGNAL IN THE BATCH.
 * 22 backward edges, one unconditional.  The conditional closures: bne 16,
 * beq 2, bgt 1, ble 1.  EIGHTEEN OF TWENTY CLOSE ON EQUALITY.  So `!=` / `==`
 * loop conditions are right nearly everywhere here, and the two signed
 * closures are the exceptions to find rather than the rule.  The raw mnemonic
 * census over the whole function would have said something else entirely --
 * this file has 442 decimal-immediate and 247 hex-immediate operands and a
 * great many signed compares that are clamps (0x270F = 9999 and 0x3E7 = 999
 * bound the stake and the display, per the .s annotation), signed-division
 * corrections (`lsr r2,r3,#31 / add r3,r2 / asr r3,#1`) and phase tests.
 *
 * TWO `.call_via` SITES.  A RUNG-8 per-opcode histogram over the RAW reference
 * under-counts `mov` and `bx` by TWO each.  Expand before trusting them.
 *
 * ONE TYPE SCREEN WORTH RUNNING FIRST.  `grep -c ldrsh` on the reference, then
 * on any candidate: `unsigned short v = *src++;` emits `ldrsh` + `lsl #16` +
 * `lsr #16` where `int v = *src++;` emits none.  This function already contains
 * the `lsl #16 / asr #16` pair at the halfword-store loop above, so the
 * distinction is live here and a wrong local type will show up as a block of
 * spurious `ldrsh`.
 */
