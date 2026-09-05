/* Func_801b4ec -- 0x0801b4ec
 *
 * ScrollPartyListUp. Advances the party-list window by one row: if the last
 * visible row is not already the last entry, run the pre-scroll hook, hold a
 * frame at state 0x21, bump the scroll sub-counter, and -- when that counter
 * has just reached 4 AND another entry would still fit -- back the counter off
 * again, mark the window dirty, advance the top row, repaint, and clear the
 * tail flag if the new window ends exactly on the last entry. Either way it
 * finishes by restoring state 1 and running the two post-step hooks a frame
 * apart.
 *
 * DIRECT SIBLING of Func_801b5c0 (src/rom_15000/rom_1aeec_a_a_c_a_c_c_b.c) --
 * same object family, same five callees, same tail. Three of that function's
 * four recorded levers carry over unchanged and were re-measured here rather
 * than transplanted; the fourth (the ROM is not an overlay cutscene script) was
 * never in play. Only ONE new decision was needed, and it is a statement
 * POSITION, not a spelling.
 *
 * THE WHOLE RESIDUE WAS BIRTH POSITION OF ONE ADDRESS LOCAL. Written with the
 * three address locals assigned together at the top -- p, q, m, then
 * `n = *p + *q + 1` -- the function is 97 lines against 97 with FOUR encodings
 * differing, and the disagreeing region is three instructions long: the
 * `mov #0xe5 / lsl #2 / add r5` build of `m` sits one slot EARLY, interleaved
 * into the `n` chain at the wrong offset.
 *
 *     rom    mov r2,#0xe5 / add r3,r1 / lsl r2,#2 / add r3,#1 / add r2,r5
 *     ours   mov r2,#0xe5 / lsl r2,#2 / add r3,r1 / add r2,r5 / add r3,#1
 *
 * Moving `m = ...` ONE statement later, to after `n = ...`, is exact. This is
 * docs/elevation.md's "read `mov rN, <base>` in the ROM as a statement
 * position" rule (the OvlFunc_886_20090c0 table) reached from a different
 * symptom: that entry's recognition line is "one scratch register is wrong and
 * nothing else is", and here no register is wrong at all -- the allocation is
 * identical and only the SCHEDULE slips. NEW, and only as a recognition line:
 * a three-instruction interleave offset in a constant-plus-base address build
 * is the same lever's other face. Deleting the local outright and inlining
 * `*(unsigned short *)(s + (0xe5 << 2))` at all three uses ALSO matches, which
 * is the confirmation: `m`'s existence is free (gcc holds the address in a
 * callee-saved register across the calls either way, because the ROM does),
 * and only where it is BORN costs anything. The named local ships because the
 * ROM's r8 says the address survives two calls.
 *
 * The narrow-store table is the sibling's, for the fifth batch running. All
 * five stores of a small literal -- 0x21 and 1 to +0x3a2, 8 to +0x3c, 0 to
 * +0x3e, 1 to +0xa -- go through the TYPED STRUCT. Written as casts they pool
 * every literal (`ldr r3, =0x1` where the ROM has `mov r3, #1`) and drag in a
 * branch-over-pool: 15 differing.
 *
 * `n` MUST BE `unsigned`. The bound test is `n + 1 < *m`; as an `int` it is the
 * only differing instruction in the function (`bge` where the ROM has `bcs`).
 * The unsigned type is what makes the halfword's promotion convert rather than
 * widen.
 *
 * THE SUB-COUNTER IS READ BACK, NOT NAMED. The ROM tests the just-stored value
 * with `lsl r3,#16 / cmp r3, 0x80<<11`, which is gcc comparing in HImode --
 * i.e. re-reading `*q`, not carrying an `int`. Naming the increment
 * (`v = *q + 1; *q = v; if (v == 4)`) gives a plain `cmp r3,#4` and costs 43 of
 * 97; the ROM's `lsl #16` immediately before a `cmp` is the cast-at-the-
 * comparison form, per the OvlFunc_922_20095dc rule.
 *
 * THE DECREMENT'S CONSTANT IS gcc's, NOT THE SOURCE'S. `*q = *q - 1` inside the
 * branch does NOT emit a `sub`: gcc distributes the -1 into HImode as +0xffff
 * (the sibling's `ldr r1, =0xffff` shape), then constant-folds it against the
 * `*q + 1` it already has in r1, giving `r1 + 0x10000` -- and 0x10000 is
 * shiftable, so it comes out `mov r2,#0x80 / lsl r2,#9 / add r3,r1,r2`. The
 * apparently wrong constant is the add/sub-chain rule: write the literal
 * spelling and let gcc do its own arithmetic.
 *
 * THE TAIL'S ARGUMENT IS A FRESH ADDRESS EXPRESSION, not `*q`. The ROM
 * recomputes `s + 0x39e` from the pool for the Func_801b9ec argument even
 * though r6 still holds it; spelling it `*q` is 7 differing. Identical to the
 * sibling, which writes the same call the same way.
 *
 * VERIFIED WITH tools/objcmp.py against the ORIGINAL asm path
 * asm/rom_15000/rom_1aeec_a_a_c_a_c_c_a.s:
 *   OK Func_801b4ec -- 212 bytes, 97 encodings and 7 relocations identical
 * That check was not optional: the reference holds TWO functions, so tryc.py
 * SKIPS its size check and its "OK" covers only the instruction stream.
 * 212 bytes is the whole span 0x0801b4ec..0x0801b5bf, up to Func_801b5c0, so
 * the trailing literal pool is inside the compared region.
 *
 * MEASURED WORSE (all at exact length, 97 of 97):
 *   m born before n (all three addresses hoisted)            4 differing
 *   n declared int (bge for bcs)                             1 differing
 *   Func_801b9ec(s, *q) instead of the fresh cast            7 differing
 *   five narrow stores as casts instead of struct fields    15 differing
 *   the increment named in an int local                     43 differing
 *
 * FLAGS: tree default GCC296_CFLAGS. No per-file Makefile rule; the landing
 * path src/rom_15000/... is built by the generic `asm/%.o: src/%.c` cross-dir
 * rule, which is the same flag group tryc.py gives a scratch path.
 */
