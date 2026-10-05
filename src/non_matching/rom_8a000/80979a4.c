/* Func_80979a4  --  0x080979a4
 *
 * PARKED at 13 differing encodings of 47.  PIN-FREE.
 *   RE-DERIVED batch 325 brief E: ref 47 / ours 47, SIZE 108 bytes against 108,
 *   relocations clean, pool (indices 42-46) IDENTICAL, first diff at index 6.
 *   A true distance, reproduced in batches 322 and 325.
 *   tools/shimcount.py: 3 register pins -- the r3/r0/r1 operands of the
 *   call_via_r3 veneer, inherited and load-bearing.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/80979a4.c asm/rom_8a000/rom_97384_c_c_a_a.s --func Func_80979a4
 *
 * *** OWNER RULING, docs/owner-decisions.md entry 3: DEFERRED TO PASS 3. ***
 * The `register int h __asm__("r4")` body that reads 0 of 47 is NOT to be
 * proposed and the ruling is not to be re-litigated.  It would be a fourth pin
 * on a veneer that already carries three; the figure is recorded there and in
 * reports/pass3-depin.md.  THE JOB HERE IS THE PIN-FREE FIGURE.
 *
 * One function in the reference file; tools/datacheck.py silent; no split, no
 * exports.
 *
 * ========================================================================
 * BATCH 325: THE 13 ARE **ONE** CAUSE, NOT A THREE-WAY ROTATION
 * ========================================================================
 * Six runs: 6-8 (3), 12 (1), 14-17 (4), 19 (1), 21-23 (3), 28 (1).
 *
 * `.18.greg` dispositions: `35 in 2  40 in 1  43 in 3  46 in 3  37 in 0
 * 34 in 5  33 in 6`.  So the park's THIRD leg -- "scratch (pool loads,
 * 0xf0<<15 build), ROM r1, ours r4", indices 14/16/17/19/21 -- is NOT an
 * allocno at all.  `43 in 3` and `46 in 3` are the `adds r3,...` results; the
 * register that rotates there is the RELOAD for insns 71, 74 and 85, which
 * `.18.greg` reports three times as `Using reg 4 for reload 0`.
 *
 * TRIAGE (the batch-325 question about every park citing REG_ALLOC_ORDER):
 * this park is BOTH cases at once -- an allocno leg with no `Using reg` line
 * (indices 6,7,8,12,23,28) feeding a reload leg with one `Using reg` line per
 * chain (indices 14,16,17,19,21).  **And the reload leg is a CONSEQUENCE of the
 * allocno leg, not an independent cause.**  `order_regs_for_reload`
 * (reload1.c:1534) puts every hard register holding a pseudo live at the insn
 * into `bad_spill_regs`.  At insn 71 ours has r0 (res), r1 (**40**), r2
 * (**35**), r3 (43's dest), r5 and r6 all busy, so find_reg's REG_ALLOC_ORDER
 * tie-break (reload1.c:1645-1662) walks past them to r4.  With the ROM's map --
 * 35 in r4, 40 in r2 -- r1 is free at insn 71 and the same tie-break stops
 * there first.
 *
 * >>> ALL 13 ENCODINGS ARE ONE CAUSE: ALLOCNOS 35 (h) AND 40 (the 0x3bffff
 * >>> compare constant) ARE SWAPPED.  There is one thing to fix, not three.
 *
 * ========================================================================
 * THE PARK'S PASS-0/PASS-1 READING IS REFUTED.  h TAKES r2 IN **PASS 0**.
 * ========================================================================
 * The park says: "res prefers r0 AND r2, and res conflicts with h, so r2 is
 * excluded in pass 0 -- which is why pass 0 finds nothing; pass 1 drops both
 * and walks REG_ALLOC_ORDER {3,2,1,0,12,14,4,...}, so it takes r2."  Two
 * reasons that cannot be what happened, both read from global.c:
 *
 *  1. `prune_preferences`' second loop merges ONLY LOWER-PRIORITY conflicting
 *     allocnos -- `if (allocno_to_order[allocno2] > i)`.  `res` is allocno 37
 *     and `.18.greg` prints the order `37 35 36 34 40 38 33`, so 37 is
 *     allocated FIRST.  **37's preferences can never enter
 *     `regs_someone_prefers(35)`.**
 *  2. The allocno that really does carry an r2 copy preference is 34 (`hi`, the
 *     third parameter, `(set (reg 34) (reg:SI 2 r2))`) -- and
 *     `prune_preferences`' FIRST loop deletes it: for an allocno with
 *     `calls_crossed != 0` it ORs `call_used_reg_set` into `temp` before
 *     `AND_COMPL_HARD_REG_SET (hard_reg_full_preferences, temp)`, and
 *     `.17.lreg` says "Register 34 ... crosses 1 call".  r2 is call-used.
 *     **So 34's r2 preference is pruned and never reaches 35 either.**
 *
 * PROOF THAT PASS 0 IS WHERE r2 WAS TAKEN, from the dump plus the build flags.
 * Pass 0's exclusion set is `used1 | ~regs_used_so_far | regs_someone_prefers`,
 * and `regs_used_so_far` is seeded with `regs_ever_live[i] || call_used_regs[i]`
 * (global.c:390-392).  The production CFLAGS carry **-fcall-used-r4**, and
 * nothing in arm.h's CONDITIONAL_REGISTER_USAGE (arm.h:801-826) takes it back,
 * so **r4 is in regs_used_so_far from the start**.  35's hard conflicts are
 * {r0,r1,r3,sp} and its class is LO_REGS, so REG_ALLOC_ORDER offers r3
 * (conflict), r2, r1 (conflict), r0 (conflict), 12 and 14 (not LO_REGS), r4.
 * **Had r2 been excluded in pass 0 as the park says, pass 0 would have returned
 * r4 and this function would MATCH.**  It returned r2.  So r2 was not excluded,
 * pass 0 succeeded, and pass 1 never ran.
 *
 * ========================================================================
 * THE LEVER, SHARPENED
 * ========================================================================
 * **Block r2 for allocno 35 in PASS 0 and pass 0 hands it r4 by itself.**  The
 * park asked for the expensive one of the three ways:
 *   (a) r2 in 35's `hard_reg_conflicts` -- needs hard r2 LIVE inside h's range.
 *       h is born from Func_8097a10's return and dies into the veneer's r1, and
 *       in that window the ROM writes r0, r1 and r3 but never r2, so this needs
 *       a three-argument call or an r2-using asm inside the window.  EXPENSIVE.
 *   (b) r2 in `regs_someone_prefers(35)` -- needs an allocno that conflicts with
 *       35, is ranked BELOW it, does **not** cross a call (or the first pruning
 *       loop eats its preference) and has an r2 **copy** preference.  The only
 *       non-call-crossing conflicting allocno here is 40, the compare constant,
 *       and it has no copy preference at all because it is
 *       `(set (reg 40) (const_int 0x3bffff))`.  **CHEAPEST: find any spelling
 *       that makes a below-h allocno receive r2 BY COPY.**
 *   (c) an r4 copy preference on 35 itself, which find_reg's post-pass override
 *       (immediately after the two passes) takes over `best_reg`.
 * Raising 40's own priority is NOT a route: allocno_compare is
 * floor_log2(n)*n/live_length and 40 at 3 refs / 14 reads 0.214 against h's 6/15
 * = 0.8; it needs SIX refs (0.857) and the ROM's stream compares that constant
 * exactly twice.  `hi` (34) at 0.2727 cannot be raised from C either -- on the
 * arm where it could be referenced again, `res == hi`, so copy propagation
 * merges the names before local-alloc runs.
 *
 * ========================================================================
 * MEASURED BATCH 325 -- pin-free, exact count 47 unless marked.  BASE 13.
 * ========================================================================
 *   13  BASE
 *   13  veneer clobber list += "r4"                      EXACTLY INERT
 *   13  veneer clobber list += "r2","r4"                  EXACTLY INERT
 *   13  veneer clobber list += "r5"                       EXACTLY INERT
 *   13  veneer clobber list += "r2"  (reproduces the park's row)   INERT
 *   13  first test spelled `h <= 0x3bffff`                EXACTLY INERT
 *   13  range tests as `(unsigned)(h - 0x3c0000) < 0x780000` and
 *       `(unsigned)(h - 0xb40000) <= 0x3bffff`            EXACTLY INERT
 *   13  arm 1 using `res` (== hi) as the veneer's first argument   INERT
 *   14  `unsigned int h` plus the subtraction form of all three tests
 *   23  `res = hi;` hoisted above the whole if-chain (47 insns, a distance)
 *  COUNT a flat `else if` chain with `res = hi` in two arms: 41 insns against
 *       46, SIZE 96 against 108.  NOT a distance.
 *
 * THE CLOBBER ROWS ARE THE SHARPEST NEGATIVE.  Four clobber lists, one of them
 * naming r4 itself, are EXACTLY inert.  That extends the park's batch-321 bound
 * -- a clobber list cannot narrow scheduling dependences -- with its
 * allocation-side twin: **a clobber list cannot put a register into an
 * allocno's hard_reg_conflicts unless the allocno is live ACROSS the asm**, and
 * h is consumed into the veneer's r1 immediately before it.  It also measures
 * that a clobber cannot usefully reach `regs_used_so_far` here, because r4 is
 * already in it via -fcall-used-r4.
 *
 * ========================================================================
 * STILL TRUE FROM EARLIER BATCHES (re-checked, not re-derived)
 * ========================================================================
 * REG_ALLOC_ORDER is {3,2,1,0,12,14,4,5,...} (arm.h:989-995).
 * -fno-expensive-optimizations is affirmatively wrong here (it pools the cheap
 * two-instruction constant build) and no flag group should be added.
 * The [offset] marker was noise -- there is no memory access in the function.
 * The veneer, the single-`res` shape and the first-arm-outside-the-else-chain
 * shape are all load-bearing and reproduce.
 * allocno_compare reproduces `.18.greg`'s printed order exactly from
 * `.17.lreg`'s refs and lengths: 37 8571, 35 8000, 36 8000, 34 2727, 40 2142,
 * 38 2000, 33 1333, with the `v1 - v2` tie-break putting 35 before 36.
 * The .s header comment is wrong -- it describes a radius scaler.  This is hue
 * interpolation in 16.16 fixed point: fmod against 360 degrees, then four
 * ranges.  Semantics inferred from the ROM and the solved sibling.
 *
 * Source asm: goldensun/asm/rom_8a000/rom_97384_c_c_a_a.s, line 8 (only function).
 */
extern int Func_8000888(int a, int b);
extern int Func_80008ac(int a, int b);
extern int Func_8097a10(int base, int v);

static inline int call_via_r3(int a, int b)
{
    register int (*_f)(int, int) __asm__("r3") = Func_8000888;
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\tr3"
        : "=r" (_a)
        : "r" (_f), "0" (_a), "r" (_b)
        : "memory", "r12"
    );
    return _a;
}
int Func_80979a4(int a, int lo, int hi)
{
    int h;
    int m;
    int res;
    int (*g)(int, int);

    h = Func_8097a10(a, 0xb4 << 17);
    if (h < 0x3c0000) {
        m = call_via_r3(hi, h);
        g = Func_80008ac;
        res = g(0x3c0000, m);
    } else {
        res = hi;
        if (h >= 0x3c0000 && h < 0xb40000) {
            ;
        } else if (h >= 0xb40000 && h < 0xf00000) {
            m = call_via_r3(res, 0xf00000 - h);
            g = Func_80008ac;
            res = g(0x3c0000, m);
        } else {
            res = lo;
        }
    }
    return res;
}
