/* Func_8093c00  --  0x08093c00, split out of asm/rom_8a000/rom_93304_a_c_c_c_c.s
 * (5 functions: Func_8093af8 0x08093af8, Func_8093c00, Func_8093e28 0x08093e28,
 *  Func_8093fa0 0x08093fa0, Func_8094154 0x08094154 -- the other four are all
 *  parked: src/non_matching/rom_8a000/{8093af8,8093e28,8093fa0,8094154}.c).
 *
 * EXACT.  objcmp:
 *     OK Func_8093c00 -- 552 bytes, 246 encodings and 28 relocations identical
 *   SIZE 552 bytes.  INSTRUCTION COUNT 246 (objcmp's figure; it counts the two
 *   in-function pool blocks and their two alignment .shorts as encodings).
 *
 * FAKEMATCH -- 3 REGISTER PINS, and it NEEDS A fakematch.txt ROW.
 *   `python3 tools/shimcount.py` reports "register pins : 3 / *** has a
 *   fakematch-class shim and NO fakematch.txt row".  All three are the
 *   `call_via_r4` helper, copied VERBATIM from the elevated exemplar
 *   src/rom_8a000/rom_97384_c_c_a_b.c (Func_8097a10), which is already booked at
 *   fakematch.txt:584.  The pins are not avoidable: `.call_via r4` expands to
 *   `mov r12, pc / bx r4`, and gcc-2.96's machine description has exactly one
 *   indirect-call pattern (`bl _call_via_%0`), so the veneer can only come from
 *   inline asm.  See docs/elevation.md "RETRACTED: `.call_via rN` is a hard wall".
 *   Book it the same way Func_8097a10 is booked.
 *
 * SPLIT SHAPE.  Func_8093c00 is the SECOND of five, so it needs a two-stage split
 * (the tree's suffixes are binary):
 *     asm/rom_8a000/rom_93304_a_c_c_c_c_a.s     Func_8093af8            (asm)
 *     asm/rom_8a000/rom_93304_a_c_c_c_c_b_a.s   -> src/.../_b_a.c, THIS FILE
 *     asm/rom_8a000/rom_93304_a_c_c_c_c_b_b.s   Func_8093e28, _8093fa0,
 *                                               _8094154               (asm)
 * No rom_93304_a_c_c_c_c_* exists in asm/ or src/, so every suffix is free.
 * The .o is named on exactly ONE linker line, on full path:
 *     stage1.ld:997    asm/rom_8a000/rom_93304_a_c_c_c_c.o(.text)
 * replace it with the three pieces in ROM order.
 * `python3 tools/datacheck.py asm/rom_8a000/rom_93304_a_c_c_c_c.s` prints NOTHING
 * -- no data sections, no required .global data exports.
 * All four sibling parks carry headers pointing at the old .s path and must be
 * repointed: 8093af8.c -> ..._c_a.s, and 8093e28/8093fa0/8094154.c -> ..._c_b_b.s.
 *
 * ============ THE ONE LEVER THAT MATTERED ============
 *
 * THE LOOP IS A `goto`, NOT A `for (;;)` WITH `break`.                  -> EXACT
 *
 * The reference's tail is
 *     cmp r0, #0xf9 / bne .L93dfa / <SetAnim(a,1)> / <WaitFrames(6)> / b .L93c4c
 *     .L93dfa: mov r2, #0 / str r2, [sp, #4]
 *     .L93dfe: ldr r0, [sp, #4]
 * i.e. the INVERTED test falls through into the re-iterate block, and the
 * `ret = 0` store sits past the back-edge.  Three spellings, measured:
 *
 *   (a) `for (;;) { ...; if (k != 0xf9) break; SetAnim; WaitFrames; }` with
 *       `return ret;` at the second collision test  --  192 of 246 aligned,
 *       4 instructions SHORT, and the frame one word small (0x10 vs 0x14).
 *       TWO separate defects, and the second explains the first:
 *         - loop.c ROTATES this shape, moving SetAnim/WaitFrames to the top
 *           behind an entry branch, so the back-edge lands in the middle;
 *         - and `return ret` at the second test lets gcc CONSTANT-PROPAGATE
 *           ret == -1 (the only def reaching it), after which cross-jumping
 *           MERGES it with the earlier literal `return -1` and `ret` never gets
 *           a stack slot at all.  A frame that is one word too small next to a
 *           value the ROM keeps in memory is the tell that a variable got
 *           propagated away.
 *   (b) the same with `break` instead of `return ret` at the second test and
 *       `ret = 0; break;` at the tail  --  161 aligned, 248 instructions (2 LONG).
 *       The slot comes back (the join of -1 and 0 forces the load) but the
 *       rotation is still there and now costs two extra instructions.
 *   (c) `loop: ... if (t2) goto out; ... if (k == 0xf9) { SetAnim; WaitFrames;
 *       goto loop; } ret = 0; out: return ret;`  --  EXACT.
 *
 * So the ROM's flow graph is reachable only by writing the back-edge as an
 * explicit `goto` INSIDE the positive arm.  `for(;;)`+`break` and the goto form
 * are semantically identical and gcc-2.96 lays them out differently: the goto
 * keeps the latch block in fallthrough position, the `for` rotates it.
 *   GENERAL FORM OF THE TELL: a loop whose LAST block before the back-edge is a
 *   conditional body (not the condition itself), with the exit's tail code placed
 *   AFTER the back-edge, is a goto in the source.  Recorded because this is a
 *   cheap check and it was worth 54 of 246 encodings here.
 *
 * ============ ALSO NEEDED, both already recorded ============
 *
 *  - THE gState OFFSET MUST BE BUILT, NOT FOLDED (docs/elevation.md:3413, whose
 *    named example is this very offset): `g = gState;` as a local
 *    `unsigned char *` first, then `*(int *)(g + (0xfa << 1))`.  The ROM builds it
 *    TWICE -- once before the loop and once inside it for Func_8092158 -- so `g`
 *    is REASSIGNED at the second site.  Leaving one assignment at the top lets gcc
 *    keep the base in a callee-saved register across the loop, which the ROM does
 *    not do (and the frame has no slot for it).
 *  - A NAMED `short *` FOR THE HALFWORD READS OF THE VECTOR.  The ROM reads the
 *    high halves of v[0] and v[2] as `mov r3,#2 / ldrsh r1,[r7,r3]` and
 *    `mov r3,#0xa / ldrsh r2,[r7,r3]` -- base r7, the vector pointer, with the
 *    byte offset in a register.  Written `((short *)v)[1]` gcc folds the frame
 *    address and emits `mov r3,sp / add r3,#6 / mov r2,#0 / ldrsh r1,[r3,r2]`,
 *    four instructions for two.  `w = (short *)v;` then `w[1]`, `w[5]` keeps the
 *    pointer as the base.  (Thumb-1 `ldrsh` has no immediate form, so the offset
 *    register is mandatory either way; what the lever buys is WHICH base.)
 *
 * ============ NOTES ON THE READING ============
 *
 *  - `mov r1, #7 / bl _Actor_SetAnim` with r0 NEVER SET is not a ROM bug: r0 still
 *    holds Func_8093af8's return, and the block is reached from either of its two
 *    call sites.  So the source is `q = Func_8093af8(a, 0xcf); if (q == 0)
 *    q = Func_8093af8(a, 0xcd); if (q != 0) { _Actor_SetAnim(q, 7); ... }` -- the
 *    value stays in r0 on both paths and gcc needs no `mov`.  Reading the missing
 *    argument setup as "the ROM passes the same actor" would have cost a `mov`.
 *  - `beq .L93cb6 / b .L93dfe` after the second collision test is gas expanding an
 *    out-of-range Thumb conditional branch (the targets are 0x148 apart, past the
 *    +-254 limit).  Nothing in the source asks for it.
 *  - `ldr r5, =0xffff0000` for the two decrements and `mov r5,#0x80 / lsl r5,#9`
 *    for the two increments: `x -= 0x10000` canonicalises to a plus of a negative,
 *    which must be pooled, while +0x10000 is shiftable.  Both fall out of the
 *    obvious spelling.
 *  - `str r2,[sp]` to store a byte value and `ldrb r2,[sp]` to read it back is an
 *    `int` variable holding a byte: gcc stores the whole word and, because the
 *    only use narrows to QImode for a `strb`, loads just the low byte.  Declaring
 *    it `unsigned char` would give a `strb` at the top, which the ROM does not have.
 *  - `mov r3,#0x55 / add r3,r6` (constant first) is what `p = &a->f55;` gives here;
 *    the sibling Func_809537c has `mov r2,r6 / add r2,#0x55` (pointer first) for
 *    the same field, from `a->f55 = 2;`.  Taking the ADDRESS and storing THROUGH
 *    the member are two different operand orders on the same offset.
 */
