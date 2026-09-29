/* Func_80b6d30 (AssignBattlePositions) -- NON-MATCHING, 4 encodings of 119.
 * 0x080b6d30, the only function in asm/rom_b5000/rom_b5a0c_c_c_c_a_c.s (no data
 * section, so datacheck.py prints nothing and NO EXPORT is required), so landing
 * would be a plain whole-file conversion.  Fresh in batch 287, re-verified at 4 in
 * batch 295.  SIZE EXACT (119 = 119).  NO SHIM, NO PIN, NO FLAG.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_b5000/80b6d30.c \
 *     asm/rom_b5000/rom_b5a0c_c_c_c_a_c.s --func Func_80b6d30
 *
 * The inner slot search is the landed Func_80b6cdc (rom_b5a0c_c_c_c_a_b.c)
 * verbatim, and the two slot stores reuse its `off`/`a` idiom -- in a NEW
 * block-scoped offset variable (a fresh `int o`; reusing the loop's `off` puts
 * it in r2 instead of the ROM's r0, 10 differing).
 *
 * THE RESIDUE, TWO PLACES, and batch 295 named the pass for both.
 *
 *  1. index 23: `mov r4, sl` (ROM) against `movs r4, #0` (ours) -- j initialised by
 *     COPYING the already-zero `ret` out of sl.  THIS IS PROVABLY UNREACHABLE FROM
 *     SOURCE, and the proof is a cost comparison, not a table question:
 *       - cse_insn (cse.c:5229) takes `src_folded` -- the constant -- whenever
 *         `src_folded_cost <= src_cost`, i.e. the CONSTANT WINS ON TIES.
 *       - COST (cse.c:509) of a pseudo is 1, because CHEAP_REG (cse.c:505) needs
 *         `REG_USERVAR_P && REGNO < FIRST_PSEUDO_REGISTER` and CHEAP_REGNO
 *         (cse.c:495) covers only the frame/stack/arg pointers, the virtuals and
 *         fixed hard regs.
 *       - notreg_cost (cse.c:725) of `(const_int 0)` is `rtx_cost (x, SET) * 2`, and
 *         arm_rtx_costs' thumb CONST_INT case (arm.c:2077-2080) returns 0 when
 *         `outer == SET` and the value is < 256.  So src_folded_cost = 0 <= 1 and
 *         the fold is unconditional.
 *     .03.cse confirms it: insn 54 goes from `(set (reg/v:SI 38) (reg/v:SI 37))` to
 *     `(set (reg/v:SI 38) (const_int 0))` with REG_EQUAL, where 37 is ret and 38 is j.
 *     THE ONLY ESCAPE IS CHEAP_REG, i.e. a hard-register user variable, and it is
 *     measured: `register int ret __asm__("r10")` is 102 of 119 and `__asm__("sl")`
 *     is 102 -- pinning ret for its whole life costs two instructions and the loop
 *     rotation.  So the park's old line "something the ROM's author wrote keeps
 *     ret's zero out of cse1's table; not found" should be retired: nothing in the
 *     same basic block can, because the decision is made on cost before the table
 *     is consulted, and the ROM's own instruction layout puts `ret = 0` (indices
 *     18/21, `movs r1,#0 / mov sl,r1`) and `j = ret` (index 23) in one block with
 *     only a call between, which does not invalidate a pseudo.
 *
 *  2. indices 82-84: `lsl r3,r5,#12 / orr r3,r7 / mov sl,r3` (ROM) against r2.  This
 *     is reload's choice of RELOAD REGISTER for `ret = (i << 12) | v` (ret lives in
 *     sl, so reload computes into a low reg and copies -- the copy is insn 381, a
 *     reload-created insn).  THE PARK MISREAD THE DUMP: the `.18.greg` line
 *     "Using reg 3 for reload 0" against insn 200 is printed by find_reg
 *     (reload1.c:1664), which selects which hard register to SPILL, not which
 *     register the reload gets.  The reload register is chosen later, per insn, by
 *     allocate_reload_reg, which walks `spill_regs` ROUND-ROBIN from
 *     `last_spill_reg` (reload1.c:5003, updated at 4937) precisely so that
 *     consecutive reloads leapfrog.  So the register at index 82 is a function of
 *     the COUNT of reload-register allocations made EARLIER in the function, not of
 *     anything written at that statement -- which is exactly why every respelling
 *     of it is inert, and why it is coupled to (1).
 *
 * DELTA to the inert list, batch 295 -- 15 further spellings, all still 4 of 119:
 *   * commuted operands `v | (i << 12)`; `!j` instead of `j == 0`; the commuted form
 *     paired with `!j`; a block-scoped temp for `i << 12` paired with the commuted
 *     form; a function-scope temp with the uncommuted form; `ret = i << 12; ret |= v`
 *     -- the park had measured several of these alone, and NO PAIR among them pays;
 *   * declaration order: `ret` last, `ret` first, `j` before `ret` -- all 4, so the
 *     spill-slot/allocno-order lever does not reach this one;
 *   * `i` declared before `j`; `o` declared at function scope rather than in the
 *     store block; the tail test written `if (v != 0x1dc && v != 0x1e3) break;`;
 *     `(int)ewram_2018000 + (i << 14)` instead of `(int)(ewram_2018000 + (i << 14))`.
 *   NEW MEASUREMENTS THAT ARE NOT INERT, recorded so nobody repeats them:
 *     `a = off; a += 4;` in the inner loop -> 75.  Hoisting `u[0x128]` into a local
 *     `id` and using it at all three call sites -> 59.  Moving `ret = 0` to AFTER
 *     the Func_80c2384 call -> 10 (the park's note covers `j = ret` before and after
 *     the call, not `ret = 0`).
 *   AND the cse-defeating barrier, measured: `__asm__ volatile ("" : "+r" (ret))`
 *     after `ret = 0` is 98, before the loop 96, non-volatile 96.  It DOES restore
 *     the copy -- 96 shows `mov r2, sl` where we had `movs r4, #0`, which is the
 *     mechanism in (1) confirmed from the other side -- but it costs two extra
 *     instructions and un-rotates the loop.  An `__asm__ ("mov %0, #0" : "=r")`
 *     producer for the zero fails to compile to a comparable stream at all (0 lines).
 *
 * INERT from earlier batches (unchanged): `for (j = ret; ...)`, `j = ret` before and
 * after the call, `ret = j = 0`, `j = ret = 0`, `j = 0; ...; ret = j` (all 4 or far
 * worse, 77-104); `-fno-gcse`, `-fno-cse-follow-jumps`, `-fno-rerun-cse-after-loop`,
 * `-fno-strength-reduce` all leave it at 4.  `+` instead of `|` is far worse (56).
 *
 * SHIMS -- NONE:
 *   register class:  0
 *   .equ class:      0
 *   other __asm__:   0
 */
