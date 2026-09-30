/* OvlFunc_common1_920 -- overlay COMMON1, asm/overlays/common/common1_a_a_a_a_c_c_a_a_a_c.s
 *
 * NOT ATTEMPTED -- BLOCKED BEFORE A CANDIDATE EXISTS.
 * There is deliberately NO "NON-MATCHING, N of M encodings differ" line and no
 * candidate body in this file. That is not an omission: any candidate for this
 * function written with the asm-label extension WOULD BE A WRONG PROGRAM (see
 * below), and this project's rule is not to ship one. parkcheck will find no
 * figure here to re-measure because there is nothing legitimate to measure yet.
 *
 * THE FUNCTION: 531 instructions, 38 labelled blocks, a 6-way dispatch on a
 * script opcode inside an outer loop, reading a script pointer from a global
 * and writing decoded fields back to nineteen separate global halfwords.
 * Prologue saves r8, r9, r10, r11 -- so it is ALSO in the register-rotation
 * targeting class elevation.md warns off ("prefer targets without it"). But
 * that is moot; the label problem is upstream of any codegen question.
 *
 * THE BLOCK: THE ASM-LABEL CAPTURE HAZARD, NINETEEN TIMES OVER
 *
 * The function reaches nineteen data labels across the object boundary, and
 * EVERY ONE of them is a one- or two-digit `.L` name:
 *
 *   .L10 .L18 .L22 .L23 .L24 .L25 .L27 .L28 .L30 .L33
 *   .L34 .L35 .L36 .L37 .L39 .L43 .L44 .L46 .L47
 *
 * elevation.md already records the mechanism ("A one-digit asm-label extern can
 * bind to gcc's OWN branch target -- a wrong program, silently", found on
 * OvlFunc_common1_1b08) and instructs: if the label number is one or two
 * digits, GENERATE the candidate's .s and grep for that exact label before
 * trusting it. Done, and the answer is the worst possible one.
 *
 * MEASURED, NOT ASSUMED -- two independent generations:
 *
 *  1. The batch-302 sibling candidate OvlFunc_890_2008488 (517 instructions,
 *     ~20 blocks -- SMALLER than this function) generates:
 *        .L2 .L3 .L5 .L6 .L7 .L8 .L9 .L10 .L11 .L12 .L13 .L14 .L15 .L16
 *     So gcc reaches .L10 on a function smaller than this one. `.L10` is
 *     already captured.
 *
 *  2. A synthetic probe matched to THIS function's shape (a switch dispatch
 *     plus ~38 labelled blocks inside a loop; scratch_elev/b302d/probe_labels.c)
 *     generates labels up to .L69, and its set contains:
 *        .L18 .L22 .L23 .L24 .L25 .L27 .L28 .L30 .L33 .L34
 *        .L35 .L36 .L37 .L39 .L43 .L44 .L47
 *     -- SEVENTEEN of the nineteen names this function needs, in one compile.
 *     (.L10 and .L46 are missing from that particular run's set, but .L10 is
 *     demonstrated captured by generation 1, and .L46 sits between .L45 and
 *     .L47, both of which that run emits. The gap is an artifact of the probe's
 *     exact block count, not a safe hole -- gcc's counter is dense through this
 *     whole range and one edit to the function moves it.)
 *
 * So all nineteen names lie inside gcc's own generated label range for a
 * function of this size and shape. An `extern ... __asm__(".L24")` binds to
 * gcc's branch target, the assembler resolves it to the wrong address, and
 * there is NO build error and NO link error. It is a silently wrong program.
 * Four-digit labels (.L16c0, .Lee064) are safe because gcc's counter does not
 * reach them; two-digit ones in a 38-block function are not merely risky, they
 * are reliably taken.
 *
 * WHY THIS IS WORSE HERE THAN ON OvlFunc_common1_1b08. 1b08 needed two labels
 * (.L4, .L5). This needs nineteen. And the file is COMMON1 -- the shared
 * overlay linked by several overlays -- so the remedy is not candidate-level.
 *
 * THE REMEDY, AND IT IS AN OWNER DECISION, NOT A CANDIDATE FIX:
 * Rename the nineteen data labels in the shared common1 data file to exported
 * names (the established spelling in this tree is gOvlCommon1_<offset>, as used
 * for the .L7 -> gOvlCommon1_3fe4 rename elevation.md records) and export the
 * new names. That edits a SHARED data file which several overlays link, so it
 * must be decided and landed by the owner, once, for the whole common1 bank --
 * not smuggled in beside one function's conversion. Note the tree has already
 * paid this cost once for a single label; nineteen is the same operation
 * nineteen times in one file, and it makes this function and very likely
 * several of its common1 siblings elevatable at all.
 *
 * WHAT COULD BE MEASURED WITHOUT THE RENAME, if someone wants a codegen figure
 * before paying for it: write the candidate against nineteen SAFELY-NAMED
 * placeholder externs instead of the .L names. objcmp compares UNLINKED
 * objects, so each of those is an R_ARM_ABS32 pool word that reads 0 in both
 * streams; the ENCODINGS would be directly comparable and only the relocation
 * SYMBOL NAMES would differ -- by construction, which is the relocation-FORM
 * non-residue, not a defect. That yields an honest codegen distance while
 * shipping nothing. It was not done in this batch because the function cannot
 * land either way until the rename happens, and the budget went to targets
 * that can.
 *
 * SHIMS: n/a -- no candidate. (Note for whoever writes one: the asm-label
 * extension is NOT a shim tools/shimcount.py counts, so a wrong program of
 * this kind would have reported PIN-FREE and looked clean.)
 *
 * SPLIT SHAPE: NO SPLIT NEEDED.
 *   asm/overlays/common/common1_a_a_a_a_c_c_a_a_a_c.s holds exactly ONE
 *   function (OvlFunc_common1_920) and tools/datacheck.py reports NO data
 *   section in it, so no text split and no data split are required and no
 *   label needs `.global` from this file. The nineteen labels above are
 *   DEFINED ELSEWHERE in the common1 bank -- that is precisely why they have
 *   to cross an object boundary and why the capture hazard applies at all.
 *
 * Verify with (once a legitimate candidate exists):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/common/common1_a_a_a_a_c_c_a_a_a_c.c \
 *     asm/overlays/common/common1_a_a_a_a_c_c_a_a_a_c.s --func OvlFunc_common1_920
 *
 * FINAL INSTALLED PATH (when unblocked):
 *   src/overlays/common/common1_a_a_a_a_c_c_a_a_a_c.c
 */
