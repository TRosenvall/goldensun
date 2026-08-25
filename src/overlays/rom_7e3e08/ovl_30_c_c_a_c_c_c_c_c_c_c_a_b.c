/* Cluster OvlFunc_957_2008d58..OvlFunc_957_2008d58 extracted from goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_a.s.
 *
 * Slotted between ovl_30_c_c_a_c_c_c_c_c_c_c_a_a.o and the rest of the overlay.
 *
 * Builds slot 0xb's position on the stack and hands it to a collision test,
 * setting a byte in the iwram_3001f30 block if it reports a hit.
 *
 * TWO THINGS ARE READ OFF THE REGISTER USE RATHER THAN GUESSED:
 *
 *   __TestCollision takes the ACTOR as well as the triple. The ROM never
 *   rewrites r0 before the `bl`, so the pointer from __MapActor_GetActor is
 *   still there as the first argument with the array in r1 -- the same reading
 *   as the rom_7ced6c members of the position-triple family in batch 46.
 *
 *   iwram_3001f30 is loaded BEFORE the GetActor call and kept in r5, a
 *   callee-saved register the prologue pushes. That is the batch-49 tell: gcc
 *   does not spend a push unless the value has to survive the call, so the load
 *   belongs above it in the source.
 */
struct Actor3 {
    unsigned char pad_00[8];
    int x, y, z;
};

extern unsigned char *iwram_3001f30;
extern struct Actor3 *__MapActor_GetActor(int slot);
extern int __TestCollision(struct Actor3 *a, int *pos);

void OvlFunc_957_2008d58(void)
{
    unsigned char *p;
    struct Actor3 *a;
    int pos[3];

    p = iwram_3001f30;
    a = __MapActor_GetActor(0xb);
    pos[0] = a->x;
    pos[1] = a->y;
    pos[2] = a->z;
    if (__TestCollision(a, pos) > 0)
        p[0x35] = 1;
}
