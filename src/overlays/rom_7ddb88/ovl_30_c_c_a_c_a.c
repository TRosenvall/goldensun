/* OvlFunc_955_200805c extracted from goldensun/asm/overlays/rom_7ddb88/ovl_30_c_c_a_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * FOUND BY THE CONSTANT-CSE SEARCH AND IT IS NOT THAT CLASS. The flag id 0x335
 * is loaded twice, which is the shape -- but once in each arm of an if/else,
 * `__SetFlag` on one side and `__ClearFlag` on the other. The two can never be
 * live together, so gcc does not hoist. -fno-rerun-cse-after-loop changes
 * nothing here (6 of 36 either way) and NO Makefile rule was added.
 *
 * Second confirmation of that caveat in two batches; see the note in
 * tools/pick_candidates.py.
 *
 * THE REAL DEFECT WAS THE STACK-ARG PAIR, and its fix is the opposite of the
 * basic-block lever's advice. Both arms pass the same pair (0x23, 0x4d) and the
 * ROM builds it FRESH IN EACH ARM. So the two locals must be assigned INSIDE
 * each arm:
 *
 *   assigned in each arm      36 lines, exact
 *   hoisted above the `if`    35 lines, 26 differ -- gcc materialises once and
 *                             keeps it across the branch
 *   passed as bare literals   36 lines, 6 differ -- one register reused for
 *                             both slots instead of two
 *
 * Hoisting a constant to a dominating block is right when the ROM builds it
 * once; here it builds it twice, and the source has to say so.
 */
extern void *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_955_200805c(void)
{
    unsigned char *a;
    int t;
    int m;
    int n;

    a = (unsigned char *)__MapActor_GetActor(0xb);
    t = *(int *)(a + 8) >> 20;
    if (t == 0x24) {
        __SetFlag(0x335);
        m = 0x23;
        n = 0x4d;
        __Func_8010704(0x23, 0x4e, 1, 1, m, n);
    } else {
        __ClearFlag(0x335);
        m = 0x23;
        n = 0x4d;
        __Func_8010704(0x22, 0x4d, 1, 1, m, n);
    }
}
