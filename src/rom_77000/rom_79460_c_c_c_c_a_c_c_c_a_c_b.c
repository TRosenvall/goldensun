/* Cluster Func_807a2bc..Func_807a2bc extracted from goldensun/asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c.s.
 *
 * Slotted between rom_79460_c_c_c_c_a_c_c_c_a_c_a.o and the rest of stage1.ld.
 *
 * A bit test on a per-unit word. The FIRST PARAMETER IS UNUSED -- GetUnit()
 * overwrites r0 before anything reads it, and the ROM saves only r1 and r2 into
 * callee-saved registers.
 *
 * THE BOOLEAN HAS TO BE AN EXPLICIT if/return, WHICH IS THE OPPOSITE OF THE
 * OBVIOUS SPELLING. The ROM ends with gcc's branchless "is it non-zero" idiom:
 *
 *     and r3, r2 / neg r0, r3 / orr r0, r3 / lsr r0, #0x1f
 *
 * Written as `return (x & (1 << bit)) != 0;` gcc rewrites the whole thing to
 * `(x >> bit) & 1` -- three instructions shorter and nothing like the ROM. So
 * does `? 1 : 0`. Both are 15 lines against 18.
 *
 *     return (x & (1 << bit)) != 0;      15 lines, rewritten to a shift
 *     return (x & (1 << bit)) ? 1 : 0;   15 lines, same rewrite
 *     v = x & (1 << bit); if (v) return 1; return 0;   18 lines, exact
 *
 * Naming the mask and the value in their own statements without the `if` is 19
 * lines and worse still. It is the STATEMENT-LEVEL branch that stops the
 * rewrite, not the naming.
 */
extern void *GetUnit(void);

int Func_807a2bc(int unused, int i, int bit)
{
    unsigned char *u;
    unsigned int off;
    int v;

    u = (unsigned char *)GetUnit();
    off = (i << 2) + (0x84 << 1);
    v = *(int *)(u + off) & (1 << bit);
    if (v)
        return 1;
    return 0;
}
