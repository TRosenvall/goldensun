/* Cluster HeightTile_7..HeightTile_7 extracted from goldensun/asm/rom_9000/rom_11ce0_a_c_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_9000/rom_11ce0_a_c_c_a_a_a.o and asm/rom_9000/rom_11ce0_a_c_c_a_a_c.o in
 * goldensun/stage1.ld.
 *
 * Interpolates between two signed height samples by a weight looked up from a
 * 16-wide table.
 *
 * UNPARKED. It was filed as "reg-alloc/scheduling divergence (register swap /
 * op-order); logic correct. Permuter seed" at one differing instruction:
 *
 *     rom    lsl r2, #0x4
 *     ours   lsl r1, #0x4
 *
 * The logic was NOT correct. The index was written `param_2 * 16 + param_3`
 * and the ROM computes `param_3 * 16 + param_2` -- the two parameters are
 * transposed, so the function was reading the wrong table entry.
 *
 * Fixing that alone gives a DIFFERENT single-instruction diff: the sum
 * accumulates into r2 where the ROM accumulates into r1. That second step is
 * genuine codegen, and the operand ORDER of the addition decides it:
 *
 *     param_3 * 16 + param_2    ->  add r2, r1   (accumulate into the shift)
 *     param_2 + param_3 * 16    ->  add r1, r2   (accumulate into the other)
 *
 * So a one-instruction register difference on an `add` is worth trying both
 * spellings of the addition before calling it allocation. Same class of fact as
 * the destructive-op note in docs/elevation.md: gcc writes the result into
 * whichever operand it evaluated first.
 *
 * Two bugs stacked, and the first hid the second -- the wrong index produced a
 * diff at the same instruction the reg-alloc issue would have, so the park's
 * description matched what was on screen while being wrong about the cause.
 */
extern unsigned char L132fc[] __asm__(".L132fc");

int HeightTile_7(signed char *param_1, int param_2, int param_3)
{
    int i, a, b;

    i = 0;
    a = param_1[i];
    i = 1;
    b = param_1[i];
    a <<= 19;
    b <<= 19;
    return a + (b - a) * L132fc[param_2 + param_3 * 16];
}
