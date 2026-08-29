/* Cluster OvlFunc_968_2009a50..OvlFunc_968_2009a50 extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_a_c.s.
 *
 * Slotted between ovl_30_c_a_c_c_c_a_c_a.o and the rest of the overlay.
 *
 * THREE LEVERS AT ONCE, and it is worth listing which, because at 36
 * instructions this is the largest function elevated in several batches and it
 * took no new ideas -- only the accumulated ones applied together.
 *
 *   THE FALLBACK CALL GOES LAST. The ROM's `bne` jumps FORWARD to
 *   OvlFunc_968_20099c0 at the end of the function; writing it as an early
 *   return puts it first and inverts the branch. `goto other;` with the call
 *   after the main body is what the ROM has.
 *
 *   THE MASK IS THE AND'S DESTINATION. `m = 0xd; m = -m; m &= f;` gives the
 *   ROM's `mov r3,#0xd / neg r3,r3 / and r3, r2` -- the full 32-bit ~0xc built
 *   with mov/neg, not the narrowed `mov r3,#0xf3` gcc emits for a plain
 *   `f & ~0xc` feeding a byte store. See
 *   src/non_matching/overlays/sprite_flags_setter.c for that class; NOTE that
 *   the compound-assignment form which fails there is what works here, so the
 *   rule in that file is narrower than it reads.
 *
 *   ONE ZERO, THREE STORES. `z = 0;` written once and stored to +0x44, +0x08
 *   and +0x10 reproduces the ROM keeping it in r5 across two calls -- the
 *   callee-saved tell from batch 49.
 */
extern void OvlFunc_968_20099c0(void);
extern void *OvlFunc_968_2008058(int a, int b, int c, int d);
extern void OvlFunc_968_200894c(void *a);
extern void __DeleteActor(void *e);

void OvlFunc_968_2009a50(void *actor)
{
    unsigned char *a;
    unsigned char *s;
    int f;
    int m;
    int z;
    void *e;

    a = (unsigned char *)actor;
    s = *(unsigned char **)(a + 0x50);
    f = s[9];
    if ((f & 0xc) != 0xc)
        goto other;
    m = 0xd;
    m = -m;
    m &= f;
    s[9] = m | 4;
    z = 0;
    *(int *)(a + 0x44) = z;
    e = OvlFunc_968_2008058(*(int *)(a + 8), 0, 0x80 << 18, 0xdf);
    OvlFunc_968_200894c(a);
    *(int *)(a + 8) = z;
    *(int *)(a + 0x10) = z;
    __DeleteActor(e);
    return;
other:
    OvlFunc_968_20099c0();
}
