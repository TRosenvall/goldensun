/* Func_80c0eec -- asm/rom_b5000/rom_bffb8_c_c_a_a.s
 *
 * MATCHING, 0 of 79 encodings.  172 bytes against 172, 79 encodings against
 * 79, 5 relocations identical.  Device-free (shimcount 0), pin-free,
 * anchor-free, no per-file flag.  Landed batch 345 brief A.
 *
 * Verify with (candidate, before install):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b345/A/p4_candidate.c \
 *     asm/rom_b5000/rom_bffb8_c_c_a_a.s --func Func_80c0eec
 *
 * Verify with (INSTALLED path, after the split below):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_b5000/rom_bffb8_c_c_a_a_b.c \
 *     asm/rom_b5000/rom_bffb8_c_c_a_a_b.s --func Func_80c0eec
 *
 * SPLIT.  The piece asm/rom_b5000/rom_bffb8_c_c_a_a.s holds THREE functions
 * and all three were parked (tools/piece.py, re-run this batch):
 *     Func_80c0eec  66 of 79   src/non_matching/rom_b5000/80c0eec.c  <- this one
 *     Func_80c0f98  21 of 61   src/non_matching/rom_b5000/80c0f98.c
 *     Func_80c1014  23 of 30   src/non_matching/rom_b5000/80c1014.c
 * This function is FIRST in the reference's order, so split_s.py --dry-run
 * writes only two parts and there is no _a:
 *     python3 tools/split_s.py asm/rom_b5000/rom_bffb8_c_c_a_a.s Func_80c0eec
 *     -> rom_bffb8_c_c_a_a_b.s  (1 function, 86 lines)   <- replaced by this .c
 *        rom_bffb8_c_c_a_a_c.s  (2 functions, 109 lines) <- the two other parks
 *        stage1.ld rewritten
 * tools/datacheck.py on the piece reports NO DATA.  No exports needed.
 *
 * ========================= WHAT THE PARK HAD WRONG =========================
 *
 * The park's verdict was "BLOCKER: LOOP ROTATION / basic-block placement" and
 * it closed with "NOT TRIED: nothing further."  Loop rotation is real and is
 * HALF of the gap -- but it is reachable from C in one token, and it was
 * masking a second, larger effect the park never saw because that effect
 * costs NO INSTRUCTION.
 *
 * EFFECT 1, the one nothing had found: THE FOUR gKeyRepeat READS WERE
 * COMMONED INTO ONE PSEUDO.  From the parked body's own .15.regmove:
 *
 *     (insn 34 ... (set (reg:SI 99) (mem/f:SI (reg:SI 39) 4)))   ; ONE load
 *     (insn 36 ... (set (reg:SI 42) (const_int 32)))
 *     (insn 38 ... (set (reg:SI 42)
 *                       (and:SI (reg:SI 99) (reg:SI 42))))
 *     ;; End of basic block 2, registers live: ... 32 39 99        <- 99 LIVE OUT
 *
 * reg 99 is the gKeyRepeat value and guards 2 and 3 reuse it, so it is live
 * past each `and`.  Reload then REMATERIALISES the load at every use, which is
 * why the emitted instruction COUNT already equalled the ROM's four loads and
 * why every instruction-count reading of this park came back clean.  What the
 * shared pseudo costs is the two-address tie: `*thumb_andsi3_insn` is
 * destructive, and because reg 99 is live past the `and`, regmove cannot tie
 * the output to it and ties the output to the MASK pseudo instead:
 *
 *     rom    ldr r3, [r5, #0] / movs r2, #MASK / ands r3, r2   dest = the LOAD
 *     ours   ldr r2, [r5, #0] / movs r3, #MASK / ands r3, r2   dest = the MASK
 *
 * The `ands` and the `cmp` are byte-identical; only the two feeding
 * instructions' destinations swap.  Guard 4 is a counter-example in the parked
 * body's own output once ANY loop note sits before it -- it gets its own
 * pseudo (reg 78), which DIES at insn 140, and regmove ties the output to the
 * load.  That is the proof that the tie follows liveness and not spelling.
 *
 * Cure: `extern volatile int gKeyRepeat;`.  NOT a device and not a guess -- it
 * is how this symbol is already declared in dozens of LANDED files in this
 * tree (src/rom_a1000/rom_a7380_c_a_a_b.c,
 * src/rom_b0000/rom_b0070_c_c_a_c_c_a_a.c,
 * src/overlays/rom_7fcd20/ovl_30_c_c_a_c_a_c_c.c, among others).  A volatile
 * read is not commoned, each guard gets a pseudo that dies at its own `and`,
 * and all four ties go the ROM's way.  gKeyPress and gKeyHeld volatile are
 * both EXACTLY INERT here, because only gKeyRepeat is read more than once.
 *
 * EFFECT 2: `while (1)` IS NOT `for (;;)` TO gcc-2.96.  With the volatile in
 * and the loop still `for (;;)` the aligned residue is 15 and is exactly the
 * park's own block-placement reading -- we emit an entry `b` over the latch
 * and lay the WaitFrames continuation AHEAD of the body where the ROM falls
 * straight into the body and puts the latch at the bottom.  Measured, with the
 * volatile in:
 *       while (1) { ... }       0 of 79, byte-identical
 *       for (;;) { ... }       66 of 79, aligned 64/15
 *       do { ... } while (1);  66 of 79, aligned 64/15  (same as for(;;))
 * The park measured three loop shapes and all three were `for`/`while(cond)`
 * forms; it never tried the plain `while (1)`.
 *
 * TWO MATCHING BODIES EXIST.  `for (;;)` plus the volatile plus a
 * `do { } while (0);` anchor after `*(int *)(base + 0x828) -= 0x64;` also
 * reaches 0.  THIS one ships: anchor-free, so it carries no pass-3 debt.
 *
 * MEASURED INERT (against the parked body unless stated):
 *   - `if (0x20 & gKeyRepeat)`, `if ((gKeyRepeat & 0x20) != 0)`, the mask in an
 *     int local: all BIT-IDENTICAL to the parked body.  C-level operand order
 *     does not reach the `and`'s RTL operand order.
 *   - the loaded value named in an int local per guard: 56 of 79, aligned 56/23.
 *   - all 31 subsets of the 5 in-loop anchor boundaries: best 8 of 79, aligned
 *     71/8.  Anchors fencing before guards 2 or 3 COST TWO INSTRUCTIONS
 *     (ni 79 -> 81, dsize +4) -- a barrier in a loop body is not free.
 *   - all eight per-file flag groups on the parked body: inert or worse.
 *   - two goto loop spellings with the volatile: 36 of 79 at ni 77, dsize -4
 *     (TWO instructions short).
 */
extern int gKeyHeld;
extern volatile int gKeyRepeat;
extern int gKeyPress;
extern int iwram_3001e74;
extern void WaitFrames(int n);

int Func_80c0eec(void)
{
    char *base;
    int r;

    if (gKeyHeld & 8) {
        while (1) {
            base = (char *)iwram_3001e74;
            if (gKeyRepeat & 0x20)
                *(int *)(base + 0x828) -= 1;
            if (gKeyRepeat & 0x10)
                *(int *)(base + 0x828) += 1;
            if (gKeyRepeat & 0x40)
                *(int *)(base + 0x828) -= 0x64;
            if (gKeyRepeat & 0x80)
                *(int *)(base + 0x828) += 0x64;
            if (gKeyPress & 1) {
                r = *(int *)(base + 0x828);
                break;
            }
            WaitFrames(1);
        }
    }
    if (gKeyHeld & 4)
        r = 0x18f;
    return r;
}
