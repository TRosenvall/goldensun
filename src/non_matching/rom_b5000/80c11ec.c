/* Func_80c11ec -- AnimateSceneElement, 0x080c11ec, 259 ROM instructions.
 * NON-MATCHING, 264 of 277 encodings differ.
 * Size 604 against the ROM's 588 (+16) and 285 encodings against 277 (+8), so
 * 264 is NOT a true distance.  tools/aligncmp.py reads 158 aligned-equal of
 * 277, 144 differing in 60 hunks.  FIRST CANDIDATE -- this is an early
 * reconstruction, not a worked park.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80c11ec.c \
 *     asm/rom_b5000/rom_c10e8_a_a_a_a_c.s --func Func_80c11ec
 *
 * THE SPLIT: NONE NEEDED, and NO NEW EXPORT.  asm/rom_b5000/rom_c10e8_a_a_a_a_c.s
 * holds this function ALONE and has no data section.  Its three data references
 * `.Lc3604`, `.Lc3620`, `.Lc3628` live in asm/rom_b5000/rom_c10e8_c.s and are
 * ALREADY `.global` there, so they come into C as
 * `extern int Lc3604[] __asm__(".Lc3604");` with nothing to add on the asm side.
 * Landing it is a whole-file conversion -- use `objcmp --whole`.
 *
 * SHIMS: 7 register pins, in the two `.call_via` inline-asm helpers.  FAKEMATCH
 * CLASS -- landing needs a fakematch.txt row, as the two landed users of the
 * same technique have (Func_8097a10 / src/rom_8a000/rom_97384_c_c_a_b.c and
 * Anim_UnleashIntro / src/rom_c9000/rom_cc5d8_a_a_b.c).
 *
 * WHY THE HELPERS.  This function has FOUR `.call_via rN` sites -- the inline
 * `mov r12, pc / bx rN` Thumb-to-ARM veneer gcc never emits -- two through r6
 * and two through r9, all four to Func_8000888 (a fixed-point multiply in
 * IWRAM).  It ALSO has one ordinary `bl _call_via_r3` (Func_80008ac) and two
 * ordinary `bl _call_via_r11` (the draw functions from gPtrs+0xb8 and +0xbc), so
 * both indirect-call forms are present in one body and the relocation list
 * distinguishes them exactly as docs/elevation.md says it does.  The r6 pair
 * uses the several-sites spelling (callee passed unpinned); the r9 pair uses the
 * high-register spelling (`register MulFn mul9 __asm__("r9")` declared once for
 * the function).  `"memory"` and `"r12"` only -- no `"lr"`, which this function
 * needs kept free because THE ROM ALLOCATES r14 AS A LOOP COUNTER
 * (`mov r14,r2 / add r14,r3 / mov r2,r14`), which REG_ALLOC_ORDER permits.
 *
 * WHAT IS ALREADY RIGHT: the relocation set; the prologue; the gPtrs+0xa0/+0x9c/
 * +0xb8 reads; the 16-particle outer loop over `base + 0x11c0` stepping 0x1c
 * with the counter in a 0x10 frame at sp+8; the three-term
 * `FastIntSqrtFP1616_RAM((v>>8)*(v>>8) + ...)`; the `t > 0xfff` split; the
 * three-step inner loop with the two multiply sites and `stmia r5!, {r4}`; the
 * Random/cos/sin respawn block with its two sign flips; the seven-entry table
 * clamp and the six-argument draw call; the three-element second loop over
 * `base + 0x1380` with `3 - q[0x10]/8`.
 *
 * MEASURED (aligned-equal of 277 / encodings):
 *   first transcription ....................................... 154 / 283
 *   + `void **g = gPtrs;` naming the table base ............... 154 / 283  INERT
 *     (the ROM's `mov r12,r2 / mov r3,r12` base copy does NOT come from naming)
 *   + the `t <= 0xfff` arm written FIRST so the ROM's `bgt` skips to the big
 *     arm instead of `ble` skipping to the small one .......... 158 / 285
 *   both ...................................................... 158 / 285  <- this file
 *
 * NEXT.  This has had one afternoon, not a campaign; the +8 is not yet located.
 * The two concrete next questions:
 *   (a) the head is two instructions short of the ROM's `ldr r2,=gPtrs /
 *       mov r12,r2 / mov r3,r12 / add r3,#0xa0` -- the base is held in ip and
 *       copied per use, and naming it is inert, so read the .18.greg dump for
 *       which pseudo the ROM gives ip;
 *   (b) check the two `.call_via` clobber lists against the ROM PER SITE -- the
 *       documented test is "look for a loop-carried or call-crossing value
 *       living in r2 or r3"; here r14 carries the inner loop counter across both
 *       r6 sites, which is evidence the original macro did NOT clobber lr and
 *       may say something about r2/r3 too.  Neither has been varied yet.
 */
typedef int (*MulFn)(int a, int b);
typedef int (*DivFn)(int a, int b);
typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

