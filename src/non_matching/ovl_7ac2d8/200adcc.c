/* OvlFunc_924_200adcc -- 0x0200adcc, asm/overlays/rom_7ac2d8/ovl_2dcc_a.s
 *
 * NON-MATCHING, 9 differing encodings of 26.  SIZE EXACT (60 = 60) and
 * INSTRUCTION COUNT EXACT (26 = 26, 22 real instructions plus 4 pool words),
 * so this IS a true distance.  Relocations identical.  ZERO pins, no devices,
 * no per-file flag group.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7ac2d8/200adcc.c asm/overlays/rom_7ac2d8/ovl_2dcc_a.s --func OvlFunc_924_200adcc
 *   XX ENCODINGS differ in 9 place(s) (ref 26, ours 26)
 *      first at index 8
 *
 * SPLIT SHAPE.  One function in the reference; tools/datacheck.py prints
 * nothing (no data section).  EXPORT LIST: EMPTY.  No split needed.
 *
 * ===================== BATCH 327: THE FIGURE WENT UP ON PURPOSE =====================
 *
 * THE PREVIOUS BODY READ 6 of 26 AND THAT 6 WAS NOT A DISTANCE.  It measured
 *   SIZE  ref 60 / ours 56     INSTRUCTION COUNT  ref 26 / ours 25
 * i.e. the stream was ONE POOL WORD SHORT, so a positional count against it
 * measured MISALIGNMENT.  This body is one edit away from that one -- `s` is
 * assigned BEFORE `i` instead of after -- and it restores the missing pool word,
 * making the comparison meaningful for the first time.  9 true > 6 false.
 *
 * NOTE WHAT THAT EDIT IS: THE EXACT REVERSE OF THIS PARK'S OWN STATED LEVER.
 * The old header read "Assigning the counter BEFORE the source pointer moves it
 * from 9 differing to 5."  It does -- and the 9 it moved away from was the
 * length-exact one.  Two batches were then spent attacking a literal pool that
 * was only missing because of that trade.  (Batch 326's "a lower figure can be a
 * worse starting point", found independently here.)
 *
 * ============ THE BLOCKER CLASS WAS WRONG: IT IS NOT CONSTANT DERIVATION ============
 *
 * The old header said gcc "notices that the save target and the source pointer
 * are 10 apart and reuses the register instead of taking a second pool entry",
 * i.e. the derivation decides the register.  IT IS THE OTHER WAY ROUND.
 *
 * Traced `-10` through every -da dump: ABSENT from .00.rtl through .17.lreg,
 * first present in .18.greg.  In .17.lreg the three addresses are three
 * independent *thumb_movsi_insn sets of three const_ints (insns 22, 25, 34).
 * No optimiser relates them.
 *
 * The pass is reload_cse_move2add (reload1.c:8840, called from :7991), which
 * runs AFTER reload.  Its own comment states the transform:
 *     (set (REGX) (CONST_INT A)) ... (set (REGX) (CONST_INT B))
 *  -> (set (REGX) (CONST_INT A)) ... (set (REGX) (plus (REGX) (CONST_INT B-A)))
 * Gates at reload1.c:8872-8905:
 *   1. reg_set_luid[regno] > last_label_luid  -- no CODE_LABEL may intervene
 *   2. the two modes are the same size (or a NOOP truncation)
 *   3. reg_offset[regno] is a CONST_INT
 *   4. GET_CODE (src) == CONST_INT && reg_base_reg[regno] < 0
 *   5. rtx_cost (new_src, PLUS) < rtx_cost (src, SET) && have_add2_insn
 *
 * BECAUSE IT RUNS AFTER RELOAD, THE HARD REGISTERS ARE ALREADY FIXED WHEN IT
 * FIRES.  It could only fire because reload had put the save pointer and `s` in
 * the SAME hard register.  In the ROM they are in DIFFERENT registers (save r3,
 * s r2) and r2's previous value there is a `ldrh` from memory, so gate 3 fails
 * and no derivation is possible.  THERE IS ONE DEFECT HERE, NOT TWO, AND IT IS
 * AN ALLOCATION DEFECT.  Do not spend another round on the pool; three
 * independent extern symbols (batch 271) were treating a symptom.
 *
 * ========================= WHAT THE 9 ARE, AND THE CAUSE OF EACH =========================
 *
 *   ROM                        this body
 *   ldr r1, =0x50000c2   d     ldr r1, =0x50000c2    d    r1   SAME
 *   ldr r3, =0x50000ce   save  ldr r2, =0x50000ce    save r2   <- RUN B
 *   ldrh r2, [r1]        tmp   ldrh r3, [r1]         tmp  r3   <- RUN B
 *   strh r2, [r3]              ldr r0, =0x50000c4    s    r0   <- RUN A
 *   ldr r2, =0x50000c4   s     strh r3, [r2]
 *   mov r0, #0           i     mov r2, #0            i    r2   <- RUN A
 *   (the loop body and the epilogue are byte-exact in both)
 *
 * RUN A, four encodings -- `s` and `i` swap r2 and r0, a GLOBAL-alloc defect.
 * .18.greg's allocno order line is the whole story:
 *     old body prints  ;; 3 regs to allocate: 32 33 34
 *     this body prints ;; 3 regs to allocate: 32 34 33
 * Moving `s`'s assignment earlier lengthens its live_length, the DENOMINATOR of
 * allocno_compare, so `i` overtakes it.  The second-ranked allocno takes r2 and
 * the third takes r0.  The ROM needs `s` ranked above `i`.
 * MEASURED INERT: ALL SIX declaration orders of d/s/i, all 9 of 26, so
 * allocno_compare's last rung (global.c:617, `return v1 - v2`) is DEAD here --
 * the arithmetic above it separates the two.  That is a TIE-ABSENT row, not a
 * lever-failed row.
 *
 * RUN B, five encodings -- the save pointer and the HI temp swap r2 and r3.
 * Present in the OLD body too; it is the real and only defect there.  Both
 * pseudos are BLOCK-LOCAL, so local-alloc decides.  REG_ALLOC_ORDER
 * (config/arm/arm.h:989) begins 3,2,1,0, so find_free_reg gives r3 to whichever
 * qty is allocated FIRST, and the order is
 *     QTY_CMP_PRI = floor_log2(n_refs) * n_refs * size / (death - birth)
 * Both have n_refs 2 and size 1, so ONLY THE SPAN MATTERS:
 *     save: span 2 -> 1*2/2 = 10000
 *     tmp : span 1 -> 1*2/1 = 20000      <- always wins r3
 * and expand_assignment is LHS-FIRST (expr.c:3402), so a direct
 * `*(vu16 *)0x50000ce = *d;` ALWAYS materialises the save address before the
 * load and always gives save the longer span.  THE ROM NEEDS THE REVERSE.
 *
 * THE ONE REMAINING CAUSE, STATED EXACTLY:  run B needs the copied halfword to
 * sit in a SINGLE HImode pseudo with n_refs 2 across the save-address insn.
 * Naming the halfword does invert the RTL order -- verified in .17.lreg of the
 * `unsigned int t` variant: d, (set (reg:HI 40) (mem/v:HI d)),
 * (set (reg:SI 35) (subreg:SI (reg:HI 40) 0)), save, store -- but the surviving
 * subreg copy gives the combined qty n_refs 4, and floor_log2(4) = 2 makes it
 * 2*4/3 = 26666, so the temp wins again.  The copy survives because reg 40 is a
 * VOLATILE mem load, which combine will not propagate through; and it cannot be
 * avoided by narrowing the temp, because PROMOTE_MODE promotes a short local and
 * then a real `lsl #16 / asr #16` pair appears.  No C spelling was found that
 * produces the needed shape.
 *
 * MEASURED INERT, batch 327 (all identical to the OLD 6-of-26-misaligned body):
 *   `int t`, `unsigned int t`, `unsigned long t` named halfword temps;
 *   a named save pointer alone; a named save pointer crossed with `unsigned int t`.
 * MEASURED WORSE, 27 instructions against 26 (the PROMOTE_MODE pair):
 *   `unsigned short t`, `short t`, `unsigned short t` with an explicit cast,
 *   and `unsigned short t` declared before the pointers.
 * MEASURED INERT at 9 of 26 on THIS body: all six declaration orders (above).
 *
 * THE TWIN TAKES THIS EDIT UNCHANGED.  src/non_matching/ovl_7ac2d8/200a648.c is
 * the same routine at 0x5000050/0x500005e/0x5000052 with a bound of 6, and the
 * same single edit moves it to 9 of 26 with ref 26 = ours 26 and the same first
 * differing index.  One mechanism, two parks.
 */
extern int iwram_3001e40;

void OvlFunc_924_200adcc(void)
{
    volatile unsigned short *d;
    volatile unsigned short *s;
    unsigned int i;

    if ((iwram_3001e40 & 7) == 0) {
        d = (volatile unsigned short *)0x50000c2;
        *(volatile unsigned short *)0x50000ce = *d;
        s = (volatile unsigned short *)0x50000c4;
        i = 0;
        do {
            *d = *s;
            i++;
            s++;
            d++;
        } while (i <= 5);
    }
}
