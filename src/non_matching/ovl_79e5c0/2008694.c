/* OvlFunc_911_2008694 -- 0x02008694,
 * asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c_c_a_c.s
 *
 * FIVE differing encodings of 142, plus two EXPECTED `_AREA` pool words.
 * Candidate: scratch_elev/b256/rich/ (final/ holds the two-function file; this
 * function is the second one). Its file-mate OvlFunc_911_20083c8 is ELEVATED,
 * under a GCSE_CFLAGS rule, so this .s is the `_c` remainder of that split.
 *
 * BLOCKER CLASS: REG_ALLOC_ORDER. At a `ldrsh rD,[rB,rI]` whose result dies at
 * the very next `cmp`, gcc gives the free low register to the ADDRESS OFFSET
 * and the ROM gives it to the ZERO INDEX. The identically shaped site four
 * instructions earlier -- where the result IS live afterwards -- comes out
 * byte-exact, which is what isolates the discriminator to the death of the
 * result rather than to the addressing form.
 *
 * ALIAS IS THE WRONG AXIS: `-fno-schedule-insns2` REGRESSES, 11 -> 34, so by
 * the recorded sign rule sched2 is already producing the ROM's order.
 *
 * REFUTED, with measurements in the scratch NOTES.md -- do not re-spend any of
 * these: 11 block spellings; ALL 120 declaration permutations (byte-identical,
 * a second clean datum for "declaration order is usually inert"); 4 pin widths
 * and registers; the volatile-asm escape; 4 guard restructurings; 8 flag
 * combinations.
 *
 * THE TWO `_AREA` WORDS ARE NOT DEFECTS. `ldr r3,=0x27` and `=0x26` at
 * gState+0x1C0 are the recorded pooled-constant-below-256 tell. Written as
 * `(int)(&_AREA_27)` and `(int)(&_AREA_26)` they produce the documented
 * signature -- one differing encoding and one ours-only relocation each --
 * which `make compare` resolves, since `_AREA_27 = 0x27` is in area.sym.
 * Written as literals gcc emits `cmp r3,#39` and the pool changes.
 *
 * `.L32d8` is already `.global` in asm/overlays/rom_79e5c0/ovl_30_c_c_c.s:20
 * and is reached with `extern unsigned char L32d8[] __asm__(".L32d8");`.
 *
 * LANDING IF CLOSED: this becomes a further split of the _c remainder, or --
 * better -- the two halves collapse back toward one TU. Note the file-mate
 * ships under -fno-gcse, so a combined TU would put BOTH functions under that
 * flag; check that this one still matches with it before merging.
 * The overlay.ld lines MUST KEEP their asm/ paths.
 */
