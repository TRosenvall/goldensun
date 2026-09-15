/* cos and sin  --  0x08002322 and 0x0800232a, asm/rom_c0/rom_1b70.s
 *
 * BLOCKER CLASS: HAND-WRITTEN ASSEMBLY. Not a codegen problem. Do not screen
 * these, and do not spend a round on them.
 *
 * THEY ARE ONE BODY WITH TWO ENTRY POINTS. `cos` is three instructions that add
 * a quarter turn and then FALL THROUGH into `sin`:
 *
 *     .thumb_func_start_noalign cos
 *         mov r1, #0x40
 *         lsl r1, #8
 *         add r0, r1
 *     .thumb_func_start_noalign sin
 *     sin:
 *         add r0, #0x20
 *         ...
 *
 * No compiler emits a second entry point into the middle of a function. gcc
 * would emit `cos` as a function that CALLS `sin`, or inlines it; either way
 * there is a `bl`/`bx` or a duplicated body, and neither is what the ROM has.
 * The `_noalign` spelling is itself part of the tell -- the two labels are two
 * bytes apart, so `cos` cannot be word-aligned the way a function entry is.
 *
 * THE REST OF THE FILE AGREES. rom_1b70.s is Camelot's hand-written runtime
 * library: `.thumb_stub` divide routines, ARM unrolled copy loops dispatched by
 * `add pc, r12, lsl #3`, `ldm`/`stm` block moves with shifted merges, and a
 * quarter-wave table reached by `adr r3, .L2344` over an `.incrom`.
 *
 * WHY THIS PARK EXISTS AT ALL: tools/elevation_candidates.py ranks `cos` FIRST
 * of 804 (score 1.5, three instructions) and `sin` eighth. The ranker scores
 * size and shape and knows nothing about hand-written code, so without a park
 * file it will keep offering them at the top of every candidate list. That is
 * the whole purpose of this file.
 *
 * SEE ALSO src/non_matching/rom_c0/2dd8.c, which proves the same thing about
 * gfree/free two files away by a different and cheaper route -- a literal pool
 * word shared between two functions, which gcc's per-function minipool cannot
 * produce. That detector does not fire here, because these two share a BODY
 * rather than a pool.
 */
