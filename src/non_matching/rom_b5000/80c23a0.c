/*
 * Func_80c23a0 (GetSpriteTableField2) -- asm/rom_b5000/rom_c1a34_a_a_c_c_a_a_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: which register receives the sum. 16 lines against 16, 4 differing,
 * and it is one swap:
 *
 *      rom   ldr r3,=table / lsl r2,r0,#3 / add r2,r3 / ldrb r0,[r2,#3]
 *      ours  ldr r2,=table / lsl r3,r0,#3 / add r3,r2 / ldrb r0,[r3,#3]
 *
 * The ROM adds the table INTO the scaled-index register; we add the index into
 * the table register.
 *
 * SETTLED: the access must go through a POINTER LOCAL (`p = table + i * 8;`
 * then `p[3]`). Written as a subscript on the array, gcc folds the +3 into the
 * offset and emits a register-offset load instead of the ROM's immediate-offset
 * one -- a different shape entirely.
 *
 * TRIED AND REJECTED: `(unsigned char *)(i * 8 + (int)table)` to invert which
 * operand is the pointer (4 differing, unchanged); a named table local assigned
 * before the guard (15 lines, 14 differing -- worse).
 *
 * The index is unsigned: `cmp r0, #0xab / bls`.
 */
extern unsigned char Lc7420[] __asm__(".Lc7420");

int Func_80c23a0(unsigned int i)
{
    unsigned char *p;

    if (i > 0xab)
        return *(unsigned short *)Lc7420;
    p = Lc7420 + i * 8;
    return (unsigned int)(p[3] << 27) >> 28;
}
