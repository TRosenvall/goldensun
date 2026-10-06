/* SoundMainBTM  [rom_f9000]
 *
 * NON-MATCHING, 11 of 12 encodings  (MEASURED batch 330, objcmp --func).
 *   THE LENGTH ARITHMETIC, DONE EXACTLY (batch 330).  objcmp's two counts are
 *   not the same number and the old header conflated them:
 *     INSTRUCTION COUNT  ref 11, ours 5     (16-bit encodings only)
 *     POOL WORD COUNT    ref 0,  ours 1
 *     plus one trailing alignment pad on each side
 *     SIZE               ref 24 bytes, ours 16
 *   So the twelve and the seven in the encodings line are 11+0+1 and 5+1+1.
 *   The reference is ELEVEN INSTRUCTIONS and this body is FIVE: the gap is a
 *   real instruction-count gap of six, NOT the padding trap.  aligncmp puts
 *   ONE encoding aligned-equal out of eleven.  The figure is therefore not a
 *   distance and cannot be made one by spelling.
 *
 *   The relocation difference is a CONSEQUENCE of the same thing, not a
 *   separate blocker: the reference carries no relocation at all and this body
 *   carries a THM_CALL to memset, because it calls memset and the ROM does not.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f9000/rom_f9a18.c \
 *     asm/rom_f9000/rom_f95e0.s --func SoundMainBTM
 *
 * Source asm: goldensun/asm/rom_f9000/rom_f95e0.s
 *
 * BLOCKER CLASS, SETTLED IN BATCH 330 BY READING THE COMPILER: gcc-2.96 CANNOT
 * EMIT THIS FUNCTION.  Not "has not been persuaded to" -- cannot.  The ROM is
 *
 *     mov ip, r4 / movs r1..r4, #0 / stmia r0!, {r1,r2,r3,r4} x4 /
 *     mov r4, ip / bx lr
 *
 * a sixty-four-byte inline zero fill.  Four independent reasons it is out of
 * reach, each checkable in one command:
 *
 *   1. THERE IS NO `clrstr` PATTERN ANYWHERE IN THE ARM PORT.  `grep -rn clrstr
 *      config/arm` returns nothing, so expand_assignment's clear_storage has no
 *      target pattern to use and every struct-to-zero of any size becomes a
 *      memset call.  That alone explains the body below.
 *   2. THE BLOCK-MOVE EXPANDER REFUSES THIS LENGTH.  arm.md:5084-5106
 *      (`movstrqi`) FAILs for TARGET_THUMB when the byte count exceeds 48.
 *      This is 64.  So even a copy FROM a static zero object would not inline.
 *   3. THE BLOCK-MOVE EXPANDER CANNOT EMIT A STORE-ONLY stmia.  arm.c:9478-9526
 *      (`thumb_expand_movstrqi`) only ever emits `movmem12b` / `movmem8b`
 *      (arm.md:5110-5145), and both patterns are load-AND-store pairs over two
 *      pointer operands.  There is no pattern in the port that stores four
 *      registers to an incrementing address without loading them from memory.
 *   4. `mov ip, r4` / `mov r4, ip` IS NOT A gcc-2.96 THUMB IDIOM.  The thumb
 *      prologue's only IP_REGNUM shuffle (arm.c:9252, arm.c:9290) saves
 *      LAST_ARG_REGNUM, which is r3, not r4; a callee-saved low register is
 *      always pushed.  Saving r4 in ip is hand-written-assembly behaviour.
 *
 * So this belongs with the rom_f9000 not-built-by-gcc-2.96 set, beside
 * RealClearChain in src/non_matching/rom_f9000/80f9a30.c, and the body below
 * exists to record the test rather than to be improved.  Do not spend a round
 * spelling it; the spelling is not what is wrong.
 *
 * Parked: logic faithful, does NOT byte-match.
 * Candidate: tools/runs/run_20260607T170736Z/SoundMainBTM-iter-4.c
 * TODO(residual): rom_f9000 Sappy/m4a region: inline unrolled 64-byte zero-init
 * with `mov ip,r4` non-interwork register save; candidate emits a memset call.
 * Sappy/agbcc import track.
 */
struct S { unsigned int w[16]; };
void SoundMainBTM(struct S *p) {
    *p = (struct S){0};
}
