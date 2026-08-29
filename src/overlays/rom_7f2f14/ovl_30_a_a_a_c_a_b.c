/* Cluster OvlFunc_968_2008058..OvlFunc_968_2008058 extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_a_a_a_c_a.s.
 *
 * Slotted between ovl_30_a_a_a_c_a_a.o and the rest of the overlay.
 *
 * THE ARGUMENTS ARE SHUFFLED ON THE WAY IN: the ROM saves r0-r2, then calls
 * __CreateActor with (r3, r0, r1, r2). So the fourth parameter becomes the
 * first argument and the rest slide down. Read off the four `mov`s before the
 * `bl`, not guessable.
 *
 * The mask uses the SAME form as OvlFunc_968_2009a50 in batch 56, two functions
 * over in this overlay: `m = 0xd; m = -m; m &= f;` -- the full 32-bit ~0xc via
 * mov/neg with the mask as the AND's destination. Note this is the form that
 * FAILS in src/non_matching/overlays/sprite_flags_setter.c; see the caveat
 * recorded there.
 *
 * The `return 0` block sits AFTER the main body in the ROM, so the null check
 * is a `goto` to a label at the end rather than an early return -- same reading
 * as GetSpriteVoice in batch 56, and the block ORDER is what the branch
 * polarity reports.
 */
extern void *__CreateActor(int a, int b, int c, int d);
extern void OvlFunc_968_2008030(void *a, int n);
extern void __Func_800c548(void *a, int n);

void *OvlFunc_968_2008058(int a0, int a1, int a2, int a3)
{
    unsigned char *a;
    unsigned char *s;
    int m;
    int f;

    a = (unsigned char *)__CreateActor(a3, a0, a1, a2);
    if (a == 0)
        goto zero;
    s = *(unsigned char **)(a + 0x50);
    f = s[9];
    m = 0xd;
    m = -m;
    m &= f;
    s[9] = m;
    OvlFunc_968_2008030(a, 0xe);
    __Func_800c548(a, 1);
    return a;
zero:
    return 0;
}
