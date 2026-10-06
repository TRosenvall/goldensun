/* Func_8029274 -- 0x08029274  (asm/rom_15000/rom_23178_a_c_c_c_a_b.s)
 *
 * *** LANDED BYTE-IDENTICAL, 0 of 40.  Batch 329 brief B.  WAS 2 of 40. ***
 * objcmp: "OK Func_8029274 -- 80 bytes, 40 encodings and 0 relocations
 * identical".  40 real instructions, no pool words in this function.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_15000/rom_23178_a_c_c_c_a_b.c asm/rom_15000/rom_23178_a_c_c_c_a_b.s --func Func_8029274
 *
 * PINS 0.  tools/shimcount.py reports "empty asm : 1  (NOT treated as a
 * fakematch in this tree)", so this file carries NO register pin, NO
 * operand-bearing shim and NO fakematch row.  21 landed .c files already carry
 * a bare empty barrier and 4 of those 21 carry no fakematch row either; one of
 * the 4, src/rom_15000/rom_23178_a_a_a_a_c_c_a_c_b.c, IS IN THIS SAME UPSTREAM
 * MODULE (rom_15000/rom_23178.s) and documents the mechanism in its own header.
 * Booked for pass 3 (docs/directory/02-depin.md) as an artifact to revisit.
 *
 * ================= WHAT CLOSED THE LAST 2, AND WHY IT WAS MISSED ============
 *
 * The retired park had the residue exactly right and its conclusion exactly
 * wrong in one word.  Its 2 was ONE sched2 rank in the digit loop:
 *
 *     rom   strb r3,[r4] / add r2,#1      ours   add r2,#1 / strb r3,[r4]
 *
 * Read out of .23.sched2 block 7 ("Ready list (t = 0): 62 56 59", chosen 59 --
 * the list prints ASCENDING rank, so the pick is the LAST entry):
 *     insn 59  r2=r2+1  (i++)    dependent insn 69, the fused cmp+branch.
 *                               TRUE dep, cost 1  ->  priority 1.
 *     insn 56  [r4]=r3  (store)  dependent insn 65 (p++), and it is an ANTI
 *                               dep.  arm_adjust_cost (arm.c:2416-2453) returns
 *                               0 for anti/output and NEVER RAISES a cost, so
 *                               priority(56) = priority(65) + 0 = 0.
 * rank_for_schedule returns on the FIRST rung, so the store never reaches the
 * dependent-count or INSN_LUID rungs where source order would favour it.  All
 * of that reproduces.  The park then wrote: "WHAT WOULD CLOSE IT: priority(56)
 * >= 1, which needs EITHER a true-dependence consumer of the store ... OR an
 * in-block dependent for p++, at no instruction cost."  Both are indeed
 * unreachable in this program.
 *
 * ** THE FIX DOES NOT RAISE THE PRIORITY.  IT REMOVES THE READY LIST. **
 * docs/elevation.md, "AN EMPTY __asm__ volatile () BEATS PRIORITY ARITHMETIC,
 * AND IS NOT A FAKEMATCH HERE", says it in terms: a traditional asm (ASM_INPUT,
 * no operands) is analysed as using and clobbering every hard register, so the
 * ready list the two insns would have competed in NEVER FORMS.  The barrier is
 * a route EVEN WHEN THE PRIORITY ARITHMETIC IS PROVABLY UNREACHABLE -- which is
 * this park's situation precisely.  That doc section even records the shape of
 * the miss: "one park had proved its window unreachable by arithmetic,
 * correctly, and its 19-spelling list omitted this one."  This park's 20-row
 * list omitted it too.
 *
 * MEASURED, this file against the same file with the barrier deleted:
 *     with the barrier after `*p = d;`        0 of 40   byte-identical
 *     CONTROL, barrier removed                2 of 40   first at index 18,
 *                                                       ref 7023 (strb r3,[r4])
 *                                                       ours 3201 (adds r2,#1)
 *
 * WHY THIS PLACEMENT AND NOT ANOTHER.  The residue was symmetric -- the park
 * showed -fno-schedule-insns2 fixes the digit loop and BREAKS the copy-back
 * loop, so no flag can serve both.  A barrier is per-block, which is exactly
 * the asymmetry the program needs: it goes in the digit loop only, and the
 * copy-back loop keeps the scheduler's (correct) order.  That is also why this
 * is not a flag in disguise.
 *
 * WHAT THE PARK GOT RIGHT AND IS KEPT HERE, all re-measured:
 *   * TWO POINTERS.  The ROM materialises the buffer address twice -- `mov r4,sp`
 *     for the digit loop and `mov r3,sp` for the copy-back loop, with the
 *     copy-back pointer and the `ip` limit both derived from that second r3.
 *     One C pointer is one pseudo in gcc-2.96 and cannot occupy r4 then r1.
 *   * THE INT-DOMAIN ADD IS THE OPERAND ORDER.  ROM `add r1,r2,r3` is
 *     INDEX-first; `q = buf + i`, `q = &buf[i]` and `q = i + buf` all give
 *     BASE-first because fold canonicalises a pointer-plus and puts the pointer
 *     operand first however it was written.  Casting to int first leaves an
 *     ordinary PLUS_EXPR whose operand order survives:
 *         q = (char *)(i + (int)buf);   ->   add r1, r2, r3
 *   * THE NAMED `mask` IS LOAD-BEARING.  0xf used directly reads 4; dropping the
 *     declaration as well reads 31 at 40 instructions.
 *
 * SPLIT.  tools/datacheck.py on the parent gave no output, exit 0.
 * tools/split_s.py --dry-run split asm/rom_15000/rom_23178_a_c_c_c_a.s into
 * rom_23178_a_c_c_c_a_b.s (1 function, 52 lines, Func_8029274) and
 * rom_23178_a_c_c_c_a_c.s (1 function, 98 lines, Func_80292c4).
 */
void Func_8029274(unsigned int val, unsigned int n, char *out)
{
    char buf[8];
    char *p;
    char *q;
    int i;
    unsigned int d;
    int mask;

    if (n > 5)
        n = 5;
    i = 0;
    if (n != 0) {
        mask = 0xf;
        p = buf;
        do {
            d = val & mask;
            if (d <= 9)
                d += 0x30;
            else
                d += 0x37;
            *p = d;
            __asm__ volatile ("");
            i++;
            val >>= 4;
            p++;
        } while (i != n);
    }
    i = n - 1;
    if (i >= 0) {
        q = (char *)(i + (int)buf);
        do {
            *out = *q;

            q--;
            out++;
        } while ((int)q >= (int)buf);
    }
}
