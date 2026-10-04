/* Func_80979a4  --  0x080979a4
 *
 * PARKED at 13 of 47.  *** 0 of 47 WITH ONE MORE PIN. ***  (batch 322, E)
 *   this body   13 of 47  (ref 47 / ours 47, pool IDENTICAL, relocations clean)
 *               tools/shimcount.py: 3 register pins -- the r3/r0/r1 operands
 *               of the `call_via_r3` veneer, inherited and load-bearing.
 *   + one pin    0 of 47  108 bytes, 47 encodings, 4 relocations identical,
 *               the ONLY change being `register int h __asm__("r4");`,
 *               i.e. 4 pins total.
 * Reported as a PARK, not a landing, under the owner's pin-free policy
 * (docs/brief-template.md "Pin policy"): adding a fourth pin to reach 0 is
 * exactly what pass 3 exists to adjudicate -- reports/pass3-depin.md.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/80979a4.c \
 *     asm/rom_8a000/rom_97384_c_c_a_a.s --func Func_80979a4
 *   For the pinned 0, change `int h;` to `register int h __asm__("r4");`
 *   and re-run the same command.  (One function in the reference file;
 *   tools/datacheck.py silent; no split, no exports.)
 *
 * ===== WHAT THE 13 ARE, PER INDEX (measured, one compile) =====
 * Indices 41-46 -- the whole literal pool -- are IDENTICAL, so NONE of the 13
 * is a pool word; all 13 are real instructions.  They are ONE three-way
 * register rotation and nothing else:
 *        value                     ROM   ours
 *        h (hue, 16.16)             r4    r2     idx 7 8 12 15 22 28
 *        0x3bffff compare constant  r2    r1     idx 6 8 23
 *        scratch (pool loads,       r1    r4     idx 14 16 17 19 21
 *          0xf0<<15 build)
 * Every one of the 13 is explained by substituting that rotation; the pinned
 * run confirms it, because pinning h alone takes the figure to 0.
 *
 * ===== THE PARK'S "NOT REACHABLE BY ALLOCATION ORDER" CLAIM: CONFIRMED, AND
 * ===== NOW WITH THE DECIDING RUNG NAMED
 * `.18.greg` prints `;; 7 regs to allocate: 37 35 36 34 40 38 33`, so this IS
 * global-alloc and `global.c:allocno_compare` is the right formula.  (Checked
 * per the brief's free discriminator -- N is 7, not 0.  But see FINDINGS.md:
 * local-alloc's QTY_CMP_PRI has floor_log2 TOO; the two formulae differ in
 * their DENOMINATOR, not in floor_log2.)
 *
 * The pseudos, read off .17.lreg, with pri = floor_log2(refs)*refs/len*10000:
 *      37 res  6 refs / 14  -> 8571     34 hi  3 refs / 11 -> 2727
 *      35 h    6 refs / 15  -> 8000     40 K   3 refs / 14 -> 2142
 *      36 m    4 refs / 10  -> 8000     38 g   4 refs / 40 -> 2000
 *                                       33 lo  2 refs / 15 -> 1333
 * Sorted descending with allocno_compare's `v1 - v2` tie-break (35 beats 36 on
 * allocno number, not on priority) this is EXACTLY the printed order.  The
 * formula reproduces the dump; nothing here is inference.
 *
 * *** THE RUNG IS global.c:find_reg's `used1`, NOT the ordering. ***  h (35)
 * does not cross a call, so `used1` = fixed_reg_set | ~LO_REGS |
 * allocno[35].hard_reg_conflicts, and the dump gives those hard conflicts as
 * {0, 1, 3, 13}.  find_reg runs TWO passes: pass 0 also excludes
 * ~regs_used_so_far and `regs_someone_prefers` (res prefers r0 AND r2, and res
 * conflicts with h, so r2 is excluded in pass 0 -- which is why pass 0 finds
 * nothing); *** pass 1 drops both of those and walks REG_ALLOC_ORDER
 * {3,2,1,0,12,14,4,...} against used1 alone, so it takes r2. ***
 * For h to reach r4, r2 must be in used1 -- i.e. **r2 must appear in h's
 * hard_reg_conflicts**, which means hard r2 must be LIVE somewhere inside h's
 * range.  The park's sentence "something must occupy hard r2 across the hue's
 * live range" is therefore right, and this is the line of compiler that makes
 * it right.
 *
 * ===== THE PARK'S ORDER ARITHMETIC IS ALSO RIGHT, AND HERE IS THE MARGIN =====
 * The other route is for the constant (40) to be allocated BEFORE h, because
 * 40 conflicts with 35 and nothing stops 40 taking r2.  40 needs
 * floor_log2(n)*n/L > 0.8.  With its real 3 refs that needs L < 3.75 against an
 * actual 14, and its two uses are 15 insns apart.  Raising refs: n=4 gives
 * 0.571, n=5 gives 0.714, and only n=6 (0.857) clears -- three more references
 * to 0x3bffff than the ROM's stream contains.
 * The nearer miss is `hi` (34), which ALREADY has a copy preference for r2
 * (insn 8 is `(set (reg 34) (reg:SI 2 r2))`) and already conflicts with h, and
 * whose allocno number is BELOW h's so a mere TIE at 8000 would put it first.
 * It needs 4 refs at L<=10, or 5 refs at L<=12, against its actual 3 at 11.
 *
 * *** AND THAT IS WHERE A SOURCE SPELLING CANNOT REACH, FOR A NEW REASON. ***
 * Two spellings that add a C-level reference to `hi` -- arm 3 calling
 * `call_via_r3(hi, ...)` instead of `(res, ...)`, and a redundant `hi = hi;` --
 * both produce `.17.lreg` dumps with **bit-identical allocator inputs**: 34
 * still 3 refs / 11 insns, 35 still 6 / 15, the allocation order still
 * `37 35 36 34 40 38 33`.  On the arm-3 path `res == hi`, so copy propagation
 * merges the two names into one pseudo before local-alloc ever runs.  **You
 * cannot raise `hi`'s ref count from C on a path where `res` already equals
 * it.**  (Read per the brief's correction #4: a flat figure with bit-identical
 * allocator inputs means the EDIT did not reach the allocator, not that the
 * lever is inert.)
 *
 * ===== THE PARK'S OWN DIAGNOSTICS, RE-CHECKED =====
 * STILL TRUE: REG_ALLOC_ORDER is {3,2,1,0,12,14,4,5,...} (arm.h:989-995);
 * -fno-expensive-optimizations is affirmatively wrong here (it pools the
 * cheap two-instruction constant build) and no flag group should be added;
 * the [offset] marker was noise (no memory access in the function at all);
 * the veneer, the single-`res` shape and the first-arm-outside-the-else-chain
 * shape are all load-bearing and reproduce.
 * CORRECTED: the park computes the hue at 0.8 and the constant at 0.214 and
 * calls that a factor of four.  Both numbers are right, but they are not the
 * pair that decides -- `hi` at 0.2727 with a standing r2 copy preference and a
 * lower allocno number is the nearest competitor, and it needs a TIE, not a
 * win.  Quote `hi`, not the constant, as the margin to beat.
 * NOT RE-WALKED: the park's own r2-pin diagnostic (17 regions -> 8).  The
 * cheaper instrument is the h/r4 pin, which reads 0 and so proves there is
 * nothing else behind the rotation.
 *
 * ===== MEASURED, all pin-free unless marked, all at exact count 47 =====
 *    0  PIN `register int h __asm__("r4")`           <- closes everything
 *   13  BASE
 *   13  PIN `register int h __asm__("r2")`           (exactly inert: already r2)
 *   13  arm 3 calling `call_via_r3(hi, ...)`         (inert; see above)
 *   13  a redundant `hi = hi;` in arm 1              (inert; deleted pre-lreg)
 *   13  `{int t = h; h = t;}` after the first call   (inert; coalesced)
 *   13  `{int d = 0xf00000 - h; ...}` hoisted        (inert)
 *   13  `"r2"` added to the veneer's clobber list    (inert -- h is NOT live
 *       ACROSS the asm, it is consumed INTO r1 just before it, so a clobber
 *       cannot give it an r2 conflict.  This is the alloc-side companion to
 *       the batch-321 bound that a clobber list cannot narrow scheduling deps.)
 *   13  every pairwise cross of the five inert edits above (15 rows)
 *   34  hoisting `res = hi;` above the first `if` (49 insns, RELOC: the pool
 *       order moves, so the figure is not a distance)
 *
 * ===== WHAT TO TRY NEXT, NAMED AND NARROW =====
 * Put hard r2 inside h's live range without spending an instruction.  h is
 * born from `Func_8097a10`'s return and dies into the veneer's r1, and in that
 * window the ROM writes r0, r1 and r3 but NEVER r2 -- r2 holds the constant.
 * So the question is whether any callee reachable here takes a third argument
 * that is ALREADY in r2 at the call site, or whether the veneer in the ROM's
 * own build had a fourth operand.  If neither, this park is closed to source
 * and the h/r4 pin is the answer; record it in reports/pass3-depin.md rather
 * than sweeping spellings, because sixteen unrelated spellings (the park's)
 * plus six more (this batch's) all tie at exactly 13.
 *
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
