/* OvlFunc_888_200a67c  --  asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a.s
 *
 * Copy the player's position into this actor, drop it 0x20000 in z, then nudge
 * one axis according to the low two bits of iwram_3001e40.
 *
 * A four-case decision tree.  Case bodies in SOURCE order 0,1,2,3 -- which is
 * also the ROM's block order -- and gcc cross-joins cases 0 and 1 at the
 * shared `add / str +8 / str +0x38` tail, which is the ROM's .L26c2.  The two
 * negative addends are written as unsigned literals so they land in the pool
 * the way the ROM has them.
 * No --cflags.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x38 - 0x14];
    int f38;
    int f3c;
    int f40;
};

extern unsigned int iwram_3001e40;
extern struct Actor *__MapActor_GetActor(int slot);

void OvlFunc_888_200a67c(struct Actor *d)
{
    struct Actor *p;

    p = __MapActor_GetActor(8);
    d->f8 = p->f8;
    d->f38 = d->f8;
    d->fc = p->fc;
    d->f3c = d->fc;
    d->f10 = p->f10 + 0xfffe0000;
    d->f40 = d->f10;
    switch (iwram_3001e40 & 3) {
    case 0:
        d->f8 = p->f8 + 0xfffc8000;
        d->f38 = d->f8;
        break;
    case 1:
        d->f8 = p->f8 + 0x30000;
        d->f38 = d->f8;
        break;
    case 2:
        d->fc = p->fc + 0x20000;
        d->f3c = d->fc;
        break;
    case 3:
        d->f10 = p->f10;
        d->f40 = d->f10;
        break;
    }
}
