struct Spr { unsigned char pad00[9]; unsigned char f9; };

struct Actor {
    unsigned char pad00[0x14];
    int f14;
    unsigned char pad18[0x23 - 0x18];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Spr *f50;
    unsigned char f54;
    unsigned char f55;
};

struct C2 { unsigned char pad00[0x14]; int f14; };
struct Ctx { unsigned char pad00[0x10]; struct C2 *f10; };

extern struct Ctx *iwram_3001f30;
extern struct Actor *_CreateActor(int a, int b, int c, int d);
extern void _DeleteActor(struct Actor *a);
extern void _Actor_SetSpriteFlags(struct Actor *a, int flags);
extern void _Actor_SetAnim(struct Actor *a, int anim);

struct Actor *CreateParticleActor(int a, int b, int c, int d)
{
    struct Ctx *ctx;
    struct Actor *act;

    ctx = iwram_3001f30;
    act = _CreateActor(a, b, c, d);
    if (act != 0) {
        if (act->f54 == 0) {
            _DeleteActor(act);
            return 0;
        }
        act->f14 = ctx->f10->f14;
        act->f55 = 4;
        act->f23 = 4;
        act->f50->f9 &= ~0xc;
        _Actor_SetSpriteFlags(act, 0);
        _Actor_SetAnim(act, 1);
    }
    return act;
}
