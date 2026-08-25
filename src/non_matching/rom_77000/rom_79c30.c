/* Func_8079c30 -- NOT MATCHING. 4 of 19 lines, same length.
 *
 * Source asm: goldensun/asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_a.s
 *
 * TWIN: Func_8079c5c in the same .s chain
 * (asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_a.s) has the identical tail --
 * call, two multiplies, bias-and-shift -- differing only in how its first
 * argument is derived. It is the same defect and this park covers both. Noted
 * in batch 56 while screening candidates; not screened separately.
 *
 * Blocker class: MULTIPLY OPERAND CANONICALISATION.
 *
 * A fixed-point scale: call, two multiplies, then a signed divide by 0x10000
 * which gcc renders as the ROM's bias-and-shift (`if (x < 0) x += 0xffff;
 * x >>= 16`). The tail matches exactly. Both multiplies have their operands the
 * wrong way round:
 *
 *     rom    mov r3, r6 / mul r3, r0      (a is the destination)
 *     ours   mov r3, r0 / mul r3, r6      (the call result is)
 *
 * Thumb `mul` is destructive -- `mul rd, rm` is rd = rd * rm -- so the operand
 * gcc puts FIRST becomes the destination, and it does not take that from the
 * source. Multiplication is commutative and gcc-2.96 canonicalises the operand
 * order before register allocation.
 *
 * FOUR SPELLINGS, ALL 4 of 19 EXCEPT WHERE NOTED:
 *
 *   c * (a * Func_8079b24(b, 0))                       4
 *   the call result named, then c * (a * t)            4
 *   the inner product named too: u = a * t; c * u      4
 *   compound assignment to force the destination:
 *     `a *= t; c *= a;`                                15, and 18 lines
 *
 * The last one is the interesting failure: `a *= t` is exactly "a is the
 * destination", and writing it that way lets gcc reuse the parameter registers
 * differently and costs an instruction elsewhere. The destination is not
 * something the source controls here.
 *
 * NEXT: nothing at the expression level. This wants either a gcc flag that
 * disables commutative canonicalisation -- none is known -- or the observation
 * that the ROM's compiler simply ordered them differently, which would make it
 * a compiler difference rather than a source one.
 */
extern int Func_8079b24(int a, int b);

int Func_8079c30(int a, int b, int c)
{
    return (c * (a * Func_8079b24(b, 0))) / 0x10000;
}
