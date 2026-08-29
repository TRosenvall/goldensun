/*
 * Func_80175c0  (MeasureString)  --  asm/rom_15000/rom_15e8c_c_a_c_c_c.s
 *
 * BLOCKER: register allocation -- one too few callee-saved registers.
 *
 * 43 ROM lines against 42 of ours, 29 differing, first difference at line 13.
 * The ROM pushes {r5, r6, r7, lr}; we push {r5, r6, lr}. Everything downstream
 * of that cascades.
 *
 * SETTLED, and worth keeping:
 *
 *   * The pooled zero was the FIRST symptom and it is fixed. Writing
 *     `*(short *)(p + 0x12f4) = 0;` emits `ldr r2, =0x0` -- a literal zero in
 *     the pool -- because the halfword store makes the 0 an operand of a HImode
 *     expression. See the halfword exception in const.sym. Assigning through a
 *     named `int z = 0;` gives the ROM's `mov r6, #0`, and the same z then
 *     feeds `str r6, [sp]` for the fifth argument, exactly as the ROM reuses it.
 *
 *   * The second store's address IS derived from the first: the ROM does
 *     `ldr r2, =0x12f4 / add r3,r5,r2 / add r2,#2 / add r3,r5,r2`, and writing
 *     the two offsets as plain 0x12f4 and 0x12f6 reproduces that derivation.
 *     No special spelling needed.
 *
 *   * The call is Func_80165d8(dest, n, 0, 0, z, 1) -- six arguments, the last
 *     two through [sp] and [sp+4], with `n` (the BufferString result) kept in
 *     r1 across the intervening compares.
 *
 * WHAT REMAINS: the ROM keeps three values in callee-saved registers across
 * the BufferString call -- the iwram base (r5), the zero (r6), and `dest` (r7).
 * We keep only two, because `dest` arrives in r0 and we move it out early
 * (`mov r6, r0` at the top) instead of after the two halfword stores, where the
 * ROM does it. Ordering the source so the stores happen before `dest` is first
 * read is the obvious next lever and has not been tried.
 */
extern unsigned char *iwram_3001e8c;
extern int BufferString(int id, int mode);
extern int Func_80165d8(int a, int b, int c, int d, int e, int f);

int Func_80175c0(int dest, int id)
{
    unsigned char *p;
    int n;
    int r;
    int z;

    p = iwram_3001e8c;
    z = 0;
    *(short *)(p + 0x12f4) = z;
    *(short *)(p + 0x12f6) = z;
    n = BufferString(id, 1);
    if (*(unsigned short *)(p + (0xeb << 4) + n * 2) == 0)
        return 0;
    if (dest == 0)
        return 0;
    r = Func_80165d8(dest, n, 0, 0, z, 1);
    if (r == 0)
        return 0;
    return r;
}
