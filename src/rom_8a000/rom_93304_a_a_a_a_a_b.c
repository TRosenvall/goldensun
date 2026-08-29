/* SetCameraTarget  --  0x0809335c
 *
 * Cut out of goldensun/asm/rom_8a000/rom_93304_a_a_a_a_a.s.
 *
 * Points the camera at a field actor. When the second argument is zero the
 * camera is also snapped to the actor's position immediately and the screen is
 * refreshed, unless the game is in mode 3.
 *
 * THE INDIRECT GLOBAL IS READ BEFORE THE NULL TEST. The ROM has
 * `ldr r3, =iwram_3001e70 / ldr r3, [r3]` ahead of `cmp r6, #0`, so the source
 * reads it into a local before the `if`; written inside the block it lands
 * after the test and costs five positions. That is the same shape as the
 * gState-base rule -- a global that is used once still wants a local when the
 * ROM hoists its load.
 *
 * The camera's x is written through the same pointer that was just handed to
 * the global (`str r7, [r3]` then `str r3, [r7]`), so it is a named `int *`
 * rather than a second `c->f8` reference -- that is what keeps r7 alive across
 * _Camera_SetTarget instead of recomputing the address.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
};

struct C {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
};

extern void **iwram_3001e70;
extern struct A *GetFieldActor(int id);
extern unsigned char *galloc_ewram(int slot, int size);
extern void _Camera_SetTarget(struct C *c, struct A *a);
extern void WaitFrames(int n);
extern void _Func_800fe9c(void);

void SetCameraTarget(int id, int flag)
{
    struct A *a;
    struct C *c;
    unsigned char *m;
    int *t;
    void **e;

    a = GetFieldActor(id);
    m = galloc_ewram(0x1b, 0xccc);
    c = *(struct C **)(m + (0xf0 << 1));
    e = iwram_3001e70;
    if (a != 0) {
        t = &c->f8;
        *e = t;
        _Camera_SetTarget(c, a);
        if (flag == 0) {
            *t = a->f8;
            c->fc = a->fc;
            c->f10 = a->f10;
            WaitFrames(1);
            if (*(short *)(m + (0xcf << 1)) != 3)
                _Func_800fe9c();
        }
    }
}
