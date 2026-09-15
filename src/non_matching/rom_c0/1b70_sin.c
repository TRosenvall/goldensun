/* sin  --  0x08002322, asm/rom_c0/rom_1b70.s
 *
 * BLOCKER CLASS: HAND-WRITTEN ASSEMBLY. Not a codegen problem. Do not screen
 * this, and do not spend a round on it.
 *
 * `sin` IS THE FALL-THROUGH TARGET OF `cos`. The two are ONE BODY WITH TWO
 * ENTRY POINTS -- `cos` at 0x0800231c is three instructions that add a quarter
 * turn and then fall into `sin` at 0x08002322, six bytes later. No compiler
 * emits a second entry point into the middle of a function, and the `_noalign`
 * spelling is part of the tell: six bytes apart, `sin` cannot be word-aligned
 * the way a function entry is.
 *
 * The body is a quarter-wave table lookup reached by `adr r3, .L2344` over an
 * `.incrom`, with the quadrant reconstructed from the carry out of two shifts
 * -- `bcc` twice, negating the result on one path. The rest of rom_1b70.s
 * agrees: `.thumb_stub` divide routines, ARM unrolled copy loops dispatched by
 * `add pc, r12, lsl #3`, and `ldm`/`stm` block moves with shifted merges.
 *
 * SEE src/non_matching/rom_c0/1b70_cos.c for the full argument. This file
 * exists as a SEPARATE park because tools/funcindex.py's park_subject() returns
 * exactly ONE subject per park file, so a single park covering both functions
 * leaves the second reading as a cold target -- which is how `sin` came back to
 * the top of the candidate list after `cos` was parked. One park file per
 * function is the tree's own convention (see 2dd8.c and 2df0.c for gfree/free)
 * and it is the convention for this reason.
 *
 * Widening park_for() to register every name a park's header mentions was tried
 * and REVERTED: tools/funcindex.py's NAME regex matches ordinary prose, and
 * `sin`, `cos` and `free` are all English words as well as symbols, so 57 parks
 * claimed `free`. A false positive there makes an elevatable function look
 * permanently parked, which is strictly worse than the missed park it fixes.
 */
