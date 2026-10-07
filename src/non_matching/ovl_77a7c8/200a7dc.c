/* OvlFunc_881_200a7dc -- NON-MATCHING.
 *
 * NON-MATCHING, 12 of 31 encodings  (MEASURED, batch 334).
 *   INSTRUCTION COUNTS NOW AGREE -- ref 30, ours 30, stream 31/31. The
 *   previous figure, thirty of thirty-one, was POSITIONAL on a stream that was
 *   two instructions short; this one is a distance.
 *   RELOCATIONS: the same symbol at the same offset now. The shifted-offset
 *   relocation the old header recorded was a consequence of the length gap and
 *   is gone with it.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77a7c8/200a7dc.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_c_a.s --func OvlFunc_881_200a7dc
 *
 * Pin-free. The piece holds this function alone.
 *
 * A scan over 12-byte records for one whose first word is 2 and whose halfword
 * at +4 is 0x8a, stopping at a first word of -1.
 *
 * ============ BATCH 334: BOTH MISSING INSTRUCTIONS, AND THE PASS ============
 *
 * The old header named ONE of the two misses and got the pass wrong. Both are
 * now located, and the statement order of this body already reproduces the
 * ROM's setup order exactly (base, 0x21, 1, w=base, 1, p=base+4, off=0, the
 * high-register move, neg):
 *
 *   (A) `mov r7, r0` -- the `w = base` copy, which the old header described
 *       correctly as the DUPLICATED BASE POINTER.
 *   (B) `ldr r3, [r2, r0]` RELOADED at the sentinel test. THE OLD HEADER NEVER
 *       MENTIONED THIS ONE. Without it we hold the first read's value in a
 *       register and compare that.
 *
 * NOT the allocator and NOT cse: BOTH die in PASS 07, gcse. In the `-da` dumps
 * the file's `.07.gcse` already shows the sentinel load as a register copy, not
 * a MEM, and the store already addressing the base's own pseudo -- so cprop
 * (gcse.c:3902) had already run on `w`. The mem count only falls between
 * `.07.gcse` and `.08.loop` because gcse's expression-hash-table dump
 * disappears; that is an artifact, not the pass.
 *
 * THE ONE LEVER THAT BUYS THE PAIR: NAME THE GLOBAL INSIDE THE LOOP.
 * `gOvl_0200e3f4 + off` at the sentinel, instead of the local `base + off`.
 * The in-loop symbol_ref is a different pseudo while gcse runs, so (B) the two
 * reads are never one expression for expr_equiv_p; then loop.c, which is PASS
 * 08 and therefore AFTER gcse, hoists the address into its own loop-invariant
 * pseudo, cse2 at 09 rewrites that to a `mov` copy, and nothing after cse2
 * propagates copies -- so (A) survives too. One spelling, both instructions,
 * and the relocation offset falls into place with them.
 *
 * The second half is the pointer-as-integer spelling at the other two
 * accesses, `off + (int)base`: it fixes the `ldr`/`str` OPERAND ORDER, since
 * the ROM has the index first at all three accesses and ordinary pointer
 * arithmetic puts the pointer first. 13 to 12.
 *
 * ============ WHAT THE REMAINING 12 ARE: ONE ALLOCATOR TIE-BREAK ============
 *
 *              neg1  ldrsh-zero  one   k     base-copy
 *     rom      r4    r5          r6    ip    r7
 *     ours     r5    r6          ip    r7    r4
 *
 * The ROM parks the CONSTANT k in the high register and we park `one` there,
 * and the ROM's copy serves the STORE where ours serves the SENTINEL. The
 * ROM's choice costs exactly one instruction MORE than ours, because Thumb
 * cannot move an immediate into a high register: a constant into ip is
 * `mov r3,#k` then `mov ip,r3`, where a register's value is one `mov`. That is
 * why the MIRROR shape -- in-loop symbol at the store, alias-split sentinel --
 * reads ref 30 against ours 29: it puts both loads through the pool-loaded
 * register exactly as the ROM does, and the copy then lands in ip so the
 * constant never pays the two-instruction entry.
 *
 * MEASURED INERT OR WORSE (batch 334):
 *   * `w = gOvl_0200e3f4;` -- a second symbol reference in the SETUP block --
 *     is BYTE-IDENTICAL to `w = base`. cse at 03 commons the two pool loads
 *     inside the one entry block and gcse cprops the copy. Naming the symbol
 *     twice only pays INSIDE THE LOOP.
 *   * `off + base` / `off + w` as POINTER arithmetic: byte-identical. The C
 *     front end rebuilds it pointer-first; only the integer cast flips the rtl.
 *   * a declaration-order sweep, eight orders including `off` first: all 13.
 *     So the rtl plus order is not pseudo-number canonicalisation.
 *   * `unsigned int` for the sentinel read: byte-identical, same alias set.
 *   * `long` for the sentinel read DOES split the alias set -- the dumps show
 *     the two MEMs carrying different set numbers, and gcse.c:1546-1551 refuses
 *     to merge two MEMs in different alias sets -- and DOES keep the second
 *     load. Alone it reads ours 27; crossed with the in-loop symbol at the
 *     store, ours 29.
 *   * `volatile int` on both reads reads ours 31: it buys the load and the copy
 *     but loop.c then strength-reduces the store address into a third induction
 *     variable, so it overshoots. A figure about the blocker, not a body.
 *   * two distinct struct types for the two reads: byte-identical. A
 *     COMPONENT_REF's alias set here is the field type's, so it does not split.
 *   * `k` written as the literal 0x21: ours 29.
 */
extern char gOvl_0200e3f4[];

void OvlFunc_881_200a7dc(void)
{
    char *base;
    char *w;
    char *p;
    int off;
    int k, one, neg1;

    base = gOvl_0200e3f4;
    k = 0x21;
    neg1 = 1;
    w = base;
    one = 1;
    p = base + 4;
    off = 0;
    neg1 = -neg1;
    for (;;) {
        if (*(int *)(off + (int)base) == 2 && *(short *)p == 0x8a) {
            *(int *)(off + (int)w) = one;
            *(int *)(p + 4) = k;
            return;
        }
        if (*(int *)(gOvl_0200e3f4 + off) == neg1)
            return;
        p += 0xc;
        off += 0xc;
    }
}
