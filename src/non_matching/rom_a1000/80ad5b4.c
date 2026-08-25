/* Func_80ad5b4  --  0x080ad5b4, asm/rom_a1000/rom_ad274_c_a_a.s
 *
 * BLOCKER CLASS: literal pool ORDERING.  A new one -- this is the first
 * function in the corpus to need both a pooled SYMBOL and a pooled INTEGER
 * CONSTANT, and they come out in the opposite order from the ROM.
 *
 * EVERY INSTRUCTION MATCHES.  tools/tryc.py reports `OK Func_80ad5b4
 * (29 lines)`.  The build still fails `make compare`, on ten bytes:
 *
 *     0x0ad5b8  ldr r3, [pc, #N]   imm 12 (ours) vs 13 (rom)
 *     0x0ad5de  ldr r3, [pc, #N]   imm  4 (ours) vs  3 (rom)
 *     0x0ad5ec  four words of pool, the two entries transposed
 *
 * The ROM's pool is  [0xffff8000][iwram_3001f2c].  gcc-2.96 emits
 * [iwram_3001f2c][0xffff8000] -- reference order, which is what
 * add_minipool_forward_ref produces: entries are kept sorted by max_address,
 * and the earlier-referenced fix is the more constrained one, so it lands
 * first.  See config/arm/arm.c:4820 in the build image.
 *
 * WHY THIS HAD NOT SHOWN UP BEFORE.  A sweep of every gcc-generated .s in the
 * tree finds ZERO literal pools that mix a symbol with an integer constant.
 * Every matched TU's pool is all-symbols or all-constants, where ordering
 * cannot be observed.  This is the first mixed pool, and it disagrees.
 *
 * THE SCREEN CANNOT SEE THIS, and says so: tryc.py normalises pool loads to
 * `=value`, so a pool at a different distance still compares equal.  It printed
 *
 *     !! the reference keeps its literal pool INSIDE the function
 *        VERIFY WITH make compare -- this screen cannot see PC-relative offsets
 *
 * and that warning was correct.  Treat an `OK` carrying it as provisional.
 *
 * WHAT WAS TRIED
 *   - Both pool constants are genuinely needed: 0x03001f2c and 0xffff8000 each
 *     cost three Thumb instructions to synthesise, so gcc pools both and so did
 *     the ROM.  Neither is a pool tell.
 *   - Reordering the C to reference 0xffff8000 first would move the `ldr` with
 *     it, and the instruction order is the part that already matches exactly.
 *   - Nothing in the source controls pool entry order; it is decided in
 *     machine_dependent_reorg after the insn stream is final.
 *
 * The most economical explanation is the same one recorded for REG_ALLOC_ORDER:
 * the original toolchain differed.  Both of the reconstructed pools in this
 * file put constants before symbols (Func_80ad508 at .Lad5ec has 0x10 and 0xc8
 * ahead of its four symbol entries), which is consistent, but two pools in one
 * file is not enough to call it a rule.
 *
 * THE C BELOW IS INSTRUCTION-EXACT and worth keeping for whenever the pool
 * question is settled.  Three things were needed for it:
 *
 *   1. `off` is ONE variable, reused -- the ROM builds 0x224 and bumps it with
 *      `add r6, #0x10`.  `i` is likewise shifted and advanced in place.
 *   2. The OR needs the constant as its destination.  `v = 0xffff8000 | b;`
 *      compiles to `orr r2, r3`; splitting it into `v = 0xffff8000; v |= b;`
 *      gives the ROM's `orr r3, r2` -- and only then does the preceding
 *      `mov r3, r2` for `v = b` survive instead of being coalesced away.
 *   3. The return type is int with no return statement.  Declared void, the
 *      epilogue is `pop {r0} / bx r0`; gcc will not use r0 as the epilogue
 *      scratch when the return type is non-void, because r0 is live at the
 *      return even with nothing assigned to it.
 */

extern char *iwram_3001f2c;

int Func_80ad5b4(int i, int a, int b, int flag)
{
    char *base;
    int off;
    int v;

    base = iwram_3001f2c;
    off = 0x224;
    if (*(int *)(base + (i * 4 + off)) != 0) {
        i *= 2;
        off += 0x10;
        *(short *)(base + (i + off)) = a;
        i += 0x23c;
        v = b;
        if (flag != 0) {
            v = 0xffff8000;
            v |= b;
        }
        *(short *)(base + i) = v;
    }
}
