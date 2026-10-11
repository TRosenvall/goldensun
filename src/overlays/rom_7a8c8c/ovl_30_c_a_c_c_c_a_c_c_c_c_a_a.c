/* OvlFunc_922_2008ed8 (0x02008ed8) -- MATCHING.
 *
 * MATCHES EXACTLY.  objcmp: "OK OvlFunc_922_2008ed8 -- 88 bytes, 41 encodings
 * and 3 relocations identical", both --func and --whole.
 * Was PARKED AT 8 OF 41 ENCODINGS (38 instructions against 38, a true
 * distance), re-derived and held at the head of batch 337.
 * tools/shimcount.py prints NOTHING: zero shims, ZERO PINS, no fakematch row.
 * Production flags, no per-file Makefile adjustment.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_a_c_c_c_c_a_a.c \
 *     asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_a_c_c_c_c_a_a.s --whole
 *
 * SPLIT SHAPE: NONE NEEDED.  tools/datacheck.py prints nothing (no data
 * section) and tools/split_s.py --dry-run says
 *   "asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_a_c_c_c_c_a_a.s holds only
 *    OvlFunc_922_2008ed8 and no data; convert it directly, no split needed"
 * The reference has no `.global` of its own, so EXPORT LIST: EMPTY.
 *
 * ================== WHAT CLOSED IT (batch 337, brief C) ==================
 *
 * Three steps.  The park's own diagnosis was the obstacle, not the compiler:
 * batch 329 had concluded "ALL EIGHT differences follow from ONE choice, which
 * hard register the sprite pointer gets" and told the next reader to probe only
 * that.  THE CAUSALITY IS BACKWARDS -- the sprite pointer's register is the
 * LAST term in the chain, not the first.
 *
 * (1) 8 -> 5.  READ FROM THE DUMPS, not inferred.  `.18.greg` says
 *       ;; 3 regs to allocate: 36 57 38
 *       ;; Register dispositions: ... 37 in 0 ... 51 in 1 ...
 *     so pseudo 37, the sprite pointer, is BLOCK-LOCAL AND ABSENT from greg's
 *     allocno list (local-alloc decides it; allocno_compare never sees it) and
 *     it does NOT cross a call, so find_reg's calls_crossed exclusion
 *     (global.c:967, :992-995) is not in play either.  Both preconditions
 *     checked, both as the park guessed, and NEITHER is the blocker.  What
 *     excluded r1 is PSEUDO 51, THE ZERO STORED TO n->f55: `.17.lreg` sets it
 *     at insn 57 and kills it at insn 82, while the sprite pointer lives insn
 *     34 -> 67.  THE TWO RANGES OVERLAP.  r1 went to the zero and the pointer
 *     took r0 because r2 and r3 were busy with the byte and the mask.
 *
 * (2) 5 -> 2, AND THIS IS THE REUSABLE MECHANISM.  A PLAIN `unsigned char`
 *     STRUCT-MEMBER STORE EXPANDS AS A READ-MODIFY-WRITE INSERT.  `.00.rtl`
 *     for `s->f9 = <expr>;` contains
 *       (insn 62 (set (reg:QI 53) (const_int 0)))          <- ~fieldmask, ZERO
 *       (insn 64 (set (reg:SI 55) (subreg (reg:QI 53))))
 *       (insn 65 (set (reg:SI 54) (and (subreg (reg:QI 52)) (reg:SI 55))))
 *       (insn 69 (set (reg:SI 57) (ior (reg/v:SI 41) (subreg (reg:QI 56)))))
 *     i.e. `(old & ~fieldmask) | value` with `~fieldmask == 0`.  combine folds
 *     the `& 0` away and THE LEFTOVER `(set (reg) 0)` IS THEN HANDED TO
 *     `n->f55 = 0`.  That is why the zero is live before the masked store, and
 *     why every "name the zero" probe in the park was exactly inert: THE ZERO
 *     WAS NEVER BEING MATERIALISED BY THE STATEMENT THAT STORES IT.
 *     Writing the same store as `*(unsigned char *)&s->f9 = w;` takes the plain
 *     MEM path -- no insert, no leftover zero.  The zero is then materialised in
 *     source order and takes r3, the +0x55 address takes r2 and the sprite
 *     pointer takes r1: ALL THREE REGISTERS BECOME THE ROM'S AT ONCE.
 *
 * (3) 2 -> 0.  The last two differences were a pure swap of `add r2, #0x55` and
 *     `strb r3, [r1, #9]` -- two independent insns, the ROM finishing the
 *     address first.  `q55 = &n->f55;` named ONE STATEMENT EARLIER starts that
 *     live range before the store and the order comes out the ROM's.
 *     The park had tried this same pointer on the ORIGINAL body and recorded
 *     "WORSE, 45 lines and 32 differing, because it forces r12 into use".  That
 *     was true of that body and is false of this one: with the insert gone there
 *     is no r12.  A LEVER MEASURED INERT (OR WORSE) AGAINST ONE BODY IS NOT
 *     INERT AGAINST ANOTHER.
 *
 * The three levers the park inherited are all still here and all still needed:
 * the named `int` mask (so ~0xc is built `mov r3,#0xd / neg r3,r3` rather than
 * narrowed to `mov r3,#0xf3`), the named `unsigned char` two (which puts the
 * constant in the `orr` destination), and the positive null test (so the return
 * constant is not hoisted above it).
 *
 * `mov r0, r5` before `bl __Actor_SetSpriteFlags` is ABSENT on both sides:
 * r0 still holds the __CreateActor return, and reload_cse deletes the copy.
 */
struct Spr { unsigned char pad00[9]; unsigned char f9; };

struct A {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Spr *f50;
    unsigned char pad54[1];
    unsigned char f55;
};

extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern void __Func_80929d8(struct A *a, int n);

struct A *OvlFunc_922_2008ed8(int a, int b, int c, int d)
{
    struct A *n;
    struct Spr *s;
    unsigned char *q23;
    unsigned char two;
    int mask;
    unsigned char w;
    unsigned char *q55;

    n = __CreateActor(d, a, b, c);
    if (n != 0) {
        s = n->f50;
        mask = ~0xc;
        w = (mask & s->f9) | 4;
        q55 = &n->f55;
        *(unsigned char *)&s->f9 = w;
        *q55 = 0;
        __Actor_SetSpriteFlags(n, 0);
        __Func_80929d8(n, 0xf);
        q23 = &n->f23;
        two = 2;
        *q23 = two | *q23;
        return n;
    }
    return 0;
}
