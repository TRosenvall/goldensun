/* OvlFunc_924_200a648 -- 0x0200a648, asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_a_a.s
 *
 * NON-MATCHING, 9 differing encodings of 26.  SIZE EXACT (60 = 60) and
 * INSTRUCTION COUNT EXACT (26 = 26, 22 real instructions plus 4 pool words),
 * so this IS a true distance.  Relocations identical.  ZERO pins, no devices,
 * no per-file flag group.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7ac2d8/200a648.c asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_a_a.s --func OvlFunc_924_200a648
 *   XX ENCODINGS differ in 9 place(s) (ref 26, ours 26)
 *      first at index 8
 *
 * SPLIT SHAPE.  One function in the reference; tools/datacheck.py prints
 * nothing (no data section).  EXPORT LIST: EMPTY.  No split needed.
 * (The old header's FIRST LINE named `ovl_22c4_c_c_c_a.s`, which is a different
 * piece and does not contain this function.  The recipe's `_a_a` was right, and
 * tools/upstream_module.py confirms it.  Fixed.)
 *
 * READ src/non_matching/ovl_7ac2d8/200adcc.c -- it is this routine's TWIN at
 * 0x50000c2/0x50000ce/0x50000c4 with a bound of 5, and it carries the full
 * mechanism.  THE TWO ARE NOW PROVEN TO SHARE ONE MECHANISM COMPLETELY: the
 * single edit below gives both of them 9 differing of 26 with ref 26 = ours 26
 * and the same first differing index (8).
 *
 * ===================== BATCH 327: THE FIGURE WENT UP ON PURPOSE =====================
 *
 * THE PREVIOUS BODY READ 6 of 26 AND THAT 6 WAS NOT A DISTANCE:
 *   SIZE  ref 60 / ours 56     INSTRUCTION COUNT  ref 26 / ours 25
 * The stream was ONE POOL WORD SHORT, so a positional count against it measured
 * MISALIGNMENT.  The one edit here -- `s` assigned BEFORE `i` rather than after
 * -- restores the pool word.  9 true beats 6 false.  This is the exact REVERSE
 * of the lever the twin's park recommended, and that trade is why batches 204 and
 * 271 were both spent on the literal pool.
 *
 * ============ THE BLOCKER CLASS WAS WRONG: IT IS NOT CONSTANT DERIVATION ============
 *
 * The `sub r2, #0xc` is absent from .00.rtl through .17.lreg and first appears in
 * .18.greg.  It is reload_cse_move2add (reload1.c:8840, called from :7991), a
 * POST-RELOAD pass, so the hard registers are already fixed when it fires.  It
 * can only fire because reload put the save pointer and `s` in the SAME hard
 * register; in the ROM they are in different registers and gate 3
 * (`reg_offset[regno]` must be a CONST_INT) fails, because r2's previous value
 * there came from a `ldrh`.  The derivation is a CONSEQUENCE of the register
 * choice, not its cause.  Full gate list in the twin's header.
 *
 * SO BATCH 271's `_PLTT_50`/`_PLTT_52`/`_PLTT_5E` PROPOSAL IS WITHDRAWN AS A
 * DIAGNOSIS, not merely as a landing.  It did remove the derivation, but by
 * removing gcc's ability to relate two constants rather than by fixing the
 * register, and it left the transposition untouched -- which is exactly what a
 * symptom treatment looks like.  It would also have needed three wram.sym
 * absolutes to buy a NON-match, which the owner-decisions standard (structural
 * impossibility, and completion) refuses anyway.
 *
 * ========================= WHAT THE 9 ARE =========================
 *
 *   ROM                        this body
 *   ldr r1, =0x5000050   d     ldr r1, =0x5000050    d    r1   SAME
 *   ldr r3, =0x500005e   save  ldr r2, =0x500005e    save r2   <- RUN B
 *   ldrh r2, [r1]        tmp   ldrh r3, [r1]         tmp  r3   <- RUN B
 *   strh r2, [r3]              ldr r0, =0x5000052    s    r0   <- RUN A
 *   ldr r2, =0x5000052   s     strh r3, [r2]
 *   mov r0, #0           i     mov r2, #0            i    r2   <- RUN A
 *   (loop body and epilogue byte-exact in both)
 *
 * RUN A (4 encodings): global-alloc ranks `i` above `s` because assigning `s`
 * earlier lengthens its live_length, the denominator of allocno_compare.
 * .18.greg's order line goes from `32 33 34` to `32 34 33`.  Second-ranked takes
 * r2, third takes r0.  ALL SIX declaration orders of d/s/i are INERT at 9 of 26,
 * so global.c:617's tie-break rung is dead here -- a TIE-ABSENT row.
 *
 * RUN B (5 encodings): local-alloc gives r3 to the HI temp instead of the save
 * pointer.  REG_ALLOC_ORDER (arm.h:989) starts 3,2,1,0, so r3 goes to whichever
 * block-local qty is allocated FIRST, ordered by
 *     QTY_CMP_PRI = floor_log2(n_refs) * n_refs * size / (death - birth)
 * Both have n_refs 2 and size 1, so only the span counts: save span 2 -> 10000,
 * temp span 1 -> 20000.  expand_assignment is LHS-FIRST (expr.c:3402), so a
 * direct `*(vu16 *)0x500005e = *d;` always materialises the save address before
 * the load and always gives the save pointer the longer span.
 *
 * THE ONE REMAINING CAUSE: run B needs the halfword in a SINGLE HImode pseudo
 * with n_refs 2 across the save-address insn.  Naming it inverts the RTL order
 * but leaves a subreg copy (n_refs 4 -> 26666, still beats 20000) because a
 * volatile mem load will not propagate through combine; narrowing the temp
 * instead trips PROMOTE_MODE and emits a real lsl/asr pair.  No C spelling found.
 *
 * MEASURED on the TWIN this batch and applying here unchanged: `int t`,
 * `unsigned int t`, `unsigned long t`, a named save pointer, and the named save
 * pointer crossed with `unsigned int t` are all INERT; `unsigned short t`,
 * `short t`, the cast form and the declared-first form are all WORSE at 27
 * instructions against 26.
 */
extern int iwram_3001e40;

void OvlFunc_924_200a648(void)
{
    volatile unsigned short *d;
    volatile unsigned short *s;
    unsigned int i;

    if ((iwram_3001e40 & 7) == 0) {
        d = (volatile unsigned short *)0x5000050;
        *(volatile unsigned short *)0x500005e = *d;
        s = (volatile unsigned short *)0x5000052;
        i = 0;
        do {
            *d = *s;
            i++;
            s++;
            d++;
        } while (i <= 6);
    }
}
