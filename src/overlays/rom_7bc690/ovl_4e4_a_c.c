/* OvlFunc_933_2008cd0  --  0x02008cd0
 *
 * The whole of goldensun/asm/overlays/rom_7bc690/ovl_4e4_a_c.s: one function,
 * no data.
 *
 * A three-tier progress gate. A percentage is computed once, then each tier's
 * save bit is cleared if the percentage has fallen back below its threshold and
 * set if it has just risen above it, with the tier number written to iwram on
 * each promotion.
 *
 * BUILT WITH CSE_CFLAGS: without it the file is 36 differing. The flag id
 * repeats are the UNREACHABLE half of the duplicate-constant rule -- ids in
 * mutually exclusive arms consumed as r0 by a `bl`, which no source spelling
 * separates. Worth 77 aligned to 61 on its own.
 *
 * BLOCKER 1b REACHES DOWN TO THE VALUE 1, which is the finding. `*(short *)(p +
 * K) = 1;` narrows to a HImode const_int, and although thumb.md's *movhi_insn
 * has an `I` alternative that would give `mov r3, #1`, the value arrives via
 * force_reg from the movhi expander and comes out of the LITERAL POOL instead:
 * `ldrh r3, .L10` with `.word 1`, where the ROM has `mov r3, #1`. A pool word
 * holding 1 looks absurd enough to be misread as a scheduling artefact. It is
 * not; it is 1b at a tiny constant.
 *
 * THE ESCAPE IS THREE SEPARATE LOCALS ASSIGNED IN THE DOMINATING BLOCK, and it
 * sharpens both of the limits recorded for the batch-182 split lever:
 *
 *   inline constants                  -> pool ldrh, wrong          (12 aligned)
 *   ONE shared local, assigned in-arm -> `mov r3, #1`, but emitted
 *                                        BEFORE the `add`          (12 aligned)
 *   THREE locals in the dominating block -> `add r2, r6, rN` then
 *                                        `mov r3, #imm`, the ROM   (exact)
 *
 * Limit 1 said that when both uses already sit in a different basic block from
 * the assignment, splitting is byte-identical. That holds only when the values
 * are the SAME. Here they differ, gcc rematerialises each constant at its own
 * store, and the three arms genuinely need three names. Limit 2 -- two
 * variables holding one constant fold back to a single const_int -- is
 * side-stepped for the same reason. Both `int` and `short` locals work, so it
 * is the dominating block doing the work and not the type.
 *
 * One symptom worth not chasing: at 123 instructions the early return degraded
 * from the ROM's `bne <epilogue>` to `beq .LCB31 / b .L2 @long jump`, purely
 * because the three spurious pool words had pushed the epilogue out of
 * conditional-branch range. THE LONG JUMP WAS A SYMPTOM OF 1b, not an
 * independent branch-polarity problem, and it closed when the pool words did.
 *
 * The remaining `bl __divsi3` vs `bl _divsi3_RAM` line is the linker alias
 * already carried at overlays/rom_7bc690/overlay.ld:70.
 */
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);

void OvlFunc_933_2008cd0(void)
{
    char *p;
    unsigned char *g;
    int pct;
    int one = 1;
    int two = 2;
    int three = 3;

    p = iwram_3001ebc;
    g = gState;
    pct = *(short *)(g + 0x232) * 100 / *(short *)(g + (0x8b << 2));
    if (__GetFlag(0x201))
        return;
    if (__GetFlag(0x302) && pct <= 0x4a) {
        __ClearFlag(0x302);
        __ClearFlag(0x303);
        __ClearFlag(0xc1 << 2);
        __ClearFlag(0x305);
    }
    if (__GetFlag(0x301) && pct <= 0x31) {
        __ClearFlag(0x301);
        __ClearFlag(0x303);
        __ClearFlag(0xc1 << 2);
        __ClearFlag(0x305);
    }
    if (__GetFlag(0xc0 << 2) && pct <= 0x18) {
        __ClearFlag(0xc0 << 2);
        __ClearFlag(0x303);
        __ClearFlag(0xc1 << 2);
        __ClearFlag(0x305);
    }
    if (!__GetFlag(0xc0 << 2) && pct > 0x18) {
        __SetFlag(0xc0 << 2);
        *(short *)(p + (0xc1 << 1)) = one;
    }
    if (!__GetFlag(0x301) && pct > 0x31) {
        __SetFlag(0x301);
        *(short *)(p + (0xc1 << 1)) = two;
    }
    if (!__GetFlag(0x302) && pct > 0x4a) {
        __SetFlag(0x302);
        *(short *)(p + (0xc1 << 1)) = three;
    }
}
