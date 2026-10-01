/* BaseAnim_ParticleSpray -- asm/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_a.s
 *
 * TRIAGE ONLY, NO CANDIDATE WRITTEN, and so NO objcmp figure.  This header
 * invents none.  The measured triage lives in docs/RECON_b313f_three.txt.
 *
 * This park exists so the function reads as ATTEMPTED.  tools/census.py
 * classifies by whether a park file exists, not by any ledger of per-function
 * verdicts -- which is how `gfree` was once handed out as an available target
 * while already being unmatchable.  Batch 313 produced real triage for this
 * function as a RECON text file, and without this park the next census would
 * have re-offered it as untouched and the analysis would have been redone.
 *
 * SPLIT: NO SPLIT NEEDED -- direct convert.
 *
 * TRUE AGGREGATES: 3 (raw grep said 3).
 * *** CHEAPEST INSTALL PATH OF ITS GROUP: NO SPLIT AT ALL. ***  A direct convert
 * -- its eight data labels are already `.global` elsewhere, so there is no gated
 * asm commit standing in front of a candidate.  Of the batch's targets this is
 * among the few where install cost is zero, and install cost is independent of
 * difficulty.
 * Carries 32 `str rX,[sp]` sites, so read its outgoing-argument space before its
 * frame depth -- a high count there makes the CALLS wide, not the frame deep.
 * Also holds an ADDRESS-TAKEN SCALAR, the seventh resolution class, at 0x88/0x8c
 * in `f(&a)` form -- neither an aggregate nor a sub-word scalar.
 *
 * Verify with (for the day a candidate exists; there is no code below this
 * header, so tools/shimcount.py reports 0 shims):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/BaseAnim_ParticleSpray.c \
 *     asm/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_a.s --func BaseAnim_ParticleSpray
 */
/* No candidate body.  See the header: this park claims no figure. */
