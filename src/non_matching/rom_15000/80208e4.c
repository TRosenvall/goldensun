/* SystemMsgBox (0x080208e4) -- NON-MATCHING, 2 differing encodings of 85.
 *
 * SIZE 204 == 204, ENCODINGS 85 == 85, 16 RELOCATIONS IDENTICAL, so the 2 is a
 * TRUE DISTANCE, not a misalignment.  PINS: 0 (tools/shimcount.py).
 * Figure re-derived in batch 325; the previous header's claim reproduced exactly.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/80208e4.c asm/rom_15000/rom_20198_a_a_c.s --func SystemMsgBox
 *
 * SPLIT: NONE NEEDED if it ever lands.  asm/rom_15000/rom_20198_a_a_c.s holds
 * exactly ONE function (one .thumb_func_start) and tools/datacheck.py reports no
 * data section and no required exports, so the whole .s would be replaced.
 *
 * ================== THE RESIDUE, UNCHANGED ==================================
 * Both differing encodings are one swap:
 *     rom    add r3,r1 / ldrb r3,[r3] / ldr r2,=iwram_3001d08 / strb r3,[r2]
 *     ours   add r3,r1 / ldr r2,=iwram_3001d08 / ldrb r3,[r3] / strb r3,[r2]
 * `.23.sched2` block 6 prints `Ready list (t = 11):  139  129` and schedules 129.
 *   129 = `r2=[*.LC6]`, the iwram_3001d08 pool load
 *   139 = `r3=[r3]`,    the ldrb
 * `expand_assignment` expands the LHS address first for a plain VAR_DECL LHS
 * (expr.c:3639-3647, its one exception gated on CALL_EXPR at expr.c:3604), so
 * LUID(129) < LUID(139).
 *
 * ================== BATCH 325: THE TIE IS TIED IN THE ROM TOO ===============
 * Every rung of `rank_for_schedule` (haifa-sched.c:4029-4116) ties, and -- this
 * is the new part -- EACH TIE IS STRUCTURAL IN THE ROM'S OWN CODE, so the ROM's
 * order can only have come from INSN_LUID:
 *   priority        7 == 7, and both 7s come from the SAME shared dependent 140
 *                   (the strb): prio(140) + insn_cost = 5 + 2.  The other edges
 *                   contribute nothing, because ARM's ADJUST_COST returns 0 for
 *                   REG_DEP_ANTI and REG_DEP_OUTPUT (arm.c:2425-2427).
 *   INSN_REG_WEIGHT gated off by `!reload_completed` (line 4046)
 *   CLASS           both 3 (last_scheduled_insn is 137 and
 *                   insn_cost(137,link,139) == 1, which line 4078 maps to 3)
 *   dependents      2 == 2, and the ROM HAS BOTH SECOND EDGES:
 *                     129 -> {140, 222 `mov r2,r8`}   -- ROM writes r2 later
 *                     139 -> {140, 143 `ldr r3,=d24`} -- ROM writes r3 later
 * so neither the priority rung nor the dependent-count rung can be moved by any
 * body that still emits the ROM's instructions.
 *
 * ================== THE "MUTUALLY EXCLUSIVE" CLAIM IS REFUTED ===============
 * The previous header's finding was: the only source form that lowers the ldrb's
 * LUID is a separate statement `b = gState.f22a; iwram_3001d08 = b;`, and that
 * "takes the store address out of r2 across the 0x22a reload, so find_reg ties
 * and r1 never joins the spill set, and all 15 rotation differences return.
 * Measured: 20 of 85."  The 20 reproduces.  The EXCLUSION does not.
 *
 *   v = gState.f4;
 *   b = gState.f22a;
 *   iwram_3001c9c = v;
 *   iwram_3001d08 = b;
 *
 * measures **8 of 85**, first diff moved from index 4 to index 56, and
 * `.18.greg` prints `Using reg 1 for reload 0` again -- the spill set is
 * {r1,r2,r3} and EVERY ONE of the ~15 rotation differences is gone, WITH the
 * ldrb below the pool load.  `v`'s pseudo sits in r2 and is live across the
 * `add r3,r3,#0x22a`, which is exactly what the previous header's NEXT MOVE
 * asked for ("find a value that is live in r2 across the add WITHOUT being the
 * store address").  It exists: it is gState.f4's value.
 *
 * Mechanism, read in the compiler.  `order_regs_for_reload` (reload1.c:1517)
 * fills spill_cost[] by `count_pseudo` (reload1.c:1490, `spill_cost[r] +=
 * REG_N_REFS (reg)`) over live_throughout + dead_or_set, and `bad_spill_regs`
 * takes only the HARD registers out of those sets -- `reg_set_to_hard_reg_set`
 * (flow.c:7781) RETURNS at the first pseudo.  `find_reg` (reload1.c:1589) scans
 * ascending, lowest spill_cost wins, and ties go to the lower
 * `inv_reg_alloc_order`, which on ARM is 3,2,1,0 (arm.h:989-995).
 * > SO r3 BEATS r2 BEATS r1 BEATS r0 ON EVERY TIE, and r1 can only be chosen at
 * > a reload where r3 AND r2 BOTH carry cost.  In the park body r3 carries the
 * > gState pseudo and r2 the iwram_3001d08 address pseudo; that is the only such
 * > reload in the function, which is why removing the r2 cost loses r1 entirely.
 *
 * ================== WHY THE 8 IS STILL NOT 0 ================================
 * The 8 is a DIFFERENT defect.  With iwram_3001c9c assigned in statement 3 its
 * LHS pool load has a high LUID and a short chain, so sched2 drops the
 * `ldr =iwram_3001c9c / str` pair BELOW the ldrb and the pool words for 0x22a
 * and iwram_3001c9c swap (objcmp prints a RELOCATIONS line: iwram_3001c9c at
 * 0xc0 against the ROM's 0xbc).  In the park body that pair is pinned high only
 * by the anti-dependence 126 -> 219, both of which use r1.
 *
 * And the two requirements are geometrically opposed: the ldrb must be below the
 * pool load in LUID, so the pool-load pseudo must be born AFTER the ldrb, while
 * the r2 cost must be live ACROSS the add, which is BEFORE the ldrb.  Only a
 * THIRD value can satisfy both, and the only third value available --
 * gState.f4's -- drags its own store down with it.
 *
 * REMAINING CAUSE, NAMED: a pseudo in r2 live across `add r3,r3,#0x22a` whose
 * death does NOT force the iwram_3001c9c store below that add.
 *
 * ================== MEASURED IN BATCH 325 (none beats 2) ====================
 *   park body (base)                                                    2
 *   `d = &iwram_3001d08;` then `b = gState.f22a;` then `*d = b;`        2  <- inert
 *   + `v` split (above)                                                 8
 *   + `v` split with `unsigned int v`                                   8
 *   + `v` split with `short v`                                         27
 *   + `v` split and `c = &iwram_3001c9c;` hoisted to the front          9
 *   + `v` split and `c` hoisted after `v`                              10
 *   `c = &iwram_3001c9c; b = ...; *c = gState.f4; iwram_3001d08 = b;`  10
 *   `b` split alone, `unsigned char b` / `int b` / reusing `int r`      20
 *   `b` split with `d = &iwram_3001d08;` AFTER the `b` statement        20
 *   `b` split, the two split statements swapped                        10
 *   `b` split + `*pw = slot;` before `iwram_3001d24 = ret;`            25
 *   `b` split with the ldrb statement above the c9c statement          26
 *   tail permutation c9c, d08, pw, d24                                  6
 *   tail permutations c9c,d24,d08,pw / c9c,d24,pw,d08 / c9c,pw,d24,d08 15, reloc
 *   tail permutation c9c, pw, d08, d24                                 28
 * The three 15s all lift iwram_3001d24's pool word above iwram_3001d08's, which
 * is why they carry a RELOCATIONS line -- pool order is first-reference order
 * (push_minipool_fix, arm.c:4820).
 *
 * KEPT, and still load-bearing: the GlobalState struct (a COMPONENT_REF keeps
 * the base in a register and the member offset as a MEM displacement, where
 * `*(int *)(gState + 4)` folds the displacement into the pool word and costs 3);
 * no `g` local; `pw` as a named `short *`; `buf2 = buf + 0x1000` as a named
 * local; the five early-exit values written into `ret`.
 */
