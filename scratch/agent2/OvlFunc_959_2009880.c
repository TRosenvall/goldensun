/* OvlFunc_959_2009880  --  asm/overlays/rom_7e7574/ovl_9dc_c_a_c_a_c_c_c_c.s
 *
 * Sibling of OvlFunc_959_200981c (already elevated, same file): is the actor
 * within two tiles vertically and six horizontally of the player?  The axes
 * are swapped relative to 981c and the window is +-2 rather than +-1.
 *
 * THE RANGE TEST MUST BE ITS OWN EARLY RETURN.  981c's spelling --
 * `if (all four conditions) return 1; return 0;` -- gives 16 differing of 52
 * here: the ROM pre-loads `mov r0, #0` BEFORE the `cmp r3, #0xc` so the range
 * failure branches straight to the epilogue, while the two window failures go
 * through a separate `mov r0, #0` block.  That is two distinct zero sites, and
 * only splitting the range test into `if (out of range) return 0;` produces
 * them.  Same family, opposite answer to its twin -- read each one off the ROM.
 * No --cflags.
 */
struct Actor { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; };
extern struct Actor *__MapActor_GetActor(int id);

int OvlFunc_959_2009880(int id)
{
    struct Actor *m;
    struct Actor *p;
    int mz, mx, pz, px;

    m = __MapActor_GetActor(id);
    p = __MapActor_GetActor(0);
    mz = m->f10 / 0x100000;
    mx = m->f8 / 0x100000;
    pz = p->f10 / 0x100000;
    px = p->f8 / 0x100000;
    if (mx - px < -6 || mx - px > 6)
        return 0;
    if (mz - 2 < pz && mz + 2 > pz)
        return 1;
    return 0;
}
