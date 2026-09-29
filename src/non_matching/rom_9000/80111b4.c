/* Func_80111b4 -- NON-MATCHING, 151 of 248 encodings differ.
 * Unattempted before batch 298.  Reference asm/rom_9000/rom_108e4_c.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/80111b4.c \
 *       asm/rom_9000/rom_108e4_c.s --func Func_80111b4
 *
 * NOT a distance: size 568 against 560 and count 252 against 248 -- FOUR OVER.
 * SHIMS: 3 register pins, so a landing needs a fakematch.txt row (precedented at
 * fakematch.txt:586).
 *
 * TWO CORRECTIONS TO THIS FUNCTION'S OWN EARLIER SCREEN, both worth keeping.
 * First, the veneer here is FOUR INLINE `.call_via r3` sites, not `bl _call_via_rN`
 * -- and docs/elevation.md RETRACTS the claim that those are a wall (51 functions
 * were written off on it).  All four are reproduced with the pinned-register helper
 * from the elevated Func_80993b0.
 * Second, a hand ledger of the reference REPEATED tryc.py's old bug: `.call_via`
 * begins with a dot, so it was read as a directive and the reference counted 228
 * instructions instead of 236, making the candidate look 5 SHORT when it is 3 OVER.
 * objcmp was right throughout.  Do not hand-count a reference containing .call_via.
 *
 * WHAT PAID: the three IWRAM bases are ONE symbol reached by NEGATIVE INDEX --
 * `(&iwram_3001e80)[-5]`, `[-4]` -- which pools the ROM's symbol with the ROM's zero
 * addend and aligned the whole 22-instruction prologue; `D` is `C->f0`, NOT `C->f348`,
 * and that one field was worth 75 (226 -> 151); and Func_80008ac needs its pointer
 * local assigned AFTER the cos/sin calls, or it lives across them, greg gives it a
 * callee-saved register, and it emits `bl _call_via_r6` for the ROM's `_call_via_r3`.
 *
 * BLOCKER: the pinned r3 is re-materialised per helper inline (+2).  The ROM loads
 * Func_8000888 once per block and the second `.call_via` reuses live r3; hard-register
 * sets are not CSE'd.  UNTRIED ROUTE: hoist the pinned declaration to block scope
 * with both __asm__ statements under one `_f`.
 */
