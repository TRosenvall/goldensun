/* UpdateScreenShake  --  0x080e155c, split out of asm/rom_c9000/rom_e0564.s (its neighbours,
 * the Anim_* routines, and the trailing .rodata stay in _a.s/_c.s; nine data
 * labels were exported for the split). Matched from scratch.
 *
 * The statements are FUSED: `x = (Random() & (w - 1)) - h / 2;` and the same
 * for y. Splitting the `- h/2` into later statements puts w/h/timer in the
 * wrong registers (32 differing). The ROM really does use h/2 for both axes.
 */
struct Bat { char pad[0x77a0]; int x; int y; int timer; };
extern struct Bat *iwram_3001eec;
extern short iwram_3001ad0[];
extern int gPhysVec[];
extern int Random(void);
void UpdateScreenShake(int w, int h)
{
    struct Bat *base = iwram_3001eec;
    if (base->timer > 0) {
        int x, y;
        x = (Random() & (w - 1)) - h / 2;
        y = (Random() & (h - 1)) - h / 2;
        iwram_3001ad0[2] = x;
        iwram_3001ad0[3] = y + 0x20;
        gPhysVec[3] = 0x78 - x;
        gPhysVec[4] = 0x78 - y;
        base->timer--;
    } else {
        iwram_3001ad0[2] = base->x;
        iwram_3001ad0[3] = base->y;
        gPhysVec[3] = 0x78;
        gPhysVec[4] = 0x78;
    }
}
