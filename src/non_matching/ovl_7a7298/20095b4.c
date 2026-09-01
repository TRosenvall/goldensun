/* OvlFunc_921_20095b4 (0x020095b4) -- NON-MATCHING.
 * Blocker class: REGISTER-ROLE SWAP (the dominant wall), plus one pooled zero.
 *
 * 101 lines against the ROM's 102, 65 differing -- and the whole body is
 * instruction-for-instruction identical.  Every difference is one of two
 * things: r5 and r6 hold each other's values throughout, and the ROM's
 * pooled zero is a register in ours.
 *
 *     rom    mov r6, r0 / mov r5, r6 / add r5, #0x64      e -> r6, ptr -> r5
 *     ours   mov r5, r0 / mov r6, r5 / add r6, #0x64      e -> r5, ptr -> r6
 *
 * FOUR LEVERS LANDED HERE and are worth keeping; the file below carries all
 * of them.  It began at 109 lines / 108 differing.
 *
 * 1. THE ALIASING TELL (-fno-strict-aliasing, ALIAS_CFLAGS).  The ROM stores
 *    e->f8, stores a SHORT through &e->f64, then re-reads e->f8.  With strict
 *    aliasing the halfword store cannot touch the int, so gcc keeps the value
 *    in a callee-saved register (r8) and never reloads -- 109 lines.  The flag
 *    restores the reload.  Measured both ways on the final source:
 *        without the flag  105 lines, 104 differing
 *        with the flag     101 lines,  65 differing
 *    This is exactly the shape docs/elevation.md calls "A MISSING RELOAD after
 *    a store of a different width is an ALIASING tell".
 *
 * 2. THE MASK MUST BE AN INT (`int mask = -13;`).  `q[9] & ~0xc` stored back
 *    into a byte lets gcc truncate to `mov r3, #0xf3`, one instruction shorter
 *    than the ROM's `mov r3, #0xd / neg r3, r3`.  Naming the mask as an int
 *    stops the narrowing.
 *
 * 3. THE SECOND BYTE ADDRESS IS DERIVED, NOT REBUILT.  The ROM has
 *    `add r2, #0x23 / strb / add r2, #0x32 / strb`.  Written as two struct
 *    members gcc rebuilds the base (`mov r3, r5 / add r3, #0x55`); a pointer
 *    advanced by 0x32 gives the ROM's chain.
 *
 * 4. THE MERGE LEVER, and it is what took this from 106 to 101.  The ROM holds
 *    &e->f64, then the stack vector, then the CreateActor result, ALL in r5.
 *    With a separate `short *ph` and `int *p` gcc spends r5, r6, r7 AND r8 --
 *    four callee-saved registers against the ROM's two.  Writing ONE `int *p`
 *    that plays both roles (`*(short *)p` for the angle) drops it to three.
 *    Merging the CreateActor result in as well is WORSE (67), so the merge is
 *    two-way, not three-way.
 *
 * WHAT IS LEFT, and what was measured against it.
 *
 * (a) The r5/r6 swap.  Both pointers are born at the top in the ROM's order,
 *     so docs/elevation.md's birth-order rule does not apply -- this is the
 *     allocator's priority order (REG_ALLOC_ORDER gives r5 to the higher
 *     priority pseudo) and the ROM ranks the pointer above the parameter
 *     while we rank the parameter above the pointer.  Inert, all at 65:
 *       - declaring `p` first, `v` first, or `n` last  (three permutations)
 *       - typing the merged pointer `short *` or `char *` instead of `int *`
 *       - merging the CreateActor result into the same variable (67, worse)
 *
 * (b) The pooled zero.  The ROM materialises the 0 it stores at +0x55 with
 *     `ldr r3, =0x0` -- a POOL load, not a `mov`.  gcc instead notices that
 *     inside `iwram_3001e40 % 3 == 0` the modulo result IS zero and reuses
 *     that register (`mov r7, r0 / cmp r7, #0` ... `strb r7, [r2]`), which is
 *     what costs the third callee-saved register.  None of the CSE flags
 *     touches it -- -fno-gcse, -fno-cse-follow-jumps, -fno-cse-skip-blocks and
 *     -fno-rerun-cse-after-loop all give exactly 65.  It is value propagation
 *     into the branch, not a CSE pass.
 *
 *     A direct probe of gcc-2.96 (scratch, six spellings) says a byte store of
 *     0 gives `mov rN, #0` from a plain literal, from an `int` local, from a
 *     `short` local and from an `unsigned short` local alike; only routing the
 *     value through a HALFWORD store pools it, and then as `ldrh`, not `ldr`.
 *     So no source spelling found here produces the ROM's SImode pool load of
 *     zero.  The likely mechanism is reload rematerialising a SPILLED constant
 *     pseudo through force_const_mem -- which is a decision about register
 *     pressure, i.e. the same wall as (a), not an independent one.
 *
 * (c) The missing `b` over the mid-function pool is a LENGTH consequence, per
 *     the settled note on the branch-over-pool class, and accounts for the
 *     one-line deficit exactly: we carry one extra `mov r7, r0` and lack both
 *     the `ldr r3, =0x0` and the `b`.
 *
 * NOT-A-DIFFERENCE: `bl __umodsi3` against the ROM's `bl _umodsi3_RAM` is the
 * linker alias this overlay already carries (overlays/rom_7a7298/overlay.ld
 * line 79, `__umodsi3 = _umodsi3_RAM;`).  tryc cannot see it.
 *
 * TEMPLATE: this function is a near-twin of the SOLVED Func_80993b0 in
 * src/rom_8a000/rom_97b54_a_c_c_a_c_c_c_c_b.c -- same sin/call_via_r3 opening,
 * same angle increment and `% 64` wrap, same stack-vector-then-CreateActor
 * tail.  That one matches with SEPARATE locals, because its first block sits
 * under an `if (*ph != -1)` guard so the store to f8 does not dominate the
 * re-read.  Here there is no guard, which is why the aliasing flag is needed
 * and why the register budget is tighter.  Statement order did not transfer
 * either: the twin wants f1c before f18, this one wants f18 before f1c.
 *
 * NEXT: nothing source-level is outstanding.  This wants whatever eventually
 * cracks the register-role-swap class; it is a good specimen for it because
 * everything else is already exact.
 */