extern void *gPtrs[];
extern int Func_8000888(int a, int b);
extern int Func_80008ac(int a, int b);
extern int FastIntSqrtFP1616_RAM(int v);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern unsigned char Lc3620[] __asm__(".Lc3620");
extern int Lc3604[] __asm__(".Lc3604");
extern int Lc3628[] __asm__(".Lc3628");

/* `mov r12, pc / bx r6` -- the inline Thumb-to-ARM veneer the ROM uses for the
 * two multiply sites inside the particle loop.  Two sites share one register,
 * so the callee is passed unpinned and the template names %1 (docs/elevation.md,
 * "one site and several sites want DIFFERENT spellings"). */
static inline int call_via_r6(MulFn f, int a, int b)
{
    register MulFn _f __asm__("r6") = f;
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile ("\t.align\t2, 0\n\tmov\tr12, pc\n\tbx\tr6"
                      : "=r" (_a) : "r" (_f), "0" (_a), "r" (_b)
                      : "memory", "r12");
    return _a;
}

static inline int call_via_r9(MulFn f, int a, int b)
{
    register MulFn _f __asm__("r9") = f;
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile ("\t.align\t2, 0\n\tmov\tr12, pc\n\tbx\tr9"
                      : "=r" (_a) : "r" (_f), "0" (_a), "r" (_b)
                      : "memory", "r12");
    return _a;
}

void Func_80c11ec(void)
{
    void **g;
    void *ctx;
    unsigned char *base;
    int *p;
    int *q;
    DrawFn draw;
    DivFn div;
    MulFn mul;
    register MulFn mul9 __asm__("r9") = Func_8000888;
    int i;
    int n;
    int j;
    int a;
    int b;
    int c;
    int m;
    int w;
    int x;
    int y;
    int t;
    int v;

    g = gPtrs;
    ctx = g[0x28];
    base = (unsigned char *)g[0x27];
    *(int *)(base + (0x9e << 5)) = 0;
    draw = (DrawFn)g[0x2e];
    p = (int *)(base + (0x8e << 5));
    for (i = 0xf; i >= 0; i--) {
        m = p[6];
        if (m != 0) {
            t = FastIntSqrtFP1616_RAM((p[0] >> 8) * (p[0] >> 8)
                                      + (p[1] >> 8) * (p[1] >> 8)
                                      + (p[2] >> 8) * (p[2] >> 8));
            if (t <= 0xfff) {
                p[6] = 0;
            } else {
                div = Func_80008ac;
                c = div(t, 0x80 << 9);
                p[6] = p[6] - 1;
                mul = Func_8000888;
                q = p;
                for (n = 2; n >= 0; n--) {
                    v = q[0];
                    t = call_via_r6(mul, -v >> 8, c);
                    t = call_via_r6(mul, t, 0x98 << 9);
                    t = q[3] - (q[3] >> 7) + t;
                    q[3] = t;
                    *q++ = v + t;
                }
            }
            m = p[6];
        }
        if (m == 0) {
            if (*(int *)(base + 0x13bc) <= 0x18) {
                a = Random();
                b = Random() + (0x80 << 9);
                c = (unsigned int)b >> 1;
                p[0] = call_via_r9(mul9, cos(a), c);
                p[1] = call_via_r9(mul9, sin(a), c);
                if (p[0] & 1)
                    p[0] = -p[0];
                if (p[1] & 1)
                    p[1] = -p[1];
                p[2] = (Random() + (0x80 << 8)) >> 2;
                p[4] = ((-p[1]) >> 7) + ((-p[0]) >> 8);
                p[5] = 0;
                p[3] = ((-p[0]) >> 7) + (p[1] >> 8);
                m = ((unsigned int)b >> 13) + 1;
                p[6] = m;
            }
            if (m == 0)
                goto next;
        }
        x = (p[0] >> 10) + 0x40;
        y = (p[1] >> 10) + 0x40;
        j = m;
        if (j < 0)
            j = 0;
        else if (j > 6)
            j = 6;
        w = Lc3620[j];
        draw(ctx, base + Lc3604[j], x - (w >> 1), y - (w >> 1), w, w);
    next:
        p += 7;
    }
    draw = (DrawFn)gPtrs[0x2f];
    q = (int *)(base + (0x9c << 5));
    for (n = 2; n >= 0; n--) {
        q[0] = q[0] + q[2];
        x = q[0] >> 10;
        q[1] = q[1] + q[3];
        y = q[1] >> 10;
        j = 3 - q[4] / 8;
        if (j >= 0) {
            q[4] = q[4] + 1;
            draw(ctx, base + Lc3628[j], x + 0x30, y + 0x30, 0x20, 0x20);
        }
        q += 5;
    }
    *(int *)(base + 0x13bc) = *(int *)(base + 0x13bc) + 1;
    *(int *)(base + (0x9e << 5)) = 1;
}