extern unsigned char *_GetUnit(int id);
extern int Func_80c23c0(int a);
extern int Func_80c2384(int a);
extern int Func_80c23a0(int a);
extern int _PreloadSpriteGFX(int a, int b, int c, int d);
extern char *iwram_3001e74;
extern unsigned char ewram_2018000[];

int Func_80b6d30(int slot)
{
    char *s;
    unsigned char *u;
    int flag;
    int v;
    int ret;
    int j;
    int i;
    int off;
    int a;

    s = iwram_3001e74;
    u = _GetUnit(slot);
    flag = Func_80c23c0(u[0x128]);
    ret = 0;
    v = Func_80c2384(u[0x128]);
    for (j = ret; j <= 1; j++) {
        if (u[0x129] != 0)
            continue;
        for (i = 0; i <= 5; i++) {
            off = i * 2;
            a = off + 4;
            if (*(short *)(s + a) != 0)
                continue;
            if (flag != 0)
                break;
            if (i > 4)
                continue;
            a = off + 6;
            if (*(short *)(s + a) == 0)
                break;
        }
        if (i == 6)
            break;
        if (_PreloadSpriteGFX(i, (int)(ewram_2018000 + (i << 14)), v + j,
                              Func_80c23a0(u[0x128])) == 0)
            return 0;
        if (j == 0)
            ret = (i << 12) | v;
        {
        int o = i * 2;
        a = o + 4;
        *(short *)(s + a) = slot;
        if (flag == 0) {
            a = o + 6;
            *(short *)(s + a) = slot;
        }
        }
        if (v == 0x1dc || v == 0x1e3)
            continue;
        break;
    }
    return ret;
}