struct S {
    unsigned char pad00[10];
    short f0a;
    unsigned char pad0c[0x30];
    short f3c;
    short f3e;
    unsigned char pad40[0x308];
    unsigned char *f348;
    unsigned char pad34c[0x50];
    unsigned short f39c;
    unsigned short f39e;
    unsigned char pad3a0[2];
    short f3a2;
};

extern void Func_801b9a8(unsigned char *s, int n);
extern void Func_801ba68(unsigned char *s, int n);
extern void Func_801b9ec(unsigned char *s, int n);
extern void Func_801b010(int a, int b);
extern void WaitFrames(int n);

void Func_801b4ec(unsigned char *s)
{
    struct S *e;
    unsigned short *p;
    unsigned short *q;
    unsigned short *m;
    unsigned char *t;
    unsigned int n;

    e = (struct S *)s;
    p = (unsigned short *)(s + (0xe7 << 2));
    q = (unsigned short *)(s + 0x39e);
    n = *p + *q + 1;
    m = (unsigned short *)(s + (0xe5 << 2));
    if (n == *m)
        return;
    Func_801b9a8(s, *q);
    e->f3a2 = 0x21;
    WaitFrames(1);
    *q = *q + 1;
    if (*q == 4 && n + 1 < *m) {
        *q = *q - 1;
        e->f3c = 8;
        *p = *p + 1;
        Func_801ba68(s, 1);
        if (*p + *q + 2 == *m)
            e->f3e = 0;
        e->f0a = 1;
    }
    e->f3a2 = 1;
    Func_801b9ec(s, *(unsigned short *)(s + 0x39e));
    WaitFrames(1);
    t = *(unsigned char **)(s + (0xd2 << 2));
    Func_801b010(*(unsigned short *)(t + 0xa), 0);
    WaitFrames(1);
}
