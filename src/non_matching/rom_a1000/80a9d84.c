/* Func_80a9d84  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_a1000/rom_a8604_c_c_a_c_a_c.s
 * Best screen: 14 instructions in disagreeing regions, of 30 (rom 30, ours 34).
 *
 * BLOCKER CLASS: loop-invariant hoisting, and the register pressure it creates.
 *
 * A structural twin of src/rom_a1000/rom_a8604_c_c_a_c_a_b.c (Func_80a9cbc),
 * which matches. The difference is a THIRD constant: this loop also writes 0xf0
 * to byte +0x0f.
 *
 * The ROM hoists two of the three constants into callee-saved registers
 * (0xf8 -> r8, 0xa8 -> r7) and materialises the third INSIDE the loop, reusing
 * a scratch register:
 *
 *      mov r3, r8 / strh r3, [r0, #6] / mov r3, #0xf0 / strh r7, [r0, #8]
 *      strb r3, [r0, #0xf]
 *
 * gcc hoists ALL THREE. That needs a second high register, so the prologue
 * grows from `mov r7, r8 / push {r7}` to `mov r7, r10 / mov r6, r8 /
 * push {r6, r7}` and our stream is four instructions longer.
 *
 * WHAT WAS TRIED
 *  1. The constant as a named local assigned inside the loop (kept below).
 *  2. The literal written directly at the store, `*(x + 0xf) = 0xf0;`.
 *     BYTE-IDENTICAL -- naming it changes nothing, the hoist happens either way.
 *  3. Flags: -fno-rerun-cse-after-loop, -fno-strength-reduce, -fno-thread-jumps.
 *     All three byte-identical to the default.
 *
 * The decision is loop-invariant motion in loop.c, which has no flag of its own
 * in this compiler, and it is not reachable from the source: a constant stored
 * every iteration IS loop-invariant however it is spelled. The ROM's compiler
 * declined to hoist the third one, presumably because it ran out of cheap
 * callee-saved registers first.
 *
 * Same shape of conclusion as src/non_matching/ovl_7bf5a8/2008704.c and
 * src/non_matching/rom_b5000/80bf54c.c: what registers get used is a
 * consequence of pressure in the original, not of how the C is written.
 */
extern unsigned char *iwram_3001f2c;
extern void Func_80a17c4(void *x);

void Func_80a9d84(void)
{
    unsigned char *p;
    unsigned char **q;
    unsigned char *x;
    int a;
    int b;
    int c;
    int i;
    int t;

    p = iwram_3001f2c;
    a = 0xf8;
    q = (unsigned char **)(p + 0xc8);
    b = 0xa8;
    i = 4;
    do {
        x = *q++;
        if (x != 0) {
            t = a;
            *(unsigned short *)(x + 6) = t;
            c = 0xf0;
            *(unsigned short *)(x + 8) = b;
            *(x + 0xf) = c;
            Func_80a17c4(x);
        }
        i--;
    } while (i >= 0);
}
