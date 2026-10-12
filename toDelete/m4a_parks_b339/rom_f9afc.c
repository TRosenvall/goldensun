/* BACKFILLED FIGURE (batch 337) -- NOT MATCHING, a SIZE mismatch: ref 20 bytes against ours 24.
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
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_f9000/rom_f9afc.c \
 *     asm/rom_f9000/rom_f95e0.s --func ply_pend
 *
 * THE FIGURE IS EVIDENCE; ANY DIAGNOSIS BELOW IS A HYPOTHESIS.  Across pass two
 * a park's stated blocker has been wrong roughly 40 times in 42.
 */
/* ply_pend  [rom_f9000]
 * Source asm: goldensun/asm/rom_f9000/rom_f95e0.s
 *
 * Parked: logic faithful, does NOT byte-match (endgame permuter seed).
 * Candidate: tools/runs/run_20260607T010203Z/ply_pend-iter-4.c
 * TODO(residual): larger branchy fn (rom_f9000); needs more than a 1-shot review. Permuter or hand-work.
 */
void ply_pend(unsigned char *mplayInfo, unsigned char *track)
{
    int count;
    count = track[2];
    if (count != 0) {
        count = count - 1;
        track[2] = count;
        *(unsigned int *)(track + 0x40) = *(unsigned int *)(track + (count << 2) + 0x44);
    }
}
