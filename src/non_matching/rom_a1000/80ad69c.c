/* Func_80ad69c  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_a1000/rom_ad274_c_c.s
 * Best screen: 5 instructions in disagreeing regions, of 25 (rom 25, ours 24).
 *
 * BLOCKER CLASS: constant-CSE, inverted -- gcc CSEs the SUM where the ROM
 * CSEs the CONSTANT.
 *
 * The ROM loads 0x219 once into r1 and keeps it live across the whole
 * function, adding it to the base pointer TWICE:
 *
 *      ldr  r1, =0x219
 *      add  r3, r2, r1      <- for the entry test
 *      ...
 *      add  r7, r2, r1      <- for the loop bound, in the pre-header
 *
 * gcc instead computes `base + 0x219` once and keeps the POINTER live,
 * rematerialising the pooled constant wherever it needs it again.  Every
 * spelling tried lands on one of two shapes and neither is the ROM's:
 *
 *   1. Two named pointer locals, `p1 = base + off` before the test and
 *      `p2 = base + off` in the pre-header.  gcc CSEs them into one add.
 *      5 of 25 -- the best, and what is kept below.
 *   2. Unnamed `*(base + off)` at the entry test.  gcc picks the shorter
 *      register-offset load `ldrb r3, [r2, r3]` instead of add-then-load, and
 *      reloads the pool a second time.  6 of 25.
 *   3. Destructive `p2 = base; p2 += off;` -- identical to (1), 5 of 25.  The
 *      pointer-walk lever does not separate them here.
 *
 * NOTE the basic-block lever does not apply: there IS a branch between the two
 * uses (the early return), and gcc merges them anyway.  Same finding as
 * src/non_matching/rom_8a000/8096ab0.c -- the lever acts on local-alloc, and
 * this is global CSE, which runs earlier and does not respect blocks.
 *
 * Everything else matches: `k = 0x8a << 1` reproduces the ROM's
 * `mov r3, #0x8a / lsl r3, #1`, and the do/while with the post-increment walk
 * `*q++` gives the exact `ldmia r5!, {r0}` the ROM uses.
 */
extern unsigned char *iwram_3001f2c;
extern void _Sprite_SetAnim(int handle, int anim);

void Func_80ad69c(void)
{
    unsigned char *base;
    unsigned char *p1;
    unsigned char *p2;
    unsigned int off;
    unsigned int k;
    int *q;
    int i;

    base = iwram_3001f2c;
    off = 0x219;
    p1 = base + off;
    i = 0;
    if (i >= *p1)
        return;
    k = 0x8a << 1;
    p2 = base + off;
    q = (int *)(base + k);
    do {
        _Sprite_SetAnim(*q++, 1);
        i++;
    } while (i < *p2);
}
