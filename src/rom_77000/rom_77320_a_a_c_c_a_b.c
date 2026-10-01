/* ===================== BATCH 316c -- GetUnit: BYTE-IDENTICAL =====================
 * 16 of 31 -> 0 of 31.  68 bytes, 31 encodings, 2 relocations identical.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_77000/rom_77320_a_a_c_c_a_b.c \
 *     asm/rom_77000/rom_77320_a_a_c_c_a.s --func GetUnit
 *
 * LANDING SHAPE: a TEXT-ONLY split of asm/rom_77000/rom_77320_a_a_c_c_a.s.
 * tools/datacheck.py prints nothing -- no data section.  split_s.py --dry-run:
 *   _a.s keeps Func_8077348 (45 lines), _b.s is GetUnit (40 lines), no _c.s.
 * Installed path is src/rom_77000/rom_77320_a_a_c_c_a_b.c.  No new export.
 *
 * SHIM: ONE.  The `__asm__ __volatile__("mov %0, lr" : "=l"(t))` below.  It needs
 * a fakematch.txt row.  tools/shimcount.py reports zero because this is a FOURTH
 * shim class it does not count: not a `register ... __asm__` pin, not an
 * `__asm__(".equ ...)` symbol declaration, not an empty `__asm__ volatile("")`.
 * Worth adding to shimcount.
 *
 * ---------------------------------------------------------------------------
 * THE PARK'S DIAGNOSIS WAS WRONG IN A SPECIFIC, MEASURABLE WAY.
 *
 * The park said "ALL 16 ARE ONE PERMUTATION" and that the permutation is FORCED
 * BY the dead `mov r3, r14` ("Without it gcc has no reason to reserve r3").
 * Both halves are false.  There are TWO independent facts and the permutation is
 * fully source-reachable without the dead move:
 *
 *   1. The ROM's register assignment -- id in r0, base/p in r2, multiplier in r3
 *      -- is reached by source alone.  It is NOT caused by the dead move.
 *   2. The dead `mov r3, r14` is a separate, single, source-UNREACHABLE
 *      instruction.  Measured on the shape where it is the ONLY thing missing.
 *
 * WHAT ACTUALLY CAUSED THE PERMUTATION: local-alloc's dest/dying-source combine.
 *
 * In the park's source the multiplier is one function-scope `k` written
 * `k = 0x14c; k *= id;`.  Thumb's `mul` is destructive, so `*thumb_mulsi3`
 * carries a `%0` matching constraint and local-alloc ties the constant's
 * quantity to the product's.  Then the final `res = k + base` has its
 * destination combined with its DYING SOURCE (the product) -- and that
 * destination is the returned value, which carries a copy-suggestion for r0
 * from `(set (reg 0 r0) res)`.  So the merged constant/product/sum quantity
 * takes r0, `id` is pushed out of r0 to r2, and `base` takes r3.  That is the
 * whole 16.
 *
 * .18.greg prints it directly, and lever 2's priority formula reproduces the
 * order exactly (floor_log2(refs)*refs/live):
 *      park:  k  refs=8 live=16 prio=1.500   <-- allocated FIRST, prefs {0,3}, takes 0
 *             id refs=5 live=12 prio=0.833   <-- then r2, because r0 is gone
 *      ours:  id refs=5 live=13 prio=0.769   <-- the only globals left are id,
 *             p  refs=3 live=5  prio=0.600       base, p; id takes r0 by its
 *             base refs=2 live=10 prio=0.200      copy preference, p/base take r2
 *
 * TWO SOURCE CHANGES, EACH A CLEAR REGRESSION ALONE, TOGETHER THE WHOLE FIGURE:
 *
 *   (a) the multiplier becomes a BLOCK-SCOPED local per arm, so it is a
 *       single-block pseudo that local-alloc handles and REG_ALLOC_ORDER
 *       (3, 2, 1, 0) hands r3;
 *   (b) the result becomes a FUNCTION-SCOPE `int res`, so it is a multi-block
 *       pseudo that local-alloc cannot combine with the multiplier at all --
 *       which is what stops the drag to r0.
 *
 * Alone, (a) measures 16 -- byte-for-byte the park's own output, which is why the
 * park recorded "two separate locals k and k2" as inert.  Alone, (b) measures 15 -- better than the park, and still useless alone.
 * Together they measure 27 by objcmp's positional count -- LOOKS LIKE A
 * REGRESSION -- because the stream is then 27 instructions against the ROM's 28
 * and every index after 0 shifts.  Structurally it is 27 of 27 instructions
 * EXACT; the only thing missing is the dead move.  Add the dead move and it is 0.
 *
 * This is exactly the brief's "edits each a clear regression, jointly a gain"
 * shape, and one-at-a-time testing cannot find it: the park measured (a) and
 * never measured (a)+(b).
 *
 * THE DEAD MOVE IS GENUINELY UNREACHABLE, AND THIS IS A MUCH STRONGER PROOF
 * THAN THE PARK'S, because it is measured on a stream where that instruction is
 * the ONLY difference.  All of these are byte-for-byte the 27-instruction
 * output -- the copy is deleted by flow1 as a dead store every time:
 *      register int u __asm__("lr");  res = u;                 27
 *      register int u __asm__("lr");  int t;  t = u;            27
 *      register int u __asm__("lr");  register int t __asm__("r3");  t = u;   27
 *      int u;  res = u;                                         27
 *      int u; int t;  t = u;  res = t;                          27
 *      void *ra = __builtin_return_address(0);  res = (int)ra;  27
 * `register volatile int u __asm__("lr")` is WORSE, not better: the volatile
 * forces a 4-byte frame (`sub sp, #4` / `add sp, #4`) and the copy is STILL
 * dropped (11 differing, size +4, relocations differ).
 *
 * WHY none of them can work: the destination of the ROM's copy is dead on every
 * path, and gcc-2.96's flow1 life analysis deletes a dead store to a pseudo
 * before local-alloc ever sees it.  The only destinations flow1 refuses to
 * delete are global_regs, fixed regs and side-effecting insns -- a file-scope
 * `register int x __asm__("r3")` would make r3 global and therefore NOT
 * allocatable, which contradicts `mov r3, #0xa6` three instructions later.  So
 * the instruction has to be transcribed, as the r9 static-chain class is
 * (docs/elevation.md "A read of r9 with no defining write is a STATIC CHAIN").
 *
 * A NEW, MEASURED COST OF A HARD-REGISTER PIN -- worth the method document.
 * `register int k __asm__("r3")` plus the transcribed move gets EVERY
 * instruction right except the tail, and reads 17 rather than 0, because the pin
 * makes both arms' final adds IDENTICAL IN EARLY RTL (`add r3, r3, r2` twice)
 * and jump optimisation CROSS-JUMPS them into one shared block, costing a
 * `mov r0, r3`.  The ROM's two arms both end `add r0, r3, r2 / b` and are NOT
 * merged -- because before allocation they are different pseudos.  So a pin can
 * lose a function by ENABLING a cross-jump the ROM does not have.  The park
 * recorded the `add r3,r3,r2 / mov r0,r3` symptom and blamed "the pin is too
 * strong to release r3 for the result"; the cause is the cross-jump.
 *
 * ALSO MEASURED, ALL INERT OR WORSE (all on the correct shape):
 *   `id * c`, `0x14c * id`, `id * 0x14c`, `int c; int k = c * id;`,
 *   `int c; int k = c; k *= id;`, `(int)base + c * id`  -- all 27.  The MULT's
 *   operand order is fixed AT EXPAND: .00.rtl already holds `(mult id c)`
 *   whichever way the C is written, so `-fno-regmove` is inert (6, unchanged),
 *   as are `-fno-gcse`, `-fno-cse-follow-jumps`, `-fno-expensive-optimizations`.
 *   Only the compound-assignment form `k *= id` keeps the constant as operand 1,
 *   and with (a)+(b) in place BOTH spellings reach 0 -- `c * id` and `k *= id`
 *   are byte-identical here, so the spelling is free and this file uses the
 *   clearer one.
 *   A named result with a SINGLE `return` at the end: 22-25, worse.
 *
 * -- scratch_elev/b316c/v_p1, v_p1b, v_p1d, v_p1e, v_p1f, v_p1g, v_p1h
 */

/* GetUnit  --  0x08077394
 *
 * Maps a unit id to a 0x14c-byte record.  Ids 0..7 index gPartyStatus directly.
 * Ids 0x80..0x85 index the block at iwram_3001f28, biased by -0xa600
 * (= 0x80 * 0x14c) so the same multiply serves both ranges.  Anything else, or
 * a null block pointer, returns 0.  Both range tests are UNSIGNED -- the ROM
 * uses bhi, not bgt, and the pooled -0xa600 (0xffff5a00, not -0x2a600) settles
 * that the bias is applied after the multiply rather than to the id.
 */

extern unsigned char gPartyStatus[];
extern char *iwram_3001f28;

void *GetUnit(unsigned int id)
{
    int t;
    int res;
    char *base;
    char *p;

    __asm__ __volatile__("mov %0, lr" : "=l"(t));
    base = gPartyStatus;
    if (id <= 7) {
        int k = 0x14c;
        k *= id;
        res = k + (int)base;
        return (void *)res;
    }
    if (id - 0x80 <= 5) {
        p = iwram_3001f28;
        if (p != 0) {
            int k = 0x14c;
            k *= id;
            res = (int)(p + k) - 0xa600;
            return (void *)res;
        }
    }
    return 0;
}
