/* Func_80b06c0  --  0x080b06c0, asm/rom_b0000/rom_b0070_a_a_c_a_c_c_c.s
 *
 * BLOCKER CLASS: two-operand versus three-operand shift.
 * Status: 22 lines against 22, TWO differing, everything else exact.
 *
 *      rom    lsl r3, r1, #0x4 / add r1, r3, #0x1
 *      ours   lsl r1, #0x4     / add r1, #0x1
 *
 * WHAT IT DOES
 * Stamps a palette-slot byte, `(slot << 4) + 1`, into six OAM-ish fields of
 * each of `n` records, whose offsets come from the halfword table at .Lb4100.
 *
 * BOTH FORMS COMPUTE THE SAME VALUE IN THE SAME REGISTER. The ROM shifts into a
 * fresh register and then adds into r1; gcc shifts r1 in place and adds in
 * place. Same instruction count, same result, different operand forms -- Thumb
 * has `lsl rd, rm, #imm` and `lsl rd, #imm`, and gcc picks the two-operand form
 * whenever the destination and source are the same pseudo.
 *
 * WHAT WAS TRIED
 *   - the shift in its own named local (`s = k << 4; v = s + 1;`), which is the
 *     usual way to force a separate register: gcc allocates `s` to `k`'s
 *     register because `k` is dead after it, and emits the same two
 *     instructions.
 *   - `v` as `unsigned char` rather than `int`: WORSE -- the whole computation
 *     narrows to byte width and three extra instructions appear. `int` is
 *     correct and is kept below.
 *
 * Nothing in C says "put this result somewhere other than its operand". This is
 * the same shape as the register-birth cases in batch 73: reachable only if the
 * two values come from statements that do different KINDS of work, and here
 * they do not -- it is one expression.
 *
 * The loop itself is exact, including the table walked with a separate pointer
 * and the counter decremented before the stores.
 */

extern unsigned short Lb4100[] __asm__(".Lb4100");

void Func_80b06c0(int n, int k, char *base)
{
    int v;
    int s;
    unsigned short *t;
    char *p;

    s = k << 4;
    v = s + 1;
    if (n > 0) {
        t = Lb4100;
        do {
            p = base + *t;
            n--;
            t++;
            p[4] = v;
            p[8] = v;
            p[0xc] = v;
            p[0x10] = v;
            p[0x14] = v;
            p[0x18] = v;
        } while (n != 0);
    }
}
