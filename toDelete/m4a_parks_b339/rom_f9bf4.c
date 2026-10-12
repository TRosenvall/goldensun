/* BACKFILLED FIGURE (batch 337) -- NOT MATCHING, a SIZE mismatch: ref 12 bytes against ours 16.
 *
 * This park carried NO PARSEABLE FIGURE AND NO RECIPE, so it was invisible to
 * every targeting instrument in this tree: absent from the figure bands,
 * NO CLAIM in parkcheck, unrankable in reports/frontier.md.  170 of 675 park
 * files were in that state; 26 of them had a compilable body, which means a
 * figure was one compile away.  tools/backfill.py found them and this is the
 * measurement.
 *
 * objcmp stops at the size check, so there is no encoding figure yet; the body must reach the reference's LENGTH before a distance exists.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_f9000/rom_f9bf4.c \
 *     asm/rom_f9000/rom_f95e0.s --func ply_lfodl
 *
 * THE FIGURE IS EVIDENCE; ANY DIAGNOSIS BELOW IS A HYPOTHESIS.  Across pass two
 * a park's stated blocker has been wrong roughly 40 times in 42.
 */
/* ply_lfodl  [rom_f9000]
 * Source asm: goldensun/asm/rom_f9000/rom_f95e0.s
 *
 * Parked: logic faithful, does NOT byte-match (endgame permuter seed).
 * Candidate: tools/runs/run_20260607T031612Z/ply_lfodl-iter-1.c
 * TODO(residual): rom_f9000 Sappy/m4a region: mov ip,lr / bx ip non-interwork epilogue. Sappy/agbcc import track.
 */
extern int Func_80f9ab4();

void ply_lfodl(unsigned int mplayInfo, unsigned char *track) {
    unsigned char r3;
    r3 = Func_80f9ab4(mplayInfo, track);
    track[0x1b] = r3;
}
