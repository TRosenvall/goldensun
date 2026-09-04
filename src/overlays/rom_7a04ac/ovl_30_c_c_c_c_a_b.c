/* OvlFunc_913_200a88c -- 0x0200a88c
 *
 * A trigger volume test. Depending on a scene flag, check the actor's position
 * against one of two sets of bounding boxes; on any hit, play the cue, hand the
 * actor its script, and latch a flag. Always returns 0.
 *
 * THE X TESTS ARE THE UNSIGNED RANGE IDIOM, the Y TESTS ARE NOT, and both come
 * out of ordinary two-sided C. Writing `x > LO && x < HI` lets gcc fold the
 * pair into one `add` of a negative constant plus one unsigned `cmp`/`bhi` --
 * the ROM's `ldr r1, =0xff3fffff / add r3, r2, r1 / ldr r1, =0x51fffe / cmp /
 * bhi`. The y bounds stay as two signed compares because their upper test is
 * shared between two boxes and gcc cross-jumps it instead of folding it.
 * docs/elevation.md records that both forms appear in one function and the
 * listing decides; here the source spelling is the SAME for both and gcc picks.
 *
 * READ THE RANGE BOUNDS ARITHMETICALLY, NOT BY EYE. The folded form encodes
 * `lo` as a negated constant and `hi - lo` as the span, so the upper bound is
 * `(2**32 - neg) + span + 1`. Three of the five boxes here were first written
 * with an upper bound guessed from the digits and every one was wrong -- the
 * spans carry into the next hex digit. The tell is immediate: the emitted span
 * constant disagrees with the ROM's, so `ldr r1, =0xb91fffe` against
 * `=0x51fffe` says the bound is wrong, not the idiom.
 *
 * THE HIT BLOCK MUST FALL THROUGH FROM THE LAST TEST. With a `return 0` at the
 * end of each branch, gcc lays the success block after the exit and every box
 * needs an extra `b` to reach it -- 91 lines against 86. Routing all the misses
 * to ONE `out:` label at the very end puts the hit block where the ROM has it,
 * immediately after the shared y test, and the fall-through costs nothing.
 * Same single-exit discipline as the other layout entries on file.
 *
 * The upper y bounds are written `<= 0x248ffff` rather than `< 0x2490000`
 * because the ROM compares against the inclusive constant; the two are
 * equivalent in C and NOT equivalent in the emitted `cmp`.
 */
extern int L3394 __asm__(".L3394");
extern int L3390 __asm__(".L3390");
extern void __PlaySound(int id);
extern void __Actor_SetScript(void *a, void *s);
extern unsigned char gScript_913__0200b2e4[];

int OvlFunc_913_200a88c(unsigned char *a)
{
    int x;

    if (L3394 != 0) {
        x = *(int *)(a + 8);
        if (x > 0xc00000 && x < 0x1120000
            && *(int *)(a + 0x10) > 0x2360000
            && *(int *)(a + 0x10) < 0x2640000)
            goto hit;
        if (x > 0xca0000 && x < 0xff0000
            && *(int *)(a + 0x10) > 0x2250000
            && *(int *)(a + 0x10) < 0x2780000)
            goto hit;
        goto out;
    }
    x = *(int *)(a + 8);
    if (x > 0xc00000 && x < 0xf40000
        && *(int *)(a + 0x10) > 0x2250000
        && *(int *)(a + 0x10) <= 0x248ffff)
        goto hit;
    if (x > 0xf40000 && x < 0x1120000
        && *(int *)(a + 0x10) > 0x23b0000
        && *(int *)(a + 0x10) <= 0x25cffff)
        goto hit;
    if (x > 0xd30000 && x < 0xff0000
        && *(int *)(a + 0x10) > 0x2540000
        && *(int *)(a + 0x10) < 0x2780000)
        goto hit;
    goto out;
hit:
    __PlaySound(0x6a);
    __Actor_SetScript(a, gScript_913__0200b2e4);
    L3390 = 1;
out:
    return 0;
}
