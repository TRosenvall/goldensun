/* OvlFunc_943_2008950 -- NOT MATCHING, 15 differing STREAM encodings of 69.
 *
 * ref 69 / ours 69 stream encodings; 52/52 16-bit instructions -- IN PHASE, so a
 * true DISTANCE.  BODY REPLACED by batch 339 brief C; the batch-338 recovery was
 * at "ref 52 / ours 51, 35 differing", a LENGTH gap, and this closes it.
 *
 * WHAT CLOSED THE LENGTH GAP: A SECOND POINTER VARIABLE.  The park reused one
 * `q` for both stores in the 0x925 arm; `q2` for the first one takes 51 -> 52
 * instructions and 35 -> 15 differing, and clears the relocation difference too.
 *
 * THE PARK NAMED THE WRONG PASS, AND THAT IS WHY ITS NEGATIVE MISLED IT.  It
 * read the ROM's two freshly built offsets against our
 *      ours  mov r2,#0xa7 / lsl r2,#1 / ... / add r2, #0x60
 * as "the main -O2 CSE, not the rerun pass", on the evidence that CSE_CFLAGS was
 * inert.  CSE_CFLAGS IS inert -- because no CSE does this.  It is
 * `reload_cse_move2add` (reload1.c:8840), run from `reload_cse_regs` whenever
 * `optimize > 0` (toplev.c:3366-3371).  The transform at reload1.c:8880-8912
 * rewrites a later `(set REGX (const_int B))` into
 * `(set REGX (plus REGX (B-A)))`, and ITS PRECONDITION IS THAT THE SAME HARD
 * REGISTER ALREADY HELD A -- `reg_set_luid[regno]` live and
 * `reg_base_reg[regno] < 0`.  The ROM's two offsets are in r1 and r3, ours were
 * both in r2.  So this was never a CSE question: it is register allocation, no
 * CSE flag can reach it, and the lever is anything that stops the two offsets
 * sharing a hard register.  The second pointer is that.
 *
 * WHAT IS LEFT IS A PURE TWO-REGISTER SWAP.  All 15 differing encodings are the
 * same instruction with r2 and r3 exchanged:
 *      rom   ldr r2,=L8 ... add r3,r2,r1 / strb r5,[r3] / mov r3,#0xd7 ...
 *      ours  ldr r3,=L8 ... add r2,r3,r1 / strb r5,[r2] / mov r2,#0xd7 ...
 * The base pointer `p` holds r3 on our side and r2 in the ROM; every short-lived
 * temporary in the arm holds the other one.  REG_ALLOC_ORDER is
 * `3, 2, 1, 0, 12, 14, 4, 5, ...` (config/arm/arm.h:989-995), so r3 goes to the
 * HIGHER-ranked allocno -- in the ROM that is the arm's temporaries, on our side
 * it is `p`.  `allocno_compare` (global.c:601-617) ranks on
 * `floor_log2(n_refs) * n_refs / live_length`, so the only axes that can flip it
 * are `p`'s reference count and its live length.  BOTH WERE SWEPT AND BOTH ARE
 * PLATEAU (see below), which is why this parks rather than landing.
 *
 * MEASURED THIS ROUND, all at 52/52 instructions unless noted:
 *   two pointer variables (this body)                        15  <- best
 *   the park's single reused `q`                       -1 insn, 35
 *   both offsets named, born before the first store    -1 insn, 35  (INERT:
 *       naming the offsets does nothing; it is the POINTER that matters)
 *   both offsets named, born after the first store     -1 insn, 35
 *   unsigned-int base + one-operation-per-statement offsets   15  (same as best)
 *   two pointers AND named offsets                           15  (same as best)
 *   the eight per-file FLAG groups on this body               15  EVERY ONE.
 *       Flag axis CLOSED -- including -O1, which is also 15.
 *   23 ANCHOR placements (`do { } while (0);`)                15  every valid
 *       one.  Anchor axis CLOSED.
 *   seven DECLARATION ORDERS of the five locals               15  every one.
 *       Declaration order is INERT here.
 *   merge via a fresh `q` instead of mutating `p`             19  worse
 *   merge not referencing `p` at all                   +1 insn, 53  worse
 *   `p` copied to a second pointer for the merge              15  inert
 *   `w` given an extra reference in the merge                 19  worse
 *   `w` born first in each arm                         -2 insn, 51  worse
 *   the two pointer roles exchanged                           15  inert
 *
 * That is 46 measurements on a 15-encoding residue with no movement at all, so
 * the next move is NOT another spelling.  It is the `.18.greg` dump: find which
 * allocno `p` is, read its `n_refs` and `live_length`, and compute what either
 * would have to become for `allocno_compare` to put the temporaries first.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/2008950.c \
 *     asm/overlays/rom_7c7b9c/ovl_30_c_a_a_a.s --func OvlFunc_943_2008950
 *
 * The original prose park is preserved below.  Keep its last paragraph -- the
 * `v = __GetFlag(0x928)` reuse is right and still in this body -- and discard its
 * BLOCKER paragraph, which named the wrong pass.
 */
