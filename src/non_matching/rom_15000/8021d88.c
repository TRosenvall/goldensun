/* Func_8021d88 (0x08021d88) -- NON-MATCHING.
 *
 * NON-MATCHING, 42 of 53 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 53, ours 52) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   SIZE ref 116 bytes, ours 112.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8021d88.c \
 *     asm/rom_15000/rom_20198_c_c_c_c_c.s --func Func_8021d88
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: spill versus high register.
 *
 * 49 lines against 50. Four values are live across the two calls and only
 * r5, r6, r7 are callee-saved (`-fcall-used-r4` removes r4). The ROM puts the
 * fourth in r4 and SPILLS it around each call; gcc takes r10 and pays a save, a
 * restore and a `mov` per use.
 *
 * MEASURED (rom 50 lines, all 49 / 38 differing):
 *   baseline
 *   -fomit-frame-pointer   (r7 is not being held back as a frame pointer)
 *   -fcall-saved-r4        46 lines, 43 differing -- worse; it lets gcc keep
 *                          the value in r4 with no spill at all
 *
 * This is the fifth specimen of the same choice, with rom_9000/800bbc0.c,
 * ovl_7e3e08/2008f94.c, rom_15000/8020150.c and rom_b5000/80b6a60.c.
 *
 * WHAT IS RIGHT: the r8 holding the third argument; the named index for the
 * `str r2, [r4, r3]` register-plus-register store; the `i * 28` built as
 * `(i * 8 - i) << 2`; and the two-statement `p = a + k; p += 0x82 << 1;`.
 *
 * NEXT: nothing source-level.
 */
extern void Func_8021cb8(void *p, int c, int m);
extern int Func_8021c64(int v, int c);

void Func_8021d88(unsigned char *a, int i, int c)
{
    unsigned char *p;
    int k;
    int j;
    int m;
    int v;
    int r;

    k = (i * 8 - i) << 2;
    p = a + k;
    m = i << 4;
    p += 0x82 << 1;
    Func_8021cb8(p, c, m);
    j = k + (0x8e << 1);
    *(int *)(a + j) = c;
    *(int *)(p + 4) = 0x80002000;
    *(int *)(p + 8) = 0;
    k += 0x88 << 1;
    v = *(unsigned short *)(a + k);
    r = Func_8021c64(v, c);
    *(unsigned short *)(p + 8) = (*(unsigned short *)(p + 8) & 0xfffffc00) | (r & 0x3ff);
}
