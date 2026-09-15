/*
 * ### BATCH 265 -- DO NOT RE-TRY. The register rotation recorded below cannot
 * be closed: rom_2dd8.s is hand-assembled, not gcc output. The proof is one
 * literal pool word at 0x08002dfc serving BOTH this function and gfree, which
 * gcc-2.96's per-function minipool cannot produce. See the batch-265 header of
 * src/non_matching/rom_c0/2dd8.c for the full argument and for the cheap
 * cross-function-pool detector it generalises to.
 */