extern int __GetFlag(int id);
extern unsigned char gScript_968__0200d508[];
extern unsigned char L4ef0[] __asm__(".L4ef0");
extern unsigned char L5028[] __asm__(".L5028");
extern unsigned char L4cf8[] __asm__(".L4cf8");
extern unsigned char L4ba8[] __asm__(".L4ba8");

unsigned char *OvlFunc_943_2008950(void)
{
    unsigned char *p;
    unsigned char *q;
    int v;
    int w;
    unsigned char *q2;

    if (__GetFlag(0x93e) != 0)
        return gScript_968__0200d508;
    if (__GetFlag(0x927) != 0)
        return L4ef0;
    v = __GetFlag(0x928);
    if (v != 0)
        return L5028;
    if (__GetFlag(0x911) != 0) {
        if (__GetFlag(0x925) != 0) {
            p = L4cf8;
            q2 = p + (0xa7 << 1);
            *q2 = v;
            q = p + (0xd7 << 1);
            w = 2;
        } else if (__GetFlag(0x922) != 0) {
            p = L4cf8;
            q = p + (0xd7 << 1);
            w = 1;
        } else {
            return L4cf8;
        }
        *q = w;
        p += 0xe3 << 1;
        *p = w;
        return L4cf8;
    }
    return L4ba8;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_943_2008950 -- 0x02008950, asm/overlays/rom_7c7b9c/ovl_30_c_a_a_a.s
 *
 * 65 vs 66 lines, 35 differing.  Candidate at scratch/L8950.c.
 * A flag dispatcher returning one of five script pointers.
 *
 * BLOCKER: gcc DERIVES a second table offset where the ROM builds both fresh.
 *      rom   mov r1,#0xa7 / lsl r1,#1 ... mov r3,#0xd7 / lsl r3,#1
 *      ours  mov r2,#0xa7 / lsl r2,#1 ... add r2, #0x60
 * 0xd7<<1 minus 0xa7<<1 is 0x60, and with the first value live in a register
 * gcc reaches the second with an `add`.
 *
 * This is the exact inverse of the case the symbol-base lever solves
 * (src/non_matching/overlays/20085a4.c, where the ROM derives and gcc will not).
 * Here gcc derives and the ROM does not, and there is no lever pointing that
 * way: naming the three offsets as separate locals in the dominating block
 * changes nothing, and neither does CSE_CFLAGS -- so the derivation is the main
 * -O2 CSE, not the rerun pass.
 *
 * Consistent with the Func_80a22f4 finding that the derived-constant machinery
 * has no per-operand granularity: it fires on whatever is in a register, and
 * the source cannot ask for it selectively in either direction.
 *
 * Worth keeping from the attempt: `v = __GetFlag(0x928); if (v) return ...;`
 * and then storing `v` later is right -- the ROM stores r5, the result register
 * of that call, as a byte it knows is zero on that path.
 *)

*/
#endif
