/* SystemMsgBox (0x080208e4) -- NON-MATCHING, 2 of 85 encodings.
 *
 * SIZE 204 == 204, ENCODINGS 85 == 85, 16 RELOCATIONS IDENTICAL, so the 2 is a
 * TRUE DISTANCE, not a misalignment.  (The park this replaces,
 * src/non_matching/rom_15000/80208e4.c, measures 23 of 85 -- re-derived, its
 * claim reproduced.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/80208e4.c \
 *     asm/rom_15000/rom_20198_a_a_c.s --func SystemMsgBox
 *
 * SPLIT: NONE NEEDED if it ever lands.  asm/rom_15000/rom_20198_a_a_c.s holds
 * exactly ONE function (one .thumb_func_start) and tools/datacheck.py reports no
 * data section and no required exports, so the whole .s would be replaced by the
 * .c.  PINS: 0 (tools/shimcount.py reports none).
 *
 * ================== THE PARK'S DIAGNOSIS, REPRODUCED AND REFUTED =============
 * Reproduced: the length, the branch structure, the five early-exit values and
 * the whole tail are exact, and most differences are "which scratch register
 * carries a value".
 *
 * REFUTED, in three parts:
 *
 *  1. "Every difference is which scratch register carries a value."  Decomposed
 *     into runs, the 23 is FIVE runs and two of them are ORDER, not register:
 *       A  prologue, `ret = 0` into r8          2   scratch r1 vs r2
 *       B  arm 1, `-9` into r8                  3   scratch r2 vs r3
 *       C  `.L2090c`, ldrsh + arg setup         5   ORDER + the zero-index reg
 *       D  arm 2, call args + `-2` into r8      5   ORDER (2) + scratch (3)
 *       E  `.L2095c`, five pooled addresses     8   local-alloc choices
 *     Block `.L2092c` (the two Func_8005a78 calls) is exact.
 *
 *  2. "WHAT IS RIGHT AND SHOULD BE KEPT: ... `g = gState;` with `g[0x22a]`."
 *     That local is the ENTIRE CAUSE OF RUNS A, B, C AND D.  Deleting it and
 *     reaching gState directly takes 23 -> 5 in one edit.
 *
 *  3. "No recorded lever addresses that choice ... NEXT: nothing source-level."
 *     The register is not spellable, but what picks it is, and it is not
 *     REG_ALLOC_ORDER.
 *
 * ================== THE MECHANISM: A ROUND-ROBIN OVER THE SPILL SET ==========
 * `(set (reg:SI 8 r8) (const_int N))` has no thumb pattern -- a high register
 * cannot take an immediate -- so reload splits every assignment to `ret` into a
 * LO_REGS scratch plus `mov r8,scratch`.  `.19.flow2` shows exactly that
 * (insn 200 `set r2,0` + insn 12 `set r8,r2`).  Runs A, B and the `-2` of D are
 * those scratches.
 *
 * `allocate_reload_reg` (reload1.c:4996-5013) does NOT consult REG_ALLOC_ORDER.
 * It starts its scan at `i = last_spill_reg` and, in the compiler's own words,
 * "We advance it round-robin between insns to use all spill regs equally".
 * `last_spill_reg` is -1 per function (reload1.c:821) and is set to the chosen
 * index at reload1.c:4937.  `spill_regs` is the ASCENDING list of hard regs in
 * `used_spill_regs` (reload1.c:3527-3536).
 *
 * So the whole function's scratch pattern is one rotation, and ours was one
 * position behind the ROM's for every reload in the function:
 *     rom   r1, r2, ..., r1, r2        ours  r2, r3, ..., r2, r2
 * which is the SAME rotation over a spill set of {r1,r2,r3} versus {r2,r3}.
 * The park's compile never prints "Using reg 1" in `.18.greg`'s reload trace;
 * this one does ("Spilling for insn 129. Using reg 1 for reload 0").
 *
 * WHICH REGISTERS JOIN THE SET is `find_reg` (reload1.c:1588-1659): candidates
 * are ranked by `spill_cost[regno]`, the summed `REG_N_REFS` of pseudos
 * allocated to that hard reg and live across the insn (`count_pseudo`, 1490),
 * and ties go to `inv_reg_alloc_order`, which on ARM is 3, 2, 1, 0
 * (config/arm/arm.h:989-995).  The decisive insn is the reload that materialises
 * 0x22a for `add r3, r3, #0x22a`: with the `iwram_3001d08` store address still
 * live in r2 at that point, r1 is the only zero-cost candidate and joins the
 * set; with it dead, r1 and r2 both cost 0 and the tie-break takes r2.
 *
 * SO THE LEVER IS NOT THE REGISTER, IT IS HOW MANY REFS ARE LIVE ACROSS ONE
 * RELOAD -- and that is ordinary source.  Worth 15 of the 23 here.
 *
 * ================== WHY THE STRUCT, AND WHAT IT IS WORTH =====================
 * Reaching gState as `*(int *)(gState + 4)` folds the displacement INTO the
 * pool word: gcc emits `.word gState+4`, `ldr r2,[r3]` and `.word 0x226`.  The
 * ROM has `.word gState`, `ldr r2,[r3,#4]` and `.word 0x22a` -- three
 * encodings.  A named `unsigned char *g` fixes that (it is what the landed
 * sibling src/rom_15000/rom_23178_a_c_b.c does, and its header records the same
 * folding trap) but costs the rotation above.  A STRUCT does both: a
 * COMPONENT_REF keeps the base in a register and the member offset as a MEM
 * displacement, with no user pseudo to perturb local-alloc.  Worth 5 -> 2.
 * The `typedef struct { unsigned char _bytes[704]; } GlobalState;` spelling is
 * already the project's convention for this symbol in a dozen parks; this is
 * the same object with the two fields this function touches named.
 *
 * ================== THE REMAINING 2, AND WHY IT IS NOT SPELLABLE =============
 * Both are one swap:
 *     rom    add r3,r1 / ldrb r3,[r3] / ldr r2,=iwram_3001d08 / strb r3,[r2]
 *     ours   add r3,r1 / ldr r2,=iwram_3001d08 / ldrb r3,[r3] / strb r3,[r2]
 *
 * `expand_assignment` (expr.c:3639-3647, "Ordinary treatment.  Expand TO to get
 * a REG or MEM rtx") expands the LHS address FIRST for a plain VAR_DECL LHS, so
 * the pool load always has the lower INSN_LUID.  Its one documented exception is
 * a CALL_EXPR right-hand side (expr.c:3594-3614, "If the rhs is a function call
 * ... call the function before we start to compute the lhs"), which does not
 * apply.
 *
 * sched2 then has both insns ready in the same cycle -- `.23.sched2` prints
 * `Ready list (t = 11):    131  121` -- and EVERY RUNG of `rank_for_schedule`
 * (haifa-sched.c:4029-4116) ties:
 *   priority        7 == 7
 *   INSN_REG_WEIGHT gated off by `!reload_completed` (line 4046)
 *   CLASS           both 3: the ldrb's dependence on the `add` has
 *                   `insn_cost == 1`, which line 4078 maps to class 3, and the
 *                   pool load is independent of it, which is also class 3
 *   dependents      2 == 2  (121 -> {214 anti-r2, 132}; 131 -> {135 anti-r3, 132})
 * so line 4115's `INSN_LUID` tie-break decides, and it decides against us.
 *
 * THE TWO FIXES ARE MUTUALLY EXCLUSIVE THROUGH THE SPILL SET, and this is the
 * finding worth carrying.  The only source form that lowers the ldrb's LUID is
 * a separate statement:
 *     b = gState.f22a;  iwram_3001d08 = b;
 * That DOES produce the ROM's order -- and it also takes the store address out
 * of r2 across the 0x22a reload, so `find_reg` ties and r1 never joins the
 * spill set, and all 15 rotation differences return.  Measured: 20 of 85.
 * Eight further spellings of the split and all six permutations of the tail
 * statements were measured; none beats 2 (figures 2, 6, 8, 10, 15, 19, 20, 23,
 * 25, 26, 28).
 *
 * NEXT MOVE: find a value that is live in r2 across the `add r3,r3,#0x22a`
 * WITHOUT being the store address -- that would re-admit r1 to the spill set
 * while leaving the ldrb's LUID low.  Nothing in the ROM's register use suggests
 * one, which is why this is a park and not a landing.
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