/* Func_80111b4 -- 0x080111b4, asm/rom_9000/rom_108e4_c.s
 *
 * The 3D camera update: jitters two axes through Func_8000888, fires the two
 * axis hooks when either crosses a 1<<20 boundary, then builds the view matrix
 * and dispatches the renderer out of gPtrs[0x2e].
 *
 * NON-MATCHING: 151 encodings of 248 differ (objcmp).  SIZE 568 against 560 and
 * INSTRUCTION COUNT 252 against 248 -- FOUR over -- so the 151 is NOT a
 * distance; both figures must land first.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/80111b4.c \
 *     asm/rom_9000/rom_108e4_c.s --func Func_80111b4
 *
 * SHIMS: 3 register pins (tools/shimcount.py), fakematch class, and it NEEDS A
 * fakematch.txt ROW -- the pins are the `.call_via r3` veneer helper below.
 * PRECEDENTED: the helper is copied from src/rom_8a000/rom_97b54_a_c_c_a_c_c_c_c_b.c
 * (Func_80993b0), which is elevated, byte-exact and already booked in
 * fakematch.txt:586.
 *
 * SPLIT: datacheck.py reports the reference carries a .rodata section and needs a
 * TEXT/DATA SPLIT, but Func_80111b4 itself "reads no data label -> split needs NO
 * new export".  The file holds SEVEN functions, so it cannot convert whole.
 *
 * ==> CORRECTION TO THIS SESSION'S OWN EARLIER SCREEN. <==
 * I first set this function aside as unverifiable because it calls
 * `_call_via_r3` / `_call_via_r4`.  That was wrong twice over.  `bl _call_via_rN`
 * is the ORDINARY indirect call gcc emits natively, and the underscore names are
 * what the tree declares verbatim, so the relocations agree exactly -- there is
 * no alias class to characterise.  The real veneer here is the FOUR inline
 * `.call_via r3` sites, and docs/elevation.md RETRACTS the claim that those are a
 * wall (see "RETRACTED: `.call_via rN` is a hard wall", which notes 51 functions
 * were written off on it).  They are reachable with the pinned-register asm
 * helper, and all four are reproduced here.
 *
 * ALSO: MY LEDGER REPEATED tryc.py's OLD BUG.  `.call_via` starts with a dot, so
 * a naive reference parser skips it as a directive -- but it is a macro from
 * include/macros.inc expanding to TWO real instructions (`mov r12, pc / bx rN`).
 * Before fixing that, the reference read 228 instructions instead of 236 and this
 * candidate looked 5 SHORT when it was 3 OVER.  objcmp assembles the reference
 * and was right throughout; the hand ledger was not.
 *
 * FOUR THINGS ESTABLISHED, each measured:
 *
 * 1. THE THREE IWRAM BASES ARE ONE SYMBOL, REACHED BY NEGATIVE INDEX.
 *    The ROM pools `=iwram_3001e80` ONCE and derives the other two addresses
 *    with `sub r2,#0x14` and `sub r3,#0x10`.  Three separate `extern` symbols
 *    would be three bare SYMBOL_REFs and pool three words: cse.c's
 *    `related_value` chains only a CONST (symbol PLUS an integer term).  So they
 *    must come off one base, and indexing from the HIGHEST -- `iwram_3001e80`
 *    with `(&iwram_3001e80)[-5]` and `[-4]` -- pools the ROM's symbol with the
 *    ROM's zero addend, rather than pooling the low symbol with a +0x14 addend.
 *    This is what made the whole 22-instruction prologue align exactly.
 *
 * 2. D IS C->f0, NOT C->f348.  `ldr r2,[r6]` is offset ZERO; f348 and f34c are
 *    the two separate spilled locals read just after.  Fixing this one field was
 *    worth 75 (226 -> 151) and closed the prologue.
 *
 * 3. Func_80008ac NEEDS A FUNCTION-POINTER LOCAL, ASSIGNED AFTER THE cos/sin
 *    CALLS.  A direct `Func_80008ac(a, b)` emits `bl Func_80008ac` and no veneer
 *    at all.  Assigning it to a local first produces the veneer -- but assigning
 *    it BEFORE the cos and sin calls makes the pointer live across them, so greg
 *    gives it a CALLEE-SAVED register and gcc emits `bl _call_via_r6` where the
 *    ROM has `bl _call_via_r3`.  The veneer symbol name follows the register, so
 *    that is a real difference, not cosmetic.  Evaluating cos and sin into locals
 *    first and assigning the pointer LAST lets it die immediately, which puts it
 *    in r3 (first in REG_ALLOC_ORDER) and matches.
 *
 * 4. gcc's `/ 0x100000` IS the ROM's rounding sequence.  `cmp / bge / add 0xfffff
 *    / asr #20` is exactly signed division by 1<<20; no manual shift is needed.
 *
 * MEASURED WORSE / INERT:
 *   helper parameterised + one caller-side `fp` local     254 insns (+2), 213
 *   x = D[0] before y = D[2] (register swap probe)        inert (151, 252)
 *
 * BLOCKER: THE PINNED r3 IS RE-MATERIALISED PER HELPER INLINE.  +2 instructions,
 * and it is the largest of the four.  Each block makes TWO calls through
 * Func_8000888; the ROM loads `ldr r3,=Func_8000888` ONCE per block and the
 * second `.call_via r3` reuses the live r3.  Ours emits the pool load twice per
 * block, because the helper's `register ... __asm__("r3")` is a fresh assignment
 * to a HARD register on each inline and hard-register sets are not CSEd.
 *
 * The open route, NOT tried: hoist the pinned declaration to BLOCK scope and put
 * the two `__asm__` statements under one `_f`, so r3 is written once and both
 * veneers read it -- i.e. abandon the one-call helper for this function and
 * inline the pair. That is more pins, so it wants the fakematch row either way.
 * Parameterising the helper does NOT substitute: it costs a caller-side copy and
 * measured +2 the wrong way.
 *
 * SMALLER RESIDUES, all still open:
 *   - ROM has an inverted long-branch PAIR (`bne .L11208 / b .L112e0`) where ours
 *     has a single `beq`; +1 for the ROM.  A code-layout consequence -- on
 *     Func_801b664 the same `bhi/b` shape fell into line by itself once the
 *     lengths agreed, so this one should be re-read last, not chased now.
 *   - one extra `mov r2,#128` for the 0x100000 mask (the two blocks each
 *     rebuild it in the ROM; ours shares one).
 *   - r7/r8 carry x and y the opposite way round from the ROM, and r9/r10 the
 *     same; the order probe above was inert, so this is greg, not statement order.
 *   - the `gPtrs + 0xb8` build is issued a few slots early.
 *
 * FLAGS: production GCC296_CFLAGS throughout; no figure here is flag-conditional.
 */
