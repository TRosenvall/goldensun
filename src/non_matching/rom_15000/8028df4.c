/* YesNoMenu -- NOT MATCHING, a SIZE mismatch: ref 96 bytes against ours 92.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/Cyesno.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/8028df4.c \
 *     asm/rom_15000/rom_23178_a_a_c_c_a.s --func YesNoMenu
 *
 * The original prose park is preserved below.
 */
extern void Func_80284dc(void);
extern void AddMenuBarOption(int n);
extern void Func_8028808(int a, int b, int c);
extern int Func_8028574(int n);
extern void Func_802851c(void);

int YesNoMenu(int a, int b, int c, int d)
{
    int k;
    int r;

    k = 0;
    Func_80284dc();
    if (c == 0)
        c = 3;
    if (a != 0)
        k = 0x11;
    AddMenuBarOption(5);
    AddMenuBarOption(6);
    Func_8028808(k, c, b);
    r = Func_8028574(d);
    Func_802851c();
    if (r == -1)
        r = 1;
    return r;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* YesNoMenu -- asm/rom_15000/rom_23178_a_a_c_c_a.s
 *
 * BLOCKER: REGISTER ALLOCATION -- a five-way permutation, ours 43 of the ROM's 45
 *
 * Both versions spend the same five callee-saved registers (r5, r6, r7, r8,
 * r10) and the same instructions; only the assignment differs:
 *
 *     rom   a->r7   b->r10  c->r5  d->r6   k->r8
 *     ours  a->r6   b->r8   c->r5  d->r10  k->r7
 *
 * The two-instruction gap is entirely the flag variable `k`.  In the ROM it
 * lands in r8, a HIGH register, which cannot take an immediate -- so `k = 0`
 * costs `mov r3, #0 / mov r8, r3` and `k = 0x11` costs `mov r3, #0x11 /
 * mov r8, r3`.  We put it in r7 and pay one instruction each time.
 *
 * MEASURED (all 39 differing of 45, ours 43 lines):
 *   int k; int r;   with k = 0 as the first statement          39
 *   int r; int k;   (declarations swapped)                     39
 *   k = 0 moved down to just before the `if (a != 0)`          39
 *
 * Nothing in the source chooses which of five equally-long live ranges gets
 * the high register.  The parameter-copy lever (batch 118) applies to the
 * ORDER of two entry `mov`s, not to the register set, and there is no second
 * value here to trade against.
 *
 * Best C: scratch/Cyesno.c.
 *)

*/
#endif
