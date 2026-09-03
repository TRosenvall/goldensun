/* Func_80979a4  --  0x080979a4
 * asm/rom_8a000/rom_97384_c_c_a_a.s, line 8 (the file's only function).
 *
 * PARKED at 13 disagreeing of 45, ours 45 lines. Structure, instruction count,
 * instruction order, constants, branch senses and pool contents are ALL exact.
 * The entire residue is a three-way register rotation.
 *
 * BLOCKER CLASS: 2, register birth order -- but with a proof that it is not
 * reachable by any statement order, which is the point of this park.
 *
 * THE ROTATION CANNOT BE REACHED BY CHANGING ALLOCATION PRIORITY. Read from
 * -da: the .17.lreg dump gives the hue value 6 refs over a 15-insn live range
 * and the compare constant 3 refs over 14. global.c ranks allocnos by
 * floor_log2(refs)*refs/live_length, so they score 0.8 and 0.214 and the hue is
 * allocated second. find_reg then walks REG_ALLOC_ORDER; the hue conflicts with
 * hard r0, r1 and r3, so it takes r2. For the ROM's assignment it would have to
 * conflict with hard r2 as well -- and for the constant to outrank it, the
 * constant would need a live range under a quarter of the hue's, which is
 * impossible in a 45-instruction function. SOMETHING MUST OCCUPY HARD r2 ACROSS
 * THE HUE'S LIVE RANGE IN THE ORIGINAL, and no spelling of these statements
 * puts it there.
 *
 * Two probes confirm the mechanism rather than assuming it. Binding the compare
 * constant to r2 with a register variable drops the disagreeing regions from 17
 * to 8 and makes every scratch placement byte-exact with the hue correctly
 * placed -- but it breaks the third range-test fold, so it is a diagnostic and
 * not a candidate. And -fno-expensive-optimizations reaches the same assignment
 * for a different reason, by stopping CSE sharing the constant across blocks so
 * that local-alloc hands a block-local constant r2. That flag is affirmatively
 * WRONG here: it also turns the ROM's cheap two-instruction constant build and
 * unsigned branch into a pooled load and a different branch, which PROVES this
 * file is built with expensive-optimizations ON. Do not add a flag group.
 *
 * THE RULE THIS PARK PROPOSES. When the only residue is a register rotation,
 * compute floor_log2(refs)*refs/live_length for the two rotated pseudos from
 * .17.lreg. If the ROM's assignment cannot be reached by any allocation ORDER,
 * stop sweeping spellings -- the ROM's value is being pushed off a register by a
 * HARD-REGISTER CONFLICT, and the lever is whatever puts a value in that hard
 * register, not how the statement is written. Sixteen unrelated spellings tying
 * at exactly 13 here is that signature.
 *
 * FOUR THINGS THAT ARE ALREADY RIGHT and should not be re-derived.
 *
 * The [offset] marker was noise CATEGORICALLY: this function contains no memory
 * access at all -- no load, no store, no field -- so there was never anything
 * for an offset spelling to apply to.
 *
 * The veneer helper transfers verbatim from the solved sibling with the callee
 * bound inside it, and reproduces the ROM's TWO pool loads of the same symbol,
 * one per arm. So the recorded "one site means bind the symbol" rule extends to
 * N source sites that gcc cross-jumps into one emitted site. The clobber list is
 * a per-function reading, not a rule -- adding registers to it or dropping the
 * memory clobber is neutral here.
 *
 * AN EMITTED VENEER TAIL REACHED FROM TWO ARMS IS STILL TWO WRITTEN-OUT SOURCE
 * COPIES. Writing it once and jumping costs 12 instructions; writing the three
 * statements in both arms and letting gcc cross-jump is what matches. INLINE ASM
 * DOES CROSS-JUMP -- it compares by template string. That is the
 * duplicated-code rule surviving contact with inline asm, which had not been
 * established before.
 *
 * And two structural levers worth ~8 instructions each, which are separate. One
 * `res` variable with a single return beats a direct return in every arm, 48
 * lines to 45, because four returns make four independent pseudos where one
 * makes one. And the first arm must sit OUTSIDE the else-chain that assigns the
 * default: the flat chain emits one extra move. The tell is the ROM's SINGLE
 * move where the value is needed on two paths -- that means the source assigned
 * it once, before the test that separates them. The middle arm's body is then
 * EMPTY, and re-assigning the default there costs the move back.
 *
 * The .s header comment is wrong -- it describes a radius scaler. This is hue
 * interpolation in 16.16 fixed point: fmod against 360 degrees, then four
 * ranges. Semantics inferred from the ROM and the solved sibling, not verified
 * against anything else.
 *
 * ~30 spellings and 16 flags measured; all sixteen unrelated spellings tie at
 * exactly 13. Screened with tools/tryc.py --align. Not built.
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
