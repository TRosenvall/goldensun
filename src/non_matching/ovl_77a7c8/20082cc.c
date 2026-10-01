/* OvlFunc_881_20082cc -- asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.s,
 * NON-MATCHING, 3 of 16 encodings (re-measured batch 317 as installed).
 * 0x020082cc, 13 instructions + TWO literal pool words.
 * Park: src/non_matching/ovl_77a7c8/20082cc.c
 *
 * Verify with (the park body, as installed):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77a7c8/20082cc.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.s --func OvlFunc_881_20082cc
 *
 * FIGURE IN  : 12 of 16 encodings, and SIZE WRONG (ref 36, ours 32) and
 *              RELOCATIONS WRONG. The park claims "13 differing positions ...
 *              to 10"; the ten is a tryc positional figure and NOT a distance,
 *              because the park body is missing a pool word. objcmp is the
 *              authority: 12 of 16, size short by 4, relocation at the wrong
 *              offset. No recipe in the tree ever measured it.
 * FIGURE OUT : 2 of 16 encodings, SIZE EXACT (36/36), and all 13 instructions
 *              in the ROM's order with the ROM's exact registers -- BUT with a
 *              measurement placeholder standing in for one symbol, see below.
 *
 * SPLIT SHAPE (combined, planned with its file-mate 20082f0 as instructed):
 *   datacheck.py asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.s -> rc=0, NO data
 *   section. The .s holds exactly TWO functions, 20082cc and 20082f0.
 *   -> IF BOTH LAND, NO SPLIT IS NEEDED AT ALL and no linker script changes:
 *      write ONE src/overlays/rom_77a7c8/ovl_30_c_a_c_a_a_c_a.c holding both
 *      functions in ROM order and verify with `objcmp --whole`.
 *   -> IF ONLY ONE LANDS, split_s.py is needed, and NOTE THE NAMING TRAP: the
 *      two dry-runs disagree, because split_s.py names by which function is cut
 *      out.  `split_s.py <s> OvlFunc_881_20082cc --dry-run` writes _b + _c;
 *      `split_s.py <s> OvlFunc_881_20082f0 --dry-run` writes _a + _b. Both
 *      rewrite overlays/rom_77a7c8/overlay.ld. Do not run both.
 * PINS: 0 in the candidate below.
 *
 * TWO INDEPENDENT BLOCKERS. THE PARK NAMED ONE AND MISATTRIBUTED THE REST.
 * ======================================================================
 * The park's residue reads: "a POOLED ZERO ... that operand was a SYMBOL whose
 * value is zero", and then "The rest of the diff is genuine register allocation
 * and is downstream of the pooled zero -- gcc spends a register on `mov r2, #0`
 * that the ROM spends on the pool load."
 *
 * BLOCKER 1 -- THE POOLED ZERO. The park's claim SURVIVES and is now proved
 * rather than asserted. Six spellings were measured
 * (scratch_elev/b317/A/probe/zero.c): a literal 0; a zero int local; a VOLATILE
 * zero int local; `(unsigned char)0x10000`; an int local holding 0x10000; an
 * unsigned local holding 0xffffff00. ALL SIX emit `mov r3, #0` / `strb r3,[r0]`.
 * gcc-2.96 Thumb will not put a value in the pool that an 8-bit `mov` can
 * build, and no 32-bit value V with V == 0 fails to be 8-bit-movable. So the
 * ROM's pool word 0x00000000 is a RELOCATION against a symbol whose linked
 * value is zero. CONFIRMED FROM THE LINKED OVERLAY, which also corrects the
 * park's picture of the layout -- arm-none-eabi-objdump on
 * overlays/rom_77a7c8/overlay.elf:
 *       20082e4: 4770       bx lr
 *       20082e6: 0000       .short 0x0000     <- alignment pad
 *       20082e8: 00000000   .word  0x00000000 <- the pooled zero, reached by
 *                                                `ldr r1,[pc,#12]` at 20082da
 *       20082ec: 03001e70   .word  0x03001e70
 *   13 instructions (26) + 2 pad + 8 pool = 36 = the ROM's size exactly.
 *
 * BLOCKER 2 -- THE POOL WORD ORDER. NEW, NOT IN THE PARK, AND IT IS WHAT THE
 * PARK CALLED "genuine register allocation". It is not register allocation:
 * with the zero supplied as a placeholder symbol, ALL THIRTEEN INSTRUCTIONS
 * COME OUT IN THE ROM'S ORDER WITH THE ROM'S EXACT REGISTERS, and the only two
 * differing encodings are the two pool-load OFFSETS at indices 0 and 7 --
 * because gcc emits the pool as [iwram_3001e70, zero] and the ROM has
 * [zero, iwram_3001e70].
 *
 *   The ROM : 4b07 = ldr r3,[pc,#28] -> 0x20082ec (iwram, the SECOND word)
 *             4903 = ldr r1,[pc,#12] -> 0x20082e8 (zero,  the FIRST word)
 *   Ours    : 4b06 / 4904, the two words swapped.
 *
 *   THIS IS THE objcmp DOCSTRING'S "POOL ORDER" CLASS (batch 218's
 *   Func_80982dc: "nine pool words are the ROM's nine, rotated"), and it is
 *   exactly the case that docstring warns tryc cannot see -- tryc normalises
 *   both sides to `=value` and reports this candidate as ONE differing line
 *   (the placeholder's name) while objcmp reports TWO encodings. I was misled
 *   by that for one round; the sweep was re-run under objcmp.
 *
 *   AND THE ORDER IS INVARIANT UNDER SOURCE ORDER. All 60 legal statement
 *   permutations were swept twice, once under tryc and once under objcmp
 *   (scratch_elev/b317/A/x3/, generator gen3.py, RESULTS_OC.txt). objcmp
 *   histogram: 3 at 2, 8 at 7, 24 at 8, 9 at 9, 16 at 11 -- and EVERY ONE of
 *   the 60 reports the identical relocation pair, iwram at 0x1c and the
 *   placeholder at 0x20. Moving the zero's materialisation to the FIRST
 *   statement does not move its pool word (t_QZBOAV: pool still iwram-first,
 *   and the instruction order degrades to 11). So the pool order is not set by
 *   insertion order and no statement order reaches it.
 *
 *   ONE THING DOES MOVE IT, and it is the wrong thing: declaring the loaded
 *   value `unsigned char` instead of `unsigned int` puts the zero's word FIRST,
 *   the ROM's order -- and simultaneously makes gcc emit a byte-mode pool load
 *   and DUMP THE POOL BEFORE THE EPILOGUE, costing an extra `b .L4` with
 *   `bx lr` after the pool (14 instructions, 40 bytes against the ROM's 36).
 *   That is the batch-71 narrow-constant symptom biting the pool allocator
 *   rather than the arithmetic, and it is worth recording: THE POOL ORDER AND
 *   THE POOL PLACEMENT ARE COUPLED THROUGH THE ENTRY'S MODE.
 *
 * WHAT MOVED IT, 12 -> 2
 * ======================
 * The same lever that carried target 2 of this brief: the park reached
 * `iwram_3001e70` so that reload materialised the symbol address and REUSED ONE
 * REGISTER for the symbol address and the pointer loaded from it (`ldr r3,[r3]`
 * where the ROM has `ldr r3,=sym / ldr r2,[r3]`). Naming the block pointer as
 * its own local makes them two quantities and gives the ROM's five registers.
 * Crossed with the width fix on the pooled value (`unsigned int`, not
 * `unsigned char`) this is 12 -> 2. Neither half alone is enough: the width fix
 * alone keeps the extra branch, and the naming alone keeps the register reuse.
 *
 * FLAGS, both exactly inert on this function: -fno-expensive-optimizations
 * (10 -> 10 on the park body, 4 -> 4 on the candidate), -fno-strict-aliasing,
 * -fno-gcse, -fno-rerun-cse-after-loop, -fno-peephole, -fno-regmove.
 * -fno-schedule-insns2 is much worse (9 of 13 positional).
 *
 * THE SYMBOL REQUEST: WITHHOLD IT.
 * ================================
 * I am NOT asking for a `*.sym` entry, and the reason is the project's own
 * standard rather than the strength of the evidence. The structural argument
 * for a zero-valued symbol is sound and is now proved on six spellings -- that
 * is the accepted class ("an 8-bit-movable value cannot reach the pool as a
 * const_int, so a pool word holding it must be a relocation"). But evidence
 * quality is a SEPARATE test from completion, and A NAME DOES NOT COMPLETE THIS
 * FUNCTION: blocker 2 is independent of it and would still leave 2 of 16 and a
 * failing `make compare`. This is the `_FILE_e4` situation exactly -- accepted
 * argument, withheld because it does not complete the function. Granting a name
 * here would spend the project's scarcest currency and land nothing.
 *
 * If the pool-order blocker is ever solved, the request becomes worth making,
 * and the second instance the park points at (src/non_matching/rom_b0000/
 * rom_b09fc.c, `ldr r6, =0x0`) is still the only disambiguating signal and is
 * still not enough on its own.
 *
 * DECLINING TO CLOSE. The park's pooled-zero diagnosis SURVIVES and is
 * strengthened; its "the rest is genuine register allocation" diagnosis is
 * REFUTED -- the rest is the pool word order, and the register allocation is
 * already exactly the ROM's.
 *
 * ---------------------------------------------------------------------------
 * THE BODY BELOW IS DEVICE-FREE, AND THAT IS WHY ITS FIGURE IS 3 AND NOT 2.
 *
 * The agent's candidate reached 2 of 16 using an `extern unsigned char
 * ZEROSYM_PLACEHOLDER[]` whose only purpose was to put a relocation where the
 * ROM has one, and LABELLED IT AS A MEASUREMENT DEVICE THAT MUST NOT SHIP --
 * correctly, by the standard in docs/elevation.md: it improves the figure by
 * changing what is measured rather than what is compiled.  That is the fifth
 * device caught in this project and the first one caught by its own author
 * before a coordinator saw it.
 *
 * The device was removed here (`z = 0;` for the placeholder's address) and the
 * body RE-MEASURED as installed:
 *     3 of 16 (ref 16 encodings, ours 15), first diff at index 0,
 *     relocation present at 0x1c against the reference's 0x20 -- a SHIFTED
 *     OFFSET consequent on being one pool word short, not a third blocker.
 * So the honest improvement is 12 of 16 WITH SIZE WRONG -> 3 of 16, and the
 * one missing encoding is exactly the pooled-zero symbol the analysis above
 * identifies.  The 2 of 16 remains useful as a figure ABOUT THE REMAINING
 * BLOCKER -- it says that once that one pool word is a relocation, everything
 * else in this function is already right.
 *
 * THE SYMBOL IS WITHHELD.  The argument for naming the zero-valued symbol is
 * sound, but the name does not COMPLETE the function, which is the _FILE_e4
 * standard: evidence quality and completion are separate tests, and a good
 * structural argument is necessary without being sufficient.
 */
extern unsigned int iwram_3001e70;


/* Copies a halfword out of the block at iwram_3001e70 into a sprite field at
 * +0x1e and clears the byte at +0x26. The symbol holds a POINTER and the
 * 0x11a offset applies to what it points at. */
unsigned int OvlFunc_881_20082cc(unsigned char *p)
{
    unsigned char *blk;
    unsigned char *q;
    unsigned int off;
    int v;
    unsigned int z;

    blk = (unsigned char *)iwram_3001e70;
    off = 0x11a;
    q = *(unsigned char **)(p + 0x50);
    blk += off;
    v = *(unsigned short *)blk;
    z = 0;
    *(unsigned short *)(q + 0x1e) = v;
    *(q + 0x26) = z;
    return 1;
}
