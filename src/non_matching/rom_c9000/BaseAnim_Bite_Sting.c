/* BaseAnim_Bite_Sting -- asm/rom_c9000/rom_ca57c_c_c_c_c_c_c_c_c_c_c_c_c.s
 *
 * TRIAGE ONLY, NO CANDIDATE WRITTEN, and so NO objcmp figure.  This header
 * invents none.  The measured triage lives in docs/RECON_BaseAnim_Bite_Sting.txt.
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
 * TRUE AGGREGATES: 5, not the 6 the raw grep reports.
 * *** ITS maxreload OF 23 IS THE DOCUMENT'S EXAMPLE OF A MISLEADING FIGURE. ***
 * All 23 are ONE ARRAY BASE ADDRESS -- `.Ledf04`, an 84-byte table read
 * `ldrb [base,index]` at 23 sites.  Partitioned, only 15 of 89 pool references
 * are true numeric constants: the LOWEST ratio of its group, making this the
 * CLOSEST of the four to a PURE REBUILD -- the exact opposite of what "highest
 * maxreload in the batch" suggests.  Partition before reading a reload figure.
 * BACK EDGES: 16, of which 13 close on `bne` and ONE is signed (`bgt`, ref line
 * 702).  So the all-`!=` loop rule applies here.
 * Also the clean sighting of the PHANTOM-LOAD-ONLY trap: +0x84/+0x88 read as
 * holes but are words of the aggregate based at +0x84, stored through a
 * materialised base the `[sp,#imm]` census cannot see.
 *
 * Verify with (for the day a candidate exists; there is no code below this
 * header, so tools/shimcount.py reports 0 shims):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/BaseAnim_Bite_Sting.c \
 *     asm/rom_c9000/rom_ca57c_c_c_c_c_c_c_c_c_c_c_c_c.s --func BaseAnim_Bite_Sting
 */
/* No candidate body.  See the header: this park claims no figure. */
