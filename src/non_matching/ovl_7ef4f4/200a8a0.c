/* OvlFunc_965_200a8a0 -- 0x0200a8a0   (overlay 965, rom_7ef4f4)
 *
 * RECON ONLY -- NO CANDIDATE WAS WRITTEN, so there is deliberately no
 * "NON-MATCHING, N of M encodings differ" line and nothing for parkcheck to
 * re-measure. Batch 302 brief D carried four targets and the instruction was to
 * prefer three well-understood functions over four shallow ones; this is the
 * one that was dropped, and this file records what was established about it so
 * the next attempt does not re-pay for it. 498 instructions.
 *
 * SPLIT SHAPE -- THIS IS THE USEFUL PART, AND IT IS THE UNUSUAL ONE OF THE FOUR.
 * A TEXT/DATA SPLIT IS REQUIRED. tools/datacheck.py, verbatim:
 *     data sections : .data
 *     functions     : OvlFunc_965_200a8a0
 *     EXPORTS       : .L391c .L39e8 .L3ac0 .L3c28 .L2fd4 gOvl_0200b014 .L302c
 *                     .L3134 .L3270 .L3330 .L34f8 .L3558 .L35b8 gOvl_0200b5f8
 *                     .L3694 .L3754 .L3784 .L388c
 *                     (already global -- NOT the set a split needs)
 *     -> converting a function here needs a TEXT/DATA SPLIT; the data must
 *        keep its own object.
 *     OvlFunc_965_200a8a0  reads no data label -> split needs NO new export
 *
 * So, concretely:
 *   - asm/overlays/rom_7ef4f4/ovl_30_c_c_c.s holds exactly ONE function
 *     (so no function-vs-function text split is needed) followed by a
 *     `.section .data` block of eighteen `.incbin` blobs from
 *     overlays/rom_7ef4f4/orig.bin.
 *   - The data must be cut into its OWN object, because a .o is built from one
 *     source file and a .c cannot emit those `.incbin` blobs.
 *   - NO label needs a new `.global`. All eighteen are ALREADY `.global`, and
 *     the function itself reads none of them, so the split adds no export.
 *     This is the easy form of the data split -- the expensive form is when the
 *     function reads a data label that is not yet exported, and that is not the
 *     case here.
 *   - The one asm-label extern the FUNCTION needs is `.L2a9c`, which is defined
 *     elsewhere in the overlay, contains a hex letter, and so cannot be
 *     captured by gcc's decimal label counter (see the capture note in
 *     PARK_OvlFunc_881_200b9fc.c -- the counter reached .L302 on a function of
 *     this size, so "four digits" is not itself the safety argument).
 *
 * WHY IT WAS DEPRIORITISED, on evidence rather than impression:
 *  - IT IS IN THE REGISTER-ROTATION TARGETING CLASS. The prologue is
 *    `push {r5, r6, lr} / mov r6, r8 / push {r6} / sub sp, #8` -- it saves a
 *    high register, which is elevation.md's documented tell for the
 *    allocno_compare class ("prefer targets without it until someone finds a
 *    handle on the processing order"). 78 parks already name REG_ALLOC_ORDER.
 *  - IT HOLDS A VALUE IN r12 ACROSS A LONG RANGE: `mov r12, r1` early, then
 *    `cmp r12, r2` and `cmp r12, r3` hundreds of instructions later. gcc-2.96
 *    Thumb does not normally allocate r12 (it is IP, a scratch), so this is
 *    either a reload artifact or a shape the allocator will not reproduce from
 *    source. Worth resolving BEFORE writing 498 instructions of C, because if
 *    it cannot be reproduced the function is blocked regardless of the reading.
 *  - IT PASSES ARGUMENTS ON THE STACK: `sub sp, #8` with `str r2, [sp]` and
 *    `str r0, [sp, #4]` immediately before calls, at four separate sites. So at
 *    least one callee takes FIVE OR SIX arguments, and the two stack slots are
 *    outgoing argument space, not spill slots -- which means the
 *    declaration-order spill-slot rule does NOT apply to them and must not be
 *    reached for. (The `expand_decl` corollary: a word-sized scalar is not an
 *    object expand_decl allocates, and outgoing argument space is not a
 *    declared local at all.)
 *
 * SUGGESTED ORDER OF WORK for the next attempt:
 *   1. Do the text/data split first and confirm `make compare` is still green
 *      BEFORE any .c exists. The split is byte-neutral by construction, so a
 *      red compare at that point is a layout mistake, and a layout mistake and
 *      a bad decompilation look identical at the end.
 *   2. Settle the r12 question on a SMALL probe before transcribing the body.
 *   3. Then read the body. The function opens with a state check against 0xb1
 *      at gState+0x1c0 and branches into a dispatch on the same value held in
 *      r12, so it is a state-machine step, not a linear cutscene -- expect the
 *      jump-table arm-body source-order rule to matter (arm bodies are emitted
 *      in SOURCE order; the ROM's order is readable off the reference).
 *
 * WARNING that cost this batch real time: tools/split_s.py has NO --dry-run.
 * It IGNORES the flag and performs the split for real -- deleting the original
 * .s and rewriting the linker script. Do not run it to "see what it would do".
 *
 * FINAL INSTALLED PATH (when done):
 *   src/overlays/rom_7ef4f4/ovl_30_c_c_c_a.c   (the text part; the .data part
 *   keeps the remaining .s object -- confirm the suffix letters against what
 *   split_s.py actually writes and against overlays/rom_7ef4f4/overlay.ld)
 *
 * Verify with (once a candidate exists):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7ef4f4/ovl_30_c_c_c_a.c \
 *     asm/overlays/rom_7ef4f4/ovl_30_c_c_c.s --func OvlFunc_965_200a8a0
 *
 * SHIMS: n/a -- no candidate.
 */
