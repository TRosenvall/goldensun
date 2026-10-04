/* SoundMainBTM  [rom_f9000]
 *
 * NON-MATCHING, 11 of 12 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 12, ours 7) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   *** RELOCATIONS DIFFER IN THEIR SYMBOLS, NOT ONLY THEIR OFFSETS --
 *   
 *   SO THIS FIGURE IS NOT A DISTANCE: `make compare` cannot pass a
 *   relocation difference.  Fix this before trusting the encoding count. ***
 *   SIZE ref 24 bytes, ours 16.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f9000/rom_f9a18.c \
 *     asm/rom_f9000/rom_f95e0.s --func SoundMainBTM
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Source asm: goldensun/asm/rom_f9000/rom_f95e0.s
 *
 * Parked: logic faithful, does NOT byte-match (endgame permuter seed).
 * Candidate: tools/runs/run_20260607T170736Z/SoundMainBTM-iter-4.c
 * TODO(residual): rom_f9000 Sappy/m4a region: inline unrolled 64-byte zero-init with `mov ip,r4` non-interwork register save; candidate emits a memset call. Sappy/agbcc import track.
 */
struct S { unsigned int w[16]; };
void SoundMainBTM(struct S *p) {
    *p = (struct S){0};
}
