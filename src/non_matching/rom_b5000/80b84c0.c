/* Func_80b84c0 -- asm/rom_b5000/rom_b8228_c_a_c_a_a_c_a_a.s
 *
 * 24 of 49 stream encodings, 39 instructions against the ROM's 41 -- TWO SHORT,
 * so the 24 is PHASE.  Position-tolerant: 42 of 49 aligned-equal (85.7%),
 * 10 differing in 6 hunks.  2 register pins (the .call_via helper) -> fakematch
 * class.  THIS PARK NOW HAS A BODY; it was prose only until batch 338, and its
 * `Best C:` (scratch/agent4/sb84c0_k.c) measured 24 of 49 aligned-equal (49.0%).
 *
 * REFERENCE REPOINTED.  The park named asm/rom_b5000/rom_b8228_c_a_c_a_a_c.s,
 * which no longer exists -- two later splits consumed it.  Func_80b84c0 is in
 * rom_b8228_c_a_c_a_a_c_a_a.s.  That is why tools/piece.py reported
 * "could not resolve a reference .s that exists on disk" for this park.
 *
 * Verify with:
 *     python3 tools/aligncmp.py src/non_matching/rom_b5000/80b84c0.c \
 *         asm/rom_b5000/rom_b8228_c_a_c_a_a_c_a_a.s Func_80b84c0 -v
 *     python3 tools/objcmp.py src/non_matching/rom_b5000/80b84c0.c \
 *         asm/rom_b5000/rom_b8228_c_a_c_a_a_c_a_a.s --func Func_80b84c0
 *
 * ---- batch 338, brief E: the register-binding half is REFUTED ----
 *
 * The park prescribed binding the callee at FUNCTION scope
 * (`register int (*f)(int,int) __asm__("r5")`).  That binding is what costs the
 * FIFTH callee-saved register: gcc-2.96 reserves an asm-bound register for the
 * whole function, so r5 is unavailable for the actor pointer, and `q` is pushed
 * up to r10 -- `push {r6, r7}` and `mov sl, r1` in the prologue, where the ROM
 * has only `push {r7}`.  The ROM instead holds the actor pointer in r5 and then
 * REUSES r5 for the callee once the pointer dies.
 *
 * BLOCK-SCOPING THE BINDING DOES NOT HELP.  Declaring it inside a `{ }` that
 * begins after PhysMove is byte-identical to the function-scope form -- measured,
 * five variants, all 24 aligned.  gcc reserves the hard register for the
 * variable's whole FUNCTION, not its block.  That is worth knowing for the whole
 * .call_via class, because the binding is the class's standard template.
 *
 * WHAT PAYS INSTEAD (24 -> 42 aligned), two independent levers:
 *
 *  1. NAME THE CALLEE BY ASM OPERAND, not by register binding: `bx %1` with the
 *     pointer as an "r" input.  gcc then allocates it freely and can reuse a
 *     register whose earlier value has died.  Worth 6 (24 -> 30).
 *  2. SPLIT THE REUSED RESULT LOCAL.  The park used one `v` for the PhysMove
 *     result and both call results; three locals (v0/v1/v2) let the first call's
 *     result take its own range.  Worth 12 (30 -> 42).  Note this is the exact
 *     INVERSE of what landed Func_80b8824 this batch -- role reuse there, role
 *     splitting here.  Neither is a rule; both are levers.
 *
 * WHAT IS LEFT, 10 differing in 6 hunks:
 *   * gcc puts the callee in r7 and `q` in r10; the ROM shares r5 between the
 *     actor pointer and the callee.  The ranges genuinely do not overlap, so
 *     this is an allocno-ORDER question (global.c `allocno_compare`), not a
 *     conflict: `q` has 3 refs over the longest range in the function and is
 *     allocated last.
 *   * PhysMove's argument setup order: ROM `mov r1, r7 / mov r0, r5`, ours the
 *     reverse.
 *   * The ROM duplicates `adds r0, r6, #0` into BOTH arms of the `if` and keeps
 *     ONE `.call_via` at the join.  Ours hoists `movs r1, #24` above the `cmp`
 *     and inverts the branch.
 *
 * THE PARK'S "a volatile asm is NOT cross-jumped" SURVIVES, and it is what
 * blocks the arms.  Writing the call in both arms raises aligned-equal to 45 of
 * 49 (23 of 51 positional) because the arms' operand setup then matches -- but
 * it ships TWO expansions, 43 instructions against 41, so it is two too many
 * instead of two too few.  That variant is kept at v4d/P_call_both_arms.c in the
 * batch scratch; it is a better aligned number on a worse structure.
 *
 * RECORDED INERT at 42 aligned: `"r2"` added to the clobber list; `"r2","r3"`;
 * a ternary for `k`; inverting the test; selecting the first asm operand into
 * its own local in both arms (three placements).  AT 30: declaring the callee
 * local last; two separate callee locals.  The r2-clobber lever that landed
 * Func_801cc50's register roles this batch is INERT here -- this function's
 * values that cross the call sites are all already callee-saved.
 */
extern int Func_8000888(int a, int b);
extern unsigned char **GetBattleActor(int id);
extern int Func_80b7f70(unsigned char *p, int n);
extern int Func_80b7ed8(void);
extern int PhysMove(unsigned char *p, int *q);
extern unsigned char *_GetUnit(int id);
extern int Func_80c23c0(int n);

/* No hard-register binding for the callee: the asm names it by OPERAND, so gcc
 * is free to allocate it -- and free to reuse a register whose earlier value
 * has died.  A function-scope `register ... __asm__("r5")` reserves r5 for the
 * whole function and forces everything else up into r10. */
#define CALL_VIA(res, ff, aa, bb) do {                                   \
    register int _a __asm__("r0") = (aa);                                \
    register int _b __asm__("r1") = (bb);                                \
    __asm__ volatile ("\t.align\t2, 0\n\tmov\tr12, pc\n\tbx\t%1"         \
        : "=r" (_a) : "r" (ff), "0" (_a), "r" (_b)                       \
        : "memory", "lr", "r12");                                        \
    (res) = _a;                                                          \
} while (0)

int Func_80b84c0(int unit, int *q)
{
    int (*f)(int, int);
    unsigned char *s;
    int *m;
    int v0, v1, v2;
    int k;

    s = *GetBattleActor(unit);
    m = (int *)Func_80b7f70(s, 0);
    s += 8;
    Func_80b7ed8();
    v0 = PhysMove(s, q);
    f = Func_8000888;
    CALL_VIA(v1, f, v0, m[6]);
    if (Func_80c23c0(_GetUnit(unit)[0x94 << 1]))
        k = 0x18;
    else
        k = 0x30;
    CALL_VIA(v2, f, v1, k);
    q[1] -= v2;
    return 0;
}
