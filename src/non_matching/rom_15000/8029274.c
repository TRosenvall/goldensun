/* Func_8029274 -- asm/rom_15000/rom_23178_a_c_c_c_a.s
 *
 * STILL NON-MATCHING, 2 differing encodings of 40 -- IMPROVED FROM 6 in batch
 * 328 brief G.  Size identical, relocations clean, 40 real instructions
 * against 40, no pool words anywhere in this function.  2 IS A TRUE DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/8029274.c asm/rom_15000/rom_23178_a_c_c_c_a.s --func Func_8029274
 *
 * --whole: the reference holds TWO functions, ['Func_8029274', 'Func_80292c4'],
 *   so landing needs a two-way split.  SPLIT SHAPE (unchanged, re-verified):
 *     tools/datacheck.py asm/.../rom_23178_a_c_c_c_a.s -> NO OUTPUT, exit 0
 *     tools/split_s.py ... Func_8029274 --dry-run ->
 *       rom_23178_a_c_c_c_a_b.s (1 function, 52 lines)   [Func_8029274]
 *       rom_23178_a_c_c_c_a_c.s (1 function, 98 lines)   [Func_80292c4]
 *       removes rom_23178_a_c_c_c_a.s, rewrites stage1.ld
 *       install path on a landing: src/rom_15000/rom_23178_a_c_c_c_a_b.c
 * PINS: 0.  No shims, no device, no per-file flag, no fakematch row.
 *
 * =================== WHAT CLOSED FOUR OF THE SIX, AND WHY ===================
 *
 * The retired park recorded BOTH halves of this fix in its own negatives list,
 * each measured ALONE and each therefore rejected:
 *
 *     "a SECOND pointer variable for the copy-back loop is 21 and two short"
 *     "v02  p = (char *)(i + (int)buf)   6"   (inert, one container)
 *
 * CROSSED, THEY ARE THE WHOLE OF CLUSTER 2.  Two pointer variables take 6 -> 3;
 * forming the second one by an INT-DOMAIN add takes 3 -> 2.  Neither works
 * without the other, and the park's nine-row cross varied only how ONE pointer
 * was formed -- the number of pointers was never a dimension.  This is the
 * batch-327 law verbatim: when a park's negatives all vary one dimension, the
 * answer is in a dimension nobody varied.
 *
 *   * TWO POINTERS.  The ROM materialises the buffer address TWICE --
 *     `mov r4, sp` for the digit loop and `mov r3, sp` for the copy-back loop,
 *     with the copy-back pointer AND the `ip` limit both derived from that
 *     second r3 (`add r1, r2, r3` / `mov r12, r3`).  One C pointer is one
 *     pseudo in gcc-2.96 and cannot occupy r4 then r1, which is exactly the
 *     tension the old park named as "the most promising thing left here".  It
 *     was right.  The park's "two short" was a MISSING PREREQUISITE: with `q`
 *     actually declared, the instruction count is 40 = 40.
 *   * THE INT-DOMAIN ADD IS THE OPERAND ORDER.  ROM `add r1, r2, r3` is
 *     INDEX-first; `q = buf + i` and `q = &buf[i]` and even `q = i + buf` all
 *     give BASE-first, because fold canonicalises a POINTER_PLUS and puts the
 *     pointer operand first regardless of how it was written.  Casting to int
 *     first leaves an ordinary PLUS_EXPR whose operand order survives:
 *         q = (char *)(i + (int)buf);   ->   add r1, r2, r3
 *     `q = buf; q += i;` is 5 -- a compound assignment makes q's own pseudo the
 *     destination (expr.c:7290-7292, no EXPAND_SUM), which is a third shape.
 *
 * ========================= THE REMAINING 2, NAMED =========================
 *
 * ONE SCHEDULING DECISION IN THE DIGIT LOOP, and it is a sched2 rank, not a
 * dependence error.  The ROM issues the store first; we issue `i++` first:
 *
 *     rom   strb r3,[r4] / add r2,#1      ours   add r2,#1 / strb r3,[r4]
 *
 * Read out of `.23.sched2`, block 7 (`Ready list (t = 0): 62 56 59`, chosen
 * 59 -- the list prints ASCENDING rank, so the pick is the LAST entry):
 *
 *     insn 59  `r2=r2+1`    (i++)   dependent: insn 69, the fused `cmp r2,r1`
 *                                   + branch.  TRUE dep -> priority 1.
 *     insn 56  `[r4]=r3`    (store) dependent: insn 65 `r4=r4+1` (p++) and the
 *                                   dependence is an ANTI dep.  arm_adjust_cost
 *                                   (arm.c:2416-2453) returns 0 for anti and
 *                                   output deps and NEVER RAISES a cost, so
 *                                   priority(56) = priority(65) + 0 = 0.
 *
 * So `rank_for_schedule` returns on the FIRST rung and the store never gets to
 * the dependent-count or INSN_LUID rungs where source order would favour it.
 *
 *   >> WHAT WOULD CLOSE IT: priority(56) >= 1, which needs EITHER a true-
 *      dependence consumer of the store (a load of the same byte -- not in this
 *      program) OR an in-block dependent for `p++`, at no instruction cost.
 *      The loop-closing compare is on `i` against `n` in the ROM (`cmp r2,r1`),
 *      so the pointer cannot be made to carry the exit test.
 *
 * `-fno-schedule-insns2` is NOT the answer and proves the residue is symmetric:
 * it fixes the digit loop and BREAKS the copy-back loop (still 2, now
 * `sub r1,#1` against `strb r3,[r5]` at idx 37).  The scheduler is right in one
 * loop and wrong in the other, so no flag can serve both.  NOT a flag row.
 *
 * ===================== MEASURED INERT / WORSE (batch 328) ====================
 * All rows one container, every row ref 40 / ours 40 unless noted.
 *   q = &buf[i]                                               3   (base-first)
 *   q = i + buf                                               3   (fold swaps it)
 *   q = buf; q += i;                                          5
 *   loop 1 fully INDEXED, `buf[i] = d`, no p at all           2   exactly inert
 *   loop 2 INDEXED on a down counter, `out[..] = buf[i]`      2   exactly inert
 *   *(unsigned char *)p = d   (alias-set / sched2 lever)      2   exactly inert
 *   unsigned char buf[8]                                      2   exactly inert
 *   *p++ = d                                                  2   exactly inert
 *   i++ last in the body / p++ before i++ / i = i + 1         2   exactly inert
 *   val >>= 4 before the store                                2   exactly inert
 *   store via a named `char` temp                             2   exactly inert
 *   *p = (char)d                                              2   exactly inert
 *   `d` as signed int                                         2   exactly inert
 *   0xf used directly instead of the named `mask`             4   WORSE
 *   `mask` declaration removed as well                       31   WORSE, 40/40
 * The last two re-confirm the old park's finding that the NAMED `mask` is
 * load-bearing.  The byte-access/alias-set lever (brief 328's "if a target has
 * a byte access and a sched2 residue, spell it both ways") is EXACTLY INERT
 * here: `char` already has alias set 0, so there is no room to move.
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
