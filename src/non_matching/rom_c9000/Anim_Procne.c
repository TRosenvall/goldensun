/* Anim_Procne -- asm/rom_c9000/rom_d1714.s
 *
 * TRIAGE ONLY, NO CANDIDATE WRITTEN, and so NO objcmp figure.  This header
 * invents none.  The measured triage lives in docs/RECON_Anim_Procne.txt.
 *
 * This park exists so the function reads as ATTEMPTED.  tools/census.py
 * classifies by whether a park file exists, not by any ledger of per-function
 * verdicts -- which is how `gfree` was once handed out as an available target
 * while already being unmatchable.  Batch 313 produced real triage for this
 * function as a RECON text file, and without this park the next census would
 * have re-offered it as untouched and the analysis would have been redone.
 *
 * SPLIT: TEXT/DATA split, 7 `.global` exports required FIRST.
 *
 * TRUE AGGREGATES: 10, not the 19 the raw grep reports -- NINE of the nineteen
 * are plain RE-MATERIALISATION, the same offset formed twice, which is the ONLY
 * one of the seven resolution classes that de-duplicating offsets can fix.
 * *** THE HARDEST OF ITS GROUP: *** 356 frame bytes (closed accounting, exact),
 * 24 spilled scalars, two maxreload quantities.
 * *** AND THE DOCUMENT'S SHARPEST BACKWARD-EDGE CORRECTION IS ABOUT THIS
 * FUNCTION. ***  A raw mnemonic census reads 27 `bne` against 38 signed, and on
 * that basis a brief called it "the function Gaia's `!=` lever would damage most:
 * 38 sites".  Classified properly it has 19 BACK EDGES, 15 closing on `bne` and
 * ZERO SIGNED LOOP CLOSURES -- the 38 signed comparisons are clamps, division
 * corrections and phase tests, none of them loop closures.  So it is the function
 * that takes the `!=` rule MOST CLEANLY, and the proxy would have argued a
 * correct lever out of it.  A COMPARISON CENSUS IS NOT A LOOP CENSUS.
 * SPLIT: needs a TEXT/DATA cut with 7 `.global` exports -- a gated asm commit
 * stands in front of any candidate, so it could not have landed this batch at
 * any candidate quality.
 *
 * Verify with (for the day a candidate exists; there is no code below this
 * header, so tools/shimcount.py reports 0 shims):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Procne.c \
 *     asm/rom_c9000/rom_d1714.s --func Anim_Procne
 */
/* No candidate body.  See the header: this park claims no figure. */
