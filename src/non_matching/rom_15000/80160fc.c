/* Func_80160fc -- NOT MATCHING, a LENGTH gap: ref 50 instructions against ours 49 (-1).
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/L60fcb.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * THE LENGTH-GAP CRITERION APPLIES, and it is this tree's most productive
 * targeting instrument -- four batches run on it closed 8 of 15, 7 of 25,
 * 8 of 25 and 10 of 32, and LOCATED the missing-or-extra instruction on
 * essentially every target.  Our side is 1 instruction(s) SHORTER, so the
 * search is which statement is missing -- see docs/elevation.md, 'OUR STREAM BEING
 * SHORTER IS A SIGNATURE'.
 * No encoding figure exists until the lengths agree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/80160fc.c \
 *     asm/rom_15000/rom_15e8c_a_c_a_a_c_a_c.s --func Func_80160fc
 *
 * The original prose park is preserved below.
 */
#include "dma.h"

extern char *iwram_3001e8c;

void Func_80160fc(void)
{
    char *b;
    char *s;
    char *d;
    unsigned char *q;
    int off;
    int m;
    int one;
    int step;

    b = iwram_3001e8c;
    off = 0xea6;
    q = (unsigned char *)(b + off);
    if (*q != 0)
        return;
    off -= 3;
    q = (unsigned char *)(b + off);
    m = *q;
    if (m == 0)
        return;
    d = (char *)0x6002000;
    if ((m & 1) != 0)
        m = 0x3f;
    m = (m & 0x3f) >> 1;
    one = 1;
    step = 0x80 << 1;
    s = b;
loop:
    if ((m & one) != 0)
        DMA3_COPY(s, d, 0x100);
    m >>= 1;
    s += step;
    d += step;
    if (m != 0)
        goto loop;
    *(unsigned char *)(b + 0xea3) = m;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* Func_80160fc -- asm/rom_15000/rom_15e8c_a_c_a_a.s
 *
 * BLOCKER: gcc will NOT derive one constant from another here, and the ROM does
 * -- the inverse of the usual constant-CSE problem.  44 of 54, ours 53 lines.
 *
 *     rom  ldr r2, =0xea6 / ... / sub r2, #0x3 / add r3, r7, r2 / ldrb
 *     ours ldr r3, =0xea6 / ... / ldr r3, =0xea3               / ldrb
 *
 * Every other blocker in the corpus is gcc commoning two constants the ROM
 * rebuilds.  This one is the reverse: the ROM's compiler related 0xea6 and
 * 0xea3 with a `sub`, and ours emits two independent pool loads.  Writing the
 * offset as one variable mutated in place (`off = 0xea6; ... off -= 3;`) does
 * not reach it -- gcc constant-folds `off` at each use, because both values
 * are compile-time constants and there is nothing to stop it.
 *
 * WHAT DID WORK, and is reusable:
 *   * The `goto` loop is right.  Both `ldr r3, =REG_DMA3SAD` and
 *     `ldr r2, =0x84000040` sit INSIDE the loop body in the ROM, which is the
 *     documented signature; a `do/while` hoists them.
 *   * `one = 1;` and `step = 0x80 << 1;` before the loop are source variables,
 *     not hoists -- the corollary to the goto rule.
 *   * A NAMED destination pointer per access (`q = (unsigned char *)(b + off);`)
 *     gets the ROM's `add r3, r7, r2 / ldrb r3, [r3]` instead of a reg+reg
 *     `ldrb r3, [r6, r3]`.  Worth 8 instructions: 52 differing at 51 lines
 *     becomes 44 at 53.
 *
 * MEASURED:
 *   offset inline, reg+reg addressing        52 of 54, ours 51 lines
 *   offset named and mutated, named pointer  44 of 54, ours 53 lines
 *
 * Best C: scratch/L60fcb.c.
 *)

*/
#endif
