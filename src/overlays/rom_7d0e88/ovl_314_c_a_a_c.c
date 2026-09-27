/* OvlFunc_947_2009074  --  0x02009074, was
 * asm/overlays/rom_7d0e88/ovl_314_c_a_a_c.s (this function alone), so it
 * converts whole. Matched from scratch.
 *
 * - `do { } while (0);` before `ret = 1;` stops sched2 hoisting `mov r2,#1`
 *   above the sub/mov r9/add (4 off without; ret++, an unsigned char ret, and
 *   reorderings all inert).
 * - `h2 = t; h2 /= 0x100000;` keeps the ROM's `mov r3,r0` copy before the
 *   signed divide (71 -> 4); `i += 2;` / `i += 3;` stop gcc folding
 *   (i+2)<<20 into the 0x20000 constant.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern int __Func_8011f54(int layer, int x, int z);
extern void *OvlFunc_common0_18(int x, int y, int z, int k);
extern void OvlFunc_947_2008da8(struct Actor *a, int h);
extern void __DeleteActor(void *p);

int OvlFunc_947_2009074(int slot, int dir)
{
    struct Actor *a;
    void *obj;
    int ret;
    int h;
    int h0;
    int h2;
    unsigned int n;
    unsigned int i;
    int x, y, z, k;
    int t;

    a = __MapActor_GetActor(slot);
    ret = 0;
    obj = 0;
    __Actor_SetSpriteFlags(a, 0);
    h = __Func_8011f54(2, a->f8, a->f10);
    h0 = h / 0x100000;
    n = h0;
    if (h0 < 0)
        n = -h0;
    n++;
    for (i = 0; i <= n; i++) {
        t = __Func_8011f54(a->f22, a->f8, (i << 20) + a->f10);
        h2 = t;
        h2 /= 0x100000;
        if (h0 < h2) {
            x = ((a->f8 >> 20) << 20) + 0x80000;
            if (dir == 0) {
                i += 2;
                z = ((i + (a->f10 >> 20)) << 20) + 0x20000;
                y = h2 << 20;
                k = 0xdf;
            } else {
                i += 3;
                z = ((i + (a->f10 >> 20)) << 20) - 0x20000;
                y = h2 << 20;
                k = 0xfd;
            }
            obj = OvlFunc_common0_18(x, y, z, k);
            h = a->f10 - z + y;
            do { } while (0);
            ret = 1;
            break;
        }
    }
    OvlFunc_947_2008da8(a, h);
    a->f8 = 0;
    a->fc = 0;
    a->f10 = 0;
    if (obj != 0)
        __DeleteActor(obj);
    return ret;
}