extern int Func_80056cc(void);
extern int Func_801776c(int, int);
extern void Func_8005c68(void);
extern int Func_8020244(int, int);
extern int Func_8005a78(int, int);
extern void Func_8005cf8(void);
extern int _MSG_0a;
extern int _MSG_0c;
extern unsigned char ewram_2000000[];
extern short ewram_2002004;
extern int iwram_3001c9c;
extern unsigned char iwram_3001d08;
extern short iwram_3001d24;

typedef struct {
    int f0;
    int f4;
    unsigned char pad8[0x222];
    unsigned char f22a;
} GlobalState;

extern GlobalState gState;

int SystemMsgBox(int a)
{
    int ret;
    int r;
    int slot;
    int buf;
    int buf2;
    short *pw;

    ret = 0;
    r = Func_80056cc();
    if (r != 0) {
        Func_801776c((int)&_MSG_0a, 1);
        ret = -9;
    } else {
        Func_8005c68();
        pw = &ewram_2002004;
        slot = Func_8020244(*pw, a);
        if (slot == -1) {
            ret = slot;
        } else {
            buf = (int)ewram_2000000;
            r = Func_8005a78(slot, buf);
            buf2 = buf + 0x1000;
            r |= Func_8005a78(slot + 3, buf2);
            if (r != 0) {
                Func_801776c((int)&_MSG_0c, 1);
                ret = -2;
            } else {
                iwram_3001c9c = gState.f4;
                iwram_3001d08 = gState.f22a;
                iwram_3001d24 = ret;
                *pw = slot;
            }
        }
    }
    Func_8005cf8();
    return ret;
}
