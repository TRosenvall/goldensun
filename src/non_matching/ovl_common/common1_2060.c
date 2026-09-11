/* OvlFunc_common1_2060 -- asm/overlays/common/common1_c_c_b.s
 *
 * LENGTH EXACT (163 against 163 encodings). Two numbers are recorded because
 * they measure different things and both appeared in the attempt: 20 by aligned
 * instruction distance, 91 by objcmp's encoding count. Trust the objcmp figure
 * as the gate number; the aligned figure is what the sweep was steered by.
 * Candidate: scratch_elev/b258/common/final/park_common1_2060.c.
 *
 * ONE LEVER IS ALREADY LOAD-BEARING AND IS THE OPPOSITE OF ITS FILE-MATES'.
 * "Name the address, not the offset" ran BOTH WAYS in one batch: common1_2c4
 * and common1_148 need the address named, and this one needs it SPLIT into base
 * plus offset -- worth 41 -> 27, and it restores a fourth push. The ROM's
 * load/store form is the discriminator, not a preference. Do not transplant the
 * file-mates' spelling here.
 *
 * `.L9` is read out of this file's own `.data`, and the verified spelling is
 * `extern int L9[] __asm__(".L9");`.
 *
 * *** LANDING IS EXPENSIVE: WHOLE+DATA, HAND SPLIT, AND THREE OVERLAYS. ***
 * asmfacts.py says `2 functions  split first` and UNDERSTATES it. The .s also
 * carries:
 *      .data   eleven .incbin ranges plus gOvlCommon1_3fe4
 *      .data1
 *      .bss    31 .lcomm
 * and 2060 reads `.L9` out of that .data, so the data MUST stay in an asm
 * sibling -- split_s.py cannot cut this, exactly the recorded WHOLE+DATA shape.
 *
 * The object is shared across three overlays, so FOUR linker lines change in
 * EACH of three scripts. That is twelve line edits by hand, which is the most
 * expensive landing shape in the tree. Worth closing the file-mate first so the
 * cut pays for two functions rather than one.
 *
 * For contrast, the sibling file common1_a_a_a_a_a_c_a.s had no data, so
 * split_s.py cut it and rewrote all six of ITS linker lines automatically. The
 * difference between those two landings is entirely the .data section.
 */
