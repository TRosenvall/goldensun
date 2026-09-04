/* OvlFunc_911_200a6cc -- 0x0200a6cc
 *
 * A trigger volume test, and a near-twin of
 * src/overlays/rom_7a04ac/ovl_30_c_c_c_c_a_b.c from batch 217: a scene flag
 * selects one of two chains of bounding boxes, and any hit plays the cue, hands
 * the actor its script and latches a flag. Same idioms, one new finding.
 *
 * LET gcc CROSS-JUMP A SHARED TAIL -- DO NOT WRITE THE SHARE YOURSELF. Two of
 * the five boxes end in the same y test against the same upper bound, and the
 * ROM reaches it from both with a shared block. Writing that share explicitly,
 * as a `goto` into a common `step:` label, gets everything right EXCEPT one
 * register: the bound's pool load lands in the register the limit vacated
 * (`ldr r1, =0x114ffff` against the ROM's `ldr r2, ...`). Writing both arms out
 * in full and letting jump.c merge the identical tails is exact.
 *
 * Six configurations were measured against that one line and the explicit-share
 * form never beat 2 differing: limit pinned to r1 (2), limit unpinned (6),
 * bound named and pinned to r2 (3), bound named unpinned (2), bound assigned
 * before the y load (2), and a barrier keeping the limit live past the compare
 * (18, and two lines LONGER). The lesson is not which pin to use -- it is that
 * AN EXPLICIT `goto` SHARE IS A CONSTRAINT ON THE ALLOCATOR that cross-jumping
 * is not. gcc merges the tails either way; only one way lets it choose the
 * registers freely first.
 *
 * The rest is batch 217's twin, unchanged: `x > LO && x < HI` folds into the
 * unsigned range idiom by itself, the bounds are computed arithmetically
 * (lo = 2**32 - neg, hi = lo + span) rather than read off the hex digits, each
 * y limit is written `lim = K; y = ...; lim <<= 16;` so the load lands between
 * the constant's mov and its shift, and every miss routes to ONE `out:` label
 * so the success block falls through where the ROM has it.
 */
extern int L369c __asm__(".L369c");
extern int L3698 __asm__(".L3698");
extern void __PlaySound(int id);
extern void __Actor_SetScript(void *a, void *s);
extern unsigned char gScript_911__0200b5ec[];

int OvlFunc_911_200a6cc(unsigned char *a)
{
    int x;
    int y;
    register int lim __asm__("r1");

    if (L369c != 0) {
        x = *(int *)(a + 8);
        if (x > 0x3b0000 && x < 0x8d0000) {
            lim = 0xd3;
            y = *(int *)(a + 0x10);
            lim <<= 16;
            if (y > lim && y <= 0x100ffff)
                goto hit;
        }
        if (x > 0x450000 && x < 0x7a0000) {
            lim = 0xc2;
            y = *(int *)(a + 0x10);
            lim <<= 16;
            if (y > lim && y <= 0x114ffff)
                goto hit;
        }
        goto out;
    }
    x = *(int *)(a + 8);
    if (x > 0x3b0000 && x < 0x6f0000) {
        lim = 0xc2;
        y = *(int *)(a + 0x10);
        lim <<= 16;
        if (y > lim && y < 0xe60000)
            goto hit;
    }
    if (x > 0x6f0000 && x < 0x8d0000) {
        lim = 0xd8;
        y = *(int *)(a + 0x10);
        lim <<= 16;
        if (y > lim && y < 0xfa0000)
            goto hit;
    }
    if (x > 0x4e0000 && x < 0x7a0000) {
        lim = 0xf1;
        y = *(int *)(a + 0x10);
        lim <<= 16;
        if (y > lim && y <= 0x114ffff)
            goto hit;
    }
    goto out;
hit:
    __PlaySound(0x6a);
    __Actor_SetScript(a, gScript_911__0200b5ec);
    L3698 = 1;
out:
    return 0;
}
