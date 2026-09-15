/* Func_801b9ec -- 0x0801b9ec, asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c.s
 *
 * BLOCKER CLASS: see src/non_matching/rom_15000/801b9a8.c, which analyses this
 * function and states it explicitly.
 *
 * THIS FILE IS A CROSS-REFERENCE, NOT A SECOND ANALYSIS. Do not duplicate the
 * work in the file above; read it and add any new measurement THERE.
 *
 * WHY IT EXISTS: tools/funcindex.py's park_subject() resolves exactly ONE
 * subject per park file, so a park covering several functions registers only
 * the first. Every other function it covers reads as a COLD TARGET in
 * tools/elevation_candidates.py and comes back to the top of the candidate
 * list. Func_801b9ec was picked up that way and re-derived from
 * scratch before the existing park was found.
 *
 * ONE PARK FILE PER FUNCTION is the convention (see rom_c0/2dd8.c and 2df0.c
 * for gfree/free, and rom_c0/1b70_cos.c and 1b70_sin.c) and this is the reason
 * for it. Widening park_for() to read every name in a park's prose was tried
 * in batch 266 and reverted: the name regex matches ordinary English, so 57
 * parks claimed "free". A false positive there hides an ELEVATABLE function
 * permanently, which is worse than the wasted round this prevents.
 */
