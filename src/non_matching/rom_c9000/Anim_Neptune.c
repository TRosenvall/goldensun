/* Anim_Neptune -- asm/rom_c9000/rom_eb754_c_c.s
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
 * SPLIT: See the recon; run tools/datacheck.py before assuming.
 *
 * TRUE AGGREGATES: 4, NOT the 5 the raw grep reports.  `sp+0x28` is
 * `add r4,sp,#0x28 / ldrh r4,[r4]` -- a u16 SCALAR, which Thumb-1 forces to
 * materialise a base because it has no sp-relative `ldrh`.
 * *** AND IT CARRIES THE BLOCK-AWARENESS TRAP LIVE: *** the register holding
 * `sp+0x48` is REASSIGNED to `gBuffer` three blocks later and then walked, so a
 * first-use scan that does not stop at a label or branch reports a phantom
 * 0x1c-byte stack aggregate there.  Resolve every `add rX,sp,#K` BLOCK-AWARE.
 *
 * Verify with (for the day a candidate exists; there is no code below this
 * header, so tools/shimcount.py reports 0 shims):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Neptune.c \
 *     asm/rom_c9000/rom_eb754_c_c.s --func Anim_Neptune
 */
/* No candidate body.  See the header: this park claims no figure. */