struct Cam {
    int *f0;                            /* 0x00 */
    int f4;                             /* 0x04 */
    int f8;                             /* 0x08 */
    int fc;                             /* 0x0c */
    unsigned char pad10[0xe4 - 0x10];
    int fe4;                            /* 0xe4 */
    int fe8;                            /* 0xe8 */
    unsigned char pad_ec[0x118 - 0xec];
    unsigned short f118;                /* 0x118 */
    unsigned short f11a;                /* 0x11a */
    unsigned char pad11c[0x348 - 0x11c];
    void *f348;                         /* 0x348 */
    int f34c;                           /* 0x34c */
};

extern void *iwram_3001e80;
extern int iwram_3001af4;
extern int iwram_3001f60;
extern unsigned int iwram_3001e40;
extern int gPhysVec[];
extern void *gPtrs[];

extern void Func_80114a0(void);
extern void Func_8011164(int a);
extern void Func_80110e0(int a);
extern int Random(void);
extern int Func_8000888(int a, int b);
extern void Func_8005258(void *a, int b, int c);
extern void InitMatrixStack(void);
extern void MatrixTranslatev(int *v);
extern void MatrixYaw(int a);
extern void MatrixPitch(int a);
extern void MatrixSetLook(void *a, int *v);
extern void Func_80123f4(int a, int *v, void *b);
extern int cos(int a);
extern int sin(int a);
extern int Func_80009c0(int *v, void *a);
extern int Func_80008ac(int a, int b);

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

void Func_80111b4(void)
{
    void *A;
    void *B;
    struct Cam *C;
    int *D;
    int *vec;
    void *E;
    void *F;
    int G;
    int x, y, xi, yi, t, u, f, old;
    int (*g)(int *, void *);
    int (*k)(int, int);
    void (*h)(void *, int *, void *, void *);
    int vec2[3];

    A = iwram_3001e80;
    B = (&iwram_3001e80)[-5];
    C = (struct Cam *)(&iwram_3001e80)[-4];
    vec = (int *)((char *)A + 0xc);
    D = C->f0;
    E = (char *)B + (0xc8 << 4);
    F = C->f348;
    G = C->f34c;
    Func_80114a0();
    if (D != 0) {
        y = D[2];
        x = D[0];
        if (C->f4 != 0) {
            t = Random();
            u = Random();
            f = C->f4;
            x += call_via_r3(f, t - u);
            C->f4 = call_via_r3(f, C->fc);
        }
        if (C->f8 != 0) {
            t = Random();
            u = Random();
            f = C->f8;
            y += call_via_r3(f, t - u);
            C->f8 = call_via_r3(f, C->fc);
        }
        xi = x / 0x100000;
        yi = y / 0x100000;
        old = C->fe4;
        if (((old ^ x) & 0x100000) != 0) {
            if (old < x)
                Func_8011164(xi + 0x10);
            else
                Func_8011164(xi - 0x10);
        }
        old = C->fe8;
        if (((old ^ y) & 0x100000) != 0) {
            if (old < y)
                Func_80110e0(yi + 0xc);
            else
                Func_80110e0(yi - 0x12);
        }
        C->fe4 = x;
        C->fe8 = y;
    }
    gPhysVec[3] = 0x78;
    gPhysVec[4] = 0x60;
    Func_8005258(F, G / 2, G * 2);
    vec[0] = *D++;
    vec[1] = 0;
    vec[2] = D[1];
    InitMatrixStack();
    MatrixTranslatev(vec);
    MatrixYaw(C->f11a);
    MatrixPitch(C->f118);
    vec2[0] = 0;
    vec2[1] = 0;
    vec2[2] = G + (0x80 << 9);
    g = Func_80009c0;
    g(vec2, A);
    InitMatrixStack();
    MatrixSetLook(A, vec);
    if (iwram_3001af4 != C->f118) {
        x = cos(C->f118);
        y = sin(C->f118);
        k = Func_80008ac;
        Func_80123f4(k(x, y), vec, B);
        iwram_3001f60 = 0;
        iwram_3001af4 = C->f118;
    }
    h = (void (*)(void *, int *, void *, void *))gPtrs[0x2e];
    h(A, vec, B, (char *)E + (iwram_3001e40 & 1) * 5 * 1024);
}