extern int Func_8000888(int a, int b);

static inline int call_via_r4(int (*f)(int, int), int a, int b)
{
    register int (*_f)(int, int) __asm__("r4") = f;
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\tr4"
        : "=r" (_a)
        : "r" (_f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12"
    );
    return _a;
}

struct Actor {
    unsigned char pad00[6];
    unsigned short f06;
    int f08;
    int f0c;
    int f10;
    int f14;
    unsigned char pad18[0x22 - 0x18];
    unsigned char f22;
    unsigned char pad23[5];
    int f28;
    unsigned char pad2c[4];
    int f30;
    int f34;
    unsigned char pad38[0x50 - 0x38];
    unsigned char *f50;
    unsigned char f54;
    unsigned char f55;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;

extern struct Actor *MapActor_GetActor(int id);
extern void vec3_translate(int dist, int angle, int *v);
extern int _TestCollision(struct Actor *a, int *v);
extern void CutsceneStart(void);
extern void CutsceneEnd(void);
extern void _Actor_SetAnim(struct Actor *a, int n);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void _Actor_SetSpriteFlags(struct Actor *a, int n);
extern void Func_8092158(int a, int b, int c);
extern struct Actor *Func_8093af8(struct Actor *a, int n);
extern int _Func_8012038(int a, int b, int c);

int Func_8093c00(void)
{
    int v[3];
    int ret;
    int sav;
    struct Actor *a;
    struct Actor *q;
    unsigned char *m;
    unsigned char *p;
    unsigned char *g;
    int dir;
    int flags;
    int k;
    short *w;

    g = gState;
    a = MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    ret = -1;
    dir = (a->f06 + 0x2000) & 0xc000;
    p = &a->f55;
    sav = *p;
    m = iwram_3001ebc;
    flags = 1;
loop:
    v[0] = (a->f08 & 0xfff00000) + 0x80000;
    v[1] = a->f0c;
    v[2] = (a->f10 & 0xfff00000) + 0x80000;
    vec3_translate(0x100000, dir, v);
    if (_TestCollision(a, v) == 1)
        return -1;
    v[0] = (a->f08 & 0xfff00000) + 0x80000;
    v[1] = a->f0c;
    v[2] = (a->f10 & 0xfff00000) + 0x80000;
    vec3_translate(0x200000, dir, v);
    if (_TestCollision(a, v))
        goto out;
    if (a->f54 == 1)
        flags = a->f50[0x26];
    CutsceneStart();
    _Actor_SetAnim(a, 6);
    WaitFrames(6);
    _PlaySound(0x98);
    _Actor_SetAnim(a, 7);
    a->f30 = 0xc0 << 10;
    a->f34 = 0x80 << 10;
    a->f28 = 0x80 << 11;
    *p &= 0x7e;
    _Actor_SetSpriteFlags(a, 0xfe & flags);
    g = gState;
    w = (short *)v;
    Func_8092158(*(int *)(g + (0xfa << 1)), w[1], w[5]);
    _Actor_SetAnim(a, 6);
    _Actor_SetSpriteFlags(a, flags);
    q = Func_8093af8(a, 0xcf);
    if (q == 0)
        q = Func_8093af8(a, 0xcd);
    if (q != 0) {
        _Actor_SetAnim(q, 7);
        a->f0c -= 0x10000;
        a->f14 -= 0x10000;
        WaitFrames(2);
        a->f0c -= 0x10000;
        a->f14 -= 0x10000;
        WaitFrames(0xa);
        a->f0c += 0x10000;
        a->f14 += 0x10000;
        WaitFrames(4);
        a->f0c += 0x10000;
        a->f14 += 0x10000;
    } else {
        WaitFrames(6);
    }
    *p = sav;
    CutsceneEnd();
    if (m != 0)
        *(int *)(m + (0xda << 1)) +=
            call_via_r4(Func_8000888, *(int *)(m + (0xd8 << 1)), 0x80 << 14);
    k = _Func_8012038(a->f22, v[0], v[2]);
    if (k == 0xf9) {
        _Actor_SetAnim(a, 1);
        WaitFrames(6);
        goto loop;
    }
    ret = 0;
out:
    return ret;
}
