/* BaseAnim_Tiamat -- asm/rom_c9000/rom_d244c_c_c.s
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
 * SPLIT: TEXT/DATA split, 1 `.global` exports required FIRST.
 *
 * TRUE AGGREGATES: 1 (raw `add rX,sp,#K` grep said 1 too -- unaffected here).
 * Uses the SIXTH frame idiom twice: `mov rX,#K` then `add rX,sp`, to get a stack
 * address into a high register.  That form is matched by NEITHER `mov rX,sp` NOR
 * `add rX,sp,#K`, so a frame read that omits it loses two sites on this function.
 * One of the two functions in its brief whose aggregate count the inflated grep
 * did NOT distort.
 *
 * Verify with (for the day a candidate exists; there is no code below this
 * header, so tools/shimcount.py reports 0 shims):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/BaseAnim_Tiamat.c \
 *     asm/rom_c9000/rom_d244c_c_c.s --func BaseAnim_Tiamat
 */
/* No candidate body.  See the header: this park claims no figure. */
