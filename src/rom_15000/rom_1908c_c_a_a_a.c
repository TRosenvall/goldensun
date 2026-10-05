/* Func_801965c (0x0801965c) -- LANDED, 0 of 48.  Batch 326 brief A.
 *
 *   objcmp --func : OK -- 104 bytes, 48 encodings and 2 relocations identical.
 *   objcmp --whole: OK whole file -- 104 bytes, 48 encodings, 2 relocations.
 *   PIN-FREE, DEVICE-FREE, no flag group, no split.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1908c_c_a_a_a.c \
 *     asm/rom_15000/rom_1908c_c_a_a_a.s --func Func_801965c
 *
 * SPLIT SHAPE: `split_s.py --dry-run asm/rom_15000/rom_1908c_c_a_a_a.s
 * Func_801965c` reports "holds only Func_801965c and no data; convert it
 * directly, no split needed".  Zero exports.  shimcount 0.
 *
 * ================= HOW IT CLOSED: TWO EDITS BOTH IN THE PARK'S REJECTED LIST
 *
 * The park (src/non_matching/rom_15000/801965c.c, 10 of 48) listed under
 * "MEASURED AND WORSE":
 *
 *     an explicit byte-offset `j` carrier 40
 *     `i < n - 1` without the decrement 44 (loop.c's invariant hoist costs an insn)
 *
 * They are the two halves of one fix and were never crossed.  Individually each
 * is worse than the park's own 10; together they are 0.
 *
 * ----- half one: the byte-offset carrier (batch 325 brief B's finding)
 *
 * The ROM's peeled load is `ldrh r3,[r6,r2]`, Thumb's register-OFFSET form,
 * i.e. `(mem:HI (plus (reg blk) (reg off)))`.  A subscript spelling
 * (`((unsigned short *)blk)[0x758 + i]`) folds to one address pseudo in the
 * EXPANDER -- `.02.jump` is bit-identical across every subscript spelling --
 * so the register-offset form is never generated at all.  Naming the byte
 * offset as its own block-scoped carrier generates two
 * `(mem:HI (plus reg reg))` and reproduces the ROM's critical block, including
 * `add r2,r6,r2` appearing only in the PREHEADER after the guard branch.
 * That body reads 31, at 44 instructions against the reference's 45.
 *
 * ----- half two: `mov r12,r5` is a LOOP-INVARIANT HOIST, not an allocator roll
 *
 * The one missing instruction was the ROM's `mov r12,r5` (the park's "cause
 * (ii)", a loop bound parked in a hi register).  It is NOT a rotation and NOT
 * reachable by adding live values:
 *
 *   * `cbranchsi4` (config/arm/arm.md:5152) constrains operand 2 as `rI` in
 *     ALTERNATIVE 1 -- so the bound operand of a Thumb `cmp` is GENERAL_REGS,
 *     not LO_REGS, and a pseudo appearing ONLY in a reg-reg move and a `cmp`
 *     may legally live in a hi register.
 *   * `REG_ALLOC_ORDER` (config/arm/arm.h:989) is
 *     `3, 2, 1, 0, 12, 14, 4, 5, 6, 7, ...` -- **r12 is FIFTH, ahead of every
 *     callee-saved low register.**  So such a pseudo takes r12 as soon as
 *     r0-r3 are occupied, which inside this loop they are (i, src pointer and
 *     two temps).
 *   * With `n--`, the bound IS `n`: one pseudo (reg 34 in `.08.loop`), the
 *     destination of a `*thumb_addsi3` and live across the call to
 *     BufferString, hence LO_REGS.  There is no second pseudo to hoist and no
 *     spelling of the loop can invent one.
 *
 * `i < n - 1` with no `n--` makes `n - 1` a LOOP INVARIANT.  `loop.c`'s
 * `move_movables` (loop.c:1847, the `m->move_insn` arm) deletes the in-loop set
 * and re-emits it with `emit_move_insn (m->set_dest, m->set_src)` before
 * `loop_start` -- a preheader insn whose destination is its own pseudo, used
 * only by the loop test.  GENERAL_REGS, r12, `mov r12,r5`.  The park's own
 * parenthetical, "loop.c's invariant hoist COSTS AN INSN", was the answer: the
 * byte-offset body was exactly one insn short.
 *
 * > SO CAUSES (i) AND (ii) ARE ONE CAUSE, as batch 325 said -- but the shared
 * > cause is the loop's INVARIANT SET, not the number of simultaneously live
 * > low-register values.  The park's "NEXT" ("supply a ninth simultaneously-live
 * > low-register value so find_reg parks the bound in a hi register") is
 * > refuted: r12 was never contended for, it was never ASKED for, because the
 * > bound was not a separate pseudo.
 *
 * MEASURED, same two edits, on the landed body: `0xeb0` written plainly and
 * `i + i` for the doubling both still 0 (cosmetic).  A straight-line
 * block-scoped `{ unsigned int m = n - 1; ... }` before the loop reads **25**
 * -- the invariant must be INSIDE the loop's invariant set for move_movables to
 * see it, so a pre-loop computation is a different program.  `i != n - 1` and
 * `n - 1 > i` each 32 with dirty relocations; `int i`, `int n`, and a `while`
 * rewrite all exactly inert at 31 on the pre-landing body.
 *
 * ----- the two levers the park already had right, retained verbatim
 *
 * 1. `z`, an int-typed zero, for the `+0x12b2` store.  `*thumb_movhi_insn`
 *    (arm.md:4318) lists `=l <- mn` as alternative 1 and `=l <- I` as
 *    alternative 5, so a HImode const_int 0 goes to the literal POOL; its
 *    `pool_range` of 64 forces a mid-function pool dump.  An SImode zero
 *    truncated at the store emits `mov r3,#0`.
 * 2. `flag`, a NAMED pointer for that store, which gives the 0x12b2 constant
 *    r3 instead of r1 and keeps `mov r7,r1` out of position 2.
 */
extern unsigned char *iwram_3001e8c;
extern void BufferString(int a, int b);

int Func_801965c(int a, unsigned short *out, unsigned int n)
{
    unsigned char *blk;
    unsigned short *flag;
    unsigned int i;
    int z;

    blk = iwram_3001e8c;
    flag = (unsigned short *)(blk + 0x12b2);
    z = 0;
    *flag = z;
    BufferString(a, 1);
    for (i = 0; i < n - 1; i++) {
        unsigned int j;

        j = 0xeb0 + i * 2;
        if ((out[i] = *(unsigned short *)(blk + j)) == 0)
            break;
    }
    out[i] = 0;
    return i;
}
