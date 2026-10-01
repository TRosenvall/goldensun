/* BaseAnim_Meteor -- asm/rom_c9000/rom_e7320_c_c.s
 *
 * TRIAGE ONLY, NO CANDIDATE WRITTEN, and so NO objcmp figure.  This header
 * invents none.  The measured triage lives in docs/RECON_BaseAnim_Meteor.txt.
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
 * TRUE AGGREGATES: 8, not the 14 the raw grep reports -- and the excess has
 * THREE different causes here: sub-word scalars (+0x34/+0x38), a LOOP END
 * SENTINEL (+0x118), and a HIDDEN REGISTER ARGUMENT at +0x11c, which is a
 * frame-top pointer handed to `Func_80e7338` IN r9.  That last one is a CALLING
 * CONVENTION, not a local, and no amount of declaration work will produce it.
 * *** SPLIT UNBLOCKED, AND A SUFFIX COLLISION. ***  Batch 312's twelve `.global`
 * exports landed, so `split_s.py --dry-run` now SUCCEEDS on this file and prints
 * a three-way 131 / 1750 / 3765 shape.  BUT five parks in this directory plus one
 * recon all name `rom_e7320_c_c_b` as their own stem and only ONE can have it --
 * the stem depends on the member's POSITION and the tool cuts one named target,
 * so re-derive stems from a FRESH dry-run before writing linker rows.
 * *** WHY IT GOT A RECON AND NOT A CANDIDATE: IT IS A TWO-VARIANT FUNCTION. ***
 * `r1` branches the prologue, BOTH frame loops and the teardown at about a dozen
 * sites, so a candidate needs the if-nesting right BEFORE any lever is
 * measurable -- and a wrong nesting is INDISTINGUISHABLE from a codegen residue.
 * That is the shape to settle first; the declaration list in the recon is already
 * derived from a closed byte accounting.
 * BACK EDGES: 23, of which 20 close on `bne` and one is signed (`ble`, ref 448).
 *
 * Verify with (for the day a candidate exists; there is no code below this
 * header, so tools/shimcount.py reports 0 shims):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/BaseAnim_Meteor.c \
 *     asm/rom_c9000/rom_e7320_c_c.s --func BaseAnim_Meteor
 */
/* No candidate body.  See the header: this park claims no figure. */
