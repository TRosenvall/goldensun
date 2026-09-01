/* OvlFunc_924_200ae6c (0x0200ae6c) -- NON-MATCHING.
 * Blocker class: argument interleave at an UNGUARDED site, TWO lines of 109.
 *
 * Exact length, and the whole residue is one pair swapped at a single call:
 *
 *     rom    mov r1, #0x80 / mov r0, r5 / lsl r1, #0x1 / bl __Func_8092950
 *     ours   mov r1, #0x80 / lsl r1, #0x1 / mov r0, r5 / bl __Func_8092950
 *
 * The ROM loads r0 before completing r1's split build. Everything else -- the
 * 0x28 stack struct, the eight-argument OvlFunc_common0_10c with four words
 * spilled, three Randoms at three different scales, the `i * 0x1999` step and
 * the two nested guards -- is exact on the first screen.
 *
 * WHY THE GUARDED-INTERLEAVE LEVER DOES NOT APPLY. That lever needs a block
 * dominating the call so gcc rematerialises the named constant there;
 * docs/elevation.md records that the dominating block may be a CALL rather than
 * a branch, which is why it was worth trying here. It is not: this call site is
 * straight-line from the function's first instruction, and naming the constant
 * makes gcc hold it live instead -- 110 lines against 109 with 99 differing,
 * which is the documented straight-line failure mode exactly.
 *
 * MEASURED, all 109 lines and 2 differing unless noted:
 *   as written                                                        2
 *   `k = 0x80 << 1;` named before the two preceding calls   110 lines, 99
 *   the callee declared to return `int` (the return-type lever)       2
 *   the callee's prototype withheld entirely                          2
 *
 * The last two are the recorded controls over r0's position in a two-argument
 * call, and neither moves it. So this is the straight-line interleave, not a
 * return-type question.
 *
 * NEXT: nothing source-level. Same class as
 * src/non_matching/ovl_7ac2d8/200cf44.c, which is 2 of 28 on the identical
 * shape in the same overlay.
 */
struct P {
    int f0;
    int f4;
    unsigned char pad8[0x18 - 8];
    short f18;
    unsigned char pad1a[2];
    void *f1c;
    unsigned char pad20[8];
};

extern int *__MapActor_GetActor(int slot);
extern unsigned int __Random(void);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __Actor_SetSpriteFlags(int *a, int f);
extern void __Func_8092950(int a, int b);
extern void OvlFunc_924_200bbd4(int x, int y, int z);
extern void OvlFunc_common0_10c(int a, int b, int c, int d,
                                int e, int f, int g, struct P *p);
extern unsigned char L5e70[] __asm__(".L5e70");

void OvlFunc_924_200ae6c(int slot)
{
    struct P p;
    int *a;
    unsigned int i;
    int x;
    int y;
    int n;

    a = __MapActor_GetActor(slot);
    *((unsigned char *)a + 0x55) = 0;
    __Actor_SetSpriteFlags(a, 0);
    __Func_8092950(slot, 0x80 << 1);
    __PlaySound(0xdd);
    p.f0 = 1;
    p.f4 = 5;
    p.f18 = 0x8f << 1;
    p.f1c = L5e70;
    i = 0;
    do {
        if (i <= 0x1f)
            OvlFunc_924_200bbd4(a[2], a[3], a[4]);
        if ((1 & i) != 0) {
            __PlaySound(0xf6);
            x = a[2] + ((__Random() * 24) >> 16 << 16);
            x += 0xfff40000;
            y = a[3] + ((__Random() << 5) >> 16 << 16);
            y += 0xfff00000;
            n = ((__Random() * 4) >> 16 << 15) + (0x80 << 8);
            OvlFunc_common0_10c(x, y, a[4], 0, n, 0, 0xcc << 14, &p);
        }
        a[3] = a[3] + i * 0x1999;
        a[0xf] = a[3];
        __WaitFrames(2);
        i++;
    } while (i <= 0x2f);
}
