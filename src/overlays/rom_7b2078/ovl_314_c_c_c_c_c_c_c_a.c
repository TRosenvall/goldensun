/* OvlFunc_926_200c140  --  0x0200c140
 *
 * Cut out of goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_c_a.s.
 *
 * Eight bursts of the same particle emitter, with a sound on every other one.
 *
 * The emitter descriptor is a 0x28-byte struct built on the stack and passed by
 * address as the eighth argument -- the same idiom as
 * src/overlays/rom_7ac2d8/ovl_314_c_b.c with different field offsets. Matched
 * on the first screen.
 *
 * Drafted by a parallel screening agent and re-screened here before wiring.
 */
struct Emit {
    int f0;
    unsigned char pad04[0x10 - 0x4];
    int f10;
    int f14;
    short f18;
    unsigned char pad1a[2];
    unsigned char *f1c;
    unsigned char pad20[8];
};

struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
};

extern unsigned char L51d8[] __asm__(".L51d8");

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void OvlFunc_common0_10c(int a, int b, int c, int d,
                                int e, int f, int g, struct Emit *h);

void OvlFunc_926_200c140(void)
{
    struct Emit s;
    struct Actor *a;
    unsigned int i;

    a = __MapActor_GetActor(8);
    s.f0 = 1;
    s.f18 = 0x119;
    s.f1c = L51d8;
    s.f10 = 0xe0 << 10;
    s.f14 = 0xc0 << 9;
    i = 0;
    do {
        __CutsceneWait(0xa);
        if ((i & 1) != 0)
            __PlaySound(0x82);
        OvlFunc_common0_10c(a->x, a->y, a->z - 0x180000, 0, 0x9999, 0,
                            0x360001, &s);
        i++;
    } while (i <= 7);
    __CutsceneWait(0x3c);
}
