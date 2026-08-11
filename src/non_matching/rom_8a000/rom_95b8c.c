/* Func_8095b8c  [rom_8a000]
 * Source asm: goldensun/asm/rom_8a000/rom_944ec_a_c_a_a_c.s
 *
 * Parked: logic faithful, does NOT byte-match. HAND-ONLY (`.L` fn -> not permutable).
 * Candidate: tools/runs/run_20260606T194103Z/Func_8095b8c-iter-7.c
 * TODO(residual): one instruction; the indexed load operand order. ROM emits
 *   `ldr r3,[r3,r1]` (computed index = base, .L9f0a4 ptr = offset); candidate emits
 *   `ldr r3,[r1,r3]`. Restructure so the index expression is the pointer base, e.g.
 *   `*(u32*)((((iwram_3001800>>2)&1)<<2) + (char*)L9f0a4)`; try variants.
 *
 * THAT SUGGESTION WAS TRIED AND IS WRONG (batch 29). Writing the index as the
 * pointer base gives THREE differing positions instead of one -- worse than the
 * form below. So do the two other obvious variants:
 *
 *   the index in its own named local, then `L9f0a4[i]`      3 differ
 *   `*(u32 *)(i + L9f0a4)` with L9f0a4 typed unsigned char  3 differ
 *   the array-index form below                              1 differ
 *
 * All three of the restructurings move the whole expression rather than just
 * the operand order, which is what makes them worse. The array-index form stays
 * the closest and the one instruction is still open.
 */
extern unsigned int iwram_3001800;
extern unsigned int L9f0a4[] __asm__(".L9f0a4");

/* Takes no arguments. Returns an entry from .L9f0a4 selected by bit 2 of
 * iwram_1800 -- which of two ride variants is currently configured.
 */
void Func_8095b8c(void *p)
{
    unsigned int v = L9f0a4[(iwram_3001800 >> 2) & 1];
    *(unsigned int *)((char *)p + 0x18) = v;
    *(unsigned int *)((char *)p + 0x1c) = v;
}