struct Ent {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[3];
    unsigned char f23;
    unsigned char pad24[0x2c];
    unsigned char *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[0xe];
    short f64;
};

extern int L31f0 __asm__(".L31f0");
extern unsigned int iwram_3001e40;
extern int __sin(int a);
extern int Func_8000888(int a, int b);
extern int __Random(void);
extern void __vec3_translate(int a, int b, int *v);
extern struct Ent *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetSpriteFlags(struct Ent *a, int n);
extern void __Actor_SetAnim(struct Ent *a, int n);
extern void __Func_80929d8(struct Ent *a, int n);
extern void __Actor_SetScript(struct Ent *a, void *script);
extern unsigned char gScript_921__0200a64c[];

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

void OvlFunc_921_20095b4(struct Ent *e)
{
    struct Ent *n;
    unsigned char *r;
    int mask;
    int v[3];
    int *p;

    p = (int *)&e->f64;
    e->f8 = L31f0 + call_via_r3(0xc0 << 11, __sin(*(short *)p << 10));
    *(short *)p = *(short *)p + 1;
    *(short *)p = (*(short *)p + 0x40) % 64;
    if (iwram_3001e40 % 3 == 0) {
        p = v;
        p[0] = e->f8;
        p[1] = e->fc + (0x80 << 10);
        p[2] = e->f10;
        __vec3_translate(__Random() * 6, __Random(), p);
        n = __CreateActor(0x11d, p[0], p[1], p[2]);
        if (n != 0) {
            mask = -13;
            n->f50[9] = n->f50[9] & mask;
            __Actor_SetSpriteFlags(n, 0);
            __Actor_SetAnim(n, 1);
            n->f18 = 0x9999;
            n->f1c = 0x9999;
            r = (unsigned char *)n + 0x23;
            *r = 2;
            r += 0x32;
            *r = 0;
            __Func_80929d8(n, 9);
            __Actor_SetScript(n, gScript_921__0200a64c);
        }
    }
}
